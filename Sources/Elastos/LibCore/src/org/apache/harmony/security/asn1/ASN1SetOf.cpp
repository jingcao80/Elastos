
#include "ASN1SetOf.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode ASN1SetOf::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** ret)
{
    bis->ReadSetOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
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

ECode ASN1SetOf::Init(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::Init(IASN1Constants::TAG_SETOF, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

