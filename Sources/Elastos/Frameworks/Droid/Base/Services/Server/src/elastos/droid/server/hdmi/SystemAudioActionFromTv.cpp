
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

ECode SystemAudioActionFromTv::constructor(
    /* [in] */ IHdmiCecLocalDevice* sourceAddress,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(sourceAddress, avrAddress, targetStatus, callback);
        HdmiUtils->VerifyAddressType(GetSourceAddress(), HdmiDeviceInfo.DEVICE_TV);
#endif
}

ECode SystemAudioActionFromTv::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        RemoveSystemAudioActionInProgress();
        SendSystemAudioModeRequest();
        return TRUE;
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
