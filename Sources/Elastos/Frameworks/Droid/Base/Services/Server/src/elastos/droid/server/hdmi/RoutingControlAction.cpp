
#include "elastos/droid/server/hdmi/RoutingControlAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String RoutingControlAction::TAG("RoutingControlAction");
const Int32 RoutingControlAction::STATE_WAIT_FOR_ROUTING_INFORMATION = 1;
const Int32 RoutingControlAction::STATE_WAIT_FOR_REPORT_POWER_STATUS = 2;
const Int32 RoutingControlAction::TIMEOUT_ROUTING_INFORMATION_MS = 1000;
const Int32 RoutingControlAction::TIMEOUT_REPORT_POWER_STATUS_MS = 1000;

RoutingControlAction::RoutingControlAction()
    : mQueryDevicePowerStatus(FALSE)
    , mNotifyInputChange(FALSE)
    , mCurrentRoutingPath(0)
{}

ECode RoutingControlAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ Int32 path,
    /* [in] */ Boolean queryDevicePowerStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(localDevice);
        mCallback = callback;
        mCurrentRoutingPath = path;
        mQueryDevicePowerStatus = queryDevicePowerStatus;
        // Callback is non-NULL when routing control action is brought up by binder API. Use
        // this as an indicator for the input change notification. These API calls will get
        // the result through this callback, not through notification. Any other events that
        // trigger the routing control is external, for which notifcation is used.
        mNotifyInputChange = (callback == NULL);
#endif
}

ECode RoutingControlAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAIT_FOR_ROUTING_INFORMATION;
        AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
        return TRUE;
#endif
}

ECode RoutingControlAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 opcode = cmd->GetOpcode();
        Byte[] params = cmd->GetParams();
        if (mState == STATE_WAIT_FOR_ROUTING_INFORMATION
                && opcode == Constants::MESSAGE_ROUTING_INFORMATION) {
            // Keep updating the physicalAddress as we receive <Routing Information>.
            // If the routing path doesn't belong to the currently active one, we should
            // ignore it since it might have come from other routing change sequence.
            Int32 routingPath = HdmiUtils->TwoBytesToInt(params);
            if (!HdmiUtils->IsInActiveRoutingPath(mCurrentRoutingPath, routingPath)) {
                return TRUE;
            }
            mCurrentRoutingPath = routingPath;
            // Stop possible previous routing change sequence if in progress.
            RemoveActionExcept(RoutingControlAction.class, this);
            AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
            return TRUE;
        } else if (mState == STATE_WAIT_FOR_REPORT_POWER_STATUS
                  && opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
            HandleReportPowerStatus(cmd->GetParams()[0]);
            return TRUE;
        }
        return FALSE;
#endif
}

ECode RoutingControlAction::HandleReportPowerStatus(
    /* [in] */ Int32 devicePowerStatus)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (IsPowerOnOrTransient(GetTvPowerStatus())) {
            if (IsPowerOnOrTransient(devicePowerStatus)) {
                SendSetStreamPath();
            } else {
                Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
            }
        }
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
#endif
}

ECode RoutingControlAction::GetTvPowerStatus(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return Tv()->GetPowerStatus();
#endif
}

ECode RoutingControlAction::IsPowerOnOrTransient(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return status == HdmiControlManager.POWER_STATUS_ON
                || status == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
#endif
}

ECode RoutingControlAction::SendSetStreamPath()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildSetStreamPath(GetSourceAddress(),
                mCurrentRoutingPath));
#endif
}

ECode RoutingControlAction::FinishWithCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        InvokeCallback(result);
        Finish();
#endif
}

ECode RoutingControlAction::HandleTimerEvent(
    /* [in] */ Int32 timeoutState)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != timeoutState || mState == STATE_NONE) {
            Slogger::W("CEC", "Timer in a wrong state. Ignored.");
            return;
        }
        switch (timeoutState) {
            case STATE_WAIT_FOR_ROUTING_INFORMATION:
                HdmiDeviceInfo device = Tv()->GetDeviceInfoByPath(mCurrentRoutingPath);
                if (device != NULL && mQueryDevicePowerStatus) {
                    Int32 deviceLogicalAddress = device->GetLogicalAddress();
                    QueryDevicePowerStatus(deviceLogicalAddress, new SendMessageCallback() {
                        //@Override
                        CARAPI OnSendCompleted(Int32 error) {
                            HandlDevicePowerStatusAckResult(
                                    error == HdmiControlManager.RESULT_SUCCESS);
                        }
                    });
                } else {
                    Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
                }
                return;
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (IsPowerOnOrTransient(GetTvPowerStatus())) {
                    Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    SendSetStreamPath();
                }
                FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
                return;
        }
#endif
}

ECode RoutingControlAction::QueryDevicePowerStatus(
    /* [in] */ Int32 address,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(GetSourceAddress(), address),
                callback);
#endif
}

ECode RoutingControlAction::HandlDevicePowerStatusAckResult(
    /* [in] */ Boolean acked)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (acked) {
            mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
            AddTimer(mState, TIMEOUT_REPORT_POWER_STATUS_MS);
        } else {
            Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
            SendSetStreamPath();
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
        }
#endif
}

ECode RoutingControlAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            // Do nothing.
        }
    }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
