
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSERVERSOCKETFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSERVERSOCKETFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLServerSocketFactoryImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLServerSocketFactoryImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI CreateServerSocket(
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx(
        /* [in] */ Int32 port,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx2(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx3(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ Elastos::Net::IInetAddress * pIAddress,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSERVERSOCKETFACTORYIMPL_H__
