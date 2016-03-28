#ifndef __ELASTOS_DROID_BLUETOOTH_CBluetoothHidDeviceAppSdpSettings_H__
#define __ELASTOS_DROID_BLUETOOTH_CBluetoothHidDeviceAppSdpSettings_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceAppSdpSettings.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceAppSdpSettings)
    , public Object
    , public IBluetoothHidDeviceAppSdpSettings
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceAppSdpSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& description,
        /* [in] */ const String& provider,
        /* [in] */ Byte subclass,
        /* [in] */ ArrayOf<Byte>* descriptors);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

public:
    String mName;
    String mDescription;
    String mProvider;
    Byte mSubclass;
    AutoPtr<ArrayOf<Byte> > mDescriptors;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif