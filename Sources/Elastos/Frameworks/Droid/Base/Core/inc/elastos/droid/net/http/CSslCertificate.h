
#ifndef __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATE_H__
#define __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATE_H__

#include "_Elastos_Droid_Net_Http_CSslCertificate.h"
#include "elastos/droid/net/http/SslCertificate.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * SSL certificate info (certificate details) class
 */
CarClass(CSslCertificate)
    , public SslCertificate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATE_H__
