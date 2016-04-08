#include "elastos/droid/R.h"
#include "elastos/droid/widget/DialerFilter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/text/CAllCapsFilter.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/method/CTextKeyListenerHelper.h"
#include "elastos/droid/text/method/CDialerKeyListenerHelper.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::EIID_ISpannable;
using Elastos::Droid::Text::CAllCapsFilter;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::ITextKeyListenerHelper;
using Elastos::Droid::Text::Method::ITextKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::EIID_IKeyListener;
using Elastos::Droid::Text::Method::CTextKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::Widget::RelativeLayout;

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//              DialerFilter::
//=====================================================================
CAR_INTERFACE_IMPL(DialerFilter, RelativeLayout, IDialerFilter)

DialerFilter::DialerFilter()
{
}

ECode DialerFilter::constructor(
    /* [in] */ IContext* context)
{
    return RelativeLayout::constructor(context);
}

ECode DialerFilter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode DialerFilter::OnFinishInflate()
{
    View::OnFinishInflate();

    // Setup the filter view
    AutoPtr<IInputFilter> allCaps;
    FAIL_RETURN(CAllCapsFilter::New((IInputFilter**)&allCaps));
    mInputFilters = ArrayOf<IInputFilter*>::Alloc(1);
    mInputFilters->Set(0, allCaps);
    AutoPtr<IView> view;
    FindViewById(R::id::hint, (IView**)&view);

    mHint = IEditText::Probe(view);
    if(mHint == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ITextView::Probe(mHint)->SetFilters(mInputFilters);

    mLetters = mHint;
    AutoPtr<ITextKeyListenerHelper> textKeyListenerHelper;
    FAIL_RETURN(CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&textKeyListenerHelper));
    AutoPtr<ITextKeyListener> textKeyListener;
    textKeyListenerHelper->GetInstance((ITextKeyListener**)&textKeyListener);
    AutoPtr<IKeyListener> keyListener;
    keyListener = (IKeyListener*)textKeyListener->Probe(EIID_IKeyListener);
    if(keyListener == NULL){
        // assert(0 && "keyListener is not null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ITextView::Probe(mLetters)->SetKeyListener(keyListener);
    ITextView::Probe(mLetters)->SetMovementMethod(NULL);
    IView::Probe(mLetters)->SetFocusable(FALSE);

    // Setup the digits view
    FindViewById(R::id::primary, (IView**)&view);
    mPrimary = IEditText::Probe(view);
    if(mPrimary == NULL){
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ITextView::Probe(mPrimary)->SetFilters(mInputFilters);

    mDigits = mPrimary;
    AutoPtr<IDialerKeyListenerHelper> dialerKeyListenerHelper;
    FAIL_RETURN(CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&dialerKeyListenerHelper));
    AutoPtr<IDialerKeyListener> dialerKeyListener;
    dialerKeyListenerHelper->GetInstance((IDialerKeyListener**)&dialerKeyListener);
    AutoPtr<IKeyListener> keyListener1;
    keyListener1 = (IKeyListener*)dialerKeyListener->Probe(EIID_IKeyListener);
    if(keyListener1 == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ITextView::Probe(mDigits)->SetKeyListener(keyListener1);
    ITextView::Probe(mDigits)->SetMovementMethod(NULL);
    IView::Probe(mDigits)->SetFocusable(FALSE);

    // Look for an icon
    FindViewById(R::id::icon, (IView**)&view);
    mIcon = IImageView::Probe(view);

    // Setup focus & highlight for this view
    SetFocusable(TRUE);

    // XXX Force the mode to QWERTY for now, since 12-key isn't supported
    mIsQwerty = TRUE;
    SetMode(DIGITS_AND_LETTERS);
    return NOERROR;
}

void DialerFilter::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    RelativeLayout::OnFinishInflate();

    if(mIcon != NULL) {
        IView::Probe(mIcon)->SetVisibility(focused ? IView::VISIBLE : IView::GONE);
    }
}

ECode DialerFilter::IsQwertyKeyboard(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mIsQwerty;
    return NOERROR;
}

ECode DialerFilter::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean handled = FALSE;
    AutoPtr<ICharSequence> sequence;
    Boolean keyEventResult = FALSE;
    AutoPtr<IKeyEventHelper> keyEventHelper;

    IKeyEventCallback* mLettersCallback = (IKeyEventCallback*)mLetters->Probe(EIID_IKeyEventCallback);
    IKeyEventCallback* mDigitsCallback = (IKeyEventCallback*)mDigits->Probe(EIID_IKeyEventCallback);
    if (mLettersCallback == NULL || mDigitsCallback == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    switch(keyCode) {
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        case IKeyEvent::KEYCODE_ENTER:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
            break;
        case IKeyEvent::KEYCODE_DEL:
            switch(mMode) {
            case DIGITS_AND_LETTERS:
                mDigitsCallback->OnKeyDown(keyCode, event, &handled);
                mLettersCallback->OnKeyDown(keyCode, event, &keyEventResult);
                handled &= keyEventResult;
                break;

            case DIGITS_AND_LETTERS_NO_DIGITS:
                mLettersCallback->OnKeyDown(keyCode, event, &handled);
                Int32 mLetterslen, mDigitslen;
                ITextView::Probe(mLetters)->GetText((ICharSequence**)&sequence);
                sequence->GetLength(&mLetterslen);
                sequence = NULL;
                ITextView::Probe(mDigits)->GetText((ICharSequence**)&sequence);
                sequence->GetLength(&mDigitslen);
                if(mLetterslen == mDigitslen) {
                    SetMode(DIGITS_AND_LETTERS);
                }
                break;

            case DIGITS_AND_LETTERS_NO_LETTERS:
                Int32 mLetterslen1, mDigitslen1;
                sequence = NULL;
                ITextView::Probe(mLetters)->GetText((ICharSequence**)&sequence);
                sequence->GetLength(&mLetterslen1);
                sequence = NULL;
                ITextView::Probe(mDigits)->GetText((ICharSequence**)&sequence);
                sequence->GetLength(&mDigitslen1);
                if(mLetterslen1 == mDigitslen1) {
                    Boolean res = FALSE;
                    mLettersCallback->OnKeyDown(keyCode, event, &res);
                    SetMode(DIGITS_AND_LETTERS);
                }
                mDigitsCallback->OnKeyDown(keyCode, event, &handled);
                break;

            case DIGITS_ONLY:
                mDigitsCallback->OnKeyDown(keyCode, event, &handled);
                break;
            case LETTERS_ONLY:
                mLettersCallback->OnKeyDown(keyCode, event, &handled);
                break;
            }
            break;

            default:
                switch (mMode) {
                    case DIGITS_AND_LETTERS: {
                        mLettersCallback->OnKeyDown(keyCode, event, &handled);
                        Boolean result = FALSE;
                        FAIL_RETURN(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&keyEventHelper));
                        keyEventHelper->IsModifierKey(keyCode, &result);
                        if(result) {
                            Boolean discard = FALSE;
                            mDigitsCallback->OnKeyDown(keyCode, event, &discard);
                            handled = TRUE;
                            break;
                        }
                        Boolean isPrint = FALSE;
                        event->IsPrintingKey(&isPrint);
                        if(isPrint || keyCode == IKeyEvent::KEYCODE_SPACE
                                ||keyCode == IKeyEvent::KEYCODE_TAB) {
                            AutoPtr<IDialerKeyListenerHelper> helper;
                            FAIL_RETURN(CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper));
                            AutoPtr<ArrayOf<Char32> > CHARACTERS;
                            helper->GetCHARACTERS((ArrayOf<Char32>**)&CHARACTERS);
                            Char32 c;
                            event->GetMatch(CHARACTERS, &c);
                            if(c != 0) {
                                mDigitsCallback->OnKeyDown(keyCode, event, &keyEventResult);
                                handled &= keyEventResult;
                            }
                            else {
                                SetMode(DIGITS_AND_LETTERS_NO_DIGITS);
                            }
                        }
                        break;
                    }
                    case DIGITS_AND_LETTERS_NO_LETTERS:
                    case DIGITS_ONLY:
                        mDigitsCallback->OnKeyDown(keyCode, event, &handled);
                        break;

                    case DIGITS_AND_LETTERS_NO_DIGITS:
                    case LETTERS_ONLY:
                        mLettersCallback->OnKeyDown(keyCode, event, &handled);
                        break;
                }
    }

    if (!handled) {
        return View::OnKeyDown(keyCode, event, result);
    }
    else {
        *result = TRUE;
        return NOERROR;
    }
}

ECode DialerFilter::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    IKeyEventCallback* mLettersCallback = (IKeyEventCallback*)mLetters->Probe(EIID_IKeyEventCallback);
    IKeyEventCallback* mDigitsCallback = (IKeyEventCallback*)mDigits->Probe(EIID_IKeyEventCallback);
    if (mLettersCallback == NULL || mDigitsCallback == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean a = FALSE, b = FALSE;
    mLettersCallback->OnKeyDown(keyCode, event, &a);
    mDigitsCallback->OnKeyDown(keyCode, event, &b);
    *result = a || b;
    return NOERROR;
}

ECode DialerFilter::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)

    *mode = mMode;
    return NOERROR;
}

ECode DialerFilter::SetMode(
    /* [in] */ Int32 newMode)
{
    switch (newMode) {
        case DIGITS_AND_LETTERS:
            MakeDigitsPrimary();
            IView::Probe(mLetters)->SetVisibility(IView::VISIBLE);
            IView::Probe(mDigits)->SetVisibility(IView::VISIBLE);
            break;
        case DIGITS_ONLY:
            MakeDigitsPrimary();
            IView::Probe(mLetters)->SetVisibility(IView::GONE);
            IView::Probe(mDigits)->SetVisibility(IView::VISIBLE);
            break;
        case LETTERS_ONLY:
            MakeLettersPrimary();
            IView::Probe(mLetters)->SetVisibility(IView::VISIBLE);
            IView::Probe(mDigits)->SetVisibility(IView::GONE);
            break;
        case DIGITS_AND_LETTERS_NO_LETTERS:
            MakeDigitsPrimary();
            IView::Probe(mLetters)->SetVisibility(IView::INVISIBLE);
            IView::Probe(mDigits)->SetVisibility(IView::VISIBLE);
            break;
        case DIGITS_AND_LETTERS_NO_DIGITS:
            MakeLettersPrimary();
            IView::Probe(mLetters)->SetVisibility(IView::VISIBLE);
            IView::Probe(mDigits)->SetVisibility(IView::INVISIBLE);
            break;
    }
    Int32 oldMode = mMode;
    mMode = newMode;
    OnModeChange(oldMode, newMode);
    return NOERROR;
}

ECode DialerFilter::MakeLettersPrimary()
{
    if(mPrimary == mLetters) {
        SwapPrimaryAndHint(TRUE);
    }
    return NOERROR;
}

ECode DialerFilter::MakeDigitsPrimary()
{
    if(mPrimary == mLetters) {
        SwapPrimaryAndHint(FALSE);
    }
    return NOERROR;
}

ECode DialerFilter::SwapPrimaryAndHint(
    /* [in] */ Boolean makeLettersPrimary)
{
    AutoPtr<ICharSequence> csq;
    ITextView::Probe(mLetters)->GetText((ICharSequence**)&csq);
    AutoPtr<IEditable> lettersText = IEditable::Probe(csq);
    csq = NULL;
    ITextView::Probe(mDigits)->GetText((ICharSequence**)&csq);
    AutoPtr<IEditable> digitsText = IEditable::Probe(csq);
    AutoPtr<IKeyListener> lettersInput;
    ITextView::Probe(mLetters)->GetKeyListener((IKeyListener**)&lettersInput);
    AutoPtr<IKeyListener> digitsInput;
    ITextView::Probe(mDigits)->GetKeyListener((IKeyListener**)&digitsInput);
    if(makeLettersPrimary) {
        mLetters = mPrimary;
        mDigits = mHint;
    }
    else {
        mLetters = mHint;
        mDigits = mPrimary;
    }

    ITextView::Probe(mLetters)->SetKeyListener(lettersInput);
    ITextView::Probe(mLetters)->SetText(ICharSequence::Probe(lettersText));
    csq = NULL;
    ITextView::Probe(mLetters)->GetText((ICharSequence**)&csq);
    lettersText = IEditable::Probe(csq);

    Int32 len = 0;
    ITextView::Probe(lettersText)->GetLength(&len);
    Selection::SetSelection(ISpannable::Probe(lettersText), len);

    ITextView::Probe(mDigits)->SetKeyListener(digitsInput);
    ITextView::Probe(mDigits)->SetText(ICharSequence::Probe(digitsText));
    csq = NULL;
    ITextView::Probe(mDigits)->GetText((ICharSequence**)&csq);
    digitsText = IEditable::Probe(csq);
    ITextView::Probe(digitsText)->GetLength(&len);
    Selection::SetSelection(ISpannable::Probe(digitsText), len);

    ITextView::Probe(mPrimary)->SetFilters(mInputFilters);
    ITextView::Probe(mHint)->SetFilters(mInputFilters);

    return NOERROR;
}

ECode DialerFilter::GetLetters(
    /* [out] */ ICharSequence** letters)
{
    VALIDATE_NOT_NULL(letters)

    Int32 isVisibility = 0;
    IView::Probe(mLetters)->GetVisibility(&isVisibility);
    if(isVisibility == IView::VISIBLE) {
        AutoPtr<ICharSequence> csq;
        ITextView::Probe(mLetters)->GetText((ICharSequence**)&csq);
        *letters = csq;
        REFCOUNT_ADD(*letters)
        return NOERROR;
    }
    else {
        AutoPtr<ICharSequence> csq;
        CString::New(String(""), (ICharSequence**)&csq);
        *letters = csq;
        REFCOUNT_ADD(*letters)
        return NOERROR;
    }
}

ECode DialerFilter::GetDigits(
    /* [out] */ ICharSequence** digits)
{
    VALIDATE_NOT_NULL(digits)

    Int32 isVisibility = 0;
    IView::Probe(mDigits)->GetVisibility(&isVisibility);
    if(isVisibility == IView::VISIBLE) {
        AutoPtr<ICharSequence> csq;
        ITextView::Probe(mDigits)->GetText((ICharSequence**)&csq);
        *digits = csq;
        REFCOUNT_ADD(*digits)
        return NOERROR;
    }
    else {
        AutoPtr<ICharSequence> csq;
        CString::New(String(""), (ICharSequence**)&csq);
        *digits = csq;
        REFCOUNT_ADD(*digits)
        return NOERROR;
    }
}

ECode DialerFilter::GetFilterText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    if(mMode != DIGITS_ONLY) {
        return GetLetters(text);
    }
    else {
        return GetDigits(text);
    }
}

ECode DialerFilter::Append(
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> csq;
    FAIL_RETURN(CString::New(text, (ICharSequence**)&csq));

    switch (mMode) {
        case DIGITS_AND_LETTERS:
            ITextView::Probe(mDigits)->Append((ICharSequence*)csq);
            ITextView::Probe(mLetters)->Append((ICharSequence*)csq);
            break;
        case DIGITS_AND_LETTERS_NO_LETTERS:
        case DIGITS_ONLY:
            ITextView::Probe(mDigits)->Append((ICharSequence*)csq);
            break;
        case DIGITS_AND_LETTERS_NO_DIGITS:
        case LETTERS_ONLY:
            ITextView::Probe(mLetters)->Append((ICharSequence*)csq);
            break;
    }
    return NOERROR;
}

ECode DialerFilter::ClearText()
{
    AutoPtr<ICharSequence> csq;
    ITextView::Probe(mLetters)->GetText((ICharSequence**)&csq);
    AutoPtr<IEditable> text = IEditable::Probe(csq);
    if (text) text->Clear();

    csq = NULL;
    ITextView::Probe(mDigits)->GetText((ICharSequence**)&csq);
    text = IEditable::Probe(csq);
    if (text) text->Clear();

    if(mIsQwerty) {
        SetMode(DIGITS_AND_LETTERS);
    }
    else {
        SetMode(DIGITS_ONLY);
    }
    return NOERROR;
}

ECode DialerFilter::SetLettersWatcher(
    /* [in] */ ITextWatcher* watcher)
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mLetters)->GetText((ICharSequence**)&text);
    AutoPtr<ISpannable> span = ISpannable::Probe(text);
    if (span == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 length = 0;
    text->GetLength(&length);
    span->SetSpan(watcher, 0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    return NOERROR;
}

ECode DialerFilter::SetDigitsWatcher(
    /* [in] */ ITextWatcher* watcher)
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mDigits)->GetText((ICharSequence**)&text);
    AutoPtr<ISpannable> span = ISpannable::Probe(text);
    if (span == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 length = 0;
    text->GetLength(&length);
    span->SetSpan(watcher, 0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
    return NOERROR;
}

ECode DialerFilter::SetFilterWatcher(
    /* [in] */ ITextWatcher* watcher)
{
    if(mMode != DIGITS_ONLY) {
        SetLettersWatcher(watcher);
    }
    else {
        SetDigitsWatcher(watcher);
    }
    return NOERROR;
}

ECode DialerFilter::RemoveFilterWatcher(
    /* [in] */  ITextWatcher* watcher)
{
    AutoPtr<ICharSequence> text;
    if(mMode != DIGITS_ONLY) {
        ITextView::Probe(mLetters)->GetText((ICharSequence**)&text);
    }
    else {
        ITextView::Probe(mDigits)->GetText((ICharSequence**)&text);
    }
    ISpannable::Probe(text)->RemoveSpan(watcher);
    return NOERROR;
}

ECode DialerFilter::OnModeChange(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode)
{
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

