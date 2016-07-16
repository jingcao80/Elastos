
#include "elastos/droid/systemui/keyguard/CKeyguardFaceUnlockView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnPhoneStateChanged(
    /* [in] */ Int32 phoneState)
{
    if (DEBUG) Logger::D(TAG, "onPhoneStateChanged(%d)", phoneState);
    if (phoneState == ITelephonyManager::CALL_STATE_RINGING) {
        if (mBiometricUnlock != NULL) {
            mBiometricUnlock->stopAndShowBackup();
        }
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnUserSwitching(
    /* [in] */ Int32 userId)
{
    if (DEBUG) Logger::D(TAG, "onUserSwitched(%d)", userId);
    if (mBiometricUnlock != NULL) {
        mBiometricUnlock->Stop();
    }
    // No longer required; static value set by KeyguardViewMediator
    // mLockPatternUtils.setCurrentUser(userId);
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    if (DEBUG) Logger::D(TAG, "onUserSwitchComplete(%d)", userId);
    if (mBiometricUnlock != NULL) {
        MaybeStartBiometricUnlock();
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnKeyguardVisibilityChanged(
    /* [in] */ Boolean showing)
{
    if (DEBUG) Logger::D(TAG, "onKeyguardVisibilityChanged(%d)", showing);
    HandleBouncerUserVisibilityChanged();
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnKeyguardBouncerChanged(
    /* [in] */ Boolean bouncer)
{
    if (DEBUG) Logger::D(TAG, "onKeyguardBouncerChanged(%d)", bouncer);
    HandleBouncerUserVisibilityChanged();
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    if (DEBUG) Logger::D(TAG, "onScreenTurnedOn()");
    HandleBouncerUserVisibilityChanged();
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    if (DEBUG) Logger::D(TAG, "onScreenTurnedOff()");
    HandleBouncerUserVisibilityChanged();
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback::OnEmergencyCallAction()
{
    if (mBiometricUnlock != NULL) {
        mBiometricUnlock->Stop();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardFaceUnlockView::MyOnClickListener, Object, IViewOnClickListener)

ECode CKeyguardFaceUnlockView::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mBiometricUnlock->StopAndShowBackup();
}

const String CKeyguardFaceUnlockView::TAG("FULKeyguardFaceUnlockView");
const Boolean CKeyguardFaceUnlockView::DEBUG = IKeyguardConstants::DEBUG;

CAR_OBJECT_IMPL(CKeyguardFaceUnlockView)

CAR_INTERFACE_IMPL(CKeyguardFaceUnlockView, LinearLayout, IKeyguardSecurityView)

CKeyguardFaceUnlockView::CKeyguardFaceUnlockView()
    : mIsBouncerVisibleToUser(FALSE)
    , mLastRotation(0)
    , mWatchingRotation(FALSE)
{
    AutoPtr<IServiceManager> helper;
    CServiceManager::AcquireSingleton((IServiceManager**)&helper);
    AutoPtr<IInterface> obj;
    helper->GetService(String("window"), (IInterface**)&obj);
    AutoPtr<IIWindowManager> mWindowManager = IIWindowManager::Probe(obj);

    AutoPtr<IIRotationWatcher> mRotationWatcher = new IRotationWatcher.Stub();

    KeyguardUpdateMonitorCallback mUpdateCallback = new MyKeyguardUpdateMonitorCallback(this);
}

ECode CKeyguardFaceUnlockView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardFaceUnlockView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CKeyguardFaceUnlockView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    InitializeBiometricUnlockView();

    mSecurityMessageDisplay = new KeyguardMessageArea::Helper(this);
    FindViewById(R::id::keyguard_selector_fade_container, (IView**)&mEcaView);
    AutoPtr<IView> bouncerFrameView;
    FindViewById(R::id::keyguard_bouncer_frame, (IView**)&bouncerFrameView);
    if (bouncerFrameView != NULL) {
        bouncerFrameView->GetBackground((IDrawable**)&mBouncerFrame);
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    mKeyguardSecurityCallback = callback;
    // TODO: formalize this in the interface or factor it out
    return ((FaceUnlock*)IObject::Probe(mBiometricUnlock))->SetKeyguardCallback(callback);
}

ECode CKeyguardFaceUnlockView::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::Reset()
{
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::OnDetachedFromWindow()
{
    if (DEBUG) Logger::D(TAG, "onDetachedFromWindow()");
    if (mBiometricUnlock != NULL) {
        mBiometricUnlock->Stop();
    }
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->RemoveCallback(mUpdateCallback);
    if (mWatchingRotation) {
        //try {
        ECode ec = NOERROR;
        FAIL_GOTO(ec = mWindowManager->RemoveRotationWatcher(mRotationWatcher), ERROR)
        mWatchingRotation = FALSE;
        //} catch (RemoteException e) {
ERROR:
        if (ec == (ECode)RemoteException)
            Logger::E(TAG, "Remote exception when removing rotation watcher");
        }
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::OnPause()
{
    if (DEBUG) Logger::D(TAG, "onPause()");
    if (mBiometricUnlock != NULL) {
        mBiometricUnlock->Stop();
    }
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->RemoveCallback(mUpdateCallback);
    if (mWatchingRotation) {
        //try {
        ECode ec = NOERROR;
        FAIL_GOTO(ec = mWindowManager->RemoveRotationWatcher(mRotationWatcher), ERROR)
        mWatchingRotation = FALSE;
        //} catch (RemoteException e) {
ERROR:
        if (ec == (ECode)RemoteException) {
            Logger::E(TAG, "Remote exception when removing rotation watcher");
        }
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::OnResume(
    /* [in] */ Int32 reason)
{
    if (DEBUG) Logger::D(TAG, "onResume()");
    {
        AutoLock syncLock(mIsBouncerVisibleToUserLock);
        mIsBouncerVisibleToUser = IsBouncerVisibleToUser();
    }
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->RegisterCallback(mUpdateCallback);

    // Registers a callback which handles stopping the biometric unlock and restarting it in
    // the new position for a 180 degree rotation change.
    if (!mWatchingRotation) {
        //try {
        ECode ec = NOERROR;
        FAIL_GOTO(ec = mLastRotation = mWindowManager.watchRotation(mRotationWatcher), ERROR)
        mWatchingRotation = TRUE;
        //} catch (RemoteException e) {
ERROR:
        if (ec == (ECode)RemoteException) {
            Logger::E(TAG, "Remote exception when adding rotation watcher");
        }
    }
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::GetCallback(
    /* [out] */ IKeyguardSecurityCallback** callback)
{
    VALIDATE_NOT_NULL(callback)

    *callback = mKeyguardSecurityCallback;
    REFCOUNT_ADD(*callback);
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    LinearLayout::OnLayout(changed, l, t, r, b);
    return mBiometricUnlock->InitializeView(mFaceUnlockAreaView);
}

void CKeyguardFaceUnlockView::InitializeBiometricUnlockView()
{
    if (DEBUG) Logger::D(TAG, "initializeBiometricUnlockView()");
    FindViewById(R::id::face_unlock_area_view, (IView**)&mFaceUnlockAreaView);
    if (mFaceUnlockAreaView != NULL) {
        mBiometricUnlock = new FaceUnlock(mContext);

        AutoPtr<IView> view;
        FindViewById(R::id::face_unlock_cancel_button, (IView**)&view);
        mCancelButton = IImageButton::Probe(view);
        AutoPtr<IViewOnClickListener> lis = new MyOnClickListener(this);
        mCancelButton->SetOnClickListener(lis);
    }
    else {
        Logger::W(TAG, "Couldn't find biometric unlock view");
    }
}

void CKeyguardFaceUnlockView::MaybeStartBiometricUnlock()
{
    if (DEBUG) Logger::D(TAG, "maybeStartBiometricUnlock()");
    if (mBiometricUnlock != NULL) {
        AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
        Boolean backupIsTimedOut = (
                monitor.getFailedUnlockAttempts() >=
                ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT);

        Boolean isBouncerVisibleToUser;
        {
            AutoLock syncLock(mIsBouncerVisibleToUserLock);
            isBouncerVisibleToUser = mIsBouncerVisibleToUser;
        }

        // Don't start it if the bouncer is not showing, but keep this view up because we want
        // it here and ready for when the bouncer does show.
        if (!isBouncerVisibleToUser) {
            mBiometricUnlock->Stop(); // It shouldn't be running but calling this can't hurt.
            return;
        }

        // Although these same conditions are handled in KeyguardSecurityModel, they are still
        // necessary here.  When a tablet is rotated 90 degrees, a configuration change is
        // triggered and everything is torn down and reconstructed.  That means
        // KeyguardSecurityModel gets a chance to take care of the logic and doesn't even
        // reconstruct KeyguardFaceUnlockView if the biometric unlock should be suppressed.
        // However, for a 180 degree rotation, no configuration change is triggered, so only
        // the logic here is capable of suppressing Face Unlock.
        Int32 state;
        monitor->GetPhoneState(&state);
        Boolean res1, res2;
        if (state == ITelephonyManager::CALL_STATE_IDLE
                && (monitor->IsAlternateUnlockEnabled(&res1), res1)
                && (monitor->GetMaxBiometricUnlockAttemptsReached(&res2), !res2)
                && !backupIsTimedOut) {
            mBiometricUnlock->Start();
        }
        else {
            mBiometricUnlock->StopAndShowBackup();
        }
    }
}

Boolean CKeyguardFaceUnlockView::IsBouncerVisibleToUser()
{
    AutoPtr<KeyguardUpdateMonitor> updateMonitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Boolean res1, res2, res3;
    return (updateMonitor->IsKeyguardBouncer(&res1), res1) &&
            (updateMonitor->IsKeyguardVisible(&res2), res2) &&
            (updateMonitor->IsScreenOn(&res3), res3);
}

void CKeyguardFaceUnlockView::HandleBouncerUserVisibilityChanged()
{
    Boolean wasBouncerVisibleToUser;
    {
        AutoLock syncLock(mIsBouncerVisibleToUserLock);
        wasBouncerVisibleToUser = mIsBouncerVisibleToUser;
        mIsBouncerVisibleToUser = IsBouncerVisibleToUser();
    }

    if (mBiometricUnlock != NULL) {
        if (wasBouncerVisibleToUser && !mIsBouncerVisibleToUser) {
            mBiometricUnlock->Stop();
        }
        else if (!wasBouncerVisibleToUser && mIsBouncerVisibleToUser) {
            MaybeStartBiometricUnlock();
        }
    }
}

ECode CKeyguardFaceUnlockView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::ShowBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::ShowBouncer(mSecurityMessageDisplay, mEcaView, mBouncerFrame, duration);
}

ECode CKeyguardFaceUnlockView::HideBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::HideBouncer(mSecurityMessageDisplay, mEcaView, mBouncerFrame, duration);
}

ECode CKeyguardFaceUnlockView::StartAppearAnimation()
{
    // TODO.
    return NOERROR;
}

ECode CKeyguardFaceUnlockView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
