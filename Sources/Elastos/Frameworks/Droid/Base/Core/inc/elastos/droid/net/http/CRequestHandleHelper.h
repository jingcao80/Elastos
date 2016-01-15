
#ifndef __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLEHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLEHELPER_H__

#include "_Elastos_Droid_Net_Http_CRequestHandleHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * RequestHandle: handles a request session that may include multiple
 * redirects, HTTP authentication requests, etc.
 *
 * {@hide}
 */
CarClass(CRequestHandleHelper)
    , public Singleton
    , public IRequestHandleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @return Basic-scheme authentication response: BASE64(username:password).
     */
    CARAPI ComputeBasicAuthResponse(
        /* [in] */ const String& username,
        /* [in] */ const String& password,
        /* [out] */ String* result);

    /**
     * @return The right authorization header (dependeing on whether it is a proxy or not).
     */
    CARAPI AuthorizationHeader(
        /* [in] */ Boolean isProxy,
        /* [out] */ String* result);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CREQUESTHANDLEHELPER_H__
