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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ContentMain.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentMain_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * Initialize application context in native side.
 **/
void ContentMain::InitApplicationContext(
    /* [in] */ IContext* context)
{
    NativeInitApplicationContext(context);
}

/**
 * Start the ContentMainRunner in native side.
 **/
Int32 ContentMain::Start()
{
    return NativeStart();
}

void ContentMain::NativeInitApplicationContext(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    Elastos_ContentMain_nativeInitApplicationContext(c);
}

Int32 ContentMain::NativeStart()
{
    return Elastos_ContentMain_nativeStart();
}

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
