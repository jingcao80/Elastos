
#include "ASN1StringType.h"
#include "core/CString.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"
#include "io/charset/CCharsets.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::CByte;
using Elastos::IO::Charset::ICharsets;
using Elastos::IO::Charset::CCharsets;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

//------------------------------------------------------
//  ASN1StringUTF8Type
//------------------------------------------------------
class ASN1StringUTF8Type : public ASN1StringType
{
public:
    ASN1StringUTF8Type(
        /* [in] */ Int32 tagNumber);

    // @Override
    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* in,
        /* [out] */ IInterface** object);

    // @Override
    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* out);
};

ASN1StringUTF8Type::ASN1StringUTF8Type(
    /* [in] */ Int32 tagNumber)
    : ASN1StringType(tagNumber)
{}

ECode ASN1StringUTF8Type::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** object)
{
    AutoPtr< ArrayOf<Byte> > buffer;
    in->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 offset;
    in->GetContentOffset(&offset);
    Int32 length;
    in->GetLength(&length);
    String str((char*)buffer->GetPayload() + offset, length);
    AutoPtr<ICharSequence> strObj;
    CString::New(str, (ICharSequence**)&strObj);
    *object = strObj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode ASN1StringUTF8Type::SetEncodingContent(
    /* [in] */ IBerOutputStream* out)
{
    AutoPtr<IInterface> obj;
    out->GetContent((IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    Int32 size = str.GetByteLength();
    AutoPtr<IArrayOf> arrays;
    CArrayOf::New(EIID_IByte, size, (IArrayOf**)&arrays);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IByte> byteObj;
        CByte::New(*(Byte*)(str.string() + i), (IByte**)&byteObj);
        arrays->Set(i, byteObj);
    }
    out->SetContent(arrays);
    out->SetLength(size);
    return NOERROR;
}


//------------------------------------------------------
//  ASN1StringType
//------------------------------------------------------
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::BMPSTRING = new ASN1StringType(IASN1Constants::TAG_BMPSTRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::IA5STRING = new ASN1StringType(IASN1Constants::TAG_IA5STRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::GENERALSTRING = new ASN1StringType(IASN1Constants::TAG_GENERALSTRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::PRINTABLESTRING = new ASN1StringType(IASN1Constants::TAG_PRINTABLESTRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::TELETEXSTRING = new ASN1StringUTF8Type(IASN1Constants::TAG_TELETEXSTRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::UNIVERSALSTRING = new ASN1StringType(IASN1Constants::TAG_UNIVERSALSTRING);
INIT_PROI_3 const AutoPtr<IASN1StringType> ASN1StringType::UTF8STRING = new ASN1StringUTF8Type(IASN1Constants::TAG_UTF8STRING);

ASN1StringType::ASN1StringType(
    /* [in] */ Int32 tagNumber)
{
    constructor(tagNumber);
}

ECode ASN1StringType::constructor(
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(tagNumber);
}

CAR_INTERFACE_IMPL(ASN1StringType, ASN1Type, IASN1StringType)

ECode ASN1StringType::CheckTag(
    /* [in] */ Int32 identifier,
    /* [ou] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId == identifier || mConstrId == identifier;
    return NOERROR;
}

ECode ASN1StringType::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;
    FAIL_RETURN(bis->ReadString(this));

    Boolean isVerify;
    if (bis->GetVerify(&isVerify), isVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1StringType::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    /* To ensure we get the correct encoding on non-ASCII platforms, specify
           that we wish to convert from ASCII to the default platform encoding */
    AutoPtr< ArrayOf<Byte> > buffer;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 offset;
    bis->GetContentOffset(&offset);
    Int32 length;
    bis->GetLength(&length);
    AutoPtr<ICharsets> charsets;
    CCharsets::AcquireSingleton((ICharsets**)&charsets);
    AutoPtr< ArrayOf<Char32> > chars;
    charsets->IsoLatin1BytesToChars(*buffer, offset, length, (ArrayOf<Char32>**)&chars);
    String str(*chars);
    AutoPtr<ICharSequence> strObj;
    CString::New(str, (ICharSequence**)&strObj);
    *object = strObj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode ASN1StringType::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    bos->EncodeTag(mId);
    return EncodeContent(bos);
}

ECode ASN1StringType::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeString();
}

ECode ASN1StringType::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> obj;
    bos->GetContent((IInterface**)&obj);
    String str;
    ICharSequence::Probe(obj)->ToString(&str);
    Int32 size = str.GetByteLength();
    AutoPtr<IArrayOf> arrays;
    CArrayOf::New(EIID_IByte, size, (IArrayOf**)&arrays);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IByte> byteObj;
        CByte::New(*(Byte*)(str.string() + i), (IByte**)&byteObj);
        arrays->Set(i, byteObj);
    }
    bos->SetContent(arrays);
    bos->SetLength(size);
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
