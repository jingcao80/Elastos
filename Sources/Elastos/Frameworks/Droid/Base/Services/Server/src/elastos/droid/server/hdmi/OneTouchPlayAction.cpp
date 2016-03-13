
#include "elastos/droid/server/hdmi/OneTouchPlayAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClientOneTouchPlayCallback;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(OneTouchPlayAction, HdmiCecFeatureAction, IOneTouchPlayAction)

const String OneTouchPlayAction::TAG("OneTouchPlayAction");
const Int32 OneTouchPlayAction::STATE_WAITING_FOR_REPORT_POWER_STATUS = 1;
const Int32 OneTouchPlayAction::LOOP_COUNTER_MAX = 10;

OneTouchPlayAction::OneTouchPlayAction()
    : mTargetAddress(0)
    , mPowerStatusCounter(0)
{}

ECode OneTouchPlayAction::Create(
    /* [in] */ HdmiCecLocalDevicePlayback* source,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback,
    /* [out] */ OneTouchPlayAction** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (source == NULL || callback == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            *result = NULL;
            return NOERROR;
        }
        return new OneTouchPlayAction(source, targetAddress,
                callback);
#endif
}

OneTouchPlayAction::OneTouchPlayAction(
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback)
    : mTargetAddress(0)
    , mPowerStatusCounter(0)
{
#if 0 // TODO: Translate codes below
        super::constructor(localDevice);
        mTargetAddress = targetAddress;
        mCallback = callback;
#endif
}

ECode OneTouchPlayAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildTextViewOn(srcAddr, mTargetAddress));
        BroadcastActiveSource();
        QueryDevicePowerStatus();
        mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
        AddTimer(mState, HdmiConfig::TIMEOUT_MS);
        *result = TRUE;
        return NOERROR;
#endif
}

ECode OneTouchPlayAction::BroadcastActiveSource()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildActiveSource(srcAddr, GetSourcePath()));
        // Because only playback device can create this action, it's safe to cast.
        Playback()->MarkActiveSource();
#endif
}

ECode OneTouchPlayAction::QueryDevicePowerStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(srcAddr,
                mTargetAddress));
#endif
}

ECode OneTouchPlayAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            *result = FALSE;
            return NOERROR;
        }
        Int32 opcode;
        cmd->GetOpcode(&opcode);
        if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
            AutoPtr<ArrayOf<Byte> > params;
            cmd->GetParams((ArrayOf<Byte>**)&params);
            Int32 status = (*params)[0];
            if (status == IHdmiControlManager::POWER_STATUS_ON) {
                BroadcastActiveSource();
                InvokeCallback(IHdmiControlManager::RESULT_SUCCESS);
                Finish();
            }
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
#endif
}

ECode OneTouchPlayAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return NOERROR;
        }
        if (state == STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            if (mPowerStatusCounter++ < LOOP_COUNTER_MAX) {
                QueryDevicePowerStatus();
                AddTimer(mState, HdmiConfig::TIMEOUT_MS);
            } else {
                // Couldn't wake up the TV for whatever reason. Report failure.
                InvokeCallback(IHdmiControlManager::RESULT_TIMEOUT);
                Finish();
            }
        }
#endif
}

ECode OneTouchPlayAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
