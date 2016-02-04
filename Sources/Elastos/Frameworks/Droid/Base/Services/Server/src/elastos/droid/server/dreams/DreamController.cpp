
#include "elastos/droid/server/dreams/DreamController.h"
#include <elastos/droid/os/UserHandle.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Service.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Content::EIID_IServiceConnection;

using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

const String DreamController::TAG("DreamController");
const Int32 DreamController::DREAM_CONNECTION_TIMEOUT;
const Int32 DreamController::DREAM_FINISH_TIMEOUT;

ECode DreamController::ProxyDiedRunnable::Run()
{
    mDRHost->mService = NULL;
    if (mDRHost->mHost->mCurrentDream.Get() == mDRHost) {
        mDRHost->mHost->StopDream(TRUE /*immediate*/);
    }
    return NOERROR;
}

ECode DreamController::ConnectedRunnable::Run()
{
    mDRHost->mConnected = TRUE;
    if (mDRHost->mHost->mCurrentDream.Get() == mDRHost && mDRHost->mService == NULL) {
        mDRHost->mHost->Attach(IIDreamService::Probe(mService));
    }
    return NOERROR;
}

ECode DreamController::DisconnectedRunnable::Run()
{
    mDRHost->mService = NULL;
    if (mDRHost->mHost->mCurrentDream.Get() == mDRHost) {
        mDRHost->mHost->StopDream(TRUE /*immediate*/);
    }
    return NOERROR;
}

ECode DreamController::StopDreamRunnable::Run()
{
    mDCHost->mListener->OnDreamStopped(mOldDream->mToken);
    return NOERROR;
}

ECode DreamController::StopUnconnectedDreamRunnable::Run()
{
    if (mDCHost->mCurrentDream != NULL && mDCHost->mCurrentDream->mBound && !mDCHost->mCurrentDream->mConnected) {
        Slogger::W(TAG, "Bound dream did not connect in the time allotted");
        mDCHost->StopDream(TRUE /*immediate*/);
    }
    return NOERROR;
}

ECode DreamController::StopStubbornDreamRunnable::Run()
{
    Slogger::W(TAG, "Stubborn dream did not finish itself in the time allotted");
    mDCHost->StopDream(TRUE /*immediate*/);
    return NOERROR;
}

CAR_INTERFACE_IMPL(DreamController::DreamRecord, Object, IServiceConnection)

DreamController::DreamRecord::DreamRecord(
    /* [in] */ IBinder* token,
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean isTest,
    /* [in] */ Boolean canDoze,
    /* [in] */ Int32 userId,
    /* [in] */ DreamController* host)
    : mToken(token)
    , mName(name)
    , mIsTest(isTest)
    , mCanDoze(canDoze)
    , mUserId(userId)
    , mWakingGently(FALSE)
    , mHost(host)
{}

ECode DreamController::DreamRecord::ProxyDied()
{
    AutoPtr<ProxyDiedRunnable> runnable = new ProxyDiedRunnable(this);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
    return NOERROR;
}

ECode DreamController::DreamRecord::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<ConnectedRunnable> runnable = new ConnectedRunnable(this, service);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
    return NOERROR;
}

ECode DreamController::DreamRecord::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoPtr<DisconnectedRunnable> runnable = new DisconnectedRunnable(this);
    Boolean result;
    mHost->mHandler->Post(runnable, &result);
    return NOERROR;
}

DreamController::DreamController(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ Listener* listener)
    : mContext(context)
    , mHandler(handler)
    , mListener(listener)
{
    AutoPtr<IWindowManagerGlobalHelper> windowMangerGlobal;
    CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&windowMangerGlobal);
    windowMangerGlobal->GetWindowManagerService((IIWindowManager**)&mIWindowManager);

    CIntent::New(IIntent::ACTION_DREAMING_STARTED, (IIntent**)&mDreamingStartedIntent);
    mDreamingStartedIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    CIntent::New(IIntent::ACTION_DREAMING_STOPPED, (IIntent**)&mDreamingStoppedIntent);
    mDreamingStoppedIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    CIntent::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntent**)&mCloseNotificationShadeIntent);
    mStopUnconnectedDreamRunnable = new StopUnconnectedDreamRunnable(this);
    mStopStubbornDreamRunnable = new StopStubbornDreamRunnable(this);
}

ECode DreamController::Dump(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("Dreamland:"));
    if (mCurrentDream != NULL) {
        pw->Println(String("  mCurrentDream:"));
        String tokens;
        mCurrentDream->mToken->ToString(&tokens);
        pw->Println(String("    mToken=") + tokens);
        String names;
        mCurrentDream->mName->ToString(&names);
        pw->Println(String("    mName=") + names);
        StringBuilder sb1("    mIsTest=");
        sb1 += mCurrentDream->mIsTest;
        pw->Println(sb1.ToString());
        StringBuilder sb6("    mCanDoze=");
        sb6 += mCurrentDream->mCanDoze;
        pw->Println(sb6.ToString());
        StringBuilder sb2("    mUserId=");
        sb2 += mCurrentDream->mUserId;
        pw->Println(sb2.ToString());
        StringBuilder sb3("    mBound=");
        sb3 += mCurrentDream->mBound;
        pw->Println(sb3.ToString());
        StringBuilder sb4("    mService=");
        sb4 += mCurrentDream->mService;
        pw->Println(sb4.ToString());
        StringBuilder sb5("    mSentStartBroadcast=");
        sb5 += mCurrentDream->mSentStartBroadcast;
        pw->Println(sb5.ToString());
        StringBuilder sb7("    mWakingGently=");
        sb7 += mCurrentDream->mWakingGently;
        pw->Println(sb7.ToString());
    }
    else {
        pw->Println(String("  mCurrentDream: NULL"));
    }
    return NOERROR;
}

ECode DreamController::StartDream(
    /* [in] */ IBinder* token,
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean isTest,
    /* [in] */ Boolean canDoze,
    /* [in] */ Int32 userId)
{
    StopDream(TRUE /*immediate*/);

    // Close the notification shade. Don't need to send to all, but better to be explicit.
    mContext->SendBroadcastAsUser(mCloseNotificationShadeIntent, UserHandle::ALL);

    Slogger::I(TAG, "Starting dream: name=%p, isTest=%d, canDoze=%d, userId=%d",
        name, isTest, canDoze, userId);

    mCurrentDream = new DreamRecord(token, name, isTest, canDoze, userId, this);

    // try {
    if (FAILED(mIWindowManager->AddWindowToken(token, IWindowManagerLayoutParams::TYPE_DREAM))) {
    // } catch (RemoteException ex) {
        // Slog.e(TAG, "Unable to add window token for dream.", ex);
        Slogger::E(TAG, "Unable to add window token for dream.");
        StopDream(TRUE /*immediate*/);
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IDreamService::SERVICE_INTERFACE,(IIntent**)&intent);

    intent->SetComponent(name);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    // try {
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(userId, (IUserHandle**)&uh);
    Boolean result;
    ECode ec = mContext->BindServiceAsUser(intent, mCurrentDream,
        IContext::BIND_AUTO_CREATE, uh, &result);
    if (!result || (ec == (ECode)E_SECURITY_EXCEPTION)) {
        Slogger::E(TAG, "Unable to bind dream service: %s", TO_CSTR(intent));
        StopDream(TRUE /*immediate*/);
        return NOERROR;
    }

    mCurrentDream->mBound = TRUE;
    mHandler->PostDelayed(mStopUnconnectedDreamRunnable, DREAM_CONNECTION_TIMEOUT, &result);
    return NOERROR;
}

ECode DreamController::StopDream(
    /* [in] */ Boolean immediate)
{
    if (mCurrentDream == NULL) {
        return NOERROR;
    }

    Boolean bval;
    if (!immediate) {
        if (mCurrentDream->mWakingGently) {
            return NOERROR; // already waking gently
        }

        if (mCurrentDream->mService != NULL) {
            // Give the dream a moment to wake up and finish itself gently.
            mCurrentDream->mWakingGently = TRUE;
            // try {
            mCurrentDream->mService->WakeUp();
            mHandler->PostDelayed(mStopStubbornDreamRunnable, DREAM_FINISH_TIMEOUT, &bval);
            return NOERROR;
            // } catch (RemoteException ex) {
            //     // oh well, we tried, finish immediately instead
            // }
        }
    }

    AutoPtr<DreamRecord> oldDream = mCurrentDream;
    mCurrentDream = NULL;
    Slogger::I(TAG, "Stopping dream: name=%p, isTest=%d, canDoze=%d, userId=%d",
        oldDream->mName.Get(), oldDream->mIsTest, oldDream->mCanDoze, oldDream->mUserId);

    mHandler->RemoveCallbacks(mStopUnconnectedDreamRunnable);
    mHandler->RemoveCallbacks(mStopStubbornDreamRunnable);

    if (oldDream->mSentStartBroadcast) {
       mContext->SendBroadcastAsUser(mDreamingStoppedIntent, UserHandle::ALL);
    }

    if (oldDream->mService != NULL) {
        // Tell the dream that it's being stopped so that
        // it can shut down nicely before we yank its window token out from
        // under it.
        // try {
        oldDream->mService->Detach();
        // } catch (RemoteException ex) {
            // we don't care; this thing is on the way out
        // }

        // try {
        AutoPtr<IProxy> proxy = (IProxy*)oldDream->mService->Probe(EIID_IProxy);
        Boolean result;
        proxy->UnlinkToDeath(oldDream, 0, &result);
        // } catch (NoSuchElementException ex) {
            // don't care
        // }
        oldDream->mService = NULL;
    }

    if (oldDream->mBound) {
        mContext->UnbindService(oldDream);
    }

    // try {
    ECode ec = mIWindowManager->RemoveWindowToken(oldDream->mToken);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::W(TAG, "Error removing window token for dream.");
    }

    AutoPtr<StopDreamRunnable> runnable = new StopDreamRunnable(this, oldDream);
    mHandler->Post(runnable, &bval);
    return NOERROR;
}

void DreamController::Attach(
    /* [in] */ IIDreamService* service)
{
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)service->Probe(EIID_IProxy);
    proxy->LinkToDeath(mCurrentDream, 0);
    ECode ec = service->Attach(mCurrentDream->mToken, mCurrentDream->mCanDoze);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
    // } catch (RemoteException ex) {
        // Slog.e(TAG, "The dream service died unexpectedly.", ex);
        StopDream(TRUE /*immediate*/);
        return;
    }

    mCurrentDream->mService = service;

    if (!mCurrentDream->mIsTest) {
        mContext->SendBroadcastAsUser(mDreamingStartedIntent, UserHandle::ALL);
        mCurrentDream->mSentStartBroadcast = TRUE;
    }
}

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos
