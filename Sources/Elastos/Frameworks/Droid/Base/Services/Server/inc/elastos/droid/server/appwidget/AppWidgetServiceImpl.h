
#ifndef __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICEIMPL_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.AppWidget.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Core.Server.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetHost;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetService;
using Elastos::Droid::Internal::Widget::IIRemoteViewsAdapterConnection;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Server::IWidgetBackupProvider;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Core::IRunnable;
using Elastos::IO::IFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

class AppWidgetServiceImpl
    : public Object
    , public IIAppWidgetService//::Stub
    , public IWidgetBackupProvider
    , public IBinder
    // no file , public DevicePolicyManagerInternal.OnCrossProfileWidgetProvidersChangeListener
{
public:
    class InnerBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InnerBroadcastReceiver();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IInterface* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AppWidgetServiceImpl* mOwner;
    };

    class CallbackHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AppWidgetServiceImpl::CallbackHandler")

        CallbackHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AppWidgetServiceImpl* owner);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    public:
        static const Int32 MSG_NOTIFY_UPDATE_APP_WIDGET = 1;
        static const Int32 MSG_NOTIFY_PROVIDER_CHANGED = 2;
        static const Int32 MSG_NOTIFY_PROVIDERS_CHANGED = 3;
        static const Int32 MSG_NOTIFY_VIEW_DATA_CHANGED = 4;

    private:
        AppWidgetServiceImpl* mOwner;
    };

    class Widget;
    class Host;
    class Provider;

    class SecurityPolicy
        : public Object
    {
    public:
        SecurityPolicy(
            /* [in] */ AppWidgetServiceImpl* owner);

        virtual CARAPI_(Boolean) IsEnabledGroupProfile(
            /* [in] */ Int32 profileId);

        virtual CARAPI GetEnabledGroupProfileIds(
            /* [in] */ Int32 userId,
            /* [out] */ ArrayOf<Int32>** result);

        virtual CARAPI EnforceServiceExistsAndRequiresBindRemoteViewsPermission(
            /* [in] */ IComponentName* componentName,
            /* [in] */ Int32 userId);

        virtual CARAPI EnforceModifyAppWidgetBindPermissions(
            /* [in] */ const String& packageName);

        virtual CARAPI EnforceCallFromPackage(
            /* [in] */ const String& packageName);

        virtual CARAPI HasCallerBindPermissionOrBindWhiteListedLocked(
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        virtual CARAPI CanAccessAppWidget(
            /* [in] */ Widget* widget,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        virtual CARAPI IsProviderInCallerOrInProfileAndWhitelListed(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 profileId,
            /* [out] */ Boolean* result);

        virtual CARAPI IsProviderWhitelListed(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 profileId,
            /* [out] */ Boolean* result);

        virtual CARAPI GetProfileParent(
            /* [in] */ Int32 profileId,
            /* [out] */ Int32* result);

        virtual CARAPI GetGroupParent(
            /* [in] */ Int32 profileId,
            /* [out] */ Int32* result);

        virtual CARAPI IsHostInPackageForUid(
            /* [in] */ Host* host,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        virtual CARAPI IsProviderInPackageForUid(
            /* [in] */ Provider* provider,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        virtual CARAPI IsHostAccessingProvider(
            /* [in] */ Host* host,
            /* [in] */ Provider* provider,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

    private:
        CARAPI IsCallerBindAppWidgetWhiteListedLocked(
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        CARAPI_(Boolean) IsParentOrProfile(
            /* [in] */ Int32 parentId,
            /* [in] */ Int32 profileId);

        CARAPI_(Boolean) IsProfileEnabled(
            /* [in] */ Int32 profileId);

    private:
        AppWidgetServiceImpl* mOwner;
    };

    class ProviderId
        : public Object
    {
    public:
        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        ProviderId(
            /* [in] */ Int32 uid,
            /* [in] */ IComponentName* componentName);

    public:
        /*const*/ Int32 mUid;
        /*const*/ AutoPtr<IComponentName> mComponentName;
    };

    class Provider
        : public Object
    {
    public:
        Provider();

        // for use while saving state (the index)
        virtual CARAPI GetUserId(
            /* [out] */ Int32* result);

        virtual CARAPI IsInPackageForUser(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // is there an instance of this provider hosted by the given app?
        virtual CARAPI HostedByPackageForUser(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        AutoPtr<ProviderId> mId;
        AutoPtr<IAppWidgetProviderInfo> mInfo;
        AutoPtr<IList> mWidgets;
        AutoPtr<IPendingIntent> mBroadcast;
        Boolean mZombie;
        // if we're in safe mode, don't prune this just because nobody references it
        Int32 mTag;
    };

    class HostId;

    class Host
        : public Object
    {
    public:
        Host();

        // for use while saving state (the index)
        virtual CARAPI GetUserId(
            /* [out] */ Int32* result);

        virtual CARAPI IsInPackageForUser(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        CARAPI HostsPackageForUser(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

    public:
        AutoPtr<HostId> mId;
        AutoPtr<IList> mWidgets;
        AutoPtr<IIAppWidgetHost> mCallbacks;
        Boolean mZombie;
        // if we're in safe mode, don't prune this just because nobody references it
        Int32 mTag;
    };

    class HostId
        : public Object
    {
    public:
        HostId(
            /* [in] */ Int32 uid,
            /* [in] */ Int32 hostId,
            /* [in] */ const String& packageName);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        /*const*/ Int32 mUid;
        /*const*/ Int32 mHostId;
        /*const*/ String mPackageName;
    };

    class Widget
        : public Object
    {
    public:
        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mAppWidgetId;
        Int32 mRestoredId;
        // tracking & remapping any restored state
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
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceConnectionProxy(
            /* [in] */ IBinder* connectionCb);

        virtual CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        virtual CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        virtual CARAPI Disconnect();

    private:
        /*const*/ AutoPtr<IIRemoteViewsAdapterConnection> mConnectionCb;
    };

    class SaveStateRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SaveStateRunnable(
            /* [in] */ Int32 userId,
            /* [in] */ AppWidgetServiceImpl* owner);

        // @Override
        CARAPI Run();

    public:
        /*const*/ Int32 mUserId;
        AppWidgetServiceImpl* mOwner;
    };

    /**
      * This class encapsulates the backup and restore logic for a user group state.
      */
    class BackupRestoreController
        : public Object
    {
    private:
        // Accumulate a list of updates that affect the given provider for a final
        // coalesced notification broadcast once restore is over.
        class RestoreUpdateRecord
            : public Object
        {
        public:
            RestoreUpdateRecord(
                /* [in] */ Int32 theOldId,
                /* [in] */ Int32 theNewId);

        public:
            Int32 mOldId;
            Int32 mNewId;
            Boolean mNotified;
        };

    public:
        BackupRestoreController(
            /* [in] */ AppWidgetServiceImpl* owner);

        virtual CARAPI GetWidgetParticipants(
            /* [in] */ Int32 userId,
            /* [out] */ IList** result);

        virtual CARAPI GetWidgetState(
            /* [in] */ const String& backedupPackage,
            /* [in] */ Int32 userId,
            /* [out] */ ArrayOf<Byte>** result);

        virtual CARAPI RestoreStarting(
            /* [in] */ Int32 userId);

        virtual CARAPI RestoreWidgetState(
            /* [in] */ const String& packageName,
            /* [in] */ ArrayOf<Byte>* restoredState,
            /* [in] */ Int32 userId);

        // Called once following the conclusion of a restore operation.  This is when we
        // send out updates to apps involved in widget-state restore telling them about
        // the new widget ID space.
        virtual CARAPI RestoreFinished(
            /* [in] */ Int32 userId);

    private:
        CARAPI_(AutoPtr<Provider>) FindProviderLocked(
            /* [in] */ IComponentName* componentName,
            /* [in] */ Int32 userId);

        CARAPI_(AutoPtr<Widget>) FindRestoredWidgetLocked(
            /* [in] */ Int32 restoredId,
            /* [in] */ Host* host,
            /* [in] */ Provider* p);

        CARAPI_(Boolean) PackageNeedsWidgetBackupLocked(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 userId);

        CARAPI_(void) StashProviderRestoreUpdateLocked(
            /* [in] */ Provider* provider,
            /* [in] */ Int32 oldId,
            /* [in] */ Int32 newId);

        CARAPI_(Boolean) AlreadyStashed(
            /* [in] */ IList* stash,
            /* [in] */ Int32 oldId,
            /* [in] */ Int32 newId);

        CARAPI_(void) StashHostRestoreUpdateLocked(
            /* [in] */ Host* host,
            /* [in] */ Int32 oldId,
            /* [in] */ Int32 newId);

        CARAPI_(void) SendWidgetRestoreBroadcastLocked(
            /* [in] */ const String& action,
            /* [in] */ Provider* provider,
            /* [in] */ Host* host,
            /* [in] */ ArrayOf<Int32>* oldIds,
            /* [in] */ ArrayOf<Int32>* newIds,
            /* [in] */ IUserHandle* userHandle);

        // We're restoring widget state for 'pkg', so we start by wiping (a) all widget
        // instances that are hosted by that app, and (b) all instances in other hosts
        // for which 'pkg' is the provider.  We assume that we'll be restoring all of
        // these hosts & providers, so will be reconstructing a correct live state.
        CARAPI_(void) PruneWidgetStateLocked(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId);

        CARAPI_(Boolean) IsProviderAndHostInUser(
            /* [in] */ Widget* widget,
            /* [in] */ Int32 userId);

        CARAPI_(AutoPtr<IBundle>) ParseWidgetIdOptions(
            /* [in] */ IXmlPullParser* parser);

        CARAPI_(Int32) CountPendingUpdates(
            /* [in] */ IList* updates);

    private:
        AppWidgetServiceImpl* mOwner;
        static const String TAG;
        static const Boolean DEBUG;
        // Version of backed-up widget state.
        static const Int32 WIDGET_STATE_VERSION = 2;
        // We need to make sure to wipe the pre-restore widget state only once for
        // a given package.  Keep track of what we've done so far here; the list is
        // cleared at the start of every system restore pass, but preserved through
        // any install-time restore operations.
        /*const*/ AutoPtr<IHashSet> mPrunedApps;
        /*const*/ AutoPtr<IHashMap> mUpdatesByProvider;
        /*const*/ AutoPtr<IHashMap> mUpdatesByHost;
    };

private:
    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InnerServiceConnection(
            /* [in] */ AppWidgetServiceImpl* owner,
            /* [in] */ IIntent* intent);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        AppWidgetServiceImpl* mOwner;
        IIntent* mIntent;
    };

    class InnerServiceConnection1
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InnerServiceConnection1(
            /* [in] */ AppWidgetServiceImpl* owner);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        AppWidgetServiceImpl* mOwner;
    };

    class LoadedWidgetState
        : public Object
    {
    public:
        LoadedWidgetState(
            /* [in] */ Widget* widget,
            /* [in] */ Int32 hostTag,
            /* [in] */ Int32 providerTag);

    public:
        /*const*/ AutoPtr<Widget> mWidget;
        /*const*/ Int32 mHostTag;
        /*const*/ Int32 mProviderTag;
    };

public:
    CAR_INTERFACE_DECL()

    AppWidgetServiceImpl();

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI SetSafeMode(
        /* [in] */ Boolean safeMode);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI StartListening(
        /* [in] */ IIAppWidgetHost* callbacks,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 hostId,
        /* [out] */ IList** updatedViews,
        /* [out, callee] */ ArrayOf<Int32>** result);

    // @Override
    CARAPI StopListening(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 hostId);

    // @Override
    CARAPI AllocateAppWidgetId(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 hostId,
        /* [out] */ Int32* result);

    // @Override
    CARAPI DeleteAppWidgetId(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId);

    // @Override
    CARAPI HasBindAppWidgetPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 grantId,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetBindAppWidgetPermission(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 grantId,
        /* [in] */ Boolean grantPermission);

    // @Override
    CARAPI CreateAppWidgetConfigIntentSender(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IIntentSender** result);

    // @Override
    CARAPI BindAppWidgetId(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 providerProfileId,
        /* [in] */ IComponentName* providerComponent,
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetAppWidgetIds(
        /* [in] */ IComponentName* componentName,
        /* [out] */ ArrayOf<Int32>** result);

    // @Override
    CARAPI GetAppWidgetIdsForHost(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 hostId,
        /* [out] */ ArrayOf<Int32>** result);

    // @Override
    CARAPI BindRemoteViewsService(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* callbacks);

    // @Override
    CARAPI UnbindRemoteViewsService(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IIntent* intent);

    // @Override
    CARAPI DeleteHost(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 hostId);

    // @Override
    CARAPI DeleteAllHosts();

    // @Override
    CARAPI GetAppWidgetInfo(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IAppWidgetProviderInfo** result);

    // @Override
    CARAPI GetAppWidgetViews(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IRemoteViews** result);

    // @Override
    CARAPI UpdateAppWidgetOptions(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IBundle* options);

    // @Override
    CARAPI GetAppWidgetOptions(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 appWidgetId,
        /* [out] */ IBundle** result);

    // @Override
    CARAPI UpdateAppWidgetIds(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IRemoteViews* views);

    // @Override
    CARAPI PartiallyUpdateAppWidgetIds(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IRemoteViews* views);

    // @Override
    CARAPI NotifyAppWidgetViewDataChanged(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ Int32 viewId);

    // @Override
    CARAPI UpdateAppWidgetProvider(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IRemoteViews* views);

    // @Override
    CARAPI GetInstalledProvidersForProfile(
        /* [in] */ Int32 categoryFilter,
        /* [in] */ Int32 profileId,
        /* [out] */ IList** result);

    virtual CARAPI SendOptionsChangedIntentLocked(
        /* [in] */ Widget* widget);

    // @Override
    CARAPI GetWidgetParticipants(
        /* [in] */ Int32 userId,
        /* [out] */ IList** result);

    // @Override
    CARAPI GetWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI RestoreStarting(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI RestoreWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ ArrayOf<Byte>* restoredState,
        /* [in] */ Int32 userId);

    // @Override
    CARAPI RestoreFinished(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI OnCrossProfileWidgetProvidersChanged(
        /* [in] */ Int32 userId,
        /* [in] */ IList* packages);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(void) ComputeMaximumWidgetBitmapMemory();

    CARAPI_(void) RegisterBroadcastReceiver();

    CARAPI_(void) RegisterOnCrossProfileProvidersChangedListener();

    CARAPI OnConfigurationChanged();

    CARAPI_(void) OnPackageBroadcastReceived(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI_(void) ResolveHostUidLocked(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 uid);

    CARAPI_(void) EnsureGroupStateLoadedLocked(
        /* [in] */ Int32 userId);

    CARAPI UpdateAppWidgetIds(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<Int32>* appWidgetIds,
        /* [in] */ IRemoteViews* views,
        /* [in] */ Boolean partially);

    CARAPI_(Int32) IncrementAndGetAppWidgetIdLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetMinAppWidgetIdLocked(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 minWidgetId);

    CARAPI_(Int32) PeekNextAppWidgetIdLocked(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<Host>) LookupOrAddHostLocked(
        /* [in] */ HostId* id);

    CARAPI_(void) DeleteHostLocked(
        /* [in] */ Host* host);

    CARAPI_(void) DeleteAppWidgetLocked(
        /* [in] */ Widget* widget);

    CARAPI_(void) CancelBroadcasts(
        /* [in] */ Provider* provider);

    // Unbinds from a RemoteViewsService when we delete an app widget
    CARAPI_(void) UnbindAppWidgetRemoteViewsServicesLocked(
        /* [in] */ Widget* widget);

    // Destroys the cached factory on the RemoteViewsService's side related to the specified intent
    CARAPI_(void) DestroyRemoteViewsService(
        /* [in] */ IIntent* intent,
        /* [in] */ Widget* widget);

    // Adds to the ref-count for a given RemoteViewsService intent
    CARAPI_(void) IncrementAppWidgetServiceRefCount(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IPair* serviceId);

    // Subtracts from the ref-count for a given RemoteViewsService intent, prompting a delete if
    // the ref-count reaches zero.
    CARAPI_(void) DecrementAppWidgetServiceRefCount(
        /* [in] */ Widget* widget);

    CARAPI_(void) SaveGroupStateAsync(
        /* [in] */ Int32 groupId);

    CARAPI_(void) UpdateAppWidgetInstanceLocked(
        /* [in] */ Widget* widget,
        /* [in] */ IRemoteViews* views,
        /* [in] */ Boolean isPartialUpdate);

    CARAPI_(void) ScheduleNotifyAppWidgetViewDataChanged(
        /* [in] */ Widget* widget,
        /* [in] */ Int32 viewId);

    CARAPI_(void) HandleNotifyAppWidgetViewDataChanged(
        /* [in] */ Host* host,
        /* [in] */ IIAppWidgetHost* callbacks,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 viewId);

    CARAPI_(void) ScheduleNotifyUpdateAppWidgetLocked(
        /* [in] */ Widget* widget,
        /* [in] */ IRemoteViews* updateViews);

    CARAPI_(void) HandleNotifyUpdateAppWidget(
        /* [in] */ Host* host,
        /* [in] */ IIAppWidgetHost* callbacks,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IRemoteViews* views);

    CARAPI_(void) ScheduleNotifyProviderChangedLocked(
        /* [in] */ Widget* widget);

    CARAPI_(void) HandleNotifyProviderChanged(
        /* [in] */ Host* host,
        /* [in] */ IIAppWidgetHost* callbacks,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* info);

    CARAPI_(void) ScheduleNotifyGroupHostsForProvidersChangedLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) HandleNotifyProvidersChanged(
        /* [in] */ Host* host,
        /* [in] */ IIAppWidgetHost* callbacks);

    static CARAPI_(Boolean) IsLocalBinder();

    static CARAPI_(AutoPtr<IRemoteViews>) CloneIfLocalBinder(
        /* [in] */ IRemoteViews* rv);

    static CARAPI_(AutoPtr<IAppWidgetProviderInfo>) CloneIfLocalBinder(
        /* [in] */ IAppWidgetProviderInfo* info);

    static CARAPI_(AutoPtr<IBundle>) CloneIfLocalBinder(
        /* [in] */ IBundle* bundle);

    CARAPI_(AutoPtr<Widget>) LookupWidgetLocked(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr<Provider>) LookupProviderLocked(
        /* [in] */ ProviderId* id);

    CARAPI_(AutoPtr<Host>) LookupHostLocked(
        /* [in] */ HostId* hostId);

    CARAPI_(void) PruneHostLocked(
        /* [in] */ Host* host);

    CARAPI_(void) LoadGroupWidgetProvidersLocked(
        /* [in] */ ArrayOf<Int32>* profileIds);

    CARAPI_(Boolean) AddProviderLocked(
        /* [in] */ IResolveInfo* ri);

    CARAPI_(void) DeleteProviderLocked(
        /* [in] */ Provider* provider);

    CARAPI_(void) SendEnableIntentLocked(
        /* [in] */ Provider* p);

    CARAPI_(void) SendUpdateIntentLocked(
        /* [in] */ Provider* provider,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    CARAPI_(void) SendDeletedIntentLocked(
        /* [in] */ Widget* widget);

    CARAPI_(void) SendDisabledIntentLocked(
        /* [in] */ Provider* provider);

    CARAPI_(void) RegisterForBroadcastsLocked(
        /* [in] */ Provider* provider,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) GetWidgetIds(
        /* [in] */ IList* widgets);

    static CARAPI_(void) DumpProvider(
        /* [in] */ Provider* provider,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(void) DumpHost(
        /* [in] */ Host* host,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(void) DumpGrant(
        /* [in] */ IPair* grant,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(void) DumpWidget(
        /* [in] */ Widget* widget,
        /* [in] */ Int32 index,
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(void) SerializeProvider(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ Provider* p);

    static CARAPI_(void) SerializeHost(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ Host* host);

    static CARAPI_(void) SerializeAppWidget(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ Widget* widget);

    // @SuppressWarnings("deprecation")
    CARAPI_(AutoPtr<Provider>) ParseProviderInfoXml(
        /* [in] */ ProviderId* providerId,
        /* [in] */ IResolveInfo* ri);

    CARAPI_(Int32) GetUidForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IActivityInfo>) GetProviderInfo(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IList>) QueryIntentReceivers(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI_(void) OnUserStarted(
        /* [in] */ Int32 userId);

    // only call from initialization -- it assumes that the data structures are all empty
    CARAPI_(void) LoadGroupStateLocked(
        /* [in] */ ArrayOf<Int32>* profileIds);

    CARAPI_(void) BindLoadedWidgets(
        /* [in] */ IList* loadedWidgets);

    CARAPI_(AutoPtr<Provider>) FindProviderByTag(
        /* [in] */ Int32 tag);

    CARAPI_(AutoPtr<Host>) FindHostByTag(
        /* [in] */ Int32 tag);

    CARAPI_(void) SaveStateLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) TagProvidersAndHosts();

    CARAPI_(void) ClearProvidersAndHostsTagsLocked();

    CARAPI_(Boolean) WriteProfileStateToFileLocked(
        /* [in] */ IFileOutputStream* stream,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) ReadProfileStateFromFileLocked(
        /* [in] */ IFileInputStream* stream,
        /* [in] */ Int32 userId,
        /* [in] */ IList* outLoadedWidgets);

    CARAPI PerformUpgradeLocked(
        /* [in] */ Int32 fromVersion);

    static CARAPI_(AutoPtr<IFile>) GetStateFile(
        /* [in] */ Int32 userId);

    static CARAPI_(AutoPtr<IAtomicFile>) GetSavedStateFile(
        /* [in] */ Int32 userId);

    CARAPI_(void) OnUserStopped(
        /* [in] */ Int32 userId);

    /**
      * Updates all providers with the specified package names, and records any providers that were
      * pruned.
      *
      * @return whether any providers were updated
      */
    CARAPI_(Boolean) UpdateProvidersForPackageLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [in] */ ISet* removedProviders);

    CARAPI_(Boolean) RemoveHostsAndProvidersForPackageLocked(
        /* [in] */ const String& pkgName,
        /* [in] */ Int32 userId);

    CARAPI_(String) GetCanonicalPackageName(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ Int32 userId);

    CARAPI_(void) SendBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* userHandle);

    CARAPI_(void) BindService(
        /* [in] */ IIntent* intent,
        /* [in] */ IServiceConnection* connection,
        /* [in] */ IUserHandle* userHandle);

    CARAPI_(void) UnbindService(
        /* [in] */ IServiceConnection* connection);

    static CARAPI_(Int32) InitMinUpdatePeriod();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String OLD_KEYGUARD_HOST_PACKAGE;
    static const String NEW_KEYGUARD_HOST_PACKAGE;
    static const Int32 KEYGUARD_HOST_ID = 0x4b455947;
    static const String STATE_FILENAME;
    static const Int32 MIN_UPDATE_PERIOD;
    // 30 minutes
    static const Int32 TAG_UNDEFINED = -1;
    static const Int32 UNKNOWN_UID = -1;
    static const Int32 LOADED_PROFILE_ID = -1;
    static const Int32 UNKNOWN_USER_ID = -10;
    // Bump if the stored widgets need to be upgraded.
    static const Int32 CURRENT_VERSION = 1;
    /*const*/ AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    // Manages active connections to RemoteViewsServices.
    /*const*/ AutoPtr<IHashMap> mBoundRemoteViewsServices;
    // Manages persistent references to RemoteViewsServices from different App Widgets.
    /*const*/ AutoPtr<IHashMap> mRemoteViewsServicesAppWidgets;
    /*const*/ Object mLock;
    /*const*/ AutoPtr<IArrayList> mWidgets;
    /*const*/ AutoPtr<IArrayList> mHosts;
    /*const*/ AutoPtr<IArrayList> mProviders;
    /*const*/ AutoPtr<IArraySet> mPackagesWithBindWidgetPermission;
    /*const*/ AutoPtr<ISparseInt32Array> mLoadedUserIds;
    /*const*/ AutoPtr<BackupRestoreController> mBackupRestoreController;
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr<IIPackageManager> mPackageManager;
    /*const*/ AutoPtr<IAlarmManager> mAlarmManager;
    /*const*/ AutoPtr<IUserManager> mUserManager;
    /*const*/ AutoPtr<IAppOpsManager> mAppOpsManager;
    /*const*/ AutoPtr<SecurityPolicy> mSecurityPolicy;
    /*const*/ AutoPtr<IHandler> mSaveStateHandler;
    /*const*/ AutoPtr<IHandler> mCallbackHandler;
    AutoPtr<ILocale> mLocale;
    /*const*/ AutoPtr<ISparseInt32Array> mNextAppWidgetIds;
    Boolean mSafeMode;
    Int32 mMaxWidgetBitmapMemory;
};

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICEIMPL_H__

