#ifndef  __ELASTOS_DROID_PHONE_CPHONEAPP_H__
#define  __ELASTOS_DROID_PHONE_CPHONEAPP_H__

#include "_Elastos_Droid_Phone_CCallFeaturesSetting.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include <elastos/droid/os/AsyncResult.h>
#include <elastos/droid/preference/PreferenceActivity.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Phone::Settings::IAccountSelectionPreference;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Top level "Call settings" UI; see res/xml/call_feature_setting.xml
 *
 * This preference screen is the root of the "Call settings" hierarchy available from the Phone
 * app; the settings here let you control various features related to phone calls (including
 * voicemail settings, the "Respond via SMS" feature, and others.)  It's used only on
 * voice-capable phone devices.
 *
 * Note that this activity is part of the package com.android.phone, even
 * though you reach it from the "Phone" app (i.e. DialtactsActivity) which
 * is from the package com.android.contacts.
 *
 * For the "Mobile network settings" screen under the main Settings app,
 * See {@link MobileNetworkSettings}.
 *
 * TODO: Settings should be split into PreferenceFragments where possible (ie. voicemail).
 *
 * @see com.android.phone.MobileNetworkSettings
 */
CarClass(CCallFeaturesSetting)
    , public PreferenceActivity
    , public ICallFeaturesSetting
    , public IDialogInterfaceOnClickListener
    , public IPreferenceOnPreferenceChangeListener
    , public IEditPhoneNumberPreferenceOnDialogClosedListener
    , public IEditPhoneNumberPreferenceGetDefaultNumberListener
{
private:
    class VoiceMailProvider
        : public Object
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::VoiceMailProvider")

        VoiceMailProvider(
            /* [in] */ const String& name,
            /* [in] */ IIntent* intent)
            : mName(name)
            , mIntent(intent)
        {}

    public:
        String mName;
        AutoPtr<IIntent> mIntent;
    };

    class VoiceMailProviderSettings
        : public Object
        , public IVoiceMailProviderSettings
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Constructs settings object, setting all conditional forwarding to the specified number
         */
        VoiceMailProviderSettings(
            /* [in] */ const String& voicemailNumber,
            /* [in] */ const String& forwardingNumber,
            /* [in] */ Int32 timeSeconds);

        VoiceMailProviderSettings(
            /* [in] */ const String& voicemailNumber,
            /* [in] */ ArrayOf<ICallForwardInfo*>* infos);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mVoicemailNumber;
        AutoPtr<ArrayOf<ICallForwardInfo*> > mForwardingSettings;

    private:
        CARAPI_(Boolean) ForwardingSettingsEqual(
            /* [in] */ ArrayOf<ICallForwardInfo*>* infos1,
            /* [in] */ ArrayOf<ICallForwardInfo*>* infos2);
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::MyHandler")

        MyHandler(
            /* [in] */ CCallFeaturesSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCallFeaturesSetting* mHost;
    };

    class MyHandler2
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::MyHandler2")

        MyHandler2(
            /* [in] */ CCallFeaturesSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCallFeaturesSetting* mHost;
    };

    class MyHandler3
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::MyHandler3")

        MyHandler3(
            /* [in] */ CCallFeaturesSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCallFeaturesSetting* mHost;
    };

    class MyHandler4
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::MyHandler4")

        MyHandler4(
            /* [in] */ CCallFeaturesSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCallFeaturesSetting* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CCallFeaturesSetting::MyRunnable")

        MyRunnable(
            /* [in] */ CCallFeaturesSetting* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CCallFeaturesSetting* mHost;
    };

    class PreferenceOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        PreferenceOnPreferenceChangeListener(
            /* [in] */ CCallFeaturesSetting* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* objValue,
            /* [out] */ Boolean* result);

    private:
        CCallFeaturesSetting* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCallFeaturesSetting();

    CARAPI constructor();

    //@Override
    CARAPI OnPause();

    // Click listener for all toggle events
    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    /**
     * Implemented to support onPreferenceChangeListener to look for preference
     * changes.
     *
     * @param preference is the preference to be changed
     * @param objValue should be the value of the selection, NOT its localized
     * display value.
     */
    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ IEditPhoneNumberPreference* preference,
        /* [in] */ Int32 buttonClicked);

    /**
     * Implemented for EditPhoneNumberPreference.GetDefaultNumberListener.
     * This method set the default values for the various
     * EditPhoneNumberPreference dialogs.
     */
    //@Override
    CARAPI OnGetDefaultNumber(
        /* [in] */ IEditPhoneNumberPreference* preference,
        /* [out] */ String* str);

    // override the startsubactivity call to make changes in state consistent.
    //@Override
    CARAPI StartActivityForResult(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    // This is a method implemented for DialogInterface.OnClickListener.
    // Used with the error dialog to close the app, voicemail dialog to just dismiss.
    // Close button is mapped to BUTTON_POSITIVE for the errors that close the activity,
    // while those that are mapped to BUTTON_NEUTRAL only move the preference focus.
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    // Migrate settings from BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_WHEN_KEY to
    // BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, if the latter does not exist.
    // Returns true if migration was performed.
    static CARAPI MigrateVoicemailVibrationSettingsIfNeeded(
        /* [in] */ ISharedPreferences* prefs,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    /**
     * Finish current Activity and go up to the top level Settings ({@link CallFeaturesSetting}).
     * This is useful for implementing "HomeAsUp" capability for second-level Settings.
     */
    static CARAPI GoUpToTopLevelSetting(
        /* [in] */ IActivity* activity);

protected:
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /*
     * Helper Methods for Activity class.
     * The initial query commands are split into two pieces now
     * for individual expansion.  This combined with the ability
     * to cancel queries allows for a much better user experience,
     * and also ensures that the user only waits to update the
     * data that is relevant.
     */
    //@Override
    CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog);

    // dialog creation method, called by showDialog()
    //@Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

    /*
     * Activity class methods
     */

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

private:
    CARAPI_(void) SwitchToPreviousVoicemailProvider();

    CARAPI_(void) OnRevertDone();

    // Voicemail button logic
    CARAPI_(void) HandleVMBtnClickRequest();

    /**
     * Wrapper around showDialog() that will silently do nothing if we're
     * not in the foreground.
     *
     * This is useful here because most of the dialogs we display from
     * this class are triggered by asynchronous events (like
     * success/failure messages from the telephony layer) and it's
     * possible for those events to come in even after the user has gone
     * to a different screen.
     */
    // TODO: this is too brittle: it's still easy to accidentally add new
    // code here that calls showDialog() directly (which will result in a
    // WindowManager$BadTokenException if called after the activity has
    // been stopped.)
    //
    // It would be cleaner to do the "if (mForeground)" check in one
    // central place, maybe by using a single Handler for all asynchronous
    // events (and have *that* discard events if we're not in the
    // foreground.)
    //
    // Unfortunately it's not that simple, since we sometimes need to do
    // actual work to handle these events whether or not we're in the
    // foreground (see the Handler code in mSetOptionComplete for
    // example.)
    CARAPI_(void) ShowDialogIfForeground(
        /* [in] */ Int32 id);

    CARAPI_(void) DismissDialogSafely(
        /* [in] */ Int32 id);

    CARAPI_(void) SaveVoiceMailAndForwardingNumber(
        /* [in] */ const String& key,
        /* [in] */ VoiceMailProviderSettings* newSettings);

    CARAPI_(void) HandleForwardingSettingsReadResult(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 idx);

    CARAPI_(AutoPtr<ICallForwardInfo>) InfoForReason(
        /* [in] */ ArrayOf<ICallForwardInfo*>* infos,
        /* [in] */ Int32 reason);

    CARAPI_(Boolean) IsUpdateRequired(
        /* [in] */ ICallForwardInfo* oldInfo,
        /* [in] */ ICallForwardInfo* newInfo);

    CARAPI_(void) ResetForwardingChangeState();

    // Called after we are done saving the previous forwarding settings if
    // we needed.
    CARAPI_(void) SaveVoiceMailAndForwardingNumberStage2();

    CARAPI_(void) SetVMNumberWithCarrier();

    /**
     * @return true if forwarding change has completed
     */
    CARAPI_(Boolean) CheckForwardingCompleted();

    /**
     * @return error string or null if successful
     */
    CARAPI_(String) CheckFwdChangeSuccess();

    /**
     * @return error string or null if successful
     */
    CARAPI_(String) CheckVMChangeSuccess();

    CARAPI_(void) HandleSetVMOrFwdMessage();

    /**
     * Called when Voicemail Provider or its forwarding settings failed. Rolls back partly made
     * changes to those settings and show "failure" dialog.
     *
     * @param msgId Message ID used for the specific error case. {@link #MSG_FW_SET_EXCEPTION} or
     * {@link #MSG_VM_EXCEPTION}
     */
    CARAPI_(void) HandleVMOrFwdSetError(
        /* [in] */ Int32 msgId);

    /**
     * Called when Voicemail Provider and its forwarding settings were successfully finished.
     * This updates a bunch of variables and show "success" dialog.
     */
    CARAPI_(void) HandleVMAndFwdSetSuccess(
        /* [in] */ Int32 msg);

    /**
     * Update the voicemail number from what we've recorded on the sim.
     */
    CARAPI_(void) UpdateVoiceNumberField();

    // set the app state with optional status.
    CARAPI_(void) ShowVMDialog(
        /* [in] */ Int32 msgStatus);

    CARAPI_(void) InitPhoneAccountPreferences();

    CARAPI_(Boolean) CanLaunchIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsAirplaneModeOn();

    CARAPI_(void) HandleTTYChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue);

    CARAPI_(void) UpdatePreferredTtyModeSummary(
        /* [in] */ Int32 TtyMode);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    /**
     * Updates the look of the VM preference widgets based on current VM provider settings.
     * Note that the provider name is loaded form the found activity via loadLabel in
     * {@link #initVoiceMailProviders()} in order for it to be localizable.
     */
    CARAPI_(void) UpdateVMPreferenceWidgets(
        /* [in] */ const String& currentProviderSetting);

    /**
     * Enumerates existing VM providers and puts their data into the list and populates
     * the preference list objects with their names.
     * In case we are called with ACTION_ADD_VOICEMAIL intent the intent may have
     * an extra string called IGNORE_PROVIDER_EXTRA with "package.activityName" of the provider
     * which should be hidden when we bring up the list of possible VM providers to choose.
     */
    CARAPI_(void) InitVoiceMailProviders();

    CARAPI_(String) MakeKeyForActivity(
        /* [in] */ IActivityInfo* ai);

    /**
     * Simulates user clicking on a passed preference.
     * Usually needed when the preference is a dialog preference and we want to invoke
     * a dialog for this preference programmatically.
     * TODO: figure out if there is a cleaner way to cause preference dlg to come up
     */
    CARAPI_(void) SimulatePreferenceClick(
        /* [in] */ IPreference* preference);

    /**
     * Saves new VM provider settings associating them with the currently selected
     * provider if settings are different than the ones already stored for this
     * provider.
     * Later on these will be used when the user switches a provider.
     */
    CARAPI_(void) MaybeSaveSettingsForVoicemailProvider(
        /* [in] */ const String& key,
        /* [in] */ VoiceMailProviderSettings* newSettings);

    /**
     * Returns settings previously stored for the currently selected
     * voice mail provider. If none is stored returns null.
     * If the user switches to a voice mail provider and we have settings
     * stored for it we will automatically change the phone's voice mail number
     * and forwarding number to the stored one. Otherwise we will bring up provider's configuration
     * UI.
     */
    CARAPI_(AutoPtr<VoiceMailProviderSettings>) LoadSettingsForVoiceMailProvider(
        /* [in] */ const String& key);

    /**
     * Deletes settings for the specified provider.
     */
    CARAPI_(void) DeleteSettingsForVoicemailProvider(
        /* [in] */ const String& key);

    CARAPI_(String) GetCurrentVoicemailProviderKey();

public:
    // Used to tell the saving logic to leave forwarding number as is
    static AutoPtr<ArrayOf<ICallForwardInfo*> > FWD_SETTINGS_DONT_TOUCH;

private:
    static const String TAG;
    static const Boolean DBG;

    //Information about logical "up" Activity
    static const String UP_ACTIVITY_PACKAGE;
    static const String UP_ACTIVITY_CLASS;

    // string constants
    static const AutoPtr<ArrayOf<String> > NUM_PROJECTION;

    // String keys for preference lookup
    // TODO: Naming these "BUTTON_*" is confusing since they're not actually buttons(!)
    static const String VOICEMAIL_SETTING_SCREEN_PREF_KEY;
    static const String BUTTON_VOICEMAIL_KEY;
    static const String BUTTON_VOICEMAIL_PROVIDER_KEY;
    static const String BUTTON_VOICEMAIL_SETTING_KEY;
    // New preference key for voicemail notification vibration
    static const String BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY;
    // Old preference key for voicemail notification vibration. Used for migration to the new
    // preference key only.
    static const String BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_WHEN_KEY;
    static const String BUTTON_VOICEMAIL_NOTIFICATION_RINGTONE_KEY;
    static const String BUTTON_FDN_KEY;

    static const String BUTTON_DTMF_KEY;
    static const String BUTTON_RETRY_KEY;
    static const String BUTTON_TTY_KEY;
    static const String BUTTON_HAC_KEY;

    static const String BUTTON_GSM_UMTS_OPTIONS;
    static const String BUTTON_CDMA_OPTIONS;

    static const String VM_NUMBERS_SHARED_PREFERENCES_NAME;

    static const String DEFAULT_OUTGOING_ACCOUNT_KEY;
    static const String PHONE_ACCOUNT_SETTINGS_KEY;

    AutoPtr<IIntent> mContactListIntent;

    /** Event for Async voicemail change call */
    static const Int32 EVENT_VOICEMAIL_CHANGED;
    static const Int32 EVENT_FORWARDING_CHANGED;
    static const Int32 EVENT_FORWARDING_GET_COMPLETED;

    static const Int32 MSG_UPDATE_VOICEMAIL_RINGTONE_SUMMARY;

    /** Handle to voicemail pref */
    static const Int32 VOICEMAIL_PREF_ID;
    static const Int32 VOICEMAIL_PROVIDER_CFG_ID;

    AutoPtr<IPhone> mPhone;

    AutoPtr<IAudioManager> mAudioManager;

    static const Int32 VM_NOCHANGE_ERROR;
    static const Int32 VM_RESPONSE_ERROR;
    static const Int32 FW_SET_RESPONSE_ERROR;
    static const Int32 FW_GET_RESPONSE_ERROR;


    // dialog identifiers for voicemail
    static const Int32 VOICEMAIL_DIALOG_CONFIRM;
    static const Int32 VOICEMAIL_FWD_SAVING_DIALOG;
    static const Int32 VOICEMAIL_FWD_READING_DIALOG;
    static const Int32 VOICEMAIL_REVERTING_DIALOG;

    // status message sent back from handlers
    static const Int32 MSG_OK;

    // special statuses for voicemail controls.
    static const Int32 MSG_VM_EXCEPTION;
    static const Int32 MSG_FW_SET_EXCEPTION;
    static const Int32 MSG_FW_GET_EXCEPTION;
    static const Int32 MSG_VM_OK;
    static const Int32 MSG_VM_NOCHANGE;

    // voicemail notification vibration string constants
    static const String VOICEMAIL_VIBRATION_ALWAYS;
    static const String VOICEMAIL_VIBRATION_NEVER;

    AutoPtr<IEditPhoneNumberPreference> mSubMenuVoicemailSettings;

    AutoPtr<IRunnable> mVoicemailRingtoneLookupRunnable;
    AutoPtr<IHandler> mVoicemailRingtoneLookupComplete;

    /** Whether dialpad plays DTMF tone or not. */
    AutoPtr<ICheckBoxPreference> mButtonAutoRetry;
    AutoPtr<ICheckBoxPreference> mButtonHAC;
    AutoPtr<IListPreference> mButtonDTMF;
    AutoPtr<IListPreference> mButtonTTY;
    AutoPtr<IPreference> mPhoneAccountSettingsPreference;
    AutoPtr<IListPreference> mVoicemailProviders;
    AutoPtr<IPreferenceScreen> mVoicemailSettingsScreen;
    AutoPtr<IPreferenceScreen> mVoicemailSettings;
    AutoPtr<IPreference> mVoicemailNotificationRingtone;
    AutoPtr<ICheckBoxPreference> mVoicemailNotificationVibrate;
    AutoPtr<IAccountSelectionPreference> mDefaultOutgoingAccount;

    /**
     * Forwarding settings we are going to save.
     */
    static AutoPtr<ArrayOf<Int32> > FORWARDING_SETTINGS_REASONS;

    AutoPtr<ISharedPreferences> mPerProviderSavedVMNumbers;

    /**
     * Results of reading forwarding settings
     */
    AutoPtr<ArrayOf<ICallForwardInfo*> > mForwardingReadResults;

    /**
     * Result of forwarding number change.
     * Keys are reasons (eg. unconditional forwarding).
     */
    AutoPtr<HashMap<Int32, AutoPtr<AsyncResult> > > mForwardingChangeResults;

    /**
     * Expected CF read result types.
     * This set keeps track of the CF types for which we've issued change
     * commands so we can tell when we've received all of the responses.
     */
    AutoPtr<HashSet<Int32> > mExpectedChangeResultReasons;

    /**
     * Result of vm number change
     */
    AutoPtr<AsyncResult> mVoicemailChangeResult;

    /**
     * Previous VM provider setting so we can return to it in case of failure.
     */
    String mPreviousVMProviderKey;

    /**
     * Id of the dialog being currently shown.
     */
    Int32 mCurrentDialogId;

    /**
     * Flag indicating that we are invoking settings for the voicemail provider programmatically
     * due to vm provider change.
     */
    Boolean mVMProviderSettingsForced;

    /**
     * Flag indicating that we are making changes to vm or fwd numbers
     * due to vm provider change.
     */
    Boolean mChangingVMorFwdDueToProviderChange;

    /**
     * True if we are in the process of vm & fwd number change and vm has already been changed.
     * This is used to decide what to do in case of rollback.
     */
    Boolean mVMChangeCompletedSuccessfully;

    /**
     * True if we had full or partial failure setting forwarding numbers and so need to roll them
     * back.
     */
    Boolean mFwdChangesRequireRollback;

    /**
     * Id of error msg to display to user once we are done reverting the VM provider to the previous
     * one.
     */
    Int32 mVMOrFwdSetError;

    /**
     * Data about discovered voice mail settings providers.
     * Is populated by querying which activities can handle ACTION_CONFIGURE_VOICEMAIL.
     * They key in this map is package name + activity name.
     * We always add an entry for the default provider with a key of empty
     * string and intent value of null.
     * @see #initVoiceMailProviders()
     */
    HashMap<String, AutoPtr<VoiceMailProvider> > mVMProvidersData;

    /** string to hold old voicemail number as it is being updated. */
    String mOldVmNumber;

    // New call forwarding settings and vm number we will be setting
    // Need to save these since before we get to saving we need to asynchronously
    // query the existing forwarding settings.
    AutoPtr<ArrayOf<ICallForwardInfo*> > mNewFwdSettings;
    String mNewVMNumber;

    Boolean mForeground;

    /**
     * We have to pull current settings from the network for all kinds of
     * voicemail providers so we can tell whether we have to update them,
     * so use this bit to keep track of whether we're reading settings for the
     * default provider and should therefore save them out when done.
     */
    Boolean mReadingSettingsForDefaultProvider;

    /**
     * Used to indicate that the voicemail preference should be shown.
     */
    Boolean mShowVoicemailPreference;

    AutoPtr<IHandler> mGetOptionComplete;

    /**
     * Callback to handle option update completions
     */
    AutoPtr<IHandler> mSetOptionComplete;

    /**
     * Callback to handle option revert completions
     */
    AutoPtr<IHandler> mRevertOptionComplete;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEAPP_H__