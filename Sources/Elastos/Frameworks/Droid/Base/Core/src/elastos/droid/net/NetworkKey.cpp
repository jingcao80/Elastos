
#include "elastos/droid/net/NetworkKey.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkKey, Object, IParcelable, INetworkKey)

ECode NetworkKey::constructor(
    /* [in] */ IWifiKey* wifiKey)
{
    mType = TYPE_WIFI;
    mWifiKey = wifiKey;
    return NOERROR;
}

NetworkKey::NetworkKey()
    : mType(0)
{}

ECode NetworkKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    ClassID this_cid, o_cid;
    IObject::Probe(TO_IINTERFACE(this))->GetClassID(&this_cid);
    IObject::Probe(o)->GetClassID(&o_cid);
    if (o == NULL || this_cid != o_cid) FUNC_RETURN(FALSE)
    AutoPtr<NetworkKey> that = (NetworkKey*) INetworkKey::Probe(o);
    FUNC_RETURN(mType == that->mType && Objects::Equals(mWifiKey, that->mWifiKey))
}

ECode NetworkKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(5);
    (*array)[0] = mType;
    Int32 hashCode;
    IObject::Probe(mWifiKey)->GetHashCode(&hashCode);
    (*array)[1] = hashCode;
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode NetworkKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    switch (mType) {
        case TYPE_WIFI:
            return IObject::Probe(mWifiKey)->ToString(result);
        default:
            // Don't throw an exception here in case someone is logging this object in a catch
            // block for debugging purposes.
            *result = String("InvalidKey");
            return NOERROR;
    }
    return NOERROR;
}

ECode NetworkKey::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mType);
    AutoPtr<IInterface> obj;
    switch (mType) {
        case TYPE_WIFI:
            parcel->ReadInterfacePtr((Handle32*)&obj);
            mWifiKey = IWifiKey::Probe(obj);
            break;
        default:
            Logger::E("NetworkKey", "Parcel has unknown type: %d", mType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode NetworkKey::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mType);
    dest->WriteInterfacePtr(mWifiKey);
    return NOERROR;
}

ECode NetworkKey::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mType;
    return NOERROR;
}

ECode NetworkKey::GetWifiKey(
    /* [out] */ IWifiKey** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mWifiKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
