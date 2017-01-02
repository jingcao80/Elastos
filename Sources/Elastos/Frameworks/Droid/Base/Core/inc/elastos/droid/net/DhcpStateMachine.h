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

#ifndef __ELASTOS_DROID_NET_DHCPSTATEMACHINE_H__
#define __ELASTOS_DROID_NET_DHCPSTATEMACHINE_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerWakeLock;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * StateMachine that interacts with the native DHCP client and can talk to
 * a controller that also needs to be a StateMachine
 *
 * The Dhcp state machine provides the following features:
 * - Wakeup and renewal using the native DHCP client  (which will not renew
 *   on its own when the device is in suspend state and this can lead to device
 *   holding IP address beyond expiry)
 * - A notification right before DHCP request or renewal is started. This
 *   can be used for any additional setup before DHCP. For example, wifi sets
 *   BT-Wifi coex settings right before DHCP is initiated
 *
 * @hide
 */
class DhcpStateMachine
    : public StateMachine
    , public IDhcpStateMachine
{
public:
    class DefaultState
        : public State
    {
    public:
        DefaultState(
            /* [in] */ DhcpStateMachine* owner)
            : mOwner(owner)
        {}

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DhcpStateMachine* mOwner;
    };

    class StoppedState
        : public State
    {
    public:
        StoppedState(
            /* [in] */ DhcpStateMachine* owner)
            : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DhcpStateMachine* mOwner;
    };

    class WaitBeforeStartState
        : public State
    {
    public:
        WaitBeforeStartState(
            /* [in] */ DhcpStateMachine* owner)
            : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DhcpStateMachine* mOwner;
    };

    class RunningState
        : public State
    {
    public:
        RunningState(
            /* [in] */ DhcpStateMachine* owner)
            : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DhcpStateMachine* mOwner;
    };

    class WaitBeforeRenewalState
        : public State
    {
    public:
        WaitBeforeRenewalState(
            /* [in] */ DhcpStateMachine* owner)
            : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        DhcpStateMachine* mOwner;
    };

private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ DhcpStateMachine* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* ctx,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("DhcpStateMachine::MyBroadcastReceiver: ")

    private:
        DhcpStateMachine* mOwner;
    };

private:
    enum DhcpAction {
        DhcpAction_START,
        DhcpAction_RENEW
    };

public:
    CAR_INTERFACE_DECL()

    DhcpStateMachine();

    static CARAPI_(AutoPtr<IDhcpStateMachine>) MakeDhcpStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ IStateMachine* controller,
        /* [in] */ const String& intf);

    CARAPI RegisterForPreDhcpNotification();

    CARAPI DoQuit();

protected:
    CARAPI_(void) OnQuitting();

private:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IStateMachine* controller,
        /* [in] */ const String& intf);

    CARAPI_(Boolean) RunDhcp(
        /* [in] */ DhcpAction dhcpAction);

public:
    /* Commands from controller to start/stop DHCP */
    static const Int32 CMD_START_DHCP;
    static const Int32 CMD_STOP_DHCP;
    static const Int32 CMD_RENEW_DHCP;

    /* Notification from DHCP state machine prior to DHCP discovery/renewal */
    static const Int32 CMD_PRE_DHCP_ACTION;
    /* Notification from DHCP state machine post DHCP discovery/renewal. Indicates
     * success/failure */
    static const Int32 CMD_POST_DHCP_ACTION;
    /* Notification from DHCP state machine before quitting */
    static const Int32 CMD_ON_QUIT;

    /* Command from controller to indicate DHCP discovery/renewal can continue
     * after pre DHCP action is complete */
    static const Int32 CMD_PRE_DHCP_ACTION_COMPLETE;

    /* Message.arg1 arguments to CMD_POST_DHCP notification */
    static const Int32 DHCP_SUCCESS;
    static const Int32 DHCP_FAILURE;

private:
    static const Int32 BASE;

    static const String TAG;
    static const Boolean DBG;

    /* A StateMachine that controls the DhcpStateMachine */
    AutoPtr<IStateMachine> mController;

    AutoPtr<IContext> mContext;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mDhcpRenewalIntent;
    AutoPtr<IPowerManagerWakeLock> mDhcpRenewWakeLock;
    static const String WAKELOCK_TAG;

    //Remember DHCP configuration from first request
    AutoPtr<IDhcpResults> mDhcpResults;

    static const Int32 DHCP_RENEW;
    static const String ACTION_DHCP_RENEW;

    //Used for sanity check on setting up renewal
    static const Int32 MIN_RENEWAL_TIME_SECS;  // 5 minutes

    /* const */ String mInterfaceName;
    Boolean mRegisteredForPreDhcpNotification;

    AutoPtr<IState> mDefaultState;
    AutoPtr<IState> mStoppedState;
    AutoPtr<IState> mWaitBeforeStartState;
    AutoPtr<IState> mRunningState;
    AutoPtr<IState> mWaitBeforeRenewalState;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCPSTATEMACHINE_H__
