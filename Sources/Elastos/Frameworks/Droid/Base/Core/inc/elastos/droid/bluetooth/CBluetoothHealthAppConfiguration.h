
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Bluetooth_CBluetoothHealthAppConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthAppConfiguration)
    , public Object
    , public IBluetoothHealthAppConfiguration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothHealthAppConfiguration();

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI GetDataType(
        /* [out] */ Int32* type);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetRole(
        /* [out] */ Int32* role);

    CARAPI GetChannelType(
        /* [out] */ Int32* type);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 role,
        /* [in] */ Int32 channelType);

private:
    String mName;
    Int32 mDataType;
    Int32 mRole;
    Int32 mChannelType;
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__
