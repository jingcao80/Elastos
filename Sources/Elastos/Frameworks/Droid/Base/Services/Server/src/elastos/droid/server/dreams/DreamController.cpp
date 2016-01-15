
#include "dreams/DreamController.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

const String DreamController::TAG("DreamController");
const Int32 DreamController::DREAM_CONNECTION_TIMEOUT;

CAR_INTERFACE_IMPL(DreamController::_Runnable, IRunnable)

ECode DreamController::ProxyDiedRunnable::Run()
{
    mDRHost->mService = NULL;
    if (mDRHost->mHost->mCurrentDream.Get() == mDRHost) {
        mDRHost->mHost->StopDream();
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
        mDRHost->mHost->StopDream();
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
        mDCHost->StopDream();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DreamController::DreamRecord, IServiceConnection)

DreamController::DreamRecord::DreamRecord(
    /* [in] */ IBinder* token,
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean isTest,
    /* [in] */ Int32 userId,
    /* [in] */ DreamController* host)
    : mToken(token)
    , mName(name)
    , mIsTest(isTest)
    , mUserId(userId)
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
    AutoPtr<IWindowManagerGlobal> windowMangerGlobal;
    CWindowManagerGlobal::AcquireSingleton((IWindowManagerGlobal**)&windowMangerGlobal);
    windowMangerGlobal->GetWindowManagerService((IIWindowManager**)&mIWindowManager);

    CIntent::New(IIntent::ACTION_DREAMING_STARTED, (IIntent**)&mDreamingStartedIntent);
    mDreamingStartedIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    CIntent::New(IIntent::ACTION_DREAMING_STOPPED, (IIntent**)&mDreamingStoppedIntent);
    mDreamingStoppedIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    CIntent::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntent**)&mCloseNotificationShadeIntent);
    mStopUnconnectedDreamRunnable = new StopUnconnectedDreamRunnable(this);
}

ECode DreamController::Dump(
    /* [in] */ IPrintWriter* pw)
{
    pw->PrintStringln(String("Dreamland:"));
    if (mCurrentDream != NULL) {
        pw->PrintStringln(String("  mCurrentDream:"));
        String tokens;
        mCurrentDream->mToken->ToString(&tokens);
        pw->PrintStringln(String("    mToken=") + tokens);
        String names;
        mCurrentDream->mName->ToString(&names);
        pw->PrintStringln(String("    mName=") + names);
        StringBuilder sb1("    mIsTest=");
        sb1 += mCurrentDream->mIsTest;
        pw->PrintStringln(sb1.ToString());
        StringBuilder sb2("    mUserId=");
        sb2 += mCurrentDream->mUserId;
        pw->PrintStringln(sb2.ToString());
        StringBuilder sb3("    mBound=");
        sb3 += mCurrentDream->mBound;
        pw->PrintStringln(sb3.ToString());
        StringBuilder sb4("    mService=");
        sb4 += mCurrentDream->mService;
        pw->PrintStringln(sb4.ToString());
        StringBuilder sb5("    mSentStartBroadcast=");
        sb5 += mCurrentDream->mSentStartBroadcast;
        pw->PrintStringln(sb5.ToString());
    }
    else {
        pw->PrintStringln(String("  mCurrentDream: NULL"));
    }
    return NOERROR;
}

ECode DreamController::StartDream(
    /* [in] */ IBinder* token,
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean isTest,
    /* [in] */ Int32 userId)
{
    StopDream();

    // Close the notification shade. Don't need to send to all, but better to be explicit.
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> userHandleAll;
    uhHelper->GetALL((IUserHandle**)&userHandleAll);
    mContext->SendBroadcastAsUser(mCloseNotificationShadeIntent, userHandleAll);

    Slogger::I(TAG, "Starting dream: name=%p, isTest=%d, userId=%d", name, isTest, userId);

    mCurrentDream = new DreamRecord(token, name, isTest, userId, this);

    // try {
    if (FAILED(mIWindowManager->AddWindowToken(token, IWindowManagerLayoutParams::TYPE_DREAM))) {
    // } catch (RemoteException ex) {
        // Slog.e(TAG, "Unable to add window token for dream.", ex);
        Slogger::E(TAG, "Unable to add window token for dream.");
        StopDream();
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IDreamService::SERVICE_INTERFACE,(IIntent**)&intent);

    intent->SetComponent(name);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    // try {
    Boolean bindService;
    FAIL_RETURN(mContext->BindService(intent, mCurrentDream,
                IContext::BIND_AUTO_CREATE, userId, &bindService));
    if (!bindService) {
        Slogger::E(TAG, "Unable to bind dream service: %p", intent.Get());
        StopDream();
        return NOERROR;
    }
    // } catch (SecurityException ex) {
        // Slogger::E(TAG, "Unable to bind dream service: " + intent, ex);
        // StopDream();
        // return;
    // }

    mCurrentDream->mBound = TRUE;
    Boolean result;
    mHandler->PostDelayed(mStopUnconnectedDreamRunnable, DREAM_CONNECTION_TIMEOUT, &result);
    return NOERROR;
}

ECode DreamController::StopDream()
{
    if (mCurrentDream == NULL) {
        return NOERROR;
    }

    AutoPtr<DreamRecord> oldDream = mCurrentDream;
    mCurrentDream = NULL;
    Slogger::I(TAG, "Stopping dream: name=%p, isTest=%d, userId=%d", oldDream->mName.Get(), oldDream->mIsTest
            , oldDream->mUserId);

    mHandler->RemoveCallbacks(mStopUnconnectedDreamRunnable);

    if (oldDream->mSentStartBroadcast) {
       AutoPtr<IUserHandleHelper> uhHelper;
       CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
       AutoPtr<IUserHandle> userHandleAll;
       uhHelper->GetALL((IUserHandle**)&userHandleAll);
       mContext->SendBroadcastAsUser(mDreamingStoppedIntent, userHandleAll);
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
        mIWindowManager->RemoveWindowToken(oldDream->mToken);
    // } catch (RemoteException ex) {
        // Slog.w(TAG, "Error removing window token for dream.", ex);
    // }

    AutoPtr<StopDreamRunnable> runnable = new StopDreamRunnable(this, oldDream);
    Boolean result;
    mHandler->Post(runnable, &result);
    return NOERROR;
}

void DreamController::Attach(
    /* [in] */ IIDreamService* service)
{
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)service->Probe(EIID_IProxy);
    proxy->LinkToDeath(mCurrentDream, 0);
    ECode ec = service->Attach(mCurrentDream->mToken);
    if (ec != NOERROR) {
    // } catch (RemoteException ex) {
        // Slog.e(TAG, "The dream service died unexpectedly.", ex);
        StopDream();
        return;
    }

    mCurrentDream->mService = service;

    if (!mCurrentDream->mIsTest) {
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        AutoPtr<IUserHandle> userHandleAll;
        uhHelper->GetALL((IUserHandle**)&userHandleAll);
        mContext->SendBroadcastAsUser(mDreamingStartedIntent, userHandleAll);
        mCurrentDream->mSentStartBroadcast = TRUE;
    }
}

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos
