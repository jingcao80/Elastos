
#ifndef __ELASTOS_TEXT_CRULEBASEDCOLLATOR_H__
#define __ELASTOS_TEXT_CRULEBASEDCOLLATOR_H__

#include "_Elastos_Text_CRuleBasedCollator.h"
#include "RuleBasedCollator.h"

using Libcore::ICU::IRuleBasedCollatorICU;
using Elastos::Text::ICharacterIterator;
using Elastos::Text::ICollationKey;

namespace Elastos {
namespace Text {

CarClass(CRuleBasedCollator) , public RuleBasedCollator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CRULEBASEDCOLLATOR_H__
