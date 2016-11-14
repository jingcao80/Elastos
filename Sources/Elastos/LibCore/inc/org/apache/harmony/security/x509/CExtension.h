
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__

#include "_Org_Apache_Harmony_Security_X509_CExtension.h"
#include "org/apache/harmony/security/asn1/ASN1OctetString.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1OctetString;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IOutputStream;
using Elastos::Security::Cert::IExtension;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtension)
    , public Object
    , public IX509Extension
    , public IExtension
{
private:
    class MyASN1OctetString
        : public ASN1OctetString
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        TO_STRING_IMPL("CExtension::MyASN1OctetString")
    };

    class MyASN1Sequence
        : public ASN1Sequence
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("CExtension::MyASN1Sequence")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI IsCritical(
        /* [out] */ Boolean* isCritical);

    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte>** value);

    CARAPI Encode(
        /* [in] */ IOutputStream* outValue);

    CARAPI GetRawExtnValue(
        /* [out, callee] */ ArrayOf<Byte>** ppRawExtnValue);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI Equals(
        /* [in] */ IInterface* pExt,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pHashCode);

    CARAPI GetDecodedExtensionValue(
        /* [out] */ IExtensionValue** ppExtensionValue);

    CARAPI GetKeyUsageValue(
        /* [out] */ IKeyUsage** ppUsageValue);

    CARAPI GetBasicConstraintsValue(
        /* [out] */ IBasicConstraints** ppBasicConstrainsValue);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CExtension();

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ Boolean critical,
        /* [in] */ IExtensionValue* pExtnValueObject);

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ Boolean critical,
        /* [in] */ ArrayOf<Byte>* pExtnValue);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* pExtnID,
        /* [in] */ Boolean critical,
        /* [in] */ ArrayOf<Byte>* pExtnValue2);

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ ArrayOf<Byte>* pExtnValue);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* pExtnID,
        /* [in] */ ArrayOf<Byte>* pExtnValue2);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* extnID,
        /* [in] */ Boolean critical,
        /* [in] */ ArrayOf<Byte>* extnValue,
        /* [in] */ ArrayOf<Byte>* rawExtnValue,
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [in] */ IExtensionValue* decodedExtValue);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    CARAPI DecodeExtensionValue();

    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * X.509 Extension encoder/decoder.
     */
    static AutoPtr<IASN1Sequence> ASN1;

protected:
    // the decoded extension value
    AutoPtr<IExtensionValue> mExtnValueObject;

private:
    // constants: the extension OIDs
    // certificate extensions:
    static AutoPtr<ArrayOf<Int32> > SUBJ_DIRECTORY_ATTRS;
    static AutoPtr<ArrayOf<Int32> > SUBJ_KEY_ID;
    static AutoPtr<ArrayOf<Int32> > KEY_USAGE;
    static AutoPtr<ArrayOf<Int32> > PRIVATE_KEY_USAGE_PERIOD;
    static AutoPtr<ArrayOf<Int32> > SUBJECT_ALT_NAME;
    static AutoPtr<ArrayOf<Int32> > ISSUER_ALTERNATIVE_NAME;
    static AutoPtr<ArrayOf<Int32> > BASIC_CONSTRAINTS;
    static AutoPtr<ArrayOf<Int32> > NAME_CONSTRAINTS;
    static AutoPtr<ArrayOf<Int32> > CRL_DISTR_POINTS;
    static AutoPtr<ArrayOf<Int32> > CERTIFICATE_POLICIES;
    static AutoPtr<ArrayOf<Int32> > POLICY_MAPPINGS;
    static AutoPtr<ArrayOf<Int32> > AUTH_KEY_ID;
    static AutoPtr<ArrayOf<Int32> > POLICY_CONSTRAINTS;
    static AutoPtr<ArrayOf<Int32> > EXTENDED_KEY_USAGE;
    static AutoPtr<ArrayOf<Int32> > FRESHEST_CRL;
    static AutoPtr<ArrayOf<Int32> > INHIBIT_ANY_POLICY;
    static AutoPtr<ArrayOf<Int32> > AUTHORITY_INFO_ACCESS;
    static AutoPtr<ArrayOf<Int32> > SUBJECT_INFO_ACCESS;
    // crl extensions:
    static AutoPtr<ArrayOf<Int32> > ISSUING_DISTR_POINT;
    // crl entry extensions:
    static AutoPtr<ArrayOf<Int32> > CRL_NUMBER;
    static AutoPtr<ArrayOf<Int32> > CERTIFICATE_ISSUER;
    static AutoPtr<ArrayOf<Int32> > INVALIDITY_DATE;
    static AutoPtr<ArrayOf<Int32> > REASON_CODE;
    static AutoPtr<ArrayOf<Int32> > ISSUING_DISTR_POINTS;

    // the value of extnID field of the structure
    AutoPtr<ArrayOf<Int32> > mExtnID;

    String mExtnID_str;

    // the value of critical field of the structure
    Boolean mCritical;

    // the value of extnValue field of the structure
    AutoPtr<ArrayOf<Byte> > mExtnValue;

    // the ASN.1 encoded form of Extension
    AutoPtr<ArrayOf<Byte> > mEncoding;

    // the raw (not decoded) value of extnValue field of the structure
    AutoPtr<ArrayOf<Byte> > mRawExtnValue;

    // tells whether extension value has been decoded or not
    /*volatile*/ Boolean mValueDecoded;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__
