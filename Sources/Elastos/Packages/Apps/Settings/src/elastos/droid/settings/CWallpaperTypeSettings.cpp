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

#include "elastos/droid/settings/CWallpaperTypeSettings.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CWallpaperTypeSettings::MyBaseSearchIndexProvider
//===============================================================================

CWallpaperTypeSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CWallpaperTypeSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CWallpaperTypeSettings::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SET_WALLPAPER, (IIntent**)&intent);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> rList; //List<ResolveInfo>
    pm->QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&rList);

    // Add indexable data for each of the matching activities
    Int32 size;
    rList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        rList->Get(i, (IInterface**)&obj);
        IResolveInfo* info = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        IPackageItemInfo* pii = IPackageItemInfo::Probe(activityInfo);
        String packageName, name;
        pii->GetPackageName(&packageName);
        pii->GetName(&name);

        AutoPtr<ICharSequence> label;
        info->LoadLabel(pm, (ICharSequence**)&label);
        if (label == NULL) {
            label = CoreUtils::Convert(packageName);
        }

        AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
        data->constructor(context);
        label->ToString(&data->mTitle);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetString(R::string::wallpaper_settings_fragment_title, &data->mScreenTitle);
        data->mIntentAction = IIntent::ACTION_SET_WALLPAPER;
        data->mIntentTargetPackage = packageName;
        data->mIntentTargetClass = name;
        result->Add((ISearchIndexableRaw*)data);
    }

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

//===============================================================================
//                  CWallpaperTypeSettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CWallpaperTypeSettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> CWallpaperTypeSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CWallpaperTypeSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CWallpaperTypeSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CWallpaperTypeSettings)

CWallpaperTypeSettings::CWallpaperTypeSettings()
{}

CWallpaperTypeSettings::~CWallpaperTypeSettings()
{}

ECode CWallpaperTypeSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode CWallpaperTypeSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);

    AddPreferencesFromResource(R::xml::wallpaper_settings);
    PopulateWallpaperTypes();
    return NOERROR;
}

void CWallpaperTypeSettings::PopulateWallpaperTypes()
{
    // Search for activities that satisfy the ACTION_SET_WALLPAPER action
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SET_WALLPAPER, (IIntent**)&intent);
    AutoPtr<IPackageManager> pm = GetPackageManager();
    AutoPtr<IList> rList;
    pm->QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&rList);

    AutoPtr<IPreferenceScreen> parent;
    GetPreferenceScreen((IPreferenceScreen**)&parent);
    IPreferenceGroup* _parent = IPreferenceGroup::Probe(parent);
    _parent->SetOrderingAsAdded(FALSE);
    // Add Preference items for each of the matching activities
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* act = IContext::Probe(activity);
    Int32 size;
    rList->GetSize(&size);
    Boolean res;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        rList->Get(i, (IInterface**)&obj);
        IResolveInfo* info = IResolveInfo::Probe(obj);
        AutoPtr<IPreference> pref;
        CPreference::New(act, (IPreference**)&pref);
        AutoPtr<IIntent> prefIntent;
        CIntent::New(intent, (IIntent**)&prefIntent);
        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        IPackageItemInfo* pii = IPackageItemInfo::Probe(activityInfo);
        String packageName, name;
        pii->GetPackageName(&packageName);
        pii->GetName(&name);
        AutoPtr<IComponentName> comp;
        CComponentName::New(packageName, name, (IComponentName**)&comp);
        prefIntent->SetComponent(comp);
        pref->SetIntent(prefIntent);
        AutoPtr<ICharSequence> label;
        info->LoadLabel(pm, (ICharSequence**)&label);
        if (label == NULL) {
            label = CoreUtils::Convert(packageName);
        }
        pref->SetTitle(label);
        _parent->AddPreference(pref, &res);
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos