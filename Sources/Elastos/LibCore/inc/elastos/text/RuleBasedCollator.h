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

#ifndef __ELASTOS_TEXT_RULEBASEDCOLLATOR_H__
#define __ELASTOS_TEXT_RULEBASEDCOLLATOR_H__

#include "Collator.h"

using Libcore::ICU::ICollationElementIteratorICU;
using Elastos::Text::ICharacterIterator;
using Elastos::Text::ICollationKey;

namespace Elastos {
namespace Text {

class RuleBasedCollator
    : public Collator
    , public IRuleBasedCollator
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IRuleBasedCollatorICU* wrapper);

    CARAPI constructor(
        /* [in] */ const String& rules);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    virtual CARAPI GetCollationElementIterator(
        /* [in] */ ICharacterIterator* source,
        /* [out] */ ICollationElementIterator** collationElementIterator);

    virtual CARAPI GetCollationElementIterator(
        /* [in] */ const String& source,
        /* [out] */ ICollationElementIterator** collationElementIterator);

    virtual CARAPI GetRules(
        /* [out] */ String* rules);

    //@Override
    CARAPI Compare(
        /* [in] */ const String& source,
        /* [in] */ const String& target,
        /* [out] */ Int32* value);

    //@Override
    CARAPI GetCollationKey(
        /* [in] */ const String& source,
        /* [out] */ ICollationKey ** collationKey);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ const String& string1,
        /* [in] */ const String& string2,
        /* [out] */ Boolean* result);
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_RULEBASEDCOLLATOR_H__
