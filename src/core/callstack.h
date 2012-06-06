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

// File: core/callstack.h - Tracking runtime call stacks.

#ifndef CORE_CALLSTACK_H_
#define CORE_CALLSTACK_H_

#include "core/basictypes.h"
#include "core/analyzer.h"

// Define the type for call stack signatures.
typedef uint64 callstack_sig_t;

// This class represents a runtime call stack of a thread.
class CallStack {
 public:
  CallStack();
  ~CallStack();

  callstack_sig_t signature();
  void OnCall(Inst *inst, address_t ret_addr);
  void OnReturn(Inst *inst, address_t target_addr);

 protected:
  typedef std::vector<Inst *> InstVec;
  typedef std::vector<address_t> TargetVec;

  callstack_sig_t curr_sig_; // The current call stack signature
  InstVec inst_vec_;
  TargetVec target_vec_;

 private:
  DISALLOW_COPY_CONSTRUCTORS(CallStack);
};

// This class stores information about runtime call stacks (all threads).
class CallStackInfo {
 public:
  CallStackInfo() {}
  ~CallStackInfo() {}

 protected:
  typedef std::map<thread_id_t, CallStack *> StackMap;

  StackMap stack_map_;

 privete:
  DISALLOW_COPY_CONSTRUCTORS(CallStackInfo);
};

// The call stack tracker analyzer is used to track runtime call
// stacks by monitoring every call and return.
class CallStackTracker : public Analyzer {
 public:
  CallStackTracer(Knob *knob, CallStackInfo *info);
  virtual ~CallStackTracer();

  virtual void Call(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                    Inst *inst, address_t ret_addr, address_t target_addr);
  virtual void Return(thread_id_t curr_thd_id, timestamp_t curr_thd_clk,
                      Inst *inst, address_t target_addr);

 protected:
  CallStackInfo *info_;

 private:
  DISALLOW_COPY_CONSTRUCTORS(CallStackTracer);
};

#endif

