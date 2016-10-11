
#ifndef __ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__

#include "_Org.Conscrypt.h"
#include "elastosx/net/ssl/SSLSocketFactory.h"

using Elastos::Net::ISocket;
using Elastos::Net::IInetAddress;
using Elastosx::Net::Ssl::SSLSocketFactory;

namespace Org {
namespace Conscrypt {

class OpenSSLSocketFactoryImpl
    : public SSLSocketFactory
    , public IOpenSSLSocketFactoryImpl
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI GetDefaultCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetSupportedCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CreateSocket(
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localHost,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ ISocket* s,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** result);

private:
    AutoPtr<ISSLParametersImpl> mSslParameters;
    // AutoPtr<IIOException> mInstantiationException;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__