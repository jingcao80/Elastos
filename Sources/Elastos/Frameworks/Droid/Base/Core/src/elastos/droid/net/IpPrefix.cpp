
#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/net/IpPrefix.h"
#include "elastos/droid/net/NetworkUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::IInteger32;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(IpPrefix, Object, IIpPrefix, IParcelable)

ECode IpPrefix::CheckAndMaskAddressAndPrefixLength()
{
    if (mAddress->GetLength() != 4 && mAddress->GetLength() != 16) {
        Logger::E("IpPrefix", "IpPrefix has %d bytes which is neither 4 nor 16", mAddress->GetLength());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NetworkUtils::MaskRawAddress(mAddress, mPrefixLength);
}

ECode IpPrefix::constructor()
{
    return NOERROR;
}

ECode IpPrefix::constructor(
    /* [in] */ ArrayOf<Byte>* address,
    /* [in] */ Int32 prefixLength)
{
    mAddress = address->Clone();
    mPrefixLength = prefixLength;
    return CheckAndMaskAddressAndPrefixLength();
}

ECode IpPrefix::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength)
{
    // We don't reuse the (byte[], int) constructor because it calls clone() on the byte array,
    // which is unnecessary because getAddress() already returns a clone.
    address->GetAddress((ArrayOf<Byte>**)&mAddress);
    mPrefixLength = prefixLength;
    return CheckAndMaskAddressAndPrefixLength();
}

ECode IpPrefix::constructor(
    /* [in] */ const String& prefix)
{
    // We don't reuse the (InetAddress, int) constructor because "error: call to this must be
    // first statement in constructor". We could factor out setting the member variables to an
    // init() method, but if we did, then we'd have to make the members non-final, or "error:
    // cannot assign a value to final variable address". So we just duplicate the code here.
    AutoPtr<IPair> ipAndMask;
    NetworkUtils::ParseIpAndMask(prefix, (IPair**)&ipAndMask);
    AutoPtr<IInterface> obj;
    ipAndMask->GetFirst((IInterface**)&obj);
    IInetAddress::Probe(obj)->GetAddress((ArrayOf<Byte>**)&mAddress);
    obj = NULL;
    ipAndMask->GetSecond((IInterface**)&obj);
    IInteger32::Probe(obj)->GetValue(&mPrefixLength);
    return CheckAndMaskAddressAndPrefixLength();
}

ECode IpPrefix::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }

    if (IIpPrefix::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IIpPrefix> iThat = IIpPrefix::Probe(obj);
    AutoPtr<IpPrefix> that = (IpPrefix*) iThat.Get();
    *result = Arrays::Equals(mAddress, that->mAddress) && mPrefixLength == that->mPrefixLength;
    return NOERROR;
}

ECode IpPrefix::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Arrays::GetHashCode(mAddress);
    *result += 11 * mPrefixLength;
    return NOERROR;
}

ECode IpPrefix::GetAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

        // try {
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    ECode ec = helper->GetByAddress(mAddress, result);
        // } catch (UnknownHostException e) {
    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        // Cannot happen. InetAddress.getByAddress can only throw an exception if the byte
        // array is the wrong length, but we check that in the constructor.
        *result = NULL;
        return NOERROR;
    }
        // }
    return NOERROR;
}

ECode IpPrefix::GetRawAddress(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAddress->Clone();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode IpPrefix::GetPrefixLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPrefixLength;
    return NOERROR;
}

ECode IpPrefix::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

        // try {
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr<IInetAddress> inetAddress;
    ECode ec = helper->GetByAddress(mAddress, (IInetAddress**)&inetAddress);
        // } catch(UnknownHostException e) {
    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        // Cosmic rays?
        Logger::E("IpPrefix", "IpPrefix with invalid address! Shouldn't happen.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
        // }
    ec = inetAddress->GetHostAddress(result);
    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        // Cosmic rays?
        Logger::E("IpPrefix", "IpPrefix with invalid address! Shouldn't happen.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result += "/";
    *result += StringUtils::ToString(mPrefixLength);
    return NOERROR;
}

ECode IpPrefix::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadArrayOf((Handle32*)&mAddress);
    parcel->ReadInt32(&mPrefixLength);
    return NOERROR;
}

ECode IpPrefix::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteArrayOf((Handle32)mAddress.Get());
    dest->WriteInt32(mPrefixLength);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
