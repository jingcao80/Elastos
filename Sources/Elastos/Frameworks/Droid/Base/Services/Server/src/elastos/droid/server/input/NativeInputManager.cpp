
#include "elastos/droid/server/input/NativeInputManager.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"
#include "elastos/droid/server/input/NativeInputApplicationHandle.h"
#include "elastos/droid/server/input/NativeInputWindowHandle.h"
#include "elastos/droid/server/power/PowerManagerService.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::CInputDevice;
using Elastos::Droid::View::CPointerIconHelper;
using Elastos::Droid::View::IPointerIconHelper;
using Elastos::Droid::Hardware::Input::CInputDeviceIdentifier;
using Elastos::Droid::Server::Power::PowerManagerService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

// The exponent used to calculate the pointer speed scaling factor.
// The scaling factor is calculated as 2 ^ (speed * exponent),
// where the speed ranges from -7 to + 7 and is supplied by the user.
static const Float POINTER_SPEED_EXPONENT = 1.0f / 4;

static AutoPtr<InputApplicationHandle> GetInputApplicationHandleObj(
    /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle)
{
    if (inputApplicationHandle == NULL) {
        return NULL;
    }
    NativeInputApplicationHandle* obj = static_cast<NativeInputApplicationHandle*>(inputApplicationHandle.get());
    return obj->getInputApplicationHandleObj();
}

static AutoPtr<InputWindowHandle> GetInputWindowHandleObj(
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
    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    AutoPtr<IKeyEvent> eventObj;
    ECode ec = helper->Obtain(nanoseconds_to_milliseconds(event->getDownTime()),
            nanoseconds_to_milliseconds(event->getEventTime()),
            event->getAction(),
            event->getKeyCode(),
            event->getRepeatCount(),
            event->getMetaState(),
            event->getDeviceId(),
            event->getScanCode(),
            event->getFlags(),
            event->getSource(),
            String(NULL),
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
    eventObj->GetKeyboardDevice(&deviceId);
    Int32 source;
    IInputEvent::Probe(eventObj)->GetSource(&source);
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
    IInputEvent::Probe(eventObj)->GetEventTime(&eventTime);

    event->initialize(deviceId, source, action, flags, keyCode, scanCode, metaState, repeatCount,
            milliseconds_to_nanoseconds(downTime),
            milliseconds_to_nanoseconds(eventTime));
    return android::OK;
}

static AutoPtr<IMotionEvent> CreateMotionEventFromNative(
    /* [in] */ const android::MotionEvent* event)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> eventObj;
    ECode ec = helper->Obtain((IMotionEvent**)&eventObj);
    if (FAILED(ec)) {
        return NULL;
    }

    Handle64 native;
    eventObj->GetNative(&native);
    android::MotionEvent* destEvent = reinterpret_cast<android::MotionEvent*>(native);
    if (!destEvent) {
        destEvent = new android::MotionEvent();
        eventObj->SetNative(reinterpret_cast<Handle64>(destEvent));
    }

    destEvent->copyFrom(event, true);
    return eventObj;
}

enum {
    WM_ACTION_PASS_TO_USER = 1,
};

NativeInputManager::NativeInputManager(
    /* [in] */ IContext* context,
    /* [in] */ CInputManagerService* service,
    /* [in] */ const android::sp<android::Looper>& looper)
    : mContext(context)
    , mService(service)
    , mLooper(looper)
    , mInteractive(TRUE)
{
    {
        android::AutoMutex _l(mLock);

        mLocked.mSystemUiVisibility = ASYSTEM_UI_VISIBILITY_STATUS_BAR_VISIBLE;
        mLocked.mPointerSpeed = 0;
        mLocked.mPointerGesturesEnabled = true;
        mLocked.mShowTouches = false;
        mLocked.mStylusIconEnabled = false;
        mLocked.mVolumeKeysRotationMode = 0;
    }

    android::sp<android::EventHub> eventHub = new android::EventHub();
    mInputManager = new android::InputManager(eventHub, this, this);
}

NativeInputManager::~NativeInputManager()
{
    mContext = NULL;
    mService = NULL;
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
        outConfig->stylusIconEnabled = mLocked.mStylusIconEnabled;
        outConfig->volumeKeysRotationMode = mLocked.mVolumeKeysRotationMode;

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
        Handle64 b = 0;
        elBitmap->GetNativeBitmap(&b);
        SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(b);
        if (bitmap) {
            outPointerIcon->bitmap = *bitmap; // use a shared pixel ref
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

/*   KeyCharacterMap   */
class NativeKeyCharacterMap
{
public:
    NativeKeyCharacterMap(int32_t deviceId, const android::sp<android::KeyCharacterMap>& map) :
        mDeviceId(deviceId), mMap(map)
    {
    }

    ~NativeKeyCharacterMap() {
    }

    inline int32_t getDeviceId() const {
        return mDeviceId;
    }

    inline const android::sp<android::KeyCharacterMap>& getMap() const {
        return mMap;
    }

private:
    int32_t mDeviceId;
    android::sp<android::KeyCharacterMap> mMap;
};

static AutoPtr<IKeyCharacterMap> KeyCharacterMap_create(
    /* [in] */ int32_t deviceId,
    /* [in] */ const android::sp<android::KeyCharacterMap>& kcm)
{
    NativeKeyCharacterMap* map = new NativeKeyCharacterMap(deviceId,
            kcm.get() ? kcm : android::KeyCharacterMap::empty());
    if (!map) {
        return NULL;
    }

    AutoPtr<IKeyCharacterMap> elKcm;
    CKeyCharacterMap::New(reinterpret_cast<Int64>(map), (IKeyCharacterMap**)&elKcm);
    return elKcm;
}

static AutoPtr<IInputDevice> InputDevice_create(
    /* [in] */ const android::InputDeviceInfo& deviceInfo)
{
    String nameObj(deviceInfo.getDisplayName().string());
    if (nameObj.IsNull()) {
        return NULL;
    }

    String descriptorObj(deviceInfo.getIdentifier().descriptor.string());
    if (descriptorObj.IsNull()) {
        return NULL;
    }

    AutoPtr<IKeyCharacterMap> kcmObj = KeyCharacterMap_create(
        deviceInfo.getId(), deviceInfo.getKeyCharacterMap());
    if (kcmObj == NULL) {
        return NULL;
    }

    const android::InputDeviceIdentifier& ident = deviceInfo.getIdentifier();

    AutoPtr<IInputDevice> inputDevice;
    CInputDevice::New(
        deviceInfo.getId(), deviceInfo.getGeneration(),
        deviceInfo.getControllerNumber(), nameObj,
        static_cast<int32_t>(ident.vendor), static_cast<int32_t>(ident.product),
        descriptorObj, deviceInfo.isExternal(), deviceInfo.getSources(),
        deviceInfo.getKeyboardType(), kcmObj.Get(), deviceInfo.hasVibrator(),
        deviceInfo.hasButtonUnderPad(), (IInputDevice**)&inputDevice);

    const android::Vector<android::InputDeviceInfo::MotionRange>& ranges = deviceInfo.getMotionRanges();
    for (size_t i = 0; i < ranges.size(); i++) {
        const android::InputDeviceInfo::MotionRange& range = ranges.itemAt(i);
        inputDevice->AddMotionRange(range.axis, range.source, range.min,
                range.max, range.flat, range.fuzz, range.resolution);
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
            inputDevicesArray->Set(i, inputDevice);
        }

        if (!error) {
            mService->NotifyInputDevicesChanged(inputDevicesArray);
        }
    }
}

/* Gets the keyboard layout for a particular input device. */
android::sp<android::KeyCharacterMap> NativeInputManager::getKeyboardLayoutOverlay(
    /* [in] */ const android::InputDeviceIdentifier& identifier)
{
    android::sp<android::KeyCharacterMap> result;

    String descriptor(identifier.descriptor.string());
    AutoPtr<IInputDeviceIdentifier> identifierObj;
    CInputDeviceIdentifier::New(descriptor, identifier.vendor, identifier.product,
            (IInputDeviceIdentifier**)&identifierObj);
    AutoPtr<ArrayOf<String> > arrayObj = mService->GetKeyboardLayoutOverlay(identifierObj);
    if (arrayObj) {
        String filenameChars = (*arrayObj)[0];
        String contentsChars = (*arrayObj)[1];
        android::KeyCharacterMap::loadContents(
                android::String8(filenameChars.string()),
                android::String8(contentsChars.string()), android::KeyCharacterMap::FORMAT_OVERLAY, &result);
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
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
    /* [in] */ const android::String8& reason)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
   ALOGD("notifyANR");
#endif

    AutoPtr<InputApplicationHandle> inputApplicationHandleObj =
            GetInputApplicationHandleObj(inputApplicationHandle);
    AutoPtr<InputWindowHandle> inputWindowHandleObj =
            GetInputWindowHandleObj(inputWindowHandle);

    Int64 newTimeout = mService->NotifyANR(inputApplicationHandleObj, inputWindowHandleObj, String(reason.string()));
    assert(newTimeout >= 0);

    return newTimeout;
}

void NativeInputManager::notifyInputChannelBroken(
    /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle)
{
#if DEBUG_INPUT_DISPATCHER_POLICY
    ALOGD("notifyInputChannelBroken");
#endif
    AutoPtr<InputWindowHandle> inputWindowHandleObj =
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
                GetNativeInputWindowHandle(windowHandleObj);
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
            GetNativeInputApplicationHandle(applicationHandleObj);
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

void NativeInputManager::setStylusIconEnabled(
    /* [in] */ bool enabled)
{
    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mLocked.mStylusIconEnabled == enabled) {
            return;
        }

        ALOGI("Setting stylus icon enabled to %s.", enabled ? "enabled" : "disabled");
        mLocked.mStylusIconEnabled = enabled;
    } // release lock

    mInputManager->getReader()->requestRefreshConfiguration(
            android::InputReaderConfiguration::CHANGE_STYLUS_ICON_ENABLED);
}

void NativeInputManager::setVolumeKeysRotation(
    /* [in] */ int32_t mode)
{
    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mLocked.mVolumeKeysRotationMode == mode) {
            return;
        }

        ALOGI("Volume keys: rotation mode set to %d.", mode);
        mLocked.mVolumeKeysRotationMode = mode;
    } // release lock

    mInputManager->getReader()->requestRefreshConfiguration(
            android::InputReaderConfiguration::CHANGE_VOLUME_KEYS_ROTATION);
}

void NativeInputManager::setInteractive(
    /* [in] */ bool interactive)
{
    mInteractive = interactive;
}

void NativeInputManager::reloadCalibration()
{
    mInputManager->getReader()->requestRefreshConfiguration(
            android::InputReaderConfiguration::TOUCH_AFFINE_TRANSFORMATION);
}

android::TouchAffineTransformation NativeInputManager::getTouchAffineTransformation(
    /* [in] */ ArrayOf<Float>* matrixArr)
{
    assert(matrixArr->GetLength() == 6);

    android::TouchAffineTransformation transform;
    transform.x_scale  = (*matrixArr)[0];
    transform.x_ymix   = (*matrixArr)[1];
    transform.x_offset = (*matrixArr)[2];
    transform.y_xmix   = (*matrixArr)[3];
    transform.y_scale  = (*matrixArr)[4];
    transform.y_offset = (*matrixArr)[5];

    return transform;
}

android::TouchAffineTransformation NativeInputManager::getTouchAffineTransformation(
    /* [in] */ const android::String8& inputDeviceDescriptor,
    /* [in] */ int32_t surfaceRotation)
{
    String descriptorObj(inputDeviceDescriptor.string());

    AutoPtr<ITouchCalibration> cal;
    mService->GetTouchCalibrationForInputDevice(
            descriptorObj, surfaceRotation, (ITouchCalibration**)&cal);

    AutoPtr< ArrayOf<Float> > matrixArr;
    cal->GetAffineTransform((ArrayOf<Float>**)&matrixArr);

    android::TouchAffineTransformation transform = getTouchAffineTransformation(matrixArr);

    return transform;
}

bool NativeInputManager::filterInputEvent(
    /* [in] */ const android::InputEvent* inputEvent,
    /* [in] */ uint32_t policyFlags)
{
    AutoPtr<IInputEvent> elInputEvent;

    switch (inputEvent->getType()) {
    case AINPUT_EVENT_TYPE_KEY:
         elInputEvent = IInputEvent::Probe(CreateKeyEventFromNative(
             static_cast<const android::KeyEvent*>(inputEvent)));
         break;
    case AINPUT_EVENT_TYPE_MOTION:
         elInputEvent = IInputEvent::Probe(CreateMotionEventFromNative(
             static_cast<const android::MotionEvent*>(inputEvent)));
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
    if (mInteractive) {
        policyFlags |= android::POLICY_FLAG_INTERACTIVE;
    }
    if ((policyFlags & android::POLICY_FLAG_TRUSTED)) {
        nsecs_t when = keyEvent->getEventTime();

        AutoPtr<IKeyEvent> keyEventObj = CreateKeyEventFromNative(keyEvent);
        Int32 wmActions;
        if (keyEventObj) {
            wmActions = mService->InterceptKeyBeforeQueueing(keyEventObj, policyFlags);
            IInputEvent::Probe(keyEventObj)->Recycle();
        }
        else {
            ALOGE("Failed to obtain key event object for interceptKeyBeforeQueueing.");
            wmActions = 0;
        }

        handleInterceptActions(wmActions, when, /*byref*/ policyFlags);
    }
    else {
        if (mInteractive) {
            policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
        }
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
    if (mInteractive) {
        policyFlags |= android::POLICY_FLAG_INTERACTIVE;
    }
    if ((policyFlags & android::POLICY_FLAG_TRUSTED) && !(policyFlags & android::POLICY_FLAG_INJECTED)) {
        if (policyFlags & android::POLICY_FLAG_INTERACTIVE) {
            policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
        }
        else {
            Int32 wmActions = mService->InterceptMotionBeforeQueueingNonInteractive(when, policyFlags);

            handleInterceptActions(wmActions, when, /*byref*/ policyFlags);
        }
    }
    else {
        if (mInteractive) {
            policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
        }
    }
}

void NativeInputManager::handleInterceptActions(
    /* [in] */ Int32 wmActions,
    /* [in] */ nsecs_t when,
    /* [in] */ uint32_t& policyFlags)
{
    if (wmActions & WM_ACTION_PASS_TO_USER) {
        policyFlags |= android::POLICY_FLAG_PASS_TO_USER;
    }
    else {
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
            IInputEvent::Probe(keyEventObj)->Recycle();
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
            IInputEvent::Probe(keyEventObj)->Recycle();

            if (fallbackKeyEventObj) {
                // Note: outFallbackKeyEvent may be the same object as keyEvent.
                if (!KeyEventToNative(fallbackKeyEventObj, outFallbackKeyEvent)) {
                    result = true;
                }
                IInputEvent::Probe(fallbackKeyEventObj)->Recycle();
            }
        }
        else {
            ALOGE("Failed to obtain key event object for dispatchUnhandledKey.");
        }
    }
    return result;
}

void NativeInputManager::pokeUserActivity(
    /* [in] */ nsecs_t eventTime,
    /* [in] */ int32_t eventType)
{
    PowerManagerService::UserActivity(eventTime, eventType);
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
        pointerIcon.bitmap.copyTo(&outSpriteIcon->bitmap, kN32_SkColorType);
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
