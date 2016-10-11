
#include "org/apache/harmony/security/x509/CExtendedKeyUsage.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::X509::ExtensionValue;
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

ECode CExtendedKeyUsage::MyASN1Oid::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> obj;
    ASN1Oid::GetDecodedObject(bis, (IInterface**)&obj);
    AutoPtr<IArrayOf> oid = IArrayOf::Probe(obj);

    Int32 length;
    oid->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        oid->Get(i, (IInterface**)&value);
        AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
        Int32 num;
        intvalue->GetValue(&num);
        (*array)[i] = num;
    }
    String str;
    CObjectIdentifier::ToString(array, &str);
    *object = TO_IINTERFACE(CoreUtils::Convert(str));
    REFCOUNT_ADD(*object);
    return NOERROR;
}

AutoPtr<IASN1Type> CExtendedKeyUsage::initASN1()
{
    AutoPtr<ASN1Oid> tmp = new MyASN1Oid();
    AutoPtr<ASN1SequenceOf> type = new ASN1SequenceOf();
    type->constructor(IASN1Type::Probe(tmp));
    return IASN1Type::Probe(type);
}

AutoPtr<IASN1Type> CExtendedKeyUsage::ASN1 = initASN1();

CAR_OBJECT_IMPL(CExtendedKeyUsage)

CAR_INTERFACE_IMPL(CExtendedKeyUsage, ExtensionValue, IExtendedKeyUsage)

ECode CExtendedKeyUsage::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(mKeys), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CExtendedKeyUsage::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Extended Key Usage: "));
    if (mKeys == NULL) {
        //try {
        ECode ec = GetExtendedKeyUsage((IList**)&mKeys);
        //} catch (IOException e) {
        if (ec == (ECode)E_IO_EXCEPTION) {
            // incorrect extension value encoding
            ExtensionValue::DumpValue(sb);
            return NOERROR;
        }
    }
    sb->Append('[');
    Int32 size;
    mKeys->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mKeys->Get(i, (IInterface**)&obj);

        sb->Append(String(" \""));
        sb->Append(obj);
        sb->Append('"');
        if (i != size - 1) {
            sb->Append(',');
        }
    }
    sb->Append(String(" ]\n"));
    return NOERROR;
}

ECode CExtendedKeyUsage::GetExtendedKeyUsage(
    /* [out] */ IList** ppExtendedKeyUsage)
{
    VALIDATE_NOT_NULL(ppExtendedKeyUsage);

    if (mKeys == NULL) {
        AutoPtr<ArrayOf<Byte> > array;
        GetEncoded((ArrayOf<Byte>**)&array);
        AutoPtr<IInterface> obj;
        ASN1->Decode(array, (IInterface**)&obj);
        mKeys = IList::Probe(obj);
    }
    *ppExtendedKeyUsage = mKeys;
    REFCOUNT_ADD(*ppExtendedKeyUsage);
    return NOERROR;
}

ECode CExtendedKeyUsage::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    return ExtensionValue::constructor(encoding);
}

ECode CExtendedKeyUsage::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CExtendedKeyUsage::SetASN1(
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