
#ifndef __ELASTOS_DROID_NET_HTTP_CREQUESTQUEUE_H__
#define __ELASTOS_DROID_NET_HTTP_CREQUESTQUEUE_H__

#include "_Elastos_Droid_Net_Http_CRequestQueue.h"
#include "elastos/droid/net/http/RequestQueue.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
CarClass(CRequestQueue)
    , public RequestQueue
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CREQUESTQUEUE_H__
