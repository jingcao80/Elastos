
#ifndef __ELASTOS_DROID_VIEW_KEYEVENT_H__
#define __ELASTOS_DROID_VIEW_KEYEVENT_H__

#include "elastos/droid/view/InputEvent.h"

using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace View {

class KeyEvent
    : public InputEvent
    , public IKeyEvent
{
public:
    /**
    * Use with {@link KeyEvent#dispatch(Callback, DispatcherState, Object)}
    * for more advanced key dispatching, such as Int64 presses.
    */
    class DispatcherState
        : public Object
        , public IDispatcherState
    {
    public:
        CAR_INTERFACE_DECL()

        DispatcherState();

        ~DispatcherState();

        CARAPI constructor();

        /**
         * Reset back to initial state.
         */
        CARAPI Reset();

        /**
         * Stop any tracking associated with this target.
         */
        CARAPI Reset(
            /* [in] */ IInterface* target);

        /**
         * Start tracking the key code associated with the given event.  This
         * can only be called on a key down.  It will allow you to see any
         * long press associated with the key, and will result in
         * {@link KeyEvent#isTracking} return true on the long press and up
         * events.
         *
         * <p>This is only needed if you are directly dispatching events, rather
         * than handling them in {@link Callback#onKeyDown}.
         */
        CARAPI StartTracking(
            /* [in] */ IKeyEvent* event,
            /* [in] */ IInterface* target);

        /**
         * Return true if the key event is for a key code that is currently
         * being tracked by the dispatcher.
         */
        CARAPI IsTracking(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* isTracking);

        /**
         * Keep track of the given event's key code as having performed an
         * action with a long press, so no action should occur on the up.
         * <p>This is only needed if you are directly dispatching events, rather
         * than handling them in {@link Callback#onKeyLongPress}.
         */
        CARAPI PerformedLongPress(
            /* [in] */ IKeyEvent* event);

        /**
         * Handle key up event to stop tracking.  This resets the dispatcher state,
         * and updates the key event state based on it.
         * <p>This is only needed if you are directly dispatching events, rather
         * than handling them in {@link Callback#onKeyUp}.
         */
        CARAPI HandleUpEvent(
            /* [in] */ IKeyEvent* event);

    private:
        Int32 mDownKeyCode;
        AutoPtr<IInterface> mDownTarget;
        AutoPtr<ISparseInt32Array> mActiveLongPresses;
    };

public:
    /**
     * Returns the maximum keycode.
     */
    static CARAPI_(Int32) GetMaxKeyCode();

    /**
     * Get the character that is produced by putting accent on the character
     * c.
     * For example, getDeadChar('`', 'e') returns &egrave;.
     */
    static CARAPI_(Int32) GetDeadChar(
        /* [in] */ Int32 accent,
        /* [in] */ Int32 c);

    /**
     * Obtains a (potentially recycled) key event.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<IKeyEvent>) Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 scancode,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 source,
        /* [in] */ const String& characters);

    /**
     * Obtains a (potentially recycled) copy of another key event.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<IKeyEvent>) Obtain(
        /* [in] */ IKeyEvent* otherEvent);

    /**
     * Create a new key event that is the same as the given one, but whose
     * event time and repeat count are replaced with the given value.
     *
     * @param event The existing event to be copied.  This is not modified.
     * @param eventTime The new event time
     * (in {@link android.os.SystemClock#uptimeMillis}) of the event.
     * @param newRepeat The new repeat count of the event.
     */
    static CARAPI ChangeTimeRepeat(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 newRepeat,
        /* [out] */ IKeyEvent** newEvent);

    /**
     * Create a new key event that is the same as the given one, but whose
     * event time and repeat count are replaced with the given value.
     *
     * @param event The existing event to be copied.  This is not modified.
     * @param eventTime The new event time
     * (in {@link android.os.SystemClock#uptimeMillis}) of the event.
     * @param newRepeat The new repeat count of the event.
     * @param newFlags New flags for the event, replacing the entire value
     * in the original event.
     */
    static CARAPI ChangeTimeRepeat(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 newRepeat,
        /* [in] */ Int32 newFlags,
        /* [out] */ IKeyEvent** newEvent);

    /**
     * Create a new key event that is the same as the given one, but whose
     * action is replaced with the given value.
     *
     * @param event The existing event to be copied.  This is not modified.
     * @param action The new action code of the event.
     */
    static CARAPI ChangeAction(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 action,
        /* [out] */ IKeyEvent** newEvent);

    /**
     * Create a new key event that is the same as the given one, but whose
     * flags are replaced with the given value.
     *
     * @param event The existing event to be copied.  This is not modified.
     * @param flags The new flags constant.
     */
    static CARAPI ChangeFlags(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ IKeyEvent** newEvent);

    /**
     * Returns true if the specified keycode is a gamepad button.
     * @return True if the keycode is a gamepad button, such as {@link #KEYCODE_BUTTON_A}.
     */
    static CARAPI_(Boolean) IsGamepadButton(
        /* [in] */ Int32 keyCode);

    /** Whether key will, by default, trigger a click on the focused view.
     * @hide
    */
    static CARAPI_(Boolean) IsConfirmKey(
        /* [in] */ Int32 keyCode);

    /**
     * Whether this key is a media key, which can be send to apps that are
     * interested in media key events.
     *
     * @hide
     */
    static CARAPI_(Boolean) IsMediaKey(
        /* [in] */ Int32 keyCode);

    /** Is this a system key? System keys can not be used for menu shortcuts.
     * @hide
     */
    static CARAPI_(Boolean) IsSystemKey(
        /* [in] */ Int32 keyCode);

    /** @hide */
    static CARAPI_(Boolean) IsWakeKey(
        /* [in] */ Int32 keyCode);

    /** @hide */
    static CARAPI_(Boolean) IsMetaKey(
        /* [in] */ Int32 keyCode);

    /**
     * Gets a mask that includes all valid modifier key meta state bits.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, the mask specifically excludes
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p>
     *
     * @return The modifier meta state mask which is a combination of
     * {@link #META_SHIFT_ON}, {@link #META_SHIFT_LEFT_ON}, {@link #META_SHIFT_RIGHT_ON},
     * {@link #META_ALT_ON}, {@link #META_ALT_LEFT_ON}, {@link #META_ALT_RIGHT_ON},
     * {@link #META_CTRL_ON}, {@link #META_CTRL_LEFT_ON}, {@link #META_CTRL_RIGHT_ON},
     * {@link #META_META_ON}, {@link #META_META_LEFT_ON}, {@link #META_META_RIGHT_ON},
     * {@link #META_SYM_ON}, {@link #META_FUNCTION_ON}.
     */
    static CARAPI_(Int32) GetModifierMetaStateMask();

    /**
     * Returns true if this key code is a modifier key.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function return false
     * for those keys.
     * </p>
     *
     * @return True if the key code is one of
     * {@link #KEYCODE_SHIFT_LEFT} {@link #KEYCODE_SHIFT_RIGHT},
     * {@link #KEYCODE_ALT_LEFT}, {@link #KEYCODE_ALT_RIGHT},
     * {@link #KEYCODE_CTRL_LEFT}, {@link #KEYCODE_CTRL_RIGHT},
     * {@link #KEYCODE_META_LEFT}, or {@link #KEYCODE_META_RIGHT},
     * {@link #KEYCODE_SYM}, {@link #KEYCODE_NUM}, {@link #KEYCODE_FUNCTION}.
     */
    static CARAPI_(Boolean) IsModifierKey(
        /* [in] */ Int32 keyCode);

    /**
     * Normalizes the specified meta state.
     * <p>
     * The meta state is normalized such that if either the left or right modifier meta state
     * bits are set then the result will also include the universal bit for that modifier.
     * </p><p>
     * If the specified meta state contains {@link #META_ALT_LEFT_ON} then
     * the result will also contain {@link #META_ALT_ON} in addition to {@link #META_ALT_LEFT_ON}
     * and the other bits that were specified in the input.  The same is process is
     * performed for shift, control and meta.
     * </p><p>
     * If the specified meta state contains synthetic meta states defined by
     * {@link MetaKeyKeyListener}, then those states are translated here and the original
     * synthetic meta states are removed from the result.
     * {@link MetaKeyKeyListener#META_CAP_LOCKED} is translated to {@link #META_CAPS_LOCK_ON}.
     * {@link MetaKeyKeyListener#META_ALT_LOCKED} is translated to {@link #META_ALT_ON}.
     * {@link MetaKeyKeyListener#META_SYM_LOCKED} is translated to {@link #META_SYM_ON}.
     * </p><p>
     * Undefined meta state bits are removed.
     * </p>
     *
     * @param metaState The meta state.
     * @return The normalized meta state.
     */
    static CARAPI_(Int32) NormalizeMetaState(
        /* [in] */ Int32 metaState);

    /**
     * Returns true if no modifiers keys are pressed according to the specified meta state.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function ignores
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p><p>
     * The meta state is normalized prior to comparison using {@link #normalizeMetaState(int)}.
     * </p>
     *
     * @param metaState The meta state to consider.
     * @return True if no modifier keys are pressed.
     * @see #hasNoModifiers()
     */
    static CARAPI_(Boolean) MetaStateHasNoModifiers(
        /* [in] */ Int32 metaState);

    /**
     * Returns true if only the specified modifier keys are pressed according to
     * the specified meta state.  Returns false if a different combination of modifier
     * keys are pressed.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function ignores
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p><p>
     * If the specified modifier mask includes directional modifiers, such as
     * {@link #META_SHIFT_LEFT_ON}, then this method ensures that the
     * modifier is pressed on that side.
     * If the specified modifier mask includes non-directional modifiers, such as
     * {@link #META_SHIFT_ON}, then this method ensures that the modifier
     * is pressed on either side.
     * If the specified modifier mask includes both directional and non-directional modifiers
     * for the same type of key, such as {@link #META_SHIFT_ON} and {@link #META_SHIFT_LEFT_ON},
     * then this method throws an illegal argument exception.
     * </p>
     *
     * @param metaState The meta state to consider.
     * @param modifiers The meta state of the modifier keys to check.  May be a combination
     * of modifier meta states as defined by {@link #getModifierMetaStateMask()}.  May be 0 to
     * ensure that no modifier keys are pressed.
     * @return True if only the specified modifier keys are pressed.
     * @throws IllegalArgumentException if the modifiers parameter contains invalid modifiers
     * @see #hasModifiers
     */
    static CARAPI MetaStateHasModifiers(
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 modifiers,
        /* [out] */ Boolean* res);

    /**
     * Returns a string that represents the symbolic name of the specified action
     * such as "ACTION_DOWN", or an equivalent numeric constant such as "35" if unknown.
     *
     * @param action The action.
     * @return The symbolic name of the specified action.
     * @hide
     */
    static CARAPI_(String) ActionToString(
        /* [in] */ Int32 action);

    /**
     * Returns a string that represents the symbolic name of the specified keycode
     * such as "KEYCODE_A", "KEYCODE_DPAD_UP", or an equivalent numeric constant
     * such as "1001" if unknown.
     *
     * @param keyCode The key code.
     * @return The symbolic name of the specified keycode.
     *
     * @see KeyCharacterMap#getDisplayLabel
     */
    static CARAPI_(String) KeyCodeToString(
        /* [in] */ Int32 keyCode);

    /**
     * Gets a keycode by its symbolic name such as "KEYCODE_A" or an equivalent
     * numeric constant such as "1001".
     *
     * @param symbolicName The symbolic name of the keycode.
     * @return The keycode or {@link #KEYCODE_UNKNOWN} if not found.
     * @see #keycodeToString(int)
     */
    static CARAPI KeyCodeFromString(
        /* [in] */ const String& symbolicName,
        /* [out] */ Int32* keyCode);

    /**
     * Returns a string that represents the symbolic name of the specified combined meta
     * key modifier state flags such as "0", "META_SHIFT_ON",
     * "META_ALT_ON|META_SHIFT_ON" or an equivalent numeric constant such as "0x10000000"
     * if unknown.
     *
     * @param metaState The meta state.
     * @return The symbolic name of the specified combined meta state flags.
     * @hide
     */
    static CARAPI_(String) MetaStateToString(
        /* [in] */ Int32 metaState);

    /** @hide */
    static CARAPI CreateFromParcelBody(
        /* [in] */ IParcel* in,
        /* [out] */ IKeyEvent** newEvent);

private:
    static CARAPI_(String) NativeKeyCodeToString(
        /* [in] */ Int32 keyCode);

    static CARAPI_(Int32) NativeKeyCodeFromString(
        /* [in] */ const String& keyCode);

    static CARAPI_(AutoPtr<IKeyEvent>) Obtain();

    static CARAPI MetaStateFilterDirectionalModifiers(
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 modifiers,
        /* [in] */ Int32 basic,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [out] */ Int32* ret);

public:
    CAR_INTERFACE_DECL()

    KeyEvent();

    ~KeyEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 action,
        /* [in] */ Int32 code);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 metaState);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 scancode);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 scancode,
        /* [in] */ Int32 flags);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 code,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 scancode,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 source);

    CARAPI constructor(
        /* [in] */ Int64 time,
        /* [in] */ const String& characters,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 flags);

    CARAPI constructor(
        /* [in] */ IKeyEvent* origEvent);

    //@Deprecated
    CARAPI constructor(
        /* [in] */ IKeyEvent* origEvent,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 newRepeat);

    // private
    CARAPI constructor(
        /* [in] */ IKeyEvent* origEvent,
        /* [in] */ Int32 action);

    //@Override
    CARAPI Copy(
        /* [out] */ IInputEvent** event);

    //@Override
    CARAPI Recycle();

    //@Override
    CARAPI RecycleIfNeededAfterDispatch();

    //@Override
    CARAPI IsTainted(
        /* [out] */ Boolean* isTainted);

    //@Override
    CARAPI SetTainted(
        /* [in] */ Boolean tainted);

    /**
     * Don't use in new code, instead explicitly check
     * {@link #getAction()}.
     *
     * @return If the action is ACTION_DOWN, returns true; else false.
     *
     * @deprecated
     * @hide
     */
    CARAPI IsDown(
        /* [out] */ Boolean* isDown);

    /**
     * Is this a system key?  System keys can not be used for menu shortcuts.
     *
     * TODO: this information should come from a table somewhere.
     * TODO: should the dpad keys be here?  arguably, because they also shouldn't be menu shortcuts
     */
    CARAPI IsSystem(
        /* [out] */ Boolean* isSystem);

    CARAPI IsWakeKey(
        /* [out] */ Boolean* wakeKey);

    //@Override
    CARAPI GetDeviceId(
        /* [out] */ Int32* deviceId);

    //@Override
    CARAPI GetDevice(
        /* [out] */ IInputDevice** device);

    //@Override
    CARAPI GetSource(
        /* [out] */ Int32* source);

    //@Override
    CARAPI SetSource(
        /* [in] */ Int32 source);

    /**
     * <p>Returns the state of the meta keys.</p>
     *
     * @return an integer in which each bit set to 1 represents a pressed
     *         meta key
     *
     * @see #isAltPressed()
     * @see #isShiftPressed()
     * @see #isSymPressed()
     * @see #isCtrlPressed()
     * @see #isMetaPressed()
     * @see #isFunctionPressed()
     * @see #isCapsLockOn()
     * @see #isNumLockOn()
     * @see #isScrollLockOn()
     * @see #META_ALT_ON
     * @see #META_ALT_LEFT_ON
     * @see #META_ALT_RIGHT_ON
     * @see #META_SHIFT_ON
     * @see #META_SHIFT_LEFT_ON
     * @see #META_SHIFT_RIGHT_ON
     * @see #META_SYM_ON
     * @see #META_FUNCTION_ON
     * @see #META_CTRL_ON
     * @see #META_CTRL_LEFT_ON
     * @see #META_CTRL_RIGHT_ON
     * @see #META_META_ON
     * @see #META_META_LEFT_ON
     * @see #META_META_RIGHT_ON
     * @see #META_CAPS_LOCK_ON
     * @see #META_NUM_LOCK_ON
     * @see #META_SCROLL_LOCK_ON
     * @see #getModifiers
     */
    CARAPI GetMetaState(
        /* [out] */ Int32* metaState);

    /**
     * Returns the state of the modifier keys.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function specifically masks out
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p><p>
     * The value returned consists of the meta state (from {@link #getMetaState})
     * normalized using {@link #normalizeMetaState(int)} and then masked with
     * {@link #getModifierMetaStateMask} so that only valid modifier bits are retained.
     * </p>
     *
     * @return An integer in which each bit set to 1 represents a pressed modifier key.
     * @see #getMetaState
     */
    CARAPI GetModifiers(
        /* [out] */ Int32* modifiers);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    /**
     * Returns true if no modifier keys are pressed.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function ignores
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p><p>
     * The meta state is normalized prior to comparison using {@link #normalizeMetaState(int)}.
     * </p>
     *
     * @return True if no modifier keys are pressed.
     * @see #metaStateHasNoModifiers
     */
    CARAPI HasNoModifiers(
        /* [out] */ Boolean* res);

    /**
     * Returns true if only the specified modifiers keys are pressed.
     * Returns false if a different combination of modifier keys are pressed.
     * <p>
     * For the purposes of this function, {@link #KEYCODE_CAPS_LOCK},
     * {@link #KEYCODE_SCROLL_LOCK}, and {@link #KEYCODE_NUM_LOCK} are
     * not considered modifier keys.  Consequently, this function ignores
     * {@link #META_CAPS_LOCK_ON}, {@link #META_SCROLL_LOCK_ON} and {@link #META_NUM_LOCK_ON}.
     * </p><p>
     * If the specified modifier mask includes directional modifiers, such as
     * {@link #META_SHIFT_LEFT_ON}, then this method ensures that the
     * modifier is pressed on that side.
     * If the specified modifier mask includes non-directional modifiers, such as
     * {@link #META_SHIFT_ON}, then this method ensures that the modifier
     * is pressed on either side.
     * If the specified modifier mask includes both directional and non-directional modifiers
     * for the same type of key, such as {@link #META_SHIFT_ON} and {@link #META_SHIFT_LEFT_ON},
     * then this method throws an illegal argument exception.
     * </p>
     *
     * @param modifiers The meta state of the modifier keys to check.  May be a combination
     * of modifier meta states as defined by {@link #getModifierMetaStateMask()}.  May be 0 to
     * ensure that no modifier keys are pressed.
     * @return True if only the specified modifier keys are pressed.
     * @throws IllegalArgumentException if the modifiers parameter contains invalid modifiers
     * @see #metaStateHasModifiers
     */
    CARAPI HasModifiers(
        /* [in] */ Int32 modifiers,
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the pressed state of the ALT meta key.</p>
     *
     * @return true if the ALT key is pressed, false otherwise
     *
     * @see #KEYCODE_ALT_LEFT
     * @see #KEYCODE_ALT_RIGHT
     * @see #META_ALT_ON
     */
    CARAPI IsAltPressed(
        /* [out] */ Boolean* isAltPressed);

    /**
     * <p>Returns the pressed state of the SHIFT meta key.</p>
     *
     * @return true if the SHIFT key is pressed, false otherwise
     *
     * @see #KEYCODE_SHIFT_LEFT
     * @see #KEYCODE_SHIFT_RIGHT
     * @see #META_SHIFT_ON
     */
    CARAPI IsShiftPressed(
        /* [out] */ Boolean* isShiftPressed);

    /**
     * <p>Returns the pressed state of the SYM meta key.</p>
     *
     * @return true if the SYM key is pressed, false otherwise
     *
     * @see #KEYCODE_SYM
     * @see #META_SYM_ON
     */
    CARAPI IsSymPressed(
        /* [out] */ Boolean* isSymPressed);

    /**
     * <p>Returns the pressed state of the CTRL meta key.</p>
     *
     * @return true if the CTRL key is pressed, false otherwise
     *
     * @see #KEYCODE_CTRL_LEFT
     * @see #KEYCODE_CTRL_RIGHT
     * @see #META_CTRL_ON
     */
    CARAPI IsCtrlPressed(
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the pressed state of the META meta key.</p>
     *
     * @return true if the META key is pressed, false otherwise
     *
     * @see #KEYCODE_META_LEFT
     * @see #KEYCODE_META_RIGHT
     * @see #META_META_ON
     */
    CARAPI IsMetaPressed(
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the pressed state of the FUNCTION meta key.</p>
     *
     * @return true if the FUNCTION key is pressed, false otherwise
     *
     * @see #KEYCODE_FUNCTION
     * @see #META_FUNCTION_ON
     */
    CARAPI IsFunctionPressed(
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the locked state of the CAPS LOCK meta key.</p>
     *
     * @return true if the CAPS LOCK key is on, false otherwise
     *
     * @see #KEYCODE_CAPS_LOCK
     * @see #META_CAPS_LOCK_ON
     */
    CARAPI IsCapsLockOn(
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the locked state of the NUM LOCK meta key.</p>
     *
     * @return true if the NUM LOCK key is on, false otherwise
     *
     * @see #KEYCODE_NUM_LOCK
     * @see #META_NUM_LOCK_ON
     */
    CARAPI IsNumLockOn(
        /* [out] */ Boolean* res);

    /**
     * <p>Returns the locked state of the SCROLL LOCK meta key.</p>
     *
     * @return true if the SCROLL LOCK key is on, false otherwise
     *
     * @see #KEYCODE_SCROLL_LOCK
     * @see #META_SCROLL_LOCK_ON
     */
    CARAPI IsScrollLockOn(
        /* [out] */ Boolean* res);

    /**
     * Retrieve the action of this key event.  May be either
     * {@link #ACTION_DOWN}, {@link #ACTION_UP}, or {@link #ACTION_MULTIPLE}.
     *
     * @return The event action: ACTION_DOWN, ACTION_UP, or ACTION_MULTIPLE.
     */
    CARAPI GetAction(
        /* [out] */ Int32* action);

    /**
     * For {@link #ACTION_UP} events, indicates that the event has been
     * canceled as per {@link #FLAG_CANCELED}.
     */
    CARAPI IsCanceled(
        /* [out] */ Boolean* isCanceled);

    /**
     * Call this during {@link Callback#onKeyDown} to have the system track
     * the key through its final up (possibly including a long press).  Note
     * that only one key can be tracked at a time -- if another key down
     * event is received while a previous one is being tracked, tracking is
     * stopped on the previous event.
     */
    CARAPI StartTracking();

    /**
     * For {@link #ACTION_UP} events, indicates that the event is still being
     * tracked from its initial down event as per
     * {@link #FLAG_TRACKING}.
     */
    CARAPI IsTracking(
        /* [out] */ Boolean* isTracking);

    /**
     * For {@link #ACTION_DOWN} events, indicates that the event has been
     * canceled as per {@link #FLAG_LONG_PRESS}.
     */
    CARAPI IsLongPress(
        /* [out] */ Boolean* isLongPress);

    /**
     * Retrieve the key code of the key event.  This is the physical key that
     * was pressed, <em>not</em> the Unicode character.
     *
     * @return The key code of the event.
     */
    CARAPI GetKeyCode(
        /* [out] */ Int32* keyCode);

    /**
     * For the special case of a {@link #ACTION_MULTIPLE} event with key
     * code of {@link #KEYCODE_UNKNOWN}, this is a raw string of characters
     * associated with the event.  In all other cases it is null.
     *
     * @return Returns a String of 1 or more characters associated with
     * the event.
     */
    CARAPI GetCharacters(
        /* [out] */ String* characters);

    /**
     * Retrieve the hardware key id of this key event.  These values are not
     * reliable and vary from device to device.
     *
     * {@more}
     * Mostly this is here for debugging purposes.
     */
    CARAPI GetScanCode(
        /* [out] */ Int32* scanCode);

    /**
     * Retrieve the repeat count of the event.  For both key up and key down
     * events, this is the number of times the key has repeated with the first
     * down starting at 0 and counting up from there.  For multiple key
     * events, this is the number of down/up pairs that have occurred.
     *
     * @return The number of times the key has repeated.
     */
    CARAPI GetRepeatCount(
        /* [out] */ Int32* repeatCount);

    /**
     * Retrieve the time of the most recent key down event,
     * in the {@link android.os.SystemClock#uptimeMillis} time base.  If this
     * is a down event, this will be the same as {@link #getEventTime()}.
     * Note that when chording keys, this value is the down time of the
     * most recently pressed key, which may <em>not</em> be the same physical
     * key of this event.
     *
     * @return Returns the most recent key down time, in the
     * {@link android.os.SystemClock#uptimeMillis} time base
     */
    CARAPI GetDownTime(
        /* [out] */ Int64* downTime);

    /**
     * Retrieve the time this event occurred,
     * in the {@link android.os.SystemClock#uptimeMillis} time base.
     *
     * @return Returns the time this event occurred,
     * in the {@link android.os.SystemClock#uptimeMillis} time base.
     */
     //@Override
    CARAPI GetEventTime(
        /* [out] */ Int64* eventTime);

    /**
     * Retrieve the time this event occurred,
     * in the {@link android.os.SystemClock#uptimeMillis} time base but with
     * nanosecond (instead of millisecond) precision.
     * <p>
     * The value is in nanosecond precision but it may not have nanosecond accuracy.
     * </p>
     *
     * @return Returns the time this event occurred,
     * in the {@link android.os.SystemClock#uptimeMillis} time base but with
     * nanosecond (instead of millisecond) precision.
     *
     * @hide
     */
    //@Override
    CARAPI GetEventTimeNano(
        /* [out] */ Int64* eventTimeNano);

    //@Override
    CARAPI GetSequenceNumber(
        /* [out] */ Int32* seq);

    /**
     * Renamed to {@link #getDeviceId}.
     *
     * @hide
     * @deprecated use {@link #getDeviceId()} instead.
     */
    //@Deprecated
    CARAPI GetKeyboardDevice(
        /* [out] */ Int32* deviceId);

    /**
     * Gets the {@link KeyCharacterMap} associated with the keyboard device.
     *
     * @return The associated key character map.
     * @throws {@link KeyCharacterMap.UnavailableException} if the key character map
     * could not be loaded because it was malformed or the default key character map
     * is missing from the system.
     *
     * @see KeyCharacterMap#load
     */
    CARAPI GetKeyCharacterMap(
        /* [out] */ IKeyCharacterMap** kcm);

    /**
     * Gets the primary character for this key.
     * In other words, the label that is physically printed on it.
     *
     * @return The display label character, or 0 if none (eg. for non-printing keys).
     */
    CARAPI GetDisplayLabel(
        /* [out] */ Char32* displayLabel);

    /**
     * Gets the Unicode character generated by the specified key and meta
     * key state combination.
     * <p>
     * Returns the Unicode character that the specified key would produce
     * when the specified meta bits (see {@link MetaKeyKeyListener})
     * were active.
     * </p><p>
     * Returns 0 if the key is not one that is used to type Unicode
     * characters.
     * </p><p>
     * If the return value has bit {@link KeyCharacterMap#COMBINING_ACCENT} set, the
     * key is a "dead key" that should be combined with another to
     * actually produce a character -- see {@link KeyCharacterMap#getDeadChar} --
     * after masking with {@link KeyCharacterMap#COMBINING_ACCENT_MASK}.
     * </p>
     *
     * @return The associated character or combining accent, or 0 if none.
     */
    CARAPI GetUnicodeChar(
        /* [out] */ Int32* unicodeChar);

    /**
     * Gets the Unicode character generated by the specified key and meta
     * key state combination.
     * <p>
     * Returns the Unicode character that the specified key would produce
     * when the specified meta bits (see {@link MetaKeyKeyListener})
     * were active.
     * </p><p>
     * Returns 0 if the key is not one that is used to type Unicode
     * characters.
     * </p><p>
     * If the return value has bit {@link KeyCharacterMap#COMBINING_ACCENT} set, the
     * key is a "dead key" that should be combined with another to
     * actually produce a character -- see {@link KeyCharacterMap#getDeadChar} --
     * after masking with {@link KeyCharacterMap#COMBINING_ACCENT_MASK}.
     * </p>
     *
     * @param metaState The meta key modifier state.
     * @return The associated character or combining accent, or 0 if none.
     */
    CARAPI GetUnicodeChar(
        /* [in] */ Int32 metaState,
        /* [out] */ Int32* unicodeChar);

    /**
     * Get the character conversion data for a given key code.
     *
     * @param results A {@link KeyCharacterMap.KeyData} instance that will be
     * filled with the results.
     * @return True if the key was mapped.  If the key was not mapped, results is not modified.
     *
     * @deprecated instead use {@link #getDisplayLabel()},
     * {@link #getNumber()} or {@link #getUnicodeChar(int)}.
     */
    //@Deprecated
    CARAPI GetKeyData(
        /* [in] */ IKeyData* keyData,
        /* [out] */ Boolean* res);

    /**
     * Gets the first character in the character array that can be generated
     * by the specified key code.
     * <p>
     * This is a convenience function that returns the same value as
     * {@link #getMatch(char[],int) getMatch(chars, 0)}.
     * </p>
     *
     * @param chars The array of matching characters to consider.
     * @return The matching associated character, or 0 if none.
     */
    CARAPI GetMatch(
        /* [in] */ ArrayOf<Char32>* chars,
        /* [out] */ Char32* match);

    /**
     * Gets the first character in the character array that can be generated
     * by the specified key code.  If there are multiple choices, prefers
     * the one that would be generated with the specified meta key modifier state.
     *
     * @param chars The array of matching characters to consider.
     * @param metaState The preferred meta key modifier state.
     * @return The matching associated character, or 0 if none.
     */
    CARAPI GetMatch(
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 modifiers,
        /* [out] */ Char32* match);

    /**
     * Gets the number or symbol associated with the key.
     * <p>
     * The character value is returned, not the numeric value.
     * If the key is not a number, but is a symbol, the symbol is retuned.
     * </p><p>
     * This method is intended to to support dial pads and other numeric or
     * symbolic entry on keyboards where certain keys serve dual function
     * as alphabetic and symbolic keys.  This method returns the number
     * or symbol associated with the key independent of whether the user
     * has pressed the required modifier.
     * </p><p>
     * For example, on one particular keyboard the keys on the top QWERTY row generate
     * numbers when ALT is pressed such that ALT-Q maps to '1'.  So for that keyboard
     * when {@link #getNumber} is called with {@link KeyEvent#KEYCODE_Q} it returns '1'
     * so that the user can type numbers without pressing ALT when it makes sense.
     * </p>
     *
     * @return The associated numeric or symbolic character, or 0 if none.
     */
    CARAPI GetNumber(
        /* [out] */ Char32* ch);

    /**
     * Returns true if this key produces a glyph.
     *
     * @return True if the key is a printing key.
     */
    CARAPI IsPrintingKey(
        /* [out] */ Boolean* res);

    /**
     * @deprecated Use {@link #dispatch(Callback, DispatcherState, Object)} instead.
     */
    //@Deprecated
    CARAPI Dispatch(
        /* [in] */ IKeyEventCallback* receiver,
        /* [out] */ Boolean* res);

    /**
     * Deliver this key event to a {@link Callback} interface.  If this is
     * an ACTION_MULTIPLE event and it is not handled, then an attempt will
     * be made to deliver a single normal event.
     *
     * @param receiver The Callback that will be given the event.
     * @param state State information retained across events.
     * @param target The target of the dispatch, for use in tracking.
     *
     * @return The return value from the Callback method that was called.
     */
    CARAPI Dispatch(
        /* [in] */ IKeyEventCallback* receiver,
        /* [in] */ IDispatcherState* state,
        /* [in] */ IInterface* target,
        /* [out] */ Boolean* res);

    CARAPI_(String) ToString();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //public static final Parcelable.Creator<KeyEvent> CREATOR
    //        = new Parcelable.Creator<KeyEvent>() {
    //    public KeyEvent createFromParcel(Parcel in) {
    //        in.readInt(); // skip token, we already know this is a KeyEvent
    //        return KeyEvent.createFromParcelBody(in);
    //    }

    //    public KeyEvent[] newArray(Int32 size) {
    //        return new KeyEvent[size];
    //    }
    //};
protected:
    static const Boolean DEBUG;
    static const String TAG;

private:
    static const Int32 LAST_KEYCODE;

    // Symbolic names of all metakeys in bit order from least significant to most significant.
    // Accordingly there are exactly 32 values in this table.
    static const String META_SYMBOLIC_NAMES[];
    static const String LABEL_PREFIX;

    // Mask of all modifier key meta states.  Specifically excludes locked keys like caps lock.
    static const Int32 META_MODIFIER_MASK;

    // Mask of all lock key meta states.
    static const Int32 META_LOCK_MASK;

    // Mask of all valid meta states.
    static const Int32 META_ALL_MASK ;

    // Mask of all synthetic meta states that are reserved for API compatibility with
    // historical uses in MetaKeyKeyListener.
    static const Int32 META_SYNTHETIC_MASK;

    // Mask of all meta states that are not valid use in specifying a modifier key.
    // These bits are known to be used for purposes other than specifying modifiers.
    static const Int32 META_INVALID_MODIFIER_MASK;

    static const Int32 MAX_RECYCLED;
    static Object gRecyclerLock;
    static Int32 gRecyclerUsed;
    static AutoPtr<IKeyEvent> gRecyclerTop;

private:
    AutoPtr<IKeyEvent> mNext;
    Int32 mDeviceId;
    Int32 mSource;
    Int32 mMetaState;
    Int32 mAction;
    Int32 mKeyCode;
    Int32 mScanCode;
    Int32 mRepeatCount;
    Int32 mFlags;
    Int64 mDownTime;
    Int64 mEventTime;
    String mCharacters;
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_KEYEVENT_H__
