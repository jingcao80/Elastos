
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothClass.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothClass)
    , public Object
    , public IBluetoothClass
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothClass();

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI HasService(
        /* [in] */ Int32 service,
        /* [out] */ Boolean* hasService);

    CARAPI GetMajorDeviceClass(
        /* [out] */ Int32* deviceClass);

    CARAPI GetDeviceClass(
        /* [out] */ Int32* deviceClass);

    CARAPI DoesClassMatch(
        /* [in] */ Int32 profile,
        /* [out] */ Boolean* isMatch);

    CARAPI GetClass(
        /* [out] */ Int32* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 classInt);

private:
    Int32 mClass;
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__
