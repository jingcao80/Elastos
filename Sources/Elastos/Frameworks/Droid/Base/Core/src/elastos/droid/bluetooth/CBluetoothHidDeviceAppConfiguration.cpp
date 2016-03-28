#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppConfiguration.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceAppConfiguration)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceAppConfiguration, Object, IBluetoothHidDeviceAppConfiguration, IParcelable)

CBluetoothHidDeviceAppConfiguration::CBluetoothHidDeviceAppConfiguration()
{
    AutoPtr<IRandom> rdm;
    CRandom::New((IRandom**)&rdm);
    rdm->NextInt64(&mHash);
}

ECode CBluetoothHidDeviceAppConfiguration::constructor()
{
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::constructor(
    /* [in] */ Int64 hash)
{
    mHash = hash;
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mHash);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mHash);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if (IBluetoothHidDeviceAppConfiguration::Probe(o) != NULL) {
        IBluetoothHidDeviceAppConfiguration* cfg = IBluetoothHidDeviceAppConfiguration::Probe(o);
        CBluetoothHidDeviceAppConfiguration* impl = (CBluetoothHidDeviceAppConfiguration*)cfg;
        *result = mHash == impl->mHash;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos