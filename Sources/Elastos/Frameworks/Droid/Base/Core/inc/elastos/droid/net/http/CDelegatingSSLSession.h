
#ifndef __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSION_H__
#define __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSION_H__

#include "_Elastos_Droid_Net_Http_CDelegatingSSLSession.h"
#include "elastos/droid/net/http/DelegatingSSLSession.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * This is only used when a {@code certificate} is available but usage
 * requires a {@link SSLSession}.
 *
 * @hide
 */
CarClass(CDelegatingSSLSession)
    , public DelegatingSSLSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSION_H__
