
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TSTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TSTINFO_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X509::IExtensions;
using Org::Apache::Harmony::Security::X509::IGeneralName;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Core::Object;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class TSTInfo
    : public Object
    , public ITSTInfo
{
private:
    class MyASN1Sequence1
        : public ASN1Sequence
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("TSTInfo::MyASN1Sequence")
    };

private:
    class MyASN1Sequence2
        : public ASN1Sequence
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("TSTInfo::MyASN1Sequence2")
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetAccuracy(
        /* [out, callee] */ ArrayOf<Int32>** ppAccuracy);

    CARAPI GetExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetGenTime(
        /* [out] */ IDate** ppDate);

    CARAPI GetMessageImprint(
        /* [out] */ IMessageImprint** ppMessageImprint);

    CARAPI GetNonce(
        /* [out] */ IBigInteger** ppNonce);

    CARAPI GetOrdering(
        /* [out] */ Boolean* pOrdering);

    CARAPI GetPolicy(
        /* [out] */ String* pPolicy);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** ppSerialNumber);

    CARAPI GetTsa(
        /* [out] */ IGeneralName** ppTsa);

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ const String& policy,
        /* [in] */ IMessageImprint* pMessageImprint,
        /* [in] */ IBigInteger* pSerialNumber,
        /* [in] */ IDate* pGenTime,
        /* [in] */ ArrayOf<Int32>* pAccuracy,
        /* [in] */ Boolean ordering,
        /* [in] */ IBigInteger* pNonce,
        /* [in] */ IGeneralName* pTsa,
        /* [in] */ IExtensions* pExtensions);

    static CARAPI GetACCURACY(
        /* [out] */ IASN1Sequence** ppAccuracy);

    static CARAPI SetACCURACY(
        /* [in] */ IASN1Sequence* pAccuracy);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initACCURACY();

    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
         Accuracy ::= SEQUENCE {
                seconds        INTEGER           OPTIONAL,
                millis     [0] INTEGER  (1..999) OPTIONAL,
                micros     [1] INTEGER  (1..999) OPTIONAL  }
     */
    static AutoPtr<IASN1Sequence> ACCURACY;

    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;

    String mPolicy;

    AutoPtr<IMessageImprint> mMessageImprint;

    AutoPtr<IBigInteger> mSerialNumber;

    AutoPtr<IDate> mGenTime;

    AutoPtr<ArrayOf<Int32> > mAccuracy;

    Boolean mOrdering;

    AutoPtr<IBigInteger> mNonce;

    AutoPtr<IGeneralName> mTsa;

    AutoPtr<IExtensions> mExtensions;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TSTINFO_H__
