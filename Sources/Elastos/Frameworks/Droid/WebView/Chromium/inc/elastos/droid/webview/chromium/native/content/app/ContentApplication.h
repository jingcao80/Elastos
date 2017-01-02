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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTAPPLICATION_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTAPPLICATION_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/base/BaseChromiumApplication.h"

using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Webkit::Webview::Chromium::Base::BaseChromiumApplication;

// import org.chromium.base.BaseChromiumApplication;
// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.content.browser.TracingControllerAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class TracingControllerElastos;

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::TracingControllerElastos;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * Basic application functionality that should be shared among all browser applications
 * based on the content layer.
 */
class ContentApplication : public BaseChromiumApplication
{
private:
    class InnerIdleHandler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        InnerIdleHandler(
            /* [in] */ ContentApplication* owner);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    private:
        ContentApplication* mOwner;
    };

public:
    CARAPI_(AutoPtr<TracingControllerElastos>) GetTracingController();

    //@Override
    CARAPI OnCreate();

    /**
     * For emulated process environment only. On a production device, the application process is
     * simply killed without calling this method. We don't need to unregister the broadcast
     * receiver in the latter case.
     */
    //@Override
    CARAPI OnTerminate();

private:
    AutoPtr<TracingControllerElastos> mTracingController;
};

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CONTENTAPPLICATION_H__
