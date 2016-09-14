#ifndef  __ELASTOS_DROID_PHONE_SETTINGS_PHONEACCOUNTSETTINGSFRAGMENT_H__
#define  __ELASTOS_DROID_PHONE_SETTINGS_PHONEACCOUNTSETTINGSFRAGMENT_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Preference.h>
#include <Elastos.Droid.Telecom.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::PreferenceFragment;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::TeleService::Services::Telephony::Sip::SipSharedPreferences;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

class PhoneAccountSettingsFragment
    : public PreferenceFragment
{
private:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
        , public IPreferenceOnPreferenceClickListener
        , public IAccountSelectionListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ PhoneAccountSettingsFragment* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* pref,
            /* [in] */ IInterface* objValue,
            /* [out] */ Boolean* result);

        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* pref,
            /* [out] */ Boolean* result);

        CARAPI OnAccountSelected(
            /* [in] */ IAccountSelectionPreference* pref,
            /* [in] */ IPhoneAccountHandle* account,
            /* [out] */ Boolean* result);

        CARAPI OnAccountSelectionDialogShow(
            /* [in] */ IAccountSelectionPreference* pref);

        CARAPI OnAccountChanged(
            /* [in] */ IAccountSelectionPreference* pref);

    private:
        PhoneAccountSettingsFragment* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneAccountSettingsFragment::Runnable")

        MyRunnable(
            /* [in] */ PhoneAccountSettingsFragment* host,
            /* [in] */ Boolean isEnabled)
            : mHost(host)
            , mIsEnabled(isEnabled)
        {}

        CARAPI Run();

    private:
        PhoneAccountSettingsFragment* mHost;
        Boolean mIsEnabled;
    };

public:
    PhoneAccountSettingsFragment();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    /**
     * Handles changes to the preferences.
     *
     * @param pref The preference changed.
     * @param objValue The changed value.
     * @return True if the preference change has been handled, and false otherwise.
     */
    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* pref,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceClick(
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

    /**
     * Handles a phone account selection, namely when a call assistant has been selected.
     *
     * @param pref The account selection preference which triggered the account selected event.
     * @param account The account selected.
     * @return True if the account selection has been handled, and false otherwise.
     */
    //@Override
    CARAPI OnAccountSelected(
        /* [in] */ IAccountSelectionPreference* pref,
        /* [in] */ IPhoneAccountHandle* account,
        /* [out] */ Boolean* result);

    /**
     * Repopulate the dialog to pick up changes before showing.
     *
     * @param pref The account selection preference dialog being shown.
     */
    //@Override
    CARAPI OnAccountSelectionDialogShow(
        /* [in] */ IAccountSelectionPreference* pref);

    /**
     * Update the configure preference summary when the call assistant changes.
     */
    //@Override
    CARAPI OnAccountChanged(
        /* [in] */ IAccountSelectionPreference* pref);

    /**
     * Queries the telecomm manager to update the account selection preference with the list of
     * call assistants, and the currently selected call assistant.
     */
    CARAPI UpdateCallAssistantModel();

private:
    CARAPI_(void) HandleSipReceiveCallsOption(
        /* [in] */ Boolean isEnabled);

    /**
     * Queries the telcomm manager to update the default outgoing account selection preference
     * with the list of outgoing accounts and the current default outgoing account.
     */
    CARAPI_(void) UpdateDefaultOutgoingAccountsModel();

    /**
     * Updates the summary on the "configure call assistant" preference. If it is the last entry,
     * show the summary for when no call assistant is selected. Otherwise, display the currently
     * selected call assistant.
     */
    CARAPI_(void) UpdateConfigureCallAssistantSummary();

private:
    static const AutoPtr<IIntent> CONNECTION_SERVICE_CONFIGURE_INTENT;

    static const String DEFAULT_OUTGOING_ACCOUNT_KEY;

    static const String CONFIGURE_CALL_ASSISTANT_PREF_KEY;
    static const String CALL_ASSISTANT_CATEGORY_PREF_KEY;
    static const String SELECT_CALL_ASSISTANT_PREF_KEY;

    static const String SIP_SETTINGS_CATEGORY_PREF_KEY;
    static const String USE_SIP_PREF_KEY;
    static const String SIP_RECEIVE_CALLS_PREF_KEY;

    String TAG;

    AutoPtr<ITelecomManager> mTelecomManager;

    AutoPtr<IAccountSelectionPreference> mDefaultOutgoingAccount;
    AutoPtr<IAccountSelectionPreference> mSelectCallAssistant;
    AutoPtr<IPreference> mConfigureCallAssistant;

    AutoPtr<IListPreference> mUseSipCalling;
    AutoPtr<ICheckBoxPreference> mSipReceiveCallsPreference;
    AutoPtr<SipSharedPreferences> mSipSharedPreferences;
};

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_SETTINGS_PHONEACCOUNTSETTINGSFRAGMENT_H__