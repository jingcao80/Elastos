
#ifndef __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastosx::Net::Ssl::ISSLServerSocketFactory;
using Elastos::Net::IServerSocket;
using Elastos::Net::IInetAddress;

namespace Org {
namespace Conscrypt {

class OpenSSLServerSocketFactoryImpl
    : public Object
    , public ISSLServerSocketFactory
    , public IOpenSSLServerSocketFactoryImpl
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

    CARAPI CreateServerSocket(
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** result);

private:
    AutoPtr<ISSLParametersImpl> mSslParameters;
    // AutoPtr<IIOException> mInstantiationException;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__