
#ifndef __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLE_H__
#define __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLE_H__

#include "_Elastos_Droid_Net_Http_CRequestHandle.h"
#include "elastos/droid/net/http/RequestHandle.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * RequestHandle: handles a request session that may include multiple
 * redirects, HTTP authentication requests, etc.
 *
 * {@hide}
 */
CarClass(CRequestHandle)
    , public RequestHandle
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLE_H__
