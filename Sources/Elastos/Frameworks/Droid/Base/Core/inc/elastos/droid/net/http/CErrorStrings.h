
#ifndef __ELASTOS_DROID_NET_HTTP_CERRORSTRINGS_H__
#define __ELASTOS_DROID_NET_HTTP_CERRORSTRINGS_H__

#include "_Elastos_Droid_Net_Http_CErrorStrings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Localized strings for the error codes defined in EventHandler.
 *
 * {@hide}
 */
CarClass(CErrorStrings)
    , public Singleton
    , public IErrorStrings
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the localized error message resource for the given error code.
     * If the code is unknown, we'll return a generic error message.
     */
    CARAPI GetString(
        /* [in] */ Int32 errorCode,
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    /**
     * Get the localized error message resource for the given error code.
     * If the code is unknown, we'll return a generic error message.
     */
    CARAPI GetResource(
        /* [in] */ Int32 errorCode,
        /* [out] */ Int32* result);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CERRORSTRINGS_H__
