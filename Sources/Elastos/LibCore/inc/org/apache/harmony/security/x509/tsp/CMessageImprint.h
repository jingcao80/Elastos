
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CMessageImprint.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CMessageImprint)
    , public Object
    , public IMessageImprint
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
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

    CARAPI constructor(
        /* [in] */ IAlgorithmIdentifier* pAlgId,
        /* [in] */ ArrayOf<Byte>* pHashedMessage);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    AutoPtr<IAlgorithmIdentifier> mAlgId;
    AutoPtr<ArrayOf<Byte> > mHashedMessage;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CMESSAGEIMPRINT_H__
