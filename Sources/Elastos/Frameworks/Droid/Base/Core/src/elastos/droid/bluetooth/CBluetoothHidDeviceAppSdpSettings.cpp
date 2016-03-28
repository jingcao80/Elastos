#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppSdpSettings.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceAppSdpSettings)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceAppSdpSettings, Object, IBluetoothHidDeviceAppSdpSettings, IParcelable)

CBluetoothHidDeviceAppSdpSettings::CBluetoothHidDeviceAppSdpSettings()
{}

ECode CBluetoothHidDeviceAppSdpSettings::constructor()
{
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& description,
    /* [in] */ const String& provider,
    /* [in] */ Byte subclass,
    /* [in] */ ArrayOf<Byte>* descriptors)
{
    mName = name;
    mDescription = description;
    mProvider = provider;
    mSubclass = subclass;
    mDescriptors = descriptors->Clone();
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadString(&mDescription);
    source->ReadString(&mProvider);
    source->ReadByte(&mSubclass);
    Handle32 tmp;
    source->ReadArrayOf(&tmp);
    mDescriptors = reinterpret_cast<ArrayOf<Byte>*>(tmp);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteString(mDescription);
    dest->WriteString(mProvider);
    dest->WriteByte(mSubclass);
    dest->WriteArrayOf(reinterpret_cast<Handle32>(mDescriptors.Get()));
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos