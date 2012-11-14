// Copyright 2011 The University of Michigan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Jie Yu (jieyu@umich.edu)

// File: core/debug_analyzer.h - The analyzer for debug purpose.

#ifndef CORE_DEBUG_ANALYZER_H_
#define CORE_DEBUG_ANALYZER_H_

// Used to have generalized printing of 64 bit types
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "core/basictypes.h"
#include "core/analyzer.h"
#include "core/logging.h"
#include "core/knob.h"

// Debug analyzer. The main function is to print every event.
class DebugAnalyzer : public Analyzer {
 public:
  DebugAnalyzer() {}
  ~DebugAnalyzer() {}

  void Register();
  bool Enabled();
  void Setup();

  void ProgramStart() {
    INFO_FMT_PRINT_SAFE("Program Start\n");
  }

  void ProgramExit() {
    INFO_FMT_PRINT_SAFE("Program Exit\n");
  }

  // address_t is of type unsigned long so the format string does not need to
  // be a platform indepenent one.
  void ImageLoad(Image *image, address_t low_addr, address_t high_addr,
                 address_t data_start, size_t data_size, address_t bss_start,
                 size_t bss_size) {
      // size_t is platform dependent in size. The macro INFO_FMT_PRINT_SAFE
      // uses snprintf() to create a formatted string. I'm not sure why, since
      // this is a C++ program, C style output is being used, but I bet there's
      // a good reason. C99 added the %z format string for size_t, so hopefully
      // that'll work here to get rid of the warnings while still keeping
      // everything working. This is done in many places in this file
    INFO_FMT_PRINT_SAFE("Image Load, name='%s', low=0x%lx, high=0x%lx, "
                        "data_start=0x%lx, data_size=%zu, "
                        "bss_start=0x%lx, bss_size=%zu\n",
                        image->name().c_str(), low_addr, high_addr,
                        data_start, data_size, bss_start, bss_size);
  }

  void ImageUnload(Image *image, address_t low_addr, address_t high_addr,
                   address_t data_start, size_t data_size, address_t bss_start,
                   size_t bss_size) {
      // Fixed more format strings for size_t
    INFO_FMT_PRINT_SAFE("Image Unload, name='%s', low=0x%lx, high=0x%lx, "
                        "data_start=0x%lx, data_size=%zu, "
                        "bss_start=0x%lx, bss_size=%zu\n",
                        image->name().c_str(), low_addr, high_addr,
                        data_start, data_size, bss_start, bss_size);
  }

  void SyscallEntry(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    int syscall_num) {
      // Again, more errors due to fomat strings. curr_thd_id is of type
      // thread_id_t which is not a long unsigned int. thread_id_t is typedefed
      // in core/basictypes.h as a uint64. On a 64 bit machine this would be a
      // long but on a 32 bit machine this would be a long long. 
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Syscall enter num = %d\n",
                        curr_thd_id, syscall_num);
  }

  void SyscallExit(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                   int syscall_num) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Syscall exit num = %d\n",
                        curr_thd_id, syscall_num);
  }

  void SignalReceived(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                      int signal_num) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Signal received, signo = %d\n",
                        curr_thd_id, signal_num);
  }

  void ThreadStart(thread_id_t curr_thd_id, thread_id_t parent_thd_id) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Thread Start, parent=%" PRIx64 "\n",
                        curr_thd_id, parent_thd_id);
  }

  void ThreadExit(thread_id_t curr_thd_id, timestamp_t curr_thd_clk) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Thread Exit\n", curr_thd_id);
  }

  void Main(thread_id_t curr_thd_id, timestamp_t curr_thd_clk) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Main Func\n", curr_thd_id);
  }

  void ThreadMain(thread_id_t curr_thd_id, timestamp_t curr_thd_clk) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Thread Main Func\n", curr_thd_id);
  }

  void BeforeMemRead(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                     Inst *inst, address_t addr, size_t size) {
    INFO_FMT_PRINT_SAFE(
	    // timestamp_t is of type uint64_t so a platform independent printf
	    // format string should be used. Hopefully this doesn't mess
	    // anything up
        "[T%" PRIx64 "] Before Read, inst='%s', addr=0x%lx, size=%zu, clk=%" PRIx64 
	"\n", curr_thd_id, inst->ToString().c_str(), addr, size, curr_thd_clk);
  }

  void AfterMemRead(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, address_t addr, size_t size) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Read, inst='%s', addr=0x%lx, size=%zu\n",
        curr_thd_id, inst->ToString().c_str(), addr, size);
  }

  void BeforeMemWrite(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                      Inst *inst, address_t addr, size_t size) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Write, inst='%s', addr=0x%lx, size=%zu, clk=%" PRIx64 
	"\n", curr_thd_id, inst->ToString().c_str(), addr, size, curr_thd_clk);
  }

  void AfterMemWrite(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                     Inst *inst, address_t addr, size_t size) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Write, inst='%s', addr=0x%lx, size=%zu\n",
        curr_thd_id, inst->ToString().c_str(), addr, size);
  }

  void BeforeAtomicInst(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                        Inst *inst, std::string type, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Atomic Inst, inst='%s', type='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), type.c_str(), addr);
  }

  void AfterAtomicInst(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                       Inst *inst, std::string type, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Atomic Inst, inst='%s', type='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), type.c_str(), addr);
  }

  void BeforeCall(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                  Inst *inst, address_t target) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Call, inst='%s', target=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), target);
  }

  void AfterCall(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                 Inst *inst, address_t target, address_t ret) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Call, inst='%s', target=0x%lx, ret=0x%lx\n"
	, curr_thd_id, inst->ToString().c_str(), target, ret);
  }

  void BeforeReturn(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, address_t target) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Return, inst='%s', target=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), target);
  }

  void AfterReturn(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                   Inst *inst, address_t target) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Return, inst='%s', target=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), target);
  }

  void BeforePthreadCreate(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                           Inst *inst) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Before PthreadCreate, inst='%s'\n",
                        curr_thd_id, inst->ToString().c_str());
  }

  void AfterPthreadCreate(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                          Inst *inst, thread_id_t child_thd_id) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] After PthreadCreate, inst='%s', child=%" 
	    PRIx64 "\n", curr_thd_id, inst->ToString().c_str(), child_thd_id);
  }

  void BeforePthreadJoin(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                         Inst *inst, thread_id_t child_thd_id) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Before PthreadJoin, inst='%s', child=%" 
	    PRIx64 "\n", curr_thd_id, inst->ToString().c_str(), child_thd_id);
  }

  void AfterPthreadJoin(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                        Inst *inst, thread_id_t child_thd_id) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] After PthreadJoin, inst='%s', child=%" 
	    PRIx64 "\n", curr_thd_id, inst->ToString().c_str(), child_thd_id);
  }

  void BeforePthreadMutexTryLock(thread_id_t curr_thd_id,
                                 timestamp_t curr_thd_clk, Inst *inst,
                                 address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadMutexTryLock, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterPthreadMutexTryLock(thread_id_t curr_thd_id,
                                timestamp_t curr_thd_clk, Inst *inst,
                                address_t addr, int ret_val) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadMutexTryLock, inst='%s', addr=0x%lx, ret_val=%d\n",
        curr_thd_id, inst->ToString().c_str(), addr, ret_val);
  }

  void BeforePthreadMutexLock(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                              Inst *inst, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadMutexLock, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterPthreadMutexLock(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                             Inst *inst, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadMutexLock, inst='%s', addr=0x%lx, clk=%" 
	PRIx64 "\n", curr_thd_id, inst->ToString().c_str(), addr, curr_thd_clk);
  }

  void BeforePthreadMutexUnlock(thread_id_t curr_thd_id,
                                timestamp_t curr_thd_clk, Inst *inst,
                                address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadMutexUnlock, inst='%s', addr=0x%lx, clk=%" 
	PRIx64 "\n", curr_thd_id, inst->ToString().c_str(), addr, curr_thd_clk);
  }

  void AfterPthreadMutexUnlock(thread_id_t curr_thd_id,
                               timestamp_t curr_thd_clk, Inst *inst,
                               address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadMutexUnlock, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void BeforePthreadCondSignal(thread_id_t curr_thd_id,
                               timestamp_t curr_thd_clk, Inst *inst,
                               address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadCondSignal, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterPthreadCondSignal(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                              Inst *inst, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadCondSignal, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void BeforePthreadCondBroadcast(thread_id_t curr_thd_id,
                                  timestamp_t curr_thd_clk, Inst *inst,
                                  address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadCondBroadcast, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterPthreadCondBroadcast(thread_id_t curr_thd_id,
                                 timestamp_t curr_thd_clk, Inst *inst,
                                 address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadCondBroadcast, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void BeforePthreadCondWait(thread_id_t curr_thd_id,
                             timestamp_t curr_thd_clk, Inst *inst,
                             address_t cond_addr, address_t mutex_addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadCondWait, inst='%s',"
        "cond_addr=0x%lx, mutex_addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), cond_addr, mutex_addr);
  }

  void AfterPthreadCondWait(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                            Inst *inst, address_t cond_addr,
                            address_t mutex_addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadCondWait, inst='%s',"
        "cond_addr=0x%lx, mutex_addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), cond_addr, mutex_addr);
  }

  void BeforePthreadCondTimedwait(thread_id_t curr_thd_id,
                                  timestamp_t curr_thd_clk, Inst *inst,
                                  address_t cond_addr, address_t mutex_addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadCondTimedwait, inst='%s',"
        "cond_addr=0x%lx, mutex_addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), cond_addr, mutex_addr);
  }

  void AfterPthreadCondTimedwait(thread_id_t curr_thd_id,
                                 timestamp_t curr_thd_clk, Inst *inst,
                                 address_t cond_addr, address_t mutex_addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadCondTimedwait, inst='%s', "
        "cond_addr=0x%lx, mutex_addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), cond_addr, mutex_addr);
  }

  void BeforePthreadBarrierInit(thread_id_t curr_thd_id,
                                timestamp_t curr_thd_clk, Inst *inst,
                                address_t addr, unsigned int count) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadBarrierInit, inst='%s', addr=0x%lx, count=%u\n",
        curr_thd_id, inst->ToString().c_str(), addr, count);
  }

  void AfterPthreadBarrierInit(thread_id_t curr_thd_id,
                               timestamp_t curr_thd_clk, Inst *inst,
                               address_t addr, unsigned int count) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadBarrierInit, inst='%s', addr=0x%lx\n, count=%u\n",
        curr_thd_id, inst->ToString().c_str(), addr, count);
  }

  void BeforePthreadBarrierWait(thread_id_t curr_thd_id,
                                timestamp_t curr_thd_clk, Inst *inst,
                                address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before PthreadBarrierWait, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterPthreadBarrierWait(thread_id_t curr_thd_id,
                               timestamp_t curr_thd_clk, Inst *inst,
                               address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After PthreadBarrierWait, inst='%s', addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void BeforeMalloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, size_t size) {
    INFO_FMT_PRINT_SAFE(
      "[T%" PRIx64 "] Before Malloc, inst='%s', size=%zu\n",
      curr_thd_id, inst->ToString().c_str(), size);
  }

  void AfterMalloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                   Inst *inst, size_t size, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Malloc, inst='%s', size=%zu, addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), size, addr);
  }

  void BeforeCalloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, size_t nmemb, size_t size) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Calloc, inst='%s', nmemb=%zu, size=%zu\n",
        curr_thd_id, inst->ToString().c_str(), nmemb, size);
  }

  void AfterCalloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                   Inst *inst, size_t nmemb, size_t size, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Calloc, inst='%s', nmemb=%zu, size=%zu, addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), nmemb, size, addr);
  }

  void BeforeRealloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                     Inst *inst, address_t ori_addr, size_t size) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] Before Realloc, inst='%s', ori_addr=0x%lx, size=%zu\n",
        curr_thd_id, inst->ToString().c_str(), ori_addr, size);
  }

  void AfterRealloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, address_t ori_addr, size_t size,
                    address_t new_addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Realloc, inst='%s', ori_addr=0x%lx, "
        "size=%zu, new_addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), ori_addr, size, new_addr);
  }

  void BeforeFree(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                  Inst *inst, address_t addr) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Before Free, inst='%s', addr=0x%lx\n",
                        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void AfterFree(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                 Inst *inst, address_t addr) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] After Free, inst='%s', addr=0x%lx\n",
                        curr_thd_id, inst->ToString().c_str(), addr);
  }

  void BeforeValloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, size_t size) {
    INFO_FMT_PRINT_SAFE("[T%" PRIx64 "] Before Valloc, inst='%s', size=%zu\n",
                        curr_thd_id, inst->ToString().c_str(), size);
  }

  void AfterValloc(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                   Inst *inst, size_t size, address_t addr) {
    INFO_FMT_PRINT_SAFE(
        "[T%" PRIx64 "] After Valloc, inst='%s', size=%zu, addr=0x%lx\n",
        curr_thd_id, inst->ToString().c_str(), size, addr);
  }

 private:
  DISALLOW_COPY_CONSTRUCTORS(DebugAnalyzer);
};

#endif

