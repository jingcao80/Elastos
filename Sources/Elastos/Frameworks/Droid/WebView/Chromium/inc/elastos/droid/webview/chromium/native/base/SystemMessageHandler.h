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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSTEMMESSAGEHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSTEMMESSAGEHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

class SystemMessageHandler
    : public Handler
{
public:
    //@Override
    ECode HandleMessage(
        /* [in] */ IMessage* msg);

    static CARAPI_(void*) ElaSystemMessageHandlerCallback_Init();

private:
    static CARAPI_(void) ScheduleWork(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ScheduleDelayedWork(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 delayedTimeTicks,
        /* [in] */ Int64 millis);

    static CARAPI_(void) RemoveAllPendingMessages(
        /* [in] */ IInterface* obj);

    SystemMessageHandler(
        /* [in] */ Int64 messagePumpDelegateNative);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ScheduleWork();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ScheduleDelayedWork(
        /* [in] */ Int64 delayedTimeTicks,
        /* [in] */ Int64 millis);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) RemoveAllPendingMessages();

    //@CalledByNative return SystemMessageHandler
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Int64 messagePumpDelegateNative);

    CARAPI_(void) NativeDoRunLoopOnce(
        /* [in] */ Int64 messagePumpDelegateNative,
        /* [in] */ Int64 delayedScheduledTimeTicks);

private:
    static const Int32 SCHEDULED_WORK = 1;
    static const Int32 DELAYED_SCHEDULED_WORK = 2;

    // Native class pointer set by the constructor of the SharedClient native class.
    Int64 mMessagePumpDelegateNative;
    Int64 mDelayedScheduledTimeTicks;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSTEMMESSAGEHANDLER_H__
