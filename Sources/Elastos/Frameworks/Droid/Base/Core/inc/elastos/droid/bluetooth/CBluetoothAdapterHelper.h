
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERHELPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERHELPER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAdapterHelper.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothAdapterHelper)
    , public Singleton
    , public IBluetoothAdapterHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetDefaultAdapter(
        /* [out] */ Elastos::Droid::Bluetooth::IBluetoothAdapter** adapter);

    CARAPI CheckBluetoothAddress(
        /* [in] */ const String& address,
        /* [out] */ Boolean* result);

private:
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERHELPER_H__
