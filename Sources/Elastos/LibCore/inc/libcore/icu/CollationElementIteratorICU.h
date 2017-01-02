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

#ifndef __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__
#define __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Text::ICharacterIterator;

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_CollationElementIteratorICU;

class CollationElementIteratorICU
    : public Object
    , public ICollationElementIteratorICU
{
public:
    CAR_INTERFACE_DECL()

    CollationElementIteratorICU();

    ~CollationElementIteratorICU();

    CARAPI Reset();

    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    CARAPI GetPrevious(
        /* [out] */ Int32* previousValue);

    CARAPI GetMaxExpansion(
        /* [in] */ Int32 order,
        /* [out] */ Int32* maxExpansion);

    CARAPI SetText(
        /* [in] */ const String& source);

    CARAPI SetText(
        /* [in] */ ICharacterIterator* source);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI SetOffset(
        /* [in] */ Int32 offset);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    static CARAPI_(Int32) PrimaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(Int32) SecondaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(Int32) TertiaryOrder(
        /* [in] */ Int32 order);

    static CARAPI_(AutoPtr<ICollationElementIteratorICU>) GetInstance(
        /* [in] */ Int64 collatorAddress,
        /* [in] */ const String& source);

private:
    CollationElementIteratorICU(
        /* [in] */ Int64 collelemiteratoraddress);

private:
    Int64 mAddress;

    const static Int32 PRIMARY_ORDER_MASK_;

    const static Int32 SECONDARY_ORDER_MASK_;

    const static Int32 TERTIARY_ORDER_MASK_;

    const static Int32 PRIMARY_ORDER_SHIFT_;

    const static Int32 SECONDARY_ORDER_SHIFT_;

    const static Int32 UNSIGNED_16_BIT_MASK_;
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_COLLATIONELEMENTITERATORICU_H__
