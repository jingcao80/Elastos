
#include "elastos/droid/server/search/Searchables.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/text/TextUtils.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

const String Searchables::TAG("Searchables");
const String Searchables::MD_LABEL_DEFAULT_SEARCHABLE("android.app.default_searchable");
const String Searchables::MD_SEARCHABLE_SYSTEM_SEARCH("*");

CAR_INTERFACE_IMPL(Searchables, Object, ISearchables)

Searchables::Searchables(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 userId)
    : mUserId(0)
    , mContext(ctx)
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
    {    AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mSearchablesMap->Get(TO_IINTERFACE(activity), (IInterface**)&obj);
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
    if (ec == (ECode)RemoteException) {
        Slogger::E(TAG, "Error getting activity info %x" + ec);
        *info = NULL;
        return NOERROR;
    }
    //}
    String refActivityName(NULL);

    // First look for activity-specific reference
    AutoPtr<IBundle> md;
    ai->GetMetaData((IBundle**)&md);
    if (md != NULL) {
        md->GetString(MD_LABEL_DEFAULT_SEARCHABLE, &refActivityName);
    }
    // If not found, try for app-wide reference
    if (refActivityName == NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        ai->GetApplicationInfo((IApplicationInfo**)&appInfo);
        md = NULL;
        appInfo->GetMetaData((IBundle**)&md);
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
        {    AutoLock syncLock(this);
            AutoPtr<IInterface> obj;
            mSearchablesMap->Get(TO_IINTERFACE(referredActivity), (IInterface**)&obj);
            result = ISearchableInfo::Probe(obj);
            if (result != NULL) {
                mSearchablesMap->Put(TO_IINTERFACE(activity), obj);
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

    Int64 ident;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&ident);

    //try
    {
        assert(0 && "try-finally");
        QueryIntentActivities(intent, IPackageManager::GET_META_DATA, (IList**)&searchList);
        AutoPtr<IList> webSearchInfoList;
        AutoPtr<IIntent> webSearchIntent;
        CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&webSearchIntent);
        QueryIntentActivities(webSearchIntent, IPackageManager::GET_META_DATA,
                (IList**)&webSearchInfoList);

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
                AutoPtr<IComponentName> name;
                CComponentName::New(ai.packageName, ai.name); IPackageItemInfo
                String pkgName, name;
                IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
                IPackageItemInfo::Probe(ai)->GetName(&name);
                AutoPtr<IComponentName> cName;
                CComponentName::New(pkgName, name, (IComponentName**)&cName);
                AutoPtr<IInterface> value;
                newSearchablesMap->Get(TO_IINTERFACE(cName), (IInterface**)&value);
                if (value == NULL) {
                    AutoPtr<ISearchableInfoHelper> helper;
                    CSearchableInfoHelper::AcquireSingleton((ISearchableInfoHelper**)&helper);
                    AutoPtr<ISearchableInfo> searchable;
                    helper->GetActivityMetaData(mContext, ai, mUserId, (ISearchableInfo**)&searchable);
                    if (searchable != NULL) {
                        newSearchablesList->Add(TO_IINTERFACE(searchable));
                        newSearchablesMap->Put(searchable.getSearchActivity(), TO_IINTERFACE(searchable));
                        Boolean res;
                        searchable->ShouldIncludeInGlobalSearch(&res);
                        if (res) {
                            newSearchablesInGlobalSearchList->Add(TO_IINTERFACE(searchable));
                        }
                    }
                }
            }
        }

        AutoPtr<IList> newGlobalSearchActivities;
        FindGlobalSearchActivities((IList**)&newGlobalSearchActivities);

        // Find the global search activity
        AutoPtr<IComponentName> newGlobalSearchActivity;
        FindGlobalSearchActivity(newGlobalSearchActivities, (IComponentName**)&newGlobalSearchActivity);

        // Find the web search activity
        AutoPtr<IComponentName> newWebSearchActivity;
        FindWebSearchActivity(newGlobalSearchActivity, (IComponentName**)&newWebSearchActivity);

        // Store a consistent set of new values
        {    AutoLock syncLock(this);
            mSearchablesMap = newSearchablesMap;
            mSearchablesList = newSearchablesList;
            mSearchablesInGlobalSearchList = newSearchablesInGlobalSearchList;
            mGlobalSearchActivities = newGlobalSearchActivities;
            mCurrentGlobalSearchActivity = newGlobalSearchActivity;
            mWebSearchActivity = newWebSearchActivity;
        }
    }
    //} finally {
FINALLY:
    return binderHelper->RestoreCallingIdentity(ident);
    //}
}

ECode Searchables::GetSearchablesList(
    /* [out] */ IObjectContainer** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mSearchablesList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Searchables::GetSearchablesInGlobalSearchList(
    /* [out] */ IObjectContainer** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mSearchablesInGlobalSearchList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Searchables::GetGlobalSearchActivities(
    /* [out] */ IObjectContainer** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mGlobalSearchActivities;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Searchables::GetGlobalSearchActivity(
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = mCurrentGlobalSearchActivity;
    REFCOUNT_ADD(*cName);
    return NOERROR;
}

ECode Searchables::GetWebSearchActivity(
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = mWebSearchActivity;
    REFCOUNT_ADD(*cName);
    return NOERROR;
}

ECode Searchables::FindGlobalSearchActivities(
    /* [out] */ IObjectContainer** container)
{
    VALIDATE_NOT_NULL(container);
    // Step 1 : Query the package manager for a list
    // of activities that can handle the GLOBAL_SEARCH intent.
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_GLOBAL_SEARCH, (IIntent**)&intent);
    AutoPtr<IObjectContainer> activities;
    QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IObjectContainer**)&activities);
    Int32 count;
    if (activities != NULL && (activities->GetObjectCount(&count), count) != 0) {
        // Step 2: Rank matching activities according to our heuristics.
        AutoPtr< List<AutoPtr<IResolveInfo> > > listTemp = TransfromContainer<IResolveInfo>(activities);
        listTemp->Sort(Searchables::ComparatorResolveInfo);
        activities = TransfromList(listTemp.Get());
    }
    *container = activities;
    REFCOUNT_ADD(*container);
    return NOERROR;
}

ECode Searchables::FindGlobalSearchActivity(
    /* [in] */ IObjectContainer* installed,
    /* [out] */IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = NULL;

    // Fetch the global search provider from the system settings,
    // and if it's still installed, return it.
    String searchProviderSetting;
    GetGlobalSearchProviderSetting(&searchProviderSetting);
    if (!TextUtils::IsEmpty(searchProviderSetting)) {
        AutoPtr<IComponentName> globalSearchComponent;
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        helper->UnflattenFromString(searchProviderSetting, (IComponentName**)&globalSearchComponent);
        Boolean isInstalled;
        if (globalSearchComponent != NULL && (IsInstalled(globalSearchComponent, &isInstalled),isInstalled)) {
            *cName = globalSearchComponent;
            REFCOUNT_ADD(*cName);
        }
    }

    return GetDefaultGlobalSearchProvider(installed, cName);
}

ECode Searchables::IsInstalled(
    /* [in] */ IComponentName* globalSearch,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst =  FALSE;

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(globalSearch);

    AutoPtr<IObjectContainer> activities;
    QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IObjectContainer**)&activities);
    Int32 count;
    if (activities != NULL && (activities->GetObjectCount(&count), count) > 0) {
        *rst = TRUE;
    }
    return NOERROR;
}

Boolean Searchables::ComparatorResolveInfo(
    /* [in] */ IResolveInfo* lhs,
    /* [in] */ IResolveInfo* rhs)
{
    if (lhs == rhs)
        return TRUE;
    Boolean lhsSystem, rhsSystem;
    IsSystemApp(lhs, &lhsSystem);
    IsSystemApp(rhs, &rhsSystem);

    if (lhsSystem && !rhsSystem) {
        return TRUE;
    }
    else if (rhsSystem && !lhsSystem) {
        return FALSE;
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
        Int32 ival = rPriorty - lPriorty;
        if (ival <= 0)
            return TRUE;
        return FALSE;
    }
}

ECode Searchables::IsSystemApp(
    /* [in] */ IResolveInfo* res,
    /* [out] */ Boolean* rst)
{
    AutoPtr<IActivityInfo> activityInfo;
    res->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> appInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    *rst = (flags & IApplicationInfo::FLAG_SYSTEM);
    return NOERROR;
}

ECode Searchables::GetDefaultGlobalSearchProvider(
    /* [in] */ IObjectContainer* providerList,
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = NULL;

    Int32 count;
    if (providerList != NULL && (providerList->GetObjectCount(&count)) != 0) {
        AutoPtr<IObjectEnumerator> em;
        providerList->GetObjectEnumerator((IObjectEnumerator**)&em);
        Boolean next;
        em->MoveNext(&next);
        if (next) {
            AutoPtr<IInterface> temp;
            em->Current((IInterface**)&temp);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
            if (ri) {
                AutoPtr<IActivityInfo> ai;
                ri->GetActivityInfo((IActivityInfo**)&ai);
                String pkgName, name;
                ai->GetPackageName(&pkgName);
                ai->GetName(&name);
                return CComponentName::New(pkgName, name, cName);
            }
        }
    }

    // Log.w(LOG_TAG, "No global search activity found");
    return NOERROR;
}

ECode Searchables::GetGlobalSearchProviderSetting(
    /* [out] */ String* settings)
{
    VALIDATE_NOT_NULL(settings);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    return secure->GetString(cr, ISettingsSecure::SEARCH_GLOBAL_SEARCH_ACTIVITY, settings);
}

ECode Searchables::FindWebSearchActivity(
    /* [in] */ IComponentName* globalSearchActivity,
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = NULL;

    if (globalSearchActivity == NULL) {
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
    String pkgName;
    globalSearchActivity->GetPackageName(&pkgName);
    intent->SetPackage(pkgName);
    AutoPtr<IObjectContainer> activities;
    QueryIntentActivities(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IObjectContainer**)&activities);

    Int32 count;
    if (activities != NULL && (activities->GetObjectCount(&count), count) > 0) {

        AutoPtr<IObjectEnumerator> em;
        activities->GetObjectEnumerator((IObjectEnumerator**)&em);
        Boolean next;
        em->MoveNext(&next);
        if (next) {
            AutoPtr<IInterface> temp;
            em->Current((IInterface**)&temp);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
            if (ri) {
                AutoPtr<IActivityInfo> ai;
                ri->GetActivityInfo((IActivityInfo**)&ai);
                String pkgName, name;
                ai->GetPackageName(&pkgName);
                ai->GetName(&name);
                // TODO: do some sanity checks here?
                return CComponentName::New(pkgName, name, cName);
            }
        }
    }
    // Log.w(LOG_TAG, "No web search activity found");
    return NOERROR;
}

ECode Searchables::QueryIntentActivities(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [out] */ List** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String type;
    intent->ResolveTypeIfNeeded(cr, &type);
    return mPm->QueryIntentActivities(intent, type, flags, mUserId, infos);
}

template<typename T>
AutoPtr<IObjectContainer> Searchables::TransfromList(
        /* [in] */ List<AutoPtr<T> >* list)
{
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    typename List<AutoPtr<T> >::Iterator it = list->Begin();
    for(; it != list->End(); it++) {
        container->Add(*it);
    }
    return container;
}

template<typename T>
AutoPtr< List<AutoPtr<T> > > Searchables::TransfromContainer(
        /* [in] */ IObjectContainer* container)
{
    AutoPtr< List<AutoPtr<T> > > list = new List<AutoPtr<T> >();
    AutoPtr<IObjectEnumerator> em;
    container->GetObjectEnumerator((IObjectEnumerator**)&em);
    Boolean next = FALSE;
    while(em->MoveNext(&next), next) {
        AutoPtr<IInterface> temp;
        em->Current((IInterface**)&temp);
        AutoPtr<T> obj = T::Probe(temp);
        list->PushBack(obj);
    }
    return list;
}

}// Search
}// Server
}// Droid
}// Elastos
