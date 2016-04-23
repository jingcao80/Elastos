#ifndef __ELASTOS_DROID_SERVER_CTHEMESERVICE_H__
#define __ELASTOS_DROID_SERVER_CTHEMESERVICE_H__

#include "_Elastos_Droid_Server_CThemeService.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IIThemeChangeListener;
using Elastos::Droid::Content::Res::IIThemeProcessingListener;
using Elastos::Droid::Content::Res::IIThemeService;
using Elastos::Droid::Content::Res::IThemeChangeRequest;
using Elastos::Droid::Content::Res::IThemeConfig;
using Elastos::Droid::Content::Res::IThemeConfigBuilder;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::IO::IFile;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Core::IComparator;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CThemeService)
    , public Object
    , public IIThemeService
    , public IBinder
{
private:
    class ThemeWorkerHandler
        : public Handler
    {
        friend class CThemeService;

    public:
        TO_STRING_IMPL("CThemeService::ThemeWorkerHandler")

        ThemeWorkerHandler(
            /* [in] */ CThemeService* host,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        static const Int32 MESSAGE_CHANGE_THEME;
        static const Int32 MESSAGE_APPLY_DEFAULT_THEME;
        static const Int32 MESSAGE_REBUILD_RESOURCE_CACHE;
        CThemeService* mHost;
    };

    class ResourceProcessingHandler
        : public Handler
    {
        friend class CThemeService;

    public:
        TO_STRING_IMPL("CThemeService::ResourceProcessingHandler")

        ResourceProcessingHandler(
            /* [in] */ CThemeService* host,
            /* [in] */ ILooper* lp);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        static const Int32 MESSAGE_QUEUE_THEME_FOR_PROCESSING;
        static const Int32 MESSAGE_DEQUEUE_AND_PROCESS_THEME;
        CThemeService* mHost;
    };

    class MyWallpaperChangeReceiver
        : public BroadcastReceiver
    {
    public:
        MyWallpaperChangeReceiver(
            /* [in] */ CThemeService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CThemeService* mHost;
    };

    class MyUserChangeReceiver
        : public BroadcastReceiver
    {
    public:
        MyUserChangeReceiver(
            /* [in] */ CThemeService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CThemeService* mHost;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        MyComparator();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ CThemeService* host);

        CARAPI Register(
            /* [in] */ Boolean bregister);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IUri> ALARM_ALERT_URI;
        AutoPtr<IUri> NOTIFICATION_URI;
        AutoPtr<IUri> RINGTONE_URI;
        CThemeService* mHost;
    };

public:
    CThemeService();

    ~CThemeService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) SystemRunning();

    // @Override
    CARAPI RequestThemeChangeUpdates(
        /* [in] */ IIThemeChangeListener* listener);

    // @Override
    CARAPI RemoveUpdates(
        /* [in] */ IIThemeChangeListener* listener);

    // @Override
    CARAPI RequestThemeChange(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Boolean removePerAppThemes);

    // @Override
    CARAPI ApplyDefaultTheme();

    // @Override
    CARAPI IsThemeApplying(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetProgress(
        /* [out] */ Int32* result);

    // @Override
    CARAPI CacheComposedIcon(
        /* [in] */ IBitmap* icon,
        /* [in] */ const String& fileName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ProcessThemeResources(
        /* [in] */ const String& themePkgName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsThemeBeingProcessed(
        /* [in] */ const String& themePkgName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RegisterThemeProcessingListener(
        /* [in] */ IIThemeProcessingListener* listener);

    // @Override
    CARAPI UnregisterThemeProcessingListener(
        /* [in] */ IIThemeProcessingListener* listener);

    // @Override
    CARAPI RebuildResourceCache();

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(void) RemoveObsoleteThemeOverlayIfExists();

    CARAPI_(Boolean) IsThemeCompatibleWithUpgradedApi(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) IsThemeApiUpToDate();

    CARAPI_(void) UpdateThemeApi();

    CARAPI_(void) DoApplyTheme(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Boolean removePerAppTheme);

    CARAPI_(void) DoApplyDefaultTheme();

    CARAPI_(void) DoRebuildResourceCache();

    CARAPI_(void) UpdateProvider(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Int64 updateTime);

    CARAPI_(Boolean) UpdateIcons(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateFonts(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateBootAnim(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateAlarms(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateNotifications(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateRingtones(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateAudible(
        /* [in] */ const String& dirPath,
        /* [in] */ const String& assetPath,
        /* [in] */ Int32 type,
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateLockscreen(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) SetCustomLockScreenWallpaper(
        /* [in] */ const String& pkgName);

    CARAPI_(Boolean) UpdateWallpaper(
        /* [in] */ const String pkgName);

    CARAPI_(Boolean) UpdateConfiguration(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Boolean removePerAppThemes);

    CARAPI_(Boolean) UpdateConfiguration(
        /* [in] */ IThemeConfig* themeConfig);

    CARAPI_(Boolean) ShouldUpdateConfiguration(
        /* [in] */ IThemeChangeRequest* request);

    static CARAPI_(AutoPtr<IThemeConfigBuilder>) CreateBuilderFrom(
        /* [in] */ IConfiguration* config,
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ const String& pkgName,
        /* [in] */ Boolean removePerAppThemes);

    static CARAPI_(void) RemovePerAppThemesFromConfig(
        /* [in] */ IThemeConfigBuilder* builder,
        /* [in] */ IThemeConfig* themeConfig);

    // Kill the current Home process, they tend to be evil and cache
    // drawable references in all apps
    CARAPI_(void) KillLaunchers(
        /* [in] */ IThemeChangeRequest* request);

    CARAPI_(Boolean) IsSetupActivity(
        /* [in] */ IResolveInfo* info);

    CARAPI_(Boolean) HandlesThemeChanges(
        /* [in] */ const String& pkgName,
        /* [in] */ IList* infos);

    CARAPI_(void) PostProgress();

    CARAPI_(void) PostFinish(
        /* [in] */ Boolean isSuccess,
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Int64 updateTime);

    CARAPI_(void) PostFinishedProcessing(
        /* [in] */ const String& pkgName);

    CARAPI_(void) BroadcastThemeChange(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Int64 updateTime);

    CARAPI_(void) IncrementProgress(
        /* [in] */ Int32 increment);

    CARAPI_(void) PurgeIconCache();

    CARAPI_(Boolean) ApplyBootAnimation(
        /* [in] */ const String& themePath);

    CARAPI_(void) ClearBootAnimation();

    CARAPI_(void) ProcessInstalledThemes();

    CARAPI_(void) SendThemeResourcesCachedBroadcast(
        /* [in] */ const String& themePkgName,
        /* [in] */ Int32 resultCode);

    /**
     * Posts a notification to let the user know the theme was not installed.
     * @param name
     */
    CARAPI_(void) PostFailedThemeInstallNotification(
        /* [in] */ const String& name);

    CARAPI_(String) GetThemeName(
        /* [in] */ IPackageInfo* pi);

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const String GOOGLE_SETUPWIZARD_PACKAGE;
    static const String CM_SETUPWIZARD_PACKAGE;

    static const Int64 MAX_ICON_CACHE_SIZE; // 32MB
    static const Int64 PURGED_ICON_CACHE_SIZE; // 24 MB

    // Defines a min and max compatible api level for themes on this system.
    static const Int32 MIN_COMPATIBLE_VERSION;

    AutoPtr<IHandlerThread> mWorker;
    AutoPtr<ThemeWorkerHandler> mHandler;
    AutoPtr<ResourceProcessingHandler> mResourceProcessingHandler;
    static AutoPtr<IContext> mContext;
    static AutoPtr<IPackageManager> mPM;
    Int32 mProgress;
    static Boolean mWallpaperChangedByUs;
    Int64 mIconCacheSize;

    Boolean mIsThemeApplying;
    AutoPtr<IRemoteCallbackList> mClients;

    AutoPtr<IRemoteCallbackList> mProcessingListeners;

    static AutoPtr<IArrayList> mThemesToProcessQueue;

    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<MyWallpaperChangeReceiver> mWallpaperChangeReceiver;

    AutoPtr<MyUserChangeReceiver> mUserChangeReceiver;

    AutoPtr<MyComparator> mOldestFilesFirstComparator;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_CTHEMESERVICE_H__