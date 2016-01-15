
#include "elastos/droid/internal/app/RestrictionsPinActivity.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL_4(RestrictionsPinActivity::LocalListener, Object, IViewOnClickListener,
    ITextWatcher, INoCopySpan, IOnEditorActionListener)

RestrictionsPinActivity::LocalListener::LocalListener(
    /* [in] */ RestrictionsPinActivity* host)
    : mHost(host)
{
}

ECode RestrictionsPinActivity::LocalListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode RestrictionsPinActivity::LocalListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode RestrictionsPinActivity::LocalListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

ECode RestrictionsPinActivity::LocalListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}

ECode RestrictionsPinActivity::LocalListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

RestrictionsPinActivity::MyRunnable::MyRunnable(
    /* [in] */ RestrictionsPinActivity* host)
    : mHost(host)
{}

ECode RestrictionsPinActivity::MyRunnable::Run()
{
    if (mHost->UpdatePinTimer(-1)) {
        // If we are no longer counting down, clear the message.
        IView::Probe(mHost->mPinErrorMessage)->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_5(RestrictionsPinActivity, AlertActivity, IRestrictionsPinActivity, IViewOnClickListener,
    ITextWatcher, INoCopySpan, IOnEditorActionListener)

RestrictionsPinActivity::RestrictionsPinActivity()
    : mHasRestrictionsPin(FALSE)
{
    mCountdownRunnable = new MyRunnable(this);
}

ECode RestrictionsPinActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AlertActivity::OnCreate(icicle);

    AutoPtr<IInterface> service;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    mUserManager = IUserManager::Probe(service);
    mUserManager->HasRestrictionsChallenge(&mHasRestrictionsPin);
    InitUi();
    SetupAlert();
    return NOERROR;
}

void RestrictionsPinActivity::InitUi()
{
    AutoPtr<IAlertControllerAlertParams> ap = mAlertParams;
    AutoPtr<ICharSequence> title;
    GetText(R::string::restr_pin_enter_admin_pin, (ICharSequence**)&title);
    ap->SetTitle(title);
    AutoPtr<IInterface> service;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::restrictions_pin_challenge, NULL, (IView**)&view);
    ap->SetView(view);

    AutoPtr<IView> tempView;
    view->FindViewById(R::id::pin_error_message, (IView**)&tempView);
    mPinErrorMessage = ITextView::Probe(tempView);
    tempView = NULL;
    view->FindViewById(R::id::pin_text, (IView**)&tempView);
    mPinText = IEditText::Probe(tempView);
    tempView = NULL;
    view->FindViewById(R::id::pin_ok_button, (IView**)&tempView);
    mOkButton = IButton::Probe(tempView);
    tempView = NULL;
    view->FindViewById(R::id::pin_cancel_button, (IView**)&tempView);
    mCancelButton = IButton::Probe(tempView);

    AutoPtr<LocalListener> listener = new LocalListener(this);
    ITextView::Probe(mPinText)->AddTextChangedListener(listener);
    IView::Probe(mOkButton)->SetOnClickListener(listener);
    IView::Probe(mCancelButton)->SetOnClickListener(listener);
}

Boolean RestrictionsPinActivity::VerifyingPin()
{
    return TRUE;
}

ECode RestrictionsPinActivity::OnResume()
{
    AlertActivity::OnResume();

    SetPositiveButtonState(FALSE);
    Boolean hasPin;
    mUserManager->HasRestrictionsChallenge(&hasPin);
    if (hasPin) {
        IView::Probe(mPinErrorMessage)->SetVisibility(IView::INVISIBLE);
        AutoPtr<LocalListener> listener = new LocalListener(this);
        ITextView::Probe(mPinText)->SetOnEditorActionListener(listener);
        UpdatePinTimer(-1);
    }
    else if (VerifyingPin()) {
        SetResult(RESULT_OK);
        Finish();
    }
    return NOERROR;
}

void RestrictionsPinActivity::SetPositiveButtonState(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mOkButton)->SetEnabled(enabled);
}

Boolean RestrictionsPinActivity::UpdatePinTimer(
    /* [in] */ Int32 pinTimerMs)
{
    if (pinTimerMs < 0) {
        mUserManager->CheckRestrictionsChallenge(String(NULL), &pinTimerMs);
    }
    Boolean enableInput;
    if (pinTimerMs >= 200) {
        // Do the count down timer for less than a minute, otherwise just say try again later.
        if (pinTimerMs <= 60000) {
            Int32 seconds = (pinTimerMs + 200) / 1000;
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            String formatString;
            resources->GetQuantityString(R::plurals::restr_pin_countdown,
                    seconds, &formatString);
            AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
            params->Set(0, CoreUtils::Convert(seconds));
            mPinErrorMessage->SetText(CoreUtils::Convert(StringUtils::Format(formatString, params)));
        }
        else {
            mPinErrorMessage->SetText(R::string::restr_pin_try_later);
        }
        enableInput = FALSE;
        IView::Probe(mPinErrorMessage)->SetVisibility(IView::VISIBLE);
        ITextView::Probe(mPinText)->SetText(CoreUtils::Convert(String("")));
        Boolean res;
        IView::Probe(mPinText)->PostDelayed(mCountdownRunnable,
            Elastos::Core::Math::Min(1000, pinTimerMs), &res);
    }
    else {
        enableInput = TRUE;
        mPinErrorMessage->SetText(R::string::restr_pin_incorrect);
    }
    IView::Probe(mPinText)->SetEnabled(enableInput);
    SetPositiveButtonState(enableInput);
    return enableInput;
}

void RestrictionsPinActivity::PerformPositiveButtonAction()
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mPinText)->GetText((ICharSequence**)&text);
    String strText;
    text->ToString(&strText);
    Int32 result;
    mUserManager->CheckRestrictionsChallenge(strText, &result);
    if (result == IUserManager::PIN_VERIFICATION_SUCCESS) {
        SetResult(RESULT_OK);
        Finish();
    }
    else if (result >= 0) {
        mPinErrorMessage->SetText(R::string::restr_pin_incorrect);
        IView::Probe(mPinErrorMessage)->SetVisibility(IView::VISIBLE);
        UpdatePinTimer(result);
        ITextView::Probe(mPinText)->SetText(CoreUtils::Convert(String("")));
    }
}

ECode RestrictionsPinActivity::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode RestrictionsPinActivity::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    AutoPtr<ICharSequence> pin;
    ITextView::Probe(mPinText)->GetText((ICharSequence**)&pin);
    Int32 length;
    SetPositiveButtonState(pin != NULL && (pin->GetLength(&length), length >= 4));
    return NOERROR;
}

ECode RestrictionsPinActivity::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

ECode RestrictionsPinActivity::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    PerformPositiveButtonAction();
    *result = TRUE;
    return NOERROR;
}

ECode RestrictionsPinActivity::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mOkButton)) {
        PerformPositiveButtonAction();
    }
    else if (v == IView::Probe(mCancelButton)) {
        SetResult(RESULT_CANCELED);
        Finish();
    }
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
