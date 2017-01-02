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

#include "elastos/droid/app/CPendingIntentFinishedDispatcher.h"
#include "unistd.h"

namespace Elastos {
namespace Droid {
namespace App {

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::EIID_IIntentReceiver;

CPendingIntentFinishedDispatcher::MyRunnable::MyRunnable(
    /* [in] */ CPendingIntentFinishedDispatcher* host)
    : mHost(host)
{}

CPendingIntentFinishedDispatcher::MyRunnable::Run()
{
    ECode ec = mHost->Run();
    mHost = NULL;       // release ref-count
    return ec;
}

CAR_INTERFACE_IMPL_2(CPendingIntentFinishedDispatcher, Object, IIntentReceiver, IBinder)

CAR_OBJECT_IMPL(CPendingIntentFinishedDispatcher)

CPendingIntentFinishedDispatcher::CPendingIntentFinishedDispatcher()
    : mResultCode(0)
{}

CPendingIntentFinishedDispatcher::~CPendingIntentFinishedDispatcher()
{
    mWho = NULL;
    mHandler = NULL;
    mIntent = NULL;
    mResultExtras = NULL;
}

ECode CPendingIntentFinishedDispatcher::constructor(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ IPendingIntentOnFinished* who,
    /* [in] */ IHandler* handler)
{
    assert(who != NULL);
    mPendingIntent = pi;
    mWho = who;
    mHandler = handler;
    return NOERROR;
}

ECode CPendingIntentFinishedDispatcher::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String&  data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    mIntent = intent;
    mResultCode = resultCode;
    mResultData = data;
    mResultExtras = extras;

    Boolean result;
    if (NULL == mHandler) {
        FAIL_RETURN(Run());
    }
    else {
        AutoPtr<IRunnable> runnable = new MyRunnable(this);
        mHandler->Post(runnable, &result);
    }
    return NOERROR;
}

ECode CPendingIntentFinishedDispatcher::Run()
{
    return mWho->OnSendFinished(mPendingIntent, mIntent, mResultCode, mResultData, mResultExtras);
}

ECode CPendingIntentFinishedDispatcher::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
