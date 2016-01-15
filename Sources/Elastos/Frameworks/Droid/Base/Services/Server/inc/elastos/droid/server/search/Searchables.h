#ifndef __ELASTOS_DROID_SERVER_SEARCH_SEARCHABLES_H__
#define __ELASTOS_DROID_SERVER_SEARCH_SEARCHABLES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Core.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::App::ISearchableInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {


class Searchables
    : public ISearchables
    , public ElRefBase
{
public:

    CAR_INTERFACE_DECL()

    Searchables(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 userId);

    CARAPI GetSearchableInfo(
        /* [in] */ IComponentName* activity,
        /* [out] */ ISearchableInfo** info);

    CARAPI BuildSearchableList();

    CARAPI GetSearchablesList(
        /* [out] */ IObjectContainer** list);

    CARAPI GetSearchablesInGlobalSearchList(
        /* [out] */ IObjectContainer** list);

    CARAPI GetGlobalSearchActivities(
        /* [out] */ IObjectContainer** list);

    CARAPI GetGlobalSearchActivity(
        /* [out] */ IComponentName** cName);

    CARAPI GetWebSearchActivity(
        /* [out] */ IComponentName** cName);

private:

    CARAPI FindGlobalSearchActivities(
        /* [out] */ IObjectContainer** container);

    CARAPI FindGlobalSearchActivity(
        /* [in] */ IObjectContainer* installed,
        /* [out] */IComponentName** cName);

    CARAPI IsInstalled(
        /* [in] */ IComponentName* globalSearch,
        /* [out] */ Boolean* rst);

    static CARAPI_(Int32) ComparatorResolveInfo(
        /* [in] */ IResolveInfo* lhs,
        /* [in] */ IResolveInfo* rhs);

    static CARAPI IsSystemApp(
        /* [in] */ IResolveInfo* res,
        /* [out] */ Boolean* rst);

    CARAPI GetDefaultGlobalSearchProvider(
        /* [in] */ IObjectContainer* providerList,
        /* [out] */ IComponentName** cName);

    CARAPI GetGlobalSearchProviderSetting(
        /* [out] */ String* settings);

    CARAPI FindWebSearchActivity(
        /* [in] */ IComponentName* globalSearchActivity,
        /* [out] */ IComponentName** cName);

    CARAPI QueryIntentActivities(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IObjectContainer** infos);

    template<typename T>
        AutoPtr<IObjectContainer> TransfromList(
            /* [in] */ List<AutoPtr<T> >* list);

    template<typename T>
        AutoPtr< List<AutoPtr<T> > > TransfromContainer(
            /* [in] */ IObjectContainer* list);

private:
    static const String TAG;// = "Searchables";

    // static strings used for XML lookups, etc.
    // TODO how should these be documented for the developer, in a more structured way than
    // the current long wordy javadoc in SearchManager.java ?
    static const String MD_LABEL_DEFAULT_SEARCHABLE;// = "android.app.default_searchable";
    static const String MD_SEARCHABLE_SYSTEM_SEARCH;// = "*";

    AutoPtr<IContext> mContext;

    AutoPtr< HashMap<AutoPtr<IComponentName>, AutoPtr<ISearchableInfo> > > mSearchablesMap;
    AutoPtr<IObjectContainer> mSearchablesList;
    AutoPtr<IObjectContainer> mSearchablesInGlobalSearchList;
    // Contains all installed activities that handle the global search
    // intent.
    AutoPtr<IObjectContainer> mGlobalSearchActivities;
    AutoPtr<IComponentName> mCurrentGlobalSearchActivity;
    AutoPtr<IComponentName> mWebSearchActivity;



    // Cache the package manager instance
    AutoPtr<IIPackageManager> mPm;
    // User for which this Searchables caches information
    Int32 mUserId;

    Object mLock;

    typedef HashMap<AutoPtr<IComponentName>, AutoPtr<ISearchableInfo> >::Iterator Iterator;
    typedef HashMap<AutoPtr<IComponentName>, AutoPtr<ISearchableInfo> > CSHashMap;
    typedef List<AutoPtr<ISearchableInfo*> > SList;
    typedef List<AutoPtr<IResolveInfo*> > RList;
};

}// Search
}// Server
}// Droid
}// Elastos

#endif

