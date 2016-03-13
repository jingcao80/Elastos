
#include "elastos/droid/server/hdmi/PowerStatusMonitorAction.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(PowerStatusMonitorAction, HdmiCecFeatureAction, IPowerStatusMonitorAction)

const String PowerStatusMonitorAction::TAG("PowerStatusMonitorAction");
const Int32 PowerStatusMonitorAction::STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;
const Int32 PowerStatusMonitorAction::STATE_WAIT_FOR_NEXT_MONITORING = 2;
const Int32 PowerStatusMonitorAction::INVALID_POWER_STATUS = IHdmiControlManager::POWER_STATUS_UNKNOWN - 1;
const Int32 PowerStatusMonitorAction::MONITIROING_INTERNAL_MS = 60000;
const Int32 PowerStatusMonitorAction::REPORT_POWER_STATUS_TIMEOUT_MS = 5000;

PowerStatusMonitorAction::PowerStatusMonitorAction()
{
#if 0 // TODO: Translate codes below
    mPowerStatus = new SparseInt32Array();
#endif
}

ECode PowerStatusMonitorAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);

#endif
}

ECode PowerStatusMonitorAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        QueryPowerStatus();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode PowerStatusMonitorAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAIT_FOR_REPORT_POWER_STATUS) {
            *result = FALSE;
            return NOERROR;
        }
        return HandleReportPowerStatus(cmd);
#endif
}

ECode PowerStatusMonitorAction::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        cmd->GetSource(&srcAddr);
        Int32 sourceAddress = srcAddr;
        Int32 oldStatus = mPowerStatus->Get(sourceAddress, INVALID_POWER_STATUS);
        if (oldStatus == INVALID_POWER_STATUS) {
            // if no device exists for incoming message, hands it over to other actions.
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        Int32 newStatus = (*params)[0] & 0xFF;
        UpdatePowerStatus(sourceAddress, newStatus, TRUE);
        *result = TRUE;
        return NOERROR;
#endif
}

ECode PowerStatusMonitorAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (mState) {
            case STATE_WAIT_FOR_NEXT_MONITORING:
                QueryPowerStatus();
                break;
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                HandleTimeout();
                break;
        }
#endif
}

ECode PowerStatusMonitorAction::HandleTimeout()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 size;
        mPowerStatus->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            Int32 logicalAddress = mPowerStatus->KeyAt(i);
            UpdatePowerStatus(logicalAddress, POWER_STATUS_UNKNOWN, FALSE);
        }
        mPowerStatus->Clear();
        mState = STATE_WAIT_FOR_NEXT_MONITORING;
#endif
}

ECode PowerStatusMonitorAction::ResetPowerStatus(
    /* [in] */ IList* deviceInfos)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPowerStatus->Clear();
        for (HdmiDeviceInfo info : deviceInfos) {
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            mPowerStatus->Append(logicalAddr, info->GetDevicePowerStatus());
        }
#endif
}

ECode PowerStatusMonitorAction::QueryPowerStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        List<HdmiDeviceInfo> deviceInfos = tv->GetDeviceInfoList(FALSE);
        ResetPowerStatus(deviceInfos);
        for (HdmiDeviceInfo info : deviceInfos) {
            Int32 logicalAddress;
            info->GetLogicalAddress(&logicalAddress);
            Int32 srcAddr;
            GetSourceAddress(&srcAddr);
            SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(srcAddr,
                    logicalAddress),
                    new SendMessageCallback() {
                        //@Override
                        CARAPI OnSendCompleted(Int32 error) {
                            // If fails to send <Give Device Power Status>,
                            // update power status into UNKNOWN.
                            if (error != Constants::SEND_RESULT_SUCCESS) {
                               UpdatePowerStatus(logicalAddress, POWER_STATUS_UNKNOWN, TRUE);
                            }
                        }
                    });
        }

        mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;

        // Add both timers, monitoring and timeout.
        AddTimer(STATE_WAIT_FOR_NEXT_MONITORING, MONITIROING_INTERNAL_MS);
        AddTimer(STATE_WAIT_FOR_REPORT_POWER_STATUS, REPORT_POWER_STATUS_TIMEOUT_MS);
#endif
}

ECode PowerStatusMonitorAction::UpdatePowerStatus(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 newStatus,
    /* [in] */ Boolean remove)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->UpdateDevicePowerStatus(logicalAddress, newStatus);

        if (remove) {
            mPowerStatus->Delete(logicalAddress);
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
