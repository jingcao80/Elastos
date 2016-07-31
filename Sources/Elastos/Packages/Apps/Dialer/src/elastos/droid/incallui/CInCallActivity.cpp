
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Phone::Common::Animation::AnimUtils;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//=============================================================
// CInCallActivity::MyAnimationListenerAdapter
//=============================================================
ECode CInCallActivity::MyAnimationListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->ShowDialpad(FALSE);
    return NOERROR;
}


//=============================================================
// CInCallActivity
//=============================================================
const String CInCallActivity::SHOW_DIALPAD_EXTRA("InCallActivity.show_dialpad");
const String CInCallActivity::DIALPAD_TEXT_EXTRA("InCallActivity.dialpad_text");
const String CInCallActivity::NEW_OUTGOING_CALL("InCallActivity.new_outgoing_call");

CAR_INTERFACE_IMPL(CInCallActivity, Activity, IInCallActivity)

CAR_OBJECT_IMPL(CInCallActivity)

CInCallActivity::CInCallActivity()
    : mIsForegroundActivity(FALSE)
    , mShowDialpadRequested(FALSE)
    , mAnimateDialpadOnShow(FALSE)
    , mShowPostCharWaitDialogOnResume(FALSE)
    , mIsLandscape(FALSE)
    , mCurrentOrientation(0)
{
    mSlideOutListener = new MyAnimationListenerAdapter(this);
}

ECode CInCallActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Logger::D("CInCallActivity", "onCreate()...  this = %s", TO_CSTR(this));

    FAIL_RETURN(Activity::OnCreate(icicle));

    // set this flag so this activity will stay in front of the keyguard
    // Have the WindowManager filter out touch events that are "too fat".
    Int32 flags = IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED
            | IWindowManagerLayoutParams::FLAG_TURN_SCREEN_ON
            | IWindowManagerLayoutParams::FLAG_IGNORE_CHEEK_PRESSES;

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddFlags(flags);

    // Setup action bar for the conference call manager.
    Boolean result;
    RequestWindowFeature(IWindow::FEATURE_ACTION_BAR_OVERLAY, &result);
    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
        actionBar->SetDisplayShowTitleEnabled(TRUE);
        actionBar->Hide();
    }

    // TODO(klp): Do we need to add this back when prox sensor is not available?
    // lp.inputFeatures |= WindowManager.LayoutParams.INPUT_FEATURE_DISABLE_USER_ACTIVITY;

    // Inflate everything in incall_screen.xml and add it to the screen.
    SetContentView(R::layout::incall_screen);

    InitializeInCall();

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    InternalResolveIntent(intent);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    config->GetOrientation(&mCurrentOrientation);
    mIsLandscape = mCurrentOrientation == IConfiguration::ORIENTATION_LANDSCAPE;

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Boolean isRtl = TextUtils::GetLayoutDirectionFromLocale(locale) ==
            IView::LAYOUT_DIRECTION_RTL;

    if (mIsLandscape) {
        AnimationUtils::LoadAnimation(this,
                isRtl ? R::anim::dialpad_slide_in_left : R::anim::dialpad_slide_in_right,
                (IAnimation**)&mSlideIn);
        AnimationUtils::LoadAnimation(this,
                isRtl ? R::anim::dialpad_slide_out_left : R::anim::dialpad_slide_out_right,
                (IAnimation**)&mSlideOut);
    }
    else {
        AnimationUtils::LoadAnimation(this, R::anim::dialpad_slide_in_bottom,
                (IAnimation**)&mSlideIn);
        AnimationUtils::LoadAnimation(this, R::anim::dialpad_slide_out_bottom,
                (IAnimation**)&mSlideOut);
    }

    mSlideIn->SetInterpolator(AnimUtils::EASE_IN);
    mSlideOut->SetInterpolator(AnimUtils::EASE_OUT);

    mSlideOut->SetAnimationListener(mSlideOutListener);

    if (icicle != NULL) {
        // If the dialpad was shown before, set variables indicating it should be shown and
        // populated with the previous DTMF text.  The dialpad is actually shown and populated
        // in onResume() to ensure the hosting CallCardFragment has been inflated and is ready
        // to receive it.
        icicle->GetBoolean(SHOW_DIALPAD_EXTRA, &mShowDialpadRequested);
        mAnimateDialpadOnShow = FALSE;
        icicle->GetString(DIALPAD_TEXT_EXTRA, &mDtmfText);
    }
    Logger::D("CInCallActivity", "onCreate(): exit");
    return NOERROR;
}

ECode CInCallActivity::OnSaveInstanceState(
    /* [in] */ IBundle* out)
{
    Boolean isVisible;
    mCallButtonFragment->IsDialpadVisible(&isVisible);
    out->PutBoolean(SHOW_DIALPAD_EXTRA, isVisible);
    if (mDialpadFragment != NULL) {
        out->PutString(DIALPAD_TEXT_EXTRA, mDialpadFragment->GetDtmfText());
    }
    return NOERROR;
}

ECode CInCallActivity::OnStart()
{
    Logger::D("CInCallActivity", "onStart()...");
    FAIL_RETURN(Activity::OnStart());

    // setting activity should be last thing in setup process
    InCallPresenter::GetInstance()->SetActivity(this);
    return NOERROR;
}

ECode CInCallActivity::OnResume()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnPause()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnStop()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnDestroy()
{
    assert(0 && "TODO");
    return NOERROR;
}

Boolean CInCallActivity::IsForegroundActivity()
{
    return mIsForegroundActivity;
}

Boolean CInCallActivity::HasPendingErrorDialog()
{
    return mDialog != NULL;
}

ECode CInCallActivity::Finish()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnBackPressed()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CInCallActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

Boolean CInCallActivity::HandleDialerKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    assert(0 && "TODO");
    return FALSE;
}

ECode CInCallActivity::OnConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    assert(0 && "TODO");
    return NOERROR;
}

AutoPtr<CCallButtonFragment> CInCallActivity::GetCallButtonFragment()
{
    assert(0 && "TODO");
    return NULL;
}

AutoPtr<CCallCardFragment> CInCallActivity::GetCallCardFragment()
{
    assert(0 && "TODO");
    return NULL;
}

void CInCallActivity::InternalResolveIntent(
    /* [in] */ IIntent* intent)
{
    assert(0 && "TODO");
}

Boolean CInCallActivity::IsEmergencyCall(
    /* [in] */ Call* call)
{
    assert(0 && "TODO");
    return FALSE;
}

void CInCallActivity::RelaunchedFromDialer(
    /* [in] */ Boolean showDialpad)
{
    assert(0 && "TODO");
}

void CInCallActivity::InitializeInCall()
{
    assert(0 && "TODO");
}

void CInCallActivity::HideDialpadForDisconnect()
{
    assert(0 && "TODO");
}

void CInCallActivity::DismissKeyguard(
    /* [in] */ Boolean dismiss)
{
    assert(0 && "TODO");
}

void CInCallActivity::ShowDialpad(
    /* [in] */ Boolean showDialpad)
{
    assert(0 && "TODO");
}

void CInCallActivity::DisplayDialpad(
    /* [in] */ Boolean showDialpad,
    /* [in] */ Boolean animate)
{
    assert(0 && "TODO");
}

Boolean CInCallActivity::IsDialpadVisible()
{
    assert(0 && "TODO");
    return FALSE;
}

void CInCallActivity::ShowConferenceCallManager()
{
    assert(0 && "TODO");
}

void CInCallActivity::ShowPostCharWaitDialog(
    /* [in] */ const String& callId,
    /* [in] */ const String& chars)
{
    assert(0 && "TODO");
}

ECode CInCallActivity::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    assert(0 && "TODO");
    return NOERROR;
}

void CInCallActivity::MaybeShowErrorDialogOnDisconnect(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    assert(0 && "TODO");
}

void CInCallActivity::DismissPendingDialogs()
{
    assert(0 && "TODO");
}

void CInCallActivity::ShowErrorDialog(
    /* [in] */ ICharSequence* msg)
{
    assert(0 && "TODO");
}

void CInCallActivity::OnDialogDismissed()
{
    assert(0 && "TODO");
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
