
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include "org/apache/harmony/security/asn1/CASN1Any.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/utils/AlgNameMapper.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::IASN1Any;
using Org::Apache::Harmony::Security::Asn1::CASN1Any;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Utils::AlgNameMapper;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode AlgorithmIdentifier::MyASN1Sequence::GetDecodedObject(
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
    AutoPtr<IArrayOf> arrayof2 = IArrayOf::Probe(obj2);
    Int32 length2;
    arrayof2->GetLength(&length2);
    AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(length2);
    for (Int32 i = 0; i < length2; i++) {
        AutoPtr<IInterface> value;
        arrayof2->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array2)[i] = num;
    }

    AutoPtr<AlgorithmIdentifier> identifier = new AlgorithmIdentifier();
    identifier->constructor(str, array2);
    *object = TO_IINTERFACE(identifier);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode AlgorithmIdentifier::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IAlgorithmIdentifier> aID = IAlgorithmIdentifier::Probe(object);

    String algorithm;
    aID->GetAlgorithm(&algorithm);
    AutoPtr<ArrayOf<Int32> > intArray;
    CObjectIdentifier::ToIntArray(algorithm, (ArrayOf<Int32>**)&intArray);
    AutoPtr<IArrayOf> array = CoreUtils::Convert(intArray);
    values->Set(0, TO_IINTERFACE(array));

    AutoPtr<ArrayOf<Byte> > parameters;
    aID->GetParameters((ArrayOf<Byte>**)&parameters);
    AutoPtr<IArrayOf> array2 = CoreUtils::ConvertByteArray(parameters);
    values->Set(1, TO_IINTERFACE(array2));
    return NOERROR;
}

AutoPtr<IASN1Sequence> AlgorithmIdentifier::initASN1()
{
    AutoPtr<IASN1Type> instance1;
    ASN1Oid::GetInstance((IASN1Type**)&instance1);
    AutoPtr<IASN1Any> instance2;
    CASN1Any::GetInstance((IASN1Any**)&instance2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, instance1);
    array->Set(1, IASN1Type::Probe(instance2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(1); // parameters are optional
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_4 AutoPtr<IASN1Sequence> AlgorithmIdentifier::ASN1 = initASN1();

CAR_INTERFACE_IMPL(AlgorithmIdentifier, Object, IAlgorithmIdentifier)

ECode AlgorithmIdentifier::GetAlgorithm(
    /* [out] */ String* pAlgorithm)
{
    VALIDATE_NOT_NULL(pAlgorithm);

    *pAlgorithm = mAlgorithm;
    return NOERROR;
}

ECode AlgorithmIdentifier::GetAlgorithmName(
    /* [out] */ String* pAlgorithmName)
{
    VALIDATE_NOT_NULL(pAlgorithmName)

    if (mAlgorithmName.IsNull()) {
        mAlgorithmName = AlgNameMapper::Map2AlgName(mAlgorithm);
        if (mAlgorithmName.IsNull()) {
            mAlgorithmName = mAlgorithm;
        }
    }
    *pAlgorithmName = mAlgorithmName;
    return NOERROR;
}

ECode AlgorithmIdentifier::GetParameters(
    /* [out, callee] */ ArrayOf<Byte>** ppParameters)
{
    VALIDATE_NOT_NULL(ppParameters)

    *ppParameters = mParameters;
    REFCOUNT_ADD(*ppParameters);
    return NOERROR;
}

ECode AlgorithmIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded)

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode AlgorithmIdentifier::Equals(
    /* [in] */ IInterface* ai,
    /* [out] */ Boolean* pResult)
{
    VALIDATE_NOT_NULL(pResult)


    if (IAlgorithmIdentifier::Probe(ai) == NULL) {
        *pResult = FALSE;
        return NOERROR;
    }
    AlgorithmIdentifier* algid = (AlgorithmIdentifier*)IAlgorithmIdentifier::Probe(ai);
    *pResult = (mAlgorithm.Equals(algid->mAlgorithm))
        && ((mParameters == NULL)
                ? algid->mParameters == NULL
                : Arrays::Equals(mParameters, algid->mParameters));
    return NOERROR;
}

ECode AlgorithmIdentifier::GetHashCode(
    /* [out] */ Int32* pResult)
{
    VALIDATE_NOT_NULL(pResult)

    *pResult = mAlgorithm.GetHashCode() * 37 + (mParameters != NULL ? Arrays::GetHashCode(mParameters) : 0);
    return NOERROR;
}

ECode AlgorithmIdentifier::DumpValue(
    /* [in] */ IStringBuilder* sb)
{
    String name;
    GetAlgorithmName(&name);
    sb->Append(name);
    if (mParameters == NULL) {
        sb->Append(String(", no params, "));
    }
    else {
        sb->Append(String(", params unparsed, "));
    }
    sb->Append(String("OID = "));
    String algorithm;
    GetAlgorithm(&algorithm);
    sb->Append(algorithm);
    return NOERROR;
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm)
{
    return constructor(algorithm, NULL, NULL);
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* parameters)
{
    return constructor(algorithm, parameters, NULL);;
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* parameters,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mAlgorithm = algorithm;
    mParameters = parameters;
    mEncoding = encoding;
    return NOERROR;
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& algorithmName)
{
    constructor(algorithm, NULL, NULL);
    mAlgorithmName = algorithmName;
    return NOERROR;
}

ECode AlgorithmIdentifier::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode AlgorithmIdentifier::SetASN1(
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