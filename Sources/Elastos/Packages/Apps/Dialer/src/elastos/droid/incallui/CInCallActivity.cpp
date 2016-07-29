
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CInCallActivity.h"
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
    assert(0 && "TODO");
    // if (mDialpadFragment != NULL) {
    //     out->PutString(DIALPAD_TEXT_EXTRA, mDialpadFragment->GetDtmfText());
    // }
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
