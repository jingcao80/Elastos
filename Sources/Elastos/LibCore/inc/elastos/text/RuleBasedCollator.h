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
