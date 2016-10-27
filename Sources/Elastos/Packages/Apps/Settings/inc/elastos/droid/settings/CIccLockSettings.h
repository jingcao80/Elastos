#ifndef __ELASTOS_DROID_SETTINGS_CICCLOCKSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CICCLOCKSETTINGS_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Settings_CIccLockSettings.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Implements the preference screen to enable/disable ICC lock and
 * also the dialogs to change the ICC PIN. In the former case, enabling/disabling
 * the ICC lock will prompt the user for the current PIN.
 * In the Change PIN case, it prompts the user for old pin, new pin and new pin
 * again before attempting to change it. Calls the SimCard interface to execute
 * these operations.
 *
 */
CarClass(CIccLockSettings)
    , public PreferenceActivity
{
private:
    class InnerListener
        : public Object
        , public IEditPinPreferenceOnPinEnteredListener
    {
    public:
        TO_STRING_IMPL("CIccLockSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CIccLockSettings* host);

        //@Override
        CARAPI OnPinEntered(
            /* [in] */ IEditPinPreference* preference,
            /* [in] */ Boolean positiveResult);

    private:
        CIccLockSettings* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CIccLockSettings::MyHandler")

        MyHandler(
            /* [in] */ CIccLockSettings* host);

        CARAPI constructor();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CIccLockSettings* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CIccLockSettings::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ CIccLockSettings* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CIccLockSettings* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CIccLockSettings();

    ~CIccLockSettings();

    CARAPI constructor();

    // For top-level settings screen to query
    static CARAPI_(Boolean) IsIccLockEnabled();

    static CARAPI_(String) GetSummary(
        /* [in] */ IContext* context);

    CARAPI OnPinEntered(
        /* [in] */ IEditPinPreference* preference,
        /* [in] */ Boolean positiveResult);

    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* out);

private:
    CARAPI_(void) UpdatePreferences();

    CARAPI_(void) ShowPinDialog();

    CARAPI_(void) SetDialogValues();

    CARAPI_(void) TryChangeIccLockState();

    CARAPI_(void) IccLockChanged(
        /* [in] */ Boolean success,
        /* [in] */ Int32 attemptsRemaining);

    CARAPI_(void) IccPinChanged(
        /* [in] */ Boolean success,
        /* [in] */ Int32 attemptsRemaining);

    CARAPI_(void) TryChangePin();

    CARAPI_(String) GetPinPasswordErrorMessage(
        /* [in] */ Int32 attemptsRemaining);

    CARAPI_(Boolean) ReasonablePin(
        /* [in] */ const String& pin);

    CARAPI_(void) ResetDialogState();

private:
    static const String TAG;// = "IccLockSettings";
    static const Boolean DBG;// = TRUE;

    static const Int32 OFF_MODE;// = 0;
    // State when enabling/disabling ICC lock
    static const Int32 ICC_LOCK_MODE;// = 1;
    // State when entering the old pin
    static const Int32 ICC_OLD_MODE;// = 2;
    // State when entering the new pin - first time
    static const Int32 ICC_NEW_MODE;// = 3;
    // State when entering the new pin - second time
    static const Int32 ICC_REENTER_MODE;// = 4;

    // Keys in xml file
    static const String PIN_DIALOG;// = "sim_pin";
    static const String PIN_TOGGLE;// = "sim_toggle";
    // Keys in icicle
    static const String DIALOG_STATE;// = "dialogState";
    static const String DIALOG_PIN;// = "dialogPin";
    static const String DIALOG_ERROR;// = "dialogError";
    static const String ENABLE_TO_STATE;// = "enableState";

    // Save and restore inputted PIN code when configuration changed
    // (ex. portrait<-->landscape) during change PIN code
    static const String OLD_PINCODE;// = "oldPinCode";
    static const String NEW_PINCODE;// = "newPinCode";

    static const Int32 MIN_PIN_LENGTH;// = 4;
    static const Int32 MAX_PIN_LENGTH;// = 8;
    // Which dialog to show next when popped up
    Int32 mDialogState;// = OFF_MODE;

    String mPin;
    String mOldPin;
    String mNewPin;
    String mError;
    // Are we trying to enable or disable ICC lock?
    Boolean mToState;

    AutoPtr<IPhone> mPhone;

    AutoPtr<IEditPinPreference> mPinDialog;
    AutoPtr<ICheckBoxPreference> mPinToggle;

    AutoPtr<IResources> mRes;

    // For async handler to identify request type
    static const Int32 MSG_ENABLE_ICC_PIN_COMPLETE = 100;
    static const Int32 MSG_CHANGE_ICC_PIN_COMPLETE = 101;
    static const Int32 MSG_SIM_STATE_CHANGED = 102;

    // For replies from IccCard interface
    AutoPtr<Handler> mHandler;

    AutoPtr<BroadcastReceiver> mSimStateReceiver;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CICCLOCKSETTINGS_H__