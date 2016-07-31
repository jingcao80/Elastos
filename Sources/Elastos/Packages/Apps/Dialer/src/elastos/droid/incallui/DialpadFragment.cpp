
#include "elastos/droid/incallui/DialpadFragment.h"
#include "elastos/droid/incallui/DialpadPresenter.h"
#include "elastos/droid/os/Looper.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::InCallUI::EIID_IUi;
using Elastos::Droid::InCallUI::EIID_IDialpadUi;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnHoverListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
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

ECode DialpadFragment::DTMFKeyListener:: Backspace(
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

ECode DialpadFragment::DTMFKeyListener:: OnKeyDown(
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

String DialpadFragment::GetDtmfText()
{
    assert(0 && "TODO");
    return String(NULL);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
