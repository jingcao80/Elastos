
#ifndef __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTCONNECTION_H__
#define __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTCONNECTION_H__

#include "_Elastos_Droid_Net_Http_CElastosHttpClientConnection.h"
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A alternate class for (@link DefaultHttpClientConnection).
 * It has better performance than DefaultHttpClientConnection
 *
 * {@hide}
 */
CarClass(CElastosHttpClientConnection)
    , public ElastosHttpClientConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTCONNECTION_H__
