/*
  Copyright (c) 2021 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/
#include <stdlib.h>
#define abort() { fprintf(stderr, "Aborting in %s line %d\n", __FILE__, __LINE__); exit(-1); }

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "caveat_fp.h"
#include "arith.h"

#include "caveat.h"
#include "opcodes.h"
#include "insn.h"
#include "shmfifo.h"
#include "core.h"
#include "perfctr.h"
#include "riscv-opc.h"
#include "ecall_nums.h"




static Addr_t emulate_brk(Addr_t addr, struct pinfo_t* info)
{
  Addr_t newbrk = addr;
  if (addr < info->brk_min)
    newbrk = info->brk_min;
  else if (addr > info->brk_max)
    newbrk = info->brk_max;

  if (info->brk == 0)
    info->brk = ROUNDUP(info->brk_min, RISCV_PGSIZE);

  uintptr_t newbrk_page = ROUNDUP(newbrk, RISCV_PGSIZE);
  if (info->brk > newbrk_page)
    munmap((void*)newbrk_page, info->brk - newbrk_page);
  else if (info->brk < newbrk_page)
    assert(mmap((void*)info->brk, newbrk_page - info->brk, -1, MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS, 0, 0) == (void*)info->brk);
  info->brk = newbrk_page;

  return newbrk;
}

static pthread_mutex_t ecall_mutex;

int proxy_ecall( struct core_t* cpu )
{
  cpu->counter.ecalls++;
  long rvnum = cpu->reg[17].l;
  if (rvnum < 0 || rvnum >= rv_syscall_entries) {
  no_mapping:
    fprintf(stderr, "RISC-V system call %ld has no mapping to host system\n", rvnum);
    fprintf(stderr, "Arguments(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n",
            cpu->reg[10].l, cpu->reg[11].l, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[14].l, cpu->reg[15].l);
    abort();
  }
  long sysnum = rv_to_host[rvnum].sysnum;
  if (simparam.ecalls) {
    fprintf(stderr, "ecall %s->%ld(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)", rv_to_host[rvnum].name, sysnum,
            cpu->reg[10].l, cpu->reg[11].l, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[14].l, cpu->reg[15].l);
  }
  switch (sysnum) {
  case -1:
    goto no_mapping;
  case -2:
    fprintf(stderr, "RISCV-V system call %s(#%ld) not supported on host system\n", rv_to_host[rvnum].name, sysnum);
    abort();
    
#if 0
  case __NR_brk:
    cpu->reg[10].l = emulate_brk(cpu->reg[10].l, &current);
    break;
#endif

  case __NR_exit:
  case __NR_exit_group:
    return 1;

  case __NR_rt_sigaction:
    fprintf(stderr, "Trying to call rt_sigaction, always succeed without error.\n");
    cpu->reg[10].l = 0;  // always succeed without error
    break;

  case __NR_clone: /* sys_clone */
    parent_func(cpu);
    break;

#if 0
  case __NR_futex:
    {
      //    cpu->reg[10].l = syscall(sysnum, cpu->reg[10].l, FUTEX_WAKE_PRIVATE, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[14].l, cpu->reg[15].l);
      int rc = syscall(sysnum, cpu->reg[10].l, cpu->reg[11].l, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[14].l, cpu->reg[15].l);
      if (rc != 0) {
	fprintf(stderr, "futex(%lx, %lx, %ld, %lx) returns error %ld\n", cpu->reg[10].l, cpu->reg[11].l, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[10].l);
	perror("proxy_ecall:");
      }
      cpu->reg[10].l = rc;
    }
    break;
#endif

  case __NR_times:
    {
      long count = (perf.h && simparam.mhz) ? cpu->counter.cycles_simulated : cpu->counter.insn_executed;
      long denominator = simparam.mhz ? simparam.mhz*1000000 : 1000000000;
      count = (double)count * sysconf(_SC_CLK_TCK) / denominator;
      struct tms tms;
      memset(&tms, 0, sizeof tms);
      tms.tms_utime = count;
      //      fprintf(stderr, "times(tms_utime=%ld)\n", tms.tms_utime);
      memcpy(cpu->reg[10].p, &tms, sizeof tms);
      cpu->reg[10].l = 0;
    }
    break;

  case __NR_gettimeofday:
    { 
      long count = (perf.h && simparam.mhz) ? cpu->counter.cycles_simulated : cpu->counter.insn_executed;
      long denominator = simparam.mhz ? simparam.mhz*1000000 : 1000000000;
      struct timeval tv;
      tv.tv_sec  = count / denominator;
      tv.tv_usec = count % denominator;
      tv.tv_sec  += cpu->counter.start_timeval.tv_sec;
      tv.tv_usec += cpu->counter.start_timeval.tv_usec;
      tv.tv_sec  += tv.tv_usec / 1000000;  // microseconds overflow
      tv.tv_usec %=              1000000;
      //      fprintf(stderr, "gettimeofday(sec=%ld, usec=%4ld)\n", tv.tv_sec, tv.tv_usec);
      memcpy(cpu->reg[10].p, &tv, sizeof tv);
      cpu->reg[10].l = 0;
    }
    break;

  case __NR_close:
    if (cpu->reg[10].l <= 2) { // Don't close stdin, stdout, stderr
      cpu->reg[10].l = 0;
      break;
    }
    goto default_case;

  default:
  default_case:
    //    pthread_mutex_lock(&ecall_mutex);
    cpu->reg[10].l = syscall(sysnum, cpu->reg[10].l, cpu->reg[11].l, cpu->reg[12].l, cpu->reg[13].l, cpu->reg[14].l, cpu->reg[15].l);
    //    pthread_mutex_unlock(&ecall_mutex);
    break;
  }
  if (simparam.ecalls) {
    pid_t tid = syscall(SYS_gettid);
    fprintf(stderr, " return %lx pid=%d, tid=%d\n", cpu->reg[10].l, getpid(), tid);
  }
  return 0;
}


static void set_csr( struct core_t* cpu, int which, long val )
{
  switch (which) {
  case CSR_USTATUS:
    cpu->state.ustatus = val;
    return;
  case CSR_FFLAGS:
    cpu->state.fcsr.flags = val;
#ifdef SOFT_FP
    softfloat_exceptionFlags = val;
#else
#endif
    return;
  case CSR_FRM:
    cpu->state.fcsr.rmode = val;
    break;
  case CSR_FCSR:
    cpu->state.fcsr_v = val;
    break;
  default:
    fprintf(stderr, "Unsupported set_csr(%d, val=%lx)\n", which, val);
    abort();
  }
#ifdef SOFT_FP
  softfloat_roundingMode = cpu->state.fcsr.rmode;
#else
  fesetround(riscv_to_c_rm(cpu->state.fcsr.rmode));
#endif
}

static long get_csr( struct core_t* cpu, int which )
{
  switch (which) {
    case CSR_USTATUS:
    return cpu->state.ustatus;
  case CSR_FFLAGS:
#ifdef SOFT_FP
    cpu->state.fcsr.flags = softfloat_exceptionFlags;
#else
#endif
    return cpu->state.fcsr.flags;
  case CSR_FRM:
    return cpu->state.fcsr.rmode;
  case CSR_FCSR:
    return cpu->state.fcsr_v;
  default:
    fprintf(stderr, "Unsupported get_csr(%d)\n", which);
    abort();
  }
}

void proxy_csr( struct core_t* cpu, const struct insn_t* p, int which )
{
  enum Opcode_t op = p->op_code;
  int regop = op==Op_csrrw || op==Op_csrrs || op==Op_csrrc;
  long old_val = 0;
  long value = regop ? p->op_rs1 : p->op_constant>>12;
  if (op==Op_csrrw || op==Op_csrrwi) {
    if (p->op_rd != 0)
      old_val = get_csr(cpu, which);
    set_csr(cpu, which, value);
  }
  else {
    old_val = get_csr(cpu, which);
    if (regop || value != 0) {
      if (op==Op_csrrs || op==Op_csrrsi)
	value = old_val |  value;
      else
	value = old_val & ~value;
      set_csr(cpu, which, value);
    }
  }
  cpu->reg[p->op_rd].l = old_val;
}
