
#include "elastos/droid/bluetooth/CBluetoothAdapterHelper.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL(CBluetoothAdapterHelper, Singleton, IBluetoothAdapterHelper);

CAR_SINGLETON_IMPL(CBluetoothAdapterHelper);

ECode CBluetoothAdapterHelper::GetDefaultAdapter(
    /* [out] */ Elastos::Droid::Bluetooth::IBluetoothAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);

    AutoPtr<IBluetoothAdapter> _adapter = CBluetoothAdapter::GetDefaultAdapter();
    *adapter = _adapter;
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode CBluetoothAdapterHelper::CheckBluetoothAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = CBluetoothAdapter::CheckBluetoothAddress(address);

    return NOERROR;
}

}
}
}

