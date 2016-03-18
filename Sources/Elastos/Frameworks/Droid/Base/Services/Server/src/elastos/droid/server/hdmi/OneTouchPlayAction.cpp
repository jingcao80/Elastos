
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/OneTouchPlayAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClientOneTouchPlayCallback;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;

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

    if (source == NULL || callback == NULL) {
        Slogger::E(TAG, "Wrong arguments");
        *result = NULL;
        return NOERROR;
    }
    *result = new OneTouchPlayAction(source, targetAddress,
            callback);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

OneTouchPlayAction::OneTouchPlayAction(
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ Int32 targetAddress,
    /* [in] */ IIHdmiControlCallback* callback)
    : mTargetAddress(0)
    , mPowerStatusCounter(0)
{
    HdmiCecFeatureAction::constructor(localDevice);
    mTargetAddress = targetAddress;
    mCallback = callback;
}

ECode OneTouchPlayAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildTextViewOn(srcAddr, mTargetAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    BroadcastActiveSource();
    QueryDevicePowerStatus();
    mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    *result = TRUE;
    return NOERROR;
}

ECode OneTouchPlayAction::BroadcastActiveSource()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    Int32 srcPath;
    GetSourcePath(&srcPath);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildActiveSource(srcAddr, srcPath, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    // Because only playback device can create this action, it's safe to cast.
    AutoPtr<HdmiCecLocalDevicePlayback> playback;
    Playback((HdmiCecLocalDevicePlayback**)&playback);
    playback->MarkActiveSource();
    return NOERROR;
}

ECode OneTouchPlayAction::QueryDevicePowerStatus()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(srcAddr,
            mTargetAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode OneTouchPlayAction::ProcessCommand(
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
}

ECode OneTouchPlayAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
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
    return NOERROR;
}

ECode OneTouchPlayAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    // try {
    ECode ec = mCallback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
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
