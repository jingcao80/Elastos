
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPREQ_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPREQ_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X509::IExtensions;
using Elastos::Core::Object;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class TimeStampReq
    : public Object
    , public ITimeStampReq
{
private:
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
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI GetCertReq(
        /* [out] */ Boolean* pCertReq);

    CARAPI GetExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetMessageImprint(
        /* [out] */ IMessageImprint** ppMessageImprint);

    CARAPI GetNonce(
        /* [out] */ IBigInteger** ppNonce);

    CARAPI GetReqPolicy(
        /* [out] */ String* pReqPolicy);

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ IMessageImprint* pMessageImprint,
        /* [in] */ const String& reqPolicy,
        /* [in] */ IBigInteger* pNonce,
        /* [in] */ Boolean certReq,
        /* [in] */ IExtensions* pExtensions);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ IMessageImprint* messageImprint,
        /* [in] */ const String& reqPolicy,
        /* [in] */ IBigInteger* nonce,
        /* [in] */ Boolean certReq,
        /* [in] */ IExtensions* extensions,
        /* [in] */ ArrayOf<Byte>* encoding);

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;

    AutoPtr<IMessageImprint> mMessageImprint;

    String mReqPolicy;

    AutoPtr<IBigInteger> mNonce;

    Boolean mCertReq;

    AutoPtr<IExtensions> mExtensions;

    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPREQ_H__
