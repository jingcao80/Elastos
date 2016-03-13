
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SystemAudioActionFromTv, SystemAudioAction, ISystemAudioActionFromTv)

ECode SystemAudioActionFromTv::constructor(
    /* [in] */ IHdmiCecLocalDevice* sourceAddress,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(sourceAddress, avrAddress, targetStatus, callback);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiUtils->VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
#endif
}

ECode SystemAudioActionFromTv::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        RemoveSystemAudioActionInProgress();
        SendSystemAudioModeRequest();
        *result = TRUE;
        return NOERROR;
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
