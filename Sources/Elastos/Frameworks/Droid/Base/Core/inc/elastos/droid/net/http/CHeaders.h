
#ifndef __ELASTOS_DROID_NET_HTTP_CHEADERS_H__
#define __ELASTOS_DROID_NET_HTTP_CHEADERS_H__

#include "_Elastos_Droid_Net_Http_CHeaders.h"
#include "elastos/droid/net/http/Headers.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Manages received headers
 *
 * {@hide}
 */
CarClass(CHeaders)
    , public Headers
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CHEADERS_H__
