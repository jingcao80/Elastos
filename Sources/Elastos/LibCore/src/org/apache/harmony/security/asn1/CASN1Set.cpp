
#include "CASN1Set.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1Set::GetId(
    /* [out] */ Int32* id)
{
    return ASN1TypeCollection::GetId(id);
}

ECode CASN1Set::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1TypeCollection::GetConstrId(constrId);
}

ECode CASN1Set::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1TypeCollection::Decode(encoded, object);
}

ECode CASN1Set::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1TypeCollection::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Set::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1TypeCollection::DecodeEx2(is, object);
}

ECode CASN1Set::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1TypeCollection::Verify(encoded);
}

ECode CASN1Set::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1TypeCollection::VerifyEx(is);
}

ECode CASN1Set::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1TypeCollection::Encode(object, encode);
}

ECode CASN1Set::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSet(this);

    if(((CBerInputStream*)bis)->mIsVerify){
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Set::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1TypeCollection::CheckTag(identifier, checkTag);
}

ECode CASN1Set::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1TypeCollection::GetDecodedObject(bis, object);
}

ECode CASN1Set::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1TypeCollection::EncodeASN(bos);
}

ECode CASN1Set::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSet(this);
}

ECode CASN1Set::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSetLength(this);
}

ECode CASN1Set::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1TypeCollection::GetEncodedLength(bos, length);
}

ECode CASN1Set::ToString(
    /* [out] */ String* result)
{
    return ASN1TypeCollection::ToString(result);
}

ECode CASN1Set::SetType(
    /* [in] */ ArrayOf<IASN1Type*>* type)
{
    return ASN1TypeCollection::SetType(type);
}

ECode CASN1Set::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type*>** type)
{
    return ASN1TypeCollection::GetType(type);
}

ECode CASN1Set::SetOptional(
    /* [in] */ ArrayOf<Boolean>* optional)
{
    return ASN1TypeCollection::SetOptional(optional);
}

ECode CASN1Set::GetOptional(
    /* [out, callee] */ ArrayOf<Boolean>** optional)
{
    return ASN1TypeCollection::GetOptional(optional);
}

ECode CASN1Set::SetDefault(
    /* [in] */ ArrayOf<IInterface *>* def)
{
    return ASN1TypeCollection::SetDefault(def);
}

ECode CASN1Set::GetDefault(
    /* [out, callee] */ ArrayOf<IInterface*>** def)
{
    return ASN1TypeCollection::GetDefault(def);
}

ECode CASN1Set::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1TypeCollection::Init(type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

