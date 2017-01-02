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

#ifndef __LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__
#define __LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__

#include <Object.h>
#include "_Libcore_ICU_CRuleBasedCollatorICU.h"

using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::Text::ICollationKey;
using Elastos::Text::ICharacterIterator;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CRuleBasedCollatorICU)
    , public Object
    , public ICloneable
    , public IRuleBasedCollatorICU
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRuleBasedCollatorICU();

    ~CRuleBasedCollatorICU();

    CARAPI constructor(
        /* [in] */ const String& rules);

    CARAPI constructor(
        /* [in] */ ILocale * locale);

    CARAPI Clone(
        /* [out] */ IInterface ** outruleicu);

    CARAPI Compare(
        /* [in] */ const String& source,
        /* [in] */ const String& target,
        /* [out] */ Int32 * value);

    CARAPI GetDecomposition(
        /* [out] */ Int32 * value);

    CARAPI SetDecomposition(
        /* [in] */ Int32 mode);

    CARAPI GetStrength(
        /* [out] */ Int32 * value);

    CARAPI SetStrength(
        /* [in] */ Int32 strength);

    CARAPI SetAttribute(
        /* [in] */ Int32 type,
        /* [in] */ Int32 value);

    CARAPI GetAttribute(
        /* [in] */ Int32 type,
        /* [out] */ Int32 * value);

    CARAPI GetCollationKey(
        /* [in] */ const String& source,
        /* [out] */ ICollationKey ** outkey);

    CARAPI GetRules(
        /* [out] */ String * str);

    CARAPI GetCollationElementIterator(
        /* [in] */ const String& source,
        /* [out] */ ICollationElementIteratorICU ** outicu);

    CARAPI GetCollationElementIterator(
        /* [in] */ ICharacterIterator * it,
        /* [out] */ ICollationElementIteratorICU ** outicu);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI Equals(
        /* [in] */ const String& source,
        /* [in] */ const String& target,
        /* [out] */ Boolean * value);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

private:
    CRuleBasedCollatorICU(
        /* [in] */ Int64 address);

    CARAPI_(String) CharacterIteratorToString(
        /* [in] */ ICharacterIterator * it);

private:
    Int64 mAddress;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CRULEBASEDCOLLATORICU_H__
