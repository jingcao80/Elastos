
#include "ASN1Oid.h"
#include <cmdef.h>
#include <elastos/StringBuilder.h>
#ifndef FOR_DERIVED
#define FOR_DERIVED
#include <Asn1TypeMacro.h>

using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class DerivedASN1Oid
    : public ASN1Oid
    , public IASN1Type
    , public ElRefBase
{
public:
    ASN1TYPE_METHODS_DECL()
    CAR_INTERFACE_DECL()
};

ASN1TYPE_METHODS_IMPL(ASN1Oid::DerivedASN1Oid, ASN1Oid)
CAR_INTERFACE_IMPL(ASN1Oid::DerivedASN1Oid, IASN1Type)

ECode ASN1Oid::DerivedASN1Oid::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    StringBuilder buf;

    //Special handling for the first packed OID element
    AutoPtr<ArrayOf<Byte> > buffer;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 contentOffset;
    bis->GetContentOffset(&contentOffset);
    Int32 octet = (*buffer)[contentOffset];
    Int32 element = octet & 0x7F;

    Int32 index = 0;
    while ((octet & 0x80) != 0) {
        index++;
        octet = (*buffer)[contentOffset + index];
        element = element << 7 | (octet & 0x7F);
    }

    if (element > 79) {
        buf.AppendChar('2');
        buf.AppendChar('.');
        buf.AppendInt32(element - 80);
    } else {
        buf.AppendInt32(element / 40);
        buf.AppendChar('.');
        buf.AppendInt32(element % 40);
    }

    // the rest of subidentifiers
    Int32 oidElement;
    bis->GetOidElement(&oidElement);
    for (Int32 j = 2; j < oidElement; j++) {
        buf.AppendChar('.');

        index++;
        octet = (*buffer)[contentOffset + index];
        element = octet & 0x7F;

        while ((octet & 0x80) != 0) {
            index++;
            octet = in.buffer[in.contentOffset + index];
            element = element << 7 | (octet & 0x7f);
        }

        buf.AppendInt32(element);
    }

    String str;
    buf.ToString(&str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    *object = cs.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ASN1Oid::DerivedASN1Oid::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    //FIXME this is a stub for a while
    AutoPtr<IObjectIdentifierHelper> hlp;
    CObjectIdentifierHelper::AcquireSingleton((IObjectIdentifierHelper**)&hlp);
    AutoPtr<IInterface> content;
    String str;
    AutoPtr<ArrayOf<Int32> > arr;
    bos->GetContent((IInterface**)&content);
    ICharSequence::Probe(content)->ToString(&str);
    hlp->ToIntArray(str, (ArrayOf<Int32>**)&arr);
    AutoPtr<IArrayOf> arrayOf;
    CArrayOf::New(EIID_IInteger32, oid->GetLength(), (IArrayOf**)&arrayOf);
    for (Int32 i = 0; i < oid->GetLength(); i++) {
        AutoPtr<IInteger32> i32;
        CInteger32::New((*oid)[i], (IInteger32**)&i32);
        arrayOf->Set(i, i32.Get());
    }
    *object = arrayOf.Get();
    REFCOUNT_ADD(*object)
    bos->SetContent(arrayOf.Get());
    return ASN1Oid::SetEncodingContent(bos);
}

AutoPtr<IASN1Type> STRING_OID;
AutoPtr<IASN1Type> ASN1Oid::ASN1 = InitStatic();

AutoPtr<IASN1Type> CASN1Oid::InitStatic()
{
    STRING_OID = new DerivedASN1Oid();
    return new CASN1Oid();
}

ECode CASN1Oid::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode ASN1Oid::GetInstanceForString(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = STRING_OID;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode ASN1Oid::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadOID();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1Oid::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    // Allocate and decode
    Int32 oidElement;
    bis->GetOidElement(&oidElement);
    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(oidElement);
    AutoPtr<ArrayOf<Byte> > buffer;
    Int32 contentOffset;
    bis->GetContentOffset(&contentOffset);
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    for (Int32 id = 1, i = 0; id < oid->GetLength(); id++, i++) {
        Int32 octet = (*buffer)[contentOffset + i];
        oidElement = octet & 0x7F;
        while ((octet & 0x80) != 0) {
            i++;
            octet = (*buffer)[contentOffset + i];
            oidElement = oidElement << 7 | (octet & 0x7f);
        }
        (*oid)[id] = oidElement;
    }
    // Special handling for the first packed OID element
    if ((*oid)[1] > 79) {
        (*oid)[0] = 2;
        (*oid)[1] = (*oid)[1] - 80;
    } else {
        (*oid)[0] = (*oid)[1] / 40;
        (*oid)[1] = (*oid)[1] % 40;
    }

    AutoPtr<IArrayOf> arrayOf;
    CArrayOf::New(EIID_IInteger32, oid->GetLength(), (IArrayOf**)&arrayOf);
    for (Int32 i = 0; i < oid->GetLength(); i++) {
        AutoPtr<IInteger32> i32;
        CInteger32::New((*oid)[i], (IInteger32**)&i32);
        arrayOf->Set(i, i32.Get());
    }
    *object = arrayOf.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ASN1Oid::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeOID();
}

ECode ASN1Oid::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> tmp;
    bos->GetContent((IInterface**)&tmp);
    AutoPtr<IArrayOf> arr = IArrayOf::Probe(tmp);
    Int32 len;
    arr->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(len);
    for (Int32 i =0; i < len; i++) {
        AutoPtr<IInterface> elem;
        arr->Get(i, (IInterface**)&elem);
        Int32 elemInt32;
        IInteger32::Probe(elem)->GetValue(&elemInt32);
        (*oid)[i] = elemInt32;
    }

    Int32 length = 0;

    // first subidentifier
    Int32 elem = (*oid)[0] * 40 + (*oid)[1];
    if (elem == 0) {
        length = 1;
    } else {
        for (; elem > 0; elem = elem >> 7) {
            length++;
        }
    }

    // the rest of subidentifiers
    for (Int32 i = 2; i < oid->GetLength(); i++) {
        if ((*oid)[i] == 0) {
            length++;
            continue;
        }
        for (elem = (*oid)[i]; elem > 0; elem = elem >> 7) {
            length++;
        }
    }
    return bos->SetLength(length);
}

ECode ASN1Oid::Init()
{
    return ASN1Primitive::Init(IASN1Constants::TAG_OID);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#undef FOR_DERIVED
#endif
