
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLSocketImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLSocketImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI Close();

    CARAPI GetInetAddress(
        /* [out] */ Elastos::Net::IInetAddress ** ppAddress);

    CARAPI GetInputStream(
        /* [out] */ Elastos::IO::IInputStream ** ppIs);

    CARAPI GetKeepAlive(
        /* [out] */ Boolean * pKeepAlive);

    CARAPI GetLocalAddress(
        /* [out] */ Elastos::Net::IInetAddress ** ppAddress);

    CARAPI GetLocalPort(
        /* [out] */ Int32 * pLocalPort);

    CARAPI GetOutputStream(
        /* [out] */ Elastos::IO::IOutputStream ** ppOs);

    CARAPI GetPort(
        /* [out] */ Int32 * pPort);

    CARAPI GetSoLinger(
        /* [out] */ Int32 * pValue);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetSendBufferSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetSoTimeout(
        /* [out] */ Int32 * pTimeout);

    CARAPI GetTcpNoDelay(
        /* [out] */ Boolean * pNoDelay);

    CARAPI SetKeepAlive(
        /* [in] */ Boolean keepAlive);

    CARAPI SetSendBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetSoLinger(
        /* [in] */ Boolean on,
        /* [in] */ Int32 timeout);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    CARAPI SetTcpNoDelay(
        /* [in] */ Boolean on);

    CARAPI ShutdownInput();

    CARAPI ShutdownOutput();

    CARAPI GetLocalSocketAddress(
        /* [out] */ Elastos::Net::ISocketAddress ** ppAddress);

    CARAPI GetRemoteSocketAddress(
        /* [out] */ Elastos::Net::ISocketAddress ** ppAddress);

    CARAPI IsBound(
        /* [out] */ Boolean * pIsBound);

    CARAPI IsConnected(
        /* [out] */ Boolean * pIsConnected);

    CARAPI IsClosed(
        /* [out] */ Boolean * pIsClosed);

    CARAPI Bind(
        /* [in] */ Elastos::Net::ISocketAddress * pLocalAddr);

    CARAPI Connect(
        /* [in] */ Elastos::Net::ISocketAddress * pRemoteAddr);

    CARAPI ConnectEx(
        /* [in] */ Elastos::Net::ISocketAddress * pRemoteAddr,
        /* [in] */ Int32 timeout);

    CARAPI IsInputShutdown(
        /* [out] */ Boolean * pIsInputShutdown);

    CARAPI IsOutputShutdown(
        /* [out] */ Boolean * pIsOutputShutdown);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean * pReuse);

    CARAPI SetOOBInline(
        /* [in] */ Boolean oobinline);

    CARAPI GetOOBInline(
        /* [out] */ Boolean * pOobinline);

    CARAPI SetTrafficClass(
        /* [in] */ Int32 value);

    CARAPI GetTrafficClass(
        /* [out] */ Int32 * pValue);

    CARAPI SendUrgentData(
        /* [in] */ Int32 value);

    CARAPI GetChannel(
        /* [out] */ Elastos::Net::ISocketChannel ** ppChannel);

    CARAPI GetFileDescriptor(
        /* [out] */ Elastos::IO::IFileDescriptor ** ppFd);

    CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String> * pSuites);

    CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String> * pProtocols);

    CARAPI GetSession(
        /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession);

    CARAPI AddHandshakeCompletedListener(
        /* [in] */ Elastosx::Net::Ssl::IHandshakeCompletedListener * pListener);

    CARAPI RemoveHandshakeCompletedListener(
        /* [in] */ Elastosx::Net::Ssl::IHandshakeCompletedListener * pListener);

    CARAPI StartHandshake();

    CARAPI SetUseClientMode(
        /* [in] */ Boolean mode);

    CARAPI GetUseClientMode(
        /* [out] */ Boolean * pMode);

    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need);

    CARAPI SetWantClientAuth(
        /* [in] */ Boolean want);

    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean * pAuth);

    CARAPI GetWantClientAuth(
        /* [out] */ Boolean * pAuth);

    CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag);

    CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean * pEnabled);

    CARAPI GetSSLParameters(
        /* [out] */ Elastosx::Net::Ssl::ISSLParameters ** ppParam);

    CARAPI SetSSLParameters(
        /* [in] */ Elastosx::Net::Ssl::ISSLParameters * pP);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETIMPL_H__
