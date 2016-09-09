
#include "elastos/droid/settings/inputmethod/CSpellCheckerPreference.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Settings::Utils;

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

//===============================================================================
//                  CSpellCheckerPreference::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CSpellCheckerPreference::InnerListener, Object, IViewOnClickListener)

CSpellCheckerPreference::InnerListener::InnerListener(
    /* [in] */ CSpellCheckerPreference* host)
    : mHost(host)
{}

ECode CSpellCheckerPreference::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  CSpellCheckerPreference
//===============================================================================

CAR_INTERFACE_IMPL(CSpellCheckerPreference, Elastos::Droid::Preference::Preference, ISpellCheckerPreference);

CAR_OBJECT_IMPL(CSpellCheckerPreference)

CSpellCheckerPreference::CSpellCheckerPreference()
    : mSelected(FALSE)
{}

CSpellCheckerPreference::~CSpellCheckerPreference()
{}

ECode CSpellCheckerPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISpellCheckerInfo* sci,
    /* [in] */ ISpellCheckerPreferenceOnRadioButtonPreferenceListener* onRadioButtonListener)
{
    Preference::constructor(context, NULL, 0);
    SetPersistent(FALSE);
    SetLayoutResource(R::layout::preference_spellchecker);
    SetWidgetLayoutResource(R::layout::preference_spellchecker_widget);
    mSci = sci;
    mOnRadioButtonListener = onRadioButtonListener;
    String id;
    sci->GetId(&id);
    SetKey(id);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<ICharSequence> cs;
    sci->LoadLabel(pm, (ICharSequence**)&cs);
    SetTitle(cs);
    String settingsActivity;
    mSci->GetSettingsActivity(&settingsActivity);
    if (TextUtils::IsEmpty(settingsActivity)) {
        SetIntent(NULL);
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        String packageName;
        mSci->GetPackageName(&packageName);
        intent->SetClassName(packageName, settingsActivity);
        SetIntent(intent);
    }
    return NOERROR;
}

ECode CSpellCheckerPreference::OnBindView(
    /* [in] */ IView* view)
{
    Preference::OnBindView(view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::pref_radio, (IView**)&tmp);
    mRadioButton = IRadioButton::Probe(tmp);
    view->FindViewById(R::id::pref_left_button, (IView**)&mPrefLeftButton);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mPrefLeftButton->SetOnClickListener(listener);
    view->FindViewById(R::id::pref_right_button, (IView**)&mSettingsButton);
    mSettingsButton->SetOnClickListener(listener);
    UpdateSelectedState(mSelected);
    return NOERROR;
}

ECode CSpellCheckerPreference::OnClick(
    /* [in] */ IView* v)
{
    if (v == mPrefLeftButton) {
        mOnRadioButtonListener->OnRadioButtonClicked(this);
        return NOERROR;
    }
    if (v == mSettingsButton) {
        OnSettingsButtonClicked();
        return NOERROR;
    }
    return NOERROR;
}

void CSpellCheckerPreference::OnSettingsButtonClicked()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    // try {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        ECode ec = NOERROR;
        if (intent != NULL) {
            // Invoke a settings activity of an spell checker.
            ec = context->StartActivity(intent);
            if (FAILED(ec)) {
                AutoPtr<IPackageManager> pm;
                context->GetPackageManager((IPackageManager**)&pm);
                AutoPtr<ICharSequence> cs;
                mSci->LoadLabel(pm, (ICharSequence**)&cs);
                AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
                formatArgs->Set(0, cs);
                String message;
                context->GetString(R::string::failed_to_open_app_settings_toast, formatArgs, &message);
                AutoPtr<IToastHelper> helper;
                CToastHelper::AcquireSingleton((IToastHelper**)&helper);
                AutoPtr<IToast> toast;
                helper->MakeText(context, CoreUtils::Convert(message), IToast::LENGTH_LONG, (IToast**)&toast);
                toast->Show();
            }
        }
    // } catch (final ActivityNotFoundException e) {
    //     final String message = context->GetString(R::string::failed_to_open_app_settings_toast,
    //             mSci->LoadLabel(context->GetPackageManager()));
    //     Toast->MakeText(context, message, Toast.LENGTH_LONG).Show();
    // }
}

ECode CSpellCheckerPreference::GetSpellCheckerInfo(
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mSci;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode CSpellCheckerPreference::SetSelected(
    /* [in] */ Boolean selected)
{
    mSelected = selected;
    UpdateSelectedState(selected);
    return NOERROR;
}

void CSpellCheckerPreference::UpdateSelectedState(
    /* [in] */ Boolean selected)
{
    if (mRadioButton != NULL) {
        ICheckable::Probe(mRadioButton)->SetChecked(selected);
        Boolean isEnable;
        IsEnabled(&isEnable);
        EnableSettingsButton(isEnable && selected);
    }
}

void CSpellCheckerPreference::EnableSettingsButton(
    /* [in] */ Boolean enabled)
{
    if (mSettingsButton == NULL) {
        return;
    }
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent == NULL) {
        mSettingsButton->SetVisibility(IView::GONE);
    }
    else {
        mSettingsButton->SetEnabled(enabled);
        mSettingsButton->SetClickable(enabled);
        mSettingsButton->SetFocusable(enabled);
        if (!enabled) {
            mSettingsButton->SetAlpha(Utils::DISABLED_ALPHA);
        }
    }
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos