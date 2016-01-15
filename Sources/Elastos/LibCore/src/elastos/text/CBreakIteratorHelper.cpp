
#include "CBreakIteratorHelper.h"
#include "BreakIterator.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CBreakIteratorHelper, Singleton, IBreakIteratorHelper)

CAR_SINGLETON_IMPL(CBreakIteratorHelper)

ECode CBreakIteratorHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return BreakIterator::GetAvailableLocales(locales);
}

ECode CBreakIteratorHelper::GetCharacterInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetCharacterInstance(instance);
}

ECode CBreakIteratorHelper::GetCharacterInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetCharacterInstance(where,instance);
}

ECode CBreakIteratorHelper::GetLineInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetLineInstance(instance);
}

ECode CBreakIteratorHelper::GetLineInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetLineInstance(where,instance);
}

ECode CBreakIteratorHelper::GetSentenceInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetSentenceInstance(instance);
}

ECode CBreakIteratorHelper::GetSentenceInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetSentenceInstance(where,instance);
}

ECode CBreakIteratorHelper::GetWordInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetWordInstance(instance);
}

ECode CBreakIteratorHelper::GetWordInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetWordInstance(where,instance);
}

} // namespace Text
} // namespace Elastos