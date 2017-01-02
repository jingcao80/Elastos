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

#ifndef __LIBCORE_ICU_ALPHABETICINDEX_H__
#define __LIBCORE_ICU_ALPHABETICINDEX_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

/**
 * Exposes icu4c's AlphabeticIndex.
 */
class AlphabeticIndex
    : public Object
    , public IAlphabeticIndex
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    AlphabeticIndex();

    virtual ~AlphabeticIndex();

public:
    CARAPI GetMaxLabelCount(
        /* [out] */ Int32* count);

    CARAPI SetMaxLabelCount(
        /* [in] */ Int32 count);

    CARAPI AddLabels(
        /* [in] */ ILocale* locale);

    CARAPI AddLabelRange(
        /* [in] */ Int32 codePointStart,
        /* [in] */ Int32 codePointEnd);

    CARAPI GetBucketCount(
        /* [out] */ Int32* count);

    CARAPI GetBucketIndex(
        /* [in] */ const String& s,
        /* [out] */ Int32* index);

    CARAPI GetBucketLabel(
        /* [in] */ Int32 index,
        /* [out] */ String* label);

    CARAPI GetImmutableIndex(
        /* [out] */ IImmutableIndex** index);

private:
    friend class ImmutableIndex;

    static CARAPI_(Int64) Create(const String& locale);

    static CARAPI_(void) Destroy(Int64 peer);

    static CARAPI_(Int32) GetMaxLabelCount(Int64 peer);

    static CARAPI_(void) SetMaxLabelCount(Int64 peer, Int32 count);

    static CARAPI_(void) AddLabels(Int64 peer, const String& locale);

    static CARAPI_(void) AddLabelRange(Int64 peer, Int32 codePointStart, Int32 codePointEnd);

    static CARAPI_(Int32) GetBucketCount(Int64 peer);

    static CARAPI_(Int32) GetBucketIndex(Int64 peer, const String& s);

    static CARAPI_(String) GetBucketLabel(Int64 peer, Int32 index);

    static CARAPI_(Int64) BuildImmutableIndex(Int64 peer);

private:
    Int64 mPeer;
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_ALPHABETICINDEX_H__