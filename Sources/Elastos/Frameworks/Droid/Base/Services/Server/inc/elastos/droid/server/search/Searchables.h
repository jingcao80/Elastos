#ifndef __ELASTOS_DROID_SERVER_SEARCH_SEARCHABLES_H__
#define __ELASTOS_DROID_SERVER_SEARCH_SEARCHABLES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::ISearchableInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Core::IComparator;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

class Searchables : public Object
{
private:
    class Comparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    Searchables(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 userId);

    CARAPI GetSearchableInfo(
        /* [in] */ IComponentName* activity,
        /* [out] */ ISearchableInfo** info);

    CARAPI BuildSearchableList();

    CARAPI GetSearchablesList(
        /* [out] */ IList** list);

    CARAPI GetSearchablesInGlobalSearchList(
        /* [out] */ IList** list);

    CARAPI GetGlobalSearchActivities(
        /* [out] */ IList** list);

    CARAPI GetGlobalSearchActivity(
        /* [out] */ IComponentName** cName);

    CARAPI GetWebSearchActivity(
        /* [out] */ IComponentName** cName);

private:
    CARAPI_(AutoPtr<IList>) FindGlobalSearchActivities();

    CARAPI_(AutoPtr<IComponentName>) FindGlobalSearchActivity(
        /* [in] */ IList* installed);

    CARAPI_(Boolean) IsInstalled(
        /* [in] */ IComponentName* globalSearch);

    static CARAPI_(Boolean) ComparatorResolveInfo(
        /* [in] */ IResolveInfo* lhs,
        /* [in] */ IResolveInfo* rhs);

    static CARAPI_(Boolean) IsSystemApp(
        /* [in] */ IResolveInfo* res);

    CARAPI_(AutoPtr<IComponentName>) GetDefaultGlobalSearchProvider(
        /* [in] */ IList* providerList);

    CARAPI_(String) GetGlobalSearchProviderSetting();

    CARAPI_(AutoPtr<IComponentName>) FindWebSearchActivity(
        /* [in] */ IComponentName* globalSearchActivity);

    CARAPI_(AutoPtr<IList>) QueryIntentActivities(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags);

private:
    static const String TAG;

    // static strings used for XML lookups, etc.
    // TODO how should these be documented for the developer, in a more structured way than
    // the current long wordy javadoc in SearchManager.java ?
    static const String MD_LABEL_DEFAULT_SEARCHABLE;
    static const String MD_SEARCHABLE_SYSTEM_SEARCH;

    static const AutoPtr<IComparator> GLOBAL_SEARCH_RANKER;

    AutoPtr<IContext> mContext;

    AutoPtr<IHashMap> mSearchablesMap;
    AutoPtr<IArrayList> mSearchablesList;
    AutoPtr<IArrayList> mSearchablesInGlobalSearchList;
    // Contains all installed activities that handle the global search
    // intent.
    AutoPtr<IList> mGlobalSearchActivities;
    AutoPtr<IComponentName> mCurrentGlobalSearchActivity;
    AutoPtr<IComponentName> mWebSearchActivity;

    // Cache the package manager instance
    AutoPtr<IIPackageManager> mPm;
    // User for which this Searchables caches information
    Int32 mUserId;
};

}// Search
}// Server
}// Droid
}// Elastos

#endif

