
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/SendKeyAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SendKeyAction, HdmiCecFeatureAction, ISendKeyAction)

const String SendKeyAction::TAG("SendKeyAction");
const Int32 SendKeyAction::STATE_PROCESSING_KEYCODE = 1;

SendKeyAction::SendKeyAction()
    : mTargetAddress(0)
    , mLastKeycode(0)
{}

ECode SendKeyAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 keycode)
{
    HdmiCecFeatureAction::constructor(source);
    mTargetAddress = targetAddress;
    mLastKeycode = keycode;
    return NOERROR;
}

ECode SendKeyAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    SendKeyDown(mLastKeycode);
    // finish action for non-repeatable key.
    Boolean isRepeatableKey;
    HdmiCecKeycode::IsRepeatableKey(mLastKeycode, &isRepeatableKey);
    if (!isRepeatableKey) {
        SendKeyUp();
        Finish();
        *result = TRUE;
        return NOERROR;
    }
    mState = STATE_PROCESSING_KEYCODE;
    AddTimer(mState, Constants::IRT_MS);
    *result = TRUE;
    return NOERROR;
}

ECode SendKeyAction::ProcessKeyEvent(
    /* [in] */ Int32 keycode,
    /* [in] */ Boolean isPressed)
{
    if (mState != STATE_PROCESSING_KEYCODE) {
        Slogger::W(TAG, "Not in a valid state");
        return NOERROR;
    }
    // A new key press event that comes in with a key code different from the last
    // one sets becomes a new key code to be used for press-and-hold operation.
    // Removes any pending timer and starts a new timer for itself.
    // Key release event indicates that the action shall be finished. Send UCR
    // command and terminate the action. Other release events are ignored.
    if (isPressed) {
        if (keycode != mLastKeycode) {
            SendKeyDown(keycode);
            Boolean isRepeatableKey;
            HdmiCecKeycode::IsRepeatableKey(keycode, &isRepeatableKey);
            if (!isRepeatableKey) {
                SendKeyUp();
                Finish();
                return NOERROR;
            }
            mActionTimer->ClearTimerMessage();
            AddTimer(mState, Constants::IRT_MS);
            mLastKeycode = keycode;
        }
    } else {
        if (keycode == mLastKeycode) {
            SendKeyUp();
            Finish();
        }
    }
    return NOERROR;
}

ECode SendKeyAction::SendKeyDown(
    /* [in] */ Int32 keycode)
{
    Int32 cecKeycode;
    HdmiCecKeycode::ElastosKeyToCecKey(keycode, &cecKeycode);
    if (cecKeycode == HdmiCecKeycode::UNSUPPORTED_KEYCODE) {
        return NOERROR;
    }
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(1);
    (*array)[0] = (Byte) (cecKeycode & 0xFF);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildUserControlPressed(srcAddr,
            mTargetAddress, array, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode SendKeyAction::SendKeyUp()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildUserControlReleased(srcAddr,
            mTargetAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode SendKeyAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Send key action doesn't need any incoming CEC command, hence does not consume it.
    *result = FALSE;
    return NOERROR;
}

ECode SendKeyAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    // Timer event occurs every IRT_MS milliseconds to perform key-repeat (or press-and-hold)
    // operation. If the last received key code is as same as the one with which the action
    // is started, plus there was no key release event in last IRT_MS timeframe, send a UCP
    // command and start another timer to schedule the next press-and-hold command.
    if (mState != STATE_PROCESSING_KEYCODE) {
        Slogger::W(TAG, "Not in a valid state");
        return NOERROR;
    }
    SendKeyDown(mLastKeycode);
    AddTimer(mState, Constants::IRT_MS);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
