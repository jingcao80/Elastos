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

#ifndef  __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_COtaStartupReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::IPhoneStateListener;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/*
 * Handles OTA Start procedure at phone power up. At phone power up, if phone is not OTA
 * provisioned (check MIN value of the Phone) and 'device_provisioned' is not set,
 * OTA Activation screen is shown that helps user activate the phone
 */
CarClass(COtaStartupReceiver)
    , public BroadcastReceiver
{
private:
    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("COtaStartupReceiver::MyPhoneStateListener")

        MyPhoneStateListener(
            /* [in] */ COtaStartupReceiver* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnOtaspChanged(
            /* [in] */ Int32 otaspMode);

    private:
        COtaStartupReceiver* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("COtaStartupReceiver::MyHandler")

        MyHandler(
            /* [in] */ COtaStartupReceiver* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        COtaStartupReceiver* mHost;
    };

public:
    CAR_OBJECT_DECL();

    COtaStartupReceiver();

    CARAPI constructor();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * On devices that provide a phone initialization wizard (such as Google Setup Wizard), we
     * allow delaying CDMA OTA setup so it can be done in a single wizard. The wizard is responsible
     * for (1) disabling itself once it has been run and/or (2) setting the 'device_provisioned'
     * flag to something non-zero and (3) calling the OTA Setup with the action below.
     *
     * NB: Typical phone initialization wizards will install themselves as the homescreen
     * (category "android.intent.category.HOME") with a priority higher than the default.
     * The wizard should set 'device_provisioned' when it completes, disable itself with the
     * PackageManager.setComponentEnabledSetting() and then start home screen.
     *
     * @return true if setup will be handled by wizard, false if it should be done now.
     */
    CARAPI_(Boolean) ShouldPostpone(
        /* [in] */ IContext* context);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 MIN_READY = 10;
    static const Int32 SERVICE_STATE_CHANGED = 11;
    AutoPtr<IContext> mContext;

    Int32 mOtaspMode;
    Boolean mPhoneStateListenerRegistered;
    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__