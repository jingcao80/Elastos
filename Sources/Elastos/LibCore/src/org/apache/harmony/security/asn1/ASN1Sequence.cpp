
#include "ASN1Sequence.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode ASN1Sequence::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* type)
{
    return ASN1TypeCollection::constructor(TAG_SEQUENCE, type);
}

CAR_INTERFACE_IMPL(ASN1Sequence, ASN1TypeCollection, IASN1Sequence)

ECode ASN1Sequence::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;

    FAIL_RETURN(bis->ReadSequence((IASN1Sequence*)this))

    Boolean isVerify;
    if (bis->GetVerify(&isVerify), isVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1Sequence::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSequence(this);
}

ECode ASN1Sequence::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSequenceLength(this);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
