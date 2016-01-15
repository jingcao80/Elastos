
#ifndef __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORYHELPER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORYHELPER_H_

#include "_Org_Apache_Http_Conn_SSL_CSSLSocketFactoryHelper.h"
#include "elastos/core/Singleton.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

CarClass(CSSLSocketFactoryHelper)
    , public Singleton
    , public ISSLSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAllowAllHostnameVerifier(
        /* [out] */ IX509HostnameVerifier** verifier);

    CARAPI GetBrowserCompatibleHostnameVerifier(
        /* [out] */ IX509HostnameVerifier** verifier);

    CARAPI GetStrictHostnameVerifier(
        /* [out] */ IX509HostnameVerifier** verifier);

    CARAPI GetSocketFactory(
        /* [out] */ ISSLSocketFactory** factory);
};

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORYHELPER_H_
