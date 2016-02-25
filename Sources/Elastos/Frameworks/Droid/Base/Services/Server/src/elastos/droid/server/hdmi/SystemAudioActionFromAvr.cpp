
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

ECode SystemAudioActionFromAvr::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(source, avrAddress, targetStatus, callback);
        HdmiUtils->VerifyAddressType(GetSourceAddress(), HdmiDeviceInfo.DEVICE_TV);
#endif
}

ECode SystemAudioActionFromAvr::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        RemoveSystemAudioActionInProgress();
        HandleSystemAudioActionFromAvr();
        return TRUE;
#endif
}

ECode SystemAudioActionFromAvr::HandleSystemAudioActionFromAvr()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mTargetAudioStatus == Tv()->IsSystemAudioActivated()) {
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (Tv()->IsProhibitMode()) {
            SendCommand(HdmiCecMessageBuilder->BuildFeatureAbortCommand(
                    GetSourceAddress(), mAvrLogicalAddress,
                    Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE, Constants::ABORT_REFUSED));
            mTargetAudioStatus = FALSE;
            SendSystemAudioModeRequest();
            return;
        }

        RemoveAction(SystemAudioAutoInitiationAction.class);

        if (mTargetAudioStatus) {
            SetSystemAudioMode(TRUE);
            StartAudioStatusAction();
        } else {
            SetSystemAudioMode(FALSE);
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
