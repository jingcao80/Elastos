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

#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

class TakeScreenshotService
    : public Elastos::Droid::App::Service
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IMessenger* callback);

        CARAPI Run();

    private:
        AutoPtr<IMessenger> mCallback;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TakeScreenshotService::MyHandler")

        MyHandler(
            /* [in] */ TakeScreenshotService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        TakeScreenshotService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

private:
    const static String TAG;

    static AutoPtr<GlobalScreenshot> mScreenshot;

    AutoPtr<IHandler> mHandler;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__