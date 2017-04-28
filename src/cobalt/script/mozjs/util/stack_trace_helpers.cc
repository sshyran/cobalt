// Copyright 2017 Google Inc. All Rights Reserved.
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

#ifndef NB_SCRIPT_STACKTRACE_H_
#define NB_SCRIPT_STACKTRACE_H_

#include "cobalt/script/mozjs/util/stack_trace_helpers.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "base/logging.h"
#include "base/stringprintf.h"
#include "cobalt/script/mozjs/util/exception_helpers.h"
#include "cobalt/script/stack_frame.h"
#include "nb/thread_local_object.h"
#include "starboard/memory.h"
#include "starboard/once.h"
#include "starboard/string.h"
#include "starboard/types.h"
#include "third_party/mozjs/js/src/jsapi.h"

namespace cobalt {
namespace script {
namespace mozjs {
namespace util {
namespace {

typedef nb::ThreadLocalObject<StackTraceGenerator> ThreadLocalJsStackTracer;

SB_ONCE_INITIALIZE_FUNCTION(ThreadLocalJsStackTracer,
                            s_thread_local_js_stack_tracer_singelton);

void ToStringAppend(const StackFrame& sf, std::string* out) {
  base::SStringPrintf(out, "%s(%d,%d):%s", sf.source_url.c_str(),
                      sf.line_number, sf.column_number,
                      sf.function_name.c_str());
}

}  // namespace.

void SetThreadLocalJSContext(JSContext* context) {
  GetThreadLocalStackTraceGenerator()->set_js_context(context);
}

JSContext* GetThreadLocalJSContext() {
  return GetThreadLocalStackTraceGenerator()->js_context();
}

StackTraceGenerator* GetThreadLocalStackTraceGenerator() {
  return s_thread_local_js_stack_tracer_singelton()->GetOrCreate();
}

//////////////////////////////////// IMPL /////////////////////////////////////

StackTraceGenerator::StackTraceGenerator() : js_context_(NULL) {}
StackTraceGenerator::~StackTraceGenerator() {}

bool StackTraceGenerator::Valid() { return js_context_ != NULL; }

bool StackTraceGenerator::GenerateStackTrace(
    int depth, nb::RewindableVector<StackFrame>* out) {
  DCHECK(thread_checker_.CalledOnValidThread());
  out->rewindAll();
  if (!Valid()) {
    return false;
  }
  GetStackTrace(js_context_, depth, out);
  return !out->empty();
}

bool StackTraceGenerator::GenerateStackTraceLines(
    int depth, nb::RewindableVector<std::string>* out) {
  DCHECK(thread_checker_.CalledOnValidThread());
  out->rewindAll();
  nb::RewindableVector<StackFrame>& stack_frames = scratch_data_.stack_frames_;
  if (!GenerateStackTrace(depth, &stack_frames)) {
    return false;
  }

  for (size_t i = 0; i < stack_frames.size(); ++i) {
    std::string& current_string = out->grow(1);
    current_string.assign("");  // Should not deallocate memory.
    StackFrame& sf = stack_frames[i];
    ToStringAppend(sf, &current_string);
  }
  return true;
}

bool StackTraceGenerator::GenerateStackTraceString(int depth,
                                                   std::string* out) {
  DCHECK(thread_checker_.CalledOnValidThread());
  out->assign("");  // Should not deallocate memory.

  nb::RewindableVector<StackFrame>& stack_frames = scratch_data_.stack_frames_;
  if (!GenerateStackTrace(depth, &stack_frames)) {
    return false;
  }

  for (size_t i = 0; i < stack_frames.size(); ++i) {
    cobalt::script::StackFrame& sf = stack_frames[i];
    ToStringAppend(sf, out);
    if (i < stack_frames.size() - 1) {
      base::SStringPrintf(out, "\n");
    }
  }
  return true;
}

bool StackTraceGenerator::GenerateStackTraceString(int depth, char* buff,
                                                   size_t buff_size) {
  DCHECK(thread_checker_.CalledOnValidThread());
  SbMemorySet(buff, 0, buff_size);
  std::string& scratch_symbol = scratch_data_.symbol_;

  if (!GenerateStackTraceString(depth, &scratch_symbol)) {
    return false;
  }

  SbStringCopy(buff, scratch_symbol.c_str(), buff_size);
  return true;
}

JSContext* StackTraceGenerator::js_context() { return js_context_; }

void StackTraceGenerator::set_js_context(JSContext* js_ctx) {
  js_context_ = js_ctx;
}

}  // namespace util
}  // namespace mozjs
}  // namespace script
}  // namespace cobalt

#endif  // NB_SCRIPT_STACKTRACE_H_