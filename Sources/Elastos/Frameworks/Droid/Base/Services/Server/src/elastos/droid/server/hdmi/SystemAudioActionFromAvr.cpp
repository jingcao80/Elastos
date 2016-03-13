
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source, avrAddress, targetStatus, callback);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiUtils->VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
#endif
}

ECode SystemAudioActionFromAvr::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        RemoveSystemAudioActionInProgress();
        HandleSystemAudioActionFromAvr();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode SystemAudioActionFromAvr::HandleSystemAudioActionFromAvr()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean isSystemAudioActivated;
        tv->IsSystemAudioActivated(&isSystemAudioActivated);
        if (mTargetAudioStatus == isSystemAudioActivated) {
            FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
            return NOERROR;
        }
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean isProhibitMode;
        tv->IsProhibitMode(&isProhibitMode);
        if (isProhibitMode) {
            Int32 srcAddr;
            GetSourceAddress(&srcAddr);
            SendCommand(HdmiCecMessageBuilder->BuildFeatureAbortCommand(srcAddr, mAvrLogicalAddress,
                    Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE, Constants::ABORT_REFUSED));
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
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
