#ifndef __ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__
#define __ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__


#include "BreakIterator.h"

using Elastos::Text::ICharacterIterator;

namespace Elastos {
namespace Text {

class RuleBasedBreakIterator
    : public BreakIterator
{
public:

    CARAPI constructor(
        /* [in] */ INativeBreakIterator* iterator);

    virtual ~RuleBasedBreakIterator();

    //@Override
    CARAPI GetCurrent(
        /* [out] */ Int32* currentValue);

    //@Override
    CARAPI GetFirst(
        /* [out] */ Int32* firstValue);

    //@Override
    CARAPI GetFollowing(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* followingValue);

    //@Override
    CARAPI GetText(
        /* [out] */ ICharacterIterator** text);

    //@Override
    CARAPI GetLast(
        /* [out] */ Int32* lastValue);

    //@Override
    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    //@Override
    CARAPI GetNext(
        /* [in] */ Int32 n,
        /* [out] */ Int32* nextValue);

    //@Override
    CARAPI GetPrevious(
        /* [out] */ Int32* previousValue);

    CARAPI SetText(
        /* [in] */ const String& newText);

    //@Override
    CARAPI SetText(
        /* [in] */ ICharacterIterator* newText);

    //@Override
    CARAPI IsBoundary(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* isBoundary);

    //@Override
    CARAPI GetPreceding(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* precedingValue);

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface ** outface);

private:

    CARAPI CheckOffset(
        /* [in] */ Int32 offset);
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_RULEBASEDBREAKITERATOR_H__
