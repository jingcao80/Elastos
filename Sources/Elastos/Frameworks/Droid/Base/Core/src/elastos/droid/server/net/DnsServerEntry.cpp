#include "elastos/droid/server/net/DnsServerEntry.h"

using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CAR_INTERFACE_IMPL_2(DnsServerEntry, Object, IComparable, IDnsServerEntry)

DnsServerEntry::DnsServerEntry()
{
}

ECode DnsServerEntry::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int64 expiry)
{
    mAddress = address;
    mExpiry = expiry;
    return NOERROR;
}

ECode DnsServerEntry::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDnsServerEntry> entry = IDnsServerEntry::Probe(other);
    Int64 oe;
    entry->GetExpiry(&oe);
    *result = oe < mExpiry ? -1 : (oe == mExpiry ? 0 : 1);
    return NOERROR;
}

ECode DnsServerEntry::GetExpiry(
    /* [out] */ Int64* expiry)
{
    VALIDATE_NOT_NULL(expiry)
    *expiry = mExpiry;
    return NOERROR;
}

ECode DnsServerEntry::SetExpiry(
    /* [in] */ Int64 expiry)
{
    mExpiry = expiry;
    return NOERROR;
}

ECode DnsServerEntry::GetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)
    *address = mAddress;
    REFCOUNT_ADD(*address)
    return NOERROR;
}

ECode DnsServerEntry::SetAddress(
    /* [in] */ IInetAddress* address)
{
    mAddress = address;
    return NOERROR;
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos
