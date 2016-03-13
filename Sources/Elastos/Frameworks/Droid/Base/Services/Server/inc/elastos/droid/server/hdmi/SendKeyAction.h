
#ifndef __ELASTOS_DROID_SERVER_HDMI_SENDKEYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SENDKEYACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDevice;
/**
 * Feature action that transmits remote control key command (User Control Press/
 * User Control Release) to CEC bus.
 *
 * <p>This action is created when a new key event is passed to CEC service. It optionally
 * does key repeat (a.k.a. press-and-hold) operation until it receives a key release event.
 * If another key press event is received before the key in use is released, CEC service
 * does not create a new action but recycles the current one by updating the key used
 * for press-and-hold operation.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
class SendKeyAction
    : public HdmiCecFeatureAction
    , public ISendKeyAction
{
public:
    CAR_INTERFACE_DECL()

    SendKeyAction();

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param targetAddress logical address of the device to send the keys to
     * @param keycode remote control key code as defined in {@link KeyEvent}
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 targetAddress,
        /* [in] */ Int32 keycode);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    /**
     * Called when a key event should be handled for the action.
     *
     * @param keycode key code of {@link KeyEvent} object
     * @param isPressed true if the key event is of {@link KeyEvent#ACTION_DOWN}
     */
    CARAPI ProcessKeyEvent(
        /* [in] */ Int32 keycode,
        /* [in] */ Boolean isPressed);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

private:
    CARAPI SendKeyDown(
        /* [in] */ Int32 keycode);

    CARAPI SendKeyUp();

private:
    static const String TAG;

    // State in which the action is at work. The state is set in {@link #Start()} and
    // persists throughout the process till it is set back to {@code STATE_NONE} at the end.
    static const Int32 STATE_PROCESSING_KEYCODE;

    // Logical address of the device to which the UCP/UCP commands are sent.
    Int32 mTargetAddress;

    // The key code of the last key press event the action is passed via processKeyEvent.
    Int32 mLastKeycode;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SENDKEYACTION_H__
