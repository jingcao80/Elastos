
#ifndef __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTBROADCASTRECEIVER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Captures {@code android.intent.action.ACTION_PACKAGE_FULLY_REMOVED} intents and triggers the
 * removal of associated {@link android.telecom.PhoneAccount}s via the
 * {@link com.android.telecom.PhoneAccountRegistrar}.
 * Note: This class listens for the {@code PACKAGE_FULLY_REMOVED} intent rather than
 * {@code PACKAGE_REMOVED} as {@code PACKAGE_REMOVED} is triggered on re-installation of the same
 * package, where {@code PACKAGE_FULLY_REMOVED} is triggered only when an application is completely
 * uninstalled.  This is desirable as we do not wish to un-register all
 * {@link android.telecom.PhoneAccount}s associated with a package being re-installed to ensure
 * the enabled state of the accounts is retained.
 */
class PhoneAccountBroadcastReceiver
    : public BroadcastReceiver
    , public IPhoneAccountBroadcastReceiver
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Receives the intents the class is configured to received.
     *
     * @param context The Context in which the receiver is running.
     * @param intent The Intent being received.
     */
    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * Handles the removal of a package by calling upon the {@link PhoneAccountRegistrar} to
     * un-register any {@link android.telecom.PhoneAccount}s associated with the package.
     *
     * @param packageName The name of the removed package.
     */
    CARAPI HandlePackageRemoved(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTBROADCASTRECEIVER_H__
