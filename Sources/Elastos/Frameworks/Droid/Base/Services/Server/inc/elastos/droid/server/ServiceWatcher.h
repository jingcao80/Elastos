#ifndef __ELASTOS_DROID_SERVER_LOCATION_SERVICEWATCHER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_SERVICEWATCHER_H__

#define HASH_FOR_CONTENT
#include <elastos/droid/ext/frameworkhash.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/content/PackageMonitor.h>
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Utility::Etl::List;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Find the best Service, and bind to it.
 * Handles run-time package changes.
 */
class ServiceWatcher
    : public Object
    , public IServiceConnection
{
private:
    class ServiceWatcherPackageMonitor
        : public PackageMonitor
    {
    public:
        ServiceWatcherPackageMonitor(
                /* [in] */ ServiceWatcher* host);

        virtual CARAPI OnPackageUpdateFinished(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        virtual CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        virtual CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        virtual CARAPI OnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid,
            /* [in] */ ArrayOf<String>* components,
            /* [out] */ Boolean* result);

    private:
        ServiceWatcher* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ ServiceWatcher* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ServiceWatcher* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    static CARAPI_(List<HashSet<AutoPtr<ISignature> > >) GetSignatureSets(
        /* [in] */ IContext* context,
        /* [in] */ List<String>* initialPackageNames);

    static CARAPI_(Boolean) IsSignatureMatch(
        /* [in] */ ArrayOf<ISignature*>* signatures,
        /* [in] */ const List<HashSet<AutoPtr<ISignature> > >& sigSets);

    ServiceWatcher(
        /* [in] */ IContext* context,
        /* [in] */ const String& logTag,
        /* [in] */ const String& action,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNamesResId,
        /* [in] */ IRunnable* newServiceWork,
        /* [in] */ IHandler* handler);

    CARAPI_(Boolean) Start();

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* binder);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI_(String) GetBestPackageName();

    CARAPI_(Int32) GetBestVersion();

    CARAPI_(AutoPtr<IBinder>) GetBinder();

    CARAPI_(void) SwitchUser();

private:

    CARAPI_(Boolean) BindBestPackageLocked(
        /* [in] */ const String& justCheckThisPackage);

    CARAPI_(void) UnbindLocked();

    CARAPI_(void) BindToPackageLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 version,
        /* [in] */ Boolean isMultiuser);

    CARAPI_(Boolean) IsSignatureMatch(
        /* [in] */ ArrayOf<ISignature*>* signatures);

public:
    const static String EXTRA_SERVICE_VERSION;
    const static String EXTRA_SERVICE_IS_MULTIUSER;

private:
    const static Boolean DBG;

    String mTag;
    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPm;
    List<HashSet<AutoPtr<ISignature> > > mSignatureSets;
    String mAction;

    /**
     * If mServicePackageName is not null, only this package will be searched for the service that
     * implements mAction. When null, all packages in the system that matches one of the signature
     * in mSignatureSets are searched.
     */
    String mServicePackageName;
    AutoPtr<IRunnable> mNewServiceWork;
    AutoPtr<IHandler> mHandler;

    AutoPtr<ServiceWatcherPackageMonitor> mPackageMonitor;
    Object mLock;

    // all fields below synchronized on mLock
    AutoPtr<IBinder> mBinder;   // connected service
    String mPackageName;  // current best package
    Int32 mVersion;  // current best version
    /**
     * Whether the currently-connected service is multiuser-aware. This can change at run-time
     * when switching from one version of a service to another.
     */
    Boolean mIsMultiuser;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif
