
#ifndef __ELASTOS_DROID_SERVER_CSYSTEMSERVER_H__
#define __ELASTOS_DROID_SERVER_CSYSTEMSERVER_H__

#include "_Elastos_Droid_Server_CSystemServer.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/display/CDisplayManagerService.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/server/pm/Installer.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/server/power/PowerManagerService.h"
#include "elastos/droid/server/statusbar/CStatusBarManagerService.h"
#include "elastos/droid/server/wallpaper/CWallpaperManagerService.h"
#include "elastos/droid/server/CTextServicesManagerService.h"
#include "elastos/droid/server/CNetworkManagementService.h"
#include "elastos/droid/server/net/CNetworkStatsService.h"
#include "elastos/droid/server/net/CNetworkPolicyManagerService.h"
#include "elastos/droid/server/CConnectivityService.h"
#include "elastos/droid/server/CNetworkScoreService.h"
#include <elastos/core/Singleton.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Server::Am::CActivityManagerService;
using Elastos::Droid::Server::Display::CDisplayManagerService;
using Elastos::Droid::Server::Input::CInputManagerService;
using Elastos::Droid::Server::Pm::Installer;
using Elastos::Droid::Server::Pm::CPackageManagerService;
using Elastos::Droid::Server::Power::PowerManagerService;
using Elastos::Droid::Server::StatusBar::CStatusBarManagerService;
using Elastos::Droid::Server::Wallpaper::CWallpaperManagerService;
using Elastos::Droid::Server::Net::CNetworkStatsService;
using Elastos::Droid::Server::Net::CNetworkPolicyManagerService;
using Elastos::Utility::ITimer;


namespace Elastos {
namespace Droid {
namespace Server {

class CInputMethodManagerService;

class SystemServer
    : public Object
{
private:
    class ServiceBundle
        : public Object
    {
    public:
        // AutoPtr<CMountService> mMountServiceF;
        AutoPtr<CNetworkManagementService> mNetworkManagementF;
        AutoPtr<CNetworkStatsService> mNetworkStatsF;
        AutoPtr<CNetworkPolicyManagerService> mNetworkPolicyF;
        AutoPtr<CConnectivityService> mConnectivityF;
        AutoPtr<CNetworkScoreService> mNetworkScoreF;
        AutoPtr<CWallpaperManagerService> mWallpaperF;
        AutoPtr<CInputMethodManagerService> mImmF;
        // AutoPtr<CLocationManagerService> mLocationF;
        // AutoPtr<CCountryDetectorService> mCountryDetectorF;
        // AutoPtr<CNetworkTimeUpdateService> mNetworkTimeUpdaterF;
        // AutoPtr<CCommonTimeManagementService> mCommonTimeMgmtServiceF;
        AutoPtr<CTextServicesManagerService> mTextServiceManagerServiceF;
        AutoPtr<CStatusBarManagerService> mStatusBarF;
        // AutoPtr<CAssetAtlasService> mAtlasF;
        AutoPtr<CInputManagerService> mInputManagerF;
        // AutoPtr<CTelephonyRegistry> mTelephonyRegistryF;
        // AutoPtr<CMediaRouterService> mMediaRouterF;
        // AutoPtr<CAudioService> mAudioServiceF;
        // AutoPtr<CMmsServiceBroker> mMmsServiceF;
    };

    class SystemReadyRunnable
        : public Runnable
    {
    public:
        SystemReadyRunnable(
            /* [in] */ SystemServer* host,
            /* [in] */ ServiceBundle* bundle);

        CARAPI Run();

    private:
        SystemServer* mHost;
        AutoPtr<ServiceBundle> mServiceBundle;
    };

public:
    SystemServer();

    CARAPI Run();

    static CARAPI StartSystemUi(
        /* [in] */ IContext* context);

private:
    /**
     * Called to initialize native system services.
     */
    static CARAPI NativeInit();

    CARAPI_(void) ReportWtf(
        /* [in] */ const char* msg,
        /* [in] */ ECode ec);

    CARAPI PerformPendingShutdown();

    CARAPI CreateSystemContext();

    /**
     * Starts the small tangle of critical services that are needed to get
     * the system off the ground.  These services have complex mutual dependencies
     * which is why we initialize them all in one place here.  Unless your service
     * is also entwined in these dependencies, it should be initialized in one of
     * the other functions.
     */
    CARAPI StartBootstrapServices();

    /**
     * Starts some essential services that are not tangled up in the bootstrap process.
     */
    CARAPI StartCoreServices();

    /**
     * Starts a miscellaneous grab bag of stuff that has yet to be refactored
     * and organized.
     */
    CARAPI StartOtherServices();

private:
    friend class SystemReadyRunnable;

    static const String TAG;

    static const String ENCRYPTING_STATE;
    static const String ENCRYPTED_STATE;

    static const Int64 SNAPSHOT_INTERVAL; // 1hr

    // The earliest supported time.  We pick one day into 1970, to
    // give any timezone code room without going into negative time.
    static const Int64 EARLIEST_SUPPORTED_TIME;

    /*
     * Implementation class names. TODO: Move them to a codegen class or load
     * them from the build system somehow.
     */
    static const String BACKUP_MANAGER_SERVICE_CLASS;
    static const String APPWIDGET_SERVICE_CLASS;
    static const String VOICE_RECOGNITION_MANAGER_SERVICE_CLASS;
    static const String PRINT_MANAGER_SERVICE_CLASS;
    static const String USB_SERVICE_CLASS;
    static const String WIFI_SERVICE_CLASS;
    static const String WIFI_P2P_SERVICE_CLASS;
    static const String ETHERNET_SERVICE_CLASS;
    static const String JOB_SCHEDULER_SERVICE_CLASS;
    static const String PERSISTENT_DATA_BLOCK_PROP;

    Int32 mFactoryTestMode;
    AutoPtr<ITimer> mProfilerSnapshotTimer;

    AutoPtr<IContext> mSystemContext;
    AutoPtr<ISystemServiceManager> mSystemServiceManager;

    // TODO: remove all of these references by improving dependency resolution and boot phases
    AutoPtr<Installer> mInstaller;
    AutoPtr<PowerManagerService> mPowerManagerService;
    AutoPtr<CActivityManagerService> mActivityManagerService;
    AutoPtr<CDisplayManagerService> mDisplayManagerService;
    AutoPtr<CPackageManagerService> mPackageManagerService;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IContentResolver> mContentResolver;

    Boolean mOnlyCore;
    Boolean mFirstBoot;
};


CarClass(CSystemServer)
    , public Singleton
    , public ISystemServer
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * The main entry point from zygote.
     */
    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__CSYSTEMSERVER_H__
