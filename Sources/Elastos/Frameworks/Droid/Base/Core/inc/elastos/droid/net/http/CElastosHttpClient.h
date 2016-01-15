
#ifndef __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENT_H__
#define __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENT_H__

#include "_Elastos_Droid_Net_Http_CElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Implementation of the Apache {@link DefaultHttpClient} that is configured with
 * reasonable default settings and registered schemes for Android.
 * Don't create this directly, use the {@link #newInstance} factory method.
 *
 * <p>This client processes cookies but does not retain them by default.
 * To retain cookies, simply add a cookie store to the HttpContext:</p>
 *
 * <pre>context.setAttribute(ClientContext.COOKIE_STORE, cookieStore);</pre>
 */
CarClass(CElastosHttpClient)
    , public ElastosHttpClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENT_H__
