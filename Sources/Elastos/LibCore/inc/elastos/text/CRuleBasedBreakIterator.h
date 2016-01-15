
#ifndef __ELASTOS_TEXT_CRULEBASEBREAKITERATOR_H__
#define __ELASTOS_TEXT_CRULEBASEBREAKITERATOR_H__

#include "_Elastos_Text_CRuleBasedBreakIterator.h"
#include "RuleBasedBreakIterator.h"

namespace Elastos {
namespace Text {

CarClass(CRuleBasedBreakIterator) , public RuleBasedBreakIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CRULEBASEBREAKITERATOR_H__
