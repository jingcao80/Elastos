
#ifndef __ELASTOS_DROID_NET_HTTP_ERRORSTRINGS_H__
#define __ELASTOS_DROID_NET_HTTP_ERRORSTRINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

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
class ErrorStrings
    : public Object
{
public:
    /**
     * Get the localized error message resource for the given error code.
     * If the code is unknown, we'll return a generic error message.
     */
    static CARAPI GetString(
        /* [in] */ Int32 errorCode,
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    /**
     * Get the localized error message resource for the given error code.
     * If the code is unknown, we'll return a generic error message.
     */
    static CARAPI GetResource(
        /* [in] */ Int32 errorCode,
        /* [out] */ Int32* result);

private:
    ErrorStrings();

    static const String LOGTAG;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_ERRORSTRINGS_H__
