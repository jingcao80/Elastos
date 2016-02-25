
#include "elastos/droid/server/hdmi/SendKeyAction.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String SendKeyAction::TAG("SendKeyAction");
const Int32 SendKeyAction::STATE_PROCESSING_KEYCODE = 1;

SendKeyAction::SendKeyAction()
    : mTargetAddress(0)
    , mLastKeycode(0)
{}

ECode SendKeyAction::constructor(
    /* [in] */ HdmiCecLocalDevice* source,
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 keycode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(source);
        mTargetAddress = targetAddress;
        mLastKeycode = keycode;

#endif
}

ECode SendKeyAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendKeyDown(mLastKeycode);
        // finish action for non-repeatable key.
        if (!HdmiCecKeycode->IsRepeatableKey(mLastKeycode)) {
            SendKeyUp();
            Finish();
            return TRUE;
        }
        mState = STATE_PROCESSING_KEYCODE;
        AddTimer(mState, Constants::IRT_MS);
        return TRUE;
#endif
}

ECode SendKeyAction::ProcessKeyEvent(
    /* [in] */ Int32 keycode,
    /* [in] */ Boolean isPressed)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_PROCESSING_KEYCODE) {
            Slogger::W(TAG, "Not in a valid state");
            return;
        }
        // A new key press event that comes in with a key code different from the last
        // one sets becomes a new key code to be used for press-and-hold operation.
        // Removes any pending timer and starts a new timer for itself.
        // Key release event indicates that the action shall be finished. Send UCR
        // command and terminate the action. Other release events are ignored.
        if (isPressed) {
            if (keycode != mLastKeycode) {
                SendKeyDown(keycode);
                if (!HdmiCecKeycode->IsRepeatableKey(keycode)) {
                    SendKeyUp();
                    Finish();
                    return;
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
#endif
}

ECode SendKeyAction::SendKeyDown(
    /* [in] */ Int32 keycode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 cecKeycode = HdmiCecKeycode->AndroidKeyToCecKey(keycode);
        if (cecKeycode == HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildUserControlPressed(GetSourceAddress(),
                mTargetAddress, new Byte[] { (Byte) (cecKeycode & 0xFF) }));
#endif
}

ECode SendKeyAction::SendKeyUp()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildUserControlReleased(GetSourceAddress(),
                mTargetAddress));
#endif
}

ECode SendKeyAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Send key action doesn't need any incoming CEC command, hence does not consume it.
        return FALSE;
#endif
}

ECode SendKeyAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Timer event occurs every IRT_MS milliseconds to perform key-repeat (or press-and-hold)
        // operation. If the last received key code is as same as the one with which the action
        // is started, plus there was no key release event in last IRT_MS timeframe, send a UCP
        // command and start another timer to schedule the next press-and-hold command.
        if (mState != STATE_PROCESSING_KEYCODE) {
            Slogger::W(TAG, "Not in a valid state");
            return;
        }
        SendKeyDown(mLastKeycode);
        AddTimer(mState, Constants::IRT_MS);
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
