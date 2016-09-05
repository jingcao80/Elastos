#ifndef  __ELASTOS_DROID_PHONE_CEDITPHONENUMBERPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CEDITPHONENUMBERPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CEditPhoneNumberPreference.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/EditTextPreference.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Preference::EditTextPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CEditPhoneNumberPreference)
    , public EditTextPreference
    , public IEditPhoneNumberPreference
{
private:
    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEditPhoneNumberPreference::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CEditPhoneNumberPreference* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEditPhoneNumberPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEditPhoneNumberPreference();

    /*
     * Constructors
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /*
     * Listeners and other state setting methods
     */
    //set the on focus change listener to be assigned to the Dialog's edittext field.
    CARAPI SetDialogOnFocusChangeListener(
        /* [in] */ IViewOnFocusChangeListener* l);

    //set the listener to be called wht the dialog is closed.
    CARAPI SetDialogOnClosedListener(
        /* [in] */ IEditPhoneNumberPreferenceOnDialogClosedListener* l);

    //set the link back to the parent activity, so that we may run the contact picker.
    CARAPI SetParentActivity(
        /* [in] */ IActivity* parent,
        /* [in] */ Int32 identifier);

    //set the link back to the parent activity, so that we may run the contact picker.
    //also set the default number listener.
    CARAPI SetParentActivity(
        /* [in] */ IActivity* parent,
        /* [in] */ Int32 identifier,
        /* [in] */ IEditPhoneNumberPreferenceGetDefaultNumberListener* l);

    /*
     * Notification handlers
     */
    //Notify the preference that the pick activity is complete.
    CARAPI OnPickActivityResult(
        /* [in] */ const String& pickedValue);

    //called when the dialog is clicked.
    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    /*
     * Toggle handling code.
     */
    //return the toggle value.
    CARAPI IsToggled(
        /* [out] */ Boolean* result);

    //set the toggle value.
    // return the current preference to allow for chaining preferences.
    CARAPI SetToggled(
        /* [in] */ Boolean checked);

    /**
     * Phone number handling code
     */
    CARAPI GetPhoneNumber(
        /* [out] */ String* number);

    //set the phone number value.
    // return the current preference to allow for chaining preferences.
    CARAPI SetPhoneNumber(
        /* [in] */ const String& number);

    /**
     * Decides how to disable dependents.
     */
    //@Override
    CARAPI ShouldDisableDependents(
        /* [out] */ Boolean* result);

    /*
     * Summary On handling code
     */
    //set the Summary for the on state (relevant only in CM_ACTIVATION mode)
    CARAPI SetSummaryOn(
        /* [in] */ ICharSequence* summary);

    //set the Summary for the on state, given a string resource id
    // (relevant only in CM_ACTIVATION mode)
    CARAPI SetSummaryOn(
        /* [in] */ Int32 summaryResId);

    //get the summary string for the on state
    CARAPI GetSummaryOn(
        /* [out] */ ICharSequence** result);

    /*
     * Summary Off handling code
     */
    //set the Summary for the off state (relevant only in CM_ACTIVATION mode)
    CARAPI SetSummaryOff(
        /* [in] */ ICharSequence* summary);

    //set the Summary for the off state, given a string resource id
    // (relevant only in CM_ACTIVATION mode)
    CARAPI SetSummaryOff(
        /* [in] */ Int32 summaryResId);

    //get the summary string for the off state
    CARAPI GetSummaryOff(
        /* [out] */ ICharSequence** result);

    CARAPI ShowPhoneNumberDialog();

protected:
    /*
     * Methods called on UI bindings
     */
    //@Override
    //called when we're binding the view to the preference.
    CARAPI OnBindView(
        /* [in] */ IView* view);

    //called when we're binding the dialog to the preference's view.
    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    /**
     * Overriding EditTextPreference's onAddEditTextToDialogView.
     *
     * This method attaches the EditText to the container specific to this
     * preference's dialog layout.
     */
    //@Override
    CARAPI OnAddEditTextToDialogView(
        /* [in] */ IView* dialogView,
        /* [in] */ IEditText* editText);

    //control the appearance of the dialog depending upon the mode.
    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    //@Override
    //When the dialog is closed, perform the relevant actions, including setting
    // phone numbers and calling the close action listener.
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    /** The phone number including any formatting characters */
    CARAPI GetRawPhoneNumber(
        /* [out] */ String* number);

    /*
     * Other code relevant to preference framework
     */
    //when setting default / initial values, make sure we're setting things correctly.
    //@Override
    CARAPI OnSetInitialValue(
        /* [in] */ Boolean restoreValue,
        /* [in] */ IInterface* defaultValue);

    //@Override
    CARAPI PersistString(
        /* [in] */ const String& value,
        /* [out] */ Boolean* str);

    /*
     * Methods to get and set from encoded strings.
     */
    //set the values given an encoded string.
    CARAPI SetValueFromString(
        /* [in] */ const String& value);

    //retrieve the state of this preference in the form of an encoded string
    CARAPI GetStringValue(
        /* [out] */ String* value);

private:
    //allowed modes for this preference.
    /** simple confirmation (OK / CANCEL) */
    static const Int32 CM_CONFIRM;
    /** toggle [(ENABLE / CANCEL) or (DISABLE / CANCEL)], use isToggled() to see requested state.*/
    static const Int32 CM_ACTIVATION;

    Int32 mConfirmationMode;

    //String constants used in storing the value of the preference
    // The preference is backed by a string that holds the encoded value, which reads:
    //  <VALUE_ON | VALUE_OFF><VALUE_SEPARATOR><mPhoneNumber>
    // for example, an enabled preference with a number of 6502345678 would read:
    //  "1:6502345678"
    static const String VALUE_SEPARATOR;
    static const String VALUE_OFF;
    static const String VALUE_ON;

    //UI layout
    AutoPtr<IImageButton> mContactPickButton;

    //Listeners
    /** Called when focus is changed between fields */
    AutoPtr<IViewOnFocusChangeListener> mDialogFocusChangeListener;
    /** Called when the Dialog is closed. */
    AutoPtr<IEditPhoneNumberPreferenceOnDialogClosedListener> mDialogOnClosedListener;
    /**
     * Used to indicate that we are going to request for a
     * default number. for the dialog.
     */
    AutoPtr<IEditPhoneNumberPreferenceGetDefaultNumberListener> mGetDefaultNumberListener;

    //Activity values
    IActivity* mParentActivity;
    AutoPtr<IIntent> mContactListIntent;
    /** Arbitrary activity-assigned preference id value */
    Int32 mPrefId;

    //similar to toggle preference
    AutoPtr<ICharSequence> mEnableText;
    AutoPtr<ICharSequence> mDisableText;
    AutoPtr<ICharSequence> mChangeNumberText;
    AutoPtr<ICharSequence> mSummaryOn;
    AutoPtr<ICharSequence> mSummaryOff;

    // button that was clicked on dialog close.
    Int32 mButtonClicked;

    //relevant (parsed) value of the mText
    String mPhoneNumber;
    Boolean mChecked;

    /**
     * Override persistString so that we can get a hold of the EditTextPreference's
     * text field.
     */
    String mEncodedText;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEDITPHONENUMBERPREFERENCE_H__