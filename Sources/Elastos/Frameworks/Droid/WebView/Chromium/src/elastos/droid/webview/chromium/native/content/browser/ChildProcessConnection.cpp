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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessConnection.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

// Names of items placed in the bind intent or connection bundle.
const String ChildProcessConnection::EXTRA_COMMAND_LINE("com.google.android.apps.chrome.extra.command_line");

// Note the FDs may only be passed in the connection bundle.
const String ChildProcessConnection::EXTRA_FILES_PREFIX("com.google.android.apps.chrome.extra.extraFile_");

const String ChildProcessConnection::EXTRA_FILES_ID_SUFFIX("_id");

const String ChildProcessConnection::EXTRA_FILES_FD_SUFFIX("_fd");

// Used to pass the CPU core count to child processes.
const String ChildProcessConnection::EXTRA_CPU_COUNT("com.google.android.apps.chrome.extra.cpu_count");

// Used to pass the CPU features mask to child processes.
const String ChildProcessConnection::EXTRA_CPU_FEATURES("com.google.android.apps.chrome.extra.cpu_features");

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
