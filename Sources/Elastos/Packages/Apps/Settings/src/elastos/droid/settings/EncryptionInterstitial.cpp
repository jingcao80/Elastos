
#include "Elastos.Droid.AccessibilityService.h"
#include "elastos/droid/settings/EncryptionInterstitial.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

const String EncryptionInterstitial::EXTRA_REQUIRE_PASSWORD("extra_require_password");

const String EncryptionInterstitial::EXTRA_PASSWORD_QUALITY("extra_password_quality");

const Int32 EncryptionInterstitial::EncryptionInterstitialFragment::ACCESSIBILITY_WARNING_DIALOG = 1;

//===============================================================================
//         EncryptionInterstitial::EncryptionInterstitialFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(EncryptionInterstitial::EncryptionInterstitialFragment::InnerListener, Object, IViewOnClickListener)

EncryptionInterstitial::EncryptionInterstitialFragment::InnerListener::InnerListener(
    /* [in] */ EncryptionInterstitialFragment* host)
    : mHost(host)
{}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  EncryptionInterstitial::EncryptionInterstitialFragment
//===============================================================================

CAR_INTERFACE_IMPL(EncryptionInterstitial::EncryptionInterstitialFragment, SettingsPreferenceFragment, IDialogInterfaceOnClickListener)

EncryptionInterstitial::EncryptionInterstitialFragment::EncryptionInterstitialFragment()
    : mPasswordRequired(FALSE)
{}

EncryptionInterstitial::EncryptionInterstitialFragment::~EncryptionInterstitialFragment()
{}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 layoutId = R::layout::encryption_interstitial;
    AutoPtr<IView> view;
    inflater->Inflate(layoutId, container, FALSE, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::encrypt_require_password, (IView**)&tmp);
    mRequirePasswordToDecryptButton = IRadioButton::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::encrypt_dont_require_password, (IView**)&tmp);
    mDontRequirePasswordToDecryptButton = IRadioButton::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::encryption_message, (IView**)&tmp);
    mEncryptionMessage = ITextView::Probe(tmp);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Int32 quality;
    intent->GetInt32Extra(EXTRA_PASSWORD_QUALITY, 0, &quality);
    Int32 msgId;
    Int32 enableId;
    Int32 disableId;
    switch (quality) {
        case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
            msgId = R::string::encryption_interstitial_message_pattern;
            enableId = R::string::encrypt_require_pattern;
            disableId = R::string::encrypt_dont_require_pattern;
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
            msgId = R::string::encryption_interstitial_message_pin;
            enableId = R::string::encrypt_require_pin;
            disableId = R::string::encrypt_dont_require_pin;
            break;
        default:
            msgId = R::string::encryption_interstitial_message_password;
            enableId = R::string::encrypt_require_password;
            disableId = R::string::encrypt_dont_require_password;
            break;
    }
    mEncryptionMessage->SetText(msgId);

    AutoPtr<InnerListener> listener = new InnerListener(this);

    IView::Probe(mRequirePasswordToDecryptButton)->SetOnClickListener(listener);
    ITextView::Probe(mRequirePasswordToDecryptButton)->SetText(enableId);

    IView::Probe(mDontRequirePasswordToDecryptButton)->SetOnClickListener(listener);
    ITextView::Probe(mDontRequirePasswordToDecryptButton)->SetText(disableId);

    Boolean res;
    intent->GetBooleanExtra(EXTRA_REQUIRE_PASSWORD, TRUE, &res);
    SetRequirePasswordState(res);

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mRequirePasswordToDecryptButton)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IAccessibilityManagerHelper> helper;
        CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
        AutoPtr<IAccessibilityManager> manager;
        helper->GetInstance(IContext::Probe(activity), (IAccessibilityManager**)&manager);

        Boolean accEn;
        manager->IsEnabled(&accEn);
        if (accEn && !mPasswordRequired) {
            SetRequirePasswordState(FALSE); // clear the UI state
            ShowDialog(ACCESSIBILITY_WARNING_DIALOG);
        }
        else {
            SetRequirePasswordState(TRUE);
        }
    }
    else {
        SetRequirePasswordState(FALSE);
    }
    return NOERROR;
}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    switch (dialogId) {
        case ACCESSIBILITY_WARNING_DIALOG: {
            AutoPtr<ILockPatternUtils> lpu;
            CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&lpu);
            Int32 quality;
            lpu->GetKeyguardStoredPasswordQuality(&quality);
            Int32 titleId;
            Int32 messageId;
            switch (quality) {
                case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
                    titleId = R::string::encrypt_talkback_dialog_require_pattern;
                    messageId = R::string::encrypt_talkback_dialog_message_pattern;
                    break;
                case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
                case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
                    titleId = R::string::encrypt_talkback_dialog_require_pin;
                    messageId = R::string::encrypt_talkback_dialog_message_pin;
                    break;
                default:
                    titleId = R::string::encrypt_talkback_dialog_require_password;
                    messageId = R::string::encrypt_talkback_dialog_message_password;
                    break;
            }

            AutoPtr<IAccessibilityManagerHelper> helper;
            CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
            AutoPtr<IAccessibilityManager> manager;
            helper->GetInstance(IContext::Probe(activity), (IAccessibilityManager**)&manager);
            AutoPtr<IList> list;// List<AccessibilityServiceInfo>
            manager->GetEnabledAccessibilityServiceList(
                    IAccessibilityServiceInfo::FEEDBACK_ALL_MASK, (IList**)&list);

            AutoPtr<ICharSequence> exampleAccessibility;
            Boolean res;
            if (list->IsEmpty(&res), res) {
                // This should never happen.  But we shouldn't crash
                exampleAccessibility = CoreUtils::Convert("");
            }
            else {
                AutoPtr<IInterface> obj;
                list->Get(0, (IInterface**)&obj);
                AutoPtr<IResolveInfo> info;
                IAccessibilityServiceInfo::Probe(obj)->GetResolveInfo((IResolveInfo**)&info);
                info->LoadLabel(GetPackageManager(), (ICharSequence**)&exampleAccessibility);
            }

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
            builder->SetTitle(titleId);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, exampleAccessibility);
            String str;
            GetString(messageId, args, &str);
            builder->SetMessage(CoreUtils::Convert(str));
            builder->SetCancelable(TRUE);
            builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
            builder->SetNegativeButton(Elastos::Droid::R::string::cancel, this);
            AutoPtr<IAlertDialog> ad;
            builder->Create((IAlertDialog**)&ad);
            *dialog = IDialog::Probe(ad);
            REFCOUNT_ADD(*dialog)
            return NOERROR;
        }
        default: return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

void EncryptionInterstitial::EncryptionInterstitialFragment::SetRequirePasswordState(
    /* [in] */ Boolean required)
{
    mPasswordRequired = required;
    ICheckable::Probe(mRequirePasswordToDecryptButton)->SetChecked(required);
    ICheckable::Probe(mDontRequirePasswordToDecryptButton)->SetChecked(!required);

    // Updates value returned by SettingsActivity->OnActivityResult().
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<CSettingsActivity> sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
    AutoPtr<IIntent> resultIntentData;
    sa->GetResultIntentData((IIntent**)&resultIntentData);
    if (resultIntentData == NULL) {
        CIntent::New((IIntent**)&resultIntentData);
    }
    resultIntentData->PutBooleanExtra(EXTRA_REQUIRE_PASSWORD, mPasswordRequired);
    sa->SetResultIntentData(resultIntentData);
}

ECode EncryptionInterstitial::EncryptionInterstitialFragment::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        SetRequirePasswordState(TRUE);
    }
    else if (which == IDialogInterface::BUTTON_NEGATIVE) {
        SetRequirePasswordState(FALSE);
    }
    return NOERROR;
}

//===============================================================================
//                  EncryptionInterstitial
//===============================================================================

EncryptionInterstitial::EncryptionInterstitial()
{}

EncryptionInterstitial::~EncryptionInterstitial()
{}

ECode EncryptionInterstitial::constructor()
{
    return SettingsActivity::constructor();
}

ECode EncryptionInterstitial::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> intentOne;
    SettingsActivity::GetIntent((IIntent**)&intentOne);
    AutoPtr<IIntent> modIntent;
    CIntent::New(intentOne, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CEncryptionInterstitialFragment"));
    *intent = modIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

Boolean EncryptionInterstitial::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    return String("Elastos.Droid.Settings.CEncryptionInterstitialFragment").Equals(fragmentName);
}

AutoPtr<IIntent> EncryptionInterstitial::CreateStartIntent(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 quality,
    /* [in] */ Boolean requirePasswordDefault)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ctx, ECLSID_CEncryptionInterstitial, (IIntent**)&intent);
    intent->PutBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE);
    intent->PutExtra(EXTRA_PREFS_SET_BACK_TEXT, String(NULL));
    String str;
    ctx->GetString(R::string::encryption_continue_button, &str);
    intent->PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, str);
    intent->PutExtra(EXTRA_PASSWORD_QUALITY, quality);
    intent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, R::string::encryption_interstitial_header);
    intent->PutBooleanExtra(EXTRA_REQUIRE_PASSWORD, requirePasswordDefault);
    return intent;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos