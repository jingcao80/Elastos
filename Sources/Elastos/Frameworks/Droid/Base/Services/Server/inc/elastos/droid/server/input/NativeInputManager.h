
#ifndef __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTMANAGER_H__
#define __ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTMANAGER_H__

#include "InputApplicationHandle.h"
#include "InputWindowHandle.h"
#include <utils/Looper.h>
#include <input/InputManager.h>
#include <input/PointerController.h>
#include <input/SpriteController.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class CInputManagerService;

class NativeInputManager
    : public virtual android::RefBase
    , public virtual android::InputReaderPolicyInterface
    , public virtual android::InputDispatcherPolicyInterface
    , public virtual android::PointerControllerPolicyInterface
{
protected:
    virtual ~NativeInputManager();

public:
    NativeInputManager(
        /* [in] */ IContext* context,
        /* [in] */ CInputManagerService* service,
        /* [in] */ const android::sp<android::Looper>& looper);

    inline android::sp<android::InputManager> getInputManager() const
    { return mInputManager; }

    void dump(android::String8& dump);

    void setDisplayViewport(
        /* [in] */ bool external,
        /* [in] */ const android::DisplayViewport& viewport);

    android::status_t registerInputChannel(
        /* [in] */ const android::sp<android::InputChannel>& inputChannel,
        /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
        /* [in] */ bool monitor);

    android::status_t unregisterInputChannel(
        /* [in] */ const android::sp<android::InputChannel>& inputChannel);

    void setInputWindows(
        /* [in] */ ArrayOf<InputWindowHandle*>* windows);

    void setFocusedApplication(
        /* [in] */ InputApplicationHandle* application);

    void setInputDispatchMode(
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean frozen);

    void setSystemUiVisibility(
        /* [in] */ int32_t visibility);

    void setPointerSpeed(
        /* [in] */ int32_t speed);

    void setShowTouches(
        /* [in] */ bool enabled);

    /* --- InputReaderPolicyInterface implementation --- */
    virtual void getReaderConfiguration(
        /* [in] */ android::InputReaderConfiguration* outConfig);

    virtual android::sp<android::PointerControllerInterface> obtainPointerController(
        /* [in] */ int32_t deviceId);

    /* Notifies the input reader policy that some input devices have changed
     * and provides information about all current input devices.
     */
    virtual void notifyInputDevicesChanged(
        /* [in] */ const android::Vector<android::InputDeviceInfo>& inputDevices);

    /* Gets the keyboard layout for a particular input device. */
    virtual android::sp<android::KeyCharacterMap> getKeyboardLayoutOverlay(
        /* [in] */ const android::String8& inputDeviceDescriptor);

    /* Gets a user-supplied alias for a particular input device, or an empty string if none. */
    virtual android::String8 getDeviceAlias(
        /* [in] */ const android::InputDeviceIdentifier& identifier);

    /* --- InputDispatcherPolicyInterface implementation --- */
    virtual void notifySwitch(
        /* [in] */ nsecs_t when,
        /* [in] */ uint32_t switchCode,
        /* [in] */ uint32_t switchValue,
        /* [in] */ uint32_t policyFlags);

    virtual void notifyConfigurationChanged(
        /* [in] */ nsecs_t when);

    virtual nsecs_t notifyANR(
        /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
        /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle);

    virtual void notifyInputChannelBroken(
        /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle);

    virtual bool filterInputEvent(
        /* [in] */ const android::InputEvent* inputEvent,
        /* [in] */ uint32_t policyFlags);

    virtual void getDispatcherConfiguration(
        /* [in] */ android::InputDispatcherConfiguration* outConfig);

    virtual bool isKeyRepeatEnabled();

    virtual void interceptKeyBeforeQueueing(
        /* [in] */ const android::KeyEvent* keyEvent,
        /* [in] */ uint32_t& policyFlags);

    virtual void interceptMotionBeforeQueueing(
        /* [in] */ nsecs_t when,
        /* [in] */ uint32_t& policyFlags);

    virtual nsecs_t interceptKeyBeforeDispatching(
        /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
        /* [in] */ const android::KeyEvent* keyEvent,
        /* [in] */ uint32_t policyFlags);

    virtual bool dispatchUnhandledKey(
        /* [in] */ const android::sp<android::InputWindowHandle>& inputWindowHandle,
        /* [in] */ const android::KeyEvent* keyEvent,
        /* [in] */ uint32_t policyFlags,
        /* [in] */ android::KeyEvent* outFallbackKeyEvent);

    virtual void pokeUserActivity(
        /* [in] */ nsecs_t eventTime,
        /* [in] */ int32_t eventType);

    virtual bool checkInjectEventsPermissionNonReentrant(
        /* [in] */ int32_t injectorPid,
        /* [in] */ int32_t injectorUid);

    /* --- PointerControllerPolicyInterface implementation --- */

    virtual void loadPointerResources(
        /* [in] */ android::PointerResources* outResources);

private:
    void updateInactivityTimeoutLocked(
        /* [in] */ const android::sp<android::PointerController>& controller);

    void handleInterceptActions(
        /* [in] */ Int32 wmActions,
        /* [in] */ nsecs_t when,
        /* [in] */ uint32_t& policyFlags);

    void ensureSpriteControllerLocked();

    // Power manager interactions.
    bool isScreenOn();
    bool isScreenBright();
    void tempWakeUp(nsecs_t eventTime);
    bool isBootFastStatus();
    bool isPowered();

private:
    android::sp<android::InputManager> mInputManager;

    AutoPtr<IContext> mContext;
    CInputManagerService* mService;
    android::sp<android::Looper> mLooper;

    android::Mutex mLock;
    struct Locked {
        // Display size information.
        android::DisplayViewport mInternalViewport;
        android::DisplayViewport mExternalViewport;

        // System UI visibility.
        int32_t mSystemUiVisibility;

        // Pointer speed.
        int32_t mPointerSpeed;

        // True if pointer gestures are enabled.
        bool mPointerGesturesEnabled;

        // Show touches feature enable/disable.
        bool mShowTouches;

        // Sprite controller singleton, created on first use.
        android::sp<android::SpriteController> mSpriteController;

        // Pointer controller singleton, created and destroyed as needed.
        android::wp<android::PointerController> mPointerController;
    } mLocked;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_NATIVEINPUTMANAGER_H__
