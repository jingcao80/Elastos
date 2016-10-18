
#include "org/apache/harmony/security/x509/COtherName.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Any.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/CASN1Explicit.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::CASN1Any;
using Org::Apache::Harmony::Security::Asn1::IASN1Any;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode COtherName::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    AutoPtr<IArrayOf> arrayof0 = IArrayOf::Probe(obj0);
    Int32 length0;
    arrayof0->GetLength(&length0);
    AutoPtr<ArrayOf<Int32> > array0 = ArrayOf<Int32>::Alloc(length0);
    for (Int32 i = 0; i < length0; i++) {
        AutoPtr<IInterface> value;
        arrayof0->Get(i, (IInterface**)&value);
        AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
        Int32 num;
        intvalue->GetValue(&num);
        (*array0)[i] = num;
    }
    String str;
    CObjectIdentifier::ToString(array0, &str);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);
    AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(obj1);
    Int32 length1;
    arrayof1->GetLength(&length1);
    AutoPtr<ArrayOf<Byte> > array1 = ArrayOf<Byte>::Alloc(length1);
    for (Int32 i = 0; i < length1; i++) {
        AutoPtr<IInterface> value;
        arrayof1->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array1)[i] = num;
    }

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<IOtherName> oname;
    COtherName::New(str, array1, array, (IOtherName**)&oname);
    *object = TO_IINTERFACE(oname);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode COtherName::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    COtherName* on = (COtherName*)IOtherName::Probe(object);

    AutoPtr<ArrayOf<Int32> > array;
    CObjectIdentifier::ToIntArray(on->mTypeID, (ArrayOf<Int32>**)&array);
    AutoPtr<IArrayOf> arrayof = CoreUtils::Convert(array);
    values->Set(0, TO_IINTERFACE(arrayof));

    AutoPtr<IArrayOf> arrayof1 = CoreUtils::ConvertByteArray(on->mValue);
    values->Set(1, TO_IINTERFACE(arrayof1));
    return NOERROR;
}

AutoPtr<IASN1Sequence> COtherName::initASN1()
{
    AutoPtr<IASN1Type> instance1;
    ASN1Oid::GetInstance((IASN1Type**)&instance1);

    AutoPtr<IASN1Any> any;
    CASN1Any::GetInstance((IASN1Any**)&any);
    AutoPtr<IASN1Explicit> instance2;
    CASN1Explicit::New(0, IASN1Type::Probe(any), (IASN1Explicit**)&instance2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, instance1);
    array->Set(1, IASN1Type::Probe(instance2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_4 AutoPtr<IASN1Sequence> COtherName::ASN1 = initASN1();

CAR_OBJECT_IMPL(COtherName)

CAR_INTERFACE_IMPL(COtherName, Object, IOtherName)

ECode COtherName::GetValue(
    /* [out, callee] */ ArrayOf<Byte>** ppValue)
{
    VALIDATE_NOT_NULL(ppValue);

    *ppValue = mValue;
    REFCOUNT_ADD(*ppValue);
    return NOERROR;
}

ECode COtherName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

COtherName::COtherName()
    : mTypeID(NULL)
{
}

ECode COtherName::constructor(
    /* [in] */ const String& typeID,
    /* [in] */ ArrayOf<Byte>* value)
{
    return constructor(typeID, value, NULL);
}

ECode COtherName::constructor(
    /* [in] */ const String& typeID,
    /* [in] */ ArrayOf<Byte>* value,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mTypeID = typeID;
    mValue = value;
    mEncoding = encoding;
    return NOERROR;
}

ECode COtherName::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode COtherName::SetASN1(
    /* [in] */ IASN1Sequence* ppAsn1)
{
    ASN1 = ppAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org