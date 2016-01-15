#include "Collator.h"
#include "CRuleBasedCollator.h"
#include "CLocale.h"
#include "ICUUtil.h"
#include "CRuleBasedCollatorICU.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::CLocale;
using Libcore::ICU::ICUUtil;
using Libcore::ICU::CRuleBasedCollatorICU;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_3(Collator, Object, ICollator, IComparator, ICloneable)

ECode Collator::CloneImpl(
    /* [in] */ ICollator* object)
{
    assert(object);
    VALIDATE_NOT_NULL(object)

    Collator* ck = (Collator*)object;

    AutoPtr<IInterface> obj;
    ICloneable::Probe(mICUColl)->Clone((IInterface**)&obj);
    ck->mICUColl = IRuleBasedCollatorICU::Probe(obj);
    return NOERROR;
}

ECode Collator::constructor()
{

    AutoPtr<ILocale> locale = CLocale::GetDefault();
    CRuleBasedCollatorICU::New(locale, (IRuleBasedCollatorICU**)&mICUColl);
    return NOERROR;
}

ECode Collator::constructor(
    /* [in] */ IRuleBasedCollatorICU* icuColl)
{
    mICUColl = icuColl;
    return NOERROR;
}

ECode Collator::Compare(
    /* [in] */ IInterface* object1,
    /* [in] */ IInterface* object2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String s1 = Object::ToString(object1);
    String s2 = Object::ToString(object2);
    return Compare(s1, s2, result);
}

ECode Collator::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    ICollator* c = ICollator::Probe(object);
    if (c == NULL) {
        return NOERROR;
    }

    Collator* collator = (Collator*)c;
    *result = Object::Equals(mICUColl, collator->mICUColl);
    return NOERROR;
}

ECode Collator::Equals(
    /* [in] */ const String& string1,
    /* [in] */ const String& string2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 i;
    Compare(string1, string2, &i);
    *result = i == 0;
    return  NOERROR;
}

ECode Collator::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return ICUUtil::GetAvailableCollatorLocales(locales);
}

ECode Collator::GetDecomposition(
    /* [out] */ Int32* decomposition)
{
    VALIDATE_NOT_NULL(decomposition);
    Int32 i;
    mICUColl->GetDecomposition(&i);
    return DecompositionMode_ICU_Java(i, decomposition);
}

ECode Collator::GetInstance(
    /* [out] */ ICollator** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<ILocale> locale = CLocale::GetDefault();
    return GetInstance(locale, instance);
}

ECode Collator::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICollator** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    if (locale == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IRuleBasedCollatorICU> icuCollator;
    CRuleBasedCollatorICU::New(locale, (IRuleBasedCollatorICU**)&icuCollator);
    AutoPtr<IRuleBasedCollator> rbc;
    FAIL_RETURN(CRuleBasedCollator::New(icuCollator, (IRuleBasedCollator**)&rbc));
    *instance = (ICollator*)rbc->Probe(EIID_ICollator);
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode Collator::GetStrength(
    /* [out] */ Int32* strength)
{
    VALIDATE_NOT_NULL(strength);
    Int32 i;
    mICUColl->GetStrength(&i);
    return Strength_ICU_Java(i, strength);
}

ECode Collator::SetDecomposition(
    /* [in] */ Int32 value)
{
    Int32 i;
    FAIL_RETURN(DecompositionMode_Java_ICU(value, &i));
    return mICUColl->SetDecomposition(i);
}

ECode Collator::SetStrength(
    /* [in] */ Int32 value)
{
    Int32 i;
    FAIL_RETURN(Strength_Java_ICU(value, &i));
    return mICUColl->SetStrength(i);
}

ECode Collator::DecompositionMode_Java_ICU(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;
    switch (mode) {
        case ICollator::CANONICAL_DECOMPOSITION:
            *value = IRuleBasedCollatorICU::VALUE_ON;
            return NOERROR;
        case ICollator::NO_DECOMPOSITION:
            *value = IRuleBasedCollatorICU::VALUE_OFF;
            return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Collator::DecompositionMode_ICU_Java(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 javaMode = mode;
    switch (mode) {
        case IRuleBasedCollatorICU::VALUE_OFF:
            javaMode = ICollator::NO_DECOMPOSITION;
            break;
        case IRuleBasedCollatorICU::VALUE_ON:
            javaMode = ICollator::CANONICAL_DECOMPOSITION;
            break;
    }
    *value = javaMode;
    return NOERROR;
}

ECode Collator::Strength_Java_ICU(
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    switch (value) {
        case ICollator::PRIMARY:
            *result = IRuleBasedCollatorICU::VALUE_PRIMARY;
            return NOERROR;
        case ICollator::SECONDARY:
            *result = IRuleBasedCollatorICU::VALUE_SECONDARY;
            return NOERROR;
        case ICollator::TERTIARY:
            *result = IRuleBasedCollatorICU::VALUE_TERTIARY;
            return NOERROR;
        case ICollator::IDENTICAL:
            *result = IRuleBasedCollatorICU::VALUE_IDENTICAL;
            return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Collator::Strength_ICU_Java(
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    switch (value) {
        case IRuleBasedCollatorICU::VALUE_PRIMARY:
            *result = ICollator::PRIMARY;
            break;
        case IRuleBasedCollatorICU::VALUE_SECONDARY:
            *result = ICollator::SECONDARY;
            break;
        case IRuleBasedCollatorICU::VALUE_TERTIARY:
            *result = ICollator::TERTIARY;
            break;
        case IRuleBasedCollatorICU::VALUE_IDENTICAL:
            *result = ICollator::IDENTICAL;
            break;
    }
    *result = -1;
    return NOERROR;
}

ECode Collator::GetICUCollator(
    /* [out] */ IRuleBasedCollatorICU** icuCollator)
{
    VALIDATE_NOT_NULL(icuCollator);
    *icuCollator = mICUColl;
    REFCOUNT_ADD(*icuCollator);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
