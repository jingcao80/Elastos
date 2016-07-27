#ifndef __ELASTOS_DROID_SETTINGS_SECURITYSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_SECURITYSETTINGS_H__

#include "Elastos.Droid.KeyStore.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Gesture lock pattern settings.
 */
class SecuritySettings
    : public SettingsPreferenceFragment
    , public IPreferenceOnPreferenceChangeListener
    , public IDialogInterfaceOnClickListener
    , public IIndexable
{
private:
    class SecuritySearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        SecuritySearchIndexProvider();

        //@Override
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); // List<SearchIndexableResource>

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); //List<SearchIndexableRaw>

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** list); //List<String>

    protected:
        Boolean mIsPrimary;
    };

public:
    CAR_INTERFACE_DECL();

    SecuritySettings();

    ~SecuritySettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* res);

    /**
     * see confirmPatternThenDisableAndClear
     */
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* res);

    virtual CARAPI StartBiometricWeakImprove();

protected:
    //@Override
    virtual CARAPI_(Int32) GetHelpResource();

private:
    static CARAPI_(Int32) GetResIdForLockUnlockScreen(
        /* [in] */ IContext* context,
        /* [in] */ ILockPatternUtils* lockPatternUtils);

    /**
     * Important!
     *
     * Don't forget to update the SecuritySearchIndexProvider if you are doing any change in the
     * logic or adding/removing preferences here.
     */
    CARAPI_(AutoPtr<IPreferenceScreen>) CreatePreferenceHierarchy();

    /*ArrayList<TrustAgentComponentInfo>*/
    static CARAPI_(AutoPtr<IArrayList>) GetActiveTrustAgents(
        /* [in] */ IPackageManager* pm,
        /* [in] */ ILockPatternUtils* utils);

    CARAPI_(Boolean) IsNonMarketAppsAllowed();

    CARAPI_(void) SetNonMarketAppsAllowed(
        /* [in] */ Boolean enabled);

    CARAPI_(void) WarnAppInstallation();

    CARAPI_(void) SetupLockAfterPreference();

    CARAPI_(void) UpdateLockAfterPreferenceSummary();

    CARAPI_(void) DisableUnusableTimeouts(
        /* [in] */ Int64 maxTimeout);

public:
    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();
    static const String TAG;

private:
    static const CARAPI_(AutoPtr<ArrayOf<String> >) InitSWITCH_PREFERENCE_KEYS();

private:
    /**
     * For Search. Please keep it in sync when updating "CreatePreferenceHierarchy()"
     */
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TRUST_AGENT_CLICK_INTENT;
    static const AutoPtr<IIntent> TRUST_AGENT_INTENT;

    // Lock Settings
    static const String KEY_UNLOCK_SET_OR_CHANGE;
    static const String KEY_BIOMETRIC_WEAK_IMPROVE_MATCHING;
    static const String KEY_BIOMETRIC_WEAK_LIVELINESS;
    static const String KEY_LOCK_ENABLED;
    static const String KEY_VISIBLE_PATTERN;
    static const String KEY_SECURITY_CATEGORY;
    static const String KEY_DEVICE_ADMIN_CATEGORY;
    static const String KEY_LOCK_AFTER_TIMEOUT;
    static const String KEY_OWNER_INFO_SETTINGS;
    static const String KEY_ADVANCED_SECURITY;
    static const String KEY_MANAGE_TRUST_AGENTS;

    static const Int32 SET_OR_CHANGE_LOCK_METHOD_REQUEST;
    static const Int32 CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST;
    static const Int32 CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF;
    static const Int32 CHANGE_TRUST_AGENT_SETTINGS;

    // Misc Settings
    static const String KEY_SIM_LOCK;
    static const String KEY_SHOW_PASSWORD;
    static const String KEY_CREDENTIAL_STORAGE_TYPE;
    static const String KEY_RESET_CREDENTIALS;
    static const String KEY_CREDENTIALS_INSTALL;
    static const String KEY_TOGGLE_INSTALL_APPLICATIONS;
    static const String KEY_POWER_INSTANTLY_LOCKS;
    static const String KEY_CREDENTIALS_MANAGER;
    static const String PACKAGE_MIME_TYPE;
    static const String KEY_TRUST_AGENT;
    static const String KEY_SCREEN_PINNING;

    // These switch preferences need special handling since they're not all stored in Settings.
    static const AutoPtr<ArrayOf<String> > SWITCH_PREFERENCE_KEYS;

    // Only allow one trust agent on the platform.
    static const Boolean ONLY_ONE_TRUST_AGENT;

    AutoPtr<IDevicePolicyManager> mDPM;

    AutoPtr<ChooseLockSettingsHelper> mChooseLockSettingsHelper;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IListPreference> mLockAfter;

    AutoPtr<ISwitchPreference> mBiometricWeakLiveliness;
    AutoPtr<ISwitchPreference> mVisiblePattern;

    AutoPtr<ISwitchPreference> mShowPassword;

    AutoPtr<IKeyStore> mKeyStore;
    AutoPtr<IPreference> mResetCredentials;

    AutoPtr<ISwitchPreference> mToggleAppInstallation;
    AutoPtr<IDialogInterface> mWarnInstallApps;
    AutoPtr<ISwitchPreference> mPowerButtonInstantlyLocks;

    Boolean mIsPrimary;

    AutoPtr<IIntent> mTrustAgentClickIntent;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SECURITYSETTINGS_H__