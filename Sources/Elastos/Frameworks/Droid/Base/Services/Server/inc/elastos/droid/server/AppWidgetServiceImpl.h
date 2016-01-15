
#ifndef __ELASTOS_DROID_SERVER_APPWIDGETSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_APPWIDGETSERVICEIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/Pair.h>
#include "elastos/droid/os/Runnable.h"

using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Core::IInteger32;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IIAppWidgetHost;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Widget::Internal::IIRemoteViewsFactory;
using Elastos::Droid::Utility::IAtomicFile;

_ETL_NAMESPACE_BEGIN

#define AUTOPTR_APPWIDGETSERVICEIMPL_HASH_FUNC

// Hash functor for Pair<Int32, AutoPtr<IIntentFilterComparison> >
template<> struct Hash< Pair<Int32, AutoPtr<IIntentFilterComparison> > >
{
   size_t operator()(Pair<Int32, AutoPtr<IIntentFilterComparison> > s) const
   {
        Int32 firstHashCode = s.mFirst;
        Int32 secondHashCode = 0;
        if (s.mSecond != NULL) {
            s.mSecond->GetHashCode(&secondHashCode);
        }

        const Int32 prime = 31;
        Int32 result = 1;
        result = prime * result + firstHashCode;
        result = prime * result + secondHashCode;
        return result;
        return (size_t)s.mFirst;
   }
};

// EqulaTo functor for Pair<Int32, AutoPtr<IIntentFilterComparison> >
template<> struct EqualTo< Pair<String, AutoPtr<IIntentFilterComparison> > >
{
    size_t operator()(
        const Pair<String, AutoPtr<IIntentFilterComparison> >& x,
        const Pair<String, AutoPtr<IIntentFilterComparison> >& y) const
    {
        if (x.mFirst != y.mFirst) {
            return FALSE;
        }

        Boolean equals = (x.mSecond == NULL && x.mSecond == NULL);
        if (!equals && x.mSecond != NULL && y.mSecond != NULL) {
            x.mSecond->Equals(y.mSecond, &equals);
        }

        return equals;
    }
};

#endif //AUTOPTR_APPWIDGETSERVICEIMPL_HASH_FUNC
_ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace Server {

class AppWidgetServiceImpl: public ElRefBase
{
public:
    class AppWidgetId;

    class SaveStateRunnable : public Runnable {
    public:
        SaveStateRunnable(
            /* [in] */ AppWidgetServiceImpl* host);

        virtual ECode Run();
    private:
        AppWidgetServiceImpl* mHost;
    };

    class Provider: public ElRefBase
    {
    public:
        Provider()
            : mUid(0)
            , mZombie(FALSE)
            , mTag(0)
        {}

    public:
        Int32 mUid;
        AutoPtr<IAppWidgetProviderInfo> mInfo;
        List<AutoPtr<AppWidgetId> > mInstances;
        AutoPtr<IPendingIntent> mBroadcast;
        Boolean mZombie; // if we're in safe mode, don't prune this just because nobody references it

        Int32 mTag; // for use while saving state (the index)
    };

    class Host: public ElRefBase
    {
    public:
        Host()
            : mUid(0)
            , mHostId(0)
            , mZombie(FALSE)
            , mTag(0)
        {}

        Boolean UidMatches(
            /* [in] */ Int32 callingUid);

    public:
        Int32 mUid;
        Int32 mHostId;
        String mPackageName;
        List<AutoPtr<AppWidgetId> > mInstances;
        AutoPtr<IIAppWidgetHost> mCallbacks;
        Boolean mZombie; // if we're in safe mode, don't prune this just because nobody references it

        Int32 mTag; // for use while saving state (the index)
    };

    class AppWidgetId: public ElRefBase
    {
    public:
        AppWidgetId() : mAppWidgetId(0) {}

    public:
        Int32 mAppWidgetId;
        AutoPtr<Provider> mProvider;
        AutoPtr<IRemoteViews> mViews;
        AutoPtr<IBundle> mOptions;
        AutoPtr<Host> mHost;
    };

    /**
     * Acts as a proxy between the ServiceConnection and the RemoteViewsAdapterConnection. This
     * needs to be a static inner class since a reference to the ServiceConnection is held globally
     * and may lead us to leak AppWidgetService instances (if there were more than one).
     */
    class ServiceConnectionProxy
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        ServiceConnectionProxy(
            /* [in] */ Pair<Int32, AutoPtr<IIntentFilterComparison> > key,
            /* [in] */ IBinder* connectionCb)
            : mConnectionCb(connectionCb)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        CARAPI Disconnect();

    private:
        friend class AppWidgetServiceImpl;
        const AutoPtr<IBinder> mConnectionCb;
    };

    class DestroyServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        DestroyServiceConnection(
            /* [in] */ IIntent* intent,
            /* [in] */ AppWidgetServiceImpl* host)
            : mIntent(intent)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        AutoPtr<IIntent> mIntent;
        AppWidgetServiceImpl* mHost;
    };

    class NotifyServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        NotifyServiceConnection(
            /* [in] */ AppWidgetServiceImpl* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        AppWidgetServiceImpl* mHost;
    };

public:
    AppWidgetServiceImpl(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [in] */ IHandler* saveStateHandler);

    ~AppWidgetServiceImpl();

    CARAPI_(void) ComputeMaximumWidgetBitmapMemory();

    CARAPI_(void) SystemReady(
        /* [in] */ Boolean safeMode);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

    CARAPI_(void) OnConfigurationChanged();

    CARAPI_(void) OnBroadcastReceived(
        /* [in] */ IIntent* intent);

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AllocateAppWidgetId(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId,
        /* [out] */ Int32* id);

    CARAPI_(void) DeleteAppWidgetId(
        /* [in] */ Int32 appWidgetId);

    CARAPI_(void) DeleteHost(
        /* [in] */ Int32 hostId);

    CARAPI_(void) DeleteAllHosts();

    CARAPI_(void) DeleteHostLocked(
        /* [in] */ Host* host);

    CARAPI_(void) DeleteAppWidgetLocked(
        /* [in] */ AppWidgetId* id);

    CARAPI_(void) CancelBroadcasts(
        /* [in] */ Provider* p);

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

    // Binds to a specific RemoteViewsService
    CARAPI BindRemoteViewsService(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* connection);

    // Unbinds from a specific RemoteViewsService
    CARAPI UnbindRemoteViewsService(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<IAppWidgetProviderInfo>) GetAppWidgetInfo(
        /* [in] */ Int32 appWidgetId);

    CARAPI_(AutoPtr<IRemoteViews>) GetAppWidgetViews(
        /* [in] */ Int32 appWidgetId);

    CARAPI_(AutoPtr<IObjectContainer>) GetInstalledProviders(
        /* [in] */ Int32 categoryFilter);

    CARAPI UpdateAppWidgetIds(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IRemoteViews* views);

    CARAPI UpdateAppWidgetOptions(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IBundle* options);

    CARAPI_(AutoPtr<IBundle>) GetAppWidgetOptions(
        /* [in] */ Int32 appWidgetId);

    CARAPI PartiallyUpdateAppWidgetIds(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IRemoteViews* views);

    CARAPI NotifyAppWidgetViewDataChanged(
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ Int32 viewId);

    CARAPI UpdateAppWidgetProvider(
        /* [in] */ IComponentName* provider,
        /* [in] */ IRemoteViews* views);

    CARAPI_(void) UpdateAppWidgetInstanceLocked(
        /* [in] */ AppWidgetId* id,
        /* [in] */ IRemoteViews* views);

    CARAPI_(void) UpdateAppWidgetInstanceLocked(
        /* [in] */ AppWidgetId* id,
        /* [in] */ IRemoteViews* views,
        /* [in] */ Boolean isPartialUpdate);

    CARAPI_(void) NotifyAppWidgetViewDataChangedInstanceLocked(
        /* [in] */ AppWidgetId* id,
        /* [in] */ Int32 viewId);

    CARAPI StartListening(
        /* [in] */ IIAppWidgetHost* callbacks,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId,
        /* [out] */ IObjectContainer** updatedViews,
        /* [out, callee] */ ArrayOf<Int32>** out);

    CARAPI_(void) StopListening(
        /* [in] */ Int32 hostId);

    CARAPI_(Boolean) CanAccessAppWidgetId(
        /* [in] */ AppWidgetId* id,
        /* [in] */ Int32 callingUid);

    CARAPI_(AutoPtr<AppWidgetId>) LookupAppWidgetIdLocked(
        /* [in] */ Int32 appWidgetId);

    CARAPI_(AutoPtr<Provider>) LookupProviderLocked(
        /* [in] */ IComponentName* provider);

    CARAPI_(AutoPtr<Host>) LookupHostLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 hostId);

    CARAPI_(AutoPtr<Host>) LookupOrAddHostLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 hostId);

    CARAPI_(void) PruneHostLocked(
        /* [in] */ Host* host);

    CARAPI_(void) LoadAppWidgetListLocked();

    CARAPI_(Boolean) AddProviderLocked(
        /* [in] */ IResolveInfo* ri);

    CARAPI_(List<AutoPtr<Provider> >::ReverseIterator) RemoveProviderLocked(
        /* [in] */ List<AutoPtr<Provider> >::ReverseIterator index,
        /* [in] */ Provider* p);

    CARAPI_(void) SendEnableIntentLocked(
        /* [in] */ Provider* p);

    CARAPI_(void) SendUpdateIntentLocked(
        /* [in] */ Provider* p,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    CARAPI_(void) RegisterForBroadcastsLocked(
        /* [in] */ Provider* p,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAppWidgetIds(
        /* [in] */ Provider* p);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAppWidgetIds(
        /* [in] */ IComponentName* provider);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAppWidgetIds(
        /* [in] */ Host* p);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetAppWidgetIdsForHost(
        /* [in] */ Int32 hostId);

    CARAPI GetUidForPackage(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* uid);

    CARAPI EnforceSystemOrCallingUid(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* uid);

    CARAPI EnforceCallingUid(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* uid);

    CARAPI_(void) SendInitialBroadcasts();

    // only call from initialization -- it assumes that the data structures are all empty
    CARAPI_(void) LoadStateLocked();

    CARAPI_(void) SaveStateLocked();

    CARAPI_(void) SaveStateAsync();

    CARAPI_(Boolean) WriteStateToFileLocked(
        /* [in] */ IFileOutputStream* stream);

    CARAPI_(void) ReadStateFromFileLocked(
        /* [in] */ IFileInputStream* stream);

    static CARAPI_(AutoPtr<IFile>) GetSettingsFile(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IAtomicFile>) SavedStateFile();

    CARAPI_(void) OnUserStopping();

    CARAPI_(void) OnUserRemoved();

    CARAPI_(Boolean) AddProvidersForPackageLocked(
        /* [in] */ const String& pkgName);

    /**
     * Updates all providers with the specified package names, and records any providers that were
     * pruned.
     *
     * @return whether any providers were updated
     */
    CARAPI_(Boolean) UpdateProvidersForPackageLocked(
        /* [in] */ const String& pkgName,
        /* [in] */ HashSet<AutoPtr<IComponentName> >* removedProviders);

    CARAPI_(Boolean) RemoveProvidersForPackageLocked(
        /* [in] */ const String& pkgName);

    CARAPI_(void) NotifyHostsForProvidersChangedLocked();

private:
    CARAPI_(void) DumpProvider(
        /* [in] */ Provider* p,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) DumpHost(
        /* [in] */ Host* host,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) DumpAppWidgetId(
        /* [in] */ AppWidgetId* id,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) EnsureStateLoadedLocked();

    CARAPI BindAppWidgetIdImpl(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IComponentName* provider,
        /* [in] */ IBundle* options);

    CARAPI_(Boolean) CallerHasBindAppWidgetPermission(
        /* [in] */ const String& packageName);

    // Unbinds from a RemoteViewsService when we delete an app widget
    CARAPI_(void) UnbindAppWidgetRemoteViewsServicesLocked(
        /* [in] */ AppWidgetId* id);

    // Destroys the cached factory on the RemoteViewsService's side related to the specified intent
    CARAPI_(void) DestroyRemoteViewsService(
        /* [in] */ IIntent* intent,
        /* [in] */ AppWidgetId* id);

    // Adds to the ref-count for a given RemoteViewsService intent
    CARAPI_(void) IncrementAppWidgetServiceRefCount(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntentFilterComparison* fc);

    // Subtracts from the ref-count for a given RemoteViewsService intent, prompting a delete if
    // the ref-count reaches zero.
    CARAPI_(void) DecrementAppWidgetServiceRefCount(
        /* [in] */ AppWidgetId* id);

    CARAPI_(Boolean) IsLocalBinder();

    CARAPI_(AutoPtr<IRemoteViews>) CloneIfLocalBinder(
        /* [in] */ IRemoteViews* rv);

    CARAPI_(AutoPtr<IAppWidgetProviderInfo>) CloneIfLocalBinder(
        /* [in] */ IAppWidgetProviderInfo* info);

    CARAPI_(AutoPtr<IBundle>) CloneIfLocalBinder(
        /* [in] */ IBundle* bundle);

    CARAPI_(AutoPtr<Provider>) ParseProviderInfoXml(
        /* [in] */ IComponentName* component,
        /* [in] */ IResolveInfo* ri);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<ILocale> mLocale;
    AutoPtr<IIPackageManager> mPm;
    AutoPtr<IAlarmManager> mAlarmManager;
    List<AutoPtr<Provider> > mInstalledProviders;
    Int32 mNextAppWidgetId;
    List<AutoPtr<AppWidgetId> > mAppWidgetIds;
    Object mAppWidgetIdsLock;
    List<AutoPtr<Host> > mHosts;
    // set of package names
    HashSet<String> mPackagesWithBindWidgetPermission;
    Boolean mSafeMode;
    Int32 mUserId;
    Boolean mStateLoaded;
    Int32 mMaxWidgetBitmapMemory;

    AutoPtr<IHandler> mSaveStateHandler;

    // These are for debugging only -- widgets are going missing in some rare instances
    List<AutoPtr<Provider> > mDeletedProviders;
    List<AutoPtr<Host> > mDeletedHosts;

private:
    static const String TAG;
    static const String SETTINGS_FILENAME;
    static const Int32 MIN_UPDATE_PERIOD;

    static Boolean DBG;

    // Manages active connections to RemoteViewsServices
    HashMap<Pair<Int32, AutoPtr<IIntentFilterComparison> >, AutoPtr<IServiceConnection> > mBoundRemoteViewsServices;
    // Manages persistent references to RemoteViewsServices from different App Widgets
    HashMap<AutoPtr<IIntentFilterComparison>, AutoPtr<HashSet<Int32> > > mRemoteViewsServicesAppWidgets;

    AutoPtr<IRunnable> mSaveStateRunnable;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_SERVER_APPWIDGETSERVICEIMPL_H__
