
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifier.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
//#include "elastos/math/CBigInteger.h"
#include <elastos/utility/Arrays.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
//using Elastos::Math::CBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CAuthorityKeyIdentifier::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    AutoPtr<IArrayOf> arrayof2 = IArrayOf::Probe(obj2);
    Int32 length2;
    arrayof2->GetLength(&length2);
    AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(length2);
    for (Int32 i = 0; i < length2; i++) {
        AutoPtr<IInterface> value;
        arrayof2->Get(i, (IInterface**)&value);
        AutoPtr<IByte> bvalue = IByte::Probe(value);
        Byte b;
        bvalue->GetValue(&b);
        (*array2)[i] = b;
    }
    AutoPtr<IBigInteger> authorityCertSerialNumber;
    if (arrayof2 != NULL) {
        assert(0);
        //CBigInteger::New(array2, (IBigInteger**)&authorityCertSerialNumber);
    }

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    AutoPtr<IArrayOf> arrayof0 = IArrayOf::Probe(obj0);
    Int32 length0;
    arrayof0->GetLength(&length0);
    AutoPtr<ArrayOf<Byte> > array0 = ArrayOf<Byte>::Alloc(length0);
    for (Int32 i = 0; i < length0; i++) {
        AutoPtr<IInterface> value;
        arrayof0->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array0)[i] = num;
    }

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);

    AutoPtr<IAuthorityKeyIdentifier> identifier;
    CAuthorityKeyIdentifier::New(array0, IGeneralNames::Probe(obj1),
            authorityCertSerialNumber, (IAuthorityKeyIdentifier**)&identifier);
    *object = TO_IINTERFACE(identifier);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CAuthorityKeyIdentifier* akid = (CAuthorityKeyIdentifier*)IAuthorityKeyIdentifier::Probe(object);

    AutoPtr<IArrayOf> array = CoreUtils::ConvertByteArray(akid->mKeyIdentifier);
    values->Set(0, TO_IINTERFACE(array));
    values->Set(1, TO_IINTERFACE(akid->mAuthorityCertIssuer));

    if (akid->mAuthorityCertSerialNumber != NULL) {
        AutoPtr<ArrayOf<Byte> > parameters;
        akid->mAuthorityCertSerialNumber->ToByteArray((ArrayOf<Byte>**)&parameters);
        AutoPtr<IArrayOf> array2 = CoreUtils::ConvertByteArray(parameters);
        values->Set(2, TO_IINTERFACE(array2));
    }
    return NOERROR;
}

AutoPtr<IASN1Type> CAuthorityKeyIdentifier::initASN1()
{
    AutoPtr<IASN1OctetString> instance1;
    assert(0);
    //ASN1OctetString::GetInstance((IASN1OctetString**)&instance1);
    AutoPtr<IASN1Implicit> implicit1;
    CASN1Implicit::New(0, IASN1Type::Probe(instance1), (IASN1Implicit**)&implicit1);

    AutoPtr<IASN1Implicit> implicit2;
    assert(0);
    //CASN1Implicit::New(1, CGeneralNames::ASN1, (IASN1Implicit**)&implicit2);

    AutoPtr<IASN1Integer> instance2;
    assert(0);
    //CASN1IntegerHelper::GetInstance((IASN1Integer**)&instance2);
    AutoPtr<IASN1Implicit> implicit3;
    CASN1Implicit::New(2, IASN1Type::Probe(instance2), (IASN1Implicit**)&implicit3);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(implicit1));
    array->Set(1, IASN1Type::Probe(implicit2));
    array->Set(2, IASN1Type::Probe(implicit3));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(0);
    tmp->SetOptional(1);
    tmp->SetOptional(2);
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CAuthorityKeyIdentifier::ASN1;// = initASN1();

CAR_OBJECT_IMPL(CAuthorityKeyIdentifier)

CAR_INTERFACE_IMPL(CAuthorityKeyIdentifier, ExtensionValue, IAuthorityKeyIdentifier)

ECode CAuthorityKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);

    return NOERROR;
}

ECode CAuthorityKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("AuthorityKeyIdentifier [\n"));
    if (mKeyIdentifier != NULL) {
        sb->Append(prefix);
        sb->Append(String("  keyIdentifier:\n"));
        assert(0);
        //sb->Append(Array::toString(keyIdentifier, prefix + "    "));
    }
    if (mAuthorityCertIssuer != NULL) {
        sb->Append(prefix);
        sb->Append(String("  authorityCertIssuer: [\n"));
        mAuthorityCertIssuer->DumpValue(sb, prefix + String("    "));
        sb->Append(prefix);
        sb->Append(String("  ]\n"));
    }
    if (mAuthorityCertSerialNumber != NULL) {
        sb->Append(prefix);
        sb->Append(String("  authorityCertSerialNumber: "));
        sb->Append(String(TO_CSTR(mAuthorityCertSerialNumber)));
        sb->Append('\n');
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte>** ppKeyIdentifier)
{
    VALIDATE_NOT_NULL(ppKeyIdentifier);

    *ppKeyIdentifier = mKeyIdentifier;
    REFCOUNT_ADD(*ppKeyIdentifier);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertIssuer(
    /* [out] */ IGeneralNames** ppIssuer)
{
    VALIDATE_NOT_NULL(ppIssuer);

    *ppIssuer = mAuthorityCertIssuer;
    REFCOUNT_ADD(*ppIssuer);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertSerialNumber(
    /* [out] */ IBigInteger** ppNumber)
{
    VALIDATE_NOT_NULL(ppNumber);

    *ppNumber = mAuthorityCertSerialNumber;
    REFCOUNT_ADD(*ppNumber);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte>* keyIdentifier,
    /* [in] */ IGeneralNames* authorityCertIssuer,
    /* [in] */ IBigInteger* authorityCertSerialNumber)
{
    mKeyIdentifier = keyIdentifier;
    mAuthorityCertIssuer = authorityCertIssuer;
    mAuthorityCertSerialNumber = authorityCertSerialNumber;
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ IAuthorityKeyIdentifier** ppKeyIdentifier)
{
    VALIDATE_NOT_NULL(ppKeyIdentifier);

    AutoPtr<IInterface> obj;
    IASN1Type::Probe(ASN1)->Decode(encoding, (IInterface**)&obj);
    AutoPtr<IAuthorityKeyIdentifier> aki = IAuthorityKeyIdentifier::Probe(obj);
    ((CAuthorityKeyIdentifier*)(aki.Get()))->mEncoding = encoding;
    *ppKeyIdentifier = aki;
    REFCOUNT_ADD(*ppKeyIdentifier);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CAuthorityKeyIdentifier::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org