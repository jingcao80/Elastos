
#include "BreakIterator.h"
#include "CRuleBasedBreakIterator.h"
#include "CLocaleHelper.h"
#include "CLocaleDataHelper.h"
#include "ICUUtil.h"
#include "CNativeBreakIteratorHelper.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ICUUtil;
using Libcore::ICU::INativeBreakIteratorHelper;
using Libcore::ICU::CNativeBreakIteratorHelper;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_2(BreakIterator, Object, IBreakIterator, ICloneable)

BreakIterator::BreakIterator()
{}

BreakIterator::BreakIterator(
    /* [in] */ INativeBreakIterator* iterator)
    :mWrapped(iterator)
{
}

ECode BreakIterator::constructor(
    /* [in] */ INativeBreakIterator* iterator)
{
    mWrapped = iterator;
    return NOERROR;
}

ECode BreakIterator::CloneImpl(
    /* [in] */ IBreakIterator* object)
{
    assert(object);
    BreakIterator* bi = (BreakIterator*)object;

    AutoPtr<IInterface> temp;
    ICloneable::Probe(mWrapped)->Clone((IInterface**)&temp);
    bi->mWrapped = INativeBreakIterator::Probe(temp);
    return NOERROR;
}

ECode BreakIterator::GetAvailableLocales(
    /* [out] */ ArrayOf<ILocale*>** locales)
{
    return ICUUtil::GetAvailableBreakIteratorLocales(locales);
}

ECode BreakIterator::GetCharacterInstance(
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetCharacterInstance(locale, instance);
}

ECode BreakIterator::GetCharacterInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    VALIDATE_NOT_NULL(where);

    AutoPtr<INativeBreakIterator> iter;
    AutoPtr<INativeBreakIteratorHelper> iterhelper;
    CNativeBreakIteratorHelper::AcquireSingleton((INativeBreakIteratorHelper **)&iterhelper);
    iterhelper->GetCharacterInstance(where,(INativeBreakIterator **)&iter);
    AutoPtr<IBreakIterator> rbbi;
    CRuleBasedBreakIterator::New(iter,(IBreakIterator **)&rbbi);
    *instance = rbbi;
    REFCOUNT_ADD(*instance);

    return NOERROR;
}

ECode BreakIterator::GetLineInstance(
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetLineInstance(locale, instance);
}

ECode BreakIterator::GetLineInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    VALIDATE_NOT_NULL(where);

    AutoPtr<INativeBreakIterator> iter;
    AutoPtr<INativeBreakIteratorHelper> iterhelper;
    CNativeBreakIteratorHelper::AcquireSingleton((INativeBreakIteratorHelper **)&iterhelper);
    iterhelper->GetLineInstance(where,(INativeBreakIterator **)&iter);
    AutoPtr<IBreakIterator> rbbi;
    CRuleBasedBreakIterator::New(iter,(IBreakIterator **)&rbbi);
    *instance = rbbi;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode BreakIterator::GetSentenceInstance(
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetSentenceInstance(locale, instance);
}

ECode BreakIterator::GetSentenceInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    VALIDATE_NOT_NULL(where);

    AutoPtr<INativeBreakIterator> iter;
    AutoPtr<INativeBreakIteratorHelper> iterhelper;
    CNativeBreakIteratorHelper::AcquireSingleton((INativeBreakIteratorHelper **)&iterhelper);
    iterhelper->GetSentenceInstance(where,(INativeBreakIterator **)&iter);
    AutoPtr<IBreakIterator> rbbi;
    CRuleBasedBreakIterator::New(iter,(IBreakIterator **)&rbbi);
    *instance = rbbi;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode BreakIterator::GetWordInstance(
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetWordInstance(locale, instance);
}

ECode BreakIterator::GetWordInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    VALIDATE_NOT_NULL(where);

    AutoPtr<INativeBreakIterator> iter;
    AutoPtr<INativeBreakIteratorHelper> iterhelper;
    CNativeBreakIteratorHelper::AcquireSingleton((INativeBreakIteratorHelper **)&iterhelper);
    iterhelper->GetWordInstance(where,(INativeBreakIterator **)&iter);
    AutoPtr<IBreakIterator> rbbi;
    FAIL_RETURN(CRuleBasedBreakIterator::New(iter,(IBreakIterator **)&rbbi));
    *instance = rbbi;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode BreakIterator::IsBoundary(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* isBoundary)
{
    return mWrapped->IsBoundary(offset,isBoundary);
}

ECode BreakIterator::GetPreceding(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* position)
{
    return mWrapped->GetPreceding(offset,position);
}

ECode BreakIterator::SetText(
    /* [in] */ const String& newText)
{
    if (newText.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return mWrapped->SetText(newText);
}

} // namespace Text
} // namespace Elastos
