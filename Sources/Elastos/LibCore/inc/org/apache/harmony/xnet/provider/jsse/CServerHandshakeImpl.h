
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHANDSHAKEIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHANDSHAKEIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CServerHandshakeImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CServerHandshakeImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetEngineOwner(
        /* [out] */ Elastosx::Net::Ssl::ISSLEngine ** ppEngineOwner);

    CARAPI SetEngineOwner(
        /* [in] */ Elastosx::Net::Ssl::ISSLEngine * pEngineOwner);

    CARAPI GetSocketOwner(
        /* [out] */ Elastosx::Net::Ssl::ISSLSocket ** ppSocketOwner);

    CARAPI SetSocketOwner(
        /* [in] */ Elastosx::Net::Ssl::ISSLSocket * pSocketOwner);

    CARAPI SetRecordProtocol(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLRecordProtocol * pRecordProtocol);

    CARAPI Start();

    CARAPI GetStatus(
        /* [out] */ Int32 * pStatus);

    CARAPI GetSession(
        /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession);

    CARAPI Unwrap(
        /* [in] */ ArrayOf<Byte> * pBytes);

    CARAPI UnwrapSSLv2(
        /* [in] */ ArrayOf<Byte> * pBytes);

    CARAPI Wrap(
        /* [out, callee] */ ArrayOf<Byte> ** ppWrap);

    CARAPI Send(
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IMessage * pMessage);

    CARAPI ComputerMasterSecret();

    CARAPI GetTask(
        /* [out] */ Elastos::Core::IRunnable ** ppTask);

    CARAPI constructor(
        /* [in] */ IInterface * pOwner);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSERVERHANDSHAKEIMPL_H__
