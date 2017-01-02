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

#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.App.h>
#include "elastos/droid/server/power/Notifier.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::EIID_IActivityManagerInternal;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Hardware::Input::EIID_IInputManagerInternal;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

const String Notifier::TAG("Notifier");
const Boolean Notifier::DEBUG = FALSE;
const Int32 Notifier::POWER_STATE_UNKNOWN = 0;
const Int32 Notifier::POWER_STATE_AWAKE = 1;
const Int32 Notifier::POWER_STATE_ASLEEP = 2;

const Int32 Notifier::MSG_USER_ACTIVITY = 1;
const Int32 Notifier::MSG_BROADCAST = 2;

//==============================================================================
//          Notifier::WakeUpBroadcastDone
//==============================================================================

Notifier::WakeUpBroadcastDone::WakeUpBroadcastDone(
    /* [in] */ Notifier* host)
    : mHost(host)
{}

ECode Notifier::WakeUpBroadcastDone::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // EventLog.writeEvent(EventLogTags.POWER_SCREEN_BROADCAST_DONE, 1,
    //         SystemClock.uptimeMillis() - mBroadcastStartTime, 1);
    mHost->SendNextBroadcast();
    return NOERROR;
}

//==============================================================================
//          Notifier::GoToSleepBroadcastDone
//==============================================================================

Notifier::GoToSleepBroadcastDone::GoToSleepBroadcastDone(
    /* [in] */ Notifier* host)
    : mHost(host)
{}

ECode Notifier::GoToSleepBroadcastDone::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // EventLog.writeEvent(EventLogTags.POWER_SCREEN_BROADCAST_DONE, 0,
    //         SystemClock.uptimeMillis() - mBroadcastStartTime, 1);
    mHost->SendNextBroadcast();
    return NOERROR;
}

//==============================================================================
//          Notifier::NotifierHandler
//==============================================================================

ECode Notifier::NotifierHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case Notifier::MSG_USER_ACTIVITY:
            mHost->SendUserActivity();
            break;

        case Notifier::MSG_BROADCAST:
            mHost->SendNextBroadcast();
            break;
    }

    return NOERROR;
}

//==============================================================================
//          Notifier::StartedRunnable
//==============================================================================

Notifier::StartedRunnable::StartedRunnable(
    /* [in] */ Notifier* host)
    : mHost(host)
{}

ECode Notifier::StartedRunnable::Run()
{
    // EventLog.writeEvent(IEventLogTags::POWER_SCREEN_STATE, 1, 0, 0, 0);
    mHost->mPolicy->WakingUp();
    mHost->mActivityManagerInternal->WakingUp();
    return NOERROR;
}

//==============================================================================
//          Notifier::FinishedRunnable
//==============================================================================

Notifier::FinishedRunnable::FinishedRunnable(
    /* [in] */ Notifier* host)
    : mHost(host)
{}

ECode Notifier::FinishedRunnable::Run()
{
    Int32 why = IWindowManagerPolicy::OFF_BECAUSE_OF_USER;
    switch (mHost->mLastGoToSleepReason) {
        case IPowerManager::GO_TO_SLEEP_REASON_DEVICE_ADMIN:
            why = IWindowManagerPolicy::OFF_BECAUSE_OF_ADMIN;
            break;
        case IPowerManager::GO_TO_SLEEP_REASON_TIMEOUT:
            why = IWindowManagerPolicy::OFF_BECAUSE_OF_TIMEOUT;
            break;
    }
    // EventLog.writeEvent(EventLogTags.POWER_SCREEN_STATE, 0, why, 0, 0);
    mHost->mPolicy->GoingToSleep(why);
    mHost->mActivityManagerInternal->GoingToSleep();
    return NOERROR;
}

//==============================================================================
//          Notifier
//==============================================================================

Notifier::Notifier(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* context,
    /* [in] */ IIBatteryStats* batteryStats,
    /* [in] */ IIAppOpsService* appOps,
    /* [in] */ ISuspendBlocker* suspendBlocker,
    /* [in] */ IWindowManagerPolicy* policy)
    : mContext(context)
    , mBatteryStats(batteryStats)
    , mAppOps(appOps)
    , mSuspendBlocker(suspendBlocker)
    , mPolicy(policy)
    , mActualPowerState(0)
    , mLastGoToSleepReason(0)
    , mPendingWakeUpBroadcast(FALSE)
    , mPendingGoToSleepBroadcast(FALSE)
    , mBroadcastedPowerState(0)
    , mBroadcastInProgress(FALSE)
    , mBroadcastStartTime(0)
    , mUserActivityPending(FALSE)
{
    AutoPtr<IInterface> obj = LocalServices::GetService(EIID_IActivityManagerInternal);
    mActivityManagerInternal = IActivityManagerInternal::Probe(obj);
    obj = LocalServices::GetService(EIID_IInputManagerInternal);
    mInputManagerInternal = IInputManagerInternal::Probe(obj);

    mWakeUpBroadcastDone = (IBroadcastReceiver*)new WakeUpBroadcastDone(this);
    mGoToSleepBroadcastDone = (IBroadcastReceiver*)new GoToSleepBroadcastDone(this);

    mHandler = new NotifierHandler(looper, this);
    ASSERT_SUCCEEDED(CIntent::New(IIntent::ACTION_SCREEN_ON, (IIntent**)&mScreenOnIntent));
    mScreenOnIntent->AddFlags(
            IIntent::FLAG_RECEIVER_REGISTERED_ONLY | IIntent::FLAG_RECEIVER_FOREGROUND);
    ASSERT_SUCCEEDED(CIntent::New(IIntent::ACTION_SCREEN_OFF, (IIntent**)&mScreenOffIntent));
    mScreenOffIntent->AddFlags(
            IIntent::FLAG_RECEIVER_REGISTERED_ONLY | IIntent::FLAG_RECEIVER_FOREGROUND);

    // Initialize interactive state for battery stats.
    // try {
    mBatteryStats->NoteInteractive(TRUE);
    // } catch (RemoteException ex) { }
}

void Notifier::OnWakeLockAcquired(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ const String& historyTag)
{
    if (DEBUG) {
        Slogger::D(TAG, "onWakeLockAcquired: flags=%d, tag=%s, packageName=%s, ownerUid=%d, ownerPid=%d, workSource=%p",
                flags, tag.string(), packageName.string(), ownerUid, ownerPid, workSource);
    }

    // try {
    Int32 monitorType = GetBatteryStatsWakeLockMonitorType(flags);
    Boolean unimportantForLogging = (flags & IPowerManager::UNIMPORTANT_FOR_LOGGING) != 0
            && ownerUid == IProcess::SYSTEM_UID;
    if (workSource != NULL) {
        mBatteryStats->NoteStartWakelockFromSource(workSource, ownerPid,
                tag, historyTag, monitorType, unimportantForLogging);
    }
    else {
        mBatteryStats->NoteStartWakelock(ownerUid, ownerPid, tag,
                historyTag, monitorType, unimportantForLogging);
        // XXX need to deal with disabled operations.
        AutoPtr<IBinder> binder;
        AutoPtr<IAppOpsManagerHelper> aomHelper;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aomHelper);
        aomHelper->GetToken(mAppOps, (IBinder**)&binder);
        Int32 result;
        mAppOps->StartOperation(binder, IAppOpsManager::OP_WAKE_LOCK,
                ownerUid, packageName, &result);
    }
    // } catch (RemoteException ex) {
    //     // Ignore
    // }
}

void Notifier::OnWakeLockChanging(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ const String& historyTag,
    /* [in] */ Int32 newFlags,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPackageName,
    /* [in] */ Int32 newOwnerUid,
    /* [in] */ Int32 newOwnerPid,
    /* [in] */ IWorkSource* newWorkSource,
    /* [in] */ const String& newHistoryTag)
{
    if (workSource != NULL && newWorkSource != NULL) {
        const Int32 monitorType = GetBatteryStatsWakeLockMonitorType(flags);
        const Int32 newMonitorType = GetBatteryStatsWakeLockMonitorType(newFlags);
        Boolean unimportantForLogging = (newFlags & IPowerManager::UNIMPORTANT_FOR_LOGGING) != 0
                && newOwnerUid == IProcess::SYSTEM_UID;
        if (DEBUG) {
            Slogger::D(TAG, "onWakeLockChanging: flags=%d, tag=%s, packageName=%s, ownerUid=%d, ownerPid=%d, workSource=%p",
                    newFlags, newTag.string(), newPackageName.string(), newOwnerUid, newOwnerPid, newWorkSource);
        }
        // try {
        mBatteryStats->NoteChangeWakelockFromSource(workSource, ownerPid, tag, historyTag,
                monitorType, newWorkSource, newOwnerPid, newTag, newHistoryTag,
                newMonitorType, unimportantForLogging);
        // } catch (RemoteException ex) {
        //     // Ignore
        // }
    }
    else {
        OnWakeLockReleased(flags, tag, packageName, ownerUid, ownerPid, workSource, historyTag);
        OnWakeLockAcquired(newFlags, newTag, newPackageName, newOwnerUid, newOwnerPid,
                newWorkSource, newHistoryTag);
    }
}

void Notifier::OnWakeLockReleased(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ const String& historyTag)
{
    if (DEBUG) {
        Slogger::D(TAG, "onWakeLockReleased: flags=%d, tag=%s, packageName=%s, ownerUid=%d, ownerPid=%d, workSource=%p",
                flags, tag.string(), packageName.string(), ownerUid, ownerPid, workSource);
    }

    // try {
    Int32 monitorType = GetBatteryStatsWakeLockMonitorType(flags);
    if (workSource != NULL) {
        mBatteryStats->NoteStopWakelockFromSource(workSource, ownerPid, tag, historyTag, monitorType);
    }
    else {
        mBatteryStats->NoteStopWakelock(ownerUid, ownerPid, tag, historyTag, monitorType);
        AutoPtr<IBinder> binder;
        AutoPtr<IAppOpsManagerHelper> aomHelper;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aomHelper);
        aomHelper->GetToken(mAppOps, (IBinder**)&binder);
        aomHelper->GetToken(mAppOps, (IBinder**)&binder);
        mAppOps->FinishOperation(binder,
                IAppOpsManager::OP_WAKE_LOCK, ownerUid, packageName);
    }
    // } catch (RemoteException ex) {
    //     // Ignore
    // }
}

Int32 Notifier::GetBatteryStatsWakeLockMonitorType(
    /* [in] */ Int32 flags)
{
    switch (flags & IPowerManager::WAKE_LOCK_LEVEL_MASK) {
        case IPowerManager::PARTIAL_WAKE_LOCK:
        case IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK:
            return IBatteryStats::WAKE_TYPE_PARTIAL;
        default:
            return IBatteryStats::WAKE_TYPE_FULL;
    }
}

void Notifier::OnInteractiveStateChangeStarted(
    /* [in] */ Boolean interactive,
    /* [in] */ Int32 reason)
{
    if (DEBUG) {
        Slogger::D(TAG, "onInteractiveChangeStarted: interactive=%d, reason=%d", interactive , reason);
    }

    {
        AutoLock syncLock(mLock);
        if (interactive) {
            // Waking up...
            if (mActualPowerState != POWER_STATE_AWAKE) {
                mActualPowerState = POWER_STATE_AWAKE;
                mPendingWakeUpBroadcast = TRUE;
                AutoPtr<StartedRunnable> runnable = new StartedRunnable(this);
                Boolean res;
                mHandler->Post(runnable, &res);
                UpdatePendingBroadcastLocked();
            }
        }
        else {
            // Going to sleep...
            mLastGoToSleepReason = reason;
        }
    }

    mInputManagerInternal->SetInteractive(interactive);

    if (interactive) {
        // try {
        mBatteryStats->NoteInteractive(TRUE);
        // } catch (RemoteException ex) { }
    }
}

void Notifier::OnInteractiveStateChangeFinished(
    /* [in] */ Boolean interactive)
{
    if (DEBUG) {
        Slogger::D(TAG, "onInteractiveChangeFinished");
    }

    {
        AutoLock syncLock(mLock);
        if (!interactive) {
            // Finished going to sleep...
            // This is a good time to make transitions that we don't want the user to see,
            // such as bringing the key guard to focus.  There's no guarantee for this,
            // however because the user could turn the device on again at any time.
            // Some things may need to be protected by other mechanisms that defer screen on.
            if (mActualPowerState != POWER_STATE_ASLEEP) {
                mActualPowerState = POWER_STATE_ASLEEP;
                mPendingGoToSleepBroadcast = TRUE;
                if (mUserActivityPending) {
                    mUserActivityPending = FALSE;
                    mHandler->RemoveMessages(MSG_USER_ACTIVITY);
                }
                AutoPtr<FinishedRunnable> runnable = new FinishedRunnable(this);
                Boolean res;
                mHandler->Post(runnable, &res);
                UpdatePendingBroadcastLocked();
            }
        }
    }

    if (!interactive) {
        // try {
        mBatteryStats->NoteInteractive(FALSE);
        // } catch (RemoteException ex) { }
    }
}

void Notifier::OnUserActivity(
    /* [in] */ Int32 event,
    /* [in] */ Int32 uid)
{
    if (DEBUG) {
        Slogger::D(TAG, "onUserActivity: event=%d, uid=%d", event, uid);
    }

    // try {
    mBatteryStats->NoteUserActivity(uid, event);
    // } catch (RemoteException ex) {
    //     // Ignore
    // }

    {    AutoLock syncLock(mLock);
        if (!mUserActivityPending) {
            mUserActivityPending = TRUE;
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MSG_USER_ACTIVITY, (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            Boolean result;
            mHandler->SendMessage(msg, &result);
        }
    }
}

void Notifier::OnWirelessChargingStarted()
{
    if (DEBUG) {
        Slogger::D(TAG, "onWirelessChargingStarted");
    }
}

void Notifier::UpdatePendingBroadcastLocked()
{
    if (!mBroadcastInProgress
            && mActualPowerState != POWER_STATE_UNKNOWN
            && (mPendingWakeUpBroadcast || mPendingGoToSleepBroadcast
                    || mActualPowerState != mBroadcastedPowerState)) {
        mBroadcastInProgress = TRUE;
        mSuspendBlocker->AcquireBlocker();
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_BROADCAST, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
}

void Notifier::FinishPendingBroadcastLocked()
{
    mBroadcastInProgress = FALSE;
    mSuspendBlocker->ReleaseBlocker();
}

void Notifier::SendUserActivity()
{
    {
        AutoLock syncLock(mLock);
        if (!mUserActivityPending) {
            return;
        }
        mUserActivityPending = FALSE;
    }

    mPolicy->UserActivity();
}

void Notifier::SendNextBroadcast()
{
    Int32 powerState;
    {    AutoLock syncLock(mLock);
        if (mBroadcastedPowerState == POWER_STATE_UNKNOWN) {
            // Broadcasted power state is unknown.  Send wake up.
            mPendingWakeUpBroadcast = FALSE;
            mBroadcastedPowerState = POWER_STATE_AWAKE;
        }
        else if (mBroadcastedPowerState == POWER_STATE_AWAKE) {
            // Broadcasted power state is awake.  Send asleep if needed.
            if (mPendingWakeUpBroadcast || mPendingGoToSleepBroadcast
                    || mActualPowerState == POWER_STATE_ASLEEP) {
                mPendingGoToSleepBroadcast = FALSE;
                mBroadcastedPowerState = POWER_STATE_ASLEEP;
            }
            else {
                FinishPendingBroadcastLocked();
                return;
            }
        }
        else {
            // Broadcasted power state is asleep.  Send awake if needed.
            if (mPendingWakeUpBroadcast || mPendingGoToSleepBroadcast
                    || mActualPowerState == POWER_STATE_AWAKE) {
                mPendingWakeUpBroadcast = FALSE;
                mBroadcastedPowerState = POWER_STATE_AWAKE;
            }
            else {
                FinishPendingBroadcastLocked();
                return;
            }
        }

        mBroadcastStartTime = SystemClock::GetUptimeMillis();
        powerState = mBroadcastedPowerState;
    }

    // EventLog.writeEvent(EventLogTags.POWER_SCREEN_BROADCAST_SEND, 1);

    if (powerState == POWER_STATE_AWAKE) {
        SendWakeUpBroadcast();
    }
    else {
        SendGoToSleepBroadcast();
    }
}

void Notifier::SendWakeUpBroadcast()
{
    if (DEBUG) {
        Slogger::D(TAG, "Sending wake up broadcast.");
    }

    // if (ActivityManagerNative::IsSystemReady()) {
    AutoPtr<IUserHandleHelper> handleHelper;
    ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
    AutoPtr<IUserHandle> all;
    handleHelper->GetALL((IUserHandle**)&all);
    mContext->SendOrderedBroadcastAsUser(mScreenOnIntent, all, String(NULL),
            mWakeUpBroadcastDone, mHandler, 0, String(NULL), NULL);
    // }
    // else {
    //     EventLog::WriteEvent(IEventLogTags::POWER_SCREEN_BROADCAST_STOP, 2, 1);
    //     SendNextBroadcast();
    // }
}

void Notifier::SendGoToSleepBroadcast()
{
    if (DEBUG) {
        Slogger::D(TAG, "Sending go to sleep broadcast.");
    }

    // if (ActivityManagerNative::IsSystemReady()) {
    AutoPtr<IUserHandleHelper> handleHelper;
    ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
    AutoPtr<IUserHandle> all;
    handleHelper->GetALL((IUserHandle**)&all);
    mContext->SendOrderedBroadcastAsUser(mScreenOffIntent, all, String(NULL),
            mGoToSleepBroadcastDone, mHandler, 0, String(NULL), NULL);
    // }
    // else {
    //     EventLog::WriteEvent(IEventLogTags::POWER_SCREEN_BROADCAST_STOP, 3, 1);
    //     SendNextBroadcast();
    // }
}

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos
