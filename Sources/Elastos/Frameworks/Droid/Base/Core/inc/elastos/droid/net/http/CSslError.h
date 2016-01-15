
#ifndef __ELASTOS_DROID_NET_HTTP_CSSLERROR_H__
#define __ELASTOS_DROID_NET_HTTP_CSSLERROR_H__

#include "_Elastos_Droid_Net_Http_CSslError.h"
#include "elastos/droid/net/http/SslError.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * This class represents a set of one or more SSL errors and the associated SSL
 * certificate.
 */
CarClass(CSslError)
    , public SslError
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CSSLERROR_H__
