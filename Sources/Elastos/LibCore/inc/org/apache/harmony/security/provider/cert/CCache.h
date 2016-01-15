
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CCACHE_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CCACHE_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CCache.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CCache)
{
public:
    CCache();

    CARAPI GetHash(
        /* [in] */ ArrayOf<Byte>* arr,
        /* [out] */ Int64* hash);

    CARAPI Contains(
        /* [in] */ Int64 prefix_hash,
        /* [out] */ Boolean* isContained);

    CARAPI Get(
        /* [in] */ Int64 hash,
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ IInterface** object);

    CARAPI Put(
        /* [in] */ Int64 hash,
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [in] */ IInterface* object);

    CARAPI constructor(
        /* [in] */ Int32 pref_size,
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ Int32 pref_size);

    CARAPI constructor();

private:
    CARAPI GetSuffHash(
        /* [in] */ ArrayOf<Byte>* arr,
        /* [out] */ Int64* sh);

private:
    // Hash code consist of 6 bytes: AABB00
    // where:
    // AA - 2 bytes for prefix hash
    //      value generated on the base of the prefix of encoding
    // BB - 2 bytes for tail hash
    //      value generated on the base of the tail of encoding
    // 00 - 2 reserved bytes equals to 0
    //
    // Note, that it is possible for 2 different arrays to have
    // the similar hash codes.

    // The masks to work with hash codes:
    // the hash code without the reserved bytes
    static Int64 HASH_MASK = 0xFFFFFFFFFFFF0000LL;
    // the hash code of the prefix
    static Int64 PREFIX_HASH_MASK = 0xFFFFFFFF00000000LL;
    // the index value contained in reserved bytes
    static Int32 INDEX_MASK = 0x00FFFF;

    // size of the cache
    Int32 mCache_size;
    // the number of bytes which will be used for array hash generation.
    Int32 mPrefix_size;

    // The following 3 arrays contain the information about cached objects.
    // This information includes: hash of the array, encoded form of the object,
    // and the object itself.
    // The hash-encoding-object correspondence is made by means of index
    // in the particular array. I.e. for index N hash contained in hashes[N]
    // corresponds to the encoding contained in encodings[N] which corresponds
    // to the object cached at cache[N]

    // array containing the hash codes of encodings
    AutoPtr<ArrayOf<Int64> > mHashes;
    // array containing the encodings of the cached objects
    AutoPtr<ArrayOf<ByteArray > > mEncodings;
    // array containing the cached objects
    AutoPtr<ArrayOf<IInterface*> > mCache;

    // This array is used to speed up the process of the search in the cache.
    // This is an ordered array of the hash codes from 'hashes' array (described
    // above) with last 2 (reserved) bytes equals to the index of
    // the hash in the 'hashes' array. I.e. hash code ABCD00 with index 10 in
    // the hashes array will be represented in this array as ABCD0A (10==0x0A)
    // So this array contains ordered <hash to index> correspondences.
    // Note, that every item in this array is unique.
    AutoPtr<ArrayOf<Int64> > mHashes_idx;

    // the index of the last cached object
    Int32 mLast_cached;// = 0;
    // cache population indicator
    Boolean mCache_is_full;// = false;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CCACHE_H__
