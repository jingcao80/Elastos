
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/KeyEvent.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/KeyCharacterMap.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include <input/Input.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Boolean KeyEvent::DEBUG = FALSE;
const String KeyEvent::TAG("KeyEvent");
const Int32 KeyEvent::LAST_KEYCODE = IKeyEvent::KEYCODE_HELP;

const String KeyEvent::META_SYMBOLIC_NAMES[] = {
    String("META_SHIFT_ON"),
    String("META_ALT_ON"),
    String("META_SYM_ON"),
    String("META_FUNCTION_ON"),
    String("META_ALT_LEFT_ON"),
    String("META_ALT_RIGHT_ON"),
    String("META_SHIFT_LEFT_ON"),
    String("META_SHIFT_RIGHT_ON"),
    String("META_CAP_LOCKED"),
    String("META_ALT_LOCKED"),
    String("META_SYM_LOCKED"),
    String("0x00000800"),
    String("META_CTRL_ON"),
    String("META_CTRL_LEFT_ON"),
    String("META_CTRL_RIGHT_ON"),
    String("0x00008000"),
    String("META_META_ON"),
    String("META_META_LEFT_ON"),
    String("META_META_RIGHT_ON"),
    String("0x00080000"),
    String("META_CAPS_LOCK_ON"),
    String("META_NUM_LOCK_ON"),
    String("META_SCROLL_LOCK_ON"),
    String("0x00800000"),
    String("0x01000000"),
    String("0x02000000"),
    String("0x04000000"),
    String("0x08000000"),
    String("0x10000000"),
    String("0x20000000"),
    String("0x40000000"),
    String("0x80000000"),
};

const String KeyEvent::LABEL_PREFIX("KEYCODE_");

const Int32 KeyEvent::META_MODIFIER_MASK =
        META_SHIFT_ON | META_SHIFT_LEFT_ON | META_SHIFT_RIGHT_ON
        | META_ALT_ON | META_ALT_LEFT_ON | META_ALT_RIGHT_ON
        | META_CTRL_ON | META_CTRL_LEFT_ON | META_CTRL_RIGHT_ON
        | META_META_ON | META_META_LEFT_ON | META_META_RIGHT_ON
        | META_SYM_ON | META_FUNCTION_ON;
const Int32 KeyEvent::META_LOCK_MASK =
        META_CAPS_LOCK_ON | META_NUM_LOCK_ON | META_SCROLL_LOCK_ON;
const Int32 KeyEvent::META_ALL_MASK = META_MODIFIER_MASK | META_LOCK_MASK;
const Int32 KeyEvent::META_SYNTHETIC_MASK =
        META_CAP_LOCKED | META_ALT_LOCKED | META_SYM_LOCKED | META_SELECTING;
const Int32 KeyEvent::META_INVALID_MODIFIER_MASK = META_LOCK_MASK | META_SYNTHETIC_MASK;

const Int32 KeyEvent::MAX_RECYCLED = 10;
Object KeyEvent::gRecyclerLock;
Int32 KeyEvent::gRecyclerUsed;
AutoPtr<IKeyEvent> KeyEvent::gRecyclerTop;

/*   KeyEvent::DispatcherState   */

CAR_INTERFACE_IMPL(KeyEvent::DispatcherState, Object, IDispatcherState);

KeyEvent::DispatcherState::DispatcherState()
    : mDownKeyCode(0)
{
    CSparseInt32Array::New((ISparseInt32Array**)&mActiveLongPresses);
}

KeyEvent::DispatcherState::~DispatcherState()
{
}

ECode KeyEvent::DispatcherState::constructor()
{
    return NOERROR;
}

ECode KeyEvent::DispatcherState::Reset()
{
    if (DEBUG)
    Logger::D(KeyEvent::TAG, "Reset: %p", this);

    mDownKeyCode = 0;
    mDownTarget = NULL;
    mActiveLongPresses->Clear();

    return NOERROR;
}

ECode KeyEvent::DispatcherState::Reset(
    /* [in] */ IInterface* target)
{
    if (mDownTarget.Get() == target) {
        if (DEBUG)
        Logger::D(TAG, "Reset in %p, %p", target, this);
        mDownKeyCode = 0;
        mDownTarget = NULL;
    }

    return NOERROR;
}

ECode KeyEvent::DispatcherState::StartTracking(
    /* [in] */ IKeyEvent* event,
    /* [in] */ IInterface* target)
{
    Int32 action;
    event->GetAction(&action);
    if (action != IKeyEvent::ACTION_DOWN) {
        Logger::E(
            KeyEvent::TAG, "Can only start tracking on a down event");

        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DEBUG)
    Logger::D(KeyEvent::TAG, "Start trackingt in %p : %p", target, this);
    event->GetKeyCode(&mDownKeyCode);
    mDownTarget = target;

    return NOERROR;
}

ECode KeyEvent::DispatcherState::IsTracking(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* isTracking)
{
    if (isTracking == NULL) {
        return E_INVALID_ARGUMENT;
    }

    assert(event);

    Int32 keyCode;
    event->GetKeyCode(&keyCode);

    *isTracking = mDownKeyCode == keyCode;

    return NOERROR;
}

ECode KeyEvent::DispatcherState::PerformedLongPress(
    /* [in] */ IKeyEvent* event)
{
    assert(event);

    Int32 keyCode;
    event->GetKeyCode(&keyCode);

    mActiveLongPresses->Put(keyCode, 1);

    return NOERROR;
}

ECode KeyEvent::DispatcherState::HandleUpEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 keyCode;
    event->GetKeyCode(&keyCode);

    if (KeyEvent::DEBUG) {
       Logger::V(KeyEvent::TAG, "Handle key up :%p", event);
    }

    Int32 index;
    mActiveLongPresses->IndexOfKey(keyCode, &index);
    if (index >= 0) {
        if (KeyEvent::DEBUG) {
            Logger::V(KeyEvent::TAG, "  Index: %d ", index);
        }
        ((KeyEvent*)event)->mFlags |=
            IKeyEvent::FLAG_CANCELED | IKeyEvent::FLAG_CANCELED_LONG_PRESS;
        mActiveLongPresses->RemoveAt(index);
    }

    if (mDownKeyCode == keyCode) {
        if (KeyEvent::DEBUG) {
            Logger::V(KeyEvent::TAG, "  Tracking!");
        }

        ((KeyEvent*)event)->mFlags |= IKeyEvent::FLAG_TRACKING;
        mDownKeyCode = 0;
        mDownTarget = NULL;
    }

    return NOERROR;
}

/*   KeyEvent   */

CAR_INTERFACE_IMPL(KeyEvent, InputEvent, IKeyEvent);

KeyEvent::KeyEvent()
    : mDeviceId(0)
    , mSource(0)
    , mMetaState(0)
    , mAction(0)
    , mKeyCode(0)
    , mScanCode(0)
    , mRepeatCount(0)
    , mFlags(0)
    , mDownTime(0ll)
    , mEventTime(0ll)
    , mCharacters(NULL)
{}

KeyEvent::~KeyEvent()
{}

ECode KeyEvent::constructor()
{
    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int32 action,
    /* [in] */ Int32 code)
{
    mAction = action;
    mKeyCode = code;
    mRepeatCount = 0;
    mDeviceId = IKeyCharacterMap::VIRTUAL_KEYBOARD;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = code;
    mRepeatCount = repeat;
    mDeviceId = IKeyCharacterMap::VIRTUAL_KEYBOARD;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 metaState)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = code;
    mRepeatCount = repeat;
    mMetaState = metaState;
    mDeviceId = IKeyCharacterMap::VIRTUAL_KEYBOARD;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 scancode)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = code;
    mRepeatCount = repeat;
    mMetaState = metaState;
    mDeviceId = deviceId;
    mScanCode = scancode;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 scancode,
    /* [in] */ Int32 flags)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = code;
    mRepeatCount = repeat;
    mMetaState = metaState;
    mDeviceId = deviceId;
    mScanCode = scancode;
    mFlags = flags;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 scancode,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 source)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = code;
    mRepeatCount = repeat;
    mMetaState = metaState;
    mDeviceId = deviceId;
    mScanCode = scancode;
    mFlags = flags;
    mSource = source;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ Int64 time,
    /* [in] */ const String& characters,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 flags)
{
    mDownTime = time;
    mEventTime = time;
    mCharacters = characters;
    mAction = ACTION_MULTIPLE;
    mKeyCode = KEYCODE_UNKNOWN;
    mRepeatCount = 0;
    mDeviceId = deviceId;
    mFlags = flags;
    mSource = IInputDevice::SOURCE_KEYBOARD;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ IKeyEvent* origEvent)
{
    if (origEvent == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<KeyEvent> event = (KeyEvent*)origEvent;
    mDownTime = event->mDownTime;
    mEventTime = event->mEventTime;
    mAction = event->mAction;
    mKeyCode = event->mKeyCode;
    mRepeatCount = event->mRepeatCount;
    mMetaState = event->mMetaState;
    mDeviceId = event->mDeviceId;
    mSource = event->mSource;
    mScanCode = event->mScanCode;
    mFlags = event->mFlags;
    mCharacters = event->mCharacters;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ IKeyEvent* origEvent,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 newRepeat)
{
    if (origEvent == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<KeyEvent> event = (KeyEvent*)origEvent;
    mDownTime = event->mDownTime;
    mEventTime = eventTime;
    mAction = event->mAction;
    mKeyCode = event->mKeyCode;
    mRepeatCount = newRepeat;
    mMetaState = event->mMetaState;
    mDeviceId = event->mDeviceId;
    mSource = event->mSource;
    mScanCode = event->mScanCode;
    mFlags = event->mFlags;
    mCharacters = event->mCharacters;

    return NOERROR;
}

ECode KeyEvent::constructor(
    /* [in] */ IKeyEvent* origEvent,
    /* [in] */ Int32 action)
{
    if (origEvent == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<KeyEvent> event = (KeyEvent*)origEvent;
    mDownTime = event->mDownTime;
    mEventTime = event->mEventTime;
    mAction = action;
    mKeyCode = event->mKeyCode;
    mRepeatCount = event->mRepeatCount;
    mMetaState = event->mMetaState;
    mDeviceId = event->mDeviceId;
    mSource = event->mSource;
    mScanCode = event->mScanCode;
    mFlags = event->mFlags;
    // Don't copy mCharacters, since one way or the other we'll lose it
    // when changing the action.

    return NOERROR;
}

AutoPtr<IKeyEvent> KeyEvent::Obtain()
{
    AutoPtr<IKeyEvent> ev;
    synchronized(gRecyclerLock) {
        ev = gRecyclerTop;
        if (ev == NULL) {
            CKeyEvent::New((IKeyEvent**)&ev);
            return ev;
        }
        AutoPtr<KeyEvent> _event = (KeyEvent*)ev.Get();
        gRecyclerTop = _event->mNext;
        gRecyclerUsed -= 1;
    }
    AutoPtr<KeyEvent> _event = (KeyEvent*)ev.Get();
    _event->mNext = NULL;
    _event->PrepareForReuse();

    return ev;
}

Int32 KeyEvent::GetMaxKeyCode()
{
    return LAST_KEYCODE;
}

Int32 KeyEvent::GetDeadChar(
    /* [in] */ Int32 accent,
    /* [in] */ Int32 c)
{
    return KeyCharacterMap::GetDeadChar(accent, c);
}

AutoPtr<IKeyEvent> KeyEvent::Obtain(
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
    /* [in] */ const String& characters)
{
    AutoPtr<IKeyEvent> ev = Obtain();
    AutoPtr<KeyEvent> _ev = (KeyEvent*)ev.Get();
    _ev->mDownTime = downTime;
    _ev->mEventTime = eventTime;
    _ev->mAction = action;
    _ev->mKeyCode = code;
    _ev->mRepeatCount = repeat;
    _ev->mMetaState = metaState;
    _ev->mDeviceId = deviceId;
    _ev->mScanCode = scancode;
    _ev->mFlags = flags;
    _ev->mSource = source;
    _ev->mCharacters = characters;
    return ev;
}

AutoPtr<IKeyEvent> KeyEvent::Obtain(
    /* [in] */ IKeyEvent* otherEvent)
{
    AutoPtr<KeyEvent> other = (KeyEvent*)otherEvent;
    AutoPtr<IKeyEvent> ev = Obtain();
    AutoPtr<KeyEvent> _ev = (KeyEvent*)ev.Get();
    _ev->mDownTime = other->mDownTime;
    _ev->mEventTime = other->mEventTime;
    _ev->mAction = other->mAction;
    _ev->mKeyCode = other->mKeyCode;
    _ev->mRepeatCount = other->mRepeatCount;
    _ev->mMetaState = other->mMetaState;
    _ev->mDeviceId = other->mDeviceId;
    _ev->mScanCode = other->mScanCode;
    _ev->mFlags = other->mFlags;
    _ev->mSource = other->mSource;
    _ev->mCharacters = other->mCharacters;
    return ev;
}

ECode KeyEvent::Copy(
    /* [out] */ IInputEvent** event)
{
    VALIDATE_NOT_NULL(event);
    AutoPtr<IInputEvent> temp = IInputEvent::Probe(Obtain(this));
    *event = temp;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode KeyEvent::Recycle()
{
    InputEvent::Recycle();
    mCharacters = NULL;

    synchronized(gRecyclerLock) {
        if (gRecyclerUsed < MAX_RECYCLED) {
            gRecyclerUsed++;
            mNext = gRecyclerTop;
            gRecyclerTop = this;
        }
    }

    return NOERROR;
}

ECode KeyEvent::RecycleIfNeededAfterDispatch()
{
    // Do nothing.
    return NOERROR;
}

ECode KeyEvent::ChangeTimeRepeat(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 newRepeat,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return CKeyEvent::New(event, eventTime, newRepeat, newEvent);
}

ECode KeyEvent::ChangeTimeRepeat(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 newRepeat,
    /* [in] */ Int32 newFlags,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    AutoPtr<IKeyEvent> ret;
    ECode ec = CKeyEvent::New(event, eventTime, newRepeat, (IKeyEvent**)&ret);
    if (FAILED(ec)) {
        return ec;
    }
    AutoPtr<KeyEvent> _ret = (KeyEvent*)ret.Get();
    _ret->mEventTime = eventTime;
    _ret->mRepeatCount = newRepeat;
    _ret->mFlags = newFlags;

    *newEvent = ret;
    REFCOUNT_ADD(*newEvent);

    return NOERROR;
}

ECode KeyEvent::ChangeAction(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 action,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return CKeyEvent::New(event, action, newEvent);
}

ECode KeyEvent::ChangeFlags(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    ECode ec = CKeyEvent::New(event, newEvent);
    if (FAILED(ec)) {
        return ec;
    }

    ((KeyEvent*)*newEvent)->mFlags = flags;

    return NOERROR;
}

ECode KeyEvent::IsTainted(
    /* [out] */ Boolean* isTainted)
{
    VALIDATE_NOT_NULL(isTainted);
    *isTainted = (mFlags & FLAG_TAINTED) != 0;

    return NOERROR;
}

ECode KeyEvent::SetTainted(
    /* [in] */ Boolean tainted)
{
    mFlags = tainted ? mFlags | FLAG_TAINTED : mFlags & ~FLAG_TAINTED;

    return NOERROR;
}

ECode KeyEvent::IsDown(
    /* [out] */ Boolean* isDown)
{
    VALIDATE_NOT_NULL(isDown);

    *isDown = (mAction == ACTION_DOWN);

    return NOERROR;
}

ECode KeyEvent::IsSystem(
    /* [out] */ Boolean* isSystem)
{
    VALIDATE_NOT_NULL(isSystem);

    *isSystem = IsSystemKey(mKeyCode);

    return NOERROR;
}

ECode KeyEvent::IsWakeKey(
    /* [out] */ Boolean* wakeKey)
{
    VALIDATE_NOT_NULL(wakeKey);
    *wakeKey = IsWakeKey(mKeyCode);

    return NOERROR;
}

Boolean KeyEvent::IsGamepadButton(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_BUTTON_A:
        case IKeyEvent::KEYCODE_BUTTON_B:
        case IKeyEvent::KEYCODE_BUTTON_C:
        case IKeyEvent::KEYCODE_BUTTON_X:
        case IKeyEvent::KEYCODE_BUTTON_Y:
        case IKeyEvent::KEYCODE_BUTTON_Z:
        case IKeyEvent::KEYCODE_BUTTON_L1:
        case IKeyEvent::KEYCODE_BUTTON_R1:
        case IKeyEvent::KEYCODE_BUTTON_L2:
        case IKeyEvent::KEYCODE_BUTTON_R2:
        case IKeyEvent::KEYCODE_BUTTON_THUMBL:
        case IKeyEvent::KEYCODE_BUTTON_THUMBR:
        case IKeyEvent::KEYCODE_BUTTON_START:
        case IKeyEvent::KEYCODE_BUTTON_SELECT:
        case IKeyEvent::KEYCODE_BUTTON_MODE:
        case IKeyEvent::KEYCODE_BUTTON_1:
        case IKeyEvent::KEYCODE_BUTTON_2:
        case IKeyEvent::KEYCODE_BUTTON_3:
        case IKeyEvent::KEYCODE_BUTTON_4:
        case IKeyEvent::KEYCODE_BUTTON_5:
        case IKeyEvent::KEYCODE_BUTTON_6:
        case IKeyEvent::KEYCODE_BUTTON_7:
        case IKeyEvent::KEYCODE_BUTTON_8:
        case IKeyEvent::KEYCODE_BUTTON_9:
        case IKeyEvent::KEYCODE_BUTTON_10:
        case IKeyEvent::KEYCODE_BUTTON_11:
        case IKeyEvent::KEYCODE_BUTTON_12:
        case IKeyEvent::KEYCODE_BUTTON_13:
        case IKeyEvent::KEYCODE_BUTTON_14:
        case IKeyEvent::KEYCODE_BUTTON_15:
        case IKeyEvent::KEYCODE_BUTTON_16:
            return TRUE;
        default:
            return FALSE;
    }
}

Boolean KeyEvent::IsConfirmKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER:
            return TRUE;
        default:
            return FALSE;
    }
}

Boolean KeyEvent::IsMediaKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
            return TRUE;
    }
    return FALSE;
}

Boolean KeyEvent::IsSystemKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_MENU:
        case IKeyEvent::KEYCODE_SOFT_RIGHT:
        case IKeyEvent::KEYCODE_HOME:
        case IKeyEvent::KEYCODE_BACK:
        case IKeyEvent::KEYCODE_CALL:
        case IKeyEvent::KEYCODE_ENDCALL:
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_POWER:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        case IKeyEvent::KEYCODE_CAMERA:
        case IKeyEvent::KEYCODE_FOCUS:
        case IKeyEvent::KEYCODE_SEARCH:
        case IKeyEvent::KEYCODE_BRIGHTNESS_DOWN:
        case IKeyEvent::KEYCODE_BRIGHTNESS_UP:
        case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK:
            return TRUE;
    }

    return FALSE;
}

Boolean KeyEvent::IsWakeKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_BACK:
        case IKeyEvent::KEYCODE_POWER:
        case IKeyEvent::KEYCODE_MENU:
        case IKeyEvent::KEYCODE_SLEEP:
        case IKeyEvent::KEYCODE_WAKEUP:
        case IKeyEvent::KEYCODE_PAIRING:
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:

            return TRUE;
    }
    return FALSE;
}

Boolean KeyEvent::IsMetaKey(
    /* [in] */ Int32 keyCode)
{
    return keyCode == IKeyEvent::KEYCODE_META_LEFT || keyCode == IKeyEvent::KEYCODE_META_RIGHT;
}

ECode KeyEvent::GetDeviceId(
    /* [out] */ Int32* deviceId)
{
    VALIDATE_NOT_NULL(deviceId);
    *deviceId = mDeviceId;

    return NOERROR;
}

ECode KeyEvent::GetDevice(
    /* [out] */ IInputDevice** device)
{
    VALIDATE_NOT_NULL(device);
    return InputEvent::GetDevice(device);
}

ECode KeyEvent::GetSource(
    /* [out] */ Int32* source)
{
    VALIDATE_NOT_NULL(source);
    *source = mSource;

    return NOERROR;
}

ECode KeyEvent::SetSource(
    /* [in] */ Int32 source)
{
    mSource = source;
    return NOERROR;
}

ECode KeyEvent::GetMetaState(
    /* [out] */ Int32* metaState)
{
    VALIDATE_NOT_NULL(metaState);

    *metaState = mMetaState;

    return NOERROR;
}

ECode KeyEvent::GetModifiers(
    /* [out] */ Int32* modifiers)
{
    VALIDATE_NOT_NULL(modifiers);
    *modifiers = NormalizeMetaState(mMetaState) & META_MODIFIER_MASK;

    return NOERROR;
}

ECode KeyEvent::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);

    *flags = mFlags;

    return NOERROR;
}

Int32 KeyEvent::GetModifierMetaStateMask()
{
    return META_MODIFIER_MASK;
}

Boolean KeyEvent::IsModifierKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case KEYCODE_SHIFT_LEFT:
        case KEYCODE_SHIFT_RIGHT:
        case KEYCODE_ALT_LEFT:
        case KEYCODE_ALT_RIGHT:
        case KEYCODE_CTRL_LEFT:
        case KEYCODE_CTRL_RIGHT:
        case KEYCODE_META_LEFT:
        case KEYCODE_META_RIGHT:
        case KEYCODE_SYM:
        case KEYCODE_NUM:
        case KEYCODE_FUNCTION:
            return TRUE;
        default:
            return FALSE;
    }
}

Int32 KeyEvent::NormalizeMetaState(
    /* [in] */ Int32 metaState)
{
    if ((metaState & (META_SHIFT_LEFT_ON | META_SHIFT_RIGHT_ON)) != 0) {
        metaState |= META_SHIFT_ON;
    }
    if ((metaState & (META_ALT_LEFT_ON | META_ALT_RIGHT_ON)) != 0) {
        metaState |= META_ALT_ON;
    }
    if ((metaState & (META_CTRL_LEFT_ON | META_CTRL_RIGHT_ON)) != 0) {
        metaState |= META_CTRL_ON;
    }
    if ((metaState & (META_META_LEFT_ON | META_META_RIGHT_ON)) != 0) {
        metaState |= META_META_ON;
    }
    if ((metaState & IMetaKeyKeyListener::META_CAP_LOCKED) != 0) {
        metaState |= META_CAPS_LOCK_ON;
    }
    if ((metaState & IMetaKeyKeyListener::META_ALT_LOCKED) != 0) {
        metaState |= META_ALT_ON;
    }
    if ((metaState & IMetaKeyKeyListener::META_SYM_LOCKED) != 0) {
        metaState |= META_SYM_ON;
    }
    return metaState & META_ALL_MASK;
}

Boolean KeyEvent::MetaStateHasNoModifiers(
    /* [in] */ Int32 metaState)
{
    return (NormalizeMetaState(metaState) & META_MODIFIER_MASK) == 0;
}

ECode KeyEvent::MetaStateHasModifiers(
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 modifiers,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Note: For forward compatibility, we allow the parameter to contain meta states
    //       that we do not recognize but we explicitly disallow meta states that
    //       are not valid modifiers.
    if ((modifiers & META_INVALID_MODIFIER_MASK) != 0) {
        Logger::E(TAG, String("modifiers must not contain ")
            + "META_CAPS_LOCK_ON, META_NUM_LOCK_ON, META_SCROLL_LOCK_ON, "
            + "META_CAP_LOCKED, META_ALT_LOCKED, META_SYM_LOCKED, "
            + "or META_SELECTING");

        return E_ILLEGAL_ARGUMENT_EXCEPTION ;
    }

    metaState = NormalizeMetaState(metaState) & META_MODIFIER_MASK;
    FAIL_RETURN(MetaStateFilterDirectionalModifiers(metaState, modifiers,
        META_SHIFT_ON, META_SHIFT_LEFT_ON, META_SHIFT_RIGHT_ON, &metaState));
    FAIL_RETURN(MetaStateFilterDirectionalModifiers(metaState, modifiers,
        META_ALT_ON, META_ALT_LEFT_ON, META_ALT_RIGHT_ON, &metaState));
    FAIL_RETURN(MetaStateFilterDirectionalModifiers(metaState, modifiers,
        META_CTRL_ON, META_CTRL_LEFT_ON, META_CTRL_RIGHT_ON, &metaState));
    FAIL_RETURN(MetaStateFilterDirectionalModifiers(metaState, modifiers,
        META_META_ON, META_META_LEFT_ON, META_META_RIGHT_ON, &metaState));
    *res = metaState == modifiers;
    return NOERROR;
}

ECode KeyEvent::MetaStateFilterDirectionalModifiers(
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 modifiers,
    /* [in] */ Int32 basic,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    Boolean wantBasic = (modifiers & basic) != 0;
    Int32 directional = left | right;
    Boolean wantLeftOrRight = (modifiers & directional) != 0;

    if (wantBasic) {
        if (wantLeftOrRight) {
            Logger::E(TAG, String("modifiers must not contain ")
                + MetaStateToString(basic) + " combined with "
                + MetaStateToString(left) + " or " + MetaStateToString(right));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *ret = metaState & ~directional;
    }
    else if (wantLeftOrRight) {
        *ret = metaState & ~basic;
    }
    else {
        *ret = metaState;
    }

    return NOERROR;
}

ECode KeyEvent::HasNoModifiers(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = MetaStateHasNoModifiers(mMetaState);

    return NOERROR;
}

ECode KeyEvent::HasModifiers(
    /* [in] */ Int32 modifiers,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return MetaStateHasModifiers(mMetaState, modifiers, res);
}

ECode KeyEvent::IsAltPressed(
    /* [out] */ Boolean* isAltPressed)
{
    VALIDATE_NOT_NULL(isAltPressed);
    *isAltPressed = (mMetaState & META_ALT_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsShiftPressed(
    /* [out] */ Boolean* isShiftPressed)
{
    VALIDATE_NOT_NULL(isShiftPressed);
    *isShiftPressed = (mMetaState & META_SHIFT_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsSymPressed(
    /* [out] */ Boolean* isSymPressed)
{
    VALIDATE_NOT_NULL(isSymPressed);
    *isSymPressed = (mMetaState & META_SYM_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsCtrlPressed(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_CTRL_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsMetaPressed(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_META_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsFunctionPressed(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_FUNCTION_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsCapsLockOn(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_CAPS_LOCK_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsNumLockOn(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_NUM_LOCK_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::IsScrollLockOn(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mMetaState & META_SCROLL_LOCK_ON) != 0;

    return NOERROR;
}

ECode KeyEvent::GetAction(
    /* [out] */ Int32* action)
{
    VALIDATE_NOT_NULL(action);
    *action = mAction;

    return NOERROR;
}

ECode KeyEvent::IsCanceled(
    /* [out] */ Boolean* isCanceled)
{
    VALIDATE_NOT_NULL(isCanceled);
    *isCanceled = (mFlags&FLAG_CANCELED) != 0;

    return NOERROR;
}

ECode KeyEvent::StartTracking()
{
    mFlags |= FLAG_START_TRACKING;

    return NOERROR;
}

ECode KeyEvent::IsTracking(
    /* [out] */ Boolean* isTracking)
{
    VALIDATE_NOT_NULL(isTracking);
    *isTracking = (mFlags&FLAG_TRACKING) != 0;

    return NOERROR;
}

ECode KeyEvent::IsLongPress(
    /* [out] */ Boolean* isLongPress)
{
    VALIDATE_NOT_NULL(isLongPress);
    *isLongPress = (mFlags&FLAG_LONG_PRESS) != 0;

    return NOERROR;
}

ECode KeyEvent::GetKeyCode(
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode);
    *keyCode = mKeyCode;

    return NOERROR;
}

ECode KeyEvent::GetCharacters(
    /* [out] */ String* characters)
{
    VALIDATE_NOT_NULL(characters);
    *characters = mCharacters;

    return NOERROR;
}

ECode KeyEvent::GetScanCode(
    /* [out] */ Int32* scanCode)
{
    VALIDATE_NOT_NULL(scanCode);
    *scanCode = mScanCode;

    return NOERROR;
}

ECode KeyEvent::GetRepeatCount(
    /* [out] */ Int32* repeatCount)
{
    VALIDATE_NOT_NULL(repeatCount);
    *repeatCount = mRepeatCount;

    return NOERROR;
}

ECode KeyEvent::GetDownTime(
    /* [out] */ Int64* downTime)
{
    VALIDATE_NOT_NULL(downTime);
    *downTime = mDownTime;

    return NOERROR;
}

ECode KeyEvent::GetEventTime(
    /* [out] */ Int64* eventTime)
{
    VALIDATE_NOT_NULL(eventTime);
    *eventTime = mEventTime;

    return NOERROR;
}

ECode KeyEvent::GetEventTimeNano(
    /* [out] */ Int64* eventTimeNano)
{
    VALIDATE_NOT_NULL(eventTimeNano);
    *eventTimeNano = mEventTime * 1000000L;

    return NOERROR;
}

ECode KeyEvent::GetSequenceNumber(
    /* [out] */ Int32* seq)
{
    VALIDATE_NOT_NULL(seq);
    return InputEvent::GetSequenceNumber(seq);
}

ECode KeyEvent::GetKeyboardDevice(
    /* [out] */ Int32* deviceId)
{
    VALIDATE_NOT_NULL(deviceId);
    *deviceId = mDeviceId;

    return NOERROR;
}

ECode KeyEvent::GetKeyCharacterMap(
    /* [out] */ IKeyCharacterMap** kcm)
{
    return KeyCharacterMap::Load(mDeviceId, kcm);
}

ECode KeyEvent::GetDisplayLabel(
    /* [out] */ Char32* displayLabel)
{
    VALIDATE_NOT_NULL(displayLabel);

    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->GetDisplayLabel(mKeyCode, displayLabel);
}

ECode KeyEvent::GetUnicodeChar(
    /* [out] */ Int32* unicodeChar)
{
    return GetUnicodeChar(mMetaState, unicodeChar);
}

ECode KeyEvent::GetUnicodeChar(
    /* [in] */ Int32 metaState,
    /* [out] */ Int32* unicodeChar)
{
    VALIDATE_NOT_NULL(unicodeChar);
    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->Get(mKeyCode, metaState, unicodeChar);
}

ECode KeyEvent::GetKeyData(
    /* [in] */ IKeyData* keyData,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->GetKeyData(mKeyCode, keyData, res);
}

ECode KeyEvent::GetMatch(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [out] */ Char32* match)
{
    return GetMatch(chars, 0, match);
}

ECode KeyEvent::GetMatch(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 modifiers,
    /* [out] */ Char32* match)
{
    VALIDATE_NOT_NULL(match);
    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->GetMatch(mKeyCode, chars, modifiers, match);
}

ECode KeyEvent::GetNumber(
    /* [out] */ Char32* ch)
{
    VALIDATE_NOT_NULL(ch);
    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->GetNumber(mKeyCode, ch);
}

ECode KeyEvent::IsPrintingKey(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IKeyCharacterMap> kcm;
    FAIL_RETURN(GetKeyCharacterMap((IKeyCharacterMap**)&kcm));

    return kcm->IsPrintingKey(mKeyCode, res);
}

ECode KeyEvent::Dispatch(
    /* [in] */ IKeyEventCallback* receiver,
    /* [out] */ Boolean* res)
{
    return Dispatch(receiver, NULL, NULL, res);
}

ECode KeyEvent::Dispatch(
    /* [in] */ IKeyEventCallback* receiver,
    /* [in] */ IDispatcherState* state,
    /* [in] */ IInterface* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        Logger::D(TAG, "Key down to %p in %p, mAction:%d", target, state, mAction);
    }

    Boolean res = FALSE;
    switch (mAction) {
    case ACTION_DOWN: {
            mFlags &= ~FLAG_START_TRACKING;

            receiver->OnKeyDown(mKeyCode, this, &res);
            if (state != NULL) {
                Boolean isLongPress;
                Boolean isTracking;

                if (res && mRepeatCount == 0
                        && (mFlags & FLAG_START_TRACKING) != 0) {
                    if (DEBUG)
                        Logger::D(TAG, "  Start tracking!");

                    state->StartTracking(this, target);
                }
                else if ((IsLongPress(&isLongPress), isLongPress)
                        && (state->IsTracking(this, &isTracking), isTracking)) {
                    receiver->OnKeyLongPress(mKeyCode, this, &res);
                    if (res) {
                        if (DEBUG) {
                            Logger::D(TAG, "  Clear from Int64 press!");
                        }

                        state->PerformedLongPress(this);
                    }
                }
            }
        }
        break;
    case ACTION_UP: {
            if (state != NULL) {
                state->HandleUpEvent(this);
            }

            FAIL_RETURN(receiver->OnKeyUp(mKeyCode, this, &res));
        }
        break;
    case ACTION_MULTIPLE: {
            Int32 count = mRepeatCount;
            Int32 code = mKeyCode;

            receiver->OnKeyMultiple(code, count, this, &res);
            if (res) {
                break;
            }

            if (code != KEYCODE_UNKNOWN) {
                mAction = ACTION_DOWN;
                mRepeatCount = 0;

                Boolean handled = FALSE;
                receiver->OnKeyDown(code, this, &handled);
                if (handled) {
                    mAction = ACTION_UP;
                    receiver->OnKeyUp(code, this, &res);
                }
                mAction = ACTION_MULTIPLE;
                mRepeatCount = count;
                res = handled;
            }
        }
        break;
    default:
        break;
    }

    *result = res;
    return NOERROR;
}

String KeyEvent::ToString()
{
    StringBuilder msg;
    msg.Append("KeyEvent { action=");
    msg.Append(ActionToString(mAction));
    msg.Append(", keyCode=");
    msg.Append(KeyCodeToString(mKeyCode));
    msg.Append(", scanCode=");
    msg.Append(mScanCode);
    if (!mCharacters.IsNull()) {
        msg.Append(", characters=\"");
        msg.Append(mCharacters);
        msg.Append("\"");
    }
    msg.Append(", metaState=");
    msg.Append(MetaStateToString(mMetaState));
    msg.Append(", flags=0x");
    msg.Append(StringUtils::ToHexString(mFlags));
    msg.Append(", repeatCount=");
    msg.Append(mRepeatCount);
    msg.Append(", eventTime=");
    msg.Append(mEventTime);
    msg.Append(", downTime=");
    msg.Append(mDownTime);
    msg.Append(", deviceId=");
    msg.Append(mDeviceId);
    msg.Append(", source=0x");
    msg.Append(StringUtils::ToHexString(mSource));
    msg.Append(" }");
    return msg.ToString();
}

String KeyEvent::ActionToString(
    /* [in] */ Int32 action)
{
    switch (action) {
        case ACTION_DOWN:
            return String("ACTION_DOWN");
        case ACTION_UP:
            return String("ACTION_UP");
        case ACTION_MULTIPLE:
            return String("ACTION_MULTIPLE");
        default:
            return StringUtils::ToString(action);
    }
}

String KeyEvent::KeyCodeToString(
    /* [in] */ Int32 keyCode)
{
    String symbolicName = NativeKeyCodeToString(keyCode);
    return symbolicName.IsNull() ? StringUtils::ToString(keyCode) : LABEL_PREFIX + symbolicName;
}

ECode KeyEvent::KeyCodeFromString(
    /* [in] */ const String& symbolicName,
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode);

    String name(symbolicName);
    if (name.StartWith(LABEL_PREFIX)) {
        name = symbolicName.Substring(LABEL_PREFIX.GetLength());
        Int32 data = NativeKeyCodeFromString(name);
        if (data > 0) {
            *keyCode = data;
            return NOERROR;
        }
    }

    *keyCode = StringUtils::ParseInt32(name, 10);

    return NOERROR;
    // try {
    //     return Integer.parseInt(symbolicName, 10);
    // } catch (NumberFormatException ex) {
    //     return KEYCODE_UNKNOWN;
    // }
}

String KeyEvent::MetaStateToString(
    /* [in] */ Int32 metaState)
{
    if (metaState == 0) {
        return String("0");
    }
    AutoPtr<StringBuilder> result;
    Int32 i = 0;
    while (metaState != 0) {
        Boolean isSet = (metaState & 1) != 0;
        metaState = (metaState >> 1) & 0x7FFFFFFF; // unsigned shift!
        if (isSet) {
            String name = META_SYMBOLIC_NAMES[i];
            if (result == NULL) {
                if (metaState == 0) {
                    return name;
                }
                result = new StringBuilder(name);
            }
            else {
                result->AppendChar('|');
                result->Append(name);
            }
        }
        i += 1;
    }
    return result->ToString();
}

ECode KeyEvent::CreateFromParcelBody(
    /* [in] */ IParcel* in,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent)
    AutoPtr<CKeyEvent> event;
    CKeyEvent::NewByFriend((CKeyEvent**)&event);
    event->ReadFromParcel(in);
    *newEvent = (IKeyEvent*)event.Get();
    REFCOUNT_ADD(*newEvent);
    return NOERROR;
}

ECode KeyEvent::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    VALIDATE_NOT_NULL(source);

    Int32 token;
    FAIL_RETURN(source->ReadInt32(&token));

    FAIL_RETURN(source->ReadInt32(&mDeviceId));
    FAIL_RETURN(source->ReadInt32(&mSource));
    FAIL_RETURN(source->ReadInt32(&mAction));
    FAIL_RETURN(source->ReadInt32(&mKeyCode));
    FAIL_RETURN(source->ReadInt32(&mRepeatCount));
    FAIL_RETURN(source->ReadInt32(&mMetaState));
    FAIL_RETURN(source->ReadInt32(&mScanCode));
    FAIL_RETURN(source->ReadInt32(&mFlags));
    FAIL_RETURN(source->ReadInt64(&mDownTime));
    FAIL_RETURN(source->ReadInt64(&mEventTime));

    return NOERROR;
}

ECode KeyEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(PARCEL_TOKEN_KEY_EVENT));

    FAIL_RETURN(dest->WriteInt32(mDeviceId));
    FAIL_RETURN(dest->WriteInt32(mSource));
    FAIL_RETURN(dest->WriteInt32(mAction));
    FAIL_RETURN(dest->WriteInt32(mKeyCode));
    FAIL_RETURN(dest->WriteInt32(mRepeatCount));
    FAIL_RETURN(dest->WriteInt32(mMetaState));
    FAIL_RETURN(dest->WriteInt32(mScanCode));
    FAIL_RETURN(dest->WriteInt32(mFlags));
    FAIL_RETURN(dest->WriteInt64(mDownTime));
    FAIL_RETURN(dest->WriteInt64(mEventTime));

    return NOERROR;
}

String KeyEvent::NativeKeyCodeToString(
    /* [in] */ Int32 keyCode)
{
    return String(android::KeyEvent::getLabel(keyCode));
}

Int32 KeyEvent::NativeKeyCodeFromString(
    /* [in] */ const String& keyCode)
{
    return android::KeyEvent::getKeyCodeFromLabel(keyCode.string());
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
