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

#ifndef __ELASTOS_DROID_APP_CPENDINGINTENT_FINISHDISPATCHER_H__
#define __ELASTOS_DROID_APP_CPENDINGINTENT_FINISHDISPATCHER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_App_CPendingIntentFinishedDispatcher.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CPendingIntentFinishedDispatcher)
    , public Object
    , public IIntentReceiver
    , public IBinder
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CPendingIntentFinishedDispatcher* host);

        CARAPI Run();

    private:
        AutoPtr<CPendingIntentFinishedDispatcher> mHost;    // hold host
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPendingIntentFinishedDispatcher();

    ~CPendingIntentFinishedDispatcher();

    CARAPI constructor(
        /* [in] */ IPendingIntent* pi,
        /* [in] */ IPendingIntentOnFinished* who,
        /* [in] */ IHandler* handler);

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI Run();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IPendingIntent> mPendingIntent;
    AutoPtr<IPendingIntentOnFinished> mWho;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IIntent> mIntent;
    Int32 mResultCode;
    String mResultData;
    AutoPtr<IBundle> mResultExtras;

};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CPENDINGINTENT_FINISHDISPATCHER_H__
