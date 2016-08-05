
#include "elastos/droid/systemui/keyguard/KeyguardPinBasedInputView.h"
#include "elastos/droid/systemui/keyguard/LiftToActivateListener.h"
#include "R.h"

using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(KeyguardPinBasedInputView::MyOnClickListener,
        Object, IViewOnClickListener)

ECode KeyguardPinBasedInputView::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->UserActivity();
}

CAR_INTERFACE_IMPL(KeyguardPinBasedInputView::MyOnClickListener2,
        Object, IViewOnClickListener)

ECode KeyguardPinBasedInputView::MyOnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    mHost->DoHapticKeyClick();
    Boolean res;
    if (IView::Probe(mHost->mPasswordEntry)->IsEnabled(&res), res) {
        mHost->VerifyPasswordAndUnlock();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardPinBasedInputView::MyOnClickListener3,
        Object, IViewOnClickListener)

ECode KeyguardPinBasedInputView::MyOnClickListener3::OnClick(
    /* [in] */ IView* v)
{
    Boolean res;
    if (IView::Probe(mHost->mPasswordEntry)->IsEnabled(&res), res) {
        mHost->mPasswordEntry->DeleteLastChar();
    }
    return mHost->DoHapticKeyClick();
}

CAR_INTERFACE_IMPL(KeyguardPinBasedInputView::MyViewOnLongClickListener,
        Object, IViewOnLongClickListener)

ECode KeyguardPinBasedInputView::MyViewOnLongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // check for time-based lockouts
    Boolean res;
    if (IView::Probe(mHost->mPasswordEntry)->IsEnabled(&res), res) {
        mHost->ResetPasswordText(TRUE /* animate */);
    }
    mHost->DoHapticKeyClick();
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardPinBasedInputView, KeyguardAbsKeyInputView,
        IViewOnKeyListener)

ECode KeyguardPinBasedInputView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode KeyguardPinBasedInputView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return KeyguardAbsKeyInputView::constructor(context, attrs);
}

ECode KeyguardPinBasedInputView::Reset()
{
    Boolean res;
    IView::Probe(mPasswordEntry)->RequestFocus(&res);
    return KeyguardAbsKeyInputView::Reset();
}

ECode KeyguardPinBasedInputView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // send focus to the password field
    return IView::Probe(mPasswordEntry)->RequestFocus(direction, previouslyFocusedRect, result);
}

ECode KeyguardPinBasedInputView::ResetState()
{
    return IView::Probe(mPasswordEntry)->SetEnabled(TRUE);
}

ECode KeyguardPinBasedInputView::SetPasswordEntryEnabled(
    /* [in] */ Boolean enabled)
{
    return IView::Probe(mPasswordEntry)->SetEnabled(enabled);
}

ECode KeyguardPinBasedInputView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    Boolean res;
    if (helper->IsConfirmKey(keyCode, &res), res) {
        PerformClick(mOkButton);
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_DEL) {
        PerformClick(mDeleteButton);
        *result = TRUE;
        return NOERROR;
    }
    if (keyCode >= IKeyEvent::KEYCODE_0 && keyCode <= IKeyEvent::KEYCODE_9) {
        Int32 number = keyCode - IKeyEvent::KEYCODE_0 ;
        PerformNumberClick(number);
        *result = TRUE;
        return NOERROR;
    }
    return KeyguardAbsKeyInputView::OnKeyDown(keyCode, event, &res);
}

void KeyguardPinBasedInputView::PerformClick(
    /* [in] */ IView* view)
{
    Boolean res;
    view->PerformClick(&res);
}

void KeyguardPinBasedInputView::PerformNumberClick(
    /* [in] */ Int32 number)
{
    switch (number) {
        case 0:
            PerformClick(mButton0);
            break;
        case 1:
            PerformClick(mButton1);
            break;
        case 2:
            PerformClick(mButton2);
            break;
        case 3:
            PerformClick(mButton3);
            break;
        case 4:
            PerformClick(mButton4);
            break;
        case 5:
            PerformClick(mButton5);
            break;
        case 6:
            PerformClick(mButton6);
            break;
        case 7:
            PerformClick(mButton7);
            break;
        case 8:
            PerformClick(mButton8);
            break;
        case 9:
            PerformClick(mButton9);
            break;
    }
}

ECode KeyguardPinBasedInputView::ResetPasswordText(
    /* [in] */ Boolean animate)
{
    return mPasswordEntry->Reset(animate);
}

ECode KeyguardPinBasedInputView::GetPasswordText(
    /* [out] */ String* text)
{
    return mPasswordEntry->GetText(text);
}

ECode KeyguardPinBasedInputView::OnFinishInflate()
{
    Int32 id;
    GetPasswordTextViewId(&id);
    AutoPtr<IView> view;
    FindViewById(id, (IView**)&view);
    mPasswordEntry = IPasswordTextView::Probe(view);
    IView::Probe(mPasswordEntry)->SetOnKeyListener(this);

    // Set selected property on so the view can send accessibility events.
    IView::Probe(mPasswordEntry)->SetSelected(TRUE);

    // Poke the wakelock any time the text is selected or modified
    AutoPtr<IViewOnClickListener> lis = new MyOnClickListener(this);
    IView::Probe(mPasswordEntry)->SetOnClickListener(lis);

    FindViewById(R::id::key_enter, (IView**)&mOkButton);
    if (mOkButton != NULL) {
        AutoPtr<IViewOnClickListener> lis2 = new MyOnClickListener2(this);
        mOkButton->SetOnClickListener(lis2);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<LiftToActivateListener> lis = new LiftToActivateListener();
        lis->constructor(context);
        mOkButton->SetOnHoverListener(IViewOnHoverListener::Probe(lis));
    }

    FindViewById(R::id::delete_button, (IView**)&mDeleteButton);
    mDeleteButton->SetVisibility(IView::VISIBLE);
    AutoPtr<IViewOnClickListener> lis3 = new MyOnClickListener3(this);
    mDeleteButton->SetOnClickListener(lis3);
    AutoPtr<IViewOnLongClickListener> lis4  = new MyViewOnLongClickListener(this);
    mDeleteButton->SetOnLongClickListener(lis4);

    FindViewById(R::id::key0, (IView**)&mButton0);
    FindViewById(R::id::key1, (IView**)&mButton1);
    FindViewById(R::id::key2, (IView**)&mButton2);
    FindViewById(R::id::key3, (IView**)&mButton3);
    FindViewById(R::id::key4, (IView**)&mButton4);
    FindViewById(R::id::key5, (IView**)&mButton5);
    FindViewById(R::id::key6, (IView**)&mButton6);
    FindViewById(R::id::key7, (IView**)&mButton7);
    FindViewById(R::id::key8, (IView**)&mButton8);
    FindViewById(R::id::key9, (IView**)&mButton9);

    Boolean res;
    IView::Probe(mPasswordEntry)->RequestFocus(&res);
    return KeyguardAbsKeyInputView::OnFinishInflate();
}

ECode KeyguardPinBasedInputView::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Boolean res;
        OnKeyDown(keyCode, event, &res);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
