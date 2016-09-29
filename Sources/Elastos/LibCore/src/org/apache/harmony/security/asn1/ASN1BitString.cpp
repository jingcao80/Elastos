
#include "ASN1BitString.h"
#include "CASN1BitString.h"
#include "CBitString.h"
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Type> ASN1BitString::sASN1 = Init();

CAR_INTERFACE_IMPL(ASN1BitString, ASN1StringType, IASN1BitString)
AutoPtr<IASN1Type> ASN1BitString::Init()
{
    AutoPtr<CASN1BitString> bs;
    CASN1BitString::NewByFriend((CASN1BitString**)&bs);
    return bs.Get();
}

ECode ASN1BitString::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode ASN1BitString::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadBitString();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1BitString::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 length, contentOffset;
    bis->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > buffer, bytes = ArrayOf<Byte>::Alloc(length - 1);
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    bis->GetContentOffset(&contentOffset);
    bytes->Copy(0, buffer, contentOffset + 1, length - 1);
    AutoPtr<IBitString> obj;
    CBitString::New(bytes, (*buffer)[contentOffset], (IBitString**)&obj);
    *object = obj.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ASN1BitString::EncodeContent(
    /* [in] */ IBerOutputStream *bos)
{
    return bos->EncodeBitString();
}

ECode ASN1BitString::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> tmp;
    bos->GetContent((IInterface**)&tmp);
    AutoPtr<IBitString> bs = IBitString::Probe(tmp);
    AutoPtr<ArrayOf<Byte> > bytes;
    bs->GetBytes((ArrayOf<Byte>**)&bytes);
    return bos->SetLength(bytes->GetLength() + 1);
}

ECode ASN1BitString::constructor()
{
    return ASN1StringType::constructor(IASN1Constants::TAG_BITSTRING);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

