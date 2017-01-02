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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_TELECOMACCOUNTREGISTRY_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_TELECOMACCOUNTREGISTRY_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Owns all data we have registered with Telecom including handling dynamic addition and
 * removal of SIMs and SIP accounts.
 */
class TelecomAccountRegistry
    : public Object
    , public ITelecomAccountRegistry
{
public:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TelecomAccountRegistry::TtyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ ITelecomAccountRegistry* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TelecomAccountRegistry* mHost;
    };

private:
    class AccountEntry
        : public Object
    {
    public:
        TO_STRING_IMPL("TelecomAccountRegistry::AccountEntry")

        AccountEntry(
            /* [in] */ TelecomAccountRegistry* host,
            /* [in] */ IPhone* phone,
            /* [in] */ Boolean isEmergency,
            /* [in] */ Boolean isDummy);

        CARAPI Teardown();

        CARAPI GetPhoneAccountHandle(
            /* [out] */ IPhoneAccountHandle** handle);

        /**
         * Registers the specified account with Telecom as a PhoneAccountHandle.
         */
    private:
        CARAPI_(AutoPtr<IPhoneAccount>) RegisterPstnPhoneAccount(
            /* [in] */ Boolean isEmergency,
            /* [in] */ Boolean isDummyAccount);

    private:
        TelecomAccountRegistry* mHost;
        AutoPtr<IPhone> mPhone;
        AutoPtr<IPhoneAccount> mAccount;
        AutoPtr<IPstnIncomingCallNotifier> mIncomingCallNotifier;
    };

    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("TelecomAccountRegistry::MyPhoneStateListener")

        MyPhoneStateListener(
            /* [in] */ TelecomAccountRegistry* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnServiceStateChanged(
            /* [in] */ IServiceState* serviceState);

    private:
        TelecomAccountRegistry* mHost;
    };

public:
    TO_STRING_IMPL("TelecomAccountRegistry")

    CAR_INTERFACE_DECL()

    TelecomAccountRegistry(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<TelecomAccountRegistry>) GetInstance(
        /* [in] */ IContext* context);

    /**
     * Sets up all the phone accounts for SIMs on first boot.
     */
    CARAPI SetupOnBoot();

    static CARAPI_(AutoPtr<IPhoneAccountHandle>) MakePstnPhoneAccountHandle(
        /* [in] */ IPhone* phone);

private:
    static CARAPI_(AutoPtr<IPhoneAccountHandle>) MakePstnPhoneAccountHandleWithPrefix(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& prefix,
        /* [in] */ Boolean isEmergency);

    /**
     * Determines if the list of {@link AccountEntry}(s) contains an {@link AccountEntry} with a
     * specified {@link PhoneAccountHandle}.
     *
     * @param handle The {@link PhoneAccountHandle}.
     * @return {@code True} if an entry exists.
     */
    CARAPI_(Boolean) HasAccountEntryForPhoneAccount(
        /* [in] */ IPhoneAccountHandle* handle);

    /**
     * Un-registers any {@link PhoneAccount}s which are no longer present in the list
     * {@code AccountEntry}(s).
     */
    CARAPI_(void) CleanupPhoneAccounts();

    CARAPI_(void) SetupAccounts();

    CARAPI_(Int32) GetPhoneAccountIcon(
        /* [in] */ Int32 index);

    CARAPI_(void) TearDownAccounts();

private:
    static const Boolean DBG; /* STOP SHIP if true */

    // Slot IDs are zero based indices but the numbered icons represent the first, second,
    // etc... SIM in the device. So that means that index 0 is SIM 1, index 1 is SIM 2 and so on.

    static const AutoPtr<ArrayOf<Int32> > sPhoneAccountIcons;

    // This icon is the one that is used when the Slot ID that we have for a particular SIM
    // is not supported, i.e. SubscriptionManager.INVALID_SLOT_ID or the 5th SIM in a phone.
    static const Int32 defaultPhoneAccountIcon;

    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    static AutoPtr<TelecomAccountRegistry> sInstance;
    AutoPtr<IContext> mContext;
    AutoPtr<ITelecomManager> mTelecomManager;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<IList> mAccounts;
    Int32 mServiceState;

    static Object sLock;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TELECOMACCOUNTREGISTRY_H__