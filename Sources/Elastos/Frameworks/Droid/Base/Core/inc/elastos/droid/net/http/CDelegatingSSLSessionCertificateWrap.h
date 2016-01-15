
#ifndef __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSIONCERTIFICATEWRAP_H__
#define __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSIONCERTIFICATEWRAP_H__

#include "_Elastos_Droid_Net_Http_CDelegatingSSLSessionCertificateWrap.h"
#include "elastos/droid/net/http/DelegatingSSLSession.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CarClass(CDelegatingSSLSessionCertificateWrap)
    , public DelegatingSSLSessionCertificateWrap
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CDELEGATINGSSLSESSIONCERTIFICATEWRAP_H__
