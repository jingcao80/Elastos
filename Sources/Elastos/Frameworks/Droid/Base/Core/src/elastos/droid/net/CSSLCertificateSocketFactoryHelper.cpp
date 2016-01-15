
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CSSLCertificateSocketFactory.h"
#include "elastos/droid/net/CSSLCertificateSocketFactoryHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CSSLCertificateSocketFactoryHelper, Singleton, ISSLCertificateSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLCertificateSocketFactoryHelper)

ECode CSSLCertificateSocketFactoryHelper::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [out] */ ISocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetDefault(handshakeTimeoutMillis, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetDefault(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetInsecure(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetInsecure(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetHttpSocketFactory(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ Org::Apache::Http::Conn::SSL::ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetHttpSocketFactory(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::VerifyHostname(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& hostname)
{
    return CSSLCertificateSocketFactory::VerifyHostname(socket, hostname);
}

} //namespace Net
} //namespace Droid
} //namespace Elastos
