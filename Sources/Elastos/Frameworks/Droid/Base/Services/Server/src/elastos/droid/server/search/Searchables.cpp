#include "search/Searchables.h"
#include "elastos/droid/text/TextUtils.h"

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

CAR_INTERFACE_IMPL(Searchables, ISearchables)

Searchables::Searchables(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 userId)
    : mUserId(0)
{
    mContext = ctx;
    mUserId = userId;
    AutoPtr<IActivityThreadHelper> helper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
    helper->GetPackageManager((IIPackageManager**)&mPm);
}

ECode Searchables::GetSearchableInfo(
    /* [in] */ IComponentName* activity,
    /* [out] */ ISearchableInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    // Step 1.  Is the result already hashed?  (case 1)
    {
        AutoLock lock(mLock);
        Iterator it = mSearchablesMap->Find(activity);
        if(it != mSearchablesMap->End() && it->mSecond != NULL)
        {
            *info = it->mSecond;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
    }

    // Step 2.  See if the current activity references a searchable.
    // Note:  Conceptually, this could be a while(true) loop, but there's
    // no point in implementing reference chaining here and risking a loop.
    // References must point directly to searchable activities.
    AutoPtr<IActivityInfo> ai;
    // try {

    FAIL_RETURN(mPm->GetActivityInfo(activity, IPackageManager::GET_META_DATA, mUserId, (IActivityInfo**)&ai));

    // } catch (RemoteException re) {
    //     Log.e(LOG_TAG, "Error getting activity info " + re);
    //     return null;
    // }
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
    if (refActivityName != NULL)
    {
        // This value is deprecated, return null
        if (refActivityName.Equals(MD_SEARCHABLE_SYSTEM_SEARCH)) {
            *info = NULL;
            return NOERROR;
        }
        String pkg;
        activity->GetPackageName(&pkg);
        String param(pkg);
        param += refActivityName;
        AutoPtr<IComponentName> referredActivity;
        if (refActivityName.GetChar(0) == '.') {
            CComponentName::New(pkg, param, (IComponentName**)&referredActivity);
        } else {
            CComponentName::New(pkg, refActivityName, (IComponentName**)&referredActivity);
        }

        // Now try the referred activity, and if found, cache
        // it against the original name so we can skip the check
        {
            AutoLock lock(mLock);
            // TODO: you will can't find item forever
            //
            Iterator it = mSearchablesMap->Find(referredActivity);
            if(it != mSearchablesMap->End() && it->mSecond != NULL)
            {
                *info = it->mSecond;
                REFCOUNT_ADD(*info);
                (*mSearchablesMap)[activity] = *info;
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
    mSearchablesMap = new CSHashMap();
    mSearchablesList = NULL;
    CObjectContainer::New((IObjectContainer**)&mSearchablesList);
    mSearchablesInGlobalSearchList = NULL;
    CObjectContainer::New((IObjectContainer**)&mSearchablesInGlobalSearchList);

    // Use intent resolver to generate list of ACTION_SEARCH & ACTION_WEB_SEARCH receivers.
    AutoPtr<IObjectContainer> searchList;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SEARCH, (IIntent**)&intent);
    Int64 ident;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&ident);
    QueryIntentActivities(intent, IPackageManager::GET_META_DATA, (IObjectContainer**)&searchList);
    AutoPtr<IObjectContainer> webSearchInfoList;
    AutoPtr<IIntent> webSearchIntent;
    CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&webSearchIntent);
    QueryIntentActivities(webSearchIntent, IPackageManager::GET_META_DATA, (IObjectContainer**)&webSearchInfoList);
    // analyze each one, generate a Searchables record, and record

    if (searchList != NULL || webSearchInfoList != NULL) {
        Int32 search_count = 0;
        if(searchList)
            searchList->GetObjectCount(&search_count);
        Int32 web_search_count = 0;
        if(webSearchInfoList)
            webSearchInfoList->GetObjectCount(&web_search_count);

        Int32 count = search_count + web_search_count;
        AutoPtr<IObjectEnumerator> sEm;
        AutoPtr<IObjectEnumerator> wEm;
        searchList->GetObjectEnumerator((IObjectEnumerator**)&sEm);
        webSearchInfoList->GetObjectEnumerator((IObjectEnumerator**)&wEm);
        for (Int32 ii = 0; ii < count; ii++) {
            // for each component, try to find metadata
            AutoPtr<IResolveInfo> rInfo;
            Boolean next = FALSE;
            if (ii < search_count)
            {
                if (sEm->MoveNext(&next), next)
                {
                    AutoPtr<IInterface> temp;
                    sEm->Current((IInterface**)&temp);
                    rInfo = IResolveInfo::Probe(temp);
                }
            } else {
                if (wEm->MoveNext(&next), next)
                {
                    AutoPtr<IInterface> temp;
                    wEm->Current((IInterface**)&temp);
                    rInfo = IResolveInfo::Probe(temp);
                }
            }
            AutoPtr<IActivityInfo> ai;
            rInfo->GetActivityInfo((IActivityInfo**)&ai);
            // Check first to avoid duplicate entries.
            String pkgName, name;
            ai->GetPackageName(&pkgName);
            ai->GetName(&name);
            AutoPtr<IComponentName> cName;
            CComponentName::New(pkgName, name, (IComponentName**)&cName);
            // TODO: you will can't find item forever
            //
            Iterator it = mSearchablesMap->Find(cName);

            if (it != mSearchablesMap->End() && it->mSecond != NULL) {
                AutoPtr<ISearchableInfo> searchable;// = SearchableInfo.getActivityMetaData(mContext, ai, mUserId);
                if (searchable != NULL) {
                    mSearchablesList->Add(searchable);
                    AutoPtr<IComponentName> key;
                    searchable->GetSearchActivity((IComponentName**)&key);
                    (*mSearchablesMap)[key] = searchable;
                    Boolean should;
                    if (searchable->ShouldIncludeInGlobalSearch(&should), should) {
                        mSearchablesInGlobalSearchList->Add(searchable);
                    }
                }
            }
        }
    }

    // Find the global search activity
    mGlobalSearchActivities = NULL;
    FindGlobalSearchActivities((IObjectContainer**)&mGlobalSearchActivities);
    mCurrentGlobalSearchActivity = NULL;
    FindGlobalSearchActivity(mGlobalSearchActivities, (IComponentName**)&mCurrentGlobalSearchActivity);

    // Find the global search activity
    mWebSearchActivity = NULL;
    FindWebSearchActivity(mCurrentGlobalSearchActivity, (IComponentName**)&mWebSearchActivity);

    binderHelper->RestoreCallingIdentity(ident);
    return NOERROR;
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
    if (activities != NULL && (activities->GetObjectCount(&count), count) != 0)
    {
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
    if (activities != NULL && (activities->GetObjectCount(&count), count) > 0)
    {
        *rst = TRUE;
    }
    return NOERROR;
}

Boolean Searchables::ComparatorResolveInfo(
    /* [in] */ IResolveInfo* lhs,
    /* [in] */ IResolveInfo* rhs)
{
    if(lhs == rhs)
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
        if(next)
        {
            AutoPtr<IInterface> temp;
            em->Current((IInterface**)&temp);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
            if (ri)
            {
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
    return  secure->GetString(cr, ISettingsSecure::SEARCH_GLOBAL_SEARCH_ACTIVITY, settings);
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
        if(next)
        {
            AutoPtr<IInterface> temp;
            em->Current((IInterface**)&temp);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(temp);
            if (ri)
            {
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
    /* [out] */ IObjectContainer** infos)
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
    for(; it != list->End(); it++)
    {
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
    while(em->MoveNext(&next), next)
    {
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
