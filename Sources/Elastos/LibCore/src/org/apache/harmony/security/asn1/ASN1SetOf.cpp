
#include "ASN1SetOf.h"
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(ASN1SetOf, ASN1ValueCollection, IASN1SetOf)
ECode ASN1SetOf::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** ret)
{
    bis->ReadSetOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, ret);
}

ECode ASN1SetOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSetOf(this);
}

ECode ASN1SetOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSetOfLength(this);
}

ECode ASN1SetOf::constructor(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::constructor(IASN1Constants::TAG_SETOF, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

