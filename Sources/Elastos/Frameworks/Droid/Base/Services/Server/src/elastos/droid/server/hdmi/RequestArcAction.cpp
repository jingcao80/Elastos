
#include "elastos/droid/server/hdmi/RequestArcAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(RequestArcAction, HdmiCecFeatureAction, IRequestArcAction)

const String RequestArcAction::TAG("RequestArcAction");
const Int32 RequestArcAction::STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE = 1;

RequestArcAction::RequestArcAction()
    : mAvrAddress(0)
{}

ECode RequestArcAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiUtils->VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
        HdmiUtils->VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
        mAvrAddress = avrAddress;
#endif
}

ECode RequestArcAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE
                || !HdmiUtils->CheckCommandSource(cmd, mAvrAddress, TAG)) {
            *result = FALSE;
            return NOERROR;
        }
        Int32 opcode;
        cmd->GetOpcode(&opcode);
        switch (opcode) {
            // Handles only <Feature Abort> here and, both <Initiate ARC> and <Terminate ARC>
            // are handled in HdmiControlService itself because both can be
            // received without <Request ARC Initiation> or <Request ARC Termination>.
            case Constants::MESSAGE_FEATURE_ABORT:
                AutoPtr<ArrayOf<Byte> > params;
                cmd->GetParams((ArrayOf<Byte>**)&params);
                Int32 originalOpcode = (*params)[0] & 0xFF;
                if (originalOpcode == Constants::MESSAGE_REQUEST_ARC_INITIATION
                        || originalOpcode == Constants::MESSAGE_REQUEST_ARC_TERMINATION) {
                    DisableArcTransmission();
                    Finish();
                    *result = TRUE;
                    return NOERROR;
                } else {
                    *result = FALSE;
                    return NOERROR;
                }
        }
        *result = FALSE;
        return NOERROR;
#endif
}

ECode RequestArcAction::DisableArcTransmission()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Start Set ARC Transmission State action.
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(LocalDevice(),
                mAvrAddress, FALSE);
        AddAndStartAction(action);
#endif
}

ECode RequestArcAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state || state != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE) {
            return NOERROR;
        }
        HdmiLogger->Debug("[T]RequestArcAction.");
        DisableArcTransmission();
        Finish();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
