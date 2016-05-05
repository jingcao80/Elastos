
#include "CASN1OctetString.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"
#include "utility/Arrays.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::CByte;
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

//TODO:
const AutoPtr<IASN1OctetString> CASN1OctetString::ASN1;// = InitASN1();

AutoPtr<IASN1OctetString> CASN1OctetString::InitASN1()
{
    AutoPtr<IASN1OctetString> asn1Obj;
    CASN1OctetString::New((IASN1OctetString**)&asn1Obj);
    return asn1Obj;
}

ECode CASN1OctetString::constructor()
{
    return ASN1StringType::constructor(IASN1Constants::TAG_OCTETSTRING);
}

ECode CASN1OctetString::GetInstance(
    /* [out] */ IASN1OctetString** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1OctetString::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;
    FAIL_RETURN(bis->ReadOctetString());

    Boolean isVerify;
    if (bis->GetVerify(&isVerify), isVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1OctetString::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr< ArrayOf<Byte> > buffer;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 offset;
    bis->GetContentOffset(&offset);
    Int32 length;
    bis->GetLength(&length);
    AutoPtr< ArrayOf<Byte> > result;
    Arrays::CopyOfRange(buffer, offset, offset + length, (ArrayOf<Byte>**)&result);
    Int32 size = result->GetLength();
    AutoPtr<IArrayOf> arrays;
    CArrayOf::New(EIID_IByte, size, (IArrayOf**)&arrays);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IByte> byteObj;
        CByte::New((*result)[i], (IByte**)&byteObj);
        arrays->Set(i, byteObj);
    }
    *object = arrays;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CASN1OctetString::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeOctetString();
}

ECode CASN1OctetString::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    Int32 length;
    IArrayOf::Probe(content)->GetLength(&length);
    return bos->SetLength(length);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
