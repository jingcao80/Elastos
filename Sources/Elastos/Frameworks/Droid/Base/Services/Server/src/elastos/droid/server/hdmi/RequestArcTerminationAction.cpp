
#include "elastos/droid/server/hdmi/RequestArcTerminationAction.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(RequestArcTerminationAction, RequestArcAction, IRequestArcTerminationAction)

const String RequestArcTerminationAction::TAG("RequestArcTerminationAction");

ECode RequestArcTerminationAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source, avrAddress);

#endif
}

ECode RequestArcTerminationAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE;
        AddTimer(mState, HdmiConfig::TIMEOUT_MS);

        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiCecMessage command = HdmiCecMessageBuilder->BuildRequestArcTermination(srcAddr, mAvrAddress);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    // If failed to send <Request ARC Termination>, start "Disabled" ARC
                    // transmission action.
                    DisableArcTransmission();
                    Finish();
                }
            }
        });
        *result = TRUE;
        return NOERROR;
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
