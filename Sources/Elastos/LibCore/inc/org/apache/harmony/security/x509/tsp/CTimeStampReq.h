
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTimeStampReq.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CTimeStampReq)
    , public Object
    , public ITimeStampReq
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI GetCertReq(
        /* [out] */ Boolean * pCertReq);

    CARAPI GetExtensions(
        /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions);

    CARAPI GetMessageImprint(
        /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint ** ppMessageImprint);

    CARAPI GetNonce(
        /* [out] */ Elastos::Math::IBigInteger ** ppNonce);

    CARAPI GetReqPolicy(
        /* [out] */ String * pReqPolicy);

    CARAPI GetVersion(
        /* [out] */ Int32 * pVer);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint * pMessageImprint,
        /* [in] */ const String& reqPolicy,
        /* [in] */ Elastos::Math::IBigInteger * pNonce,
        /* [in] */ Boolean certReq,
        /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pExtensions);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
