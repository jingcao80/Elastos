
#include "org/apache/harmony/security/x509/CExtensions.h"
#include "org/apache/harmony/security/x509/CExtension.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "elastos/utility/CHashSet.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/Collections.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::X509::CExtension;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Collections;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CExtensions::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IList> values = IList::Probe(con);

    AutoPtr<IExtensions> extensions;
    CExtensions::New(values, (IExtensions**)&extensions);
    *object = extensions;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CExtensions::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    CExtensions* extensions = (CExtensions*)IExtensions::Probe(object);
    if (extensions->mExtensions == NULL) {
        CArrayList::New(values);
    }
    else {
        *values = ICollection::Probe(extensions->mExtensions);
        REFCOUNT_ADD(*values);
    }
    return NOERROR;
}

AutoPtr<IASN1Type> CExtensions::initASN1()
{
    AutoPtr<ASN1SequenceOf> tmp = new MyASN1SequenceOf();
    tmp->constructor(IASN1Type::Probe(CExtension::ASN1));
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CExtensions::ASN1 = initASN1();

static AutoPtr<IList> initSUPPORTED_CRITICAL()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(8);
    array->Set(0, String("2.5.29.15"));
    array->Set(1, String("2.5.29.19"));
    array->Set(2, String("2.5.29.32"));
    array->Set(3, String("2.5.29.17"));
    array->Set(4, String("2.5.29.30"));
    array->Set(5, String("2.5.29.36"));
    array->Set(6, String("2.5.29.37"));
    array->Set(7, String("2.5.29.54"));

    AutoPtr<IList> list;
    Arrays::AsList(array, (IList**)&list);
    return list;
}

AutoPtr<IList> CExtensions::SUPPORTED_CRITICAL = initSUPPORTED_CRITICAL();

CAR_OBJECT_IMPL(CExtensions)

CAR_INTERFACE_IMPL(CExtensions, Object, IExtensions)

ECode CExtensions::GetSize(
    /* [out] */ Int32* pSize)
{
    VALIDATE_NOT_NULL(pSize);

    if (mExtensions != NULL) {
        mExtensions->GetSize(pSize);
    }
    else {
        *pSize = 0;
    }
    return NOERROR;
}

ECode CExtensions::GetCriticalExtensions(
    /* [out] */ ISet** ppExtensions)
{
    VALIDATE_NOT_NULL(ppExtensions);

    AutoPtr<ISet> resultCritical = mCritical;
    if (resultCritical == NULL) {
        MakeOidsLists();
        resultCritical = mCritical;
    }
    *ppExtensions = resultCritical;
    REFCOUNT_ADD(*ppExtensions);
    return NOERROR;
}

ECode CExtensions::GetNonCriticalExtensions(
    /* [out] */ ISet** ppExtensions)
{
    VALIDATE_NOT_NULL(ppExtensions);

    AutoPtr<ISet> resultNoncritical = mNoncritical;
    if (resultNoncritical == NULL) {
        MakeOidsLists();
        resultNoncritical = mNoncritical;
    }
    *ppExtensions = resultNoncritical;
    REFCOUNT_ADD(*ppExtensions);
    return NOERROR;
}

ECode CExtensions::HasUnsupportedCritical(
    /* [out] */ Boolean* pHasUnsupportedCritical)
{
    VALIDATE_NOT_NULL(pHasUnsupportedCritical);

    AutoPtr<IBoolean> resultHasUnsupported = mHasUnsupported;
    if (resultHasUnsupported == NULL) {
        MakeOidsLists();
        resultHasUnsupported = mHasUnsupported;
    }
    Boolean res;
    resultHasUnsupported->GetValue(&res);
    *pHasUnsupportedCritical = res;
    return NOERROR;
}

ECode CExtensions::MakeOidsLists()
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    Int32 size ;
    mExtensions->GetSize(&size);
    AutoPtr<ISet> localCritical;
    CHashSet::New(size, (ISet**)&localCritical);
    AutoPtr<ISet> localNoncritical;
    CHashSet::New(size, (ISet**)&localNoncritical);
    AutoPtr<IBoolean> localHasUnsupported = CoreUtils::Convert(FALSE);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mExtensions->Get(i, (IInterface**)&obj);
        AutoPtr<IX509Extension> extension = IX509Extension::Probe(obj);

        String oid;
        Elastos::Security::Cert::IExtension::Probe(extension)->GetId(&oid);
        Boolean res;
        if (Elastos::Security::Cert::IExtension::Probe(extension)->IsCritical(&res), res) {
            if ((SUPPORTED_CRITICAL->Contains(TO_IINTERFACE(CoreUtils::Convert(oid)), &res), res)) {
                localHasUnsupported = CoreUtils::Convert(TRUE);
            }
            localCritical->Add(TO_IINTERFACE(CoreUtils::Convert(oid)));
        }
        else {
            localNoncritical->Add(TO_IINTERFACE(CoreUtils::Convert(oid)));
        }
    }

    mCritical = localCritical;
    mNoncritical = localNoncritical;
    mHasUnsupported = localHasUnsupported;
    return NOERROR;
}

ECode CExtensions::GetExtensionByOID(
    /* [in] */ const String& oid,
    /* [out] */ IX509Extension** ppExtensionByOID)
{
    VALIDATE_NOT_NULL(ppExtensionByOID);

    if (mExtensions == NULL) {
        *ppExtensionByOID = NULL;
        return NOERROR;
    }
    AutoPtr<IHashMap> localOidMap = mOidMap;
    if (localOidMap == NULL) {
        CHashMap::New((IHashMap**)&localOidMap);

        Int32 size ;
        mExtensions->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mExtensions->Get(i, (IInterface**)&obj);
            AutoPtr<IX509Extension> extension = IX509Extension::Probe(obj);

            String id;
            Elastos::Security::Cert::IExtension::Probe(extension)->GetId(&id);
            localOidMap->Put(TO_IINTERFACE(CoreUtils::Convert(id)), TO_IINTERFACE(extension));
        }
        mOidMap = localOidMap;
    }

    AutoPtr<IInterface> obj;
    localOidMap->Get(TO_IINTERFACE(CoreUtils::Convert(oid)), (IInterface**)&obj);
    *ppExtensionByOID = IX509Extension::Probe(obj);
    REFCOUNT_ADD(*ppExtensionByOID);
    return NOERROR;
}

ECode CExtensions::ValueOfKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean>** ppValueOfKeyUsage)
{
    VALIDATE_NOT_NULL(ppValueOfKeyUsage);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.15"), (IX509Extension**)&tmp);
    AutoPtr<IX509Extension> extension = IX509Extension::Probe(tmp);

    if (extension == NULL) {
        *ppValueOfKeyUsage = NULL;
        return NOERROR;
    }

    AutoPtr<IKeyUsage> uValue;
    extension->GetKeyUsageValue((IKeyUsage**)&uValue);
    AutoPtr<IKeyUsage> kUsage = uValue;
    if (kUsage == NULL) {
        *ppValueOfKeyUsage = NULL;
        return NOERROR;
    }

    return kUsage->GetKeyUsage((ArrayOf<Boolean>**)&ppValueOfKeyUsage);
}

ECode CExtensions::ValueOfExtendedKeyUsage(
    /* [out] */ IList** ppValueOfExtendedKeyUsage)
{
    VALIDATE_NOT_NULL(ppValueOfExtendedKeyUsage);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.37"), (IX509Extension**)&tmp);
    AutoPtr<IX509Extension> extension = IX509Extension::Probe(tmp);
    if (extension == NULL) {
        *ppValueOfExtendedKeyUsage = NULL;
        return NOERROR;
    }

    AutoPtr<IExtensionValue> eValue;
    extension->GetDecodedExtensionValue((IExtensionValue**)&eValue);
    return IExtendedKeyUsage::Probe(eValue)->GetExtendedKeyUsage(ppValueOfExtendedKeyUsage);
}

ECode CExtensions::ValueOfBasicConstrains(
    /* [out] */ Int32* pValueOfBasicConstrains)
{
    VALIDATE_NOT_NULL(pValueOfBasicConstrains);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.19"), (IX509Extension**)&tmp);
    AutoPtr<IX509Extension> extension = IX509Extension::Probe(tmp);

    if (extension == NULL) {
        *pValueOfBasicConstrains = -1;
        return NOERROR;
    }
    AutoPtr<IBasicConstraints> bc;
    extension->GetBasicConstraintsValue((IBasicConstraints**)&bc);
    Boolean res;
    if (bc == NULL || (bc->GetCa(&res), !res)) {
        *pValueOfBasicConstrains = -1;
        return NOERROR;
    }
    return bc->GetPathLenConstraint(pValueOfBasicConstrains);
}

ECode CExtensions::ValueOfSubjectAlternativeName(
    /* [out] */ ICollection** ppValueOfSubjectAlternativeName)
{
    VALIDATE_NOT_NULL(ppValueOfSubjectAlternativeName);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.17"), (IX509Extension**)&tmp);
    return DecodeGeneralNames(IX509Extension::Probe(tmp), ppValueOfSubjectAlternativeName);
}

ECode CExtensions::ValueOfIssuerAlternativeName(
    /* [out] */ ICollection** ppValueOfIssuerAlternativeName)
{
    VALIDATE_NOT_NULL(ppValueOfIssuerAlternativeName);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.18"), (IX509Extension**)&tmp);
    return DecodeGeneralNames(IX509Extension::Probe(tmp), ppValueOfIssuerAlternativeName);
}

ECode CExtensions::DecodeGeneralNames(
    /* [in] */ IX509Extension* extension,
    /* [out] */ ICollection** names)
{
    VALIDATE_NOT_NULL(names);

    if (extension == NULL) {
        *names = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > value;
    Elastos::Security::Cert::IExtension::Probe(extension)->GetValue((ArrayOf<Byte>**)&value);
    AutoPtr<IInterface> obj;
    CGeneralNames::ASN1->Decode(value, (IInterface**)&obj);
    AutoPtr<ICollection> collection;
    IGeneralNames::Probe(obj)->GetPairsList((ICollection**)&collection);

    /*
     * If the extension had any invalid entries, we may have an empty
     * collection at this point, so just return null.
     */
    Int32 size;
    collection->GetSize(&size);
    if (size == 0) {
        *names = NULL;
        return NOERROR;
    }

    return Collections::UnmodifiableCollection(collection, names);
}

ECode CExtensions::ValueOfCertificateIssuerExtension(
    /* [out] */ IX500Principal** ppValueOfCertificateIssuerExtension)
{
    VALIDATE_NOT_NULL(ppValueOfCertificateIssuerExtension);

    AutoPtr<IX509Extension> tmp;
    GetExtensionByOID(String("2.5.29.29"), (IX509Extension**)&tmp);
    AutoPtr<IX509Extension> extension = IX509Extension::Probe(tmp);

    if (extension == NULL) {
        *ppValueOfCertificateIssuerExtension = NULL;
        return NOERROR;
    }

    AutoPtr<IExtensionValue> eValue;
    extension->GetDecodedExtensionValue((IExtensionValue**)&eValue);
    return ICertificateIssuer::Probe(eValue)->GetIssuer(ppValueOfCertificateIssuerExtension);
}

ECode CExtensions::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CExtensions::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* pResult)
{
    VALIDATE_NOT_NULL(pResult);

    if (IExtensions::Probe(other) == NULL) {
        *pResult = FALSE;
        return NOERROR;
    }
    CExtensions* that = (CExtensions*)IExtensions::Probe(other);
    Boolean empty;
    if (mExtensions == NULL || (mExtensions->IsEmpty(&empty), empty)) {
        *pResult =  (that->mExtensions == NULL || (that->mExtensions->IsEmpty(&empty), empty));
    }
    else {
        mExtensions->Equals(that->mExtensions, pResult);
    }
    return NOERROR;
}

ECode CExtensions::GetHashCode(
    /* [out] */ Int32* pResult)
{
    VALIDATE_NOT_NULL(pResult);

    Int32 hashCode = 0;
    if (mExtensions != NULL) {
        mExtensions->GetHashCode(&hashCode);
    }
    *pResult = hashCode;
    return NOERROR;
}

ECode CExtensions::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    Int32 num = 1;
    Int32 size ;
    mExtensions->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mExtensions->Get(i, (IInterface**)&obj);
        AutoPtr<IX509Extension> extension = IX509Extension::Probe(obj);

        sb->Append('\n');
        sb->Append(prefix);
        sb->Append('[');
        sb->Append(num++);
        sb->Append(String("]: "));
        extension->DumpValue(sb, prefix);
    }

    return NOERROR;
}

ECode CExtensions::constructor()
{
    mExtensions = NULL;
    return NOERROR;
}

ECode CExtensions::constructor(
    /* [in] */ IList* extensions)
{
    mExtensions = extensions;
    return NOERROR;
}

ECode CExtensions::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CExtensions::SetASN1(
    /* [in] */ IASN1Type* asn1)
{
    ASN1 = asn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org