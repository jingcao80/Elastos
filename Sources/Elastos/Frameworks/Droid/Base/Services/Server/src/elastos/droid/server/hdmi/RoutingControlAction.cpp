
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/RoutingControlAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// RoutingControlAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(RoutingControlAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

RoutingControlAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ RoutingControlAction* host)
    : mHost(host)
{}

ECode RoutingControlAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    return mHost->HandlDevicePowerStatusAckResult(
            error == IHdmiControlManager::RESULT_SUCCESS);
}

//=============================================================================
// RoutingControlAction
//=============================================================================
CAR_INTERFACE_IMPL(RoutingControlAction, HdmiCecFeatureAction, IRoutingControlAction)

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
    HdmiCecFeatureAction::constructor(localDevice);
    mCallback = callback;
    mCurrentRoutingPath = path;
    mQueryDevicePowerStatus = queryDevicePowerStatus;
    // Callback is non-NULL when routing control action is brought up by binder API. Use
    // this as an indicator for the input change notification. These API calls will get
    // the result through this callback, not through notification. Any other events that
    // trigger the routing control is external, for which notifcation is used.
    mNotifyInputChange = (callback == NULL);
    return NOERROR;
}

ECode RoutingControlAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WAIT_FOR_ROUTING_INFORMATION;
    AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
    *result = TRUE;
    return NOERROR;
}

ECode RoutingControlAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    if (mState == STATE_WAIT_FOR_ROUTING_INFORMATION
            && opcode == Constants::MESSAGE_ROUTING_INFORMATION) {
        // Keep updating the physicalAddress as we receive <Routing Information>.
        // If the routing path doesn't belong to the currently active one, we should
        // ignore it since it might have come from other routing change sequence.
        Int32 routingPath = HdmiUtils::TwoBytesToInt32(params);
        if (!HdmiUtils::IsInActiveRoutingPath(mCurrentRoutingPath, routingPath)) {
            *result = TRUE;
            return NOERROR;
        }
        mCurrentRoutingPath = routingPath;
        // Stop possible previous routing change sequence if in progress.
        RemoveActionExcept(ECLSID_CRoutingControlAction, this);
        AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
        *result = TRUE;
        return NOERROR;
    } else if (mState == STATE_WAIT_FOR_REPORT_POWER_STATUS
              && opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
        HandleReportPowerStatus((*params)[0]);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode RoutingControlAction::HandleReportPowerStatus(
    /* [in] */ Int32 devicePowerStatus)
{
    Int32 tvPowerStatus;
    GetTvPowerStatus(&tvPowerStatus);
    if (IsPowerOnOrTransient(tvPowerStatus)) {
        if (IsPowerOnOrTransient(devicePowerStatus)) {
            SendSetStreamPath();
        } else {
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            Tv((IHdmiCecLocalDeviceTv**)&tv);
            ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
        }
    }
    FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
    return NOERROR;
}

ECode RoutingControlAction::GetTvPowerStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    return ((HdmiCecLocalDeviceTv*) tv.Get())->GetPowerStatus(result);
}

Boolean RoutingControlAction::IsPowerOnOrTransient(
    /* [in] */ Int32 status)
{
    return status == IHdmiControlManager::POWER_STATUS_ON
            || status == IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON;
}

ECode RoutingControlAction::SendSetStreamPath()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildSetStreamPath(srcAddr, mCurrentRoutingPath, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode RoutingControlAction::FinishWithCallback(
    /* [in] */ Int32 result)
{
    InvokeCallback(result);
    Finish();
    return NOERROR;
}

ECode RoutingControlAction::HandleTimerEvent(
    /* [in] */ Int32 timeoutState)
{
    if (mState != timeoutState || mState == STATE_NONE) {
        Slogger::W("CEC", "Timer in a wrong state. Ignored.");
        return NOERROR;
    }
    switch (timeoutState) {
        case STATE_WAIT_FOR_ROUTING_INFORMATION:
            {
                AutoPtr<IHdmiCecLocalDeviceTv> tv;
                Tv((IHdmiCecLocalDeviceTv**)&tv);
                AutoPtr<IHdmiDeviceInfo> device;
                ((HdmiCecLocalDeviceTv*) tv.Get())->GetDeviceInfoByPath(mCurrentRoutingPath, (IHdmiDeviceInfo**)&device);
                if (device != NULL && mQueryDevicePowerStatus) {
                    Int32 deviceLogicalAddress;
                    device->GetLogicalAddress(&deviceLogicalAddress);
                    QueryDevicePowerStatus(deviceLogicalAddress, new InnerSub_SendMessageCallback(this));
                } else {
                    AutoPtr<IHdmiCecLocalDeviceTv> tv;
                    Tv((IHdmiCecLocalDeviceTv**)&tv);
                    ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
                }
                return NOERROR;
            }
        case STATE_WAIT_FOR_REPORT_POWER_STATUS:
            {
                Int32 tvPowerStatus;
                GetTvPowerStatus(&tvPowerStatus);
                if (IsPowerOnOrTransient(tvPowerStatus)) {
                    AutoPtr<IHdmiCecLocalDeviceTv> tv;
                    Tv((IHdmiCecLocalDeviceTv**)&tv);
                    ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    SendSetStreamPath();
                }
                FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
                return NOERROR;
            }
    }
    return NOERROR;
}

ECode RoutingControlAction::QueryDevicePowerStatus(
    /* [in] */ Int32 address,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(srcAddr, address, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd, callback);
    return NOERROR;
}

ECode RoutingControlAction::HandlDevicePowerStatusAckResult(
    /* [in] */ Boolean acked)
{
    if (acked) {
        mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
        AddTimer(mState, TIMEOUT_REPORT_POWER_STATUS_MS);
    } else {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
        SendSetStreamPath();
        FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
    }
    return NOERROR;
}

ECode RoutingControlAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    if (mCallback == NULL) {
        return NOERROR;
    }
    // try {
    mCallback->OnComplete(result);
    // } catch (RemoteException e) {
        // Do nothing.
    // }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
