
#ifndef __ELASTOS_DROID_SERVER_TELECOM_QUICKRESPONSEUTILS_H__
#define __ELASTOS_DROID_SERVER_TELECOM_QUICKRESPONSEUTILS_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Utils class that exposes some helper routines to used to manage the QuickResponses
 */
class QuickResponseUtils
    : public Object
{
public:
    /**
     * As of L, QuickResponses were moved from Telephony to Telecom. Because of
     * this, we need to make sure that we migrate any old QuickResponses to our
     * current SharedPreferences.  This is a lazy migration as it happens only when
     * the QuickResponse settings are viewed or if they are queried via RespondViaSmsManager.
     */
    static CARAPI MaybeMigrateLegacyQuickResponses(
        /* [in] */ IContext* context);

public:
    static const String LOG__TAG;

    // SharedPreferences file name for our persistent settings.
    static const String SHARED_PREFERENCES_NAME;

    // Preference keys for the 4 "canned responses"; see RespondViaSmsManager$Settings.
    // Since (for now at least) the number of messages is fixed at 4, and since
    // SharedPreferences can't deal with arrays anyway, just store the messages
    // as 4 separate strings.
    static const Int32 NUM_CANNED_RESPONSES;
    static const String KEY_CANNED_RESPONSE_PREF_1;
    static const String KEY_CANNED_RESPONSE_PREF_2;
    static const String KEY_CANNED_RESPONSE_PREF_3;
    static const String KEY_CANNED_RESPONSE_PREF_4;

private:
    static const String PACKAGE_NAME_TELEPHONY;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_QUICKRESPONSEUTILS_H__
