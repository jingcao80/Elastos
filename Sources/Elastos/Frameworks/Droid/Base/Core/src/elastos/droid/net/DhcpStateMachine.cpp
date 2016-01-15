
#include "elastos/droid/net/DhcpStateMachine.h"
#ifdef DROID_CORE
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#endif
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/CDhcpResults.h"
#include "elastos/droid/net/CDhcpStateMachine.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemClock;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

const String DhcpStateMachine::TAG("DhcpStateMachine");
const Boolean DhcpStateMachine::DBG = FALSE;
const String DhcpStateMachine::WAKELOCK_TAG("DHCP");
const Int32 DhcpStateMachine::DHCP_RENEW = 0;
const String DhcpStateMachine::ACTION_DHCP_RENEW("android.net.wifi.DHCP_RENEW");
const Int32 DhcpStateMachine::MIN_RENEWAL_TIME_SECS = 5 * 60;
const Int32 DhcpStateMachine::BASE = IProtocol::BASE_DHCP;
const Int32 DhcpStateMachine::CMD_START_DHCP = BASE + 1;
const Int32 DhcpStateMachine::CMD_STOP_DHCP = BASE + 2;
const Int32 DhcpStateMachine::CMD_RENEW_DHCP = BASE + 3;
const Int32 DhcpStateMachine::CMD_PRE_DHCP_ACTION = BASE + 4;
const Int32 DhcpStateMachine::CMD_POST_DHCP_ACTION = BASE + 5;
const Int32 DhcpStateMachine::CMD_ON_QUIT = BASE + 6;
const Int32 DhcpStateMachine::CMD_PRE_DHCP_ACTION_COMPLETE = BASE + 7;
const Int32 DhcpStateMachine::DHCP_SUCCESS = 1;
const Int32 DhcpStateMachine::DHCP_FAILURE = 2;

CAR_INTERFACE_IMPL(DhcpStateMachine, StateMachine, IDhcpStateMachine)

//==============================================================
// DhcpStateMachine::DefaultState
//==============================================================
ECode DhcpStateMachine::DefaultState::Exit()
{
    if (DBG) Logger::D(TAG, "DefaultState::Exit");
    mOwner->mContext->UnregisterReceiver(mOwner->mBroadcastReceiver);
    return NOERROR;
}

ECode DhcpStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    IObject::Probe(msg)->ToString(&s);
    if (DBG) Logger::D(TAG, "DefaultState::ProcessMessage %s\n", s.string());
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_RENEW_DHCP:
            Logger::E(TAG, "Error! Failed to handle a DHCP renewal on %s", mOwner->mInterfaceName.string());
            mOwner->mDhcpRenewWakeLock->ReleaseLock();
            break;
        default:
            {
                String s;
                IObject::Probe(msg)->ToString(&s);
                Logger::E(TAG, "Error! unhandled message  %s", s.string());
            }
            break;
    }
    return HANDLED;
}

String DhcpStateMachine::DefaultState::GetName()
{
    return String("DefaultState");
}

//==============================================================
// DhcpStateMachine::StoppedState
//==============================================================
ECode DhcpStateMachine::StoppedState::Enter()
{
    if (DBG) Logger::D(TAG, "StoppedState::Enter\n");
    return NOERROR;
}

ECode DhcpStateMachine::StoppedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Boolean retValue = HANDLED;
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_START_DHCP:
            if (DBG) Logger::D("DhcpStateMachine::StoppedState", "ProcessMessage CMD_START_DHCP");
            if (mOwner->mRegisteredForPreDhcpNotification) {
                /* Notify controller before starting DHCP */
                mOwner->mController->SendMessage(CMD_PRE_DHCP_ACTION);
                mOwner->TransitionTo(mOwner->mWaitBeforeStartState);
            }
            else {
                if (mOwner->RunDhcp(DhcpAction_START)) {
                    mOwner->TransitionTo(mOwner->mRunningState);
                }
            }
            break;
        case CMD_STOP_DHCP:
            //ignore
            break;
        default:
            retValue = NOT_HANDLED;
            break;
    }
    *result = retValue;
    return NOERROR;
}

String DhcpStateMachine::StoppedState::GetName()
{
    return String("StoppedState");
}

//==============================================================
// DhcpStateMachine::WaitBeforeStartState
//==============================================================
ECode DhcpStateMachine::WaitBeforeStartState::Enter()
{
    if (DBG) Logger::D(TAG, "WaitBeforeStartState::Enter\n");
    return NOERROR;
}

ECode DhcpStateMachine::WaitBeforeStartState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Boolean retValue = HANDLED;
    String s;
    IObject::Probe(msg)->ToString(&s);
    if (DBG) Logger::D(TAG, "WaitBeforeStartState::ProcessMessage %s\n", s.string());
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_PRE_DHCP_ACTION_COMPLETE:
            if (mOwner->RunDhcp(DhcpAction_START)) {
                mOwner->TransitionTo(mOwner->mRunningState);
            }
            else {
                mOwner->TransitionTo(mOwner->mStoppedState);
            }
            break;
        case CMD_STOP_DHCP:
            mOwner->TransitionTo(mOwner->mStoppedState);
            break;
        case CMD_START_DHCP:
            //ignore
            break;
        default:
            retValue = NOT_HANDLED;
            break;
    }
    *result = retValue;
    return NOERROR;
}

String DhcpStateMachine::WaitBeforeStartState::GetName()
{
    return String("WaitBeforeStartState");
}

//==============================================================
// DhcpStateMachine::RunningState
//==============================================================
ECode DhcpStateMachine::RunningState::Enter()
{
    if (DBG) Logger::D(TAG, "RunningState::Enter\n");
    return NOERROR;
}

ECode DhcpStateMachine::RunningState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Boolean retValue = HANDLED;
    String s;
    IObject::Probe(msg)->ToString(&s);
    if (DBG) Logger::D(TAG, "RunningState::ProcessMessage %s\n", s.string());
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_STOP_DHCP:
            mOwner->mAlarmManager->Cancel(mOwner->mDhcpRenewalIntent);
            Boolean dhcpStoped;
            NetworkUtils::StopDhcp(mOwner->mInterfaceName, &dhcpStoped);
            if (!dhcpStoped) {
                Logger::E(TAG, "Failed to stop Dhcp on %s", mOwner->mInterfaceName.string());
            }
            mOwner->TransitionTo(mOwner->mStoppedState);
            break;
        case CMD_RENEW_DHCP:
            if (mOwner->mRegisteredForPreDhcpNotification) {
                /* Notify controller before starting DHCP */
                mOwner->mController->SendMessage(CMD_PRE_DHCP_ACTION);
                mOwner->TransitionTo(mOwner->mWaitBeforeRenewalState);
                //mDhcpRenewWakeLock is released in WaitBeforeRenewalState
            }
            else {
                if (!mOwner->RunDhcp(DhcpAction_RENEW)) {
                    mOwner->TransitionTo(mOwner->mStoppedState);
                }
                mOwner->mDhcpRenewWakeLock->ReleaseLock();
            }
            break;
        case CMD_START_DHCP:
            //ignore
            break;
        default:
            retValue = NOT_HANDLED;
    }
    *result = retValue;
    return NOERROR;
}

String DhcpStateMachine::RunningState::GetName()
{
    return String("DhcpStateMachine::RunningState");
}

//==============================================================
// DhcpStateMachine::WaitBeforeRenewalState
//==============================================================
ECode DhcpStateMachine::WaitBeforeRenewalState::Enter()
{
    if (DBG) Logger::D(TAG, "WaitBeforeRenewalState::Enter\n");
    return NOERROR;
}

ECode DhcpStateMachine::WaitBeforeRenewalState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Boolean retValue = HANDLED;
    String s;
    IObject::Probe(msg)->ToString(&s);
    if (DBG) Logger::D(TAG, "WaitBeforeRenewalState::ProcessMessage %s\n", s.string());
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_STOP_DHCP:
            mOwner->mAlarmManager->Cancel(mOwner->mDhcpRenewalIntent);
            Boolean dhcpStoped;
            NetworkUtils::StopDhcp(mOwner->mInterfaceName, &dhcpStoped);
            if (!dhcpStoped) {
                Logger::E(TAG, "Failed to stop Dhcp on %s", mOwner->mInterfaceName.string());
            }
            mOwner->TransitionTo(mOwner->mStoppedState);
            break;
        case CMD_PRE_DHCP_ACTION_COMPLETE:
            if (mOwner->RunDhcp(mOwner->DhcpAction_RENEW)) {
               mOwner->TransitionTo(mOwner->mRunningState);
            }
            else {
               mOwner->TransitionTo(mOwner->mStoppedState);
            }
            break;
        case CMD_START_DHCP:
            //ignore
            break;
        default:
            retValue = NOT_HANDLED;
            break;
    }
    *result = retValue;
    return NOERROR;
}

ECode DhcpStateMachine::WaitBeforeRenewalState::Exit()
{
    if (DBG) Logger::D(TAG, "WaitBeforeRenewalState::Exit");
    mOwner->mDhcpRenewWakeLock->ReleaseLock();
    return NOERROR;
}

String DhcpStateMachine::WaitBeforeRenewalState::GetName()
{
    return String("DhcpStateMachine::WaitBeforeRenewalState");
}

//==============================================================
// DhcpStateMachine::MyBroadcastReceiver
//==============================================================
DhcpStateMachine::MyBroadcastReceiver::MyBroadcastReceiver(
   /* [in] */ DhcpStateMachine* owner)
   : mOwner(owner)
{}

//@Override
ECode DhcpStateMachine::MyBroadcastReceiver::OnReceive(
   /* [in] */ IContext* ctx,
   /* [in] */ IIntent* intent)
{
   //DHCP renew
    String s;
    ToString(&s);
    if (DBG) Logger::D(TAG, "Sending a DHCP renewal %s", s.string());
    //Lock released after 40s in worst case scenario
    mOwner->mDhcpRenewWakeLock->AcquireLock(40000);
    mOwner->SendMessage(CMD_RENEW_DHCP);
    return NOERROR;
}

//==============================================================
// DhcpStateMachine
//==============================================================
DhcpStateMachine::DhcpStateMachine()
    : StateMachine(TAG)
{}

ECode DhcpStateMachine::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IStateMachine* controller,
    /* [in] */ const String& intf)
{
    mController = controller;
    mContext = context;
    mInterfaceName = intf;
    mRegisteredForPreDhcpNotification = FALSE;

    mDefaultState = new DefaultState(this);
    mStoppedState = new StoppedState(this);
    mWaitBeforeStartState = new WaitBeforeStartState(this);
    mRunningState = new RunningState(this);
    mWaitBeforeRenewalState = new WaitBeforeRenewalState(this);

    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&mAlarmManager);
    AutoPtr<IIntent> dhcpRenewalIntent;
    CIntent::New(ACTION_DHCP_RENEW, NULL, (IIntent**)&dhcpRenewalIntent);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    helper->GetBroadcast(mContext, DHCP_RENEW, dhcpRenewalIntent, 0, (IPendingIntent**)&mDhcpRenewalIntent);

    AutoPtr<IPowerManager> powerManager;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&powerManager);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_TAG, (IPowerManagerWakeLock**)&mDhcpRenewWakeLock);
    mDhcpRenewWakeLock->SetReferenceCounted(FALSE);

    mBroadcastReceiver = new MyBroadcastReceiver(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(ACTION_DHCP_RENEW, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, intentFilter, (IIntent**)&intent);

    AddState((State*)mDefaultState.Get());
    AddState((State*)mStoppedState.Get(), (State*)mDefaultState.Get());
    AddState((State*)mWaitBeforeStartState.Get(), (State*)mDefaultState.Get());
    AddState((State*)mRunningState.Get(), (State*)mDefaultState.Get());
    AddState((State*)mWaitBeforeRenewalState.Get(), (State*)mDefaultState.Get());

    SetInitialState((State*)mStoppedState.Get());
    return NOERROR;
}

AutoPtr<IDhcpStateMachine> DhcpStateMachine::MakeDhcpStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ IStateMachine* controller,
    /* [in] */ const String& intf)
{
    AutoPtr<IDhcpStateMachine> dsm;
    CDhcpStateMachine* obj = new CDhcpStateMachine();
    obj->constructor(context, controller, intf);
    obj->Start();
    dsm = IDhcpStateMachine::Probe(obj);
    return dsm;
}

/**
* This sends a notification right before DHCP request/renewal so that the
* controller can do certain actions before DHCP packets are sent out.
* When the controller is ready, it sends a CMD_PRE_DHCP_ACTION_COMPLETE message
* to indicate DHCP can continue
*
* This is used by Wifi at this time for the purpose of doing BT-Wifi coex
* handling during Dhcp
*/
ECode DhcpStateMachine::RegisterForPreDhcpNotification()
{
    mRegisteredForPreDhcpNotification = TRUE;
    return NOERROR;
}

/**
* Quit the DhcpStateMachine.
*
* @hide
*/
ECode DhcpStateMachine::DoQuit()
{
    Quit();
    return NOERROR;
}

void DhcpStateMachine::OnQuitting()
{
    mController->SendMessage(CMD_ON_QUIT);
}

Boolean DhcpStateMachine::RunDhcp(
    /* [in] */ DhcpAction dhcpAction)
{
    Boolean success = FALSE;
    AutoPtr<IDhcpResults> dhcpResults;
    CDhcpResults::New((IDhcpResults**)&dhcpResults);

    if (dhcpAction == DhcpAction_START) {
        /* Stop any existing DHCP daemon before starting new */
        Boolean b;
        NetworkUtils::StopDhcp(mInterfaceName, &b);
        if (DBG) Logger::D(TAG, "DHCP request on %s", mInterfaceName.string());
        NetworkUtils::RunDhcp(mInterfaceName, dhcpResults, &success);
    }
    else if (dhcpAction == DhcpAction_RENEW) {
        if (DBG) Logger::D(TAG, "DHCP renewal on %s", mInterfaceName.string());
        NetworkUtils::RunDhcpRenew(mInterfaceName, dhcpResults, &success);
        if (success) dhcpResults->UpdateFromDhcpRequest(mDhcpResults);
    }

    if (success) {
        if (DBG) Logger::D(TAG, "DHCP succeeded on %s", mInterfaceName.string());
        Int64 leaseDuration;
        Int32 i32;
        dhcpResults->GetLeaseDuration(&i32); //int to long conversion
        leaseDuration = i32;

        //Sanity check for renewal
        if (leaseDuration >= 0) {
            //TODO: would be good to notify the user that his network configuration is
            //bad and that the device cannot renew below MIN_RENEWAL_TIME_SECS
            if (leaseDuration < MIN_RENEWAL_TIME_SECS) {
                leaseDuration = MIN_RENEWAL_TIME_SECS;
            }
            //Do it a bit earlier than half the lease duration time
            //to beat the native DHCP client and avoid extra packets
            //48% for one hour lease time = 29 minutes
            mAlarmManager->SetExact(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
                    SystemClock::GetElapsedRealtime() +
                    leaseDuration * 480, //in milliseconds
                    mDhcpRenewalIntent);
        }
        else {
            //infinite lease time, no renewal needed
        }

        mDhcpResults = dhcpResults;
        AutoPtr<IMessage> m;
        mController->ObtainMessage(CMD_POST_DHCP_ACTION, DHCP_SUCCESS, 0, dhcpResults, (IMessage**)&m);
        mController->SendMessage(m);
    }
    else {
        String s;
        NetworkUtils::GetDhcpError(&s);
        Logger::E(TAG, "DHCP failed on %s: ", mInterfaceName.string(), s.string());
        Boolean b;
        NetworkUtils::StopDhcp(mInterfaceName, &b);
        AutoPtr<IMessage> m;
        mController->ObtainMessage(CMD_POST_DHCP_ACTION, DHCP_FAILURE, 0, (IMessage**)&m);
        mController->SendMessage(m);
    }
    return success;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
