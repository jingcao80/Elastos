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

#ifndef __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__
#define __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__

#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>

using Elastos::Droid::App::IIProfileManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Wifi::IWifiManager;

namespace Elastos {
namespace Droid {
namespace Server {

class ProfileTriggerHelper
    : public BroadcastReceiver
{
private:
    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ProfileTriggerHelper::MyContentObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ ProfileTriggerHelper* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        ProfileTriggerHelper* mHost;
    };

public:
    TO_STRING_IMPL("ProfileTriggerHelper")

    ProfileTriggerHelper();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIProfileManager* service);

    CARAPI_(void) UpdateEnabled();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) CheckTriggers(
        /* [in] */ Int32 type,
        /* [in] */ const String& id,
        /* [in] */ Int32 newState);

    CARAPI_(String) GetActiveSSID();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IIProfileManager> mService;
    AutoPtr<IWifiManager> mWifiManager;
    String mLastConnectedSSID;
    AutoPtr<IIntentFilter> mIntentFilter;
    Boolean mFilterRegistered;

    AutoPtr<IContentObserver> mSettingsObserver;

private:
    static const String TAG;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__
