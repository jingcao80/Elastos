
#ifndef __ELASTOS_DROID_SERVER_CAPPWIDGETSERVICE_H__
#define __ELASTOS_DROID_SERVER_CAPPWIDGETSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CAppWidgetService.h"
#include "AppWidgetServiceImpl.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.CoreLibrary.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::AppWidget::IIAppWidgetService;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Redirects calls to this service to the instance of the service for the appropriate user.
 */

CarClass(CAppWidgetService)
{
private:
    class UserBroadcastReceiver : public BroadcastReceiver
    {
    public:
        UserBroadcastReceiver(
            /* [in] */ CAppWidgetService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAppWidgetService::UserBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CAppWidgetService* mHost;
    };

    class SysBroadcastReceiver: public BroadcastReceiver
    {
    public:
        SysBroadcastReceiver(
            /* [in] */ CAppWidgetService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAppWidgetService::SysBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CAppWidgetService*  mHost;
    };

public:
    CAppWidgetService();

    ~CAppWidgetService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemReady(
        /* [in] */ Boolean safeMode);

    CARAPI AllocateAppWidgetId(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId,
        /* [out] */ Int32* id);

    CARAPI GetAppWidgetIdsForHost(
        /* [in] */ Int32 hostId,
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI DeleteAppWidgetId(
        /* [in] */ Int32 appWidgetId);

    CARAPI DeleteHost(
        /* [in] */ Int32 hostId);

    CARAPI DeleteAllHosts();

    CARAPI BindAppWidgetId(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IComponentName* provider,
        /* [in] */ IBundle* options);

    CARAPI BindAppWidgetIdIfAllowed(
         /* [in] */ const String& packageName,
         /* [in] */ Int32 appWidgetId,
         /* [in] */ IComponentName* provider,
         /* [in] */ IBundle* options,
         /* [out] */ Boolean* result);

    CARAPI HasBindAppWidgetPermission(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI SetBindAppWidgetPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean permission);

    CARAPI BindRemoteViewsService(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* connection,
        /* [in] */ Int32 userId);

    CARAPI StartListening(
        /* [in] */ IIAppWidgetHost* host,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId,
        /* [out] */ IObjectContainer** updatedViews,
        /* [out, callee] */ ArrayOf<Int32>** out);

    CARAPI StartListeningAsUser(
        /* [in] */ IIAppWidgetHost* host,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId,
        /* [out] */ IObjectContainer** updatedViews,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Int32>** out);

    CARAPI OnUserRemoved(
        /* [in] */ Int32 userId);

    CARAPI OnUserStopping(
        /* [in] */ Int32 userId);

    CARAPI GetAppWidgetIds(
        /* [in] */ IComponentName* provider,
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI GetAppWidgetInfo(
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IAppWidgetProviderInfo** info);

    CARAPI GetAppWidgetViews(
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IParcelable** views);

    CARAPI UpdateAppWidgetOptions(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IBundle* options);

    CARAPI GetAppWidgetOptions(
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IBundle** options);

    CARAPI GetInstalledProviders(
        /* [in] */ Int32 categoryFilter,
        /* [out] */ IObjectContainer** providers);

    CARAPI NotifyAppWidgetViewDataChanged(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ Int32 viewId);

    CARAPI PartiallyUpdateAppWidgetIds(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IParcelable* views);

    CARAPI StopListening(
        /* [in] */ Int32 hostId);

    CARAPI StopListeningAsUser(
        /* [in] */ Int32 hostId,
        /* [in] */ Int32 userId);

    CARAPI UnbindRemoteViewsService(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI UpdateAppWidgetIds(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IParcelable* views);

    CARAPI UpdateAppWidgetProvider(
        /* [in] */ IComponentName* provider,
        /* [in] */ IParcelable* views);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * This returns the user id of the caller, if the caller is not the system process,
     * otherwise it assumes that the calls are from the lockscreen and hence are meant for the
     * current user. TODO: Instead, have lockscreen make explicit calls with userId
     */
    CARAPI_(Int32) GetCallingOrCurrentUserId();

    CARAPI_(AutoPtr<AppWidgetServiceImpl>) GetImplForUser(
        /* [in] */ Int32 userId);

private:
    friend class SysBroadcastReceiver;

    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<ILocale> mLocale;
    AutoPtr<IPackageManager> mPackageManager;
    Boolean mSafeMode;

    AutoPtr<IHandler> mSaveStateHandler;

    HashMap<Int32, AutoPtr<AppWidgetServiceImpl> > mAppWidgetServices;
    Object mAppWidgetServicesLock;
    AutoPtr<UserBroadcastReceiver> mUserBroadcastReceiver;
    AutoPtr<SysBroadcastReceiver> mBroadcastReceiver;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CAPPWIDGETSERVICE_H__
