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

#include "ImmutableIndex.h"
#include <unicode/alphaindex.h>
#include <unicode/uniset.h>
#include "AutoLock.h"
#include "ElStringByteSink.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_ImmutableIndex = {
    { 0xfd73b454, 0x5192, 0x4d43, { 0x9a, 0x18, 0xc5, 0xc2, 0x70, 0x10, 0x49, 0xa0 } },
    (char *)c_pElastos_CoreLibraryUunm,
    0xf6f1637e };

CAR_INTERFACE_IMPL(ImmutableIndex, Object, IImmutableIndex)

ImmutableIndex::ImmutableIndex(Int64 peer)
{
    mPeer = peer;
}

ImmutableIndex::~ImmutableIndex()
{
    {    AutoLock syncLock(this);
        AlphabeticIndex::Destroy(mPeer);
        mPeer = 0;
    }
}

ECode ImmutableIndex::GetBucketCount(
    /* [out] */ Int32* count)
{
    *count = GetBucketCount(mPeer);
    return NOERROR;
}

ECode ImmutableIndex::GetBucketIndex(
    /* [in] */ const String& s,
    /* [out] */ Int32* index)
{
    *index = GetBucketIndex(mPeer, s);
    return NOERROR;
}

ECode ImmutableIndex::GetBucketLabel(
    /* [in] */ Int32 index,
    /* [out] */ String* label)
{
    *label = GetBucketLabel(mPeer, index);
    return NOERROR;
}

static NATIVE(AlphabeticIndex::ImmutableIndex)* immutableIndexFromPeer(Int64 peer) {
  return reinterpret_cast<NATIVE(AlphabeticIndex::ImmutableIndex)*>(static_cast<uintptr_t>(peer));
}

Int32 ImmutableIndex::GetBucketCount(Int64 peer)
{
    NATIVE(AlphabeticIndex::ImmutableIndex)* ii = immutableIndexFromPeer(peer);
    return ii->getBucketCount();
}

Int32 ImmutableIndex::GetBucketIndex(Int64 peer, const String& s)
{
    NATIVE(AlphabeticIndex::ImmutableIndex)* ii = immutableIndexFromPeer(peer);
    if(s.IsNull())
        return -1;
    UErrorCode status = U_ZERO_ERROR;
    Int32 result = ii->getBucketIndex(UnicodeString::fromUTF8(s.string()), status);
    if(!U_SUCCESS(status))
        return -1;
    return result;
}

String ImmutableIndex::GetBucketLabel(Int64 peer, Int32 index)
{
    NATIVE(AlphabeticIndex::ImmutableIndex)* ii = immutableIndexFromPeer(peer);
    const NATIVE(AlphabeticIndex::Bucket)* bucket = ii->getBucket(index);
    if(NULL == bucket) {
        return String(NULL);
    }

    // Return "" for the underflow/inflow/overflow buckets.
    if (bucket->getLabelType() != U_ALPHAINDEX_NORMAL) {
        return String("");
    }

    const UnicodeString& label(bucket->getLabel());
    String s;
    ElStringByteSink sink(&s);
    label.toUTF8(sink);
    return s;
}

ECode ImmutableIndex::ToString(
        /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = String("ImmutableIndex");
    return NOERROR;
}

ECode ImmutableIndex::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID)

    *pCLSID = ECLSID_ImmutableIndex;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
