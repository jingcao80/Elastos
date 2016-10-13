#ifndef __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Search_CSearchManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/server/search/Searchables.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IISearchManager;
using Elastos::Droid::App::ISearchableInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IList;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CarClass(CSearchManagerService)
    , public Object
    , public IISearchManager
    , public IBinder
{
public:
    class BootCompletedReceiver
        : public BroadcastReceiver
    {
    private:
        class MyThread : public Thread
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

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CSearchManagerService::BootCompletedReceiver: ")

    private:
        CSearchManagerService* mHost;
    };

    class UserReceiver
        : public BroadcastReceiver
    {
    public:
        UserReceiver(
            /* [in] */ CSearchManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CSearchManagerService::UserReceiver: ")

    private:
        CSearchManagerService* mHost;
    };

    class GlobalSearchProviderObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CSearchManagerService::GlobalSearchProviderObserver")

        GlobalSearchProviderObserver();

        CARAPI constructor(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ CSearchManagerService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IContentResolver> mResolver;
        CSearchManagerService* mHost;
    };

private:
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

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSearchManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetSearchableInfo (
        /* [in] */ IComponentName* launchActivity,
        /* [out] */ ISearchableInfo** info);

    CARAPI GetSearchablesInGlobalSearch(
        /* [out] */ IList** infos);

    CARAPI GetGlobalSearchActivities(
        /* [out] */ IList** infos);

    CARAPI GetGlobalSearchActivity(
        /* [out] */ IComponentName** name);

    CARAPI GetWebSearchActivity(
        /* [out] */ IComponentName** name);

    CARAPI GetAssistIntent(
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** name);

    CARAPI LaunchAssistAction(
        /* [in] */ Int32 requestType,
        /* [in] */ const String& hint,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(AutoPtr<Searchables>) GetSearchables(
        /* [in] */ Int32 userId);

    CARAPI OnUserRemoved(
        /* [in] */ Int32 userId);

private:
    // general debugging support
    static const String TAG;

    // Context that the service is running in.
    AutoPtr<IContext> mContext;

    // This field is initialized lazily in getSearchables(), and then never modified.
    AutoPtr<ISparseArray> mSearchables;
};

}// Search
}// Server
}// Droid
}// Elastos
#endif
