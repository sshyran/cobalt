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

#ifndef STARBOARD_SHARED_UWP_APPLICATION_UWP_H_
#define STARBOARD_SHARED_UWP_APPLICATION_UWP_H_

#include <agile.h>

#include <string>
#include <unordered_map>

#include "starboard/configuration.h"
#include "starboard/mutex.h"
#include "starboard/shared/internal_only.h"
#include "starboard/shared/starboard/application.h"
#include "starboard/shared/starboard/command_line.h"
#include "starboard/shared/uwp/winrt_workaround.h"
#include "starboard/types.h"
#include "starboard/window.h"

namespace starboard {
namespace shared {
namespace uwp {

// Returns win32's GetModuleFileName(). For cases where we'd like an argv[0].
std::string GetArgvZero();

class ApplicationUwp : public shared::starboard::Application {
 public:
  ApplicationUwp();
  ~ApplicationUwp() SB_OVERRIDE;

  static ApplicationUwp* Get() {
    return static_cast<ApplicationUwp*>(shared::starboard::Application::Get());
  }

  SbWindow CreateWindowForUWP(const SbWindowOptions* options);

  bool DestroyWindow(SbWindow window);

  void DispatchStart() {
    shared::starboard::Application::DispatchStart();
  }

  // public for IFrameworkView subclass
  void SetCommandLine(int argc, const char** argv) {
    shared::starboard::Application::SetCommandLine(argc, argv);
  }

  // public for IFrameworkView subclass
  bool DispatchAndDelete(Application::Event* event) {
    return shared::starboard::Application::DispatchAndDelete(event);
  }

  Platform::Agile<Windows::UI::Core::CoreWindow> GetCoreWindow() const {
    return core_window_;
  }

  // public for IFrameworkView subclass
  void SetCoreWindow(Windows::UI::Core::CoreWindow^ window) {
    core_window_ = window;
  }

  void OnKeyEvent(Windows::UI::Core::CoreWindow^ sender,
      Windows::UI::Core::KeyEventArgs^ args, bool up);

  void Inject(Event* event) SB_OVERRIDE;

  void SetStartLink(const char* link) SB_OVERRIDE {
    shared::starboard::Application::SetStartLink(link);
  }

 private:
  // --- Application overrides ---
  bool IsStartImmediate() SB_OVERRIDE { return false; }
  void Initialize() SB_OVERRIDE;
  void Teardown() SB_OVERRIDE;
  Event* GetNextEvent() SB_OVERRIDE;
  bool DispatchNextEvent() SB_OVERRIDE;
  void InjectTimedEvent(TimedEvent* timed_event) SB_OVERRIDE;
  void CancelTimedEvent(SbEventId event_id) SB_OVERRIDE;
  TimedEvent* GetNextDueTimedEvent() SB_OVERRIDE;
  SbTimeMonotonic GetNextTimedEventTargetTime() SB_OVERRIDE;

  // The single open window, if any.
  SbWindow window_;
  Platform::Agile<Windows::UI::Core::CoreWindow> core_window_;

  Mutex mutex_;
  // Locked by mutex_
  std::unordered_map<SbEventId, Windows::System::Threading::ThreadPoolTimer^>
    timer_event_map_;
};

}  // namespace uwp
}  // namespace shared
}  // namespace starboard

#endif  // STARBOARD_SHARED_UWP_APPLICATION_UWP_H_