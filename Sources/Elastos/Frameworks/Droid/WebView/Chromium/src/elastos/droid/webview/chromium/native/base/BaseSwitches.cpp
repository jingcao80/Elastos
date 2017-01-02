//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/webkit/webview/chromium/native/base/BaseSwitches.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

// Block onCreate() of Chrome until a Java debugger is attached.
const String BaseSwitches::WAIT_FOR_JAVA_DEBUGGER("wait-for-java-debugger");

// Overrides low-end device detection, disabling low-end device optimizations.
const String BaseSwitches::DISABLE_LOW_END_DEVICE_MODE("disable-low-end-device-mode");

// Overrides low-end device detection, enabling low-end device optimizations.
const String BaseSwitches::ENABLE_LOW_END_DEVICE_MODE("enable-low-end-device-mode");

// Adds additional thread idle time information into the trace event output.
const String BaseSwitches::ENABLE_IDLE_TRACING("enable-idle-tracing");

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
