
#ifndef __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEDNAME_H__
#define __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEDNAME_H__

#include "_Elastos_Droid_Net_Http_CSslCertificateDName.h"
#include "elastos/droid/net/http/SslCertificate.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A distinguished name helper class: a 3-tuple of:
 * <ul>
 *   <li>the most specific common name (CN)</li>
 *   <li>the most specific organization (O)</li>
 *   <li>the most specific organizational unit (OU)</li>
 * <ul>
 */
CarClass(CSslCertificateDName)
    , public SslCertificateDName
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CSSLCERTIFICATEDNAME_H__
