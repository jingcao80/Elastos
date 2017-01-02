//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "AddressCache.h"
#include "CSystem.h"
#include "CBasicLruCache.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Libcore::Utility::CBasicLruCache;

namespace Elastos {
namespace Net {

/**
 * When the cache contains more entries than this, we start dropping the oldest ones.
 * This should be a power of two to avoid wasted space in our custom map.
 */
const Int32 AddressCache::MAX_ENTRIES = 16;

// The TTL for the Java-level cache is short, just 2s.
const Int64 AddressCache::TTL_NANOS = 2 * 1000000000ll;

//=================================================================
// AddressCache::AddressCacheKey
//=================================================================
AddressCache::AddressCacheKey::AddressCacheKey(
    /* [in] */ const String& hostname,
    /* [in] */ Int32 netId)
    : mHostname(hostname)
    , mNetId(netId)
{
}

ECode AddressCache::AddressCacheKey::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (Probe(EIID_IInterface) == other) {
        *result = TRUE;
    }

    AddressCacheKey* ask = (AddressCacheKey*)IObject::Probe(other);
    *result = mHostname.Equals(ask->mHostname);
    return NOERROR;
}

ECode AddressCache::AddressCacheKey::GetHashCode(
    /* [out] */ Int32* hashcode)
{
    VALIDATE_NOT_NULL(hashcode)
    Int32 result = 17;
    result = 31 * result + mNetId;
    result = 31 * result + mHostname.GetHashCode();
    *hashcode = result;
    return NOERROR;
}

//=================================================================
// AddressCacheEntry
//=================================================================

AddressCacheEntry::AddressCacheEntry(
    /* [in] */ ArrayOf<IInetAddress*>* value)
    : mValue(value)
    , mExpiryNanos(0)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetNanoTime(&mExpiryNanos);
    mExpiryNanos += AddressCache::TTL_NANOS;
}

AddressCacheEntry::AddressCacheEntry(
    /* [in] */ const String& value)
    : mStringValue(value)
    , mExpiryNanos(0)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetNanoTime(&mExpiryNanos);
    mExpiryNanos += AddressCache::TTL_NANOS;
}

AddressCacheEntry::~AddressCacheEntry()
{
    mValue = NULL;
}

//=================================================================
// AddressCache
//=================================================================

AddressCache::AddressCache()
{
    CBasicLruCache::New(MAX_ENTRIES, (IBasicLruCache**)&mCache);
}

AddressCache::~AddressCache()
{
    if (mCache) {
        mCache->EvictAll();
        mCache = NULL;
    }
}

void AddressCache::Clear()
{
    mCache->EvictAll();
}

/**
 * Returns the cached addresses associated with 'hostname'. Returns null if nothing is known
 * about 'hostname'. Returns an empty array if 'hostname' is known not to exist.
 */
AutoPtr<AddressCacheEntry> AddressCache::Get(
    /* [in] */ const String& hostname,
    /* [in] */ Int32 netId)
{
    AutoPtr<AddressCacheKey> key = new AddressCacheKey(hostname, netId);
    AutoPtr<IInterface> obj;
    mCache->Get(TO_IINTERFACE(key), (IInterface**)&obj);

    if (obj != NULL) {
        AutoPtr<AddressCacheEntry> entry = (AddressCacheEntry*)IObject::Probe(obj);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nanos;
        system->GetNanoTime(&nanos);

        // Do we have a valid cache entry?
        if (entry->mExpiryNanos >= nanos) {
            return entry;
        }
    }

    // Either we didn't find anything, or it had expired.
    // No need to remove expired entries: the caller will provide a replacement shortly.
    return NULL;
}

/**
 * Associates the given 'addresses' with 'hostname'. The association will expire after a
 * certain length of time.
 */
void AddressCache::Put(
    /* [in] */ const String& hostname,
    /* [in] */ Int32 netId,
    /* [in] */ ArrayOf<IInetAddress*>* addresses)
{
    AutoPtr<AddressCacheKey> key = new AddressCacheKey(hostname, netId);
    AutoPtr<AddressCacheEntry> entry = new AddressCacheEntry(addresses);
    AutoPtr<IInterface> outValue;
    mCache->Put(TO_IINTERFACE(key), TO_IINTERFACE(entry), (IInterface**)&outValue);
}

/**
 * Records that 'hostname' is known not to have any associated addresses. (I.e. insert a
 * negative cache entry.)
 */
void AddressCache::PutUnknownHost(
    /* [in] */ const String& hostname,
    /* [in] */ Int32 netId,
    /* [in] */ const String& detailMessage)
{
    AutoPtr<AddressCacheKey> key = new AddressCacheKey(hostname, netId);
    AutoPtr<AddressCacheEntry> entry = new AddressCacheEntry(detailMessage);
    AutoPtr<IInterface> outValue;
    mCache->Put(TO_IINTERFACE(key), TO_IINTERFACE(entry), (IInterface**)&outValue);
}

} // namespace Net
} // namespace Elastos
