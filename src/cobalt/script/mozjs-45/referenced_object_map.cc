// Copyright 2016 The Cobalt Authors. All Rights Reserved.
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

#include "cobalt/script/mozjs-45/referenced_object_map.h"

#include <utility>

#include "cobalt/script/mozjs-45/mozjs_global_environment.h"
#include "cobalt/script/mozjs-45/util/algorithm_helpers.h"
#include "cobalt/script/mozjs-45/wrapper_private.h"
#include "nb/memory_scope.h"
#include "third_party/mozjs-45/js/src/jsapi.h"

namespace cobalt {
namespace script {
namespace mozjs {

ReferencedObjectMap::ReferencedObjectMap(JSContext* context)
    : context_(context) {}

ReferencedObjectMap::~ReferencedObjectMap() {
  DCHECK(referenced_objects_.empty());
}

// Add/Remove a reference from a WrapperPrivate to a JSValue.
void ReferencedObjectMap::AddReferencedObject(Wrappable* wrappable,
                                              JS::HandleValue referee) {
  TRACK_MEMORY_SCOPE("Javascript");
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(!referee.isNullOrUndefined());
  DCHECK(referee.isGCThing());

  // Force a wrapper to get created for |wrappable| in order to ensure it will
  // get traced by SpiderMonkey, allowing us to find |referee| later.
  WrapperPrivate* wrapper_private = WrapperPrivate::GetFromWrappable(
      wrappable, context_,
      MozjsGlobalEnvironment::GetFromContext(context_)->wrapper_factory());

  referenced_objects_.insert(
      std::make_pair(wrappable, WeakHeapObject(context_, referee)));
}

void ReferencedObjectMap::RemoveReferencedObject(Wrappable* wrappable,
                                                 JS::HandleValue referee) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  auto pair_range = referenced_objects_.equal_range(wrappable);
  for (auto it = pair_range.first; it != pair_range.second; ++it) {
    JS::RootedValue element(context_, it->second.GetValue());
    if (util::IsSameGcThing(context_, referee, element)) {
      // There may be multiple mappings between a specific owner and a JS
      // object. Only remove the first mapping.
      referenced_objects_.erase(it);
      return;
    }
  }
  DLOG(WARNING) << "No reference to the specified object found.";
}

void ReferencedObjectMap::TraceReferencedObjects(JSTracer* trace,
                                                 Wrappable* wrappable) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  auto pair_range = referenced_objects_.equal_range(wrappable);
  for (auto it = pair_range.first; it != pair_range.second; ++it) {
    it->second.Trace(trace);
  }
}

void ReferencedObjectMap::RemoveNullReferences() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  for (auto it = referenced_objects_.begin(); it != referenced_objects_.end();
       /*Incremented in the loop */) {
    if (it->second.WasCollected()) {
      auto erase_iterator = it++;
      referenced_objects_.erase(erase_iterator);
    } else {
      DCHECK(it->second.IsGcThing());
      ++it;
    }
  }
}

}  // namespace mozjs
}  // namespace script
}  // namespace cobalt
