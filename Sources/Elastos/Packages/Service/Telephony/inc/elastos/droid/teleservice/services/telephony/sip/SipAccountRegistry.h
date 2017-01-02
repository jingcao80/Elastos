//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPACCOUNTREGISTRY_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPACCOUNTREGISTRY_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Core::Runnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

/**
 * Manages the {@link PhoneAccount} entries for SIP calling.
 */
class SipAccountRegistry
    : public Object
{
private:
    class AccountEntry
        : public Object
    {
    public:
        TO_STRING_IMPL("SipAccountRegistry")

    //     AccountEntry(
    //         /* [in] */ SipProfile* profile);

    //     CARAPI GetProfile(
    //         /* [out] */ SipProfile** profile);

    //     /**
    //      * Starts the SIP service associated with the SIP profile.
    //      *
    //      * @param sipManager The SIP manager.
    //      * @param context The context.
    //      * @param isReceivingCalls {@code True} if the sip service is being started to make and
    //      *          receive calls.  {@code False} if the sip service is being started only for
    //      *          outgoing calls.
    //      * @return {@code True} if the service started successfully.
    //      */
    //     CARAPI StartSipService(
    //         /* [in] */ SipManager* sipManager,
    //         /* [in] */ IContext* context,
    //         /* [in] */ Boolean isReceivingCalls,
    //         /* [out] */ Boolean* result);

    //     /**
    //      * Stops the SIP service associated with the SIP profile.  The {@code SipAccountRegistry} is
    //      * informed when the service has been stopped via an intent which triggers
    //      * {@link SipAccountRegistry#removeSipProfile(String)}.
    //      *
    //      * @param sipManager The SIP manager.
    //      * @return {@code True} if stop was successful.
    //      */
    //     CARAPI StopSipService(
    //         /* [in] */ SipManager* sipManager,
    //         /* [out] */ Boolean* result);

    // private:
    //     AutoPtr<SipProfile> mProfile;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("SipAccountRegistry::MyRunnable")

        MyRunnable(
            /* [in] */ SipAccountRegistry* host,
            /* [in] */ IContext* context,
            /* [in] */ const String& sipUri)
            : mHost(host)
            , mContext(context)
            , mSipUri(sipUri)
        {}

        //@Override
        CARAPI Run();

    private:
        SipAccountRegistry* mHost;
        AutoPtr<IContext> mContext;
        String mSipUri;
    };

public:
    TO_STRING_IMPL("SipAccountRegistry")

    static CARAPI_(AutoPtr<SipAccountRegistry>) GetInstance();

    CARAPI Setup(
        /* [in] */ IContext* context);

    /**
     * Starts the SIP service for the specified SIP profile and ensures it has a valid registered
     * {@link PhoneAccount}.
     *
     * @param context The context.
     * @param sipUri The Uri of the {@link SipProfile} to start, or {@code null} for all.
     */
    CARAPI StartSipService(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    /**
     * Removes a {@link SipProfile} from the account registry.  Does not stop/close the associated
     * SIP service (this method is invoked via an intent from the SipService once a profile has
     * been stopped/closed).
     *
     * @param sipUri The Uri of the {@link SipProfile} to remove from the registry.
     */
    CARAPI RemoveSipProfile(
        /* [in] */ const String& sipUri);

    /**
     * Stops a SIP profile and un-registers its associated {@link android.telecom.PhoneAccount}.
     * Called after a SIP profile is deleted.  The {@link AccountEntry} will be removed when the
     * service has been stopped.  The {@code SipService} fires the {@code ACTION_SIP_REMOVE_PHONE}
     * intent, which triggers {@link SipAccountRegistry#removeSipProfile(String)} to perform the
     * removal.
     *
     * @param context The context.
     * @param sipUri The {@code Uri} of the sip profile.
     */
    CARAPI StopSipService(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    /**
     * Causes the SIP service to be restarted for all {@link SipProfile}s.  For example, if the user
     * toggles the "receive calls" option for SIP, this method handles restarting the SIP services
     * in the new mode.
     *
     * @param context The context.
     */
    CARAPI RestartSipService(
        /* [in] */ IContext* context);

private:
    SipAccountRegistry();

    /**
     * Performs an asynchronous call to
     * {@link SipAccountRegistry#startSipProfiles(android.content.Context, String)}, starting the
     * specified SIP profile and registering its {@link android.telecom.PhoneAccount}.
     *
     * @param context The context.
     * @param sipUri A specific SIP uri to start.
     */
    CARAPI_(void) StartSipProfilesAsync(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    /**
     * Loops through all SIP accounts from the SIP database, starts each service and registers
     * each with the telecom framework. If a specific sipUri is specified, this will only register
     * the associated SIP account.
     *
     * @param context The context.
     * @param sipUri A specific SIP uri to start, or {@code null} to start all.
     */
    CARAPI_(void) StartSipProfiles(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    /**
     * Starts the SIP service for a sip profile and saves a new {@code AccountEntry} in the
     * registry.
     *
     * @param profile The {@link SipProfile} to start.
     * @param sipManager The SIP manager.
     * @param context The context.
     * @param isReceivingCalls {@code True} if the profile should be started such that it can
     *      receive incoming calls.
     */
    // CARAPI_(void) startSipServiceForProfile(
    //     /* [in] */ SipProfile* profile,
    //     /* [in] */ SipManager* sipManager,
    //     /* [in] */ IContext* context,
    //     /* [in] */ Boolean isReceivingCalls);

    /**
     * Retrieves the {@link AccountEntry} from the registry with the specified Uri.
     *
     * @param sipUri The Uri of the profile to retrieve.
     * @return The {@link AccountEntry}, or {@code null} is it was not found.
     */
    CARAPI_(AutoPtr<AccountEntry>) GetAccountEntry(
        /* [in] */ const String& sipUri);

    CARAPI_(void) Log(
        /* [in] */ const String& message);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */
    static const AutoPtr<SipAccountRegistry> INSTANCE;

    AutoPtr<IList> mAccounts;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPACCOUNTREGISTRY_H__