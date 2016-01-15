
#ifndef __ELASTOS_DROID_NET_HTTP_CX509TRUSTMANAGEREXTENSIONS_H__
#define __ELASTOS_DROID_NET_HTTP_CX509TRUSTMANAGEREXTENSIONS_H__

#include "_Elastos_Droid_Net_Http_CX509TrustManagerExtensions.h"
#include "elastos/droid/net/http/X509TrustManagerExtensions.h"

using Org::Conscrypt::ITrustManagerImpl;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * X509TrustManager wrapper exposing Android-added features.
 * <p>
 * The checkServerTrusted method allows callers to perform additional
 * verification of certificate chains after they have been successfully verified
 * by the platform.
 * </p>
 */
CarClass(CX509TrustManagerExtensions)
    , public X509TrustManagerExtensions
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CX509TRUSTMANAGEREXTENSIONS_H__
