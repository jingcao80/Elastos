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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTMAIN_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTMAIN_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * This class is used to initialize all types of process. It corresponds to
 * content/public/app/content_main.h which is not used in Android as it has
 * the different initialization process.
 *
 * TODO(michaelbai): Refactorying the BrowserProcessMain.java and the
 * ChildProcessService.java to start ContentMain, and run the process
 * specific initialization code in ContentMainRunner::Initialize.
 *
 **/
//@JNINamespace("content")
class ContentMain
{
public:
    /**
     * Initialize application context in native side.
     **/
    static CARAPI_(void) InitApplicationContext(
        /* [in] */ IContext* context);

    /**
     * Start the ContentMainRunner in native side.
     **/
    static CARAPI_(Int32) Start();

private:
    static CARAPI_(void) NativeInitApplicationContext(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) NativeStart();
};

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTMAIN_H__
