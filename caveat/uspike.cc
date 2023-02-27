/*
  Copyright (c) 2021 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>

#include <map>

#include "options.h"
#include "caveat.h"

option<long> conf_report("report", 1000, "Status report every N milliseconds");
option<>     conf_gdb("gdb",	0, "localhost:1234", "Remote GDB connection");
option<bool> conf_show("show",	false, true,			"Show instruction trace");
option<bool> conf_calls("calls",	false, true,			"Show function calls and returns");

extern std::map<long, const char*> fname; // dictionary of pc->name

class hart_t : public hart_base_t {
  long _executed;
public:
  hart_t(int argc, const char* argv[], const char* envp[], simfunc_t simfunc)
    :hart_base_t(argc, argv, envp, simfunc) { _executed=0; }
  hart_t(simfunc_t simulator) :hart_base_t(this, simulator) { _executed=0; }
  hart_t* new_hart(hart_t* from) { return new hart_t(simulator); }

  long executed() { return _executed; }
  void more_insn(long n) { _executed+=n; }
  static long total_count();
  static hart_t* list() { return (hart_t*)hart_base_t::list(); }
  hart_t* next() { return (hart_t*)hart_base_t::next(); }
  static hart_t* find(int tid) { return (hart_t*)hart_base_t::find(tid); }
  friend void simulator(hart_base_t* h, Header_t* bb);
  friend void status_report();
};

long hart_t::total_count()
{
  long total = 0;
  for (hart_t* p=hart_t::list(); p; p=p->next())
    total += p->executed();
  return total;
}

void status_report()
{
  double realtime = elapse_time();
  long total = hart_t::total_count();
  fprintf(stderr, "\r\33[2K%12ld insns %3.1fs %3.1f MIPS ", total, realtime, total/1e6/realtime);
  if (hart_t::num_harts() <= 16) {
    char separator = '(';
    for (hart_t* p=hart_t::list(); p; p=p->next()) {
      fprintf(stderr, "%c", separator);
      fprintf(stderr, "%1ld%%", 100*p->executed()/total);
      separator = ',';
    }
    fprintf(stderr, ")");
  }
  else if (hart_t::num_harts() > 1)
    fprintf(stderr, "(%d cores)", hart_t::num_harts());
}

void simulator(hart_base_t* h, Header_t* bb)
{
  hart_t* c = (hart_t*)h;
  c->more_insn(bb->count);
}

void exit_func()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "EXIT_FUNC() called\n\n");
  status_report();
  fprintf(stderr, "\n");
}  

static jmp_buf return_to_top_level;

static void segv_handler(int, siginfo_t*, void*) {
  longjmp(return_to_top_level, 1);
}
  
  
hart_t* mycpu;

#ifdef DEBUG
void signal_handler(int nSIGnum, siginfo_t* si, void* vcontext)
{
  //  ucontext_t* context = (ucontext_t*)vcontext;
  //  context->uc_mcontext.gregs[]
  fprintf(stderr, "\n\nsignal_handler, signum=%d, tid=%d\n", nSIGnum, gettid());
  hart_t* thisCPU = hart_t::find(gettid());
  thisCPU->debug_print();
  //  mycpu->debug_print();
  exit(-1);
  //  longjmp(return_to_top_level, 1);
}
#endif

void* status_thread(void* arg)
{
  while (1) {
    usleep(1000*conf_report);
    status_report();
  }
}

int main(int argc, const char* argv[], const char* envp[])
{
  parse_options(argc, argv, "uspike: user-mode RISC-V interpreter derived from Spike");
  if (argc == 0)
    help_exit();
  mycpu = new hart_t(argc, argv, envp, simulator);
  start_time();

#ifdef DEBUG
  if (!conf_gdb) {
    static struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    sigemptyset(&action.sa_mask);
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    //    action.sa_sigaction = segv_handler;
    action.sa_sigaction = signal_handler;
    sigaction(SIGSEGV, &action, NULL);
    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGINT,  &action, NULL);
    if (setjmp(return_to_top_level) != 0) {
      fprintf(stderr, "SIGSEGV signal was caught\n");
      mycpu->print_debug_trace();
      exit(-1);
    }
  }
#endif

  dieif(atexit(exit_func), "atexit failed");
  if (conf_gdb)
    controlled_by_gdb(conf_gdb, mycpu);
  else if (conf_show) {
    while (1)
      mycpu->single_step(true);
  }
  else if (conf_calls) {
    int indent = 0;
    while (1) {
#if 1
      long oldpc = mycpu->pc();
      Insn_t insn = decoder(oldpc);
      Opcode_t op = insn.opcode();
      
      if (op==Op_c_ret || op==Op_ret) {
	indent--;
      }
#endif 
      mycpu->single_step(true);
#if 1
      if (op==Op_jal || op==Op_c_jalr || op==Op_jalr) {
	indent++;
	long pc = mycpu->pc();
	auto it = fname.find(pc);
	//	fprintf(stderr, "\r%*s%s 0x%lx ", indent*4, "", op_name[op], pc);
	//	fprintf(stderr, " %s from 0x%lx\n", it==fname.end() ? "NOT FOUND" : it->second, oldpc);
	fprintf(stderr, "\r%*s%s\n", indent*4, "", it==fname.end() ? "NOT FOUND" : it->second);
      }
#endif
    }      
  }
  else {
    if (conf_report > 0) {
      pthread_t tnum;
      dieif(pthread_create(&tnum, 0, status_thread, 0), "failed to launch status_report thread");
    }
    mycpu->interpreter();
  }
}
