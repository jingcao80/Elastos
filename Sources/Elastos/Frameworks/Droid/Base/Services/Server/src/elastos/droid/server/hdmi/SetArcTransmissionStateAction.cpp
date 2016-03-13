
#include "elastos/droid/server/hdmi/SetArcTransmissionStateAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SetArcTransmissionStateAction, HdmiCecFeatureAction, ISetArcTransmissionStateAction)

const String SetArcTransmissionStateAction::TAG("SetArcTransmissionStateAction");
const Int32 SetArcTransmissionStateAction::STATE_WAITING_TIMEOUT = 1;

SetArcTransmissionStateAction::SetArcTransmissionStateAction()
    : mEnabled(FALSE)
    , mAvrAddress(0)
{}

ECode SetArcTransmissionStateAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiUtils->VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
        HdmiUtils->VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
        mAvrAddress = avrAddress;
        mEnabled = enabled;
#endif
}

ECode SetArcTransmissionStateAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mEnabled) {
            // Enable ARC status immediately after sending <Report Arc Initiated>.
            // If AVR responds with <Feature Abort>, disable ARC status again.
            // This is different from spec that says that turns ARC status to
            // "Enabled" if <Report ARC Initiated> is acknowledged and no
            // <Feature Abort> is received.
            // But implemented this way to save the time having to wait for
            // <Feature Abort>.
            SetArcStatus(TRUE);
            // If succeeds to send <Report ARC Initiated>, wait general timeout
            // to check whether there is no <Feature Abort> for <Report ARC Initiated>.
            mState = STATE_WAITING_TIMEOUT;
            AddTimer(mState, HdmiConfig::TIMEOUT_MS);
            SendReportArcInitiated();
        } else {
            SetArcStatus(FALSE);
            Finish();
        }
        *result = TRUE;
        return NOERROR;
#endif
}

ECode SetArcTransmissionStateAction::SendReportArcInitiated()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiCecMessage command = HdmiCecMessageBuilder->BuildReportArcInitiated(srcAddr, mAvrAddress);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    // If fails to send <Report ARC Initiated>, disable ARC and
                    // send <Report ARC Terminated> directly.
                    SetArcStatus(FALSE);
                    HdmiLogger->Debug("Failed to send <Report Arc Initiated>.");
                    Finish();
                }
            }
        });
#endif
}

ECode SetArcTransmissionStateAction::SetArcStatus(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean wasEnabled = tv->SetArcStatus(enabled);
        Slogger::I(TAG, "Change arc status [old:" + wasEnabled + ", new:" + enabled + "]");

        // If enabled before and set to "disabled" and send <Report Arc Terminated> to
        // av reciever.
        if (!enabled && wasEnabled) {
            Int32 srcAddr;
            GetSourceAddress(&srcAddr);
            SendCommand(HdmiCecMessageBuilder->BuildReportArcTerminated(srcAddr,
                    mAvrAddress));
        }
#endif
}

ECode SetArcTransmissionStateAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_TIMEOUT) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 opcode;
        cmd->GetOpcode(&opcode);
        Int32 opcode = opcode;
        if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
            AutoPtr<ArrayOf<Byte> > params;
            cmd->GetParams((ArrayOf<Byte>**)&params);
            Int32 originalOpcode = (*params)[0] & 0xFF;
            if (originalOpcode == Constants::MESSAGE_REPORT_ARC_INITIATED) {
                HdmiLogger->Debug("Feature aborted for <Report Arc Initiated>");
                SetArcStatus(FALSE);
                Finish();
                *result = TRUE;
                return NOERROR;
            }
        }
        *result = FALSE;
        return NOERROR;
#endif
}

ECode SetArcTransmissionStateAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state || mState != STATE_WAITING_TIMEOUT) {
            return NOERROR;
        }
        // Expire timeout for <Feature Abort>.
        Finish();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
