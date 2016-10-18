
#include "org/apache/harmony/security/x509/CKeyUsage.h"
//#include "org/apache/harmony/security/asn1/CASN1NamedBitList.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

//using Org::Apache::Harmony::Security::Asn1::CASN1NamedBitList;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1BitString;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

AutoPtr<IASN1Type> CKeyUsage::initASN1()
{
    AutoPtr<IASN1BitString> str;
    assert(0);
    Logger::E("CKeyUsage", "TODO: need CASN1NamedBitList");
    //CASN1NamedBitList::New(9, (IASN1BitString**)&str);
    return IASN1Type::Probe(str);
}

AutoPtr<IASN1Type> CKeyUsage::ASN1;// = initASN1();

static AutoPtr<ArrayOf<String> > initUSAGES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(9);
    array->Set(0, String("digitalSignature"));
    array->Set(1, String("nonRepudiation"));
    array->Set(2, String("keyEncipherment"));
    array->Set(3, String("dataEncipherment"));
    array->Set(4, String("keyAgreement"));
    array->Set(5, String("keyCertSign"));
    array->Set(6, String("cRLSign"));
    array->Set(7, String("encipherOnly"));
    array->Set(8, String("decipherOnly"));
    return array;
}

AutoPtr<ArrayOf<String> > CKeyUsage::USAGES = initUSAGES();

CAR_OBJECT_IMPL(CKeyUsage)

CAR_INTERFACE_IMPL(CKeyUsage, ExtensionValue, IKeyUsage)

ECode CKeyUsage::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        AutoPtr<IArrayOf> arrayof = CoreUtils::Convert(mKeyUsage);
        ASN1->Encode(TO_IINTERFACE(arrayof), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CKeyUsage::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("KeyUsage [\n"));
    for (Int32 i = 0; i < mKeyUsage->GetLength(); i++) {
        if ((*mKeyUsage)[i]) {
            sb->Append(prefix);
            sb->Append(String("  "));
            sb->Append((*USAGES)[i]);
            sb->Append('\n');
        }
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CKeyUsage::GetKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean>** ppKeyUsage)
{
    VALIDATE_NOT_NULL(ppKeyUsage);

    *ppKeyUsage = mKeyUsage;
    REFCOUNT_ADD(*ppKeyUsage);
    return NOERROR;
}

ECode CKeyUsage::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj);

    Int32 length;
    arrayof->GetLength(&length);
    AutoPtr<ArrayOf<Boolean> > array = ArrayOf<Boolean>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        arrayof->Get(i, (IInterface**)&value);
        AutoPtr<IBoolean> b = IBoolean::Probe(value);
        Boolean num;
        b->GetValue(&num);
        (*array)[i] = num;
    }

    mKeyUsage = array;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org