
#ifndef __ELASTOS_DROID_NET_HTTP_CLOGGINGEVENTHANDLER_H__
#define __ELASTOS_DROID_NET_HTTP_CLOGGINGEVENTHANDLER_H__

#include "_Elastos_Droid_Net_Http_CLoggingEventHandler.h"
#include "elastos/droid/net/http/LoggingEventHandler.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
CarClass(CLoggingEventHandler)
    , public LoggingEventHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CLOGGINGEVENTHANDLER_H__
