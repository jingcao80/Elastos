
#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

ECode CWordIterator::SetCharSequence(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    WordIterator::SetCharSequence(charSequence, start, end);
    return NOERROR;
}

ECode CWordIterator::GetBeginning(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::GetBeginning(offset);
    return NOERROR;
}

ECode CWordIterator::GetEnd(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::GetEnd(offset);
    return NOERROR;
}

ECode CWordIterator::Preceding(
    /* [in] */ Int32 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::Preceding(position);
    return NOERROR;
}

ECode CWordIterator::Following(
    /* [in] */ Int32 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::Following(position);
    return NOERROR;
}

ECode CWordIterator::constructor()
{
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> locale;
    localeHelp->GetDefault((ILocale**)&locale);

    WordIterator::Init(locale);
    return NOERROR;
}

ECode CWordIterator::constructor(
    /* [in] */ ILocale* locale)
{
    WordIterator::Init(locale);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

