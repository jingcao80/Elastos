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

#include "BasicLruCache.h"
#include "CLinkedHashMap.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::IMapEntry;

namespace Libcore {
namespace Utility {

CAR_INTERFACE_IMPL(BasicLruCache, Object, IBasicLruCache)

BasicLruCache::BasicLruCache()
    : mMaxSize(1)
{
}

BasicLruCache::BasicLruCache(
    /* [in] */ Int32 maxSize)
{
    assert(maxSize > 0);
    mMaxSize = maxSize;
    ASSERT_SUCCEEDED(CLinkedHashMap::New(0, 0.75f, TRUE, (IMap**)&mMap));
}

BasicLruCache::~BasicLruCache()
{
}

ECode BasicLruCache::constructor(
    /* [in] */ Int32 maxSize)
{
    if (maxSize <= 0) {
        // throw new IllegalArgumentException("mMaxSize <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxSize = maxSize;
    FAIL_RETURN(CLinkedHashMap::New(0, 0.75f, TRUE, (IMap**)&mMap));
    return NOERROR;
}

ECode BasicLruCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** ouface)
{
    VALIDATE_NOT_NULL(ouface)

    {    AutoLock syncLock(this);
        if (key == NULL) {
            // throw new NullPointerException("key == null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IInterface> result;
        mMap->Get(key, (IInterface**)&result);
        if (result != NULL) {
            *ouface = result;
            REFCOUNT_ADD(*ouface)
            return NOERROR;
        }

        result = Create(key);

        AutoPtr<IInterface> midface;
        if (result != NULL) {
            mMap->Put(key, result, (IInterface**)&midface);
            TrimToSize(mMaxSize);
        }
        *ouface = result;
        REFCOUNT_ADD(*ouface)
    }
    return NOERROR;
}

ECode BasicLruCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    {    AutoLock syncLock(this);
        if (key == NULL || value == NULL) {
            if (outface) {
                *outface = NULL;
            }
            // throw new NullPointerException("key or value is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        mMap->Put(key, value, outface);
        TrimToSize(mMaxSize);
    }
    return NOERROR;
}

ECode BasicLruCache::Snapshot(
    /* [out] */ IMap** outmap)
{
    VALIDATE_NOT_NULL(outmap)

    {    AutoLock syncLock(this);
        AutoPtr<IMap> res;
        FAIL_RETURN(CLinkedHashMap::New(mMap, (IMap**)&res));
        *outmap = res;
        REFCOUNT_ADD(*outmap)
    }
    return NOERROR;
}

ECode BasicLruCache::EvictAll()
{
    {    AutoLock syncLock(this);
        TrimToSize(0);
    }
    return NOERROR;
}

void BasicLruCache::EntryEvicted(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
}

AutoPtr<IInterface> BasicLruCache::Create(
    /* [in] */ IInterface* key)
{
    return NULL;
}

void BasicLruCache::TrimToSize(
    /* [in] */ Int32 mMaxSize)
{
    Int32 len = 0;
    while ((mMap->GetSize(&len), len) > mMaxSize) {
        AutoPtr<IMapEntry> toEvict;
        ((CLinkedHashMap*)mMap.Get())->GetEldest((IMapEntry**)&toEvict);
        AutoPtr<IInterface> key;
        toEvict->GetKey((IInterface**)&key);
        AutoPtr<IInterface> value;
        toEvict->GetValue((IInterface**)&value);
        AutoPtr<IInterface> outface;
        mMap->Remove(key, (IInterface**)&outface);
        EntryEvicted(key, value);
    }
}

} // namespace Utility
} // namespace Libcore
