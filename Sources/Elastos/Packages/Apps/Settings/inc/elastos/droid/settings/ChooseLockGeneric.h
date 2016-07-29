#ifndef __ELASTOS_DROID_SETTINGS_CHOOSELOCKGENERIC_H__
#define __ELASTOS_DROID_SETTINGS_CHOOSELOCKGENERIC_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::IChooseLockGenericInternalActivity;

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
// using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace Settings {

class ChooseLockGeneric
    : public SettingsActivity
{
public:
    class ChooseLockGenericFragment
        : public SettingsPreferenceFragment
    {
    public:
        ChooseLockGenericFragment();

        ~ChooseLockGenericFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

        //@Override
        CARAPI OnResume();

        //@Override
        CARAPI OnPreferenceTreeClick(
            /* [in] */ IPreferenceScreen* preferenceScreen,
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** view);

        //@Override
        CARAPI OnActivityResult(
            /* [in] */ Int32 requestCode,
            /* [in] */ Int32 resultCode,
            /* [in] */ IIntent* data);

        //@Override
        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

    protected:
        /**
         * Invokes an activity to change the user's pattern, password or PIN based on given quality
         * and minimum quality specified by DevicePolicyManager. If quality is
         * {@link DevicePolicyManager#PASSWORD_QUALITY_UNSPECIFIED}, password is cleared.
         *
         * @param quality the desired quality. Ignored if DevicePolicyManager requires more security
         * @param disabled whether or not to show LockScreen at all. Only meaningful when quality is
         * {@link DevicePolicyManager#PASSWORD_QUALITY_UNSPECIFIED}
         */
        virtual CARAPI UpdateUnlockMethodAndFinish(
            /* [in] */ Int32 quality,
            /* [in] */ Boolean disabled);

        //@Override
        virtual CARAPI_(Int32) GetHelpResource();

    private:
        /**
         * If the device has encryption already enabled, then ask the user if they
         * also want to encrypt the phone with this password.
         *
         * @param quality
         * @param disabled
         */
        CARAPI_(void) MaybeEnableEncryption(
            /* [in] */ Int32 quality,
            /* [in] */ Boolean disabled);

        CARAPI_(void) UpdatePreferencesOrFinish();

        /** increases the quality if necessary, and returns whether biometric is allowed */
        CARAPI_(Int32) UpgradeQuality(
            /* [in] */ Int32 quality,
            /* [in] */ IBoolean* allowBiometric);

        CARAPI_(Int32) UpgradeQualityForDPM(
            /* [in] */ Int32 quality);

        CARAPI_(Int32) UpgradeQualityForKeyStore(
            /* [in] */ Int32 quality);

        /***
         * Disables preferences that are less secure than required quality.
         *
         * @param quality the requested quality.
         */
        CARAPI_(void) DisableUnusablePreferences(
            /* [in] */ Int32 quality,
            /* [in] */ IBoolean* allowBiometric);

        CARAPI_(void) UpdatePreferenceSummaryIfNeeded();

        /**
         * Check whether the key is allowed for fallback (e.g. bio sensor). Returns TRUE if it's
         * supported as a backup.
         *
         * @param key
         * @return TRUE if allowed
         */
        CARAPI_(Boolean) AllowedForFallback(
            /* [in] */ const String& key);

        CARAPI_(AutoPtr<IIntent>) GetBiometricSensorIntent();

    public:
        static const String MINIMUM_QUALITY_KEY;
        static const String ENCRYPT_REQUESTED_QUALITY;
        static const String ENCRYPT_REQUESTED_DISABLED;

    private:
        static const Int32 MIN_PASSWORD_LENGTH;
        static const String KEY_UNLOCK_BACKUP_INFO;
        static const String KEY_UNLOCK_SET_OFF;
        static const String KEY_UNLOCK_SET_NONE;
        static const String KEY_UNLOCK_SET_BIOMETRIC_WEAK;
        static const String KEY_UNLOCK_SET_PIN;// = "unlock_set_pin";
        static const String KEY_UNLOCK_SET_PASSWORD;// = "unlock_set_password";
        static const String KEY_UNLOCK_SET_PATTERN;// = "unlock_set_pattern";
        static const Int32 CONFIRM_EXISTING_REQUEST;
        static const Int32 FALLBACK_REQUEST;
        static const Int32 ENABLE_ENCRYPTION_REQUEST;
        static const String PASSWORD_CONFIRMED;

        static const String WAITING_FOR_CONFIRMATION;
        static const String FINISH_PENDING;

        static const Boolean ALWAY_SHOW_TUTORIAL;

        AutoPtr<ChooseLockSettingsHelper> mChooseLockSettingsHelper;
        AutoPtr<IDevicePolicyManager> mDPM;
        AutoPtr<IKeyStore> mKeyStore;
        Boolean mPasswordConfirmed;
        Boolean mWaitingForConfirmation;
        Boolean mFinishPending;
        Int32 mEncryptionRequestQuality;
        Boolean mEncryptionRequestDisabled;
        Boolean mRequirePassword;
        AutoPtr<ILockPatternUtils> mLockPatternUtils;
    };

public:
    ChooseLockGeneric();

    ~ChooseLockGeneric();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    static const String CONFIRM_CREDENTIALS;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

namespace Elastos {
namespace Droid {
namespace Settings {

class InternalActivity
    : public ChooseLockGeneric
    , public IChooseLockGenericInternalActivity
{
public:
    CAR_INTERFACE_DECL()

    InternalActivity();

    ~InternalActivity();

    CARAPI constructor();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CHOOSELOCKGENERIC_H__