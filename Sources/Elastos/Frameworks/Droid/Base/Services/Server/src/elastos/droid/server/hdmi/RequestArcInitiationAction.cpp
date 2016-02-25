
#include "elastos/droid/server/hdmi/RequestArcInitiationAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String RequestArcInitiationAction::TAG("RequestArcInitiationAction");

ECode RequestArcInitiationAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(source, avrAddress);

#endif
}

ECode RequestArcInitiationAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);

        HdmiCecMessage command = HdmiCecMessageBuilder->BuildRequestArcInitiation(
                GetSourceAddress(), mAvrAddress);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    // If failed to send <Request ARC Initiation>, start "Disabled"
                    // ARC transmission action.
                    DisableArcTransmission();
                    Finish();
                }
            }
        });
        return TRUE;
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
