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

#include "elastos/droid/systemui/screenshot/TakeScreenshotService.h"

using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

//---------------------------------------------------------
// TakeScreenshotService::MyRunnable
//---------------------------------------------------------

TakeScreenshotService::MyRunnable::MyRunnable(
    /* [in] */ IMessenger* callback)
    : mCallback(callback)
{}

ECode TakeScreenshotService::MyRunnable::Run()
{
    AutoPtr<IMessageHelper> mh;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&mh);
    AutoPtr<IMessage> reply;
    mh->Obtain(NULL, 1, (IMessage**)&reply);
    ECode ec = mCallback->Send(reply);
    if (FAILED(ec)) return E_REMOTE_EXCEPTION;
    return NOERROR;
}

//---------------------------------------------------------
// TakeScreenshotService::MyHandler
//---------------------------------------------------------
TakeScreenshotService::MyHandler::MyHandler(
    /* [in] */ TakeScreenshotService* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode TakeScreenshotService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case 1: {
            AutoPtr<IMessenger> callback;
            msg->GetReplyTo((IMessenger**)&callback);
            if (mHost->mScreenshot == NULL) {
                mHost->mScreenshot = new GlobalScreenshot((IContext*)mHost);
            }
            AutoPtr<MyRunnable> runnable = new MyRunnable(callback);
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHost->mScreenshot->TakeScreenshot(runnable, arg1 > 0, arg2 > 0);
        }
    }
    return NOERROR;
}

//---------------------------------------------------------
// TakeScreenshotService
//---------------------------------------------------------
const String TakeScreenshotService::TAG("TakeScreenshotService");
AutoPtr<GlobalScreenshot> TakeScreenshotService::mScreenshot;
CARAPI TakeScreenshotService::constructor()
{
    AutoPtr<MyHandler> h = new MyHandler(this);
    mHandler = h;
    return Service::constructor();
}

ECode TakeScreenshotService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    AutoPtr<IMessenger> m;
    CMessenger::New(mHandler, (IMessenger**)&m);
    m->GetBinder(b);
    return NOERROR;
}

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
