
#include "elastos/droid/bluetooth/le/AdvertiseCallback.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                          AdvertiseCallback
//=====================================================================
CAR_INTERFACE_IMPL(AdvertiseCallback, Object, IAdvertiseCallback);

AdvertiseCallback::AdvertiseCallback()
{
}

AdvertiseCallback::~AdvertiseCallback()
{
}

ECode AdvertiseCallback::OnStartSuccess(
    /* [in] */ IAdvertiseSettings* settingsInEffect)
{
    return NOERROR;
}

ECode AdvertiseCallback::OnStartFailure(
    /* [in] */ Int32 errorCode)
{
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
