
#include "CASN1Sequence.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1TYPE_METHODS_IMPL(CASN1Sequence, ASN1Sequence)

ECode CASN1Sequence::SetType(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::SetType(type);
}

ECode CASN1Sequence::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type*>** type)
{
    return ASN1Sequence::GetType(type);
}

ECode CASN1Sequence::SetOPTIONAL(
    /* [in] */ ArrayOf<Boolean>* optional)
{
    return ASN1Sequence::SetOPTIONAL(optional);
}

ECode CASN1Sequence::GetOPTIONAL(
    /* [out, callee] */ ArrayOf<Boolean>** optional)
{
    return ASN1Sequence::GetOPTIONAL(optional);
}

ECode CASN1Sequence::SetDEFAULT(
    /* [in] */ ArrayOf<IInterface*>* def)
{
    return ASN1Sequence::SetDEFAULT(def);
}

ECode CASN1Sequence::GetDEFAULT(
    /* [out, callee] */ ArrayOf<IInterface*>** def)
{
    return ASN1Sequence::GetDEFAULT(def);
}

ECode CASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    return ASN1Sequence::GetValues(object, values);
}

ECode CASN1Sequence::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::Init(IASN1Constants::TAG_SEQUENCE, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

