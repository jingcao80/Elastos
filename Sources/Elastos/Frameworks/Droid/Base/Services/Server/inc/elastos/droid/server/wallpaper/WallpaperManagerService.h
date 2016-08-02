
#ifndef __ELASTOS_DROID_SERVER_WALLPAPER_WALLPAPERMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_WALLPAPER_WALLPAPERMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/os/FileObserver.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IWallpaperInfo;
using Elastos::Droid::App::IIWallpaperManager;
using Elastos::Droid::App::IIWallpaperManagerCallback;
using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Service::Wallpaper::IIWallpaperConnection;
using Elastos::Droid::Service::Wallpaper::IIWallpaperEngine;
using Elastos::Droid::Service::Wallpaper::IIWallpaperService;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IIWindowManager;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wallpaper {

class WallpaperManagerService
    : public Object
    , public IWallpaperManagerService
    , public IIWallpaperManager
    , public IBinder
{
public:
    class WallpaperObserver;
    class WallpaperConnection;

protected:
    class WallpaperData
        : public Object
        , public IWallpaperData
    {
        friend class WallpaperManagerService;
    public:
        CAR_INTERFACE_DECL()

        WallpaperData(
            /* [in] */ Int32 userId,
            /* [in] */ WallpaperManagerService* host);

    public:
        Int32 mUserId;

        AutoPtr<IFile> mWallpaperFile;

        /**
         * Client is currently writing a new image wallpaper.
         */
        Boolean mImageWallpaperPending;

        /**
         * Resource name if using a picture from the wallpaper gallery
         */
        String mName;

        /**
         * The component name of the currently set live wallpaper.
         */
        AutoPtr<IComponentName> mWallpaperComponent;

        /**
         * The component name of the wallpaper that should be set next.
         */
        AutoPtr<IComponentName> mNextWallpaperComponent;

        AutoPtr<WallpaperConnection> mConnection;
        Int64 mLastDiedTime;
        Boolean mWallpaperUpdating;
        AutoPtr<WallpaperObserver> mWallpaperObserver;

    private:
        /**
         * List of callbacks registered they should each be notified when the wallpaper is changed.
         */
        AutoPtr<IRemoteCallbackList> mCallbacks;
        // private RemoteCallbackList<IWallpaperManagerCallback> callbacks
        //         = new RemoteCallbackList<IWallpaperManagerCallback>();

        WallpaperManagerService* mHost;

    protected:
        Int32 mWidth;
        Int32 mHeight;

        AutoPtr<IRect> mPadding;
    };

public:
    class KeyguardWallpaperData : public Object
    {
    public:
        KeyguardWallpaperData(
            /* [in] */ Int32 userId,
            /* [in] */ WallpaperManagerService* host);

    public:
        Int32 mUserId;

        AutoPtr<IFile> mWallpaperFile;

        /**
         * Client is currently writing a new image wallpaper.
         */
        Boolean mImageWallpaperPending;

        /**
         * Resource name if using a picture from the wallpaper gallery
         */
        String mName;

        Int32 mWidth;
        Int32 mHeight;

    private:
        /**
         * List of callbacks registered they should each be notified when the wallpaper is changed.
         */
        AutoPtr<IRemoteCallbackList> mCallbacks;
        WallpaperManagerService* mHost;

        friend class WallpaperManagerService;
    };

    /**
     * Observes the wallpaper for changes and notifies all IWallpaperServiceCallbacks
     * that the wallpaper has changed. The CREATE is triggered when there is no
     * wallpaper set and is created for the first time. The CLOSE_WRITE is triggered
     * everytime the wallpaper is changed.
     */
    class WallpaperObserver
        : public FileObserver
    {
    public:
        WallpaperObserver(
            /* [in] */ WallpaperData* wallpaper,
            /* [in] */ KeyguardWallpaperData* keyguardWallpaper,
            /* [in] */ WallpaperManagerService* host);

        // @Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    protected:
        AutoPtr<WallpaperData> mWallpaper;
        AutoPtr<KeyguardWallpaperData> mKeyguardWallpaper;
        AutoPtr<IFile> mWallpaperDir;
        AutoPtr<IFile> mWallpaperFile;
        AutoPtr<IFile> mKeyguardWallpaperFile;

    private:
        WallpaperManagerService* mHost;
    };

    class WallpaperConnection
        : public Object
        , public IWallpaperConnection
        , public IIWallpaperConnection
        , public IBinder
        , public IServiceConnection
    {
        friend class WallpaperData;
        friend class WallpaperManagerService;
    public:
        CAR_INTERFACE_DECL()

        WallpaperConnection();

        CARAPI constructor(
            /* [in] */ IWallpaperInfo* info,
            /* [in] */ IWallpaperData* wallpaper);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        // @Override
        CARAPI AttachEngine(
            /* [in] */ IIWallpaperEngine* engine);

        // @Override
        CARAPI EngineShown(
            /* [in] */ IIWallpaperEngine* engine);

        // @Override
        CARAPI SetWallpaper(
            /* [in] */ const String& name,
            /* [out] */ IParcelFileDescriptor** descriptor);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    protected:
        AutoPtr<IWallpaperInfo> mInfo;
        AutoPtr<IBinder> mToken;
        AutoPtr<IIWallpaperService> mService;
        AutoPtr<IIWallpaperEngine> mEngine;
        AutoPtr<IWallpaperData> mWallpaper;
        AutoPtr<IIRemoteCallback> mReply;

        Boolean mDimensionsChanged;
        Boolean mPaddingChanged;

    private:
        WallpaperManagerService* mHost;
    };

    class MyPackageMonitor
        : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /*  [in] */ WallpaperManagerService* host);

        // @Override
        CARAPI OnPackageUpdateFinished(
            /* [in] */ const String& packageName,
            /* [in] */  Int32 uid);

        // @Override
        CARAPI OnPackageModified(
            /* [in] */ const String& packageName);

        // @Override
        CARAPI OnPackageUpdateStarted(
            /* [in] */ const String& packageName,
            /* [in] */  Int32 uid);

        // @Override
        CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnSomePackagesChanged();

    protected:
        CARAPI DoPackagesChangedLocked(
            /* [in] */ Boolean doit,
            /* [in] */ IWallpaperData* wallpaper,
            /* [out] */ Boolean* result);

    private:
        WallpaperManagerService* mHost;
    };

    class SystemRunningBR
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WallpaperManagerService::SystemRunningBR")

        SystemRunningBR(
            /* [in] */ WallpaperManagerService* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WallpaperManagerService* mHost;
    };

    class SystemRunningObserver
        : public Object
        , public IIUserSwitchObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        SystemRunningObserver(
            /* [in] */ WallpaperManagerService* host);

        // @Override
        CARAPI OnUserSwitching(
            /* [in] */ Int32 newUserId,
            /* [in] */ IIRemoteCallback* reply);

        // @Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 newUserId);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        WallpaperManagerService* mHost;
    };

    friend class WallpaperObserver;
    friend class WallpaperConnection;
    friend class MyPackageMonitor;
public:
    CAR_INTERFACE_DECL()

    WallpaperManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemRunning();

    /** Called by SystemBackupAgent */
    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI ClearWallpaper();

    CARAPI ClearKeyguardWallpaper();

    CARAPI_(void) ClearKeyguardWallpaperLocked(
        /* [in] */ Int32 userId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI HasNamedWallpaper(
        /* [in] */ const String& name,
        /* [out] */ Boolean* hasNamedWallpaper);

    CARAPI SetDimensionHints(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GetWidthHint(
        /* [out] */ Int32* widthHint);

    CARAPI GetHeightHint(
        /* [out] */ Int32* heightHint);

    CARAPI SetDisplayPadding(
        /* [in] */ IRect* padding);

    CARAPI GetWallpaper(
        /* [in] */ IIWallpaperManagerCallback* cb,
        /* [out] */ IBundle** outParams,
        /* [out] */ IParcelFileDescriptor** wallpaper);

    CARAPI GetKeyguardWallpaper(
        /* [in] */ IIWallpaperManagerCallback* cb,
        /* [out] */ IBundle** outParams,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI GetWallpaperInfo(
        /* [out] */ IWallpaperInfo** info);

    CARAPI IsKeyguardWallpaperSet(
        /* [out] */ Boolean* result);

    CARAPI SetWallpaper(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI UpdateWallpaperBitmapLocked(
        /* [in] */ const String& name,
        /* [in] */ IWallpaperData* wallpaper,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI SetKeyguardWallpaper(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) UpdateKeyguardWallpaperBitmapLocked(
        /* [in] */ const String& name,
        /* [in] */ KeyguardWallpaperData* wallpaper);

    CARAPI SetWallpaperComponent(
        /* [in] */ IComponentName* name);

    // Called by SystemBackupAgent after files are restored to disk.
    CARAPI SettingsRestored();

    ///pay for someone's error,he write a ToString in Binder.car
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return NOERROR;
    }
    ///pay for someone's error

protected:
    // @Override
    CARAPI Finalize();

    CARAPI OnStoppingUser(
        /* [in] */ Int32 userId);

    CARAPI OnRemoveUser(
        /* [in] */ Int32 userId);

    CARAPI SwitchUser(
        /* [in] */ Int32 userId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI SwitchWallpaper(
        /* [in] */ IWallpaperData* wallpaper,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI ClearWallpaperLocked(
        /* [in] */ Boolean defaultFailed,
        /* [in] */ Int32 userId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI BindWallpaperComponentLocked(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Boolean force,
        /* [in] */ Boolean fromUser,
        /* [in] */ IWallpaperData* wallpaper,
        /* [in] */ IIRemoteCallback* reply,
        /* [out] */ Boolean* isBinded);

    CARAPI DetachWallpaperLocked(
        /* [in] */ IWallpaperData* wallpaper);

    CARAPI ClearWallpaperComponentLocked(
        /* [in] */ IWallpaperData* wallpaper);

    CARAPI AttachServiceLocked(
        /* [in] */ IWallpaperConnection* conn,
        /* [in] */ IWallpaperData* wallpaper);

    CARAPI RestoreNamedResourceLocked(
        /* [in] */ IWallpaperData* wallpaper,
        /* [out] */ Boolean* isRestored);

private:
    static CARAPI GetWallpaperDir(
        /* [in] */ Int32 userId,
        /* [out] */ IFile** wallpaperDir);

    CARAPI GetDefaultDisplaySize(
        /* [out] */ IPoint** displaySize);

    CARAPI NotifyCallbacksLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) NotifyCallbacksLocked(
        /* [in] */ KeyguardWallpaperData* wallpaper);

    CARAPI CheckPermission(
        /* [in] */ const String& permission);

    static CARAPI_(AutoPtr<IJournaledFile>) MakeJournaledFile(
        /* [in] */ Int32 userId);

    static CARAPI_(AutoPtr<IJournaledFile>) MakeJournaledFile(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI SaveSettingsLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) SaveSettingsLocked(
        /* [in] */ KeyguardWallpaperData* wallpaper);

    CARAPI MigrateFromOld();

    CARAPI_(Int32) GetAttributeInt32(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& name,
        /* [in] */ Int32 defValue);

    CARAPI LoadSettingsLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) LoadKeyguardSettingsLocked(
        /* [in] */ Int32 userId);

    CARAPI_(Int32) GetMaximumSizeDimension();

protected:
    const static String TAG;
    const static Boolean DEBUG;

    // final Object mLock = new Object[0];

    /**
     * Minimum time between crashes of a wallpaper service for us to consider
     * restarting it vs. just reverting to the static wallpaper.
     */
    const static Int64 MIN_WALLPAPER_CRASH_TIME = 10000;
    const static String WALLPAPER;
    const static String WALLPAPER_INFO;
    static const String KEYGUARD_WALLPAPER;
    static const String KEYGUARD_WALLPAPER_INFO;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWindowManager> mIWindowManager;
    AutoPtr<IIPackageManager> mIPackageManager;
    AutoPtr<MyPackageMonitor> mMonitor;
    AutoPtr<WallpaperData> mLastWallpaper;

    /**
     * Name of the component used to display bitmap wallpapers from either the gallery or
     * built-in wallpapers.
     */
    AutoPtr<IComponentName> mImageWallpaper;

    AutoPtr<ISparseArray> mWallpaperMap;
    HashMap<Int32, AutoPtr<KeyguardWallpaperData> > mKeyguardWallpaperMap;

    Int32 mCurrentUserId;

private:
    friend class KeyguardWallpaperData;
};

} // namespace Wallpaper
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WALLPAPER_WALLPAPERMANAGERSERVICE_H__