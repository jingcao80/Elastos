
#include "elastos/droid/systemui/keyguard/KeyguardViewBase.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardViewBase::MyKeyguardActivityLauncher::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)

    *context = mHost->mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode KeyguardViewBase::MyKeyguardActivityLauncher::SetOnDismissAction(
    /* [in] */ IKeyguardHostViewOnDismissAction* action)
{
    return mHost->SetOnDismissAction(action);
}

ECode KeyguardViewBase::MyKeyguardActivityLauncher::GetLockPatternUtils(
    /* [out] */ ILockPatternUtils** utils)
{
    VALIDATE_NOT_NULL(utils)

    *utils = mHost->mLockPatternUtils;
    REFCOUNT_ADD(*utils)
    return NOERROR;
}

ECode KeyguardViewBase::MyKeyguardActivityLauncher::RequestDismissKeyguard()
{
    return mHost->Dismiss(FALSE);
}

static const Boolean KeyguardViewBase::DEBUG = IKeyguardConstants::DEBUG;

static const Boolean KeyguardViewBase::KEYGUARD_MANAGES_VOLUME = FALSE;
static const String KeyguardViewBase::TAG("KeyguardViewBase");

const String KeyguardViewBase::ENABLE_MENU_KEY_FILE("/data/local/enable_menu_key");

CAR_INTERFACE_IMPL(KeyguardViewBase, FrameLayout, IKeyguardSecurityContainerSecurityCallback)

KeyguardViewBase::KeyguardViewBase()
{
    mActivityLauncher = new MyKeyguardActivityLauncher(this);
}

ECode KeyguardViewBase::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode KeyguardViewBase::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode KeyguardViewBase::DispatchDraw(
    /* [in] */ ICanvas canvas)
{
    FrameLayout::DispatchDraw(canvas);
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->KeyguardDoneDrawing();
    }
    return NOERROR;
}

ECode KeyguardViewBase::SetOnDismissAction(
    /* [in] */ IKeyguardHostViewOnDismissAction* action)
{
    mDismissAction = action;
    return NOERROR;
}

ECode KeyguardViewBase::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_security_container, (IView**)&view);
    mSecurityContainer = IKeyguardSecurityContainer::Probe(view);
    CLockPatternUtils::New(mContext, )&mLockPatternUtils);
    mSecurityContainer->SetLockPatternUtils(mLockPatternUtils);
    mSecurityContainer->SetSecurityCallback(this);
    return mSecurityContainer->ShowPrimarySecurityScreen(FALSE);
    // mSecurityContainer.updateSecurityViews(false /* not bouncing */);
}

ECode KeyguardViewBase::Show()
{
    if (DEBUG) Logger::D(TAG, "show()");
    return mSecurityContainer->ShowPrimarySecurityScreen(FALSE);
}

ECode KeyguardViewBase::Dismiss(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return Dismiss(FALSE, result);
}

ECode KeyguardViewBase::ShowBouncer(
    /* [in] */ Boolean show)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> what;
    resources->GetText(
            show ? R::string::keyguard_accessibility_show_bouncer
                    : R::string::keyguard_accessibility_hide_bouncer, (ICharSequence**)&what);
    AnnounceForAccessibility(what);
    return AnnounceCurrentSecurityMethod();
}

ECode KeyguardViewBase::HandleBackKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 selection;
    mSecurityContainer->GetCurrentSecuritySelection(&selection);
    if (selection == ISecurityMode::Account) {
        // go back to primary screen
        mSecurityContainer->ShowPrimarySecurityScreen(FALSE /*turningOff*/);
        *result = TRUE;
        return NOERROR;
    }
    mSecurityContainer->GetCurrentSecuritySelection(&selection);
    if (selection != ISecurityMode::None) {
        mSecurityContainer->Dismiss(FALSE);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardViewBase::AnnounceCurrentSecurityMethod()
{
    return mSecurityContainer->AnnounceCurrentSecurityMethod();
}

ECode KeyguardViewBase::GetSecurityContainer(
    /* [out] */ IKeyguardSecurityContainer** container)
{
    VALIDATE_NOT_NULL(container)

    *container = mSecurityContainer;
    REFCOUNT_ADD(*container)
    return NOERROR;
}

ECode KeyguardViewBase::Dismiss(
    /* [in] */ Boolean authenticated,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mSecurityContainer->ShowNextSecurityScreenOrFinish(authenticated, result);
}

ECode KeyguardViewBase::Finish()
{
    // If the alternate unlock was suppressed, it can now be safely
    // enabled because the user has left keyguard.
    KeyguardUpdateMonitor::GetInstance(mContext)->SetAlternateUnlockEnabled(TRUE);

    // If there's a pending runnable because the user interacted with a widget
    // and we're leaving keyguard, then run it.
    Boolean deferKeyguardDone = FALSE;
    if (mDismissAction != NULL) {
        mDismissAction->OnDismiss(&deferKeyguardDone);
        mDismissAction = NULL;
    }
    if (mViewMediatorCallback != NULL) {
        if (deferKeyguardDone) {
            mViewMediatorCallback->KeyguardDonePending();
        }
        else {
            mViewMediatorCallback->KeyguardDone(TRUE);
        }
    }
    return NOERROR;
}

ECode KeyguardViewBase::OnSecurityModeChanged(
    /* [in] */ ISecurityMode* securityMode,
    /* [in] */ Boolean needsInput)
{
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->SetNeedsInput(needsInput);
    }
    return NOERROR;
}

ECode KeyguardViewBase::UserActivity()
{
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->UserActivity();
    }
    return NOERROR;
}

ECode KeyguardViewBase::OnUserActivityTimeoutChanged()
{
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->OnUserActivityTimeoutChanged();
    }
    return NOERROR;
}

ECode KeyguardViewBase::OnPause()
{
    if (DEBUG) Logger::D(TAG, String.format("screen off, instance %s at %s",
            Integer.toHexString(hashCode()), SystemClock.uptimeMillis()));
    // Once the screen turns off, we no longer consider this to be first boot and we want the
    // biometric unlock to start next time keyguard is shown.
    KeyguardUpdateMonitor::GetInstance(mContext)->SetAlternateUnlockEnabled(TRUE);
    mSecurityContainer->ShowPrimarySecurityScreen(TRUE);
    mSecurityContainer->OnPause();
    return ClearFocus();
}

ECode KeyguardViewBase::OnResume()
{
    if (DEBUG) Logger::D(TAG, "screen on, instance " + Integer.toHexString(hashCode()));
    mSecurityContainer->ShowPrimarySecurityScreen(FALSE);
    mSecurityContainer->OnResume(IKeyguardSecurityView::SCREEN_ON);
    return RequestFocus();
}

ECode KeyguardViewBase::StartAppearAnimation()
{
    return mSecurityContainer->StartAppearAnimation();
}

ECode KeyguardViewBase::StartDisappearAnimation(
    /* [in] */ I IRunnable** finishRunnable)
{
    Boolean res;
    mSecurityContainer->StartDisappearAnimation(finishRunnable, &res);
    if (!res && finishRunnable != NULL) {
        finishRunnable->Run();
    }
    return NOERROR;
}

ECode KeyguardViewBase::VerifyUnlock()
{
    KeyguardSecurityModel::SecurityMode securityMode;
    mSecurityContainer->GetSecurityMode(&securityMode);
    if (securityMode == KeyguardSecurityModel::None) {
        if (mViewMediatorCallback != NULL) {
            mViewMediatorCallback->KeyguardDone(TRUE);
        }
    }
    else if (securityMode != KeyguardSecurityModel::Pattern
            && securityMode != KeyguardSecurityModel::PIN
            && securityMode != KeyguardSecurityModel::Password) {
        // can only verify unlock when in pattern/password mode
        if (mViewMediatorCallback != NULL) {
            mViewMediatorCallback->KeyguardDone(FALSE);
        }
    }
    else {
        // otherwise, go to the unlock screen, see if they can verify it
        mSecurityContainer->VerifyUnlock();
    }
    return NOERROR;
}

ECode KeyguardViewBase::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (InterceptMediaKey(event)) {
        *result = TRUE;
        return NOERROR;
    }
    return FrameLayout::DispatchKeyEvent(event, result);
}

ECode KeyguardViewBase::InterceptMediaKey(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 keyCode;
    event->GetKeyCode(&keyCode);

    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        switch (keyCode) {
            case IKeyEvent::KEYCODE_MEDIA_PLAY:
            case IKeyEvent::KEYCODE_MEDIA_PAUSE:
            case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
                /* Suppress PLAY/PAUSE toggle when phone is ringing or
                 * in-call to avoid music playback */
                if (mTelephonyManager == NULL) {
                    AutoPtr<IContext> context;
                    GetContext((IContext**)&context);
                    AutoPtr<IInterface> obj;
                    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
                    mTelephonyManager = ITelephonyManager::Probe(obj);
                }
                Int32 state;
                mTelephonyManager->GetCallState(&state);
                if (mTelephonyManager != NULL && state != ITelephonyManager::CALL_STATE_IDLE) {
                    *result = TRUE;  // suppress key event
                    return NOERROR;
                }
            case IKeyEvent::KEYCODE_MUTE:
            case IKeyEvent::KEYCODE_HEADSETHOOK:
            case IKeyEvent::KEYCODE_MEDIA_STOP:
            case IKeyEvent::KEYCODE_MEDIA_NEXT:
            case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
            case IKeyEvent::KEYCODE_MEDIA_REWIND:
            case IKeyEvent::KEYCODE_MEDIA_RECORD:
            case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
            case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK: {
                HandleMediaKeyEvent(event);
                *result = TRUE;
                return NOERROR;
            }

            case IKeyEvent::KEYCODE_VOLUME_UP:
            case IKeyEvent::KEYCODE_VOLUME_DOWN:
            case IKeyEvent::KEYCODE_VOLUME_MUTE: {
                if (KEYGUARD_MANAGES_VOLUME) {
                    {
                        AutoLock syncLock(this);
                        if (mAudioManager == NULL) {
                            AutoPtr<IContext> context;
                            GetContext((IContext**)&context);
                            AutoPtr<IInterface> obj;
                            context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
                            mAudioManager = IAudioManager::Probe(obj);
                        }
                    }
                    // Volume buttons should only function for music (local or remote).
                    // TODO: Actually handle MUTE.
                    mAudioManager->AdjustSuggestedStreamVolume(
                            keyCode == IKeyEvent::KEYCODE_VOLUME_UP
                                    ? IAudioManager::ADJUST_RAISE
                                    : IAudioManager::ADJUST_LOWER /* direction */,
                            IAudioManager::STREAM_MUSIC /* stream */, 0 /* flags */);
                    // Don't execute default volume behavior
                    *result = TRUE;
                    return NOERROR;
                }
                else {
                    *result = FALSE;
                    return NOERROR;
                }
            }
        }
    }
    else if ((event->GetAction(&action), action) == IKeyEvent::ACTION_UP) {
        switch (keyCode) {
            case IKeyEvent::KEYCODE_MUTE:
            case IKeyEvent::KEYCODE_HEADSETHOOK:
            case IKeyEvent::KEYCODE_MEDIA_PLAY:
            case IKeyEvent::KEYCODE_MEDIA_PAUSE:
            case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
            case IKeyEvent::KEYCODE_MEDIA_STOP:
            case IKeyEvent::KEYCODE_MEDIA_NEXT:
            case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
            case IKeyEvent::KEYCODE_MEDIA_REWIND:
            case IKeyEvent::KEYCODE_MEDIA_RECORD:
            case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
            case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK: {
                HandleMediaKeyEvent(event);
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

void KeyguardViewBase::HandleMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    {
        AutoLock syncLock(this);
        if (mAudioManager == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
            mAudioManager = IAudioManager::Probe(obj);
        }
    }
    mAudioManager->DispatchMediaKeyEvent(keyEvent);
}

ECode KeyguardViewBase::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    FrameLayout::DispatchSystemUiVisibilityChanged(visibility);

    if (IActivity::Probe(mContext) == NULL) {
        SetSystemUiVisibility(STATUS_BAR_DISABLE_BACK);
    }
    return NOERROR;
}

Boolean KeyguardViewBase::ShouldEnableMenuKey()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Boolean configDisabled;
    res->GetBoolean(R::bool_::config_disableMenuKeyInLockScreen, &configDisabled);

    Boolean isTestHarness;
    ActivityManager::IsRunningInTestHarness(&isTestHarness);

    AutoPtr<IFile> file;
    CFile::New(ENABLE_MENU_KEY_FILE, (IFile**)&file);
    Boolean fileOverride;
    file->Exists(&fileOverride);

    return !configDisabled || isTestHarness || fileOverride;
}

ECode KeyguardViewBase::HandleMenuKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // The following enables the MENU key to work for testing automation
    if (ShouldEnableMenuKey()) {
        Dismiss();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardViewBase::SetViewMediatorCallback(
    /* [in] */ IViewMediatorCallback* viewMediatorCallback)
{
    mViewMediatorCallback = viewMediatorCallback;
    // Update ViewMediator with the current input method requirements
    return mViewMediatorCallback->SetNeedsInput(mSecurityContainer.needsInput());
}

ECode KeyguardViewBase::GetActivityLauncher(
    /* [out] */ IKeyguardActivityLauncher** launcher)
{
    VALIDATE_NOT_NULL(launcher)

    *launcher = mActivityLauncher;
    REFCOUNT_ADD(*launcher)
    return NOERROR;
}

ECode KeyguardViewBase::ShowAssistant()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<IIntent> intent;
    ISearchManager::Probe(obj)->GetAssistIntent(mContext, TRUE, IUserHandle::USER_CURRENT, (IIntent**)&intent);

    if (intent == NULL) return NOERROR;

    AutoPtr<IActivityOptionsHelper> helper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    AutoPtr<IActivityOptions> opts;
    helper->MakeCustomAnimation(mContext,
            R::anim::keyguard_action_assist_enter, R::anim::keyguard_action_assist_exit,
            handler, NULL, (IActivityOptions**)&opts);

    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IBundle> bundle;
    opts->ToBundle((IBundle**)&bundle);
    return mActivityLauncher->LaunchActivityWithAnimation(intent, FALSE, bundle, NULL, NULL);
}

ECode KeyguardViewBase::LaunchCamera()
{
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    return mActivityLauncher->LaunchCamera(handler, NULL);
}

ECode KeyguardViewBase::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    return mSecurityContainer->SetLockPatternUtils(utils);
}

ECode KeyguardViewBase::GetSecurityMode(
    /* [out] */ iSecurityMode** mose)
{
    VALIDATE_NOT_NULL(mose);

    return mSecurityContainer->GetSecurityMode(mose);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
