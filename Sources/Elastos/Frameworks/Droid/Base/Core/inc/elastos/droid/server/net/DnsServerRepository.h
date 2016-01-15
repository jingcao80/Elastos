#ifndef __ELASTOS_DROID_SERVER_NET_DNSSERVERREPOSITORY_H__
#define __ELASTOS_DROID_SERVER_NET_DNSSERVERREPOSITORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Server::Net::IDnsServerRepository;
using Elastos::Core::Object;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ISet;

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
class DnsServerRepository
    : public Object
    , public IDnsServerRepository
{
public:
    CAR_INTERFACE_DECL()

    DnsServerRepository();

    CARAPI constructor();

    /** Sets the DNS servers of the provided LinkProperties object to the current servers. */
    CARAPI SetDnsServersOn(
        /* [in] */ ILinkProperties* lp);

    /**
     * Notifies the class of new DNS server information.
     * @param lifetime the time in seconds that the DNS servers are valid.
     * @param addresses the string representations of the IP addresses of the DNS servers to use.
     */
    CARAPI AddServers(
        /* [in] */ Int64 lifetime,
        /* [in] */ ArrayOf<String>* addresses,
        /* [out] */ Boolean* isAdded);

private:
    CARAPI_(Boolean) UpdateExistingEntry(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int64 expiry);

    CARAPI_(Boolean) UpdateCurrentServers();

private:
    /** Stores up to {@code NUM_CURRENT_SERVERS} DNS servers we're currently using. */
    // private Set<InetAddress> mCurrentServers;
    AutoPtr<ISet> mCurrentServers;

    const static String TAG;

    /**
     * Stores all the DNS servers we know about, for use when the current servers expire.
     * Always sorted in order of decreasing expiry. The elements in this list are also the values
     * of mIndex, and may be elements in mCurrentServers.
     */
    // private ArrayList<DnsServerEntry> mAllServers;
     AutoPtr<IArrayList> mAllServers;

    /**
     * Indexes the servers so we can update their lifetimes more quickly in the common case where
     * servers are not being added, but only being refreshed.
     */
    // private HashMap<InetAddress, DnsServerEntry> mIndex;
     AutoPtr<IHashMap> mIndex;

};


} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_DNSSERVERREPOSITORY_H__