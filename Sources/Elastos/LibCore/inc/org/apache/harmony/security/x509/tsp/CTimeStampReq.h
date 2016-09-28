
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTimeStampReq.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X509::IExtensions;
using Elastos::Core::Object;
using Elastos::Math::IBigInteger;

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

private:
    // TODO: Add your private member variables here.
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
