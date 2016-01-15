
#ifndef __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPCLIENTCONNECTION_H__
#define __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPCLIENTCONNECTION_H__

#include "org/apache/http/impl/SocketHttpClientConnection.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Default implementation of a client-side HTTP connection.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 561083 $
 *
 * @since 4.0
 */
class DefaultHttpClientConnection : public SocketHttpClientConnection
{
public:
    DefaultHttpClientConnection();

    CARAPI Bind(
        /* [in] */ ISocket* socket,
        /* [in] */ IHttpParams* params);

    CARAPI ToString(
        /* [out] */ String* str);
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPCLIENTCONNECTION_H__
