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

#ifndef __LIBCORE_ICU_IMMUTABLEINDEX_H__
#define __LIBCORE_ICU_IMMUTABLEINDEX_H__

#include "Object.h"
#include "AlphabeticIndex.h"


namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_ImmutableIndex;

class ImmutableIndex
    : public Object
    , public IImmutableIndex
{
public:
    CAR_INTERFACE_DECL()

protected:
    ~ImmutableIndex();

private:
    ImmutableIndex(
        /* [in] */ Int64 peer);

public:
    CARAPI GetBucketCount(
        /* [out] */ Int32* count);

    CARAPI GetBucketIndex(
        /* [in] */ const String& s,
        /* [out] */ Int32* index);

    CARAPI GetBucketLabel(
        /* [in] */ Int32 index,
        /* [out] */ String* label);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

private:
    CARAPI_(Int32) GetBucketCount(Int64 peer);

    CARAPI_(Int32) GetBucketIndex(Int64 peer, const String& s);

    CARAPI_(String) GetBucketLabel(Int64 peer, Int32 index);


private:
    Int64 mPeer;

    friend class AlphabeticIndex;
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_IMMUTABLEINDEX_H__