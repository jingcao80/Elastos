
#include "elastos/droid/systemui/keyguard/FaceUnlock.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode FaceUnlock::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* iservice)
{
    Logger::D(TAG, "Connected to Face Unlock service");
    mHost->mService = IIFaceLockInterface::Probe(iservice);
    return mHost->mHandler->SendEmptyMessage(MSG_SERVICE_CONNECTED);
}

ECode FaceUnlock::MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    Logger::E(TAG, "Unexpected disconnect from Face Unlock service");
    return mHost->mHandler->SendEmptyMessage(MSG_SERVICE_DISCONNECTED);
}

const Boolean FaceUnlock::DEBUG = KeyguardConstants::DEBUG;
const String FaceUnlock::TAG("FULLockscreen");
const String FaceUnlock::FACE_LOCK_PACKAGE("com.android.facelock");

CAR_INTERFACE_IMPL(FaceUnlock, BiometricSensorUnlock, IHandlerCallback)

FaceUnlock::FaceUnlock(
    /* [in] */ IContext* context)
    : mContext(context)
    , mServiceRunning(FALSE)
    , mBoundToService(FALSE)
    , MSG_SERVICE_CONNECTED(0)
    , MSG_SERVICE_DISCONNECTED(1)
    , MSG_UNLOCK(2)
    , MSG_CANCEL(3)
    , MSG_REPORT_FAILED_ATTEMPT(4)
    , MSG_POKE_WAKELOCK(5)
    , mIsRunning(FALSE)
{
    mConnection = new MyServiceConnection(this);

    CFaceUnlockFaceLockCallback::New((IIFaceLockCallback**)&mFaceUnlockCallback);

    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    CHandler::New(this, (IHandler**)&mHandler);
}

ECode FaceUnlock::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* keyguardScreenCallback)
{
    mKeyguardScreenCallback = keyguardScreenCallback;
    return NOERROR;
}

ECode FaceUnlock::InitializeView(
    /* [in] */ IView* biometricUnlockView)
{
    Logger::D(TAG, "initializeView()");
    mFaceUnlockView = biometricUnlockView;
    return NOERROR;
}

ECode FaceUnlock::IsRunning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsRunning;
    return NOERROR;
}

ECode FaceUnlock::StopAndShowBackup()
{
    if (DEBUG) Logger::D(TAG, "stopAndShowBackup()");
    return mHandler->SendEmptyMessage(MSG_CANCEL);
}

ECode FaceUnlock::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) Logger::D(TAG, "start()");

    AutoPtr<ILooper> lopper;
    mHandler->GetLooper((ILooper**)&lopper);
    AutoPtr<ILooper> mlopper;
    Looper::MyLooper((ILooper**)&mlopper)
    if (lopper != mlopper) {
        Logger::E(TAG, "start() called off of the UI thread");
    }

    if (mIsRunning) {
        Logger::W(TAG, "start() called when already running");
    }

    if (!mBoundToService) {
        Logger::D(TAG, "Binding to Face Unlock service for user="
                + mLockPatternUtils.getCurrentUser());
        assert(0);
        // mContext->BindServiceAsUser(
        //         new Intent(IFaceLockInterface.class.getName()).setPackage(FACE_LOCK_PACKAGE),
        //         mConnection,
        //         Context.BIND_AUTO_CREATE,
        //         new UserHandle(mLockPatternUtils.getCurrentUser()));
        mBoundToService = TRUE;
    }
    else {
        Logger::W(TAG, "Attempt to bind to Face Unlock when already bound");
    }

    mIsRunning = TRUE;
    *result = TRUE;
    return NOERROR;
}

ECode FaceUnlock::Stop(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) Logger::D(TAG, "stop()");

    AutoPtr<ILooper> lopper;
    mHandler->GetLooper((ILooper**)&lopper);
    AutoPtr<ILooper> mlopper;
    Looper::MyLooper((ILooper**)&mlopper)
    if (lopper != mlopper) {
        Logger::E(TAG, "stop() called from non-UI thread");
    }

    // Clearing any old service connected messages.
    mHandler->RemoveMessages(MSG_SERVICE_CONNECTED);

    Boolean mWasRunning = mIsRunning;

    StopUi();

    if (mBoundToService) {
        if (mService != NULL) {
            //try {
            mService->UnregisterCallback(mFaceUnlockCallback);
            //} catch (RemoteException e) {
                // Not much we can do
            //}
        }
        Logger::D(TAG, "Unbinding from Face Unlock service");
        mContext->UnbindService(mConnection);
        mBoundToService = FALSE;
    }
    else {
        // This is usually not an error when this happens.  Sometimes we will tell it to
        // unbind multiple times because it's called from both onWindowFocusChanged and
        // onDetachedFromWindow.
        if (DEBUG) Logger::D(TAG, "Attempt to unbind from Face Unlock when not bound");
    }
    mIsRunning = FALSE;
    *result = mWasRunning;
    return NOERROR;
}

ECode FaceUnlock::CleanUp()
{
    if (DEBUG) Logger::D(TAG, "cleanUp()");
    if (mService != NULL) {
        //try {
        mService->UnregisterCallback(mFaceUnlockCallback);
        //} catch (RemoteException e) {
            // Not much we can do
        //}
        StopUi();
        mService = NULL;
    }
    return NOERROR;
}

ECode FaceUnlock::GetQuality(
    /* [out] */ Int32* quality)
{
    VALIDATE_NOT_NULL(quality);

    *quality = IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK;
    return NOERROR;
}

ECode FaceUnlock::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SERVICE_CONNECTED:
            HandleServiceConnected();
            break;
        case MSG_SERVICE_DISCONNECTED:
            HandleServiceDisconnected();
            break;
        case MSG_UNLOCK:
            HandleUnlock(msg.arg1);
            break;
        case MSG_CANCEL:
            HandleCancel();
            break;
        case MSG_REPORT_FAILED_ATTEMPT:
            HandleReportFailedAttempt();
            break;
        case MSG_POKE_WAKELOCK:
            HandlePokeWakelock(msg.arg1);
            break;
        default:
            Logger::E(TAG, "Unhandled message");
            *result = FALSE;
            return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode FaceUnlock::HandleServiceConnected()
{
    Logger::D(TAG, "handleServiceConnected()");

    // It is possible that an unbind has occurred in the time between the bind and when this
    // function is reached.  If an unbind has already occurred, proceeding on to call startUi()
    // can result in a fatal error.  Note that the onServiceConnected() callback is
    // asynchronous, so this possibility would still exist if we executed this directly in
    // onServiceConnected() rather than using a handler.
    if (!mBoundToService) {
        Logger::D(TAG, "Dropping startUi() in handleServiceConnected() because no longer bound");
        return NOERROR;
    }

    //try {
    ECode ec = mService->RegisterCallback(mFaceUnlockCallback);
    //} catch (RemoteException e) {
    if (ec == (ECode)RemoteException) {
        Logger::E(TAG, "Caught exception connecting to Face Unlock: " + e.toString());
        mService = NULL;
        mBoundToService = FALSE;
        mIsRunning = FALSE;
        return NOERROR;
    }

    if (mFaceUnlockView != NULL) {
        AutoPtr<IBinder> windowToken;
        mFaceUnlockView->GetWindowToken((IBinder**)&windowToken);
        if (windowToken != NULL) {
            // When switching between portrait and landscape view while Face Unlock is running,
            // the screen will eventually go dark unless we poke the wakelock when Face Unlock
            // is restarted.
            mKeyguardScreenCallback->UserActivity();

            AutoPtr<ArrayOf<Int32> > position = ArrayOf<Int32>::Allco(2);
            mFaceUnlockView->GetLocationInWindow((ArrayOf<Int32>**)&position);
            Int32 width;
            mFaceUnlockView->GetWidth(&width);
            Int32 height;
            mFaceUnlockView->GetHeight(&height);
            StartUi(windowToken, (*position)[0], (*position)[1], width), height);
        }
        else {
            Logger::E(TAG, "windowToken is null in handleServiceConnected()");
        }
    }
    return NOERROR;
}

ECode FaceUnlock::HandleServiceDisconnected()
{
    Logger::E(TAG, "handleServiceDisconnected()");
    // TODO: this lock may no longer be needed now that everything is being called from a
    // handler
    {
        AutoLock lock(mServiceRunningLock);
        mService = NULL;
        mServiceRunning = FALSE;
    }
    mBoundToService = FALSE;
    mIsRunning = FALSE;
    return NOERROR;
}

ECode FaceUnlock::HandleUnlock(
    /* [in] */ Int32 authenticatedUserId)
{
    if (DEBUG) Logger::D(TAG, "handleUnlock()");
    Stop();
    Int32 currentUserId;
    mLockPatternUtils->GetCurrentUser(&currentUserId);
    if (authenticatedUserId == currentUserId) {
        if (DEBUG) Logger::D(TAG, "Unlocking for user %d", authenticatedUserId);
        mKeyguardScreenCallback->ReportUnlockAttempt(TRUE);
        mKeyguardScreenCallback->Dismiss(TRUE);
    }
    else {
        Logger::D(TAG, "Ignoring unlock for authenticated user (%d) because the current user is %d",
                authenticatedUserId, currentUserId);
    }
    return NOERROR;
}

ECode FaceUnlock::HandleCancel()
{
    if (DEBUG) Logger::D(TAG, "handleCancel()");
    // We are going to the backup method, so we don't want to see Face Unlock again until the
    // next time the user visits keyguard.
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->SetAlternateUnlockEnabled(FALSE);

    mKeyguardScreenCallback->ShowBackupSecurity();
    Stop();
    return mKeyguardScreenCallback->UserActivity();
}

ECode FaceUnlock::HandleReportFailedAttempt()
{
    if (DEBUG) Logger::D(TAG, "handleReportFailedAttempt()");
    // We are going to the backup method, so we don't want to see Face Unlock again until the
    // next time the user visits keyguard.
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->SetAlternateUnlockEnabled(FALSE);

    return mKeyguardScreenCallback->ReportUnlockAttempt(FALSE);
}

ECode FaceUnlock::HandlePokeWakelock(
    /* [in] */ Int32 millis)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj);
    Boolean res;
    if (powerManager->IsScreenOn(&res), res) {
        mKeyguardScreenCallback->UserActivity();
    }
    return NOERROR;
}

void FaceUnlock::StartUi(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    if (DEBUG) Logger::D(TAG, "startUi()");
    {
        AutoLock lock(mServiceRunningLock);
        if (!mServiceRunning) {
            Logger::D(TAG, "Starting Face Unlock");
            //try {
            Boolean res;
            mLockPatternUtils->IsBiometricWeakLivelinessEnabled(&res);
            ECode ec = mService->StartUi(windowToken, x, y, w, h, res);
            //} catch (RemoteException e) {
            if (ec == (ECode)RemoteException) {
                Logger::E(TAG, "Caught exception starting Face Unlock: " + e.toString());
                return;
            }
            mServiceRunning = TRUE;
        }
        else {
            Logger::W(TAG, "startUi() attempted while running");
        }
    }
}

void FaceUnlock::StopUi()
{
    if (DEBUG) Logger::D(TAG, "stopUi()");
    // Note that attempting to stop Face Unlock when it's not running is not an issue.
    // Face Unlock can return, which stops it and then we try to stop it when the
    // screen is turned off.  That's why we check.
    {
        AutoLock lock(mServiceRunningLock);
        if (mServiceRunning) {
            Logger::D(TAG, "Stopping Face Unlock");
            //try {
            ECode ec = mService->StopUi();
            //} catch (RemoteException e) {
            if (ec == (ECode)RemoteException) {
                Logger::E(TAG, "Caught exception stopping Face Unlock: " + e.toString());
            }
            mServiceRunning = FALSE;
        }
        else {
            // This is usually not an error when this happens.  Sometimes we will tell it to
            // stop multiple times because it's called from both onWindowFocusChanged and
            // onDetachedFromWindow.
            if (DEBUG) Logger::D(TAG, "stopUi() attempted while not running");
        }
    }
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
