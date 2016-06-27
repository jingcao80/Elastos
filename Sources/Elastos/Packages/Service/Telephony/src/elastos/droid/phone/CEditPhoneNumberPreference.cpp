
#include "elastos/droid/phone/CEditPhoneNumberPreference.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::IMovementMethod;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::IArrowKeyMovementMethodHelper;
using Elastos::Droid::Text::Method::CArrowKeyMovementMethodHelper;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CEditPhoneNumberPreference::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CEditPhoneNumberPreference::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mParentActivity != NULL) {
        mHost->mParentActivity->StartActivityForResult(mHost->mContactListIntent, mHost->mPrefId);
    }
    return NOERROR;
}

const Int32 CEditPhoneNumberPreference::CM_CONFIRM = 0;

const Int32 CEditPhoneNumberPreference::CM_ACTIVATION = 1;

const String CEditPhoneNumberPreference::VALUE_SEPARATOR = String(":");
const String CEditPhoneNumberPreference::VALUE_OFF = String("0");
const String CEditPhoneNumberPreference::VALUE_ON = String("1");

CAR_INTERFACE_IMPL(CEditPhoneNumberPreference, EditTextPreference, IEditPhoneNumberPreference)

CAR_OBJECT_IMPL(CEditPhoneNumberPreference)

CEditPhoneNumberPreference::CEditPhoneNumberPreference()
    : mConfirmationMode(0)
    , mPrefId(0)
    , mButtonClicked(0)
    , mPhoneNumber(NULL)
    , mChecked(FALSE)
    , mEncodedText(NULL)
{
}

ECode CEditPhoneNumberPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    EditTextPreference::constructor(context, attrs);

    SetDialogLayoutResource(Elastos::Droid::Server::Telephony::R::layout::pref_dialog_editphonenumber);

    //create intent to bring up contact list
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&mContactListIntent);
    mContactListIntent->SetType(IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE);

    //get the edit phone number default settings
    AutoPtr<ITypedArray> a;
    assert(0);
    // context->ObtainStyledAttributes(attrs, Elastos::Droid::Server::Telephony::R::styleable::EditPhoneNumberPreference, 0,
    //         Elastos::Droid::Server::Telephony::R::style::EditPhoneNumberPreference, (ITypedArray**)&a);
    String str;
    a->GetString(Elastos::Droid::Server::Telephony::R::styleable::EditPhoneNumberPreference_enableButtonText, &str);
    mEnableText = CoreUtils::Convert(str);
    a->GetString(Elastos::Droid::Server::Telephony::R::styleable::EditPhoneNumberPreference_disableButtonText, &str);
    mDisableText = CoreUtils::Convert(str);
    a->GetString(Elastos::Droid::Server::Telephony::R::styleable::EditPhoneNumberPreference_changeNumButtonText, &str);
    mChangeNumberText = CoreUtils::Convert(str);
    a->GetInt32(Elastos::Droid::Server::Telephony::R::styleable::EditPhoneNumberPreference_confirmMode, 0, &mConfirmationMode);
    a->Recycle();

    a = NULL;
    //get the summary settings, use CheckBoxPreference as the standard.
    assert(0);
    //context->ObtainStyledAttributes(attrs, Elastos::Droid::R::styleable::CheckBoxPreference, 0, 0, (ITypedArray**)&a);
    a->GetString(Elastos::Droid::R::styleable::CheckBoxPreference_summaryOn, &str);
    mSummaryOn = CoreUtils::Convert(str);
    a->GetString(Elastos::Droid::R::styleable::CheckBoxPreference_summaryOff, &str);
    mSummaryOff = CoreUtils::Convert(str);
    return a->Recycle();
}

ECode CEditPhoneNumberPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CEditPhoneNumberPreference::OnBindView(
    /* [in] */ IView* view)
{
    EditTextPreference::OnBindView(view);

    // Sync the summary view
    AutoPtr<IView> _view;
    view->FindViewById(Elastos::Droid::R::id::summary, (IView**)&_view);
    AutoPtr<ITextView> summaryView = ITextView::Probe(_view);
    if (summaryView != NULL) {
        AutoPtr<ICharSequence> sum;
        Int32 vis;

        //set summary depending upon mode
        if (mConfirmationMode == CM_ACTIVATION) {

            if (mChecked) {
                if (mSummaryOn == NULL) {
                    GetSummary((ICharSequence**)&sum);
                }
                else {
                    sum = mSummaryOn;
                }
            }
            else {
                if (mSummaryOff == NULL) {
                    GetSummary((ICharSequence**)&sum);
                }
                else {
                    sum = mSummaryOff;
                }
            }
        }
        else {
            GetSummary((ICharSequence**)&sum);
        }

        if (sum != NULL) {
            summaryView->SetText(sum);
            vis = IView::VISIBLE;
        }
        else {
            vis = IView::GONE;
        }

        Int32 visibility;
        IView::Probe(summaryView)->GetVisibility(&visibility);
        if (vis != visibility) {
            IView::Probe(summaryView)->SetVisibility(vis);
        }
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    // default the button clicked to be the cancel button.
    mButtonClicked = IDialogInterface::BUTTON_NEGATIVE;

    EditTextPreference::OnBindDialogView(view);

    //get the edittext component within the number field
    AutoPtr<IEditText> editText;
    GetEditText((IEditText**)&editText);
    //get the contact pick button within the number field
    AutoPtr<IView> _view;
    view->FindViewById(Elastos::Droid::Server::Telephony::R::id::select_contact, (IView**)&_view);
    mContactPickButton = IImageButton::Probe(_view);

    //setup number entry
    if (editText != NULL) {
        // see if there is a means to get a default number,
        // and set it accordingly.
        if (mGetDefaultNumberListener != NULL) {
            String defaultNumber;
            mGetDefaultNumberListener->OnGetDefaultNumber(this, &defaultNumber);
            if (defaultNumber != NULL) {
                mPhoneNumber = defaultNumber;
            }
        }
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mPhoneNumber);
        ITextView::Probe(editText)->SetText(cchar);
        AutoPtr<IArrowKeyMovementMethodHelper> helper;
        CArrowKeyMovementMethodHelper::AcquireSingleton((IArrowKeyMovementMethodHelper**)&helper);
        AutoPtr<IMovementMethod> method;
        helper->GetInstance((IMovementMethod**)&method);
        ITextView::Probe(editText)->SetMovementMethod(method);
        AutoPtr<IDialerKeyListenerHelper> helper2;
        CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper2);
        AutoPtr<IDialerKeyListener> listener;
        helper2->GetInstance((IDialerKeyListener**)&listener);
        ITextView::Probe(editText)->SetKeyListener(IKeyListener::Probe(listener));
        IView::Probe(editText)->SetOnFocusChangeListener(mDialogFocusChangeListener);
    }

    //set contact picker
    if (mContactPickButton != NULL) {
        AutoPtr<IViewOnClickListener> listener = new MyViewOnClickListener(this);
        IView::Probe(mContactPickButton)->SetOnClickListener(listener);
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::OnAddEditTextToDialogView(
    /* [in] */IView* dialogView,
    /* [in] */IEditText* editText)
{
    // look for the container object
    AutoPtr<IView> view;
    dialogView->FindViewById(Elastos::Droid::Server::Telephony::R::id::edit_container, (IView**)&view);
    AutoPtr<IViewGroup> container = IViewGroup::Probe(view);

    // add the edittext to the container.
    if (container != NULL) {
        container->AddView(IView::Probe(editText), IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT);
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    // modified so that we just worry about the buttons being
    // displayed, since there is no need to hide the edittext
    // field anymore.
    if (mConfirmationMode == CM_ACTIVATION) {
        if (mChecked) {
            builder->SetPositiveButton(mChangeNumberText, this);
            builder->SetNeutralButton(mDisableText, this);
        }
        else {
            builder->SetPositiveButton((ICharSequence*)NULL, NULL);
            builder->SetNeutralButton(mEnableText, this);
        }
    }
    // set the call icon on the title.
    return builder->SetIcon(Elastos::Droid::Server::Telephony::R::mipmap::ic_launcher_phone);
}

ECode CEditPhoneNumberPreference::SetDialogOnFocusChangeListener(
    /* [in] */ IViewOnFocusChangeListener* l)
{
    mDialogFocusChangeListener = l;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetDialogOnClosedListener(
    /* [in] */ IEditPhoneNumberPreferenceOnDialogClosedListener* l)
{
    mDialogOnClosedListener = l;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetParentActivity(
    /* [in] */ IActivity* parent,
    /* [in] */ Int32 identifier)
{
    mParentActivity = parent;
    mPrefId = identifier;
    mGetDefaultNumberListener = NULL;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetParentActivity(
    /* [in] */ IActivity* parent,
    /* [in] */ Int32 identifier,
    /* [in] */ IEditPhoneNumberPreferenceGetDefaultNumberListener* l)
{
    mParentActivity = parent;
    mPrefId = identifier;
    mGetDefaultNumberListener = l;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::OnPickActivityResult(
    /* [in] */ const String& pickedValue)
{
    AutoPtr<IEditText> editText;
    GetEditText((IEditText**)&editText);
    if (editText != NULL) {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(pickedValue);
        ITextView::Probe(editText)->SetText(cchar);
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // The neutral button (button3) is always the toggle.
    if ((mConfirmationMode == CM_ACTIVATION) && (which == IDialogInterface::BUTTON_NEUTRAL)) {
        //flip the toggle if we are in the correct mode.
        Boolean res;
        IsToggled(&res);
        SetToggled(!res);
    }
    // record the button that was clicked.
    mButtonClicked = which;
    return EditTextPreference::OnClick(dialog, which);
}

ECode CEditPhoneNumberPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    // A positive result is technically either button1 or button3.
    if ((mButtonClicked == IDialogInterface::BUTTON_POSITIVE) ||
            (mButtonClicked == IDialogInterface::BUTTON_NEUTRAL)){
        AutoPtr<IEditText> editText;
        GetEditText((IEditText**)&editText);
        AutoPtr<ICharSequence> cchar;
        ITextView::Probe(editText)->GetText((ICharSequence**)&cchar);
        String str;
        cchar->ToString(&str);
        SetPhoneNumber(str);

        EditTextPreference::OnDialogClosed(positiveResult);

        String str2;
        GetStringValue(&str2);
        SetText(str2);
    }
    else {
        EditTextPreference::OnDialogClosed(positiveResult);
    }

    // send the clicked button over to the listener.
    if (mDialogOnClosedListener != NULL) {
        mDialogOnClosedListener->OnDialogClosed(this, mButtonClicked);
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::IsToggled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mChecked;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetToggled(
    /* [in] */ Boolean checked)
{
    mChecked = checked;
    String value;
    GetStringValue(&value);
    SetText(value);
    return NotifyChanged();
}

ECode CEditPhoneNumberPreference::GetPhoneNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number)

    // return the phone number, after it has been stripped of all
    // irrelevant text.
    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    return helper->StripSeparators(mPhoneNumber, number);
}

ECode CEditPhoneNumberPreference::GetRawPhoneNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number)

    *number = mPhoneNumber;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetPhoneNumber(
    /* [in] */ const String& number)
{
    mPhoneNumber = number;
    String value;
    GetStringValue(&value);
    SetText(value);
    return NotifyChanged();
}

ECode CEditPhoneNumberPreference::OnSetInitialValue(
    /* [in] */ Boolean restoreValue,
    /* [in] */ IInterface* defaultValue)
{
    String value;
    GetStringValue(&value);

    String str;
    if (restoreValue) {
        GetPersistedString(value, &str);
    }
    else {
        AutoPtr<ICharSequence> obj = ICharSequence::Probe(defaultValue);
        obj->ToString(&str);
    }
    return SetValueFromString(str);
}

ECode CEditPhoneNumberPreference::ShouldDisableDependents(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // There is really only one case we care about, but for consistency
    // we fill out the dependency tree for all of the cases.  If this
    // is in activation mode (CF), we look for the encoded toggle value
    // in the string.  If this in confirm mode (VM), then we just
    // examine the number field.
    // Note: The toggle value is stored in the string in an encoded
    // manner (refer to setValueFromString and getStringValue below).
    Boolean shouldDisable = FALSE;
    if ((mConfirmationMode == CM_ACTIVATION) && (mEncodedText != NULL)) {
        AutoPtr<ArrayOf<String> > inValues;
        StringUtils::Split(mEncodedText, String(":"), 2, (ArrayOf<String>**)&inValues);
        shouldDisable = (*inValues)[0].Equals(VALUE_ON);
    }
    else {
        shouldDisable = (TextUtils::IsEmpty(mPhoneNumber) && (mConfirmationMode == CM_CONFIRM));
    }
    *result = shouldDisable;
    return NOERROR;
}

ECode CEditPhoneNumberPreference::PersistString(
    /* [in] */ const String& value,
    /* [out] */ Boolean* str)
{
    VALIDATE_NOT_NULL(str)

    mEncodedText = value;
    return EditTextPreference::PersistString(value, str);
}

ECode CEditPhoneNumberPreference::SetSummaryOn(
    /* [in] */ ICharSequence* summary)
{
    mSummaryOn = summary;
    Boolean res;
    IsToggled(&res);
    if (res) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetSummaryOn(
    /* [in] */ Int32 summaryResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(summaryResId, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    return SetSummaryOn(cchar);
}

ECode CEditPhoneNumberPreference::GetSummaryOn(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSummaryOn;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetSummaryOff(
    /* [in] */ ICharSequence* summary)
{
    mSummaryOff = summary;
    Boolean res;
    IsToggled(&res);
    if (!res) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetSummaryOff(
    /* [in] */ Int32 summaryResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(summaryResId, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    return SetSummaryOff(cchar);
}

ECode CEditPhoneNumberPreference::GetSummaryOff(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSummaryOff;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEditPhoneNumberPreference::SetValueFromString(
    /* [in] */ const String& value)
{
    AutoPtr<ArrayOf<String> > inValues;
    StringUtils::Split(value, String(":"), 2, (ArrayOf<String>**)&inValues);
    SetToggled((*inValues)[0].Equals(VALUE_ON));
    return SetPhoneNumber((*inValues)[1]);
}

ECode CEditPhoneNumberPreference::GetStringValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)

    StringBuilder sb;
    Boolean res;
    IsToggled(&res);
    sb += res ? VALUE_ON : VALUE_OFF;
    sb += VALUE_SEPARATOR;
    String str;
    GetPhoneNumber(&str);
    sb += str;
    return sb.ToString(value);
}

ECode CEditPhoneNumberPreference::ShowPhoneNumberDialog()
{
    return ShowDialog(NULL);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos