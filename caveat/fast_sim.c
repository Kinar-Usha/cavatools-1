/*
  Copyright (c) 2021 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/
//#define DEBUG
#include <stdlib.h>
#define abort() { fprintf(stderr, "Aborting in %s line %d\n", __FILE__, __LINE__); exit(-1); }

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "caveat.h"
#include "caveat_fp.h"
#include "arith.h"
#include "opcodes.h"
#include "insn.h"
#include "shmfifo.h"
#include "core.h"


#define trace_mem(code, a)  0
#define trace_bbk(code, v)
#define advance(sz)
#define restart()
#define update_regfile(rd, val)



pthread_mutex_t amolock;

static void amoBegin(struct core_t* cpu, const char* name, int r1, int r2)
{
  pid_t tid = syscall(SYS_gettid);
  fprintf(stderr, "%s%s(%s=%lx, %s=%lx)", color[tid%8], name, regName[r1], IR(r1).l, regName[r2], IR(r2).l);
  pthread_mutex_lock(&amolock);
}

static void amoEnd(struct core_t* cpu, int rd)
{
  pthread_mutex_unlock(&amolock);
  fprintf(stderr, "->%s=%lx\e[39m\n", regName[rd], IR(rd).l);
}

//#define amoB(cpu, name, r1, r2) amoBegin(cpu, name, r1, r2)
//#define amoE(cpu, rd)           amoEnd(cpu, rd)
#define amoB(cpu, name, r1, r2)
#define amoE(cpu, rd)


#define MEM_ACTION(a)  0
#define JUMP_ACTION()
#define STATS_ACTION()

#include "imacros.h"


/* Special instructions, may exit simulation */

int ecall_wrapper(struct core_t* cpu)
{
  int rc = proxy_ecall(cpu);
  //  pid_t tid = syscall(SYS_gettid);
  //  fprintf(stderr, "ecall_wrapper, pid=%d, tid=%d\n", getpid(), tid);
  return rc;
}




void fast_sim(struct core_t* cpu)
{ 
  pid_t tid = syscall(SYS_gettid);
  Addr_t PC = cpu->pc;
  long icount = simparam.report;
  while (1) {			/* exit by special opcodes above */
    while (icount > 0) {
      //#ifdef DEBUG
      if (simparam.visible) {
	fprintf(stderr, "%s %x", color[tid%8], tid);
	fprintf(stderr, "F ");
	print_pc(PC, stderr);
	print_insn(PC, stderr);
	fprintf(stderr, "\e[39m");
      }
      //#endif
      const struct insn_t* p = insn(PC);
      switch (p->op_code) {
#include "execute_insn.h"
      case Op_zero:
	fprintf(stderr, "ZERO opcode at %lx\n", PC);
	abort();		/* should never occur */
      case Op_illegal:
	cpu->state.mcause = 2;	/* Illegal instruction */
	goto stop_run;
      default:
	cpu->state.mcause = 10; /* Unknown instruction */
	goto stop_run;
      }
      IR(0).l = 0L;
      icount--;
    }
    STATS(cpu);
    status_report(cpu, stderr);
    icount = simparam.report;
  }

 stop_run:
  status_report(cpu, stderr);
}





void single_step(struct core_t* cpu)
{
  long icount = 1;
#define PC cpu->pc
  register const struct insn_t* p = insn(PC);
  switch (p->op_code) {
#include "execute_insn.h"
  case Op_zero:
    abort();		/* should never occur */
  case Op_illegal:
    cpu->state.mcause = 2;	/* Illegal instruction */
    break;
  default:
    cpu->state.mcause = 10; /* Unknown instruction */
    break;
  }
 stop_run:
  IR(0).l = 0L;
  STATS(cpu);
}
