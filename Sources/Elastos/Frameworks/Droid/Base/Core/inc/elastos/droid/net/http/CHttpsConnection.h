
#ifndef __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTION_H__
#define __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTION_H__

#include "_Elastos_Droid_Net_Http_CHttpsConnection.h"
#include "elastos/droid/net/http/HttpsConnection.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A Connection connecting to a secure http server or tunneling through
 * a http proxy server to a https server.
 *
 * @hide
 */
CarClass(CHttpsConnection)
    , public HttpsConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTION_H__
