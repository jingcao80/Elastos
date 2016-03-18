
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SystemAudioActionFromAvr, SystemAudioAction, ISystemAudioActionFromAvr)

ECode SystemAudioActionFromAvr::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    SystemAudioAction::constructor(source, avrAddress, targetStatus, callback);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    HdmiUtils::VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
    return NOERROR;
}

ECode SystemAudioActionFromAvr::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    RemoveSystemAudioActionInProgress();
    HandleSystemAudioActionFromAvr();
    *result = TRUE;
    return NOERROR;
}

ECode SystemAudioActionFromAvr::HandleSystemAudioActionFromAvr()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Boolean isSystemAudioActivated;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsSystemAudioActivated(&isSystemAudioActivated);
    if (mTargetAudioStatus == isSystemAudioActivated) {
        FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
        return NOERROR;
    }
    Boolean isProhibitMode;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsProhibitMode(&isProhibitMode);
    if (isProhibitMode) {
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildFeatureAbortCommand(srcAddr, mAvrLogicalAddress,
                Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE, Constants::ABORT_REFUSED, (IHdmiCecMessage**)&cmd);
        SendCommand(cmd);
        mTargetAudioStatus = FALSE;
        SendSystemAudioModeRequest();
        return NOERROR;
    }

    RemoveAction(ECLSID_CSystemAudioAutoInitiationAction);

    if (mTargetAudioStatus) {
        SetSystemAudioMode(TRUE);
        StartAudioStatusAction();
    } else {
        SetSystemAudioMode(FALSE);
        FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
