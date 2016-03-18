
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"

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
    SystemAudioAction::constructor(sourceAddress, avrAddress, targetStatus, callback);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    HdmiUtils::VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
    return NOERROR;
}

ECode SystemAudioActionFromTv::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    RemoveSystemAudioActionInProgress();
    SendSystemAudioModeRequest();
    *result = TRUE;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
