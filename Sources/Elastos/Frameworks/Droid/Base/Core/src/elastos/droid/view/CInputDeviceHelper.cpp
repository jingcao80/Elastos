
#include "elastos/droid/view/CInputDeviceHelper.h"
#include "elastos/droid/view/CInputDevice.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CInputDeviceHelper, Singleton, IInputDeviceHelper)

CAR_SINGLETON_IMPL(CInputDeviceHelper)

ECode CInputDeviceHelper::GetDevice(
    /* [in] */ Int32 id,
    /* [out] */ IInputDevice** device)
{
    return CInputDevice::GetDevice(id, device);
}

ECode CInputDeviceHelper::GetDeviceIds(
    /* [out, callee] */ ArrayOf<Int32>** deviceIds)
{
    return CInputDevice::GetDeviceIds(deviceIds);
}

} // namespace View
} // namespace Droid
} // namespace Elastos