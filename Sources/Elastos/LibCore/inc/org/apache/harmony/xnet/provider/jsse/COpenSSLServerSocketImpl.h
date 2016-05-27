
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLServerSocketImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLServerSocketImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetImpl(
        /* [out] */ Elastos::Net::ISocketImpl ** ppImpl);

    CARAPI Accept(
        /* [out] */ Elastos::Net::ISocket ** ppSocket);

    CARAPI Close();

    CARAPI GetInetAddress(
        /* [out] */ Elastos::Net::IInetAddress ** ppAddress);

    CARAPI GetLocalPort(
        /* [out] */ Int32 * pPort);

    CARAPI GetSoTimeout(
        /* [out] */ Int32 * pTimeout);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    CARAPI Bind(
        /* [in] */ Elastos::Net::ISocketAddress * pLocalAddr);

    CARAPI BindEx(
        /* [in] */ Elastos::Net::ISocketAddress * pLocalAddr,
        /* [in] */ Int32 backlog);

    CARAPI GetLocalSocketAddress(
        /* [out] */ Elastos::Net::ISocketAddress ** ppAddress);

    CARAPI IsBound(
        /* [out] */ Boolean * pIsBound);

    CARAPI IsClosed(
        /* [out] */ Boolean * pIsClosed);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean * pReuse);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetChannel(
        /* [out] */ Elastos::Net::IServerSocketChannel ** ppChannel);

    CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

    CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String> * pSuites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String> * pProtocols);

    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need);

    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean * pNeed);

    CARAPI SetWantClientAuth(
        /* [in] */ Boolean want);

    CARAPI GetWantClientAuth(
        /* [out] */ Boolean * pWant);

    CARAPI SetUseClientMode(
        /* [in] */ Boolean mode);

    CARAPI GetUseClientMode(
        /* [out] */ Boolean * pMode);

    CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag);

    CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean * pEnabled);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETIMPL_H__
