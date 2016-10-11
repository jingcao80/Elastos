
#include "CASN1Integer.h"
#include "CBerInputStream.h"
#include "CArrayOf.h"
#include "CBigInteger.h"
#include "CByte.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::INumber;
using Elastos::Math::CBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Integer> CASN1Integer::ASN1 = InitStatic();

CAR_OBJECT_IMPL(CASN1Integer)
AutoPtr<IASN1Integer> CASN1Integer::InitStatic()
{
    AutoPtr<IASN1Integer> ai;
    CASN1Integer::New((IASN1Integer**)&ai);
    return ai;
}

ECode CASN1Integer::GetInstance(
    /* [out] */ IASN1Integer** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

AutoPtr<IASN1Integer> CASN1Integer::GetInstance()
{
    AutoPtr<IASN1Integer> tmp;
    GetInstance((IASN1Integer**)&tmp);
    return tmp;
}

ECode CASN1Integer::ToIntValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ Int32* intValue)
{
    Int32 size;
    IArrayOf::Probe(decoded)->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > dec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0 ; i < size; i++) {
        AutoPtr<IInterface> bt;
        Byte elem;
        IArrayOf::Probe(decoded)->Get(i, (IInterface**)&bt);
        IByte::Probe(bt)->GetValue(&elem);
        (*dec)[i] = elem;
    }
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(*dec, (IBigInteger**)&bi);
    return INumber::Probe(bi)->Int32Value(intValue);
}

ECode CASN1Integer::ToBigIntegerValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ IBigInteger** bigInteger)
{
    Int32 size;
    IArrayOf::Probe(decoded)->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > dec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0 ; i < size; i++) {
        AutoPtr<IInterface> bt;
        Byte elem;
        IArrayOf::Probe(decoded)->Get(i, (IInterface**)&bt);
        IByte::Probe(bt)->GetValue(&elem);
        (*dec)[i] = elem;
    }
    return CBigInteger::New(*dec, bigInteger);
}

ECode CASN1Integer::FromIntValue(
    /* [in] */ Int32 value,
    /* [out] */ IInterface** fromIntValue)
{
    VALIDATE_NOT_NULL(fromIntValue)
    AutoPtr<IBigInteger> bi;
    CBigInteger::ValueOf(value, (IBigInteger**)&bi);
    AutoPtr<ArrayOf<Byte> > bytesArray;
    bi->ToByteArray((ArrayOf<Byte>**)&bytesArray);
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, bytesArray->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < bytesArray->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*bytesArray)[i], (IByte**)&bt);
        arr->Set(i, bt.Get());
    }
    *fromIntValue = arr.Get();
    REFCOUNT_ADD(*fromIntValue)
    return NOERROR;
}

ECode CASN1Integer::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadInteger();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Integer::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 length, contentOffset;
    bis->GetLength(&length);
    bis->GetContentOffset(&contentOffset);
    AutoPtr<ArrayOf<Byte> > buffer, bytesEncoded = ArrayOf<Byte>::Alloc(length);
    bytesEncoded->Copy(0, buffer, contentOffset, length);
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, length, (IArrayOf**)&arr);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IByte> bt;
        CByte::New((*bytesEncoded)[i], (IByte**)&bt);
        arr->Set(i, bt.Get());
    }
    *object = arr.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CASN1Integer::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeInteger();
}

ECode CASN1Integer::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    Int32 size;
    IArrayOf::Probe(content)->GetLength(&size);
    return bos->SetLength(size);
}

ECode CASN1Integer::constructor()
{
    return ASN1Primitive::constructor(IASN1Constants::TAG_INTEGER);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

