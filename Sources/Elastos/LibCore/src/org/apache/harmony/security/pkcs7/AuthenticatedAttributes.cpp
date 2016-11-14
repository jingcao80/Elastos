
#include "AuthenticatedAttributes.h"
#include "x501/CAttributeTypeAndValue.h"

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValue;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<ArrayOf<Byte> > encoded;
    bis->GetEncoded((ArrayOf<Byte>**)&encoded);
    AutoPtr<IInterface> tmp;
    bis->GetContent((IInterface**)&tmp);
    AutoPtr<IList> lst = IList::Probe(tmp);
    *object = (IObject*)new AuthenticatedAttributes(encoded, lst);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

INIT_PROI_5 AutoPtr<ASN1SetOf> AuthenticatedAttributes::ASN1 = InitStatic();

AutoPtr<ASN1SetOf> AuthenticatedAttributes::InitStatic()
{
    AutoPtr<ASN1SetOf> ret = new ASN1SetOfDerived();
    ret->constructor(IASN1Type::Probe(CAttributeTypeAndValue::ASN1));
    return ret;
}

AuthenticatedAttributes::AuthenticatedAttributes(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [in] */ IList* authenticatedAttributes)
{
    mEncoding = encoding;
    mAuthenticatedAttributes = authenticatedAttributes;
}

ECode AuthenticatedAttributes::GetAttributes(
    /* [out] */ IList** attributes)
{
    VALIDATE_NOT_NULL(attributes)
    *attributes = mAuthenticatedAttributes;
    REFCOUNT_ADD(*attributes)
    return NOERROR;
}

ECode AuthenticatedAttributes::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

