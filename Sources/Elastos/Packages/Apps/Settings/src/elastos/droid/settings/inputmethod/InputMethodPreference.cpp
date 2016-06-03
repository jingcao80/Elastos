
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/settings/inputmethod/InputMethodPreference.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::InputMethod::IInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::CInputMethodUtils;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String InputMethodPreference::TAG("InputMethodPreference");
const String InputMethodPreference::EMPTY_TEXT("");

//===============================================================================
//                  InputMethodPreference::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(InputMethodPreference::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener);

InputMethodPreference::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ InputMethodPreference* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

InputMethodPreference::DialogInterfaceOnClickListener::~DialogInterfaceOnClickListener()
{}

ECode InputMethodPreference::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (mId) {
        case 0:
            // The user confirmed to enable a 3rd party IME.
            mHost->SetChecked(TRUE);
            mHost->mOnSaveListener->OnSaveInputMethodPreference(mHost);
            mHost->NotifyChanged();
            break;
        case 1:
            // The user canceled to enable a 3rd party IME.
            mHost->SetChecked(FALSE);
            mHost->mOnSaveListener->OnSaveInputMethodPreference(mHost);
            mHost->NotifyChanged();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  InputMethodPreference
//===============================================================================

CAR_INTERFACE_IMPL_3(InputMethodPreference, SwitchPreference, IInputMethodPreference,
        IPreferenceOnPreferenceClickListener, IPreferenceOnPreferenceChangeListener);

InputMethodPreference::InputMethodPreference(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ Boolean isImeEnabler,
    /* [in] */ Boolean isAllowedByOrganization,
    /* [in] */ IOnSavePreferenceListener* onSaveListener)
    : mHasPriorityInSorting(FALSE)
    , mIsAllowedByOrganization(FALSE)
{
    SwitchPreference::constructor(context);
    SetPersistent(FALSE);
    mImi = imi;
    mIsAllowedByOrganization = isAllowedByOrganization;
    mOnSaveListener = onSaveListener;
    if (!isImeEnabler) {
        // Hide switch widget.
        SetWidgetLayoutResource(0 /* widgetLayoutResId */);
    }
    // Disable on/off switch texts.
    SetSwitchTextOn(CoreUtils::Convert(EMPTY_TEXT));
    SetSwitchTextOff(CoreUtils::Convert(EMPTY_TEXT));
    String id;
    imi->GetId(&id);
    SetKey(id);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICharSequence> seq;
    imi->LoadLabel(packageManager, (ICharSequence**)&seq);
    SetTitle(seq);
    String settingsActivity;
    imi->GetSettingsActivity(&settingsActivity);
    if (TextUtils::IsEmpty(settingsActivity)) {
        SetIntent(NULL);
    }
    else {
        // Set an intent to invoke settings activity of an input method.
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        String name;
        imi->GetPackageName(&name);
        intent->SetClassName(name, settingsActivity);
        SetIntent(intent);
    }
    mInputMethodSettingValues = InputMethodSettingValuesWrapper::GetInstance(context);
    AutoPtr<IInputMethodUtils> imu;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imu);
    Boolean res;
    mHasPriorityInSorting = (imu->IsSystemIme(imi, &res), res)
            && mInputMethodSettingValues->IsValidSystemNonAuxAsciiCapableIme(imi, context);
    SetOnPreferenceClickListener(this);
    SetOnPreferenceChangeListener(this);
}

InputMethodPreference::~InputMethodPreference()
{}

AutoPtr<IInputMethodInfo> InputMethodPreference::GetInputMethodInfo()
{
    return mImi;
}

Boolean InputMethodPreference::IsImeEnabler()
{
    // If this {@link SwitchPreference} doesn't have a widget layout, we explicitly hide the
    // switch widget at constructor.
    Int32 result;
    GetWidgetLayoutResource(&result);
    return result != 0;
}

ECode InputMethodPreference::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // Always returns FALSE to prevent default behavior.
    // See {@link TwoStatePreference#OnClick()}.
    if (!IsImeEnabler()) {
        // Prevent disabling an IME because this preference is for invoking a settings activity.
        return NOERROR;
    }
    Boolean res;
    if (IsChecked(&res), res) {
        // Disable this IME.
        SetChecked(FALSE);
        mOnSaveListener->OnSaveInputMethodPreference(this);
        return NOERROR;
    }

    AutoPtr<IInputMethodUtils> imu;
    CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imu);
    if (imu->IsSystemIme(mImi, &res), res) {
        // Enable a system IME. No need to show a security warning dialog.
        SetChecked(TRUE);
        mOnSaveListener->OnSaveInputMethodPreference(this);
        return NOERROR;
    }
    // Enable a 3rd party IME.
    ShowSecurityWarnDialog(mImi);
    return NOERROR;
}

ECode InputMethodPreference::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Always returns TRUE to prevent invoking an intent without catching exceptions.
    // See {@link Preference#PerformClick(PreferenceScreen)}/
    if (IsImeEnabler()) {
        // Prevent invoking a settings activity because this preference is for enabling and
        // disabling an input method.
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    // try {
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    ECode ec;
    if (intent != NULL) {
        // Invoke a settings activity of an input method.
        ec = context->StartActivity(intent);
    }
    if (FAILED(ec)) {
    // } catch (final ActivityNotFoundException e) {
        Logger::D(TAG, "IME's Settings Activity Not Found, 0x%08x", ec);
        AutoPtr<IPackageManager> packageManager;
        context->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<ICharSequence> seq;
        mImi->LoadLabel(packageManager, (ICharSequence**)&seq);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, seq);
        String message;
        context->GetString(R::string::failed_to_open_app_settings_toast,
                args, &message);
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(context, CoreUtils::Convert(message),
                IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
    // }
    }
    *result = TRUE;
    return NOERROR;
}

void InputMethodPreference::UpdatePreferenceViews()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Boolean isAlwaysChecked = mInputMethodSettingValues->IsAlwaysCheckedIme(
            mImi, context);
    // Only when this preference has a switch and an input method should be always enabled,
    // this preference should be disabled to prevent accidentally disabling an input method.
    SetEnabled(!((isAlwaysChecked && IsImeEnabler()) || (!mIsAllowedByOrganization)));
    SetChecked(mInputMethodSettingValues->IsEnabledImi(mImi));
    SetSummary(CoreUtils::Convert(GetSummaryString()));
}

AutoPtr<IInputMethodManager> InputMethodPreference::GetInputMethodManager()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    return IInputMethodManager::Probe(obj);
}

String InputMethodPreference::GetSummaryString()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (!mIsAllowedByOrganization) {
        String str;
        context->GetString(
                R::string::accessibility_feature_or_input_method_not_allowed, &str);
        return str;
    }
    AutoPtr<IInputMethodManager> imm = GetInputMethodManager();
    AutoPtr<IList> subtypes;//List<InputMethodSubtype>
    imm->GetEnabledInputMethodSubtypeList(mImi, TRUE, (IList**)&subtypes);
    AutoPtr<IArrayList> subtypeLabels;//ArrayList<CharSequence>
    CArrayList::New((IArrayList**)&subtypeLabels);
    Int32 size;
    subtypes->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        subtypes->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(obj);
        String name;
        mImi->GetPackageName(&name);
        AutoPtr<IServiceInfo> info;
        mImi->GetServiceInfo((IServiceInfo**)&info);
        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        AutoPtr<ICharSequence> label;
        subtype->GetDisplayName(
              context, name, applicationInfo, (ICharSequence**)&label);
        subtypeLabels->Add(label);
    }
    // TODO: A delimiter of subtype labels should be localized.
    AutoPtr< ArrayOf<IInterface*> > args;
    subtypeLabels->ToArray((ArrayOf<IInterface*>**)&args);
    return TextUtils::Join(CoreUtils::Convert(", "), args);
}

void InputMethodPreference::ShowSecurityWarnDialog(
    /* [in] */ IInputMethodInfo* imi)
{
    Boolean res;
    if (mDialog != NULL && (IDialog::Probe(mDialog)->IsShowing(&res), res)) {
        IDialogInterface::Probe(mDialog)->Dismiss();
    }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetCancelable(TRUE /* cancelable */);
    builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IServiceInfo> info;
    imi->GetServiceInfo((IServiceInfo**)&info);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(applicationInfo)->LoadLabel(packageManager, (ICharSequence**)&label);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, label);

    String str;
    context->GetString(R::string::ime_security_warning, args, &str);
    builder->SetMessage(CoreUtils::Convert(str));
    AutoPtr<DialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(this, 0);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener);
    AutoPtr<DialogInterfaceOnClickListener> listener1 = new DialogInterfaceOnClickListener(this, 1);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, listener1);
    builder->Create((IAlertDialog**)&mDialog);
    IDialog::Probe(mDialog)->Show();
}

Int32 InputMethodPreference::CompareTo(
    /* [in] */ InputMethodPreference* rhs,
    /* [in] */ ICollator* collator)
{
    if (this == rhs) {
        return 0;
    }
    if (mHasPriorityInSorting == rhs->mHasPriorityInSorting) {
        AutoPtr<ICharSequence> t0;
        GetTitle((ICharSequence**)&t0);
        AutoPtr<ICharSequence> t1;
        rhs->GetTitle((ICharSequence**)&t1);
        if (TextUtils::IsEmpty(t0)) {
            return 1;
        }
        if (TextUtils::IsEmpty(t1)) {
            return -1;
        }
        Int32 result;
        collator->Compare(t0, t1, &result);
        return result;
    }
    // Prefer always checked system IMEs
    return mHasPriorityInSorting ? -1 : 1;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
