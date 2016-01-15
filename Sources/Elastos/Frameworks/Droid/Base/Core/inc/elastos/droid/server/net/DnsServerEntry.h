#ifndef __ELASTOS_DROID_SERVER_NET_DNSSERVERENTRY_H__
#define __ELASTOS_DROID_SERVER_NET_DNSSERVERENTRY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Net::IDnsServerEntry;
using Elastos::Core::IComparable;
using Elastos::Core::Object;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

/**
 * Represents a DNS server entry with an expiry time.
 *
 * Implements Comparable so DNS server entries can be sorted by lifetime, longest-lived first.
 * The ordering of entries with the same lifetime is unspecified, because given two servers with
 * identical lifetimes, we don't care which one we use, and only comparing the lifetime is much
 * faster than comparing the IP address as well.
 *
 * Note: this class has a natural ordering that is inconsistent with equals.
 */
class DnsServerEntry
    : public Object
    , public IComparable
    , public IDnsServerEntry
{
public:
    CAR_INTERFACE_DECL()

    DnsServerEntry();

    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int64 expiry);

    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    CARAPI GetExpiry(
        /* [out] */ Int64* expiry);

    CARAPI SetExpiry(
        /* [in] */ Int64 expiry);

    CARAPI GetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI SetAddress(
        /* [in] */ IInetAddress* address);

public:
    /** The IP address of the DNS server. */
    AutoPtr<IInetAddress> mAddress;

    /** The time until which the DNS server may be used. A Java millisecond time as might be
      * returned by currentTimeMillis(). */
    Int64 mExpiry;

};


} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_DNSSERVERENTRY_H__