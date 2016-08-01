
#include "elastos/droid/incallui/DialpadFragment.h"
#include "elastos/droid/os/Looper.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::InCallUI::EIID_IUi;
using Elastos::Droid::InCallUI::EIID_IDialpadUi;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Phone::Common::Dialpad::IDialpadKeyButton;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnHoverListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//==========================================================
// DialpadFragment::DTMFKeyListener
//==========================================================
static AutoPtr<ArrayOf<Char32> > InitCharacters()
{
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(12);
    (*chars)[0] = '0';
    (*chars)[1] = '1';
    (*chars)[2] = '2';
    (*chars)[3] = '3';
    (*chars)[4] = '4';
    (*chars)[5] = '5';
    (*chars)[6] = '6';
    (*chars)[7] = '7';
    (*chars)[8] = '8';
    (*chars)[9] = '9';
    (*chars)[10] = '#';
    (*chars)[11] = '*';
    return chars;
}
const AutoPtr<ArrayOf<Char32> > DialpadFragment::DTMFKeyListener::DTMF_CHARACTERS = InitCharacters();

DialpadFragment::DTMFKeyListener::DTMFKeyListener(
    /* [in] */ DialpadFragment* host)
    : mHost(host)
{
    DialerKeyListener::constructor();
}

AutoPtr<ArrayOf<Char32> > DialpadFragment::DTMFKeyListener::GetAcceptedChars()
{
    return DTMF_CHARACTERS;
}

ECode DialpadFragment::DTMFKeyListener::Backspace(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;
    return NOERROR;
}

Boolean DialpadFragment::DTMFKeyListener::IsAcceptableModifierKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_ALT_LEFT:
        case IKeyEvent::KEYCODE_ALT_RIGHT:
        case IKeyEvent::KEYCODE_SHIFT_LEFT:
        case IKeyEvent::KEYCODE_SHIFT_RIGHT:
            return TRUE;
        default:
            return FALSE;
    }
}

ECode DialpadFragment::DTMFKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    // if (DBG) log("DTMFKeyListener.onKeyDown, keyCode " + keyCode + ", view " + view);

    // find the character
    Char32 c = (Char32)Lookup(event, ISpannable::Probe(content));

    // if not a long press, and parent onKeyDown accepts the input
    Int32 count;
    Boolean result;
    if ((event->GetRepeatCount(&count), count == 0) &&
            (DialerKeyListener::OnKeyDown(view, content, keyCode, event, &result), result)) {

        Boolean keyOK = Ok(GetAcceptedChars(), c);

        // if the character is a valid dtmf code, start playing the tone and send the
        // code.
        if (keyOK) {
            Logger::D("DialpadFragment", "DTMFKeyListener reading '%d' from input.", c);
            ((DialpadPresenter*)mHost->GetPresenter().Get())->ProcessDtmf(c);
        }
        else {
            Logger::D("DialpadFragment", "DTMFKeyListener rejecting '%d' from input.", c);
        }
        return TRUE;
    }
    return FALSE;
}

ECode DialpadFragment::DTMFKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    assert(0 && "TODO");
    return NOERROR;
}

Boolean DialpadFragment::DTMFKeyListener::OnKeyDown(
    /* [in] */ IKeyEvent* event)
{
    assert(0 && "TODO");
    return FALSE;
}

Boolean DialpadFragment::DTMFKeyListener::OnKeyUp(
    /* [in] */ IKeyEvent* event)
{
    assert(0 && "TODO");
    return FALSE;
}

Boolean DialpadFragment::DTMFKeyListener::IsKeyEventAcceptable (
    /* [in] */ IKeyEvent* event)
{
    assert(0 && "TODO");
    return FALSE;
}


// ECode DialpadFragment::DTMFKeyLis;


//==========================================================
// DialpadFragment::StaticInitializer
//==========================================================
DialpadFragment::StaticInitializer::StaticInitializer()
{
    // Map the buttons to the display characters
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::one] = '1';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::two] = '2';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::three] = '3';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::four] = '4';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::five] = '5';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::six] = '6';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::seven] = '7';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::eight] = '8';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::nine] = '9';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::zero] = '0';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::pound] = '#';
    DialpadFragment::sDisplayMap[Elastos::Droid::Dialer::R::id::star] = '*';
}


//==========================================================
// DialpadFragment::StopDtmfRunnable
//==========================================================
ECode DialpadFragment::StopDtmfRunnable::Run()
{
    mPresenter->StopDtmf();
    return NOERROR;
}


//==========================================================
// DialpadFragment
//==========================================================
const Int32 DialpadFragment::ACCESSIBILITY_DTMF_STOP_DELAY_MILLIS;
HashMap<Int32, Char32> DialpadFragment::sDisplayMap;
DialpadFragment::StaticInitializer DialpadFragment::sInitializer;

static AutoPtr<IHandler> InitHandler()
{
    AutoPtr<IHandler> handler;
    CHandler::New(Looper::GetMainLooper(), (IHandler**)&handler);
    return handler;
}
const AutoPtr<IHandler> DialpadFragment::sHandler = InitHandler();

CAR_INTERFACE_IMPL_6(DialpadFragment, BaseFragment
        , IUi
        , IDialpadUi
        , IViewOnTouchListener
        , IViewOnKeyListener
        , IViewOnHoverListener
        , IViewOnClickListener)

ECode DialpadFragment::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&service);
    AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(service);
    // When accessibility is on, simulate press and release to preserve the
    // semantic meaning of performClick(). Required for Braille support.
    Boolean isEnabled;
    if (accessibilityManager->IsEnabled(&isEnabled), isEnabled) {
        Int32 id;
        v->GetId(&id);
        // Checking the press state prevents double activation.
        Boolean isPressed;
        if (v->IsPressed(&isPressed), !isPressed) {
            HashMap<Int32, Char32>::Iterator it = sDisplayMap.Find(id);
            if (it != sDisplayMap.End()) {
                AutoPtr<DialpadPresenter> presenter = (DialpadPresenter*)GetPresenter().Get();
                presenter->ProcessDtmf(it->mSecond);
                Boolean result;
                AutoPtr<IRunnable> runnable = new StopDtmfRunnable(presenter);
                sHandler->PostDelayed(runnable, ACCESSIBILITY_DTMF_STOP_DELAY_MILLIS, &result);
            }
        }
    }
    return NOERROR;
}

ECode DialpadFragment::OnHover(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* comsumed)
{
    VALIDATE_NOT_NULL(comsumed)
    // When touch exploration is turned on, lifting a finger while inside
    // the button's hover target bounds should perform a click action.
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&service);
    AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(service);

    Boolean isEnabled, isTouchEnabled;
    if ((accessibilityManager->IsEnabled(&isEnabled), isEnabled)
            && (accessibilityManager->IsTouchExplorationEnabled(&isTouchEnabled), isTouchEnabled)) {
        Int32 left;
        v->GetPaddingLeft(&left);
        Int32 width, pRight;
        v->GetWidth(&width);
        v->GetPaddingRight(&pRight);
        Int32 right = width - pRight;
        Int32 top;
        v->GetPaddingTop(&top);
        Int32 height, pBottom;
        v->GetHeight(&height);
        v->GetPaddingBottom(&pBottom);
        Int32 bottom = height - pBottom;

        Int32 masked;
        event->GetActionMasked(&masked);
        switch (masked) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                // Lift-to-type temporarily disables double-tap activation.
                v->SetClickable(FALSE);
                break;
            case IMotionEvent::ACTION_HOVER_EXIT: {
                Float fx, fy;
                event->GetX(&fx);
                event->GetY(&fy);
                Int32 x = (Int32)fx;
                Int32 y = (Int32)fy;
                if ((x > left) && (x < right) && (y > top) && (y < bottom)) {
                    Boolean result;
                    v->PerformClick(&result);
                }
                v->SetClickable(TRUE);
                break;
            }

        }
    }

    *comsumed = FALSE;
    return NOERROR;
}

ECode DialpadFragment::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::D("DialpadFragment", "onKey:  keyCode %d, view %s", keyCode, TO_CSTR(v));

    if (keyCode == IKeyEvent::KEYCODE_DPAD_CENTER) {
        Int32 viewId;
        v->GetId(&viewId);
        HashMap<Int32, Char32>::Iterator it = sDisplayMap.Find(viewId);
        if (it != sDisplayMap.End()) {
            Int32 action;
            event->GetAction(&action);
            switch (action) {
                case IKeyEvent::ACTION_DOWN: {
                    Int32 count;
                    if (event->GetRepeatCount(&count), count == 0) {
                        AutoPtr<DialpadPresenter> presenter = (DialpadPresenter*)GetPresenter().Get();
                        presenter->ProcessDtmf(it->mSecond);
                    }
                    break;
                }
                case IKeyEvent::ACTION_UP: {
                    AutoPtr<DialpadPresenter> presenter = (DialpadPresenter*)GetPresenter().Get();
                    presenter->StopDtmf();
                    break;
                }
                // do not return true [handled] here, since we want the
                // press / click animation to be handled by the framework.
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DialpadFragment::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::D("DialpadFragment", "onTouch");
    Int32 viewId;
    v->GetId(&viewId);

    // if the button is recognized
    HashMap<Int32, Char32>::Iterator it = sDisplayMap.Find(viewId);
    if (it != sDisplayMap.End()) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_DOWN: {
                // Append the character mapped to this button, to the display.
                // start the tone
                AutoPtr<DialpadPresenter> presenter = (DialpadPresenter*)GetPresenter().Get();
                presenter->ProcessDtmf(it->mSecond);
                break;
            }
            case IMotionEvent::ACTION_UP:
            case IMotionEvent::ACTION_CANCEL: {
                // stop the tone on ANY other event, except for MOVE.
                AutoPtr<DialpadPresenter> presenter = (DialpadPresenter*)GetPresenter().Get();
                presenter->StopDtmf();
                break;
            }
        }
        // do not return true [handled] here, since we want the
        // press / click animation to be handled by the framework.
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IPresenter> DialpadFragment::CreatePresenter()
{
    AutoPtr<IPresenter> p = (IPresenter*)new DialpadPresenter();
    return p;
}

AutoPtr<IUi> DialpadFragment::GetUi()
{
    return IUi::Probe(this);
}

ECode DialpadFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return BaseFragment::OnCreate(savedInstanceState);
}

ECode DialpadFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> parent;
    inflater->Inflate(
            Elastos::Droid::Dialer::R::layout::dtmf_twelve_key_dialer_view, container, FALSE, (IView**)&parent);
    AutoPtr<IView> temp;
    parent->FindViewById(Elastos::Droid::Dialer::R::id::dialpad_view, (IView**)&temp);
    mDialpadView = IDialpadView::Probe(temp);
    mDialpadView->SetCanDigitsBeEdited(FALSE);
    IView::Probe(mDialpadView)->SetBackgroundResource(Elastos::Droid::Dialer::R::color::incall_dialpad_background);
    temp = NULL;
    parent->FindViewById(Elastos::Droid::Dialer::R::id::digits, (IView**)&temp);
    mDtmfDialerField = IEditText::Probe(temp);
    if (mDtmfDialerField != NULL) {
        mDialerKeyListener = new DTMFKeyListener(this);
        AutoPtr<ITextView> tv = ITextView::Probe(mDtmfDialerField);
        tv->SetKeyListener(IKeyListener::Probe(mDialerKeyListener));
        // remove the long-press context menus that support
        // the edit (copy / paste / select) functions.
        IView::Probe(mDtmfDialerField)->SetLongClickable(FALSE);
        tv->SetElegantTextHeight(FALSE);
        ConfigureKeypadListeners(IView::Probe(mDialpadView));
    }

    *view = parent;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode DialpadFragment::OnDestroyView()
{
    mDialerKeyListener = NULL;
    return BaseFragment::OnDestroyView();
}

String DialpadFragment::GetDtmfText()
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mDtmfDialerField)->GetText((ICharSequence**)&text);
    String str;
    text->ToString(&str);
    return str;
}

void DialpadFragment::SetDtmfText(
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    ITextView::Probe(mDtmfDialerField)->SetText(cs);
}

ECode DialpadFragment::SetVisible(
    /* [in] */ Boolean on)
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    if (on) {
        v->SetVisibility(IView::VISIBLE);
    }
    else {
        v->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

void DialpadFragment::AnimateShowDialpad()
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IView> temp;
    v->FindViewById(Elastos::Droid::Dialer::R::id::dialpad_view, (IView**)&temp);
    AutoPtr<IDialpadView> dialpadView = IDialpadView::Probe(temp);
    dialpadView->AnimateShow();
}

ECode DialpadFragment::AppendDigitsToField(
    /* [in] */ Char32 digit)
{
    if (mDtmfDialerField != NULL) {
        // TODO: maybe *don't* manually append this digit if
        // mDialpadDigits is focused and this key came from the HW
        // keyboard, since in that case the EditText field will
        // get the key event directly and automatically appends
        // whetever the user types.
        // (Or, a cleaner fix would be to just make mDialpadDigits
        // *not* handle HW key presses.  That seems to be more
        // complicated than just setting focusable="false" on it,
        // though.)
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(mDtmfDialerField)->GetText((ICharSequence**)&cs);
        String str;
        cs->ToString(&str);
        str.Append(digit);
        cs = NULL;
        CString::New(str, (ICharSequence**)&cs);
        ITextView::Probe(mDtmfDialerField)->SetText(cs);
    }
    return NOERROR;
}

Boolean DialpadFragment::OnDialerKeyDown(
    /* [in] */ IKeyEvent* event)
{
    Logger::D("DialpadFragment", "Notifying dtmf key down.");
    if (mDialerKeyListener != NULL) {
        return mDialerKeyListener->OnKeyDown(event);
    }
    else {
        return FALSE;
    }
}

Boolean DialpadFragment::OnDialerKeyUp(
    /* [in] */ IKeyEvent* event)
{
    Logger::D("DialpadFragment", "Notifying dtmf key up.");
    if (mDialerKeyListener != NULL) {
        return mDialerKeyListener->OnKeyUp(event);
    }
    else {
        return FALSE;
    }
}

void DialpadFragment::ConfigureKeypadListeners(
    /* [in] */ IView* fragmentView)
{
    AutoPtr<ArrayOf<Int32> > buttonIds = ArrayOf<Int32>::Alloc(12);
    (*buttonIds)[0] = Elastos::Droid::Dialer::R::id::zero;
    (*buttonIds)[1] = Elastos::Droid::Dialer::R::id::one;
    (*buttonIds)[2] = Elastos::Droid::Dialer::R::id::two;
    (*buttonIds)[3] = Elastos::Droid::Dialer::R::id::three;
    (*buttonIds)[4] = Elastos::Droid::Dialer::R::id::four;
    (*buttonIds)[5] = Elastos::Droid::Dialer::R::id::five;
    (*buttonIds)[6] = Elastos::Droid::Dialer::R::id::six;
    (*buttonIds)[7] = Elastos::Droid::Dialer::R::id::seven;
    (*buttonIds)[8] = Elastos::Droid::Dialer::R::id::eight;
    (*buttonIds)[9] = Elastos::Droid::Dialer::R::id::nine;
    (*buttonIds)[10] = Elastos::Droid::Dialer::R::id::star;
    (*buttonIds)[11] = Elastos::Droid::Dialer::R::id::pound;
    for (Int32 i = 0; i < buttonIds->GetLength(); i++) {
        AutoPtr<IView> view;
        fragmentView->FindViewById((*buttonIds)[i], (IView**)&view);
        view->SetOnTouchListener(IViewOnTouchListener::Probe(this));
        view->SetOnKeyListener(IViewOnKeyListener::Probe(this));
        view->SetOnHoverListener(IViewOnHoverListener::Probe(this));
        view->SetOnClickListener(IViewOnClickListener::Probe(this));
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
