
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNUTILS_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::IAlarmClockInfo;
//TODO: Need IProfileManager
// using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::App::Trust::ITrustManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Object;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class LockPatternUtils
    : public Object
    , public ILockPatternUtils
{
private:
    class LockAsyncTask : public AsyncTask
    {
    public:
        LockAsyncTask(
            /* [in] */ Int32 type,
            /* [in] */ const String& password,
            /* [in] */ IInterface* service);

        virtual CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        virtual CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        Int32 mType;
        String mPassword;
        AutoPtr<IInterface> mService;
    };

public:
    CAR_INTERFACE_DECL()

    LockPatternUtils();

    CARAPI GetDevicePolicyManager(
        /* [out] */ IDevicePolicyManager** mger);

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI GetRequestedMinimumPasswordLength(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordQuality(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordHistoryLength(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumLetters(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumUpperCase(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumLowerCase(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumNumeric(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumSymbols(
        /* [out] */ Int32* result);

    virtual CARAPI GetRequestedPasswordMinimumNonLetter(
        /* [out] */ Int32* result);

    virtual CARAPI ReportFailedPasswordAttempt();

    virtual CARAPI ReportSuccessfulPasswordAttempt();

    virtual CARAPI SetCurrentUser(
        /* [in] */ Int32 userId);

    virtual CARAPI GetCurrentUser(
        /* [out] */ Int32* user);

    virtual CARAPI RemoveUser(
        /* [in] */ Int32 userId);

    virtual CARAPI CheckPattern(
        /* [in] */ IList* pattern,
        /* [out] */ Boolean* result);

    virtual CARAPI CheckPassword(
        /* [in] */ const String& password,
        /* [out] */ Boolean* result);

    /**
     * Check to see if vold already has the password.
     * Note that this also clears vold's copy of the password.
     * @return Whether the vold password matches or not.
     */
    CARAPI CheckVoldPassword(
        /* [out] */ Boolean* match);

    virtual CARAPI CheckPasswordHistory(
        /* [in] */ const String& password,
        /* [out] */ Boolean* result);

    virtual CARAPI SavedPatternExists(
        /* [out] */ Boolean* result);

    virtual CARAPI SavedPasswordExists(
        /* [out] */ Boolean* result);

    virtual CARAPI IsPatternEverChosen(
        /* [out] */ Boolean* result);

    virtual CARAPI IsBiometricWeakEverChosen(
        /* [out] */ Boolean* result);

    virtual CARAPI GetActivePasswordQuality(
        /* [out] */ Int32* result);

    virtual CARAPI ClearLock(
        /* [in] */ Boolean isFallback);

    virtual CARAPI SetLockScreenDisabled(
        /* [in] */ Boolean disable);

    virtual CARAPI IsLockScreenDisabled(
        /* [out] */ Boolean* disabled);

    virtual CARAPI DeleteTempGallery();

    virtual CARAPI DeleteGallery();

    virtual CARAPI SaveLockPattern(
        /* [in] */ IList* pattern);

    virtual CARAPI SaveLockPattern(
        /* [in] */ IList* pattern,
        /* [in] */ Boolean isFallback);

    static CARAPI_(Int32) ComputePasswordQuality(
        /* [in] */ const String& password);

    virtual CARAPI SaveLockPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 quality);

    virtual CARAPI SaveLockPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 quality,
        /* [in] */ Boolean isFallback);

    virtual CARAPI SaveLockPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 quality,
        /* [in] */ Boolean isFallback,
        /* [in] */ Int32 userHandle);

    /**
     * Gets whether the device is encrypted.
     *
     * @return Whether the device is encrypted.
     */
    static CARAPI_(Boolean) IsDeviceEncrypted();

    /**
     * Determine if the device supports encryption, even if it's set to default. This
     * differs from isDeviceEncrypted() in that it returns true even if the device is
     * encrypted with the default password.
     * @return true if device encryption is enabled
     */
    static CARAPI_(Boolean) IsDeviceEncryptionEnabled();

    /**
     * Clears the encryption password.
     */
    CARAPI ClearEncryptionPassword();

    virtual CARAPI GetKeyguardStoredPasswordQuality(
        /* [out] */ Int32* result);

    /**
     * Retrieves the quality mode for {@param userHandle}.
     * {@see DevicePolicyManager#getPasswordQuality(android.content.ComponentName)}
     *
     * @return stored password quality
     */
    CARAPI GetKeyguardStoredPasswordQuality(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* result);

    virtual CARAPI UsingBiometricWeak(
        /* [out] */ Boolean* result);

    /**
     * Deserialize a pattern.
     * @param string The pattern serialized with {@link #patternToString}
     * @return The pattern.
     */
    virtual CARAPI StringToPattern(
        /* [in] */ const String& string,
        /* [out] */ IList** list);

    /**
     * Serialize a pattern.
     * @param pattern The pattern.
     * @return The pattern in string form.
     */
    virtual CARAPI PatternToString(
        /* [in] */ IList* pattern,
        /* [out] */ String* str);

    /**
     * Serialize a pattern.
     * @param pattern The pattern.
     * @param patternGridSize the pattern size
     * @return The pattern in string form.
     */
    static CARAPI PatternToString(
        /* [in] */ IList* pattern,
        /* [in] */ Int32 patternGridSize,
        /* [out] */ String* str);

    /*
     * Generate an SHA-1 hash for the pattern. Not the most secure, but it is
     * at least a second level of protection. First level is that the file
     * is in a location only readable by the system process.
     * @param pattern the gesture pattern.
     * @return the hash of the pattern in a byte array.
     */
    virtual CARAPI PatternToHash(
        /* [in] */ IList* pattern,
        /* [out, callee] */ ArrayOf<Byte>** arr);

    CARAPI PasswordToHash(
        /* [in] */ const String& password,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Byte>** arr);

    virtual CARAPI IsLockPasswordEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI IsLockPatternEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI IsBiometricWeakInstalled(
        /* [out] */ Boolean* result);

    virtual CARAPI SetBiometricWeakLivelinessEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsBiometricWeakLivelinessEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI SetLockPatternEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsVisiblePatternEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI SetVisiblePatternEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsTactileFeedbackEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI GetLockPatternSize(
        /* [out] */ Byte* result);

    virtual CARAPI SetLockPatternSize(
        /* [in] */ Int64 size);

    virtual CARAPI SetVisibleDotsEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsVisibleDotsEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI SetShowErrorPath(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsShowErrorPath(
        /* [out] */ Boolean* result);

    virtual CARAPI SetLockoutAttemptDeadline(
        /* [out] */ Int64* result);

    virtual CARAPI GetLockoutAttemptDeadline(
        /* [out] */ Int64* result);

    virtual CARAPI IsPermanentlyLocked(
        /* [out] */ Boolean* locked);

    virtual CARAPI SetPermanentlyLocked(
        /* [in] */ Boolean locked);

    virtual CARAPI IsEmergencyCallCapable(
        /* [out] */ Boolean* result);

    virtual CARAPI IsPukUnlockScreenEnable(
        /* [out] */ Boolean* result);

    virtual CARAPI IsEmergencyCallEnabledWhileSimLocked(
        /* [out] */ Boolean* result);

    virtual CARAPI GetNextAlarm(
        /* [out] */ IAlarmClockInfo** alarm);

    virtual CARAPI GetAppWidgets(
        /* [out, callee] */ ArrayOf<Int32>** widgets);

    virtual CARAPI WriteFallbackAppWidgetId(
        /* [in] */ Int32 appWidgetId);

    virtual CARAPI GetFallbackAppWidgetId(
        /* [out] */ Int32* id);

    virtual CARAPI AddAppWidget(
        /* [in] */ Int32 widgetId,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result);

    virtual CARAPI RemoveAppWidget(
        /* [in] */ Int32 widgetId,
        /* [out] */ Boolean* result);

    virtual CARAPI IsSecure(
        /* [out] */ Boolean* result);

    virtual CARAPI GetActiveProfileLockMode(
        /* [out] */ Int32* result);

    /**
     * Sets the emergency button visibility based on isEmergencyCallCapable().
     *
     * If the emergency button is visible, sets the text on the emergency button
     * to indicate what action will be taken.
     *
     * If there's currently a call in progress, the button will take them to the call
     * @param button The button to update
     * @param shown Indicates whether the given screen wants the emergency button to show at all
     * @param showIcon Indicates whether to show a phone icon for the button.
     */
    virtual CARAPI UpdateEmergencyCallButtonState(
        /* [in] */ IButton* button,
        /* [in] */ Boolean shown,
        /* [in] */ Boolean showIcon);

    virtual CARAPI ResumeCall();

    /**
     * @return {@code true} if there is a call currently in progress, {@code false} otherwise.
     */
    CARAPI IsInCall(
        /* [out] */ Boolean* result);

    virtual CARAPI SetPowerButtonInstantlyLocks(
        /* [in] */ Boolean enabled);

    virtual CARAPI GetPowerButtonInstantlyLocks(
        /* [out] */ Boolean* locks);

    static CARAPI_(Boolean) IsSafeModeEnabled();

    CARAPI SetOwnerInfo(
        /* [in] */ const String& info,
        /* [in] */ Int32 userId);

    CARAPI SetOwnerInfoEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetOwnerInfo(
        /* [in] */ Int32 userId,
        /* [out] */ String* info);

    CARAPI IsOwnerInfoEnabled(
        /* [out] */ Boolean* enabled);

    /*
     * Returns the maximum length of a sequential characters.  A sequence is defined as
     * monotonically increasing characters with a constant interval or the same character repeated.
     *
     * For example:
     * maxLengthSequence("1234") == 4
     * maxLengthSequence("1234abc") == 4
     * maxLengthSequence("aabc") == 3
     * maxLengthSequence("qwertyuio") == 1
     * maxLengthSequence("@ABC") == 3
     * maxLengthSequence(";;;;") == 4 (anything that repeats)
     * maxLengthSequence(":;<=>") == 1  (ordered, but not composed of alphas or digits)
     *
     * @param string the pass
     * @return the number of sequential letters or digits
     */
    static CARAPI_(Int32) MaxLengthSequence(
        /* [in] */ const String& string);

    /**
     * Determine whether the user has selected any non-system widgets in keyguard
     *
     * @return true if widgets have been selected
     */
    CARAPI HasWidgetsEnabledInKeyguard(
        /* [in] */ Int32 userid,
        /* [out] */ Boolean* has);

    CARAPI GetWidgetsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI GetWidgetsEnabled(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* enabled);

    CARAPI SetWidgetsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetWidgetsEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId);

    CARAPI SetEnabledTrustAgents(
        /* [in] */ ICollection/*<ComponentName>*/* activeTrustAgents);

    CARAPI GetEnabledTrustAgents(
        /* [out] */ IList** list);

    CARAPI SetEnabledTrustAgents(
        /* [in] */ ICollection/*<ComponentName>*/* activeTrustAgents,
        /* [in] */ Int32 userId);

    CARAPI GetEnabledTrustAgents(
        /* [in] */ Int32 userId,
        /* [out] */ IList** list);

    /**
     * @see android.app.trust.TrustManager#reportRequireCredentialEntry(int)
     */
    CARAPI RequireCredentialEntry(
        /* [in] */ Int32 userId);

    CARAPI IsCredentialRequiredToDecrypt(
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    CARAPI SetCredentialRequiredToDecrypt(
        /* [in] */ Boolean required);

private:
    CARAPI_(AutoPtr<ITrustManager>) GetTrustManager();

    CARAPI_(AutoPtr<IILockSettings>) GetLockSettings();

    CARAPI_(Int32) GetCurrentOrCallingUserId();

    CARAPI_(void) UpdateEncryptionPassword(
        /* [in] */ Int32 type,
        /* [in] */ const String& password);

    CARAPI_(String) GetSalt(
        /* [in] */ Int32 userId);

    static CARAPI_(String) ToHex(
        /* [in] */ ArrayOf<Byte>* ary);

    CARAPI_(Boolean) GetBoolean(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Boolean defaultValue,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) GetBoolean(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Boolean defaultValue);

    CARAPI_(void) SetBoolean(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId);

    CARAPI_(void) SetBoolean(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Boolean enabled);

    static CARAPI_(String) CombineStrings(
        /* [in] */ ArrayOf<Int32>* list,
        /* [in] */ const String& separator);

    CARAPI_(void) WriteAppWidgets(
        /* [in] */ ArrayOf<Int32>* AppWidgetIds);

    CARAPI_(Int64) GetInt64(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Int64 defaultValue,
        /* [in] */ Int32 userHandle);

    CARAPI_(Int64) GetInt64(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Int64 defaultValue);

    CARAPI_(void) SetInt64(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Int64 value);

    CARAPI_(void) SetInt64(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userHandle);

    CARAPI_(String) GetString(
        /* [in] */ const String& secureSettingKey);

    CARAPI_(String) GetString(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) SetString(
        /* [in] */ const String& secureSettingKey,
        /* [in] */ const String& value,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) FinishBiometricWeak();

    CARAPI_(void) UpdateCryptoUserInfo();

    static CARAPI_(Int32) CategoryChar(
        /* [in] */ Char32 c);

    static CARAPI_(Int32) MaxDiffCategory(
        /* [in] */ Int32 category);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAppWidgets(
        /* [in] */ Int32 userId);

    CARAPI_(void) OnAfterChangingPassword();

    AutoPtr<IDevicePolicyManager> GetDevicePolicyManager();

    // CARAPI_(AutoPtr<ITelecomManager>) GetTelecommManager();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String LOCK_SCREEN_OWNER_INFO;
    static const String LOCK_SCREEN_OWNER_INFO_ENABLED;
    static const String ENABLED_TRUST_AGENTS;

    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IDevicePolicyManager> mDevicePolicyManager;
    AutoPtr<IILockSettings> mLockSettingsService;
//TODO: Need IProfileManager
    // AutoPtr<IProfileManager> mProfileManager;

    Boolean mMultiUserMode;

    static volatile Int32 sCurrentUserId;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_LOCKPATTERNUTILS_H__
