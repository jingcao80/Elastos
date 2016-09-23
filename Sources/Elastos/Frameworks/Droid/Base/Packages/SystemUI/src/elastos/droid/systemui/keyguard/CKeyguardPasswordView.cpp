
#include "elastos/droid/systemui/keyguard/CKeyguardPasswordView.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/CoreUtils.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListenerHelper;
using Elastos::Droid::Text::Method::CTextKeyListenerHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(CKeyguardPasswordView::PasswordListener, Object,
    IOnEditorActionListener, ITextWatcher)

CKeyguardPasswordView::PasswordListener::PasswordListener(
    /* [in] */ CKeyguardPasswordView* host)
    : mHost(host)
{}

ECode CKeyguardPasswordView::PasswordListener:: OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode CKeyguardPasswordView::PasswordListener:: BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CKeyguardPasswordView::PasswordListener:: AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

ECode CKeyguardPasswordView::PasswordListener:: OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}


ECode CKeyguardPasswordView::MyRunnable::Run()
{
    Boolean res;
    IView::Probe(mHost->mPasswordEntry)->RequestFocus(&res);
    if (mReason != IKeyguardSecurityView::SCREEN_ON || mHost->mShowImeAtScreenOn) {
        mHost->mImm->ShowSoftInput(IView::Probe(mHost->mPasswordEntry),
                IInputMethodManager::SHOW_IMPLICIT, &res);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardPasswordView::MyOnClickListener, Object, IViewOnClickListener)

ECode CKeyguardPasswordView::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mCallback->UserActivity();
}

CAR_INTERFACE_IMPL(CKeyguardPasswordView::MyTextWatcher, Object, ITextWatcher)

ECode CKeyguardPasswordView::MyTextWatcher::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

ECode CKeyguardPasswordView::MyTextWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CKeyguardPasswordView::MyTextWatcher::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    if (mHost->mCallback != NULL) {
        mHost->mCallback->UserActivity();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardPasswordView::MyOnClickListener2,
        Object, IViewOnClickListener)

ECode CKeyguardPasswordView::MyOnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    mHost->mCallback->UserActivity(); // Leave the screen on a bit longer
    return mHost->mImm->ShowInputMethodPicker();
}

CAR_OBJECT_IMPL(CKeyguardPasswordView)

CKeyguardPasswordView::CKeyguardPasswordView()
    : mShowImeAtScreenOn(FALSE)
    , mDisappearYTranslation(0)
{
}

ECode CKeyguardPasswordView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardPasswordView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    KeyguardAbsKeyInputView::constructor(context, attrs);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::kg_show_ime_at_screen_on, &mShowImeAtScreenOn);

    AutoPtr<IResources> resources2;
    GetResources((IResources**)&resources2);
    resources2->GetDimensionPixelSize(R::dimen::disappear_y_translation, &mDisappearYTranslation);

    AutoPtr<IAnimationUtils> helper;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
    helper->LoadInterpolator(context, Elastos::Droid::R::interpolator::linear_out_slow_in,
            (IInterpolator**)&mLinearOutSlowInInterpolator);
    return helper->LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_linear_in,
            (IInterpolator**)&mFastOutLinearInInterpolator);
}

ECode CKeyguardPasswordView::ResetState()
{
    mSecurityMessageDisplay->SetMessage(R::string::kg_password_instructions,
            FALSE);
    return IView::Probe(mPasswordEntry)->SetEnabled(TRUE);
}

ECode CKeyguardPasswordView::GetPasswordTextViewId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = R::id::passwordEntry;
    return NOERROR;
}

ECode CKeyguardPasswordView::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode CKeyguardPasswordView::OnResume(
    /* [in] */ Int32 reason)
{
    KeyguardAbsKeyInputView::OnResume(reason);

    // Wait a bit to focus the field so the focusable flag on the window is already set then.
    AutoPtr<IRunnable> r = new MyRunnable(this, reason);
    Boolean res;
    return Post(r, &res);
}

ECode CKeyguardPasswordView::OnPause()
{
    KeyguardAbsKeyInputView::OnPause();

    AutoPtr<IBinder> res;
    GetWindowToken((IBinder**)&res);
    Boolean tmp;
    return mImm->HideSoftInputFromWindow(res, 0, &tmp);
}

ECode CKeyguardPasswordView::Reset()
{
    KeyguardAbsKeyInputView::Reset();
    Boolean res;
    return IView::Probe(mPasswordEntry)->RequestFocus(&res);
}

ECode CKeyguardPasswordView::OnFinishInflate()
{
    KeyguardAbsKeyInputView::OnFinishInflate();

    Boolean imeOrDeleteButtonVisible = FALSE;

    AutoPtr<PasswordListener> listener = new PasswordListener(this);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mImm = IInputMethodManager::Probe(obj);

    Int32 id;
    GetPasswordTextViewId(&id);
    AutoPtr<IView> view;
    FindViewById(id, (IView**)&view);
    mPasswordEntry = ITextView::Probe(view);

    AutoPtr<ITextKeyListenerHelper> helper;
    CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&helper);
    AutoPtr<ITextKeyListener> ret;
    helper->GetInstance((ITextKeyListener**)&ret);
    mPasswordEntry->SetKeyListener(IKeyListener::Probe(ret));
    mPasswordEntry->SetInputType(IInputType::TYPE_CLASS_TEXT
            | IInputType::TYPE_TEXT_VARIATION_PASSWORD);
    mPasswordEntry->SetOnEditorActionListener(listener);
    mPasswordEntry->AddTextChangedListener(listener);

    // Poke the wakelock any time the text is selected or modified
    AutoPtr<IViewOnClickListener> lis = new MyOnClickListener(this);
    IView::Probe(mPasswordEntry)->SetOnClickListener(lis);

    // Set selected property on so the view can send accessibility events.
    IView::Probe(mPasswordEntry)->SetSelected(TRUE);

    AutoPtr<ITextWatcher> watcher = new MyTextWatcher(this);
    mPasswordEntry->AddTextChangedListener(watcher);

    Boolean res;
    IView::Probe(mPasswordEntry)->RequestFocus(&res);

    // If there's more than one IME, enable the IME switcher button
    AutoPtr<IView> switchImeButton;
    FindViewById(R::id::switch_ime_button, (IView**)&switchImeButton);
    if (switchImeButton != NULL && HasMultipleEnabledIMEsOrSubtypes(mImm, FALSE)) {
        switchImeButton->SetVisibility(IView::VISIBLE);
        imeOrDeleteButtonVisible = TRUE;
        AutoPtr<IViewOnClickListener> lis = new MyOnClickListener2(this);
        switchImeButton->SetOnClickListener(lis);
    }

    // If no icon is visible, reset the start margin on the password field so the text is
    // still centered.
    if (!imeOrDeleteButtonVisible) {
        AutoPtr<IViewGroupLayoutParams> params;
        IView::Probe(mPasswordEntry)->GetLayoutParams((IViewGroupLayoutParams**)&params);
        if (IViewGroupMarginLayoutParams::Probe(params) != NULL) {
            AutoPtr<IViewGroupMarginLayoutParams> mlp =
                    IViewGroupMarginLayoutParams::Probe(params);
            mlp->SetMarginStart(0);
            IView::Probe(mPasswordEntry)->SetLayoutParams(params);
        }
    }
    return NOERROR;
}

ECode CKeyguardPasswordView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // send focus to the password field
    return IView::Probe(mPasswordEntry)->RequestFocus(direction,
            previouslyFocusedRect, result);
}

ECode CKeyguardPasswordView::ResetPasswordText(
    /* [in] */ Boolean animate)
{
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
    return mPasswordEntry->SetText(cchar);
}

ECode CKeyguardPasswordView::GetPasswordText(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<ICharSequence> cchar;
    mPasswordEntry->GetText((ICharSequence**)&cchar);
    return cchar->ToString(str);
}

ECode CKeyguardPasswordView::SetPasswordEntryEnabled(
    /* [in] */ Boolean enabled)
{
    return IView::Probe(mPasswordEntry)->SetEnabled(enabled);
}

Boolean CKeyguardPasswordView::HasMultipleEnabledIMEsOrSubtypes(
    /* [in] */ IInputMethodManager* imm,
    /* [in] */ Boolean shouldIncludeAuxiliarySubtypes)
{
    AutoPtr<IList> enabledImis;
    imm->GetEnabledInputMethodList((IList**)&enabledImis);

    // Number of the filtered IMEs
    Int32 filteredImisCount = 0;

    Int32 size;
    enabledImis->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        enabledImis->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

        // We can return true immediately after we find two or more filtered IMEs.
        if (filteredImisCount > 1) return TRUE;
        AutoPtr<IList> subtypes;
        imm->GetEnabledInputMethodSubtypeList(imi, TRUE, (IList**)&subtypes);
        // IMEs that have no subtypes should be counted.
        Boolean res;
        if (subtypes->IsEmpty(&res), res) {
            ++filteredImisCount;
            continue;
        }

        Int32 auxCount = 0;
        Int32 _size;
        subtypes->GetSize(&_size);
        for (Int32 j = 0; j < _size; j++) {
            AutoPtr<IInterface> obj;
            subtypes->Get(j, (IInterface**)&obj);
            AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(obj);

            Boolean res;
            if (subtype->IsAuxiliary(&res), res) {
                ++auxCount;
            }
        }
        Int32 nonAuxCount = _size - auxCount;

        // IMEs that have one or more non-auxiliary subtypes should be counted.
        // If shouldIncludeAuxiliarySubtypes is true, IMEs that have two or more auxiliary
        // subtypes should be counted as well.
        if (nonAuxCount > 0 || (shouldIncludeAuxiliarySubtypes && auxCount > 1)) {
            ++filteredImisCount;
            continue;
        }
    }

    AutoPtr<IList> list;
    imm->GetEnabledInputMethodSubtypeList(NULL, FALSE, (IList**)&list);
    return filteredImisCount > 1
    // imm.getEnabledInputMethodSubtypeList(null, false) will return the current IME's enabled
    // input method subtype (The current IME should be LatinIME.)
            || (list->GetSize(&size), size) > 1;
}

ECode CKeyguardPasswordView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardPasswordView::GetWrongPasswordStringId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = R::string::kg_wrong_password;
    return NOERROR;
}

ECode CKeyguardPasswordView::StartAppearAnimation()
{
    SetAlpha(0.0f);
    SetTranslationY(0.0f);

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(1);
    animator->WithLayer();
    animator->SetDuration(300);
    return animator->SetInterpolator(ITimeInterpolator::Probe(mLinearOutSlowInInterpolator));
}

ECode CKeyguardPasswordView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0.0f);
    animator->TranslationY(mDisappearYTranslation);
    animator->SetInterpolator(ITimeInterpolator::Probe(mFastOutLinearInInterpolator));
    animator->SetDuration(100);
    animator->WithEndAction(finishRunnable);
    *result = TRUE;
    return NOERROR;
}

ECode CKeyguardPasswordView::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    if (mCallback != NULL) {
        mCallback->UserActivity();
    }
    return NOERROR;
}

ECode CKeyguardPasswordView::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

ECode CKeyguardPasswordView::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

ECode CKeyguardPasswordView::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Check if this was the result of hitting the enter key
    Boolean isSoftImeEvent = event == NULL
            && (actionId == IEditorInfo::IME_NULL
            || actionId == IEditorInfo::IME_ACTION_DONE
            || actionId == IEditorInfo::IME_ACTION_NEXT);

    Boolean isKeyboardEnterKey = FALSE;
    if (event != NULL) {
        Int32 keyCode;
        event->GetKeyCode(&keyCode);
        AutoPtr<IKeyEventHelper> helper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
        Boolean res;
        helper->IsConfirmKey(keyCode, &res);

        Int32 action;
        event->GetAction(&action);

        isKeyboardEnterKey = res && action == IKeyEvent::ACTION_DOWN;
    }

    if (isSoftImeEvent || isKeyboardEnterKey) {
        VerifyPasswordAndUnlock();
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
