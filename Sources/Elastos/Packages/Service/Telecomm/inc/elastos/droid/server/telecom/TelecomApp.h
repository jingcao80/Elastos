
#ifndef __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/TelecomServiceImpl.h"
#include <elastos/droid/app/Application.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Application;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Top-level Application class for Telecom.
 */
class TelecomApp
    : public Application
{
public:
    /** {@inheritDoc} */
    // @Override
    CARAPI OnCreate();

    CARAPI GetMissedCallNotifier(
        /* [out] */ MissedCallNotifier** result);

    CARAPI GetPhoneAccountRegistrar(
        /* [out] */ PhoneAccountRegistrar** result);

private:
    /**
     * The Telecom service implementation.
     */
    AutoPtr<TelecomServiceImpl> mTelecomService;

    /**
     * Missed call notifier. Exists here so that the instance can be shared with
     * {@link TelecomBroadcastReceiver}.
     */
    AutoPtr<MissedCallNotifier> mMissedCallNotifier;

    /**
     * Blacklist call notifier. Exists here so that the instance can be shared with
     * {@link TelecomBroadcastReceiver}.
     */
    AutoPtr<BlacklistCallNotifier> mBlacklistCallNotifier;

    /**
     * Maintains the list of registered {@link android.telecom.PhoneAccountHandle}s.
     */
    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    /**
     * The calls manager for the Telecom service.
     */
    AutoPtr<CallsManager> mCallsManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TELECOMAPP_H__
