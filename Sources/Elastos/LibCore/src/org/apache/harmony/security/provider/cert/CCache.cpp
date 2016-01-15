
#include "CCache.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CCache::CCache()
    : mCache_size(0)
    , mPrefix_size(0)
    , mLast_cached(0)
    , mCache_is_full(FALSE)
{}

ECode CCache::GetHash(
    /* [in] */ ArrayOf<Byte>* arr,
    /* [out] */ Int64* ret)
{
    VALIDATA_NOT_NULL(ret)
    Int64 hash = 0;
    for (Int32 i=1; i < mPrefix_size; i++) {
        hash += ((*arr)[i] & 0xFF);
    } // it takes about 2 bytes for prefix_size == 28

    // shift to the correct place
    hash = hash << 32;
    *ret = hash;
    return NOERROR;
}

ECode CCache::Contains(
    /* [in] */ Int64 prefix_hash,
    /* [out] */ Boolean* isContained)
{
    VALIDATA_NOT_NULL(isContained)
    if (prefix_hash == 0) {
        *isContained = FALSE;
        return NOERROR;
    }

    AutoPtr<IArrays> arrays;
    CArrays::AcquireSingleton((IArrays**)&arrays);
    Int32 index;
    arrays->BinarySearchInt64(mHashes_idx, prefix_hash, &index);
    Int32 idx = -1 * index - 1;
    int idx = -1*Arrays.binarySearch(hashes_idx, prefix_hash)-1;
    if (idx == mCache_size) {
        *isContained = FALSE;
        return NOERROR;
    } else {
        *isContained = ((*mHashes_idx)[idx] & PREFIX_HASH_MASK) == prefix_hash;
        return NOERROR;
    }
}

ECode CCache::Get(
    /* [in] */ Int64 hash,
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ IInterface** object)
{
    VALIDATA_NOT_NULL(object)
    Int64 suffHash;
    GetSuffHash(encoding, &suffHash);
    hash |= suffHash;
    if (hash == 0) {
        return NOERROR
    }
    AutoPtr<IArrays> arrays;
    CArrays::AcquireSingleton((IArrays**)&arrays);
    Int32 index;
    arrays->BinarySearchInt64(mHashes_idx, hash, &index);
    Int32 idx = -1 * index - 1;
    if (idx == mCache_size) {
        return NOERROR;
    }
    while (((*mHashes_idx)[idx] & HASH_MASK) == hash) {
        Int32 i = (Int32)((*mHashes_idx)[idx] & INDEX_MASK) - 1;
        if (encoding->Equals((*mEncodings)[i])) {
            *object = (*mCache)[i];
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
        idx++;
        if (idx == mCache_size) {
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CCache::Put(
    /* [in] */ Int64 hash,
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [in] */ IInterface* object)
{
    // check for empty space in the cache
    if (mLast_cached == mCache_size) {
        // so cache is full, will erase the first entry in the
        // cache (oldest entry). it could be better to throw out
        // rarely used value instead of oldest one..
        mLast_cached = 0;
        mCache_is_full = TRUE;
    }
    // index pointing to the item of the table to be overwritten
    Int32 index = mLast_cached++;

    // improve the hash value with info from the tail of encoding
    Int64 suffHash;
    GetSuffHash(encoding, &suffHash);
    hash |= suffHash;

    AutoPtr<IArrays> arrays;
    CArrays::AcquireSingleton((IArrays**)&arrays);
    if (mCache_is_full) {
        // indexing hash value to be overwritten:
        Int64 idx_hash = ((*mHashes)[index] | (index + 1));
        Int32 idx;
        arrays->BinarySearchInt64(mHashes_idx, idx_hash, &idx);
        if (idx < 0) {
            // it will never happen because we use saved hash value
            // (hashes[index])
            PFL_EX("WARNING! %d", idx)
            idx = -(idx + 1);
        }
        Int64 new_hash_idx = (hash | (index + 1));
        Int32 new_idx;
        arrays->BinarySearchInt64(mHashes_idx, new_hash_idx, &new_idx);

        if (new_idx >= 0) {
            // it's possible when we write the same hash in the same cell
            if (idx != new_idx) {
                // it will never happen because we use the same
                // hash and the same index in hash table
                PFL_EX("WARNING: ");
                PFL_EX(">> idx: %d new_idx: %d", idx, new_idx)
            }
        } else {
            new_idx = -(new_idx + 1);
            // replace in sorted array
            if (new_idx > idx) {
                mHashes_idx->Copy(idx, mHashes_idx, idx + 1, new_idx - idx - 1);
                (*mHashes_idx)[new_idx-1] = new_hash_idx;
            } else if (idx > new_idx) {
                mHashes_idx->Copy(new_idx+1, mHashes_idx, new_idx, idx - new_idx);
                (*mHashes_idx)[new_idx] = new_hash_idx;
            } else { // idx == new_idx
                (*mHashes_idx)[new_idx] = new_hash_idx;
            }
        }
    } else {
        Int64 idx_hash = (hash | (index + 1));
        int idx;
        arrays->BinarySearchInt64(mHashes_idx, idx_hash, &idx);
        if (idx < 0) {
            // it will always be true because idx_hash depends on index
            idx = -(idx + 1);
        }
        idx = idx - 1;
        if (idx != mCache_size - index - 1) {
            // if not in the cell containing 0 (free cell), do copy:
            mHashes_idx->Copy(cache_size - index - 1, mHashes_idx, cache_size - index,
                idx - (cache_size - index) + 1);
        }
        (*mHashes_idx)[idx] = idx_hash;
    }
    // overwrite the values in the tables:
    mHashes->Set(index, hash);
    mEncodings->Set(index, encoding);
    mCache->Set(index, object);
    return NOERROR;
}

ECode CCache::constructor(
    /* [in] */ Int32 pref_size,
    /* [in] */ Int32 size)
{
    mCache_size = size;
    mPrefix_size = pref_size;
    mHashes = ArrayOf<Int64>::Alloc(mCache_size);
    mHashes_idx = ArrayOf<Int64>::Alloc(mCache_size);
    mEncodings = ArrayOf<ByteArray >::Alloc(mCache_size);
    mCache = ArrayOf<IInterface*>::Alloc(mCache_size);
    return NOERROR;
}

ECode CCache::constructor(
    /* [in] */ Int32 pref_size)
{
    return constructor(pref_size, 9);
}

ECode CCache::constructor()
{
    return constructor(28, 9);
}

ECode CCache::GetSuffHash(
    /* [in] */ ArrayOf<Byte>* arr,
    /* [out] */ Int64* sh)
{
    VALIDATA_NOT_NULL(sh)
    Int64 hash_addon = 0;
    for (Int32 i = arr->GetLength() - 1; i > arr->GetLength() - mPrefix_size; i--) {
        hash_addon += ((*arr)[i] & 0xFF);
    }
    *sh = hash_addon << 16;
    return NOERROR;
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

