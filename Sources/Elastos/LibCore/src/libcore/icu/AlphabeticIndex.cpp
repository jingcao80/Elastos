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

#include "Elastos.CoreLibrary.Utility.h"
#include "AlphabeticIndex.h"
#include "ElStringByteSink.h"
#include "AutoLock.h"
#include "ImmutableIndex.h"
#include <unicode/alphaindex.h>
#include <unicode/uniset.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Libcore {
namespace ICU {

CAR_INTERFACE_IMPL(AlphabeticIndex, Object, IAlphabeticIndex)

AlphabeticIndex::AlphabeticIndex()
    : mPeer(0)
{
}

ECode AlphabeticIndex::constructor(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale)

    String s;
    locale->ToString(&s);
    mPeer = Create(s);
    return NOERROR;
}

AlphabeticIndex::~AlphabeticIndex()
{
    Destroy(mPeer);
    mPeer = 0;
}

ECode AlphabeticIndex::GetMaxLabelCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    {    AutoLock syncLock(this);
        *count = GetMaxLabelCount(mPeer);
    }
    return NOERROR;
}

ECode AlphabeticIndex::SetMaxLabelCount(
    /* [in] */ Int32 count)
{
    {    AutoLock syncLock(this);
        SetMaxLabelCount(mPeer, count);
    }
    return NOERROR;
}

ECode AlphabeticIndex::AddLabels(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale)

    {    AutoLock syncLock(this);
        String s;
        locale->ToString(&s);
        AddLabels(mPeer, s);
    }
    return NOERROR;
}

ECode AlphabeticIndex::AddLabelRange(
    /* [in] */ Int32 codePointStart,
    /* [in] */ Int32 codePointEnd)
{
    {    AutoLock syncLock(this);
        AddLabelRange(mPeer, codePointStart, codePointEnd);
    }
    return NOERROR;
}

ECode AlphabeticIndex::GetBucketCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    {    AutoLock syncLock(this);
        *count = GetBucketCount(mPeer);
    }
    return NOERROR;
}

ECode AlphabeticIndex::GetBucketIndex(
    /* [in] */ const String& s,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    {    AutoLock syncLock(this);
        *index = GetBucketIndex(mPeer, s);
    }
    return NOERROR;
}

ECode AlphabeticIndex::GetBucketLabel(
    /* [in] */ Int32 index,
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label)

    {    AutoLock syncLock(this);
        *label = GetBucketLabel(mPeer, index);
    }
    return NOERROR;
}

ECode AlphabeticIndex::GetImmutableIndex(
        /* [out] */ IImmutableIndex** index)
{
    VALIDATE_NOT_NULL(index)

    {    AutoLock syncLock(this);
        *index = new ImmutableIndex(BuildImmutableIndex(mPeer));
        REFCOUNT_ADD(*index);
    }
    return NOERROR;
}

Int64 AlphabeticIndex::Create(
    /* [in] */ const String& locale)
{
    UErrorCode status = U_ZERO_ERROR;
    NATIVE(Locale) icuLocale;
    icuLocale.setToBogus();
    icuLocale = NATIVE(Locale)::createFromName(locale);
    if (icuLocale.isBogus()) {
        return 0;
    }
    NATIVE(AlphabeticIndex)* ai = new NATIVE(AlphabeticIndex)(icuLocale, status);
    if(!U_SUCCESS(status))
        return 0;
    return reinterpret_cast<uintptr_t>(ai);
}

static NATIVE(AlphabeticIndex)* fromPeer(
    /* [in] */ Int64 peer)
{
  return reinterpret_cast<NATIVE(AlphabeticIndex)*>(static_cast<uintptr_t>(peer));
}

void AlphabeticIndex::Destroy(
    /* [in] */ Int64 peer)
{
    delete fromPeer(peer);
}

Int32 AlphabeticIndex::GetMaxLabelCount(
    /* [in] */ Int64 peer)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    return ai->getMaxLabelCount();
}

void AlphabeticIndex::SetMaxLabelCount(
    /* [in] */ Int64 peer,
    /* [in] */ Int32 count)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    UErrorCode status = U_ZERO_ERROR;
    ai->setMaxLabelCount(count, status);
}

void AlphabeticIndex::AddLabels(
    /* [in] */ Int64 peer,
    /* [in] */ const String& locale)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    NATIVE(Locale) icuLocale;
    icuLocale.setToBogus();
    icuLocale = NATIVE(Locale)::createFromName(locale);
    if (icuLocale.isBogus()) {
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    ai->addLabels(icuLocale, status);
}

void AlphabeticIndex::AddLabelRange(
    /* [in] */ Int64 peer,
    /* [in] */ Int32 codePointStart,
    /* [in] */ Int32 codePointEnd)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    UErrorCode status = U_ZERO_ERROR;
    ai->addLabels(UnicodeSet(codePointStart, codePointEnd), status);
}

Int32 AlphabeticIndex::GetBucketCount(
    /* [in] */ Int64 peer)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    UErrorCode status = U_ZERO_ERROR;
    Int32 result = ai->getBucketCount(status);
    if (!U_SUCCESS(status)) {
        return -1;
    }
    return result;
}

Int32 AlphabeticIndex::GetBucketIndex(
    /* [in] */ Int64 peer,
    /* [in] */ const String& s)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    if (s.IsNull()) {
        return -1;
    }
    UnicodeString string;
    AutoPtr<ArrayOf<Char16> > char16s = s.GetChar16s();
    string.setTo(false, (UChar*)char16s->GetPayload(), char16s->GetLength());
    UErrorCode status = U_ZERO_ERROR;
    Int32 result = ai->getBucketIndex(string, status);
    if (!U_SUCCESS(status)) {
        return -1;
    }
    return result;
}

String AlphabeticIndex::GetBucketLabel(
    /* [in] */ Int64 peer,
    /* [in] */ Int32 index)
{
    if (index < 0) {
        return String(NULL);
    }

    // Iterate to the nth bucket.
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    UErrorCode status = U_ZERO_ERROR;
    ai->resetBucketIterator(status);
    if (!U_SUCCESS(status)) {
        return String(NULL);
    }
    for (Int32 i = 0; i <= index; ++i)
    {
        if (!ai->nextBucket(status)) {
            return String(NULL);
        }
        if (!U_SUCCESS(status)) {
            return String(NULL);
        }
    }

    // Return "" for the underflow/inflow/overflow buckets.
    if (ai->getBucketLabelType() != U_ALPHAINDEX_NORMAL) {
        return String("");
    }

    const UnicodeString& label(ai->getBucketLabel());
    String s;
    ElStringByteSink sink(&s);
    label.toUTF8(sink);
    return s;
}

Int64 AlphabeticIndex::BuildImmutableIndex(
    /* [in] */ Int64 peer)
{
    NATIVE(AlphabeticIndex)* ai = fromPeer(peer);
    UErrorCode status = U_ZERO_ERROR;
    NATIVE(AlphabeticIndex::ImmutableIndex)* ii = ai->buildImmutableIndex(status);
    if (!U_SUCCESS(status)) {
        return 0;
    }
    return reinterpret_cast<uintptr_t>(ii);
}

} // namespace ICU
} // namespace Libcore
