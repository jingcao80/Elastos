
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/DevicePowerStatusAction.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(DevicePowerStatusAction, Object, IDevicePowerStatusAction)

const String DevicePowerStatusAction::TAG("DevicePowerStatusAction");
const Int32 DevicePowerStatusAction::STATE_WAITING_FOR_REPORT_POWER_STATUS = 1;

DevicePowerStatusAction::DevicePowerStatusAction()
    : mTargetAddress(0)
{}

ECode DevicePowerStatusAction::Create(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback,
    /* [out] */ DevicePowerStatusAction** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (source == NULL || callback == NULL) {
        Slogger::E(TAG, "Wrong arguments");
        *result = NULL;
        return NOERROR;
    }
    *result = new DevicePowerStatusAction();
    (*result)->constructor(source, targetAddress, callback);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DevicePowerStatusAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback)
{
    mTargetAddress = targetAddress;
    mCallback = callback;
    HdmiCecFeatureAction::constructor(localDevice);
    return NOERROR;
}

ECode DevicePowerStatusAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    QueryDevicePowerStatus();
    mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    *result = TRUE;
    return NOERROR;
}

ECode DevicePowerStatusAction::QueryDevicePowerStatus()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(srcAddr, mTargetAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode DevicePowerStatusAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAITING_FOR_REPORT_POWER_STATUS) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
        AutoPtr<ArrayOf<Byte> > array;
        cmd->GetParams((ArrayOf<Byte>**)&array);
        Int32 status = (*array)[0];
        InvokeCallback(status);
        Finish();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DevicePowerStatusAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        return NOERROR;
    }
    if (state == STATE_WAITING_FOR_REPORT_POWER_STATUS) {
        // Got no response from TV. Report status 'unknown'.
        InvokeCallback(IHdmiControlManager::POWER_STATUS_UNKNOWN);
        Finish();
    }
    return NOERROR;
}

ECode DevicePowerStatusAction::InvokeCallback(
    /* [in] */ Int32 result)
{
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
