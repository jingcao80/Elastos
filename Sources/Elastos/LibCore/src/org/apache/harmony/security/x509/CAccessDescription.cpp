
#include "org/apache/harmony/security/x509/CAccessDescription.h"
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
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

ECode CAccessDescription::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj1;
    values->Get(0, (IInterface**)&obj1);
    AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(obj1);
    Int32 length1;
    arrayof1->GetLength(&length1);
    AutoPtr<ArrayOf<Int32> > array1 = ArrayOf<Int32>::Alloc(length1);
    for (Int32 i = 0; i < length1; i++) {
        AutoPtr<IInterface> value;
        arrayof1->Get(i, (IInterface**)&value);
        AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
        Int32 num;
        intvalue->GetValue(&num);
        (*array1)[i] = num;
    }
    String str;
    CObjectIdentifier::ToString(array1, &str);

    AutoPtr<IInterface> obj2;
    values->Get(1, (IInterface**)&obj2);
    AutoPtr<IGeneralName> name = IGeneralName::Probe(obj2);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);

    AutoPtr<IAccessDescription> description;
    CAccessDescription::New((IAccessDescription**)&description);
    ((CAccessDescription*)description.Get())->constructor(str, name, array);
    *object = TO_IINTERFACE(description);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CAccessDescription::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<CAccessDescription> ad = (CAccessDescription*)IAccessDescription::Probe(object);

    AutoPtr<ArrayOf<Int32> > intArray;
    CObjectIdentifier::ToIntArray(ad->mAccessMethod, (ArrayOf<Int32>**)&intArray);
    AutoPtr<IArrayOf> array = CoreUtils::Convert(intArray);
    values->Set(0, TO_IINTERFACE(array));
    values->Set(1, TO_IINTERFACE(ad->mAccessLocation));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CAccessDescription::initASN1()
{
    AutoPtr<IASN1Type> instance;
    ASN1Oid::GetInstance((IASN1Type**)&instance);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, IASN1Type::Probe(instance));
    array->Set(1, IASN1Type::Probe(CGeneralName::ASN1));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> CAccessDescription::ASN1 = initASN1();

CAR_OBJECT_IMPL(CAccessDescription)

CAR_INTERFACE_IMPL(CAccessDescription, Object, IAccessDescription)

ECode CAccessDescription::constructor()
{
    return NOERROR;
}

ECode CAccessDescription::constructor(
    /* [in] */ const String& accessMethod,
    /* [in] */ IGeneralName* accessLocation,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mAccessMethod = accessMethod;
    mAccessLocation = accessLocation;
    mEncoding = encoding;
    return NOERROR;
}

ECode CAccessDescription::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        assert(0);
        //mEncoding = ASN1.encode(this);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CAccessDescription::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder res;
    res += "\n-- AccessDescription:";
    res += "\naccessMethod:  ";
    res += mAccessMethod;
    res += "\naccessLocation:  ";
    res += mAccessLocation;
    res += "\n-- AccessDescription END\n";
    return res.ToString(pStr);
}

ECode CAccessDescription::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CAccessDescription::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org