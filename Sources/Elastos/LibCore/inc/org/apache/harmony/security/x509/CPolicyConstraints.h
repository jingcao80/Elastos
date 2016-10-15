
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CPolicyConstraints.h"
#include "ExtensionValue.h"
#include "ASN1Sequence.h"

using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPolicyConstraints)
    , public ExtensionValue
    , public IPolicyConstraints
{
public:
    /**
     * X.509 PolicyConstraints encoder/decoder.
     */
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* types);

    protected:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IBigInteger* pRequireExplicitPolicy,
        /* [in] */ IBigInteger* pInhibitPolicyMapping);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    CARAPI constructor(
        /* [in] */ IBigInteger* pRequireExplicitPolicy,
        /* [in] */ IBigInteger* pInhibitPolicyMapping,
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of requireExplicitPolicy field of the structure */
    AutoPtr<IBigInteger> mRequireExplicitPolicy;
    /** the value of inhibitPolicyMapping field of the structure */
    AutoPtr<IBigInteger> mInhibitPolicyMapping;
    /** the ASN.1 encoded form of PolicyConstraints */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
