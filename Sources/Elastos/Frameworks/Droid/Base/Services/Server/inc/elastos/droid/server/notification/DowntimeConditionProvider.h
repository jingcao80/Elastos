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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_DOWNTIMECONDITIONPROVIDER_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_DOWNTIMECONDITIONPROVIDER_H__

#include <_Elastos.Droid.Server.h>
#include "elastos/droid/server/notification/ManagedServices.h"
#include "elastos/droid/service/notification/ConditionProviderService.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Server::Notification::ManagedServices;
using Elastos::Droid::Service::Notification::ConditionProviderService;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Service::Notification::IZenModeConfigDowntimeInfo;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Core::ICharSequence;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/** Built-in zen condition provider for managing downtime */
class DowntimeConditionProvider
    : public ConditionProviderService
{
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ DowntimeConditionProvider* host);

        ~MyBroadcastReceiver();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        // @Override
        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "DowntimeConditionProvider::MyBroadcastReceiver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        DowntimeConditionProvider* mHost;
    };

public:
    DowntimeConditionProvider();

    ~DowntimeConditionProvider();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DumpFilter* filter);

    CARAPI_(void) AttachBase(
        /* [in] */ IContext* base);

    CARAPI_(void) SetCallback(
        /* [in] */ IDowntimeConditionProviderCallback* callback);

    // @Override
    CARAPI OnConnected();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnRequestConditions(
        /* [in] */ Int32 relevance);

    // @Override
    CARAPI OnSubscribe(
        /* [in] */ IUri* conditionId);

    // @Override
    CARAPI OnUnsubscribe(
        /* [in] */ IUri* conditionId);

    CARAPI_(void) SetConfig(
        /* [in] */ IZenModeConfig* config);

    CARAPI_(Boolean) IsInDowntime();

    CARAPI_(AutoPtr<ICondition>) CreateCondition(
        /* [in] */ IZenModeConfigDowntimeInfo* downtime,
        /* [in] */ Int32 state);

    CARAPI_(Boolean) IsDowntimeCondition(
        /* [in] */ ICondition* condition);

private:
    CARAPI_(void) Init();

    CARAPI_(void) UpdateDays();

    CARAPI_(Boolean) IsInDowntime(
        /* [in] */ Int64 time);

    CARAPI_(Boolean) IsInDowntime(
        /* [in] */ Int32 daysOffset,
        /* [in] */ Int64 time,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    CARAPI_(void) ReevaluateDowntime();

    CARAPI_(void) FireDowntimeChanged();

    CARAPI_(void) UpdateAlarms();

    CARAPI_(Int32) GetDayOfWeek(
        /* [in] */ Int64 time);

    CARAPI_(Int64) GetTime(
        /* [in] */ Int64 millis,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 min);

    CARAPI_(Int64) AddDays(
        /* [in] */ Int64 time,
        /* [in] */ Int32 days);

    CARAPI_(void) UpdateAlarm(
        /* [in] */ const String& action,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 hr,
        /* [in] */ Int32 min);

    static CARAPI_(String) Ts(
        /* [in] */ Int64 time);

public:
    static const AutoPtr<IComponentName> COMPONENT; // new ComponentName("android", DowntimeConditionProvider.class.getName());

private:
    static const String TAG;
    static const Boolean DEBUG;// Log.isLoggable(TAG, Log.DEBUG);

    static const String ENTER_ACTION;
    static const Int32 ENTER_CODE;
    static const String EXIT_ACTION;
    static const Int32 EXIT_CODE;
    static const String EXTRA_TIME;

    AutoPtr<ICalendar> mCalendar;// = Calendar.getInstance();
    AutoPtr<IContext> mContext;// = this;
    AutoPtr<IArraySet> mDays;
    // final ArraySet<Integer> mDays = new ArraySet<Integer>();

    Boolean mConnected;
    Boolean mInDowntime;
    AutoPtr<IZenModeConfig> mConfig;
    AutoPtr<IDowntimeConditionProviderCallback> mCallback;
    AutoPtr<BroadcastReceiver> mReceiver;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_DOWNTIMECONDITIONPROVIDER_H__
