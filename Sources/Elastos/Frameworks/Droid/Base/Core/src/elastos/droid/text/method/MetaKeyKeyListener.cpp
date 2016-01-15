
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"

using Elastos::Droid::View::IKeyCharacterMap;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

// These bits are privately used by the meta key key listener.
// They are deliberately assigned values outside of the representable range of an 'Int32'
// so as not to conflict with any meta key states publicly defined by KeyEvent.
const Int64 MetaKeyKeyListener::META_CAP_USED = 1L << 32;
const Int64 MetaKeyKeyListener::META_ALT_USED = 1L << 33;
const Int64 MetaKeyKeyListener::META_SYM_USED = 1L << 34;

const Int64 MetaKeyKeyListener::META_CAP_PRESSED = 1L << 40;
const Int64 MetaKeyKeyListener::META_ALT_PRESSED = 1L << 41;
const Int64 MetaKeyKeyListener::META_SYM_PRESSED = 1L << 42;

const Int64 MetaKeyKeyListener::META_CAP_RELEASED = 1L << 48;
const Int64 MetaKeyKeyListener::META_ALT_RELEASED = 1L << 49;
const Int64 MetaKeyKeyListener::META_SYM_RELEASED = 1L << 50;

const Int64 MetaKeyKeyListener::META_SHIFT_MASK = IMetaKeyKeyListener::META_SHIFT_ON | IMetaKeyKeyListener::META_CAP_LOCKED | META_CAP_USED | META_CAP_PRESSED | META_CAP_RELEASED;
const Int64 MetaKeyKeyListener::META_ALT_MASK = IMetaKeyKeyListener::META_ALT_ON | IMetaKeyKeyListener::META_ALT_LOCKED | META_ALT_USED | META_ALT_PRESSED | META_ALT_RELEASED;
const Int64 MetaKeyKeyListener::META_SYM_MASK = IMetaKeyKeyListener::META_SYM_ON | IMetaKeyKeyListener::META_SYM_LOCKED | META_SYM_USED | META_SYM_PRESSED | META_SYM_RELEASED;

const AutoPtr<IInterface> MetaKeyKeyListener::CAP /*= MetaKeyKeyListener::NewNoCopySpan()*/;// = new NoCopySpan.Concrete();
const AutoPtr<IInterface> MetaKeyKeyListener::ALT /*= MetaKeyKeyListener::NewNoCopySpan()*/;// = new NoCopySpan.Concrete();
const AutoPtr<IInterface> MetaKeyKeyListener::SYM /*= MetaKeyKeyListener::NewNoCopySpan()*/;// = new NoCopySpan.Concrete();
const AutoPtr<IInterface> MetaKeyKeyListener::SELECTING /*= MetaKeyKeyListener::NewNoCopySpan()*/;// = new NoCopySpan.Concrete();

/**
 * The meta key has been pressed but has not yet been used.
 */
const Int32 MetaKeyKeyListener::PRESSED = ISpanned::SPAN_MARK_MARK | (1 << ISpanned::SPAN_USER_SHIFT);

/**
 * The meta key has been pressed and released but has still
 * not yet been used.
 */
const Int32 MetaKeyKeyListener::RELEASED = ISpanned::SPAN_MARK_MARK | (2 << ISpanned::SPAN_USER_SHIFT);

/**
 * The meta key has been pressed and used but has not yet been released.
 */
const Int32 MetaKeyKeyListener::USED = ISpanned::SPAN_MARK_MARK | (3 << ISpanned::SPAN_USER_SHIFT);

/**
 * The meta key has been pressed and released without use, and then
 * pressed again; it may also have been released again.
 */
const Int32 MetaKeyKeyListener::LOCKED = ISpanned::SPAN_MARK_MARK | (4 << ISpanned::SPAN_USER_SHIFT);

const Int32 MetaKeyKeyListener::PRESSED_RETURN_VALUE = 1;

const Int32 MetaKeyKeyListener::LOCKED_RETURN_VALUE = 2;

CAR_INTERFACE_IMPL(MetaKeyKeyListener, Object, IMetaKeyKeyListener)

// AutoPtr<IInterface> MetaKeyKeyListener::NewNoCopySpan()
// {
// #ifdef DROID_CORE
//     AutoPtr<CNoCopySpan> noCopySpan;
//     CNoCopySpan::NewByFriend((CNoCopySpan**)&noCopySpan);
// #else
//     AutoPtr<INoCopySpan> noCopySpan;
//     CNoCopySpan::New((INoCopySpan**)&noCopySpan);
// #endif
//     return noCopySpan->Probe(EIID_IInterface);
// }
MetaKeyKeyListener::MetaKeyKeyListener()
{}

MetaKeyKeyListener::~MetaKeyKeyListener()
{}

ECode MetaKeyKeyListener::ResetMetaState(
    /* [in] */ ISpannable* text)
{
    text->RemoveSpan(CAP);
    text->RemoveSpan(ALT);
    text->RemoveSpan(SYM);
    text->RemoveSpan(SELECTING);
    return NOERROR;
}

Int32 MetaKeyKeyListener::GetActive(
    /* [in] */ ICharSequence* text,
    /* [in] */ IInterface* meta,
    /* [in] */ Int32 on,
    /* [in] */ Int32 lock)
{
    if (!(ISpanned::Probe(text))) {
        return 0;
    }

    AutoPtr<ISpanned> sp = ISpanned::Probe(text);
    Int32 flag;
    sp->GetSpanFlags(meta, &flag);

    if (flag == LOCKED) {
        return lock;
    }
    else if (flag != 0) {
        return on;
    }
    else {
        return 0;
    }
}

ECode MetaKeyKeyListener::AdjustMetaAfterKeypress(
    /* [in] */ ISpannable* content)
{
    Adjust(content, CAP);
    Adjust(content, ALT);
    Adjust(content, SYM);
    return NOERROR;
}

ECode MetaKeyKeyListener::IsMetaTracker(
    /* [in] */ ICharSequence* text,
    /* [in] */ IInterface* what,
    /* [out] */ Boolean* ret)
{
    *ret = what == CAP || what == ALT || what == SYM ||
           what == SELECTING;
    return NOERROR;
}

ECode MetaKeyKeyListener::IsSelectingMetaTracker(
    /* [in] */ ICharSequence* text,
    /* [in] */ IInterface* what,
    /* [out] */ Boolean* ret)
{
    *ret = what == SELECTING;
    return NOERROR;
}

void MetaKeyKeyListener::Adjust(
    /* [in] */ ISpannable* content,
    /* [in] */ IInterface* what)
{
    Int32 current;
    ISpanned::Probe(content)->GetSpanFlags(what, &current);

    if (current == PRESSED) {
        content->SetSpan(what, 0, 0, USED);
    }
    else if (current == RELEASED) {
        content->RemoveSpan(what);
    }
}

ECode MetaKeyKeyListener::ResetLockedMeta(
    /* [in] */ ISpannable* content)
{
    ResetLock(content, CAP);
    ResetLock(content, ALT);
    ResetLock(content, SYM);
    ResetLock(content, SELECTING);
    return NOERROR;
}

void MetaKeyKeyListener::ResetLock(
    /* [in] */ ISpannable* content,
    /* [in] */ IInterface* what)
{
    Int32 current;
    ISpanned::Probe(content)->GetSpanFlags(what, &current);

    if (current == LOCKED)
        content->RemoveSpan(what);
}

ECode MetaKeyKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    if (keyCode == IKeyEvent::KEYCODE_SHIFT_LEFT || keyCode == IKeyEvent::KEYCODE_SHIFT_RIGHT) {
        Press(content, CAP);
        *ret = TRUE;
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_ALT_LEFT || keyCode == IKeyEvent::KEYCODE_ALT_RIGHT
            || keyCode == IKeyEvent::KEYCODE_NUM) {
        Press(content, ALT);
        *ret = TRUE;
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_SYM) {
        Press(content, SYM);
        *ret = TRUE;
        return NOERROR;
    }

    *ret = FALSE;
    return NOERROR; // no super to call through to
}

void MetaKeyKeyListener::Press(
    /* [in] */ IEditable* content,
    /* [in] */ IInterface* what)
{
    Int32 state;
    ISpanned::Probe(content)->GetSpanFlags(what, &state);

    if (state == PRESSED) {
        ; // repeat before use
    }
    else if (state == RELEASED) {
        ISpannable::Probe(content)->SetSpan(what, 0, 0, LOCKED);
    }
    else if (state == USED) {
        ; // repeat after use
    }
    else if (state == LOCKED) {
        ISpannable::Probe(content)->RemoveSpan(what);
    }
    else {
        ISpannable::Probe(content)->SetSpan(what, 0, 0, PRESSED);
    }
}

ECode MetaKeyKeyListener::StartSelecting(
    /* [in] */ IView* view,
    /* [in] */ ISpannable* content)
{
    return content->SetSpan(SELECTING, 0, 0, PRESSED);
}

ECode MetaKeyKeyListener::StopSelecting(
    /* [in] */ IView* view,
    /* [in] */ ISpannable* content)
{
    return content->RemoveSpan(SELECTING);
}

ECode MetaKeyKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    if (keyCode == IKeyEvent::KEYCODE_SHIFT_LEFT || keyCode == IKeyEvent::KEYCODE_SHIFT_RIGHT) {
        ReleaseInternal(content, CAP, event);
        *ret = TRUE;
        return NOERROR;    }

    if (keyCode == IKeyEvent::KEYCODE_ALT_LEFT || keyCode == IKeyEvent::KEYCODE_ALT_RIGHT
            || keyCode == IKeyEvent::KEYCODE_NUM) {
        ReleaseInternal(content, ALT, event);
        *ret = TRUE;
        return NOERROR;    }

    if (keyCode == IKeyEvent::KEYCODE_SYM) {
        ReleaseInternal(content, SYM, event);
        *ret = TRUE;
        return NOERROR;    }

    *ret = FALSE; // no super to call through to
    return NOERROR;
}

void MetaKeyKeyListener::ReleaseInternal(
    /* [in] */ IEditable* content,
    /* [in] */ IInterface* what,
    /* [in] */ IKeyEvent* event)
{
    assert(content != 0);
    assert(event != 0);
    Int32 current;
    ISpanned::Probe(content)->GetSpanFlags(what, &current);

    AutoPtr<IKeyCharacterMap> kcm;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
    Int32 modifierBehavior;
    kcm->GetModifierBehavior(&modifierBehavior);

    switch (modifierBehavior) {
    case IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED_OR_TOGGLED: {
        if (current == USED) {
            ISpannable::Probe(content)->RemoveSpan(what);
        }
        else if (current == PRESSED) {
            ISpannable::Probe(content)->SetSpan(what, 0, 0, RELEASED);
        }
        break;
    }
    default:
        ISpannable::Probe(content)->RemoveSpan(what);
        break;
    }
}

ECode MetaKeyKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return ClearMetaKeyState(content, states);
}

ECode MetaKeyKeyListener::ClearMetaKeyState(
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    if ((states & IMetaKeyKeyListener::META_SHIFT_ON) != 0) {
        return ISpannable::Probe(content)->RemoveSpan(CAP);
    }
    if ((states & IMetaKeyKeyListener::META_ALT_ON) != 0) {
        return ISpannable::Probe(content)->RemoveSpan(ALT);
    }
    if ((states & IMetaKeyKeyListener::META_SYM_ON) != 0) {
        return ISpannable::Probe(content)->RemoveSpan(SYM);
    }
    if ((states & IMetaKeyKeyListener::META_SELECTING) != 0) {
        return ISpannable::Probe(content)->RemoveSpan(SELECTING);
    }
}

ECode MetaKeyKeyListener::ResetLockedMeta(
    /* [in] */ Int64 state,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    if ((state & IMetaKeyKeyListener::META_CAP_LOCKED) != 0) {
        state &= ~META_SHIFT_MASK;
    }
    if ((state & IMetaKeyKeyListener::META_ALT_LOCKED) != 0) {
        state &= ~META_ALT_MASK;
    }
    if ((state & IMetaKeyKeyListener::META_SYM_LOCKED) != 0) {
        state &= ~META_SYM_MASK;
    }

    *ret = state;
    return NOERROR;
}

ECode MetaKeyKeyListener::GetMetaState(
    /* [in] */ Int64 state,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 result = 0;

    if ((state & IMetaKeyKeyListener::META_CAP_LOCKED) != 0) {
        result |= IMetaKeyKeyListener::META_CAP_LOCKED;
    }
    else if ((state & IMetaKeyKeyListener::META_SHIFT_ON) != 0) {
        result |= IMetaKeyKeyListener::META_SHIFT_ON;
    }

    if ((state & IMetaKeyKeyListener::META_ALT_LOCKED) != 0) {
        result |= IMetaKeyKeyListener::META_ALT_LOCKED;
    }
    else if ((state & IMetaKeyKeyListener::META_ALT_ON) != 0) {
        result |= IMetaKeyKeyListener::META_ALT_ON;
    }

    if ((state & IMetaKeyKeyListener::META_SYM_LOCKED) != 0) {
        result |= IMetaKeyKeyListener::META_SYM_LOCKED;
    }
    else if ((state & IMetaKeyKeyListener::META_SYM_ON) != 0) {
        result |= IMetaKeyKeyListener::META_SYM_ON;
    }

    *ret = result;
    return NOERROR;
}

ECode MetaKeyKeyListener::GetMetaState(
    /* [in] */ Int64 state,
    /* [in] */ Int32 meta,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    switch (meta) {
        case IMetaKeyKeyListener::META_SHIFT_ON:
            if ((state & IMetaKeyKeyListener::META_CAP_LOCKED) != 0) return MetaKeyKeyListener::LOCKED_RETURN_VALUE;
            if ((state & IMetaKeyKeyListener::META_SHIFT_ON) != 0) return MetaKeyKeyListener::PRESSED_RETURN_VALUE;
            *ret = 0;
            return NOERROR;

        case IMetaKeyKeyListener::META_ALT_ON:
            if ((state & IMetaKeyKeyListener::META_ALT_LOCKED) != 0) return MetaKeyKeyListener::LOCKED_RETURN_VALUE;
            if ((state & IMetaKeyKeyListener::META_ALT_ON) != 0) return MetaKeyKeyListener::PRESSED_RETURN_VALUE;
            *ret = 0;
            return NOERROR;

        case IMetaKeyKeyListener::META_SYM_ON:
            if ((state & IMetaKeyKeyListener::META_SYM_LOCKED) != 0) return MetaKeyKeyListener::LOCKED_RETURN_VALUE;
            if ((state & IMetaKeyKeyListener::META_SYM_ON) != 0) return MetaKeyKeyListener::PRESSED_RETURN_VALUE;
            *ret = 0;
            return NOERROR;

        default:
            *ret = 0;
            return NOERROR;
    }
}

ECode MetaKeyKeyListener::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    VALIDATE_NOT_NULL(event)
    Int32 metaState;
    event->GetMetaState(&metaState);
    AutoPtr<IKeyCharacterMap> keyCharacterMap;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&keyCharacterMap);
    Int32 behavior;
    keyCharacterMap->GetModifierBehavior(&behavior);
    if (behavior == IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED_OR_TOGGLED) {
            Int32 tmpOut;
            GetMetaState(text, &tmpOut);
            metaState |= tmpOut;
    }
    *ret = metaState;
    return NOERROR;
}

ECode MetaKeyKeyListener::GetMetaState(
       /* [in] */ ICharSequence* text,
       /* [in] */ Int32 meta,
       /* [in] */ IKeyEvent* event,
       /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 metaState;
    event->GetMetaState(&metaState);
    AutoPtr<IKeyCharacterMap> kcm;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
    Int32 behavior;
    if ((kcm->GetModifierBehavior(&behavior), behavior)
            == IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED_OR_TOGGLED) {
        Int32 tmpOut;
        GetMetaState(text, &tmpOut);
        metaState |= tmpOut;
    }
    if (IMetaKeyKeyListener::META_SELECTING == meta) {
        // #getMetaState(long, int) does not support META_SELECTING, but we want the same
        // behavior as #getMetaState(CharSequence, int) so we need to do it here
        if ((metaState & IMetaKeyKeyListener::META_SELECTING) != 0) {
            // META_SELECTING is only ever set to PRESSED and can't be LOCKED, so return 1
            *ret = 1;
            return NOERROR;
        }
        *ret = 0;
        return NOERROR;
    }
    return GetMetaState((Int64)metaState, meta, ret);
}

ECode MetaKeyKeyListener::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = GetActive(text, CAP, IMetaKeyKeyListener::META_SHIFT_ON, IMetaKeyKeyListener::META_CAP_LOCKED) |
           GetActive(text, ALT, IMetaKeyKeyListener::META_ALT_ON, IMetaKeyKeyListener::META_ALT_LOCKED) |
           GetActive(text, SYM, IMetaKeyKeyListener::META_SYM_ON, IMetaKeyKeyListener::META_SYM_LOCKED) |
           GetActive(text, SELECTING, IMetaKeyKeyListener::META_SELECTING, IMetaKeyKeyListener::META_SELECTING);
    return NOERROR;
}

ECode MetaKeyKeyListener::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 meta,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    switch (meta) {
        case IMetaKeyKeyListener::META_SHIFT_ON:
            *ret =  GetActive(text, CAP, MetaKeyKeyListener::PRESSED_RETURN_VALUE, MetaKeyKeyListener::LOCKED_RETURN_VALUE);
            return NOERROR;
        case IMetaKeyKeyListener::META_ALT_ON:
            *ret =  GetActive(text, ALT, MetaKeyKeyListener::PRESSED_RETURN_VALUE, MetaKeyKeyListener::LOCKED_RETURN_VALUE);
            return NOERROR;
        case IMetaKeyKeyListener::META_SYM_ON:
            *ret =  GetActive(text, SYM, MetaKeyKeyListener::PRESSED_RETURN_VALUE, MetaKeyKeyListener::LOCKED_RETURN_VALUE);
            return NOERROR;
        case IMetaKeyKeyListener::META_SELECTING:
            *ret =  GetActive(text, SELECTING, MetaKeyKeyListener::PRESSED_RETURN_VALUE, MetaKeyKeyListener::LOCKED_RETURN_VALUE);
            return NOERROR;
        default:
            *ret =  0;
            return NOERROR;
        }
}

ECode MetaKeyKeyListener::AdjustMetaAfterKeypress(
    /* [in] */ Int64 state,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    if ((state & META_CAP_PRESSED) != 0) {
        state = (state & ~META_SHIFT_MASK) | IMetaKeyKeyListener::META_SHIFT_ON | META_CAP_USED;
    }
    else if ((state & META_CAP_RELEASED) != 0) {
        state &= ~META_SHIFT_MASK;
    }

    if ((state & META_ALT_PRESSED) != 0) {
        state = (state & ~META_ALT_MASK) | META_ALT_ON | META_ALT_USED;
    } else if ((state & META_ALT_RELEASED) != 0) {
        state &= ~META_ALT_MASK;
    }

    if ((state & META_SYM_PRESSED) != 0) {
        state = (state & ~META_SYM_MASK) | IMetaKeyKeyListener::META_SYM_ON | META_SYM_USED;
    }
    else if ((state & META_SYM_RELEASED) != 0) {
        state &= ~META_SYM_MASK;
    }
    *ret = state;
    return NOERROR;
}

ECode MetaKeyKeyListener::HandleKeyDown(
    /* [in] */ Int64 state,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    if (keyCode == IKeyEvent::KEYCODE_SHIFT_LEFT || keyCode == IKeyEvent::KEYCODE_SHIFT_RIGHT) {
        *ret = Press(state, IMetaKeyKeyListener::META_SHIFT_ON, META_SHIFT_MASK,
                IMetaKeyKeyListener::META_CAP_LOCKED, META_CAP_PRESSED, META_CAP_RELEASED, META_CAP_USED);
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_ALT_LEFT || keyCode == IKeyEvent::KEYCODE_ALT_RIGHT
            || keyCode == IKeyEvent::KEYCODE_NUM) {
        *ret = Press(state, IMetaKeyKeyListener::META_ALT_ON, META_ALT_MASK,
                IMetaKeyKeyListener::META_ALT_LOCKED, META_ALT_PRESSED, META_ALT_RELEASED, META_ALT_USED);
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_SYM) {
        *ret = Press(state, IMetaKeyKeyListener::META_SYM_ON, META_SYM_MASK,
                IMetaKeyKeyListener::META_SYM_LOCKED, META_SYM_PRESSED, META_SYM_RELEASED, META_SYM_USED);
        return NOERROR;
    }
    *ret = state;
    return NOERROR;
}

Int64 MetaKeyKeyListener::Press(
    /* [in] */ Int64 state,
    /* [in] */ Int32 what,
    /* [in] */ Int64 mask,
    /* [in] */ Int64 locked,
    /* [in] */ Int64 pressed,
    /* [in] */ Int64 released,
    /* [in] */ Int64 used)
{
    if ((state & pressed) != 0) {
        // repeat before use
    }
    else if ((state & released) != 0) {
        state = (state &~ mask) | what | locked;
    }
    else if ((state & used) != 0) {
        // repeat after use
    }
    else if ((state & locked) != 0) {
        state &= ~mask;
    }
    else {
        state |= what | pressed;
    }
    return state;
}

ECode MetaKeyKeyListener::HandleKeyUp(
    /* [in] */ Int64 state,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int64* ret)
{
    if (keyCode == IKeyEvent::KEYCODE_SHIFT_LEFT || keyCode == IKeyEvent::KEYCODE_SHIFT_RIGHT) {
        *ret = ReleaseInternal(state, IMetaKeyKeyListener::META_SHIFT_ON, META_SHIFT_MASK,
                META_CAP_PRESSED, META_CAP_RELEASED, META_CAP_USED, event);
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_ALT_LEFT || keyCode == IKeyEvent::KEYCODE_ALT_RIGHT
            || keyCode == IKeyEvent::KEYCODE_NUM) {
        *ret = ReleaseInternal(state, IMetaKeyKeyListener::META_ALT_ON, META_ALT_MASK,
                META_ALT_PRESSED, META_ALT_RELEASED, META_ALT_USED, event);
        return NOERROR;
    }

    if (keyCode == IKeyEvent::KEYCODE_SYM) {
        *ret = ReleaseInternal(state, IMetaKeyKeyListener::META_SYM_ON, META_SYM_MASK,
                META_SYM_PRESSED, META_SYM_RELEASED, META_SYM_USED, event);
        return NOERROR;
    }

    *ret = state;
    return NOERROR;
}


Int64 MetaKeyKeyListener::ReleaseInternal(
    /* [in] */ Int64 state,
    /* [in] */ Int32 what,
    /* [in] */ Int64 mask,
    /* [in] */ Int64 pressed,
    /* [in] */ Int64 released,
    /* [in] */ Int64 used,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IKeyCharacterMap> kcm;
    event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
    Int32 modifierBehavior;
    kcm->GetModifierBehavior(&modifierBehavior);
    switch (modifierBehavior) {
        case IKeyCharacterMap::MODIFIER_BEHAVIOR_CHORDED_OR_TOGGLED:
            if ((state & used) != 0) {
                state &= ~mask;
            }
            else if ((state & pressed) != 0) {
                state |= what | released;
            }
            break;

        default:
            state &= ~mask;
            break;
    }
    return state;
}

ECode MetaKeyKeyListener::ClearMetaKeyState(
    /* [in] */ Int64 state,
    /* [in] */ Int32 which,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    if ((which & IMetaKeyKeyListener::META_SHIFT_ON) != 0 && (state & IMetaKeyKeyListener::META_CAP_LOCKED) != 0) {
        state &= ~META_SHIFT_MASK;
    }
    if ((which & IMetaKeyKeyListener::META_ALT_ON) != 0 && (state & IMetaKeyKeyListener::META_ALT_LOCKED) != 0) {
        state &= ~META_ALT_MASK;
    }
    if ((which & IMetaKeyKeyListener::META_SYM_ON) != 0 && (state & IMetaKeyKeyListener::META_SYM_LOCKED) != 0) {
        state &= ~META_SYM_MASK;
    }
    *ret = state;
    return NOERROR;
}


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
