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

#ifndef __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_Elastos_DevSamples_NotificationDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace NotificationDemo {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
        Int32 mRef;
    };

public:
    CAR_OBJECT_DECL()

    CActivityOne();

    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI StartStatusBarNotification();

    CARAPI StartNotification();
    CARAPI UpdateNotification();
    CARAPI CancelNotification();

    CARAPI NotifyNotification(
        /* [in] */ Boolean isUpdate);

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI SendNotification(
        /* [in] */ const String& title,
        /* [in] */ const String& message);

private:
    friend class MyListener;

    static const String TAG;
    Int32 mNotificationID;
    Int32 mNumMessages;
};

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__
