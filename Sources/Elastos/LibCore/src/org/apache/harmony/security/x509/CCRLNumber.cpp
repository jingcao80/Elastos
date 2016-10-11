
#include "org/apache/harmony/security/x509/CCRLNumber.h"
#include "org/apache/harmony/security/asn1/CASN1Integer.h"
#include "elastos/core/Math.h"
//#include "elastos/math/CBigInteger.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::INumber;
using Elastos::Core::IBoolean;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Math::IBigInteger;
//using Elastos::Math::CBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

AutoPtr<IASN1Type> CCRLNumber::ASN1 = IASN1Type::Probe(CASN1Integer::GetInstance());

CAR_OBJECT_IMPL(CCRLNumber)

CAR_INTERFACE_IMPL(CCRLNumber, ExtensionValue, ICRLNumber)

ECode CCRLNumber::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        AutoPtr<ArrayOf<Byte> > array;
        mNumber->ToByteArray((ArrayOf<Byte>**)&array);
        AutoPtr<IArrayOf> arrayof = CoreUtils::Convert(array);
        ASN1->Encode(arrayof, (ArrayOf<Byte>**)&mEncoding);

    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CCRLNumber::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("CRL Number: [ "));
    sb->Append(mNumber);
    sb->Append(String(" ]\n"));
    return NOERROR;
}

ECode CCRLNumber::GetNumber(
    /* [out] */ IBigInteger** ppNumber)
{
    VALIDATE_NOT_NULL(ppNumber);

    *ppNumber = mNumber;
    REFCOUNT_ADD(*ppNumber);
    return NOERROR;
}

ECode CCRLNumber::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(obj);

    Int32 length;
    values->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        values->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array)[i] = num;
    }

    assert(0);
    //CBigInteger::New(array, (IBigInteger**)&mNumber);
    return NOERROR;
}

ECode CCRLNumber::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCRLNumber::SetASN1(
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