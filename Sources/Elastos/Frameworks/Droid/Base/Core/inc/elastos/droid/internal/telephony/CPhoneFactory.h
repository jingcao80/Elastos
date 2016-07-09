
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEFACTORY_H__

#include "_Elastos_Droid_Internal_Telephony_CPhoneFactory.h"
#include "elastos/droid/internal/telephony/ModemStackController.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Core::IInteger32;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CPhoneFactory)
    , public Singleton
    , public IPhoneFactory
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    //***** Class Methods

    CARAPI MakeDefaultPhones(
        /* [in] */ IContext* context);

    /**
     * FIXME replace this with some other way of making these
     * instances
     */
    CARAPI MakeDefaultPhone(
        /* [in] */ IContext* context);

    CARAPI GetCdmaPhone(
        /* [in] */ Int32 phoneId,
        /* [out] */ IPhone** result);

    CARAPI GetGsmPhone(
        /* [in] */ Int32 phoneId,
        /* [out] */ IPhone** result);

    CARAPI GetDefaultPhone(
        /* [out] */ IPhone** result);

    CARAPI GetPhone(
        /* [in] */ Int32 phoneId,
        /* [out] */ IPhone** result);

    CARAPI GetPhones(
        /* [out] */ ArrayOf<IPhone*>** result);

    CARAPI GetCdmaPhone(
        /* [out] */ IPhone** result);

    CARAPI GetGsmPhone(
        /* [out] */ IPhone** result);

    CARAPI GetContext(
        /* [out] */ IContext** result);

    /**
     * Makes a {@link SipPhone} object.
     * @param sipUri the local SIP URI the phone runs on
     * @return the {@code SipPhone} object or NULL if the SIP URI is not valid
     */
    CARAPI MakeSipPhone(
        /* [in] */ const String& sipUri,
        /* [out] */ ISipPhone** result);

    /* Sets the default subscription. If only one phone instance is active that
     * subscription is set as default subscription. If both phone instances
     * are active the first instance "0" is set as default subscription
     */
    CARAPI SetDefaultSubscription(
        /* [in] */ Int32 subId);

    CARAPI CalculatePreferredNetworkType(
        /* [in] */ IContext* context,
        /* [out] */ Int32* result);

    /**
     * Returns the preferred network type that should be set in the modem.
     *
     * @param context The current {@link Context}.
     * @return the preferred network mode that should be set.
     */
    // TODO: Fix when we "properly" have TelephonyDevController/SubscriptionController ..
    CARAPI CalculatePreferredNetworkType(
        /* [in] */ IContext* context,
        /* [in] */ Int32 phoneId,
        /* [out] */ Int32* result);

    /* Gets the default subscription */
    CARAPI GetDefaultSubscription(
        /* [out] */ Int64* result);

    /* Gets User preferred Voice subscription setting*/
    CARAPI GetVoiceSubscription(
        /* [out] */ Int32* result);

    /* Returns User Prompt property,  enabed or not */
    CARAPI IsPromptEnabled(
        /* [out] */ Boolean* result);

    /*Sets User Prompt property,  enabed or not */
    CARAPI SetPromptEnabled(
        /* [in] */ Boolean enabled);

    /* Returns User SMS Prompt property,  enabled or not */
    CARAPI IsSMSPromptEnabled(
        /* [out] */ Boolean* result);

    /*Sets User SMS Prompt property,  enable or not */
    CARAPI SetSMSPromptEnabled(
        /* [in] */ Boolean enabled);

    /* Gets User preferred Data subscription setting*/
    CARAPI GetDataSubscription(
        /* [out] */ Int64* result);

    /* Gets User preferred SMS subscription setting*/
    CARAPI GetSMSSubscription(
        /* [out] */ Int32* result);

    CARAPI SetVoiceSubscription(
        /* [in] */ Int32 subId);

    CARAPI SetDataSubscription(
        /* [in] */ Int64 subId);

    CARAPI SetSMSSubscription(
        /* [in] */ Int32 subId);

    /**
     * Makes a {@link ImsPhone} object.
     * @return the {@code ImsPhone} object or NULL if the exception occured
     */
    CARAPI MakeImsPhone(
        /* [in] */ IPhoneNotifier* phoneNotifier,
        /* [in] */ IPhone* defaultPhone,
        /* [out] */ IImsPhone** result);

    CARAPI GetSubInfoRecordUpdater(
        /* [out] */ ISubInfoRecordUpdater** result);

private:
    static CARAPI InstantiateCustomRIL(
        /* [in] */ String sRILClassname,
        /* [in] */ IContext* context,
        /* [in] */ Int32 networkMode,
        /* [in] */ Int32 cdmaSubscription,
        /* [in] */ IInteger32* instanceId,
        /* [out] */ IInterface** result);

    static CARAPI_(Boolean) IsValidphoneId(
        /* [in] */ Int32 phoneId);

    static CARAPI_(Int32) GetDefaultPhoneId();

public:
    static const String LOGTAG;
    static const Int32 SOCKET_OPEN_RETRY_MILLIS;
    static const Int32 SOCKET_OPEN_MAX_RETRY;

    //***** Class Variables

    // lock sLockProxyPhones protects both sProxyPhones and sProxyPhone
    static Object sLockProxyPhones;

private:
    static AutoPtr<ArrayOf<IPhone*> > sProxyPhones;
    static AutoPtr<IPhoneProxy> sProxyPhone;

    static AutoPtr<ArrayOf<ICommandsInterface*> > sCommandsInterfaces;

    static AutoPtr<IProxyController> mProxyController;
    static AutoPtr<IUiccController> mUiccController;

    static AutoPtr<ICommandsInterface> sCommandsInterface;
    static AutoPtr<ISubInfoRecordUpdater> sSubInfoRecordUpdater;
    static AutoPtr<IModemBindingPolicyHandler> sModemBindingPolicyHandler;
    static AutoPtr<ModemStackController> sModemStackController;

    static Boolean sMadeDefaults;
    static AutoPtr<IPhoneNotifier> sPhoneNotifier;
    static AutoPtr<IContext> sContext;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEFACTORY_H__
