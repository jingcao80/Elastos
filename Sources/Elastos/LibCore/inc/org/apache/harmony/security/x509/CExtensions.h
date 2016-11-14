
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__

#include "_Org_Apache_Harmony_Security_X509_CExtensions.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X509::IX509Extension;
using Elastos::Core::Object;
using Elastos::Core::IBoolean;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ICollection;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtensions)
    , public Object
    , public IExtensions
{
private:
    class MyASN1SequenceOf
        : public ASN1SequenceOf
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** values);

        TO_STRING_IMPL("CExtensions::MyASN1SequenceOf")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSize(
        /* [out] */ Int32* pSize);

    CARAPI GetCriticalExtensions(
        /* [out] */ ISet** ppExtensions);

    CARAPI GetNonCriticalExtensions(
        /* [out] */ ISet** ppExtensions);

    CARAPI HasUnsupportedCritical(
        /* [out] */ Boolean* pHasUnsupportedCritical);

    CARAPI GetExtensionByOID(
        /* [in] */ const String& oid,
        /* [out] */ IX509Extension** ppExtensionByOID);

    CARAPI ValueOfKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean>** ppValueOfKeyUsage);

    CARAPI ValueOfExtendedKeyUsage(
        /* [out] */ IList** ppValueOfExtendedKeyUsage);

    CARAPI ValueOfBasicConstrains(
        /* [out] */ Int32* pValueOfBasicConstrains);

    CARAPI ValueOfSubjectAlternativeName(
        /* [out] */ ICollection** ppValueOfSubjectAlternativeName);

    CARAPI ValueOfIssuerAlternativeName(
        /* [out] */ ICollection** ppValueOfIssuerAlternativeName);

    CARAPI ValueOfCertificateIssuerExtension(
        /* [out] */ IX500Principal** ppValueOfCertificateIssuerExtension);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pAi,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pResult);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* pExtensions);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* asn1);

private:
    //
    // Makes the separated lists with oids of critical
    // and non-critical extensions
    //
    CARAPI MakeOidsLists();

    /**
     * Given an X.509 extension that encodes GeneralNames, return it in the
     * format expected by APIs.
     */
    static CARAPI DecodeGeneralNames(
        /* [in] */ IX509Extension* extension,
        /* [out] */ ICollection** names);

    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * Custom X.509 Extensions decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    // Supported critical extensions oids:
    static AutoPtr<IList> SUPPORTED_CRITICAL;

    // the values of extensions of the structure
    AutoPtr<IList> mExtensions;

    // to speed up access, the following fields cache values computed
    // from the extensions field, initialized using the "single-check
    // idiom".

    /*volatile*/ AutoPtr<ISet> mCritical;
    /*volatile*/ AutoPtr<ISet> mNoncritical;
    // the flag showing is there any unsupported critical extension
    // in the list of extensions or not.
    /*volatile*/ AutoPtr<IBoolean> mHasUnsupported;

    // map containing the oid of extensions as a keys and
    // Extension objects as values
    /*volatile*/ AutoPtr<IHashMap> mOidMap;

    // the ASN.1 encoded form of Extensions
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
