#ifndef  __ELASTOS_DROID_PHONE_CFDNSETTING_H__
#define  __ELASTOS_DROID_PHONE_CFDNSETTING_H__

#include "_Elastos_Droid_Phone_CFdnSetting.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * FDN settings UI for the Phone app.
 * Rewritten to look and behave closer to the other preferences.
 */
CarClass(CFdnSetting)
    , public PreferenceActivity
    , public IFdnSetting
    , public IEditPinPreferenceOnPinEnteredListener
    , public IDialogInterfaceOnCancelListener
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CFdnSetting::MyHandler")

        MyHandler(
            /* [in] */ CFdnSetting* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CFdnSetting* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CFdnSetting();

    CARAPI constructor();

    //@Override
    CARAPI OnPinEntered(
        /* [in] */ IEditPinPreference* preference,
        /* [in] */ Boolean positiveResult);

    /**
     * Cancel listener for the PUK2 request alert dialog.
     */
    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    /**
     * Save the state of the pin change.
     */
    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* _out);

private:
    /**
     * Attempt to toggle FDN activation.
     */
    CARAPI_(void) ToggleFDNEnable(
        /* [in] */ Boolean positiveResult);

    /**
     * Attempt to change the pin.
     */
    CARAPI_(void) UpdatePINChangeState(
        /* [in] */ Boolean positiveResult);

    /**
     * Display a toast for message, like the rest of the settings.
     */
    CARAPI_(void) DisplayMessage(
        /* [in] */ Int 32strId,
        /* [in] */ Int 32attemptsRemaining);

    CARAPI_(void) DisplayMessage(
        /* [in] */ Int 32strId);

    /**
     * The next two functions are for updating the message field on the dialog.
     */
    CARAPI_(void) DisplayPinChangeDialog();

    CARAPI_(void) DisplayPinChangeDialog(
        /* [in] */ Int32 strId,
        /* [in] */ Boolean shouldDisplay);

    /**
     * Reset the state of the pin change dialog.
     */
    CARAPI_(void) ResetPinChangeState();

    /**
     * Reset the state of the pin change dialog solely for PUK2 use.
     */
    CARAPI_(void) ResetPinChangeStateForPUK2();

    /**
     * Validate the pin entry.
     *
     * @param pin This is the pin to validate
     * @param isPuk Boolean indicating whether we are to treat
     * the pin input as a puk.
     */
    CARAPI_(Boolean) ValidatePin(
        /* [in] */ const String& pin,
        /* [in] */ Boolean isPuk);

    /**
     * Reflect the updated FDN state in the UI.
     */
    CARAPI_(void) UpdateEnableFDN();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String LOG_TAG = PhoneGlobals.LOG_TAG;
    static const Boolean DBG = FALSE;

    AutoPtr<IPhone> mPhone;

    /**
     * Events we handle.
     * The first is used for toggling FDN enable, the second for the PIN change.
     */
    static const Int32 EVENT_PIN2_ENTRY_COMPLETE = 100;
    static const Int32 EVENT_PIN2_CHANGE_COMPLETE = 200;

    // String keys for preference lookup
    // We only care about the pin preferences here, the manage FDN contacts
    // Preference is handled solely in xml.
    static const String BUTTON_FDN_ENABLE_KEY = "button_fdn_enable_key";
    static const String BUTTON_CHANGE_PIN2_KEY = "button_change_pin2_key";

    AutoPtr<IEditPinPreference> mButtonEnableFDN;
    AutoPtr<IEditPinPreference> mButtonChangePin2;

    // State variables
    String mOldPin;
    String mNewPin;
    String mPuk2;
    static const Int32 PIN_CHANGE_OLD = 0;
    static const Int32 PIN_CHANGE_NEW = 1;
    static const Int32 PIN_CHANGE_REENTER = 2;
    static const Int32 PIN_CHANGE_PUK = 3;
    static const Int32 PIN_CHANGE_NEW_PIN_FOR_PUK = 4;
    static const Int32 PIN_CHANGE_REENTER_PIN_FOR_PUK = 5;
    Int32 mPinChangeState;
    Boolean mIsPuk2Locked;    // Indicates we know that we are PUK2 blocked.

    static const String SKIP_OLD_PIN_KEY = "skip_old_pin_key";
    static const String PIN_CHANGE_STATE_KEY = "pin_change_state_key";
    static const String OLD_PIN_KEY = "old_pin_key";
    static const String NEW_PIN_KEY = "new_pin_key";
    static const String DIALOG_MESSAGE_KEY = "dialog_message_key";
    static const String DIALOG_PIN_ENTRY_KEY = "dialog_pin_entry_key";

    // size limits for the pin.
    static const Int32 MIN_PIN_LENGTH = 4;
    static const Int32 MAX_PIN_LENGTH = 8;

    /**
     * Handler for asynchronous replies from the sim.
     */
    AutoPtr<IHandler> mFDNHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CFDNSETTING_H__