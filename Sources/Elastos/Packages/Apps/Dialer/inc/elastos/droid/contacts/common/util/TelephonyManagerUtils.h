
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_TELEPHONYMANAGERUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_TELEPHONYMANAGERUTILS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * This class provides several TelephonyManager util functions.
 */
class TelephonyManagerUtils
{
public:
    /**
     * Gets the voicemail tag from Telephony Manager.
     * @param context Current application context
     * @return Voicemail tag, the alphabetic identifier associated with the voice mail number.
     */
    static CARAPI GetVoiceMailAlphaTag(
        /* [in] */ IContext* context,
        /* [out] */ String* tag);

    /**
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in based on the network location. If the network location does not exist, fall
     *         back to the locale setting.
     */
    static CARAPI GetCurrentCountryIso(
        /* [in] */ IContext* context,
        /* [in] */ ILocale* locale,
        /* [out] */ String* iso);

    /**
     * @param context Current application context.
     * @return True if there is a subscription which supports video calls. False otherwise.
     */
    static CARAPI_(Boolean) HasVideoCallSubscription(
        /* [in] */ IContext* context);

private:
    static const String TAG;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_TELEPHONYMANAGERUTILS_H__
