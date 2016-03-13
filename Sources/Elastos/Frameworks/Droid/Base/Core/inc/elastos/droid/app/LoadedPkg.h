
#ifndef __ELASTOS_DROID_APP_LOADEDPKG_H__
#define __ELASTOS_DROID_APP_LOADEDPKG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::View::IDisplayAdjustments;

using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

class LoadedPkg
    : public Object
    , public ILoadedPkg
{
public:
    class ReceiverDispatcher
        : public Object
        , public IReceiverDispatcher
    {
    public:
        class Args
            : public BroadcastReceiver::PendingResult
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            Args();

            virtual ~Args();

            CARAPI constructor(
                /* [in] */ IIntent* intent,
                /* [in] */ Int32 resultCode,
                /* [in] */ const String& resultData,
                /* [in] */ IBundle* resultExtras,
                /* [in] */ Boolean ordered,
                /* [in] */ Boolean sticky,
                /* [in] */ Int32 sendingUser,
                /* [in] */ ReceiverDispatcher* rd);

            CARAPI Run();

        public:
            AutoPtr<IIntent> mCurIntent;
            Boolean mOrdered;
            AutoPtr<ReceiverDispatcher> mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        ReceiverDispatcher();

        virtual ~ReceiverDispatcher();

        CARAPI constructor(
            /* [in] */ IBroadcastReceiver* receiver,
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ IInstrumentation* instrumentation,
            /* [in] */ Boolean registered);

        CARAPI Validate(
           /* [in] */ IContext* context,
           /* [in] */ IHandler* handler);

        //CARAPI GetLocation(
        //    /* [out] */ IIntentReceiverLeaked* location);

        CARAPI_(AutoPtr<IBroadcastReceiver>) GetIntentReceiver();

        CARAPI_(AutoPtr<IIntentReceiver>) GetIIntentReceiver();

        CARAPI_(void) PerformReceive(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ Int32 sendingUser);

    public:
        AutoPtr<IIntentReceiver> mIIntentReceiver;
        AutoPtr<IBroadcastReceiver> mReceiver;
        AutoPtr<IContext> mContext;
        AutoPtr<IHandler> mActivityThread;
        AutoPtr<IInstrumentation> mInstrumentation;
        Boolean mRegistered;
        //AutoPtr<IIntentReceiverLeaked> mLocation;
        Boolean mForgotten;
        Boolean mHasUsed;
    };

    class ServiceDispatcher
        : public Object
        , public IServiceDispatcher
    {
    private:
        class ConnectionInfo
            : public Object
        {
        public:
            AutoPtr<IBinder> mBinder;
            AutoPtr<IProxyDeathRecipient> mDeathMonitor;
        };

        class RunConnection
            : public Runnable
        {
        public:
            RunConnection(
               /* [in] */ ServiceDispatcher* serviceDispatcher,
               /* [in] */ IComponentName* name,
               /* [in] */ IBinder* service,
               /* [in] */ Int32 command);

            virtual CARAPI Run();

        private:
            AutoPtr<ServiceDispatcher> mServiceDispatcher;
            AutoPtr<IComponentName> mName;
            AutoPtr<IBinder> mService;
            Int32 mCommand;
        };

        class DeathMonitor
            : public Object
            , public IProxyDeathRecipient
        {
        public:
            CAR_INTERFACE_DECL()

            DeathMonitor(
                /* [in] */ IComponentName* name,
                /* [in] */ IBinder* service,
                /* [in] */ ServiceDispatcher* owner);

            //@Override
            CARAPI ProxyDied();

        private:
            AutoPtr<IComponentName> mName;
            AutoPtr<IBinder> mService;
            AutoPtr<IWeakReference> mOwner;
        };

    public:
        CAR_INTERFACE_DECL()

        ServiceDispatcher(
           /* [in] */ IServiceConnection* conn,
           /* [in] */ IContext* context,
           /* [in] */ IHandler* activityThread,
           /* [in] */ Int32 flags);

        CARAPI Validate(
           /* [in] */ IContext* context,
           /* [in] */ IHandler* handler);

        CARAPI_(void) DoForget();

        CARAPI_(AutoPtr<IServiceConnection>) GetServiceConnection();

        CARAPI_(AutoPtr<IIServiceConnection>) GetIServiceConnection();

        CARAPI_(Int32) GetFlags();

        CARAPI Connected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI DoConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI Death(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI DoDeath(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

    private:
        AutoPtr<IIServiceConnection> mIServiceConnection;
        AutoPtr<IServiceConnection> mConnection;
        AutoPtr<IContext> mContext;
        AutoPtr<IHandler> mActivityThread;
        // private final ServiceConnectionLeaked mLocation;
        Int32 mFlags;
        Boolean mDied;
        Boolean mForgotten;
        HashMap<AutoPtr<IComponentName>, AutoPtr<ConnectionInfo> > mActiveConnections;
        Object mLock;
    };

public:
    CAR_INTERFACE_DECL()

    LoadedPkg();

    virtual ~LoadedPkg();

    CARAPI constructor(
        /* [in] */ IActivityThread* activityThread,
        /* [in] */ IApplicationInfo* aInfo,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IClassLoader* baseLoader,
        /* [in] */ Boolean securityViolation,
        /* [in] */ Boolean includeCode,
        /* [in] */ Boolean registerPackage);

    CARAPI constructor(
        /* [in] */ IActivityThread* activityThread);

    CARAPI InstallSystemApplicationInfo(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ IClassLoader* classLoader);

    AutoPtr<IApplicationInfo> AdjustNativeLibraryPaths(
        /* [in] */ IApplicationInfo* info);

    CARAPI_(AutoPtr<IApplication>) GetApplication();

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    CARAPI_(Boolean) IsSecurityViolation();

    CARAPI IsSecurityViolation(
        /* [out] */ Boolean* bval);

    CARAPI GetCompatibilityInfo(
    /* [out] */ ICompatibilityInfo** info);

    CARAPI SetCompatibilityInfo(
        /* [in] */ ICompatibilityInfo* compatInfo);

    CARAPI GetClassLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI GetAppDir(
        /* [out] */ String* appDir);

    CARAPI GetLibDir(
        /* [out] */ String* appDir);

    CARAPI GetResDir(
        /* [out] */ String* resDir);

    CARAPI GetSplitAppDirs(
        /* [out, callee] */ ArrayOf<String>** dirs);

    CARAPI GetSplitResDirs(
        /* [out, callee] */ ArrayOf<String>** dirs);

    CARAPI GetOverlayDirs(
        /* [out, callee] */ ArrayOf<String>** dirs);

    CARAPI GetDataDir(
        /* [out] */ String* dataDir);

    CARAPI GetDataDirFile(
        /* [out] */ IFile** dirFile);

    CARAPI GetAssets(
        /* [in] */ IActivityThread* mainThread,
        /* [out] */ IAssetManager** manager);

    CARAPI GetResources(
        /* [in] */ IActivityThread* activityThread,
        /* [out] */ IResources** res);

    CARAPI MakeApplication(
        /* [in] */ Boolean forceDefaultAppClass,
        /* [in] */ IInstrumentation* instrumentation,
        /* [out] */ IApplication** app);

    CARAPI RemoveContextRegistrations(
        /* [in] */ IContext* context,
        /* [in] */ const String& who,
        /* [in] */ const String& what);

    CARAPI GetReceiverDispatcher(
        /* [in] */ IBroadcastReceiver* c,
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IInstrumentation* instrumentation,
        /* [in] */ Boolean registered,
        /* [out] */ IIntentReceiver** ir);

    CARAPI ForgetReceiverDispatcher(
        /* [in] */ IContext* context,
        /* [in] */ IBroadcastReceiver* r,
        /* [out] */ IIntentReceiver** ir);

    CARAPI  GetServiceDispatcher(
        /* [in] */ IServiceConnection* c,
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 flags,
        /* [out] */ IIServiceConnection** result);

    CARAPI ForgetServiceDispatcher(
        /* [in] */ IContext* context,
        /* [in] */ IServiceConnection* c,
        /* [out] */ IIServiceConnection** result);

    static CARAPI GetSystemClassLoader(
        /* [out] */ IClassLoader** loader);

private:
    /**
     * Setup value for Thread.getContextClassLoader(). If the
     * package will not run in in a VM with other packages, we set
     * the Java context ClassLoader to the
     * PackageInfo.getClassLoader value. However, if this VM can
     * contain multiple packages, we intead set the Java context
     * ClassLoader to a proxy that will warn about the use of Java
     * context ClassLoaders and then fall through to use the
     * system ClassLoader.
     *
     * <p> Note that this is similar to but not the same as the
     * android.content.Context.getClassLoader(). While both
     * context class loaders are typically set to the
     * PathClassLoader used to load the package archive in the
     * single application per VM case, a single Android process
     * may contain several Contexts executing on one thread with
     * their own logical ClassLoaders while the Java context
     * ClassLoader is a thread local. This is why in the case when
     * we have multiple packages per VM we do not set the Java
     * context ClassLoader to an arbitrary but instead warn the
     * user to set their own if we detect that they are using a
     * Java library that expects it to be set.
     */
    CARAPI_(void) InitializeJavaContextClassLoader();

    CARAPI RewriteRValues(
        /* [in] */ IClassLoader* cl,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 id);

public:
    String mPackageName;
    AutoPtr<IResources> mResources;

private:
    static const String TAG;

    // CActivityThread has this's reference
    IActivityThread* mActivityThread;
    AutoPtr<IApplicationInfo> mApplicationInfo;
    String mAppDir;
    String mResDir;
    AutoPtr<ArrayOf<String> > mSplitAppDirs;
    AutoPtr<ArrayOf<String> > mSplitResDirs;
    AutoPtr<ArrayOf<String> > mOverlayDirs;
    AutoPtr<ArrayOf<String> > mSharedLibraries;
    String mDataDir;
    String mLibDir;
    AutoPtr<IFile> mDataDirFile;
    AutoPtr<IClassLoader> mBaseClassLoader;
    Boolean mSecurityViolation;
    Boolean mIncludeCode;
    Boolean mRegisterPackage;
    AutoPtr<IDisplayAdjustments> mDisplayAdjustments;// = new DisplayAdjustments();
    AutoPtr<IClassLoader> mClassLoader;
    AutoPtr<IApplication> mApplication;

    typedef HashMap<AutoPtr<IBroadcastReceiver>, AutoPtr<ReceiverDispatcher> > ReceiverMap;
    typedef typename ReceiverMap::Iterator ReceiverMapIterator;
    HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> > mReceivers;
    HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> > mUnregisteredReceivers;
    Object mReceiversLock;

    typedef HashMap<AutoPtr<IServiceConnection>, AutoPtr<ServiceDispatcher> > ServiceMap;
    typedef typename ServiceMap::Iterator ServiceMapIterator;
    HashMap<AutoPtr<IContext>, AutoPtr<ServiceMap> > mServices;
    HashMap<AutoPtr<IContext>, AutoPtr<ServiceMap> > mUnboundServices;
    Object mServicesLock;

    Int32 mClientCount;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_LOADEDPKG_H__
