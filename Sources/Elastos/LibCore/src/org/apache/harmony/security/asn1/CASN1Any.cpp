
#include "CASN1Any.h"
#include "CBerInputStream.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

INIT_PROI_3 AutoPtr<IASN1Any> CASN1Any::sASN1 = Init();

CAR_OBJECT_IMPL(CASN1Any)
CAR_INTERFACE_IMPL(CASN1Any, ASN1Type, IASN1Any)

AutoPtr<IASN1Any> CASN1Any::Init()
{
    return new CASN1Any();
}

ECode CASN1Any::GetInstance(
    /* [out] */ IASN1Any** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Any::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Type::GetId(id);
}

ECode CASN1Any::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Type::GetConstrId(constrId);
}

ECode CASN1Any::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Type::Decode(encoded, object);
}

ECode CASN1Any::Decode(
    /* [in] */ ArrayOf<Byte> *encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface **object)
{
    return ASN1Type::Decode(encoded, offset, encodingLen, object);
}

ECode CASN1Any::Decode(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Type::Decode(is, object);
}

ECode CASN1Any::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Type::Verify(encoded);
}

ECode CASN1Any::Verify(
    /* [in] */ IInputStream* is)
{
    return ASN1Type::Verify(is);
}

ECode CASN1Any::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Type::Encode(object, encode);
}

ECode CASN1Any::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    // only read content, doesn't check it
    bis->ReadContent();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Any::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    VALIDATE_NOT_NULL(checkTag)
    *checkTag = TRUE; //all tags are OK
    return NOERROR;
}

ECode CASN1Any::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 offset, tagOffset;
    bis->GetOffset(&offset);
    bis->GetTagOffset(&tagOffset);
    AutoPtr<ArrayOf<Byte> > bytesEncoded = ArrayOf<Byte>::Alloc(offset- tagOffset);
    bytesEncoded->Copy(0, ((BerInputStream*)bis)->mBuffer, tagOffset, bytesEncoded->GetLength());
    AutoPtr<IArrayOf> temp = CoreUtils::ConvertByteArray(bytesEncoded);
    *object = temp;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CASN1Any::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeANY();
}

ECode CASN1Any::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeANY();
}

ECode CASN1Any::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    assert(IArrayOf::Probe(content));
    Int32 len = 0;
    IArrayOf::Probe(content)->GetLength(&len);
    return bos->SetLength(len);
}

ECode CASN1Any::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return bos->GetLength(length);
}

ECode CASN1Any::ToString(
    /* [out] */ String* result)
{
    return ASN1Type::ToString(result);
}

ECode CASN1Any::constructor()
{
    return ASN1Type::constructor(IASN1Constants::TAG_ANY);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

