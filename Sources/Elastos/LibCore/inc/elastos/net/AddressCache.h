
#ifndef __ELASTOS_NET_ADDRESSCACHE_H__
#define __ELASTOS_NET_ADDRESSCACHE_H__

#include "elastos/core/Object.h"

using Libcore::Utility::IBasicLruCache;

namespace Elastos {
namespace Net {

class AddressCacheEntry
    : public Object
{
public:
    AddressCacheEntry(
        /* [in] */ ArrayOf<IInetAddress*>* value);

    AddressCacheEntry(
        /* [in] */ const String& value);

    ~AddressCacheEntry();

    // Either an InetAddress[] for a positive entry,
    // or a String detail message for a negative entry.
    AutoPtr< ArrayOf<IInetAddress*> > mValue;
    String mStringValue;

    /**
     * The absolute expiry time in nanoseconds. Nanoseconds from System.nanoTime is ideal
     * because -- unlike System.currentTimeMillis -- it can never go backwards.
     *
     * We don't need to worry about overflow with a TTL_NANOS of 2s.
     */
    Int64 mExpiryNanos;
};

/**
 * Implements caching for {@code InetAddress}. We use a unified cache for both positive and negative
 * cache entries.
 *
 * TODO: benchmark and optimize InetAddress until we get to the point where we can just rely on
 * the C library level caching. The main thing caching at this level buys us is avoiding repeated
 * conversions from 'struct sockaddr's to InetAddress[].
 */
class AddressCache
    : public Object
{
private:
    class AddressCacheKey
        : public Object
    {
    public:
        AddressCacheKey(
            /* [in] */ const String& hostname,
            /* [in] */ Int32 netId);

        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashcode);

        private:
            String mHostname;
            Int32 mNetId;
    };

public:
    AddressCache();

    ~AddressCache();

    /**
     * Removes all entries from the cache.
     */
    CARAPI_(void) Clear();

    /**
     * Returns the cached InetAddress[] for 'hostname' on network 'netId'. Returns null
     * if nothing is known about 'hostname'. Returns a String suitable for use as an
     * UnknownHostException detail message if 'hostname' is known not to exist.
     */
    CARAPI_(AutoPtr<AddressCacheEntry>) Get(
        /* [in] */ const String& hostname,
        /* [in] */ Int32 netId);

    /**
     * Associates the given 'addresses' with 'hostname'. The association will expire after a
     * certain length of time.
     */
    CARAPI_(void) Put(
        /* [in] */ const String& hostname,
        /* [in] */ Int32 netId,
        /* [in] */ ArrayOf<IInetAddress*>* addresses);

    /**
     * Records that 'hostname' is known not to have any associated addresses. (I.e. insert a
     * negative cache entry.)
     */
    CARAPI_(void) PutUnknownHost(
        /* [in] */ const String& hostname,
        /* [in] */ Int32 netId,
        /* [in] */ const String& detailMessage);

private:
    friend class AddressCacheEntry;

    /**
     * When the cache contains more entries than this, we start dropping the oldest ones.
     * This should be a power of two to avoid wasted space in our custom map.
     */
    static const Int32 MAX_ENTRIES;

    //The TTL for the Java-level cache is short ,just 2s.
    static const Int64 TTL_NANOS;

    // The actual cache.
    AutoPtr<IBasicLruCache> mCache;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_ADDRESSCACHE_H__
