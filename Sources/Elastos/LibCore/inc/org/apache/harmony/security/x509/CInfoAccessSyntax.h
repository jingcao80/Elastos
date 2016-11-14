
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__

#include "_Org_Apache_Harmony_Security_X509_CInfoAccessSyntax.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X509::ExtensionValue;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInfoAccessSyntax)
    , public ExtensionValue
    , public IInfoAccessSyntax
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

        TO_STRING_IMPL("CInfoAccessSyntax::MyASN1Sequence")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI constructor(
        /* [in] */ IList* accessDescriptions,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ IInfoAccessSyntax** ppSyntax);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 DER X.509 AuthorityInfoAccessSyntax and SubjectInfoAccessSyntax
     * encoder/decoder class.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    AutoPtr<IList> mAccessDescriptions;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
