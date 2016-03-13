
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/DeviceSelectAction.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// DeviceSelectAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(DeviceSelectAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

DeviceSelectAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ DeviceSelectAction* host)
    : mHost(host)
{}

ECode DeviceSelectAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error == Constants::SEND_RESULT_NAK) {
        mHost->InvokeCallback(IHdmiControlManager::RESULT_COMMUNICATION_FAILED);
        mHost->Finish();
        return NOERROR;
    }
    return NOERROR;
}

//=============================================================================
// DeviceSelectAction
//=============================================================================
CAR_INTERFACE_IMPL(DeviceSelectAction, HdmiCecFeatureAction, IDeviceSelectAction)

const String DeviceSelectAction::TAG("DeviceSelect");
const Int32 DeviceSelectAction::TIMEOUT_TRANSIT_TO_STANDBY_MS = 5 * 1000;
const Int32 DeviceSelectAction::TIMEOUT_POWER_ON_MS = 5 * 1000;
const Int32 DeviceSelectAction::LOOP_COUNTER_MAX = 20;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY = 2;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_DEVICE_POWER_ON = 3;

DeviceSelectAction::DeviceSelectAction()
    : mPowerStatusCounter(0)
{}

ECode DeviceSelectAction::constructor(
    /* [in] */ IHdmiCecLocalDeviceTv* source,
    /* [in] */ IHdmiDeviceInfo* target,
    /* [in] */ IIHdmiControlCallback* callback)
{
    HdmiCecFeatureAction::constructor(IHdmiCecLocalDevice::Probe(source));
    mCallback = callback;
    mTarget = target;
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    Int32 targetAddr;
    GetTargetAddress(&targetAddr);
    HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(srcAddr, targetAddr, (IHdmiCecMessage**)&mGivePowerStatus);
    return NOERROR;
}

ECode DeviceSelectAction::GetTargetAddress(
    /* [out] */ Int32* result)
{
    return mTarget->GetLogicalAddress(result);
}

ECode DeviceSelectAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Seq #9
    QueryDevicePowerStatus();
    *result = TRUE;
    return NOERROR;
}

ECode DeviceSelectAction::QueryDevicePowerStatus()
{
    SendCommand(mGivePowerStatus, new InnerSub_SendMessageCallback(this));
    mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    return NOERROR;
}

ECode DeviceSelectAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    Int32 targetAddr;
    GetTargetAddress(&targetAddr);
    if (srcAddr != targetAddr) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 opcode;
    cmd->GetOpcode(&opcode);
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    switch (mState) {
        case STATE_WAIT_FOR_REPORT_POWER_STATUS:
            if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
                return HandleReportPowerStatus((*params)[0], result);
            }
            *result = FALSE;
            return NOERROR;
        default:
            break;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DeviceSelectAction::HandleReportPowerStatus(
    /* [in] */ Int32 powerStatus,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (powerStatus) {
        case IHdmiControlManager::POWER_STATUS_ON:
            SendSetStreamPath();
            *result = TRUE;
            return NOERROR;
        case IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_STANDBY:
            if (mPowerStatusCounter < 4) {
                mState = STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY;
                AddTimer(mState, TIMEOUT_TRANSIT_TO_STANDBY_MS);
            } else {
                SendSetStreamPath();
            }
            *result = TRUE;
            return NOERROR;
        case IHdmiControlManager::POWER_STATUS_STANDBY:
            if (mPowerStatusCounter == 0) {
                TurnOnDevice();
            } else {
                SendSetStreamPath();
            }
            *result = TRUE;
            return NOERROR;
        case IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON:
            if (mPowerStatusCounter < LOOP_COUNTER_MAX) {
                mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
                AddTimer(mState, TIMEOUT_POWER_ON_MS);
            } else {
                SendSetStreamPath();
            }
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DeviceSelectAction::TurnOnDevice()
{
    Int32 logicalAddr;
    mTarget->GetLogicalAddress(&logicalAddr);
    SendUserControlPressedAndReleased(logicalAddr, HdmiCecKeycode::CEC_KEYCODE_POWER);
    SendUserControlPressedAndReleased(logicalAddr, HdmiCecKeycode::CEC_KEYCODE_POWER_ON_FUNCTION);
    mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
    AddTimer(mState, TIMEOUT_POWER_ON_MS);
    return NOERROR;
}

ECode DeviceSelectAction::SendSetStreamPath()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    Int32 physicalAddr;
    mTarget->GetPhysicalAddress(&physicalAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildSetStreamPath(srcAddr, physicalAddr, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    InvokeCallback(IHdmiControlManager::RESULT_SUCCESS);
    Finish();
    return NOERROR;
}

ECode DeviceSelectAction::HandleTimerEvent(
    /* [in] */ Int32 timeoutState)
{
    if (mState != timeoutState) {
        Slogger::W(TAG, "Timer in a wrong state. Ignored.");
        return NOERROR;
    }
    switch (mState) {
        case STATE_WAIT_FOR_REPORT_POWER_STATUS:
            {
                AutoPtr<IHdmiCecLocalDeviceTv> tv;
                Tv((IHdmiCecLocalDeviceTv**)&tv);
                Boolean isPowerStandbyOrTransient;
                ((HdmiCecLocalDeviceTv*) tv.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
                if (isPowerStandbyOrTransient) {
                    InvokeCallback(IHdmiControlManager::RESULT_INCORRECT_MODE);
                    Finish();
                    return NOERROR;
                }
                SendSetStreamPath();
            }
            break;
        case STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY:
        case STATE_WAIT_FOR_DEVICE_POWER_ON:
            mPowerStatusCounter++;
            QueryDevicePowerStatus();
            break;
    }
    return NOERROR;
}

ECode DeviceSelectAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    if (mCallback == NULL) {
        return NOERROR;
    }
    // try {
    ECode ec = mCallback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Callback failed:%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
