
#include "elastos/droid/view/CInputDeviceHelper.h"
#include "elastos/droid/view/CInputDevice.h"

using namespace Elastos::Droid::View;

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
