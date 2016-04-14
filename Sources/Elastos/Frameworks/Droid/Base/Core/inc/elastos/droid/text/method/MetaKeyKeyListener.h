#ifndef __ELASTOS_DROID_TEXT_METHOD_METAKEYKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_METAKEYKEYLISTENER_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * This base class encapsulates the behavior for tracking the state of
 * meta keys such as SHIFT, ALT and SYM as well as the pseudo-meta state of selecting text.
 * <p>
 * Key listeners that care about meta state should inherit from this class;
 * you should not instantiate this class directly in a client.
 * </p><p>
 * This class provides two mechanisms for tracking meta state that can be used
 * together or independently.
 * </p>
 * <ul>
 * <li>Methods such as {@link #handleKeyDown(Int64, Int32, KeyEvent)} and
 * {@link #getMetaState(Int64)} operate on a meta key state bit mask.</li>
 * <li>Methods such as {@link #onKeyDown(View, Editable, Int32, KeyEvent)} and
 * {@link #getMetaState(CharSequence, Int32)} operate on meta key state flags stored
 * as spans in an {@link Editable} text buffer.  The spans only describe the current
 * meta key state of the text editor; they do not carry any positional information.</li>
 * </ul>
 * <p>
 * The behavior of this class varies according to the keyboard capabilities
 * described by the {@link KeyCharacterMap} of the keyboard device such as
 * the {@link KeyCharacterMap#getModifierBehavior() key modifier behavior}.
 * </p><p>
 * {@link MetaKeyKeyListener} implements chorded and toggled key modifiers.
 * When key modifiers are toggled Int32o a latched or locked state, the state
 * of the modifier is stored in the {@link Editable} text buffer or in a
 * meta state Int32eger managed by the client.  These latched or locked modifiers
 * should be considered to be held <b>in addition to</b> those that the
 * keyboard already reported as being pressed in {@link KeyEvent#getMetaState()}.
 * In other words, the {@link MetaKeyKeyListener} augments the meta state
 * provided by the keyboard; it does not replace it.  This distinction is important
 * to ensure that meta keys not handled by {@link MetaKeyKeyListener} such as
 * {@link KeyEvent#KEYCODE_CAPS_LOCK} or {@link KeyEvent#KEYCODE_NUM_LOCK} are
 * taken Int32o consideration.
 * </p><p>
 * To ensure correct meta key behavior, the following pattern should be used
 * when mapping key codes to characters:
 * </p>
 * <code>
 * private char getUnicodeChar(TextKeyListener listener, KeyEvent event, Editable textBuffer) {
 *     // Use the combined meta states from the event and the key listener.
 *     Int32 metaState = event.getMetaState() | listener.getMetaState(textBuffer);
 *     return event.getUnicodeChar(metaState);
 * }
 * </code>
 */
//public abstract
class MetaKeyKeyListener
    : public Object
    , public IMetaKeyKeyListener
{
public:
    CAR_INTERFACE_DECL()

    MetaKeyKeyListener();

    virtual ~MetaKeyKeyListener();

    /**
     * Resets all meta state to inactive.
     */
    static CARAPI ResetMetaState(
        /* [in] */ ISpannable* text);

    /**
     * Gets the state of the meta keys.
     *
     * @param text the buffer in which the meta key would have been pressed.
     *
     * @return an Int32eger in which each bit set to one represents a pressed
     *         or locked meta key.
     */
    static /*const*/ CARAPI GetMetaState(
        /* [in] */ ICharSequence* text,
        /* [out] */ Int32* ret);

    /**
     * Gets the state of a particular meta key.
     *
     * @param meta META_SHIFT_ON, META_ALT_ON, META_SYM_ON, or META_SELECTING
     * @param text the buffer in which the meta key would have been pressed.
     *
     * @return 0 if inactive, 1 if active, 2 if locked.
     */
    static /*const*/ CARAPI GetMetaState(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 meta,
        /* [out] */ Int32* ret);

     /**
     * Gets the state of the meta keys.
     *
     * @param state the current meta state bits.
     *
     * @return an Int32eger in which each bit set to one represents a pressed
     *         or locked meta key.
     */
    static /*const*/ CARAPI GetMetaState(
        /* [in] */ Int64 state,
        /* [out] */ Int32* ret);

    /**
     * Gets the state of a particular meta key.
     *
     * @param state the current state bits.
     * @param meta META_SHIFT_ON, META_ALT_ON, or META_SYM_ON
     *
     * @return 0 if inactive, 1 if active, 2 if locked.
     */
    static /*const*/ CARAPI GetMetaState(
        /* [in] */ Int64 state,
        /* [in] */ Int32 meta,
        /* [out] */ Int32* ret);

    static /*const*/ CARAPI GetMetaState(
        /* [in] */ ICharSequence* text,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Int32* ret);

    static /*const*/ CARAPI GetMetaState(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 meta,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Int32* ret);

    /**
     * Call this method after you handle a keypress so that the meta
     * state will be reset to unshifted (if it is not still down)
     * or primed to be reset to unshifted (once it is released).
     */
    static CARAPI AdjustMetaAfterKeypress(
        /* [in] */ ISpannable* content);

    /**
     * Returns true if this object is one that this class would use to
     * keep track of any meta state in the specified text.
     */
    static CARAPI IsMetaTracker(
        /* [in] */ ICharSequence* text,
        /* [in] */ IInterface* what,
        /* [out] */ Boolean* ret);

    /**
     * Returns true if this object is one that this class would use to
     * keep track of the selecting meta state in the specified text.
     */
    static CARAPI IsSelectingMetaTracker(
        /* [in] */ ICharSequence* text,
        /* [in] */ IInterface* what,
        /* [out] */ Boolean* ret);

    /**
     * Handles presses of the meta keys.
     */
    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    /**
     * Start selecting text.
     * @hide pending API review
     */
    static CARAPI StartSelecting(
        /* [in] */ IView* view,
        /* [in] */ ISpannable* content);

    /**
     * Stop selecting text.  This does not actually collapse the selection;
     * call {@link android.text.Selection#setSelection} too.
     * @hide pending API review
     */
    static CARAPI StopSelecting(
        /* [in] */ IView* view,
        /* [in] */ ISpannable* content);

    /**
     * Handles release of the meta keys.
     */
    CARAPI OnKeyUp(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI ClearMetaKeyState(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 states);

    static CARAPI ClearMetaKeyState(
        /* [in] */ IEditable* content,
        /* [in] */ Int32 states);

    /**
     * Call this if you are a method that ignores the locked meta state
     * (arrow keys, for example) and you handle a key.
     */
    static CARAPI ResetLockedMeta(
        /* [in] */ Int64 state,
        /* [out] */ Int64* ret);

    // ---------------------------------------------------------------------
    // Version of API that operates on a state bit mask
    // ---------------------------------------------------------------------

    /**
     * Call this method after you handle a keypress so that the meta
     * state will be reset to unshifted (if it is not still down)
     * or primed to be reset to unshifted (once it is released).  Takes
     * the current state, returns the new state.
     */
    static CARAPI AdjustMetaAfterKeypress(
        /* [in] */ Int64 state,
        /* [out] */ Int64* ret);

    /**
     * Handles presses of the meta keys.
     */
    static CARAPI HandleKeyDown(
        /* [in] */ Int64 state,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Int64* ret);

    /**
     * Handles release of the meta keys.
     */
    static CARAPI HandleKeyUp(
        /* [in] */ Int64 state,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Int64* ret);

    /**
     * Clears the state of the specified meta key if it is locked.
     * @param state the meta key state
     * @param which meta keys to clear, may be a combination of {@link #META_SHIFT_ON},
     * {@link #META_ALT_ON} or {@link #META_SYM_ON}.
     */
    CARAPI ClearMetaKeyState(
        /* [in] */ Int64 state,
        /* [in] */ Int32 which,
        /* [out] */ Int64* ret);

    /**
     * Call this if you are a method that ignores the locked meta state
     * (arrow keys, for example) and you handle a key.
     */
    static CARAPI ResetLockedMeta(
        /* [in] */ ISpannable* content);

private:
    static CARAPI_(Int32) GetActive(
        /* [in] */ ICharSequence* text,
        /* [in] */ IInterface* meta,
        /* [in] */ Int32 on,
        /* [in] */ Int32 lock);

    static CARAPI_(void) Adjust(
        /* [in] */ ISpannable* content,
        /* [in] */ IInterface* what);

    static CARAPI_(void) ResetLock(
        /* [in] */ ISpannable* content,
        /* [in] */ IInterface* what);

    CARAPI_(void) Press(
        /* [in] */ IEditable* content,
        /* [in] */ IInterface* what);

    CARAPI_(void) ReleaseInternal(
        /* [in] */ IEditable* content,
        /* [in] */ IInterface* what,
        /* [in] */ IKeyEvent* event);

    static CARAPI_(Int64) Press(
        /* [in] */ Int64 state,
        /* [in] */ Int32 what,
        /* [in] */ Int64 mask,
        /* [in] */ Int64 locked,
        /* [in] */ Int64 pressed,
        /* [in] */ Int64 released,
        /* [in] */ Int64 used);

    static CARAPI_(Int64) ReleaseInternal(
        /* [in] */ Int64 state,
        /* [in] */ Int32 what,
        /* [in] */ Int64 mask,
        /* [in] */ Int64 pressed,
        /* [in] */ Int64 released,
        /* [in] */ Int64 used,
        /* [in] */ IKeyEvent* event);

private:
    // These bits are privately used by the meta key key listener.
    // They are deliberately assigned values outside of the representable range of an 'Int32'
    // so as not to conflict with any meta key states publicly defined by KeyEvent.
    static const Int64 META_CAP_USED;// = 1L << 32;
    static const Int64 META_ALT_USED;// = 1L << 33;
    static const Int64 META_SYM_USED;// = 1L << 34;

    static const Int64 META_CAP_PRESSED;// = 1L << 40;
    static const Int64 META_ALT_PRESSED;// = 1L << 41;
    static const Int64 META_SYM_PRESSED;// = 1L << 42;

    static const Int64 META_CAP_RELEASED;// = 1L << 48;
    static const Int64 META_ALT_RELEASED;// = 1L << 49;
    static const Int64 META_SYM_RELEASED;// = 1L << 50;

    static const Int64 META_SHIFT_MASK;// = META_SHIFT_ON | META_CAP_LOCKED | META_CAP_USED | META_CAP_PRESSED | META_CAP_RELEASED;
    static const Int64 META_ALT_MASK;// = META_ALT_ON | META_ALT_LOCKED | META_ALT_USED | META_ALT_PRESSED | META_ALT_RELEASED;
    static const Int64 META_SYM_MASK;// = META_SYM_ON | META_SYM_LOCKED | META_SYM_USED | META_SYM_PRESSED | META_SYM_RELEASED;

    static const AutoPtr<IInterface> CAP;// = new NoCopySpan.Concrete();
    static const AutoPtr<IInterface> ALT;// = new NoCopySpan.Concrete();
    static const AutoPtr<IInterface> SYM;// = new NoCopySpan.Concrete();
    static const AutoPtr<IInterface> SELECTING;// = new NoCopySpan.Concrete();

    /**
     * The meta key has been pressed but has not yet been used.
     */
    static const Int32 PRESSED;// = Spannable.SPAN_MARK_MARK | (1 << Spannable.SPAN_USER_SHIFT);

    /**
     * The meta key has been pressed and released but has still
     * not yet been used.
     */
    static const Int32 RELEASED;// = Spannable.SPAN_MARK_MARK | (2 << Spannable.SPAN_USER_SHIFT);

    /**
     * The meta key has been pressed and used but has not yet been released.
     */
    static const Int32 USED;// = Spannable.SPAN_MARK_MARK | (3 << Spannable.SPAN_USER_SHIFT);

    /**
     * The meta key has been pressed and released without use, and then
     * pressed again; it may also have been released again.
     */
    static const Int32 LOCKED;// = Spannable.SPAN_MARK_MARK | (4 << Spannable.SPAN_USER_SHIFT);

    static const Int32 PRESSED_RETURN_VALUE;

    static const Int32 LOCKED_RETURN_VALUE;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_METAKEYKEYLISTENER_H__
