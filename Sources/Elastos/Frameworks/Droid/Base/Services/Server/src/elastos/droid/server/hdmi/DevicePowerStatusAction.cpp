
#include "elastos/droid/server/hdmi/DevicePowerStatusAction.h"
#include <Elastos.Droid.Hardware.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String DevicePowerStatusAction::TAG("DevicePowerStatusAction");
const Int32 DevicePowerStatusAction::STATE_WAITING_FOR_REPORT_POWER_STATUS = 1;

ECode DevicePowerStatusAction::Create(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback,
    /* [out] */ DevicePowerStatusAction** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (source == NULL || callback == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            return NULL;
        }
        return new DevicePowerStatusAction(source, targetAddress, callback);

#endif
}

DevicePowerStatusAction::DevicePowerStatusAction(
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback)
    : mTargetAddress(targetAddress)
    , mCallback(callback)
{
#if 0 // TODO: Translate codes below
    HdmiCecFeatureAction::constructor(localDevice);
#endif
}

ECode DevicePowerStatusAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        queryDevicePowerStatus();
        mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
        addTimer(mState, HdmiConfig.TIMEOUT_MS);
        return true;

#endif
}

ECode DevicePowerStatusAction::QueryDevicePowerStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendCommand(HdmiCecMessageBuilder.buildGiveDevicePowerStatus(getSourceAddress(),
                mTargetAddress));

#endif
}

ECode DevicePowerStatusAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            return FALSE;
        }
        if (cmd.getOpcode() == Constants::MESSAGE_REPORT_POWER_STATUS) {
            int status = cmd.getParams()[0];
            invokeCallback(status);
            finish();
            return true;
        }
        return FALSE;

#endif
}

ECode DevicePowerStatusAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return;
        }
        if (state == STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            // Got no response from TV. Report status 'unknown'.
            invokeCallback(HdmiControlManager.POWER_STATUS_UNKNOWN);
            finish();
        }

#endif
}

ECode DevicePowerStatusAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            mCallback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
