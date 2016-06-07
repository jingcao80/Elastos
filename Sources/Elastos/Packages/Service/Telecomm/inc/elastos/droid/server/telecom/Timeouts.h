
#ifndef __ELASTOS_DROID_SERVER_TELECOM_TIMEOUTS_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TIMEOUTS_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * A helper class which serves only to make it easier to lookup timeout values. This class should
 * never be instantiated, and only accessed through the {@link #get(String, long)} method.
 *
 * These methods are safe to call from any thread, including the UI thread.
 */
class Timeouts
    : public Object
{
public:
    /**
     * Returns the longest period, in milliseconds, to wait for the query for direct-to-voicemail
     * to complete. If the query goes beyond this timeout, the incoming call screen is shown to the
     * user.
     */
    static CARAPI GetDirectToVoicemailMillis(
        /* [in] */ IContentResolver* contentResolver,
        /* [out] */ Int64* result);

private:
    Timeouts();

    /**
     * Returns the timeout value from Settings or the default value if it hasn't been changed. This
     * method is safe to call from any thread, including the UI thread.
     *
     * @param contentResolver The content resolved.
     * @param key Settings key to retrieve.
     * @param defaultValue Default value, in milliseconds.
     * @return The timeout value from Settings or the default value if it hasn't been changed.
     */
    static CARAPI Get(
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ const String& key,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* result);

private:
    /** A prefix to use for all keys so to not clobber the global namespace. */
    static const String PREFIX;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TIMEOUTS_H__
