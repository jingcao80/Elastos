
#include "elastos/droid/server/hdmi/PowerStatusMonitorAction.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

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
        super(source);

#endif
}

ECode PowerStatusMonitorAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        QueryPowerStatus();
        return TRUE;
#endif
}

ECode PowerStatusMonitorAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAIT_FOR_REPORT_POWER_STATUS) {
            return FALSE;
        }
        return HandleReportPowerStatus(cmd);
#endif
}

ECode PowerStatusMonitorAction::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 sourceAddress = cmd->GetSource();
        Int32 oldStatus = mPowerStatus->Get(sourceAddress, INVALID_POWER_STATUS);
        if (oldStatus == INVALID_POWER_STATUS) {
            // if no device exists for incoming message, hands it over to other actions.
            return FALSE;
        }
        Int32 newStatus = cmd->GetParams()[0] & 0xFF;
        UpdatePowerStatus(sourceAddress, newStatus, TRUE);
        return TRUE;
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
        for (Int32 i = 0; i < mPowerStatus->Size(); ++i) {
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
            mPowerStatus->Append(info->GetLogicalAddress(), info->GetDevicePowerStatus());
        }
#endif
}

ECode PowerStatusMonitorAction::QueryPowerStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<HdmiDeviceInfo> deviceInfos = Tv()->GetDeviceInfoList(FALSE);
        ResetPowerStatus(deviceInfos);
        for (HdmiDeviceInfo info : deviceInfos) {
            final Int32 logicalAddress = info->GetLogicalAddress();
            SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(GetSourceAddress(),
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
        Tv()->UpdateDevicePowerStatus(logicalAddress, newStatus);

        if (remove) {
            mPowerStatus->Delete(logicalAddress);
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
