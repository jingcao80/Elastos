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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_COUNTDOWNCONDITIONPROVIDER_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_COUNTDOWNCONDITIONPROVIDER_H__

#include "elastos/droid/server/notification/ManagedServices.h"
#include "elastos/droid/service/notification/ConditionProviderService.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Server::Notification::ManagedServices;
using Elastos::Droid::Service::Notification::ConditionProviderService;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/** Built-in zen condition provider for simple time-based conditions */
class CountdownConditionProvider
    : public ConditionProviderService
{
private:
    class Receiver
        : public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ CountdownConditionProvider* host);

        ~Receiver();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        // @Override
        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "CountdownConditionProvider::Receiver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        CountdownConditionProvider* mHost;
    };

public:
    CountdownConditionProvider();

    ~CountdownConditionProvider();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DumpFilter* filter);

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

    static CARAPI_(String) TryParseDescription(
        /* [in] */ IUri* conditionUri);

    CARAPI_(void) AttachBase(
        /* [in] */ IContext* base);

private:
    static CARAPI_(AutoPtr<ICondition>) NewCondition(
        /* [in] */ Int64 time,
        /* [in] */ Int32 state);

    static CARAPI_(String) Ts(
        /* [in] */ Int64 time);

public:
    static const AutoPtr<IComponentName> COMPONENT; // new ComponentName("android", CountdownConditionProvider.class.getName());

private:
    static const String TAG;
    static const Boolean DEBUG;// Log.isLoggable(TAG, Log.DEBUG);

    static const String ACTION;// CountdownConditionProvider.class.getName();
    static const Int32 REQUEST_CODE;
    static const String EXTRA_CONDITION_ID;

    AutoPtr<IContext> mContext;
    AutoPtr<Receiver> mReceiver;

    Boolean mConnected;
    Int64 mTime;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_COUNTDOWNCONDITIONPROVIDER_H__
