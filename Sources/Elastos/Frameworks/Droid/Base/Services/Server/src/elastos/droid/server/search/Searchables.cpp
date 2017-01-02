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

#include "elastos/droid/server/search/Searchables.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CSearchableInfoHelper;
using Elastos::Droid::App::ISearchableInfoHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CAR_INTERFACE_IMPL(Searchables::Comparator, Object, IComparator)

ECode Searchables::Comparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IResolveInfo> lhs = IResolveInfo::Probe(_lhs);
    AutoPtr<IResolveInfo> rhs = IResolveInfo::Probe(_rhs);
    if (lhs == rhs) {
        *result = 0;
        return NOERROR;
    }

    Boolean lhsSystem = Searchables::IsSystemApp(lhs);
    Boolean rhsSystem = Searchables::IsSystemApp(rhs);

    if (lhsSystem && !rhsSystem) {
        *result = -1;
        return NOERROR;
    }
    else if (rhsSystem && !lhsSystem) {
        *result = 1;
        return NOERROR;
    }
    else {
        // Either both system engines, or both non system
        // engines.
        //
        // Note, this isn't a typo. Higher priority numbers imply
        // higher priority, but are "lower" in the sort order.
        Int32 rPriorty, lPriorty;
        rhs->GetPriority(&rPriorty);
        lhs->GetPriority(&lPriorty);
        *result = rPriorty - lPriorty;
        return NOERROR;
    }
}

const String Searchables::TAG("Searchables");
const String Searchables::MD_LABEL_DEFAULT_SEARCHABLE("android.app.default_searchable");
const String Searchables::MD_SEARCHABLE_SYSTEM_SEARCH("*");
const AutoPtr<IComparator> Searchables::GLOBAL_SEARCH_RANKER = new Searchables::Comparator();

Searchables::Searchables(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 userId)
    : mContext(ctx)
    , mUserId(userId)
{
    mPm = AppGlobals::GetPackageManager();
}

ECode Searchables::GetSearchableInfo(
    /* [in] */ IComponentName* activity,
    /* [out] */ ISearchableInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    // Step 1.  Is the result already hashed?  (case 1)
    AutoPtr<ISearchableInfo> result;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mSearchablesMap->Get(activity, (IInterface**)&obj);
        result = ISearchableInfo::Probe(obj);
        if (result != NULL) {
            *info = result;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
    }

    // Step 2.  See if the current activity references a searchable.
    // Note:  Conceptually, this could be a while(true) loop, but there's
    // no point in implementing reference chaining here and risking a loop.
    // References must point directly to searchable activities.

    AutoPtr<IActivityInfo> ai;
    //try {
    ECode ec = mPm->GetActivityInfo(activity, IPackageManager::GET_META_DATA, mUserId, (IActivityInfo**)&ai);
    //} catch (RemoteException re) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Error getting activity info 0x%x", ec);
        *info = NULL;
        return NOERROR;
    }
    //}
    String refActivityName;

    // First look for activity-specific reference
    AutoPtr<IBundle> md;
    IPackageItemInfo::Probe(ai)->GetMetaData((IBundle**)&md);
    if (md != NULL) {
        md->GetString(MD_LABEL_DEFAULT_SEARCHABLE, &refActivityName);
    }
    // If not found, try for app-wide reference
    if (refActivityName == NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        md = NULL;
        IPackageItemInfo::Probe(appInfo)->GetMetaData((IBundle**)&md);
        if (md != NULL) {
            md->GetString(MD_LABEL_DEFAULT_SEARCHABLE, &refActivityName);
        }
    }

    // Irrespective of source, if a reference was found, follow it.
    if (refActivityName != NULL) {
        // This value is deprecated, return null
        if (refActivityName.Equals(MD_SEARCHABLE_SYSTEM_SEARCH)) {
            *info = NULL;
            return NOERROR;
        }
        String pkg;
        activity->GetPackageName(&pkg);
        AutoPtr<IComponentName> referredActivity;
        if (refActivityName.GetChar(0) == '.') {
            String param(pkg);
            param += refActivityName;
            CComponentName::New(pkg, param, (IComponentName**)&referredActivity);
        }
        else {
            CComponentName::New(pkg, refActivityName, (IComponentName**)&referredActivity);
        }

        // Now try the referred activity, and if found, cache
        // it against the original name so we can skip the check
        {
            AutoLock syncLock(this);
            AutoPtr<IInterface> obj;
            mSearchablesMap->Get(referredActivity, (IInterface**)&obj);
            result = ISearchableInfo::Probe(obj);
            if (result != NULL) {
                mSearchablesMap->Put(activity, obj);
                *info = result;
                REFCOUNT_ADD(*info);
                return NOERROR;
            }
        }
    }

    // Step 3.  None found. Return null.
    *info = NULL;
    return NOERROR;
}

ECode Searchables::BuildSearchableList()
{
    // These will become the new values at the end of the method
    AutoPtr<IHashMap> newSearchablesMap;
    CHashMap::New((IHashMap**)&newSearchablesMap);
    AutoPtr<IArrayList> newSearchablesList;
    CArrayList::New((IArrayList**)&newSearchablesList);
    AutoPtr<IArrayList> newSearchablesInGlobalSearchList;
    CArrayList::New((IArrayList**)&newSearchablesInGlobalSearchList);

    // Use intent resolver to generate list of ACTION_SEARCH & ACTION_WEB_SEARCH receivers.
    AutoPtr<IList> searchList;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SEARCH, (IIntent**)&intent);

    Int64 ident = Binder::ClearCallingIdentity();

    //try
    {
        searchList = QueryIntentActivities(intent, IPackageManager::GET_META_DATA);
        AutoPtr<IList> webSearchInfoList;
        AutoPtr<IIntent> webSearchIntent;
        CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&webSearchIntent);
        webSearchInfoList = QueryIntentActivities(webSearchIntent, IPackageManager::GET_META_DATA);

        // analyze each one, generate a Searchables record, and record
        if (searchList != NULL || webSearchInfoList != NULL) {
            Int32 search_count = 0;
            if (searchList != NULL) {
                searchList->GetSize(&search_count);
            }
            Int32 web_search_count = 0;
            if (webSearchInfoList != NULL) {
                webSearchInfoList->GetSize(&web_search_count);
            }

            Int32 count = search_count + web_search_count;
            for (Int32 ii = 0; ii < count; ii++) {
                // for each component, try to find metadata
                AutoPtr<IInterface> obj;
                if (ii < search_count) {
                    searchList->Get(ii, (IInterface**)&obj);
                }
                else {
                    webSearchInfoList->Get(ii - search_count, (IInterface**)&obj);
                }
                AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
                AutoPtr<IActivityInfo> ai;
                info->GetActivityInfo((IActivityInfo**)&ai);

                // Check first to avoid duplicate entries.
                String pkgName, name;
                IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
                IPackageItemInfo::Probe(ai)->GetName(&name);
                AutoPtr<IComponentName> cName;
                CComponentName::New(pkgName, name, (IComponentName**)&cName);
                AutoPtr<IInterface> value;
                newSearchablesMap->Get(cName, (IInterface**)&value);
                if (value == NULL) {
                    AutoPtr<ISearchableInfoHelper> helper;
                    CSearchableInfoHelper::AcquireSingleton((ISearchableInfoHelper**)&helper);
                    AutoPtr<ISearchableInfo> searchable;
                    helper->GetActivityMetaData(mContext, ai, mUserId, (ISearchableInfo**)&searchable);
                    if (searchable != NULL) {
                        newSearchablesList->Add(searchable);
                        AutoPtr<IComponentName> sa;
                        searchable->GetSearchActivity((IComponentName**)&sa);
                        newSearchablesMap->Put(sa, searchable);
                        Boolean res;
                        searchable->ShouldIncludeInGlobalSearch(&res);
                        if (res) {
                            newSearchablesInGlobalSearchList->Add(searchable);
                        }
                    }
                }
            }
        }

        AutoPtr<IList> newGlobalSearchActivities = FindGlobalSearchActivities();

        // Find the global search activity
        AutoPtr<IComponentName> newGlobalSearchActivity = FindGlobalSearchActivity(newGlobalSearchActivities);

        // Find the web search activity
        AutoPtr<IComponentName> newWebSearchActivity = FindWebSearchActivity(newGlobalSearchActivity);

        // Store a consistent set of new values
        {
            AutoLock syncLock(this);
            mSearchablesMap = newSearchablesMap;
            mSearchablesList = newSearchablesList;
            mSearchablesInGlobalSearchList = newSearchablesInGlobalSearchList;
            mGlobalSearchActivities = newGlobalSearchActivities;
            mCurrentGlobalSearchActivity = newGlobalSearchActivity;
            mWebSearchActivity = newWebSearchActivity;
        }
    }
    //} finally {

    return Binder::RestoreCallingIdentity(ident);
    //}
}

AutoPtr<IList> Searchables::FindGlobalSearchActivities()
{
    // Step 1 : Query the package manager for a list
    // of activities that can handle the GLOBAL_SEARCH intent.
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_GLOBAL_SEARCH, (IIntent**)&intent);
    AutoPtr<IList> activities = QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY);
    Boolean isEmpty = FALSE;
    if (activities != NULL && !(activities->IsEmpty(&isEmpty), isEmpty)) {
        // Step 2: Rank matching activities according to our heuristics.
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(activities, GLOBAL_SEARCH_RANKER);
    }
    return activities;
}

AutoPtr<IComponentName> Searchables::FindGlobalSearchActivity(
    /* [in] */ IList* installed)
{
    // Fetch the global search provider from the system settings,
    // and if it's still installed, return it.
    String searchProviderSetting = GetGlobalSearchProviderSetting();
    if (!TextUtils::IsEmpty(searchProviderSetting)) {
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        AutoPtr<IComponentName> globalSearchComponent;
        helper->UnflattenFromString(searchProviderSetting, (IComponentName**)&globalSearchComponent);
        if (globalSearchComponent != NULL && IsInstalled(globalSearchComponent)) {
            return globalSearchComponent;
        }
    }

    return GetDefaultGlobalSearchProvider(installed);
}

Boolean Searchables::IsInstalled(
    /* [in] */ IComponentName* globalSearch)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_GLOBAL_SEARCH, (IIntent**)&intent);
    intent->SetComponent(globalSearch);

    AutoPtr<IList> activities = QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY);
    Boolean isEmpty;
    if (activities != NULL && !(activities->IsEmpty(&isEmpty), isEmpty)) {
        return TRUE;
    }
    return FALSE;
}

Boolean Searchables::IsSystemApp(
    /* [in] */ IResolveInfo* res)
{
    AutoPtr<IActivityInfo> activityInfo;
    res->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    return (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
}

AutoPtr<IComponentName> Searchables::GetDefaultGlobalSearchProvider(
    /* [in] */ IList* providerList)
{
    AutoPtr<IComponentName> cName;
    Boolean isEmpty;
    if (providerList != NULL && !(providerList->IsEmpty(&isEmpty), isEmpty)) {
        AutoPtr<IInterface> temp;
        providerList->Get(0, (IInterface**)&temp);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
        if (ri) {
            AutoPtr<IActivityInfo> ai;
            ri->GetActivityInfo((IActivityInfo**)&ai);
            String pkgName, name;
            IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
            IPackageItemInfo::Probe(ai)->GetName(&name);
            CComponentName::New(pkgName, name, (IComponentName**)&cName);
        }
    }

    Logger::W(TAG, "No global search activity found");
    return cName;
}

String Searchables::GetGlobalSearchProviderSetting()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String settings;
    secure->GetString(cr, ISettingsSecure::SEARCH_GLOBAL_SEARCH_ACTIVITY, &settings);
    return settings;
}

AutoPtr<IComponentName> Searchables::FindWebSearchActivity(
    /* [in] */ IComponentName* globalSearchActivity)
{
    if (globalSearchActivity == NULL) {
        return NULL;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
    String pkgName;
    globalSearchActivity->GetPackageName(&pkgName);
    intent->SetPackage(pkgName);
    AutoPtr<IList> activities = QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY);

    AutoPtr<IComponentName> cName;
    Boolean isEmpty;
    if (activities != NULL && !(activities->IsEmpty(&isEmpty), isEmpty)) {
        AutoPtr<IInterface> temp;
        activities->Get(0, (IInterface**)&temp);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
        if (ri) {
            AutoPtr<IActivityInfo> ai;
            ri->GetActivityInfo((IActivityInfo**)&ai);
            String pkgName, name;
            IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
            IPackageItemInfo::Probe(ai)->GetName(&name);
            CComponentName::New(pkgName, name, (IComponentName**)&cName);
        }
    }
    Logger::W(TAG, "No web search activity found");
    return cName;
}

AutoPtr<IList> Searchables::QueryIntentActivities(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String type;
    intent->ResolveTypeIfNeeded(cr, &type);
    AutoPtr<IList> activities;
    mPm->QueryIntentActivities(intent, type, flags, mUserId, (IList**)&activities);
    return activities;
}

ECode Searchables::GetSearchablesList(
    /* [out] */ IList** list)
{
    AutoLock lock(this);
    return CArrayList::New(ICollection::Probe(mSearchablesList), list);
}

ECode Searchables::GetSearchablesInGlobalSearchList(
    /* [out] */ IList** list)
{
    AutoLock lock(this);
    return CArrayList::New(ICollection::Probe(mSearchablesInGlobalSearchList), list);
}

ECode Searchables::GetGlobalSearchActivities(
    /* [out] */ IList** list)
{
    AutoLock lock(this);
    return CArrayList::New(ICollection::Probe(mGlobalSearchActivities), list);
}

ECode Searchables::GetGlobalSearchActivity(
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    AutoLock lock(this);
    *cName = mCurrentGlobalSearchActivity;
    REFCOUNT_ADD(*cName);
    return NOERROR;
}

ECode Searchables::GetWebSearchActivity(
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    AutoLock lock(this);
    *cName = mWebSearchActivity;
    REFCOUNT_ADD(*cName);
    return NOERROR;
}

}// Search
}// Server
}// Droid
}// Elastos
