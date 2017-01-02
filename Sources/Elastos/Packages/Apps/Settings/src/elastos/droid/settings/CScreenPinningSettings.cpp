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
#include "elastos/droid/settings/CScreenPinningSettings.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "R.h"

using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Widget::CSwitchBar;
using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CScreenPinningSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CScreenPinningSettings::InnerListener, Object, ISwitchBarOnSwitchChangeListener)

CScreenPinningSettings::InnerListener::InnerListener(
    /* [in] */ CScreenPinningSettings* host)
    : mHost(host)
{}

ECode CScreenPinningSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

//===============================================================================
//                  CScreenPinningSettings::MyBaseSearchIndexProvider
//===============================================================================

CScreenPinningSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CScreenPinningSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CScreenPinningSettings::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IList> result; //List<SearchIndexableRaw>
    CArrayList::New((IList**)&result);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    // Add fragment title
    AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw;
    data->constructor(context);
    res->GetString(R::string::screen_pinning_title, &data->mTitle);
    res->GetString(R::string::screen_pinning_title, &data->mScreenTitle);
    result->Add((IObject*)data);

    // Screen pinning description.
    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    res->GetString(R::string::screen_pinning_description, &data->mTitle);
    res->GetString(R::string::screen_pinning_title, &data->mScreenTitle);
    result->Add((IObject*)data);

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

//===============================================================================
//                  CScreenPinningSettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CScreenPinningSettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> CScreenPinningSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CScreenPinningSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CScreenPinningSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CScreenPinningSettings)

CScreenPinningSettings::CScreenPinningSettings()
{}

CScreenPinningSettings::~CScreenPinningSettings()
{}

ECode CScreenPinningSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode CScreenPinningSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    CSettingsActivity* activity = (CSettingsActivity*)ISettingsActivity::Probe(act);

    activity->GetSwitchBar((ISwitchBar**)&mSwitchBar);
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    AutoPtr<InnerListener> listener = new InnerListener(this);
    switchBar->AddOnSwitchChangeListener(listener);
    switchBar->Show();
    switchBar->SetChecked(IsLockToAppEnabled());
    return NOERROR;
}

ECode CScreenPinningSettings::OnDestroyView()
{
    SettingsPreferenceFragment::OnDestroyView();

    AutoPtr<InnerListener> listener = new InnerListener(this);
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    switchBar->RemoveOnSwitchChangeListener(listener);
    switchBar->Hide();
    return NOERROR;
}

ECode CScreenPinningSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    return inflater->Inflate(R::layout::screen_pinning_instructions, NULL, result);
}

Boolean CScreenPinningSettings::IsLockToAppEnabled()
{
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    // try {
    Int32 data;
    ECode ec = sys->GetInt32(GetContentResolver(), ISettingsSystem::LOCK_TO_APP_ENABLED, &data);
    if (SUCCEEDED(ec)) {
        return data != 0;
    }
    // } catch (SettingNotFoundException e) {
    return FALSE;
    // }
}

void CScreenPinningSettings::SetLockToAppEnabled(
    /* [in] */ Boolean isEnabled)
{
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    Boolean res;
    sys->PutInt32(GetContentResolver(), ISettingsSystem::LOCK_TO_APP_ENABLED,
            isEnabled ? 1 : 0, &res);
}

ECode CScreenPinningSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    SetLockToAppEnabled(isChecked);
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos