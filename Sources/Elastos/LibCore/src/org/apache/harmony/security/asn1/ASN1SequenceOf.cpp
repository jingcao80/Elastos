
#include "ASN1SequenceOf.h"
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(ASN1SequenceOf, ASN1ValueCollection, IASN1SequenceOf);

ECode ASN1SequenceOf::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSequenceOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1SequenceOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSequenceOf(this);
}

ECode ASN1SequenceOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSequenceOfLength(this);
}

ECode ASN1SequenceOf::constructor(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::constructor(IASN1Constants::TAG_SEQUENCE, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

