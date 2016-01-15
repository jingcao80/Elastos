
#include "CRuleBasedCollatorICU.h"
#include "NativeCollation.h"
#include "CCollationKeyICU.h"
#include "CollationElementIteratorICU.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_ICloneable;

namespace Libcore {
namespace ICU {

CAR_OBJECT_IMPL(CRuleBasedCollatorICU)

CAR_INTERFACE_IMPL_2(CRuleBasedCollatorICU, Object, IRuleBasedCollatorICU, ICloneable)

CRuleBasedCollatorICU::CRuleBasedCollatorICU()
{}

CRuleBasedCollatorICU::CRuleBasedCollatorICU(
    /* [in] */ Int64 address)
    : mAddress(address)
{
}

CRuleBasedCollatorICU::~CRuleBasedCollatorICU()
{
    NativeCollation::CloseCollator(mAddress);
}

ECode CRuleBasedCollatorICU::constructor(
    /* [in] */ const String& rules)
{
    if (rules.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NativeCollation::OpenCollatorFromRules(rules, VALUE_OFF, VALUE_DEFAULT_STRENGTH,&mAddress);
}

ECode CRuleBasedCollatorICU::constructor(
    /* [in] */ ILocale * locale)
{
    return NativeCollation::OpenCollator(locale, &mAddress);
}

ECode CRuleBasedCollatorICU::Clone(
    /* [out] */ IInterface ** outruleicu)
{
    VALIDATE_NOT_NULL(outruleicu)

    AutoPtr<IRuleBasedCollatorICU> ans;
    Int64 value(0);
    FAIL_RETURN(NativeCollation::SafeClone(mAddress, &value));
    ans =  new CRuleBasedCollatorICU(value);
    *outruleicu = ans;
    REFCOUNT_ADD(*outruleicu);

    return NOERROR;
}

ECode CRuleBasedCollatorICU::Compare(
    /* [in] */ const String& source,
    /* [in] */ const String& target,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = NativeCollation::Compare(mAddress, source, target);
    return NOERROR;
}

ECode CRuleBasedCollatorICU::GetDecomposition(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    return NativeCollation::GetAttribute(mAddress, DECOMPOSITION_MODE,value);
}

ECode CRuleBasedCollatorICU::SetDecomposition(
    /* [in] */ Int32 mode)
{
    return NativeCollation::SetAttribute(mAddress, DECOMPOSITION_MODE, mode);
}

ECode CRuleBasedCollatorICU::GetStrength(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    return NativeCollation::GetAttribute(mAddress, STRENGTH,value);
}

ECode CRuleBasedCollatorICU::SetStrength(
    /* [in] */ Int32 strength)
{
    return NativeCollation::SetAttribute(mAddress, STRENGTH, strength);
}

ECode CRuleBasedCollatorICU::SetAttribute(
    /* [in] */ Int32 type,
    /* [in] */ Int32 value)
{
    return NativeCollation::SetAttribute(mAddress, type, value);;
}

ECode CRuleBasedCollatorICU::GetAttribute(
    /* [in] */ Int32 type,
    /* [out] */ Int32 * value)
{
    return NativeCollation::GetAttribute(mAddress, type,value);
}

ECode CRuleBasedCollatorICU::GetCollationKey(
    /* [in] */ const String& source,
    /* [out] */ ICollationKey ** outkey)
{
    VALIDATE_NOT_NULL(outkey)

    if (source.IsNull()) {
        *outkey = NULL;
    } else {
        AutoPtr<ArrayOf<Byte> > key = NativeCollation::GetSortKey(mAddress, source);
        if (!(key->GetPayload())) {
            *outkey = NULL;
        } else {
            AutoPtr<ICollationKeyICU> ans;
            FAIL_RETURN(CCollationKeyICU::New(source, key ,(ICollationKeyICU **)&ans));
            *outkey = ICollationKey::Probe(ans);
            REFCOUNT_ADD(*outkey);
        }
    }
    return NOERROR;
}

ECode CRuleBasedCollatorICU::GetRules(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    *str = NativeCollation::GetRules(mAddress);
    return NOERROR;
}

ECode CRuleBasedCollatorICU::GetCollationElementIterator(
    /* [in] */ const String& source,
    /* [out] */ ICollationElementIteratorICU ** outicu)
{
    VALIDATE_NOT_NULL(outicu)

    AutoPtr<ICollationElementIteratorICU> ans = CollationElementIteratorICU::GetInstance(mAddress, source);
    *outicu = ans;
    REFCOUNT_ADD(*outicu);

    return NOERROR;
}

ECode CRuleBasedCollatorICU::GetCollationElementIterator(
    /* [in] */ ICharacterIterator * it,
    /* [out] */ ICollationElementIteratorICU ** outicu)
{
    VALIDATE_NOT_NULL(outicu)

    // We only implement the String-based API, so build a string from the iterator.
    return GetCollationElementIterator(CharacterIteratorToString(it),outicu);
}

String CRuleBasedCollatorICU::CharacterIteratorToString(
    /* [in] */ ICharacterIterator * it)
{
    StringBuilder result;
    Char32 ch(0);
    for (it->GetCurrent(&ch); ch != ICharacterIterator::DONE; it->GetNext(&ch)) {
        result.AppendChar(ch);
    }
    return result.ToString();
}

ECode CRuleBasedCollatorICU::GetHashCode(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = 42; // No-one uses RuleBasedCollatorICU as a hash key.
    return NOERROR;
}

ECode CRuleBasedCollatorICU::Equals(
    /* [in] */ const String& source,
    /* [in] */ const String& target,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)

    Int32 outcom(0);
    Compare(source, target,&outcom);
    *value = (outcom == 0);
    return NOERROR;
}

// @Override
ECode CRuleBasedCollatorICU::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> res = (PInterface) this->Probe(EIID_IInterface);
    if (object == res) {
        *value = TRUE;
        return NOERROR;
    }
    AutoPtr<IRuleBasedCollatorICU> rhs = IRuleBasedCollatorICU::Probe(object);
    if (!rhs) {
        return NOERROR;
    }

    String str1, str2;
    GetRules(&str1);
    rhs->GetRules(&str2);
    Int32 intvalue1,intvalue2;
    GetStrength(&intvalue1);
    rhs->GetStrength(&intvalue2);
    Int32 decvalue1, decvalue2;
    GetDecomposition(&decvalue1);
    rhs->GetDecomposition(&decvalue2);

    *value = str1.Equals(str2) && intvalue1 == intvalue2 && decvalue1 == decvalue2;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
