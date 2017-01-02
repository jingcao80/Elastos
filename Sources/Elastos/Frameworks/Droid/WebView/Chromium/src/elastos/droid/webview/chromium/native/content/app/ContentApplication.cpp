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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ContentApplication.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/TracingControllerElastos.h"

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

//=====================================================================
//            ContentApplication::InnerIdleHandler
//=====================================================================

CAR_INTERFACE_IMPL(ContentApplication::InnerIdleHandler, Object, IIdleHandler);

ContentApplication::InnerIdleHandler::InnerIdleHandler(
    /* [in] */ ContentApplication* owner)
    : mOwner(owner)
{
}

ECode ContentApplication::InnerIdleHandler::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Will retry if the native library has not been initialized.
    // these code may be not needed, because in elastos no libraryloader need.
    // if (!LibraryLoader::IsInitialized()) {
    //     *result = TRUE;
    //     return result;
    // }

    // try {
        AutoPtr<IContext> context = IContext::Probe(mOwner);
        mOwner->GetTracingController()->RegisterReceiver(context);
    // } catch (SecurityException e) {
    //     // Happens if the process is isolated. Ignore.
    // }

    // Remove the idle handler.
    *result = FALSE;

    return NOERROR;
}

//=====================================================================
//                     ContentApplication
//=====================================================================

AutoPtr<TracingControllerElastos> ContentApplication::GetTracingController()
{
    if (mTracingController == NULL) {
        AutoPtr<IContext> context = this;
        mTracingController = new TracingControllerElastos(context);
    }
    return mTracingController;
}

//@Override
ECode ContentApplication::OnCreate()
{
    BaseChromiumApplication::OnCreate();

    // Delay TracingControllerAndroid.registerReceiver() until the main loop is idle.
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<IMessageQueue> queue;
    helper->GetMyQueue((IMessageQueue**)&queue);
    AutoPtr<IIdleHandler> handler = new InnerIdleHandler(this);
    queue->AddIdleHandler(handler);

    return NOERROR;
}

/**
 * For emulated process environment only. On a production device, the application process is
 * simply killed without calling this method. We don't need to unregister the broadcast
 * receiver in the latter case.
 */
//@Override
ECode ContentApplication::OnTerminate()
{
    // try {
        AutoPtr<IContext> context = this;
        GetTracingController()->UnregisterReceiver(context);
    // } catch (SecurityException e) {
    //     // Happens if the process is isolated. Ignore.
    // }

    BaseChromiumApplication::OnTerminate();

    return NOERROR;
}

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
