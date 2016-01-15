
#ifndef __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__

#include "_Elastos_Droid_Net_Http_CHttpsConnectionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A Connection connecting to a secure http server or tunneling through
 * a http proxy server to a https server.
 *
 * @hide
 */
CarClass(CHttpsConnectionHelper)
    , public Singleton
    , public IHttpsConnectionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     *
     * @param sessionDir directory to cache SSL sessions
     */
    CARAPI InitializeEngine(
        /* [in] */ IFile* sessionDir);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__
