//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/notification/CRedactionInterstitial.h"
#include "../R.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICheckable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CRedactionInterstitial::RedactionInterstitialFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CRedactionInterstitial::RedactionInterstitialFragment::InnerListener, Object, IViewOnClickListener)

CRedactionInterstitial::RedactionInterstitialFragment::InnerListener::InnerListener(
    /* [in] */ RedactionInterstitialFragment* host)
    : mHost(host)
{}

ECode CRedactionInterstitial::RedactionInterstitialFragment::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  CRedactionInterstitial::RedactionInterstitialFragment
//===============================================================================

CRedactionInterstitial::RedactionInterstitialFragment::RedactionInterstitialFragment()
{}

CRedactionInterstitial::RedactionInterstitialFragment::~RedactionInterstitialFragment()
{}

ECode CRedactionInterstitial::RedactionInterstitialFragment::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode CRedactionInterstitial::RedactionInterstitialFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::redaction_interstitial, container, FALSE, (IView**)&view);
    AutoPtr<IView> tmpShowAllButton;
    view->FindViewById(R::id::show_all, (IView**)&tmpShowAllButton);
    mShowAllButton = IRadioButton::Probe(tmpShowAllButton);
    AutoPtr<IView> tmpRedactSensitiveButton;
    view->FindViewById(R::id::redact_sensitive, (IView**)&tmpRedactSensitiveButton);
    mRedactSensitiveButton = IRadioButton::Probe(tmpRedactSensitiveButton);
    AutoPtr<IView> tmpHideAllButton;
    view->FindViewById(R::id::hide_all, (IView**)&tmpHideAllButton);
    mHideAllButton = IRadioButton::Probe(tmpHideAllButton);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    tmpShowAllButton->SetOnClickListener(listener);
    tmpRedactSensitiveButton->SetOnClickListener(listener);
    tmpHideAllButton->SetOnClickListener(listener);

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRedactionInterstitial::RedactionInterstitialFragment::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    LoadFromSettings();
    return NOERROR;
}

void CRedactionInterstitial::RedactionInterstitialFragment::LoadFromSettings()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);

    Int32 data;
    secure->GetInt32(GetContentResolver(),
                ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, 0, &data);
    const Boolean enabled = data != 0;
    secure->GetInt32(GetContentResolver(),
                ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 1, &data);
    const Boolean show = data != 0;
    ICheckable::Probe(mShowAllButton)->SetChecked(enabled && show);
    ICheckable::Probe(mRedactSensitiveButton)->SetChecked(enabled && !show);
    ICheckable::Probe(mHideAllButton)->SetChecked(!enabled);
}

ECode CRedactionInterstitial::RedactionInterstitialFragment::OnClick(
    /* [in] */ IView* v)
{
    const Boolean show = (v == IView::Probe(mShowAllButton));
    const Boolean enabled = (v != IView::Probe(mHideAllButton));

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, show ? 1 : 0, &res);
    secure->PutInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, enabled ? 1 : 0, &res);
    return NOERROR;
}

//===============================================================================
//                  CRedactionInterstitial
//===============================================================================

CAR_OBJECT_IMPL(CRedactionInterstitial)

CRedactionInterstitial::CRedactionInterstitial()
{}

CRedactionInterstitial::~CRedactionInterstitial()
{}

ECode CRedactionInterstitial::constructor()
{
    return SettingsActivity::constructor();
}

ECode CRedactionInterstitial::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    AutoPtr<IIntent> tmpIntent;
    SettingsActivity::GetIntent((IIntent**)&tmpIntent);
    AutoPtr<IIntent> modIntent;
    CIntent::New(tmpIntent, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment"));

    *intent = modIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

Boolean CRedactionInterstitial::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    return String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment").Equals(fragmentName);
}

AutoPtr<IIntent> CRedactionInterstitial::CreateStartIntent(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ctx, ECLSID_CRedactionInterstitial, (IIntent**)&intent);
    intent->PutBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE);
    intent->PutExtra(EXTRA_PREFS_SET_BACK_TEXT, String(NULL));
    String str;
    ctx->GetString(R::string::app_notifications_dialog_done, &str);
    intent->PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, str);
    return intent;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos