
#ifndef __ELASTOS_DROID_NET_HTTP_CSSLERRORHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CSSLERRORHELPER_H__

#include "_Elastos_Droid_Net_Http_CSslErrorHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * This class represents a set of one or more SSL errors and the associated SSL
 * certificate.
 */
CarClass(CSslErrorHelper)
    , public Singleton
    , public ISslErrorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates an SslError object from a chromium error code.
     * @param error The chromium error code
     * @param certificate The associated SSL certificate
     * @param url The associated URL.
     * @hide  chromium error codes only available inside the framework
     */
    CARAPI SslErrorFromChromiumErrorCode(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* cert,
        /* [in] */ const String& url,
        /* [out] */ ISslError** sslError);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CSSLERRORHELPER_H__
