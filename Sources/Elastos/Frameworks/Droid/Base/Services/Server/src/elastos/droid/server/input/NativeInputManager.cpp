
#include "input/NativeInputManager.h"
#include "input/CInputManagerService.h"
#include "elastos/droid/view/NativeInputChannel.h"
#include "power/CPowerManagerService.h"
#include <input/InputManager.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

#include <skia/core/SkImageDecoder.h>
#include <skia/core/SkStream.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::CInputDevice;
using Elastos::Droid::View::CPointerIconHelper;
using Elastos::Droid::View::IPointerIconHelper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

#define UNUSED(x) ((void)x)

// The exponent used to calculate the pointer speed scaling factor.
// The scaling factor is calculated as 2 ^ (speed * exponent),
// where the speed ranges from -7 to + 7 and is supplied by the user.
static const Float POINTER_SPEED_EXPONENT = 1.0f / 4;

static InputApplicationHandle* GetInputApplicationHandleObj(
    /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle)
{
    if (inputApplicationHandle == NULL) {
        return NULL;
    }
    NativeInputApplicationHandle* obj = static_cast<NativeInputApplicationHandle*>(inputApplicationHandle.get());
    return obj->getInputApplicationHandleObj();
}

static InputWindowHandle* GetInputWindowHandleObj(
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle)
{
    if (inputWindowHandle == NULL) {
        return NULL;
    }
    return static_cast<NativeInputWindowHandle*>(inputWindowHandle.get())->
            getInputWindowHandleObj();
}

static AutoPtr<IKeyEvent> CreateKeyEventFromNative(
    /* [in] */ const android::KeyEvent* event)
{
    AutoPtr<IKeyEvent> eventObj;
    ECode ec = CKeyEvent::New(nanoseconds_to_milliseconds(event->getDownTime()),
            nanoseconds_to_milliseconds(event->getEventTime()),
            event->getAction(),
            event->getKeyCode(),
            event->getRepeatCount(),
            event->getMetaState(),
            event->getDeviceId(),
            event->getScanCode(),
            event->getFlags(),
            event->getSource(),
            (IKeyEvent**)&eventObj);
    if (FAILED(ec)) {
//        ALOGE("An exception occurred while obtaining a key event.");
        return NULL;
    }
    return eventObj;
}

static android::status_t KeyEventToNative(
    /* [in] */ IKeyEvent* eventObj,
    /* [in] */ android::KeyEvent* event)
{
    Int32 deviceId;
    eventObj->GetDeviceId(&deviceId);
    Int32 source;
    eventObj->GetSource(&source);
    Int32 metaState;
    eventObj->GetMetaState(&metaState);
    Int32 action;
    eventObj->GetAction(&action);
    Int32 keyCode;
    eventObj->GetKeyCode(&keyCode);
    Int32 scanCode;
    eventObj->GetScanCode(&scanCode);
    Int32 repeatCount;
    eventObj->GetRepeatCount(&repeatCount);
    Int32 flags;
    eventObj->GetFlags(&flags);
    Int64 downTime;
    eventObj->GetDownTime(&downTime);
    Int64 eventTime;
    eventObj->GetEventTime(&eventTime);

    event->initialize(deviceId, source, action, flags, keyCode, scanCode, metaState, repeatCount,
            milliseconds_to_nanoseconds(downTime),
            milliseconds_to_nanoseconds(eventTime));
    return android::OK;
}

static AutoPtr<IMotionEvent> CreateMotionEventFromNative(
    /* [in] */ const android::MotionEvent* event)
{
    AutoPtr<IMotionEvent> eventObj;
    if (FAILED(CMotionEvent::New((IMotionEvent**)&eventObj))) {
        return NULL;
    }

    Handle32 native;
    eventObj->GetNative(&native);
    android::MotionEvent* destEvent = (android::MotionEvent*)native;
    if (!destEvent) {
        destEvent = new android::MotionEvent();
        eventObj->SetNative((Handle32)destEvent);
    }

    destEvent->copyFrom(event, true);
    return eventObj;
}

enum {
    WM_ACTION_PASS_TO_USER = 1,
    WM_ACTION_WAKE_UP = 2,
    WM_ACTION_GO_TO_SLEEP = 4,
};

NativeInputManager::NativeInputManager(
    /* [in] */ IContext* context,
    /* [in] */ CInputManagerService* service,
    /* [in] */ const android::sp<android::Looper>& looper)
    : mContext(context)
    , mService(service)
    , mLooper(looper)
{
    {
        android::AutoMutex _l(mLock);

        mLocked.mSystemUiVisibility = ASYSTEM_UI_VISIBILITY_STATUS_BAR_VISIBLE;
        mLocked.mPointerSpeed = 0;
        mLocked.mPointerGesturesEnabled = true;
        mLocked.mShowTouches = false;
    }

    android::sp<android::EventHub> eventHub = new android::EventHub();
    mInputManager = new android::InputManager(eventHub, this, this);
}

NativeInputManager::~NativeInputManager()
{
}

void NativeInputManager::dump(
    /* [in] */ android::String8& dump)
{
    mInputManager->getReader()->dump(dump);
    dump.append("\n");

    mInputManager->getDispatcher()->dump(dump);
    dump.append("\n");
}

void NativeInputManager::setDisplayViewport(
    /* [in] */ bool external,
    /* [in] */ const android::DisplayViewport& viewport)
{
    bool changed = false;
    {
        android::AutoMutex _l(mLock);

        android::DisplayViewport& v = external ? mLocked.mExternalViewport : mLocked.mInternalViewport;
        if (v != viewport) {
            changed = true;
            v = viewport;

            if (!external) {
                android::sp<android::PointerController> controller = mLocked.mPointerController.promote();
                if (controller != NULL) {
                    controller->setDisplayViewport(
                            viewport.logicalRight - viewport.logicalLeft,
                            viewport.logicalBottom - viewport.logicalTop,
                            viewport.orientation);
                }
            }
        }
    }

    if (changed) {
        mInputManager->getReader()->requestRefreshConfiguration(
                android::InputReaderConfiguration::CHANGE_DISPLAY_INFO);
    }
}

android::status_t NativeInputManager::registerInputChannel(
    /* [in] */ const android::sp<android::InputChannel>& inputChannel,
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
    /* [in] */ bool monitor)
{
    return mInputManager->getDispatcher()->registerInputChannel(
            inputChannel, inputWindowHandle, monitor);
}

android::status_t NativeInputManager::unregisterInputChannel(
    /* [in] */ const android::sp<android::InputChannel>& inputChannel)
{
    return mInputManager->getDispatcher()->unregisterInputChannel(inputChannel);
}

void NativeInputManager::getReaderConfiguration(
    /* [in] */ android::InputReaderConfiguration* outConfig)
{
    Int32 virtualKeyQuietTime = mService->GetVirtualKeyQuietTimeMillis();
    outConfig->virtualKeyQuietTime = milliseconds_to_nanoseconds(virtualKeyQuietTime);

    outConfig->excludedDeviceNames.clear();

    AutoPtr<ArrayOf<String> > excludedDeviceNames = mService->GetExcludedDeviceNames();

    Int32 length = excludedDeviceNames->GetLength();
    for (Int32 i = 0; i < length; i++) {
        const char* deviceNameChars = (*excludedDeviceNames)[i].string();
        outConfig->excludedDeviceNames.add(android::String8(deviceNameChars));
    }

    Int32 hoverTapTimeout = mService->GetHoverTapTimeout();
    Int32 doubleTapTimeout = mService->GetDoubleTapTimeout();
    Int32 longPressTimeout = mService->GetLongPressTimeout();
    outConfig->pointerGestureTapInterval = milliseconds_to_nanoseconds(hoverTapTimeout);

    // We must ensure that the tap-drag interval is significantly shorter than
    // the long-press timeout because the tap is held down for the entire duration
    // of the double-tap timeout.
    Int32 tapDragInterval = Elastos::Core::Math::Max(Elastos::Core::Math::Min(longPressTimeout - 100,
            doubleTapTimeout), hoverTapTimeout);
    outConfig->pointerGestureTapDragInterval =
            milliseconds_to_nanoseconds(tapDragInterval);

    Int32 hoverTapSlop = mService->GetHoverTapSlop();
    outConfig->pointerGestureTapSlop = hoverTapSlop;

    { // acquire lock
        android::AutoMutex _l(mLock);

        outConfig->pointerVelocityControlParameters.scale = exp2f(mLocked.mPointerSpeed
                * POINTER_SPEED_EXPONENT);
        outConfig->pointerGesturesEnabled = mLocked.mPointerGesturesEnabled;

        outConfig->showTouches = mLocked.mShowTouches;

        outConfig->setDisplayInfo(false /*external*/, mLocked.mInternalViewport);
        outConfig->setDisplayInfo(true /*external*/, mLocked.mExternalViewport);
    } // release lock
}

/* Pointer icon styles.
 * Must match the definition in android.view.PointerIcon.
 */
enum {
    POINTER_ICON_STYLE_CUSTOM = -1,
    POINTER_ICON_STYLE_NULL = 0,
    POINTER_ICON_STYLE_ARROW = 1000,
    POINTER_ICON_STYLE_SPOT_HOVER = 2000,
    POINTER_ICON_STYLE_SPOT_TOUCH = 2001,
    POINTER_ICON_STYLE_SPOT_ANCHOR = 2002,
};

/*
 * Describes a pointer icon.
 */
struct PointerIcon
{
    inline PointerIcon() {
        reset();
    }

    int32_t style;
    SkBitmap bitmap;
    float hotSpotX;
    float hotSpotY;

    inline bool isNullIcon() {
        return style == POINTER_ICON_STYLE_NULL;
    }

    inline void reset() {
        style = POINTER_ICON_STYLE_NULL;
        bitmap.reset();
        hotSpotX = 0;
        hotSpotY = 0;
    }
};

static android::status_t PointerIcon_load(
    /* [in] */ IPointerIcon* pointerIcon,
    /* [in] */ IContext* context,
    /* [in] */ PointerIcon* outPointerIcon)
{
    outPointerIcon->reset();

    if (!pointerIcon) {
        return android::OK;
    }

    AutoPtr<IPointerIcon> loadedPointerIcon;
    ECode ec = pointerIcon->Load(context, (IPointerIcon**)&loadedPointerIcon);
    if (FAILED(ec) || loadedPointerIcon == NULL) {
        ALOGW("An exception occurred while loading a pointer icon.");
        return android::UNKNOWN_ERROR;
    }

    loadedPointerIcon->GetStyle(&outPointerIcon->style);
    loadedPointerIcon->GetHotSpotX(&outPointerIcon->hotSpotX);
    loadedPointerIcon->GetHotSpotY(&outPointerIcon->hotSpotY);

    AutoPtr<IBitmap> elBitmap;
    loadedPointerIcon->GetBitmap((IBitmap**)&elBitmap);
    if (elBitmap) {
        Handle32 b = 0;
        elBitmap->GetNativeBitmap(&b);
        SkBitmap* bitmap = (SkBitmap*)b;
        if (bitmap) {
            outPointerIcon->bitmap = *(SkBitmap*)bitmap; // use a shared pixel ref
        }
    }

    return android::OK;
}

android::sp<android::PointerControllerInterface> NativeInputManager::obtainPointerController(
    /* [in] */ int32_t deviceId)
{
    android::AutoMutex _l(mLock);

    android::sp<android::PointerController> controller = mLocked.mPointerController.promote();
    if (controller == NULL) {
        ensureSpriteControllerLocked();

        controller = new android::PointerController(this, mLooper, mLocked.mSpriteController);
        mLocked.mPointerController = controller;
        android::DisplayViewport& v = mLocked.mInternalViewport;
        controller->setDisplayViewport(
                v.logicalRight - v.logicalLeft,
                v.logicalBottom - v.logicalTop,
                v.orientation);

        AutoPtr<IPointerIcon> elPointerIcon = mService->GetPointerIcon();
        if (elPointerIcon != NULL) {
            PointerIcon pointerIcon;
            android::status_t status = PointerIcon_load(elPointerIcon, mContext, &pointerIcon);
            if (!status && !pointerIcon.isNullIcon()) {
                controller->setPointerIcon(android::SpriteIcon(pointerIcon.bitmap,
                        pointerIcon.hotSpotX, pointerIcon.hotSpotY));
            } else {
                controller->setPointerIcon(android::SpriteIcon());
            }
        }

        updateInactivityTimeoutLocked(controller);
    }
    return controller;
}

void NativeInputManager::ensureSpriteControllerLocked()
{
    if (mLocked.mSpriteController == NULL) {
        Int32 layer = mService->GetPointerLayer();
        mLocked.mSpriteController = new android::SpriteController(mLooper, layer);
    }
}

static AutoPtr<IKeyCharacterMap> KeyCharacterMap_create(
    /* [in] */ int32_t deviceId,
    /* [in] */ const android::sp<android::KeyCharacterMap>& kcm)
{
    android::KeyCharacterMap* map = kcm.get();
    if (!map)
        map = android::KeyCharacterMap::empty().get();

    AutoPtr<IKeyCharacterMap> elKcm;
    CKeyCharacterMap::New(deviceId, reinterpret_cast<Int32>(map), (IKeyCharacterMap**)&elKcm);

    return elKcm;
}

static AutoPtr<IInputDevice> InputDevice_create(
    /* [in] */ const android::InputDeviceInfo& deviceInfo)
{
    AutoPtr<IKeyCharacterMap> kcm = KeyCharacterMap_create(
        deviceInfo.getId(), deviceInfo.getKeyCharacterMap());

    AutoPtr<IInputDevice> inputDevice;
    CInputDevice::New(
        deviceInfo.getId(), deviceInfo.getGeneration(),
        String(deviceInfo.getDisplayName().string()),
        String(deviceInfo.getIdentifier().descriptor.string()),
        deviceInfo.isExternal(), deviceInfo.getSources(),
        deviceInfo.getKeyboardType(), kcm.Get(), deviceInfo.hasVibrator(),
        (IInputDevice**)&inputDevice);

    const android::Vector<android::InputDeviceInfo::MotionRange>& ranges = deviceInfo.getMotionRanges();
    for (size_t i = 0; i < ranges.size(); i++) {
        const android::InputDeviceInfo::MotionRange& range = ranges.itemAt(i);
        inputDevice->AddMotionRange(
            range.axis, range.source, range.min, range.max, range.flat, range.fuzz);
    }

    return inputDevice;
}

void NativeInputManager::notifyInputDevicesChanged(
    /* [in] */ const android::Vector<android::InputDeviceInfo>& inputDevices)
{
    size_t count = inputDevices.size();
    AutoPtr<ArrayOf<IInputDevice*> > inputDevicesArray =
            ArrayOf<IInputDevice*>::Alloc(count);

    if (inputDevicesArray != NULL) {
        bool error = false;
        for (size_t i = 0; i < count; i++) {
            AutoPtr<IInputDevice> inputDevice = InputDevice_create(inputDevices.itemAt(i));
            if (inputDevice == NULL) {
                error = true;
                break;
            }
            inputDevicesArray->Set(i, inputDevice.Get());
        }

        if (!error) {
            mService->NotifyInputDevicesChanged(inputDevicesArray);
        }
    }
}

/* Gets the keyboard layout for a particular input device. */
android::sp<android::KeyCharacterMap> NativeInputManager::getKeyboardLayoutOverlay(
    /* [in] */ const android::String8& inputDeviceDescriptor)
{
    android::sp<android::KeyCharacterMap> result;
    AutoPtr<ArrayOf<String> > array = mService->GetKeyboardLayoutOverlay(
        String(inputDeviceDescriptor.string()));

    if (array != NULL) {
        android::KeyCharacterMap::loadContents(
            android::String8((*array)[0].string()), android::String8((*array)[1].string()),
            android::KeyCharacterMap::FORMAT_OVERLAY, &result);
    }

    return result;
}

/* Gets a user-supplied alias for a particular input device, or an empty string if none. */
android::String8 NativeInputManager::getDeviceAlias(
    /* [in] */ const android::InputDeviceIdentifier& identifier)
{
    String alias = mService->GetDeviceAlias(String(identifier.uniqueId.string()));
    android::String8 result;
    if (!alias.IsNull()) {
        result.setTo(alias.string());
    }

    return result;
}

void NativeInputManager::notifySwitch(
        /* [in] */ nsecs_t when,
        /* [in] */ uint32_t switchValues,
        /* [in] */ uint32_t switchMask,
        /* [in] */ uint32_t policyFlags)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
   ALOGD("notifySwitch - when=%lld, switchValues=0x%08x, switchMask=0x%08x, policyFlags=0x%x",
            when, switchValues, switchMask, policyFlags);
#endif

    mService->NotifySwitch(when, switchValues, switchMask);
}

void NativeInputManager::notifyConfigurationChanged(
    /* [in] */ nsecs_t when)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
    ALOGD("notifyConfigurationChanged - when=%lld", when);
#endif

   mService->NotifyConfigurationChanged(when);
}

nsecs_t NativeInputManager::notifyANR(
    /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
   ALOGD("notifyANR");
#endif

    InputApplicationHandle* inputApplicationHandleObj =
            GetInputApplicationHandleObj(inputApplicationHandle);
    InputWindowHandle* inputWindowHandleObj =
            GetInputWindowHandleObj(inputWindowHandle);

    Int64 newTimeout = mService->NotifyANR(inputApplicationHandleObj, inputWindowHandleObj);
    assert(newTimeout >= 0);

    return newTimeout;
}

void NativeInputManager::notifyInputChannelBroken(
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
    ALOGD("notifyInputChannelBroken");
#endif
    InputWindowHandle* inputWindowHandleObj =
            GetInputWindowHandleObj(inputWindowHandle);
    if (inputWindowHandleObj) {
        mService->NotifyInputChannelBroken(inputWindowHandleObj);
    }
}

void NativeInputManager::getDispatcherConfiguration(
    /* [in] */ android::InputDispatcherConfiguration* outConfig)
{
    Int32 keyRepeatTimeout = mService->GetKeyRepeatTimeout();
    outConfig->keyRepeatTimeout = milliseconds_to_nanoseconds(keyRepeatTimeout);

    Int32 keyRepeatDelay = mService->GetKeyRepeatDelay();
    outConfig->keyRepeatDelay = milliseconds_to_nanoseconds(keyRepeatDelay);
}

bool NativeInputManager::isKeyRepeatEnabled()
{
    // Only enable automatic key repeating when the screen is on.
    return isScreenOn();
}

void NativeInputManager::setInputWindows(
    /* [in] */ ArrayOf<InputWindowHandle*>* windowHandleObjArray)
{
    android::Vector<android::sp<android::InputWindowHandle> > windowHandles;

    Int32 length = windowHandleObjArray != NULL ? windowHandleObjArray->GetLength() : 0;
    for (Int32 i = 0; i < length; i++) {
        InputWindowHandle* windowHandleObj = (*windowHandleObjArray)[i];
        if (! windowHandleObj) {
            break; // found null element indicating end of used portion of the array
        }

        android::sp<android::InputWindowHandle> windowHandle =
                InputWindowHandle::GetHandle(windowHandleObj);
        if (windowHandle != NULL) {
            windowHandles.push(windowHandle);
        }
    }

    mInputManager->getDispatcher()->setInputWindows(windowHandles);

    // Do this after the dispatcher has updated the window handle state.
    bool newPointerGesturesEnabled = true;
    size_t numWindows = windowHandles.size();
    for (size_t i = 0; i < numWindows; i++) {
        const android::sp<android::InputWindowHandle>& windowHandle = windowHandles.itemAt(i);
        const android::InputWindowInfo* windowInfo = windowHandle->getInfo();
        if (windowInfo && windowInfo->hasFocus && (windowInfo->inputFeatures
                & android::InputWindowInfo::INPUT_FEATURE_DISABLE_TOUCH_PAD_GESTURES)) {
            newPointerGesturesEnabled = false;
        }
    }

    uint32_t changes = 0;
    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mLocked.mPointerGesturesEnabled != newPointerGesturesEnabled) {
            mLocked.mPointerGesturesEnabled = newPointerGesturesEnabled;
            changes |= android::InputReaderConfiguration::CHANGE_POINTER_GESTURE_ENABLEMENT;
        }
    } // release lock

    if (changes) {
        mInputManager->getReader()->requestRefreshConfiguration(changes);
    }
}

void NativeInputManager::setFocusedApplication(
    /* [in] */ InputApplicationHandle* applicationHandleObj)
{
    android::sp<android::InputApplicationHandle> applicationHandle =
            InputApplicationHandle::GetHandle(applicationHandleObj);
    mInputManager->getDispatcher()->setFocusedApplication(applicationHandle);
}

void NativeInputManager::setInputDispatchMode(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean frozen)
{
    mInputManager->getDispatcher()->setInputDispatchMode(enabled, frozen);
}

void NativeInputManager::setSystemUiVisibility(
    /* [in] */ int32_t visibility)
{
    android::AutoMutex _l(mLock);

    if (mLocked.mSystemUiVisibility != visibility) {
        mLocked.mSystemUiVisibility = visibility;

        android::sp<android::PointerController> controller = mLocked.mPointerController.promote();
        if (controller != NULL) {
            updateInactivityTimeoutLocked(controller);
        }
    }
}

void NativeInputManager::updateInactivityTimeoutLocked(
    /* [in] */ const android::sp<android::PointerController>& controller)
{
    bool lightsOut = mLocked.mSystemUiVisibility & ASYSTEM_UI_VISIBILITY_STATUS_BAR_HIDDEN;
    controller->setInactivityTimeout(lightsOut
            ? android::PointerController::INACTIVITY_TIMEOUT_SHORT
            : android::PointerController::INACTIVITY_TIMEOUT_NORMAL);
}

void NativeInputManager::setPointerSpeed(
    /* [in] */ int32_t speed)
{
    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mLocked.mPointerSpeed == speed) {
            return;
        }

        ALOGI("Setting pointer speed to %d.", speed);
        mLocked.mPointerSpeed = speed;
    } // release lock

    mInputManager->getReader()->requestRefreshConfiguration(
            android::InputReaderConfiguration::CHANGE_POINTER_SPEED);
}

void NativeInputManager::setShowTouches(
    /* [in] */ bool enabled)
{
    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mLocked.mShowTouches == enabled) {
            return;
        }

        ALOGI("Setting show touches feature to %s.", enabled ? "enabled" : "disabled");
        mLocked.mShowTouches = enabled;
    } // release lock

    mInputManager->getReader()->requestRefreshConfiguration(
            android::InputReaderConfiguration::CHANGE_SHOW_TOUCHES);
}

bool NativeInputManager::isScreenOn()
{
    return CPowerManagerService::isScreenOn();
}

bool NativeInputManager::isScreenBright()
{
    return CPowerManagerService::isScreenBright();
}

bool NativeInputManager::filterInputEvent(
    /* [in] */ const android::InputEvent* inputEvent,
    /* [in] */ uint32_t policyFlags)
{
    AutoPtr<IInputEvent> elInputEvent;

    switch (inputEvent->getType()) {
    case AINPUT_EVENT_TYPE_KEY:
         elInputEvent = CreateKeyEventFromNative(
             static_cast<const android::KeyEvent*>(inputEvent));
         break;
    case AINPUT_EVENT_TYPE_MOTION:
         elInputEvent = CreateMotionEventFromNative(
             static_cast<const android::MotionEvent*>(inputEvent));
        break;
    default:
        return true; // dispatch the event normally
    }

    if (!elInputEvent) {
        ALOGE("Failed to obtain input event object for filterInputEvent.");
        return true; // dispatch the event normally
    }

    // The callee is responsible for recycling the event.
    return mService->FilterInputEvent(elInputEvent, policyFlags);
}

void NativeInputManager::interceptKeyBeforeQueueing(
    /* [in] */ const android::KeyEvent* keyEvent,
    /* [in] */ uint32_t& policyFlags)
{
    // Policy:
    // - Ignore untrusted events and pass them along.
    // - Ask the window manager what to do with normal events and trusted injected events.
    // - For normal events wake and brighten the screen if currently off or dim.
    if ((policyFlags & android::POLICY_FLAG_TRUSTED)) {
        nsecs_t when = keyEvent->getEventTime();
        bool isScreenOn = this->isScreenOn();
        bool isScreenBright = this->isScreenBright();

        AutoPtr<IKeyEvent> keyEventObj = CreateKeyEventFromNative(keyEvent);
        Int32 wmActions;
        if (keyEventObj) {
            wmActions = mService->InterceptKeyBeforeQueueing(
                    keyEventObj, policyFlags, isScreenOn);
            keyEventObj->Recycle();
        }
        else {
            ALOGE("Failed to obtain key event object for interceptKeyBeforeQueueing.");
            wmActions = 0;
        }

        if (!(policyFlags & android::POLICY_FLAG_INJECTED)) {
            if (!isScreenOn) {
                policyFlags |= android::POLICY_FLAG_WOKE_HERE;
            }

            if (!isScreenBright) {
                policyFlags |= android::POLICY_FLAG_BRIGHT_HERE;
            }
        }

        if (isBootFastStatus()) {
            if (isPowered() == true) {
                if (isScreenOn == false) {
                    if (keyEvent->getKeyCode()==AKEYCODE_POWER) {
                         tempWakeUp(0);
                    }
                }
            }
        }

        handleInterceptActions(wmActions, when, /*byref*/ policyFlags);
    }
    else {
        policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
    }
}

void NativeInputManager::interceptMotionBeforeQueueing(
    /* [in] */ nsecs_t when,
    /* [in] */ uint32_t& policyFlags)
{
    // Policy:
    // - Ignore untrusted events and pass them along.
    // - No special filtering for injected events required at this time.
    // - Filter normal events based on screen state.
    // - For normal events brighten (but do not wake) the screen if currently dim.
    if ((policyFlags & android::POLICY_FLAG_TRUSTED) && !(policyFlags & android::POLICY_FLAG_INJECTED)) {
        if (isScreenOn()) {
            policyFlags |= android::POLICY_FLAG_PASS_TO_USER;

            if (!isScreenBright()) {
                policyFlags |= android::POLICY_FLAG_BRIGHT_HERE;
            }
        }
        else {
            Int32 wmActions = mService->InterceptMotionBeforeQueueingWhenScreenOff(policyFlags);

            policyFlags |= android::POLICY_FLAG_WOKE_HERE | android::POLICY_FLAG_BRIGHT_HERE;
            handleInterceptActions(wmActions, when, /*byref*/ policyFlags);
        }
    }
    else {
        policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
    }
}

void NativeInputManager::handleInterceptActions(
    /* [in] */ Int32 wmActions,
    /* [in] */ nsecs_t when,
    /* [in] */ uint32_t& policyFlags)
{
    // TODO:
    //
    if (wmActions & WM_ACTION_GO_TO_SLEEP) {
#if DEBUG_INPUT_DISPATCHER_POLICY
        ALOGD("handleInterceptActions: Going to sleep.");
#endif
        CPowerManagerService::goToSleep(when);
    }

    if (wmActions & WM_ACTION_WAKE_UP) {
#if DEBUG_INPUT_DISPATCHER_POLICY
        ALOGD("handleInterceptActions: Waking up.");
#endif
        CPowerManagerService::wakeUp(when);
    }

    if (wmActions & WM_ACTION_PASS_TO_USER) {
        policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
    } else {
#if DEBUG_INPUT_DISPATCHER_POLICY
        ALOGD("handleInterceptActions: Not passing key to user.");
#endif
    }
}

nsecs_t NativeInputManager::interceptKeyBeforeDispatching(
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
    /* [in] */ const android::KeyEvent* keyEvent,
    /* [in] */ uint32_t policyFlags)
{
    // Policy:
    // - Ignore untrusted events and pass them along.
    // - Filter normal events and trusted injected events through the window manager policy to
    //   handle the HOME key and the like.
    nsecs_t result = 0;
    if (policyFlags & android::POLICY_FLAG_TRUSTED) {
        // Note: inputWindowHandle may be null.
        InputWindowHandle* inputWindowHandleObj = GetInputWindowHandleObj(inputWindowHandle);
        AutoPtr<IKeyEvent> keyEventObj = CreateKeyEventFromNative(keyEvent);
        if (keyEventObj != NULL) {
            Int64 delayMillis = mService->InterceptKeyBeforeDispatching(
                    inputWindowHandleObj, keyEventObj, policyFlags);
            keyEventObj->Recycle();
            if (delayMillis < 0) {
                result = -1;
            }
            else if (delayMillis > 0) {
                result = milliseconds_to_nanoseconds(delayMillis);
            }
        }
        else {
            ALOGE("Failed to obtain key event object for interceptKeyBeforeDispatching.");
        }
    }
    return result;
}

bool NativeInputManager::dispatchUnhandledKey(
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
    /* [in] */ const android::KeyEvent* keyEvent,
    /* [in] */ uint32_t policyFlags,
    /* [in] */ android::KeyEvent* outFallbackKeyEvent)
{
    // Policy:
    // - Ignore untrusted events and do not perform default handling.
    bool result = false;
    if (policyFlags & android::POLICY_FLAG_TRUSTED) {

        // Note: inputWindowHandle may be null.
        InputWindowHandle* inputWindowHandleObj = GetInputWindowHandleObj(inputWindowHandle);
        AutoPtr<IKeyEvent> keyEventObj = CreateKeyEventFromNative(keyEvent);
        if (keyEventObj != NULL) {
            AutoPtr<IKeyEvent> fallbackKeyEventObj = mService->DispatchUnhandledKey(
                    inputWindowHandleObj, keyEventObj, policyFlags);
            keyEventObj->Recycle();

            if (fallbackKeyEventObj) {
                // Note: outFallbackKeyEvent may be the same object as keyEvent.
                if (!KeyEventToNative(fallbackKeyEventObj, outFallbackKeyEvent)) {
                    result = true;
                }
                fallbackKeyEventObj->Recycle();
            }
        }
        else {
            ALOGE("Failed to obtain key event object for dispatchUnhandledKey.");
        }
    }
    return result;
}

bool NativeInputManager::isBootFastStatus()
{
    return CPowerManagerService::isBootFastStatus();
}

bool NativeInputManager::isPowered()
{
    return CPowerManagerService::isPowered();
}

void NativeInputManager::tempWakeUp(nsecs_t eventTime)
{
    CPowerManagerService::tempWakeuUp(eventTime);
}

void NativeInputManager::pokeUserActivity(
    /* [in] */ nsecs_t eventTime,
    /* [in] */ int32_t eventType)
{
    CPowerManagerService::userActivity(eventTime, eventType);
}

bool NativeInputManager::checkInjectEventsPermissionNonReentrant(
    /* [in] */ int32_t injectorPid,
    /* [in] */ int32_t injectorUid)
{
    Boolean result = mService->CheckInjectEventsPermission(injectorPid, injectorUid);
    return (bool)result;
}

static AutoPtr<IPointerIcon> PointerIcon_getSystemIcon(
    /* [in] */ IContext* context,
    /* [in] */ int32_t style)
{
    AutoPtr<IPointerIconHelper> piHelper;
    CPointerIconHelper::AcquireSingleton((IPointerIconHelper**)&piHelper);
    AutoPtr<IPointerIcon> pointerIcon;
    ECode ec = piHelper->GetSystemIcon(context, style, (IPointerIcon**)&pointerIcon);
    if (FAILED(ec)) {
        ALOGW("An exception occurred while getting a pointer icon with style %d.", style);
        return NULL;
    }

    return pointerIcon;
}

static android::status_t PointerIcon_loadSystemIcon(
    /* [in] */ IContext* context,
    /* [in] */ int32_t style,
    /* [in] */ PointerIcon* outPointerIcon)
{
    AutoPtr<IPointerIcon> pointerIcon = PointerIcon_getSystemIcon(context, style);
    if (!pointerIcon) {
        outPointerIcon->reset();
        return android::UNKNOWN_ERROR;
    }

    android::status_t status = PointerIcon_load(
        pointerIcon, context, outPointerIcon);

    return status;
}

static void loadSystemIconAsSprite(
    /* [in] */ IContext* context,
    /* [in] */ int32_t style,
    /* [in] */ android::SpriteIcon* outSpriteIcon)
{
    PointerIcon pointerIcon;
    android::status_t status = PointerIcon_loadSystemIcon(
        context, style, &pointerIcon);
    if (!status) {
        pointerIcon.bitmap.copyTo(&outSpriteIcon->bitmap, SkBitmap::kARGB_8888_Config);
        outSpriteIcon->hotSpotX = pointerIcon.hotSpotX;
        outSpriteIcon->hotSpotY = pointerIcon.hotSpotY;
    }
}

void NativeInputManager::loadPointerResources(
    /* [in] */ android::PointerResources* outResources)
{
    loadSystemIconAsSprite(mContext, POINTER_ICON_STYLE_SPOT_HOVER,
            &outResources->spotHover);
    loadSystemIconAsSprite(mContext, POINTER_ICON_STYLE_SPOT_TOUCH,
            &outResources->spotTouch);
    loadSystemIconAsSprite(mContext, POINTER_ICON_STYLE_SPOT_ANCHOR,
            &outResources->spotAnchor);
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
