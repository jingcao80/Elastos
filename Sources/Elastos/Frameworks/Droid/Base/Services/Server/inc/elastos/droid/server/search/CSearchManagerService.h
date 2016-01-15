#ifndef __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICE_H__

#include "Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Search_CSearchManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/PackageMonitor.h"
#include "elastos/droid/database/ContentObserver.h"
#include "search/Searchables.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::App::IISearchManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CarClass(CSearchManagerService)
{
public:

    CSearchManagerService();

    CSearchManagerService(
        /* [in] */ IContext* cxt);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetSearchableInfo (
        /* [in] */ IComponentName* launchActivity,
        /* [out] */ ISearchableInfo** info);

    CARAPI GetSearchablesInGlobalSearch(
        /* [out] */ IObjectContainer** infos);

    CARAPI GetGlobalSearchActivities(
        /* [out] */ IObjectContainer** infos);

    CARAPI GetGlobalSearchActivity(
        /* [out] */ IComponentName** name);

    CARAPI GetWebSearchActivity(
        /* [out] */ IComponentName** name);

    CARAPI GetAssistIntent(
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** name);

    CARAPI ToString(
        /* [out] */ String* str);
protected:
    CARAPI Init(
        /* [in] */ IContext* context);
private:

    CARAPI GetSearchables(
        /* [in] */ Int32 userId,
        /* [out] */ ISearchables** rst);

    CARAPI OnUserRemoved(
        /* [in] */ Int32 userId);

    class BootCompletedReceiver
        : public BroadcastReceiver
    {
    private:
        class MyThread : public ThreadBase
        {
        public:
            MyThread(
                /* [in] */ BootCompletedReceiver* host);

            CARAPI Run();

        private:
            BootCompletedReceiver* mHost;
        };

    public:
        BootCompletedReceiver(
            /* [in] */ CSearchManagerService* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CSearchManagerService::BootCompletedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CSearchManagerService* mHost;
    };

    class UserReceiver
        : public BroadcastReceiver
    {
    public:
        UserReceiver(
            /* [in] */ CSearchManagerService* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CSearchManagerService::UserReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CSearchManagerService* mHost;
    };

    class MyPackageMonitor
        : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ CSearchManagerService* host);

        CARAPI OnSomePackagesChanged();

        CARAPI OnPackageModified(
            /* [in] */ const String& pkg);
    private:
        CARAPI_(void) UpdateSearchables();

    private:
        CSearchManagerService* mHost;
    };

    class GlobalSearchProviderObserver
        : public ContentObserver
    {
    public:
        GlobalSearchProviderObserver(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ CSearchManagerService* host);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        AutoPtr<IContentResolver> mResolver;
        CSearchManagerService* mHost;
    };
private:

    // general debugging support
    static const String TAG;// = "SearchManagerService";

    // Context that the service is running in.
    AutoPtr<IContext> mContext;

    // This field is initialized lazily in getSearchables(), and then never modified.
    typedef HashMap<Int32, AutoPtr<ISearchables> > IntHashMap;
    typedef HashMap<Int32, AutoPtr<ISearchables> >::Iterator Iterator;
    IntHashMap mSearchables;//new SparseArray<Searchables>();
    Object mLock;
};

}// Search
}// Server
}// Droid
}// Elastos
#endif
