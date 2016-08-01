
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/PostCharDialogFragment.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
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
// CInCallActivity::MyDialogOnClickListener
//=============================================================
CAR_INTERFACE_IMPL(CInCallActivity::MyDialogOnClickListener, Object, IDialogInterfaceOnClickListener)

ECode CInCallActivity::MyDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->OnDialogDismissed();
    return NOERROR;
}


//=============================================================
// CInCallActivity::MyDialogOnCancelListener
//=============================================================
CAR_INTERFACE_IMPL(CInCallActivity::MyDialogOnCancelListener, Object, IDialogInterfaceOnCancelListener)

ECode CInCallActivity::MyDialogOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->OnDialogDismissed();
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
    mCallButtonFragment->DisplayDialpad(FALSE /* show */, TRUE /* animate */);
}

void CInCallActivity::DismissKeyguard(
    /* [in] */ Boolean dismiss)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    if (dismiss) {
        win->AddFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD);
    }
    else {
        win->ClearFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD);
    }
}

void CInCallActivity::ShowDialpad(
    /* [in] */ Boolean showDialpad)
{
    // If the dialpad is being shown and it has not already been loaded, replace the dialpad
    // placeholder with the actual fragment before continuing.
    if (mDialpadFragment == NULL && showDialpad) {
        AutoPtr<IFragmentTransaction> loadTransaction;
        mChildFragmentManager->BeginTransaction((IFragmentTransaction**)&loadTransaction);
        AutoPtr<IView> fragmentContainer = FindViewById(Elastos::Droid::Dialer::R::id::dialpadFragmentContainer);
        AutoPtr<CDialpadFragment> temp;
        CDialpadFragment::NewByFriend((CDialpadFragment**)&temp);
        mDialpadFragment = temp;
        Int32 id;
        fragmentContainer->GetId(&id);
        EGuid clsId;
        mDialpadFragment->GetClassID(&clsId);
        loadTransaction->Replace(id, IFragment::Probe(mDialpadFragment), String(clsId.mUunm));
        Int32 value;
        loadTransaction->CommitAllowingStateLoss(&value);
        Boolean result;
        mChildFragmentManager->ExecutePendingTransactions(&result);
    }

    AutoPtr<IFragmentTransaction> ft;
    mChildFragmentManager->BeginTransaction((IFragmentTransaction**)&ft);
    if (showDialpad) {
        ft->Show(mDialpadFragment);
    } else {
        ft->Hide(mDialpadFragment);
    }
    Int32 value;
    ft->CommitAllowingStateLoss(&value);
}

void CInCallActivity::DisplayDialpad(
    /* [in] */ Boolean showDialpad,
    /* [in] */ Boolean animate)
{
    // If the dialpad is already visible, don't animate in. If it's gone, don't animate out.
    if ((showDialpad && IsDialpadVisible()) || (!showDialpad && !IsDialpadVisible())) {
        return;
    }
    // We don't do a FragmentTransaction on the hide case because it will be dealt with when
    // the listener is fired after an animation finishes.
    if (!animate) {
        ShowDialpad(showDialpad);
    }
    else {
        if (showDialpad) {
            ShowDialpad(TRUE);
            mDialpadFragment->AnimateShowDialpad();
        }
        mCallCardFragment->OnDialpadVisiblityChange(showDialpad);
        AutoPtr<IView> v;
        mDialpadFragment->GetView((IView**)&v);
        v->StartAnimation(showDialpad ? mSlideIn : mSlideOut);
    }

    InCallPresenter::GetInstance()->GetProximitySensor()->OnDialpadVisible(showDialpad);
}

Boolean CInCallActivity::IsDialpadVisible()
{
    Boolean isVisible;
    return mDialpadFragment != NULL && (mDialpadFragment->IsVisible(&isVisible), isVisible);
}

void CInCallActivity::ShowConferenceCallManager()
{
    mConferenceManagerFragment->SetVisible(TRUE);
}

void CInCallActivity::ShowPostCharWaitDialog(
    /* [in] */ const String& callId,
    /* [in] */ const String& chars)
{
    if (IsForegroundActivity()) {
        AutoPtr<PostCharDialogFragment> fragment = new PostCharDialogFragment(callId,  chars);
        AutoPtr<IFragmentManager> fragmentManager;
        GetFragmentManager((IFragmentManager**)&fragmentManager);
        fragment->Show(fragmentManager, String("postCharWait"));

        mShowPostCharWaitDialogOnResume = FALSE;
        mShowPostCharWaitDialogCallId = String(NULL);
        mShowPostCharWaitDialogChars = String(NULL);
    }
    else {
        mShowPostCharWaitDialogOnResume = TRUE;
        mShowPostCharWaitDialogCallId = callId;
        mShowPostCharWaitDialogChars = chars;
    }
}

ECode CInCallActivity::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    if (mCallCardFragment != NULL) {
        mCallCardFragment->DispatchPopulateAccessibilityEvent(event);
    }
    return Activity::DispatchPopulateAccessibilityEvent(event, isConsumed);
}

void CInCallActivity::MaybeShowErrorDialogOnDisconnect(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    Logger::D("CInCallActivity", "maybeShowErrorDialogOnDisconnect");

    Boolean isFinishing;
    AutoPtr<ICharSequence> desc;
    Int32 code;
    if ((IsFinishing(&isFinishing), !isFinishing) &&
            (disconnectCause->GetDescription((ICharSequence**)&desc), !TextUtils::IsEmpty(desc))
            && ((disconnectCause->GetCode(&code), code == IDisconnectCause::ERROR) ||
                    (disconnectCause->GetCode(&code), code == IDisconnectCause::RESTRICTED))) {
        ShowErrorDialog(desc);
    }
}

void CInCallActivity::DismissPendingDialogs()
{
    if (mDialog != NULL) {
        IDialogInterface::Probe(mDialog)->Dismiss();
        mDialog = NULL;
    }
    mAnswerFragment->DismissPendingDialogues();
}

void CInCallActivity::ShowErrorDialog(
    /* [in] */ ICharSequence* msg)
{
    Logger::I("CInCallActivity", "Show Dialog: %s", TO_CSTR(msg));

    DismissPendingDialogs();

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);
    builder->SetMessage(msg);
    AutoPtr<IDialogInterfaceOnClickListener> listener = (IDialogInterfaceOnClickListener*)new MyDialogOnClickListener(this);
    builder->SetPositiveButton(Elastos::Droid::Dialer::R::string::ok, listener);
    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener  = new MyDialogOnCancelListener(this);
    builder->SetOnCancelListener(cancelListener);
    AutoPtr<IAlertDialog> temp;
    builder->Create((IAlertDialog**)&temp);
    mDialog = temp;

    AutoPtr<IDialog> d = IDialog::Probe(mDialog);
    AutoPtr<IWindow> win;
    d->GetWindow((IWindow**)&win);
    win->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
    d->Show();
}

void CInCallActivity::OnDialogDismissed()
{
    mDialog = NULL;
    InCallPresenter::GetInstance()->OnDismissDialog();
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
