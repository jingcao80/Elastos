
#ifndef __ELASTOS_DROID_NET_HTTP_CHTTPAUTHHEADER_H__
#define __ELASTOS_DROID_NET_HTTP_CHTTPAUTHHEADER_H__

#include "_Elastos_Droid_Net_Http_CHttpAuthHeader.h"
#include "elastos/droid/net/http/HttpAuthHeader.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * HttpAuthHeader: a class to store HTTP authentication-header parameters.
 * For more information, see: RFC 2617: HTTP Authentication.
 *
 * {@hide}
 */
CarClass(CHttpAuthHeader)
    , public HttpAuthHeader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CHTTPAUTHHEADER_H__
