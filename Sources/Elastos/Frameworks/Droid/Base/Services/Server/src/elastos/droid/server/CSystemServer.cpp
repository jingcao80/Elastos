
#include "elastos/droid/server/CSystemServer.h"
#include "elastos/droid/server/SystemServiceManager.h"
#include <elastos/droid/server/LocalServices.h>

#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Webkit.h>
#include <elastos/droid/R.h>
#include <elastos/droid/os/FactoryTest.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/Build.h>
#include <elastos/utility/logging/Slogger.h>

#include <SensorService.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <utils/misc.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::FactoryTest;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::App::IContextImpl;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Webkit::IWebViewFactory;
// using Elastos::Droid::Webkit::CWebViewFactory;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Server {

const String SystemServer::TAG("SystemServer");

const String SystemServer::ENCRYPTING_STATE("trigger_restart_min_framework");
const String SystemServer::ENCRYPTED_STATE("1");

const Int64 SystemServer::SNAPSHOT_INTERVAL = 60 * 60 * 1000; // 1hr
const Int64 SystemServer::EARLIEST_SUPPORTED_TIME = 86400 * 1000;

const String SystemServer::BACKUP_MANAGER_SERVICE_CLASS(
    "com.android.server.backup.BackupManagerService$Lifecycle");
const String SystemServer::APPWIDGET_SERVICE_CLASS(
    "com.android.server.appwidget.AppWidgetService");
const String SystemServer::VOICE_RECOGNITION_MANAGER_SERVICE_CLASS(
    "com.android.server.voiceinteraction.VoiceInteractionManagerService");
const String SystemServer::PRINT_MANAGER_SERVICE_CLASS(
    "com.android.server.print.PrintManagerService");
const String SystemServer::USB_SERVICE_CLASS(
    "com.android.server.usb.UsbService$Lifecycle");
const String SystemServer::WIFI_SERVICE_CLASS(
    "com.android.server.wifi.WifiService");
const String SystemServer::WIFI_P2P_SERVICE_CLASS(
    "com.android.server.wifi.p2p.WifiP2pService");
const String SystemServer::ETHERNET_SERVICE_CLASS(
    "com.android.server.ethernet.EthernetService");
const String SystemServer::JOB_SCHEDULER_SERVICE_CLASS(
    "com.android.server.job.JobSchedulerService");
const String SystemServer::PERSISTENT_DATA_BLOCK_PROP("ro.frp.pst");

//===============================================================================
// CSystemServer
//===============================================================================
CAR_INTERFACE_IMPL(CSystemServer, Object, ISystemServer)

CAR_SINGLETON_IMPL(CSystemServer)

ECode CSystemServer::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    AutoPtr<SystemServer> systemServer = new SystemServer();
    return systemServer->Run();
}

//===============================================================================
// SystemServer
//===============================================================================
SystemServer::SystemServer()
    : mOnlyCore(FALSE)
    , mFirstBoot(FALSE)
{
    // Check for factory test mode.
    mFactoryTestMode = FactoryTest::GetMode();
}

ECode SystemServer::NativeInit()
{
    char propBuf[PROPERTY_VALUE_MAX];
    property_get("system_init.startsensorservice", propBuf, "1");
    if (strcmp(propBuf, "1") == 0) {
        // Start the sensor service
        android::SensorService::instantiate();
    }
    return NOERROR;
}

ECode SystemServer::Run()
{
    ECode ec = NOERROR;

    // If a device's clock is before 1970 (before 0), a lot of
    // APIs crash dealing with negative numbers, notably
    // java.io.File#setLastModified, so instead we fake it and
    // hope that time from cell towers or NTP fixes it shortly.
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 ms;
    system->GetCurrentTimeMillis(&ms);
    if (ms < EARLIEST_SUPPORTED_TIME) {
        Slogger::W(TAG, "System clock is before 1970; setting to 1970.");
        SystemClock::SetCurrentTimeMillis(EARLIEST_SUPPORTED_TIME);
    }

    // Here we go!
    Slogger::I(TAG, "Entered the Android system server!");
    // EventLog.writeEvent(EventLogTags.BOOT_PROGRESS_SYSTEM_RUN, SystemClock::uptimeMillis());

    // In case the runtime switched since last boot (such as when
    // the old runtime was removed in an OTA), set the system
    // property so that it is in sync. We can't do this in
    // libnativehelper's JniInvocation::Init code where we already
    // had to fallback to a different runtime because it is
    // running as root and we need to be the system user to set
    // the property. http://b/11463182

    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    // systemProperties->Set(String("persist.sys.dalvik.vm.lib.2"), VMRuntime.getRuntime().vmLibrary());

    // // Enable the sampling profiler.
    // if (SamplingProfilerIntegration.isEnabled()) {
    //     SamplingProfilerIntegration.start();
    //     mProfilerSnapshotTimer = new Timer();
    //     mProfilerSnapshotTimer.schedule(new TimerTask() {
    //         @Override
    //         public void run() {
    //             SamplingProfilerIntegration.writeSnapshot("system_server", NULL);
    //         }
    //     }, SNAPSHOT_INTERVAL, SNAPSHOT_INTERVAL);
    // }

    // // Mmmmmm... more memory!
    // VMRuntime.getRuntime().clearGrowthLimit();

    // // The system server has to run all of the time, so it needs to be
    // // as efficient as possible with its memory usage.
    // VMRuntime.getRuntime().setTargetHeapUtilization(0.8f);

    // Some devices rely on runtime fingerprint generation, so make sure
    // we've defined it before booting further.
    Build::EnsureFingerprintProperty();

    // Within the system server, it is an error to access Environment paths without
    // explicitly specifying a user.
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    // environment->SetUserRequired(TRUE);

    // Ensure binder calls into the system always run at foreground priority.
    // BinderInternal.disableBackgroundScheduling(true);

    // Prepare the main looper thread (this thread).
    // android.os.Process.setThreadPriority(
    //         android.os.Process.THREAD_PRIORITY_FOREGROUND);
    // android.os.Process.setCanSelfBackground(false);
    AutoPtr<ILooperHelper> looper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looper);
    looper->PrepareMainLooper();

    // Initialize native services.
    // System.loadLibrary("android_servers");
    NativeInit();

    // Check whether we failed to shut down last time we tried.
    // This call may not return.
    PerformPendingShutdown();

    // Initialize the system context.
    CreateSystemContext();

    // // Create the system service manager.
    mSystemServiceManager = (ISystemServiceManager*)new SystemServiceManager(mSystemContext);
    LocalServices::AddService(EIID_ISystemServiceManager, mSystemServiceManager.Get());

    // Start services.
    {
        ec = StartBootstrapServices();
        FAIL_GOTO(ec, _EXIT_)

        ec = StartCoreServices();
        FAIL_GOTO(ec, _EXIT_)

        ec = StartOtherServices();
        FAIL_GOTO(ec, _EXIT_)
    }

    // // For debug builds, log event loop stalls to dropbox for analysis.
    // if (StrictMode.conditionallyEnableDebugLogging()) {
    //     Slogger::I(TAG, "Enabled StrictMode for system server main thread.");
    // }

    // Loop forever.
    looper->Loop();

    Slogger::E(TAG, "Main thread loop unexpectedly exited");
    return E_RUNTIME_EXCEPTION;

_EXIT_:
    Slogger::E("System", "******************************************");
    Slogger::E("System", "************ Failure starting system services, error ecode: %08x", ec);
    return ec;
}

void SystemServer::ReportWtf(
    /* [in] */ const char* msg,
    /* [in] */ ECode ec)
{
    Slogger::W(TAG, "***********************************************");
    Slogger::W(TAG, "BOOT FAILURE %s, error code: %08x", msg, ec);
}

ECode SystemServer::PerformPendingShutdown()
{
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    String shutdownAction;
    assert(0 && "TODO");
    // systemProperties->Get(ShutdownThread::SHUTDOWN_ACTION_PROPERTY, String(""), &shutdownAction);
    if (!shutdownAction.IsNullOrEmpty()) {
        Boolean reboot = (shutdownAction.GetChar(0) == '1');

        String reason;
        if (shutdownAction.GetLength() > 1) {
            reason = shutdownAction.Substring(1, shutdownAction.GetLength());
        }

        // ShutdownThread::RebootOrShutdown(reboot, reason);
    }
    return NOERROR;
}

ECode SystemServer::CreateSystemContext()
{
    AutoPtr<IActivityThread> activityThread;
    AutoPtr<IActivityThreadHelper> helper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
    helper->GetSystemMain((IActivityThread**)&activityThread);
    AutoPtr<IContextImpl> ctxImpl;
    activityThread->GetSystemContext((IContextImpl**)&ctxImpl);
    mSystemContext = IContext::Probe(ctxImpl);
    mSystemContext->SetTheme(R::style::Theme_DeviceDefault_Light_DarkActionBar);
    return NOERROR;
}

ECode SystemServer::StartBootstrapServices()
{
    AutoPtr<ISystemService> service;

    // // Wait for installd to finish starting up so that it has a chance to
    // // create critical directories such as /data/user with the appropriate
    // // permissions.  We need this to complete before we initialize other services.
    // mInstaller = mSystemServiceManager.startService(Installer.class);

    // // Activity manager runs the show.
    // mActivityManagerService = mSystemServiceManager.startService(
    //         ActivityManagerService.Lifecycle.class).getService();
    // mActivityManagerService.setSystemServiceManager(mSystemServiceManager);

    // // Power manager needs to be started early because other services need it.
    // // Native daemons may be watching for it to be registered so it must be ready
    // // to handle incoming binder calls immediately (including being able to verify
    // // the permissions for those calls).
    // mPowerManagerService = mSystemServiceManager.startService(PowerManagerService.class);

    // // Now that the power manager has been started, let the activity manager
    // // initialize power management features.
    // mActivityManagerService.initPowerManagement();

    // Display manager is needed to provide display metrics before package manager
    // starts up.
    service = NULL;
    CDisplayManagerService::New(mSystemContext, (ISystemService**)&service);
    mSystemServiceManager->StartService(service);
    mDisplayManagerService = (CDisplayManagerService*)service.Get();

    // We need the default display before we can initialize the package manager.
    mSystemServiceManager->StartBootPhase(ISystemService::PHASE_WAIT_FOR_DEFAULT_DISPLAY);

    // // Only run "core" apps if we're encrypting the device.
    // String cryptState = SystemProperties.get("vold.decrypt");
    // if (ENCRYPTING_STATE.equals(cryptState)) {
    //     Slogger::W(TAG, "Detected encryption in progress - only parsing core apps");
    //     mOnlyCore = true;
    // } else if (ENCRYPTED_STATE.equals(cryptState)) {
    //     Slogger::W(TAG, "Device encrypted - only parsing core apps");
    //     mOnlyCore = true;
    // }

    // // Start the package manager.
    // Slogger::I(TAG, "Package Manager");
    // mPackageManagerService = PackageManagerService.main(mSystemContext, mInstaller,
    //         mFactoryTestMode != FactoryTest.FACTORY_TEST_OFF, mOnlyCore);
    // mFirstBoot = mPackageManagerService.isFirstBoot();
    // mPackageManager = mSystemContext.getPackageManager();

    // Slogger::I(TAG, "User Service");
    // ServiceManager.addService(Context.USER_SERVICE, UserManagerService.getInstance());

    // // Initialize attribute cache used to cache resources from packages.
    // AttributeCache.init(mSystemContext);

    // // Set up the Application instance for the system process and get started.
    // mActivityManagerService.setSystemProcess();
    return NOERROR;
}


ECode SystemServer::StartCoreServices()
{
    // // Manages LEDs and display backlight.
    // mSystemServiceManager.startService(LightsService.class);

    // // Tracks the battery level.  Requires LightService.
    // mSystemServiceManager.startService(BatteryService.class);

    // // Tracks application usage stats.
    // mSystemServiceManager.startService(UsageStatsService.class);
    // mActivityManagerService.setUsageStatsManager(
    //         LocalServices.getService(UsageStatsManagerInternal.class));

    // // Tracks whether the updatable WebView is in a ready state and watches for update installs.
    // mSystemServiceManager.startService(WebViewUpdateService.class);
    return NOERROR;
}


ECode SystemServer::StartOtherServices()
{
    ECode ec = NOERROR;
    AutoPtr<IContext> context = mSystemContext;
    // AutoPtr<CAccountManagerService> accountManager;
    // AutoPtr<CContentService> contentService;
    // AutoPtr<CVibratorService> vibrator;
    // AutoPtr<IIAlarmManager> alarm;
    // AutoPtr<CMountService> mountService;
    // AutoPtr<CNetworkManagementService> networkManagement;
    // AutoPtr<CNetworkStatsService> networkStats;
    // AutoPtr<CNetworkPolicyManagerService> networkPolicy;
    // AutoPtr<CConnectivityService> connectivity;
    // AutoPtr<CNetworkScoreService> networkScore;
    // AutoPtr<CNsdService> serviceDiscovery= NULL;
    // AutoPtr<CWindowManagerService> wm;
    // AutoPtr<CBluetoothManagerService> bluetooth;
    // AutoPtr<CUsbService> usb;
    // AutoPtr<CSerialService> serial;
    // AutoPtr<CNetworkTimeUpdateService> networkTimeUpdater;
    // AutoPtr<CCommonTimeManagementService> commonTimeMgmtService;
    // AutoPtr<CInputManagerService> inputManager;
    // AutoPtr<CTelephonyRegistry> telephonyRegistry;
    // AutoPtr<CConsumerIrService> consumerIr;
    // AutoPtr<CAudioService> audioService;
    // AutoPtr<CMmsServiceBroker> mmsService;

    // AutoPtr<CStatusBarManagerService> statusBar;
    // AutoPtr<IINotificationManager> notification;
    // AutoPtr<CInputMethodManagerService> imm;
    // AutoPtr<CWallpaperManagerService> wallpaper;
    // AutoPtr<CLocationManagerService> location;
    // AutoPtr<CCountryDetectorService> countryDetector;
    // AutoPtr<CTextServicesManagerService> tsms;
    // AutoPtr<CLockSettingsService> lockSettings;
    // AutoPtr<CAssetAtlasService> atlas;
    // AutoPtr<CMediaRouterService> mediaRouter;

    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);

    Boolean disableStorage, disableMedia, disableBluetooth, disableTelephony;
    Boolean disableLocation, disableSystemUI, disableNonCoreServices, disableNetwork;
    systemProperties->GetBoolean(String("config.disable_storage"), FALSE, &disableStorage);
    systemProperties->GetBoolean(String("config.disable_media"), FALSE, &disableMedia);
    systemProperties->GetBoolean(String("config.disable_bluetooth"), FALSE, &disableBluetooth);
    systemProperties->GetBoolean(String("config.disable_telephony"), FALSE, &disableTelephony);
    systemProperties->GetBoolean(String("config.disable_location"), FALSE, &disableLocation);
    systemProperties->GetBoolean(String("config.disable_systemui"), FALSE, &disableSystemUI);
    systemProperties->GetBoolean(String("config.disable_noncore"), FALSE, &disableNonCoreServices);
    systemProperties->GetBoolean(String("config.disable_network"), FALSE, &disableNetwork);
    String str;
    systemProperties->Get(String("ro.kernel.qemu"), &str);
    Boolean isEmulator = str.Equals("1");

    // try {
    Slogger::I(TAG, "Reading configuration...");
//     SystemConfig.getInstance();

//     Slogger::I(TAG, "Scheduling Policy");
//     ServiceManager.addService("scheduling_policy", new SchedulingPolicyService());

//     Slogger::I(TAG, "Telephony Registry");
//     telephonyRegistry = new TelephonyRegistry(context);
//     ServiceManager.addService("telephony.registry", telephonyRegistry);

//     Slogger::I(TAG, "Entropy Mixer");
//     ServiceManager.addService("entropy", new EntropyMixer(context));

    context->GetContentResolver((IContentResolver**)&mContentResolver);

//     // The AccountManager must come before the ContentService
//     try {
//         // TODO: seems like this should be disable-able, but req'd by ContentService
//         Slogger::I(TAG, "Account Manager");
//         accountManager = new AccountManagerService(context);
//         ServiceManager.addService(Context.ACCOUNT_SERVICE, accountManager);
//     } catch (Throwable e) {
//         Slog.e(TAG, "Failure starting Account Manager", ec);
//     }

//     Slogger::I(TAG, "Content Manager");
//     contentService = ContentService.main(context,
//             mFactoryTestMode == FactoryTest.FACTORY_TEST_LOW_LEVEL);

//     Slogger::I(TAG, "System Content Providers");
//     mActivityManagerService.installSystemProviders();

//     Slogger::I(TAG, "Vibrator Service");
//     vibrator = new VibratorService(context);
//     ServiceManager.addService("vibrator", vibrator);

//     Slogger::I(TAG, "Consumer IR Service");
//     consumerIr = new ConsumerIrService(context);
//     ServiceManager.addService(Context.CONSUMER_IR_SERVICE, consumerIr);

//     mSystemServiceManager.startService(AlarmManagerService.class);
//     alarm = IAlarmManager.Stub.asInterface(
//             ServiceManager.getService(Context.ALARM_SERVICE));

//     Slogger::I(TAG, "Init Watchdog");
//     final Watchdog watchdog = Watchdog.getInstance();
//     watchdog.init(context, mActivityManagerService);

//     Slogger::I(TAG, "Input Manager");
//     inputManager = new InputManagerService(context);

//     Slogger::I(TAG, "Window Manager");
//     wm = WindowManagerService.main(context, inputManager,
//             mFactoryTestMode != FactoryTest.FACTORY_TEST_LOW_LEVEL,
//             !mFirstBoot, mOnlyCore);
//     ServiceManager.addService(Context.WINDOW_SERVICE, wm);
//     ServiceManager.addService(Context.INPUT_SERVICE, inputManager);

//     mActivityManagerService.setWindowManager(wm);

//     inputManager.setWindowManagerCallbacks(wm.getInputMonitor());
//     inputManager.start();

//     // TODO: Use service dependencies instead.
//     mDisplayManagerService.windowManagerAndInputReady();

//     // Skip Bluetooth if we have an emulator kernel
//     // TODO: Use a more reliable check to see if this product should
//     // support Bluetooth - see bug 988521
//     if (isEmulator) {
//         Slogger::I(TAG, "No Bluetooh Service (emulator)");
//     } else if (mFactoryTestMode == FactoryTest.FACTORY_TEST_LOW_LEVEL) {
//         Slogger::I(TAG, "No Bluetooth Service (factory test)");
//     } else if (!context.getPackageManager().hasSystemFeature
//                (PackageManager.FEATURE_BLUETOOTH)) {
//         Slogger::I(TAG, "No Bluetooth Service (Bluetooth Hardware Not Present)");
//     } else if (disableBluetooth) {
//         Slogger::I(TAG, "Bluetooth Service disabled by config");
//     } else {
//         Slogger::I(TAG, "Bluetooth Manager Service");
//         bluetooth = new BluetoothManagerService(context);
//         ServiceManager.addService(BluetoothAdapter.BLUETOOTH_MANAGER_SERVICE, bluetooth);
//     }
// } catch (RuntimeException e) {
//     Slog.e("System", "******************************************");
//     Slog.e("System", "************ Failure starting core service", ec);
// }


    // // Bring up services needed for UI.
    // if (mFactoryTestMode != FactoryTest.FACTORY_TEST_LOW_LEVEL) {
    //     //if (!disableNonCoreServices) { // TODO: View depends on these; mock them?
    //     if (true) {
    //         try {
    //             Slogger::I(TAG, "Input Method Service");
    //             imm = new InputMethodManagerService(context, wm);
    //             ServiceManager.addService(Context.INPUT_METHOD_SERVICE, imm);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Input Manager Service", ec);
    //         }

    //         try {
    //             Slogger::I(TAG, "Accessibility Manager");
    //             ServiceManager.addService(Context.ACCESSIBILITY_SERVICE,
    //                     new AccessibilityManagerService(context));
    //         } catch (Throwable e) {
    //             ReportWtf("starting Accessibility Manager", ec);
    //         }
    //     }
    // }

    // try {
    //     wm.displayReady();
    // } catch (Throwable e) {
    //     ReportWtf("making display ready", ec);
    // }

    // try {
    //     mPackageManagerService.performBootDexOpt();
    // } catch (Throwable e) {
    //     ReportWtf("performing boot dexopt", ec);
    // }

    // try {
    //     ActivityManagerNative.getDefault().showBootMessage(
    //             context.getResources().getText(
    //                     com.android.internal.R.string.android_upgrading_starting_apps),
    //             false);
    // } catch (RemoteException e) {
    // }

    // if (mFactoryTestMode != FactoryTest.FACTORY_TEST_LOW_LEVEL) {
    //     if (!disableStorage &&
    //         !"0".equals(SystemProperties.get("system_init.startmountservice"))) {
    //         try {
    //             /*
    //              * NotificationManagerService is dependant on MountService,
    //              * (for media / usb notifications) so we must start MountService first.
    //              */
    //             Slogger::I(TAG, "Mount Service");
    //             mountService = new MountService(context);
    //             ServiceManager.addService("mount", mountService);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Mount Service", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG,  "LockSettingsService");
    //             lockSettings = new LockSettingsService(context);
    //             ServiceManager.addService("lock_settings", lockSettings);
    //         } catch (Throwable e) {
    //             ReportWtf("starting LockSettingsService service", ec);
    //         }

    //         if (!SystemProperties.get(PERSISTENT_DATA_BLOCK_PROP).equals("")) {
    //             mSystemServiceManager.startService(PersistentDataBlockService.class);
    //         }

    //         // Always start the Device Policy Manager, so that the API is compatible with
    //         // API8.
    //         mSystemServiceManager.startService(DevicePolicyManagerService.Lifecycle.class);
    //     }

    //     if (!disableSystemUI) {
    //         try {
    //             Slogger::I(TAG, "Status Bar");
    //             statusBar = new StatusBarManagerService(context, wm);
    //             ServiceManager.addService(Context.STATUS_BAR_SERVICE, statusBar);
    //         } catch (Throwable e) {
    //             ReportWtf("starting StatusBarManagerService", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "Clipboard Service");
    //             ServiceManager.addService(Context.CLIPBOARD_SERVICE,
    //                     new ClipboardService(context));
    //         } catch (Throwable e) {
    //             ReportWtf("starting Clipboard Service", ec);
    //         }
    //     }

    //     if (!disableNetwork) {
    //         try {
    //             Slogger::I(TAG, "NetworkManagement Service");
    //             networkManagement = NetworkManagementService.create(context);
    //             ServiceManager.addService(Context.NETWORKMANAGEMENT_SERVICE, networkManagement);
    //         } catch (Throwable e) {
    //             ReportWtf("starting NetworkManagement Service", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "Text Service Manager Service");
    //             tsms = new TextServicesManagerService(context);
    //             ServiceManager.addService(Context.TEXT_SERVICES_MANAGER_SERVICE, tsms);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Text Service Manager Service", ec);
    //         }
    //     }

    //     if (!disableNetwork) {
    //         try {
    //             Slogger::I(TAG, "Network Score Service");
    //             networkScore = new NetworkScoreService(context);
    //             ServiceManager.addService(Context.NETWORK_SCORE_SERVICE, networkScore);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Network Score Service", ec);
    //         }

    //         try {
    //             Slogger::I(TAG, "NetworkStats Service");
    //             networkStats = new NetworkStatsService(context, networkManagement, alarm);
    //             ServiceManager.addService(Context.NETWORK_STATS_SERVICE, networkStats);
    //         } catch (Throwable e) {
    //             ReportWtf("starting NetworkStats Service", ec);
    //         }

    //         try {
    //             Slogger::I(TAG, "NetworkPolicy Service");
    //             networkPolicy = new NetworkPolicyManagerService(
    //                     context, mActivityManagerService,
    //                     (IPowerManager)ServiceManager.getService(Context.POWER_SERVICE),
    //                     networkStats, networkManagement);
    //             ServiceManager.addService(Context.NETWORK_POLICY_SERVICE, networkPolicy);
    //         } catch (Throwable e) {
    //             ReportWtf("starting NetworkPolicy Service", ec);
    //         }

    //         mSystemServiceManager.startService(WIFI_P2P_SERVICE_CLASS);
    //         mSystemServiceManager.startService(WIFI_SERVICE_CLASS);
    //         mSystemServiceManager.startService(
    //                     "com.android.server.wifi.WifiScanningService");

    //         mSystemServiceManager.startService("com.android.server.wifi.RttService");

    //         if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_ETHERNET)) {
    //             mSystemServiceManager.startService(ETHERNET_SERVICE_CLASS);
    //         }

    //         try {
    //             Slogger::I(TAG, "Connectivity Service");
    //             connectivity = new ConnectivityService(
    //                     context, networkManagement, networkStats, networkPolicy);
    //             ServiceManager.addService(Context.CONNECTIVITY_SERVICE, connectivity);
    //             networkStats.bindConnectivityManager(connectivity);
    //             networkPolicy.bindConnectivityManager(connectivity);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Connectivity Service", ec);
    //         }

    //         try {
    //             Slogger::I(TAG, "Network Service Discovery Service");
    //             serviceDiscovery = NsdService.create(context);
    //             ServiceManager.addService(
    //                     Context.NSD_SERVICE, serviceDiscovery);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Service Discovery Service", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "UpdateLock Service");
    //             ServiceManager.addService(Context.UPDATE_LOCK_SERVICE,
    //                     new UpdateLockService(context));
    //         } catch (Throwable e) {
    //             ReportWtf("starting UpdateLockService", ec);
    //         }
    //     }

    //     /*
    //      * MountService has a few dependencies: Notification Manager and
    //      * AppWidget Provider. Make sure MountService is completely started
    //      * first before continuing.
    //      */
    //     if (mountService != NULL && !mOnlyCore) {
    //         mountService.waitForAsecScan();
    //     }

    //     try {
    //         if (accountManager != NULL)
    //             accountManager->SystemReady();
    //     } catch (Throwable e) {
    //         ReportWtf("making Account Manager Service ready", ec);
    //     }

    //     try {
    //         if (contentService != NULL)
    //             contentService->SystemReady();
    //     } catch (Throwable e) {
    //         ReportWtf("making Content Service ready", ec);
    //     }

    //     mSystemServiceManager.startService(NotificationManagerService.class);
    //     notification = INotificationManager.Stub.asInterface(
    //             ServiceManager.getService(Context.NOTIFICATION_SERVICE));
    //     networkPolicy.bindNotificationManager(notification);

    //     mSystemServiceManager.startService(DeviceStorageMonitorService.class);

    //     if (!disableLocation) {
    //         try {
    //             Slogger::I(TAG, "Location Manager");
    //             location = new LocationManagerService(context);
    //             ServiceManager.addService(Context.LOCATION_SERVICE, location);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Location Manager", ec);
    //         }

    //         try {
    //             Slogger::I(TAG, "Country Detector");
    //             countryDetector = new CountryDetectorService(context);
    //             ServiceManager.addService(Context.COUNTRY_DETECTOR, countryDetector);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Country Detector", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "Search Service");
    //             ServiceManager.addService(Context.SEARCH_SERVICE,
    //                     new SearchManagerService(context));
    //         } catch (Throwable e) {
    //             ReportWtf("starting Search Service", ec);
    //         }
    //     }

    //     try {
    //         Slogger::I(TAG, "DropBox Service");
    //         ServiceManager.addService(Context.DROPBOX_SERVICE,
    //                 new DropBoxManagerService(context, new File("/data/system/dropbox")));
    //     } catch (Throwable e) {
    //         ReportWtf("starting DropBoxManagerService", ec);
    //     }

    //     if (!disableNonCoreServices && context.getResources().getBoolean(
    //                 R.bool.config_enableWallpaperService)) {
    //         try {
    //             Slogger::I(TAG, "Wallpaper Service");
    //             wallpaper = new WallpaperManagerService(context);
    //             ServiceManager.addService(Context.WALLPAPER_SERVICE, wallpaper);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Wallpaper Service", ec);
    //         }
    //     }

    //     if (!disableMedia && !"0".equals(SystemProperties.get("system_init.startaudioservice"))) {
    //         try {
    //             Slogger::I(TAG, "Audio Service");
    //             audioService = new AudioService(context);
    //             ServiceManager.addService(Context.AUDIO_SERVICE, audioService);
    //         } catch (Throwable e) {
    //             ReportWtf("starting Audio Service", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         mSystemServiceManager.startService(DockObserver.class);
    //     }

    //     if (!disableMedia) {
    //         try {
    //             Slogger::I(TAG, "Wired Accessory Manager");
    //             // Listen for wired headset changes
    //             inputManager.setWiredAccessoryCallbacks(
    //                     new WiredAccessoryManager(context, inputManager));
    //         } catch (Throwable e) {
    //             ReportWtf("starting WiredAccessoryManager", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_USB_HOST)
    //                 || mPackageManager.hasSystemFeature(
    //                         PackageManager.FEATURE_USB_ACCESSORY)) {
    //             // Manage USB host and device support
    //             mSystemServiceManager.startService(USB_SERVICE_CLASS);
    //         }

    //         try {
    //             Slogger::I(TAG, "Serial Service");
    //             // Serial port support
    //             serial = new SerialService(context);
    //             ServiceManager.addService(Context.SERIAL_SERVICE, serial);
    //         } catch (Throwable e) {
    //             Slog.e(TAG, "Failure starting SerialService", ec);
    //         }
    //     }

    //     mSystemServiceManager.startService(TwilightService.class);

    //     mSystemServiceManager.startService(UiModeManagerService.class);

    //     mSystemServiceManager.startService(JobSchedulerService.class);

    //     if (!disableNonCoreServices) {
    //         if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_BACKUP)) {
    //             mSystemServiceManager.startService(BACKUP_MANAGER_SERVICE_CLASS);
    //         }

    //         if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_APP_WIDGETS)) {
    //             mSystemServiceManager.startService(APPWIDGET_SERVICE_CLASS);
    //         }

    //         if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_VOICE_RECOGNIZERS)) {
    //             mSystemServiceManager.startService(VOICE_RECOGNITION_MANAGER_SERVICE_CLASS);
    //         }
    //     }

    //     try {
    //         Slogger::I(TAG, "DiskStats Service");
    //         ServiceManager.addService("diskstats", new DiskStatsService(context));
    //     } catch (Throwable e) {
    //         ReportWtf("starting DiskStats Service", ec);
    //     }

    //     try {
    //         // need to add this service even if SamplingProfilerIntegration.isEnabled()
    //         // is false, because it is this service that detects system property change and
    //         // turns on SamplingProfilerIntegration. Plus, when sampling profiler doesn't work,
    //         // there is little overhead for running this service.
    //         Slogger::I(TAG, "SamplingProfiler Service");
    //         ServiceManager.addService("samplingprofiler",
    //                     new SamplingProfilerService(context));
    //     } catch (Throwable e) {
    //         ReportWtf("starting SamplingProfiler Service", ec);
    //     }

    //     if (!disableNetwork) {
    //         try {
    //             Slogger::I(TAG, "NetworkTimeUpdateService");
    //             networkTimeUpdater = new NetworkTimeUpdateService(context);
    //         } catch (Throwable e) {
    //             ReportWtf("starting NetworkTimeUpdate service", ec);
    //         }
    //     }

    //     if (!disableMedia) {
    //         try {
    //             Slogger::I(TAG, "CommonTimeManagementService");
    //             commonTimeMgmtService = new CommonTimeManagementService(context);
    //             ServiceManager.addService("commontime_management", commonTimeMgmtService);
    //         } catch (Throwable e) {
    //             ReportWtf("starting CommonTimeManagementService service", ec);
    //         }
    //     }

    //     if (!disableNetwork) {
    //         try {
    //             Slogger::I(TAG, "CertBlacklister");
    //             CertBlacklister blacklister = new CertBlacklister(context);
    //         } catch (Throwable e) {
    //             ReportWtf("starting CertBlacklister", ec);
    //         }
    //     }

    //     if (!disableNonCoreServices) {
    //         // Dreams (interactive idle-time views, a/k/a screen savers, and doze mode)
    //         mSystemServiceManager.startService(DreamManagerService.class);
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "Assets Atlas Service");
    //             atlas = new AssetAtlasService(context);
    //             ServiceManager.addService(AssetAtlasService.ASSET_ATLAS_SERVICE, atlas);
    //         } catch (Throwable e) {
    //             ReportWtf("starting AssetAtlasService", ec);
    //         }
    //     }

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_PRINTING)) {
    //         mSystemServiceManager.startService(PRINT_MANAGER_SERVICE_CLASS);
    //     }

    //     mSystemServiceManager.startService(RestrictionsManagerService.class);

    //     mSystemServiceManager.startService(MediaSessionService.class);

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_HDMI_CEC)) {
    //         mSystemServiceManager.startService(HdmiControlService.class);
    //     }

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_LIVE_TV)) {
    //         mSystemServiceManager.startService(TvInputManagerService.class);
    //     }

    //     if (!disableNonCoreServices) {
    //         try {
    //             Slogger::I(TAG, "Media Router Service");
    //             mediaRouter = new MediaRouterService(context);
    //             ServiceManager.addService(Context.MEDIA_ROUTER_SERVICE, mediaRouter);
    //         } catch (Throwable e) {
    //             ReportWtf("starting MediaRouterService", ec);
    //         }

    //         mSystemServiceManager.startService(TrustManagerService.class);

    //         mSystemServiceManager.startService(FingerprintService.class);

    //         try {
    //             Slogger::I(TAG, "BackgroundDexOptService");
    //             BackgroundDexOptService.schedule(context);
    //         } catch (Throwable e) {
    //             ReportWtf("starting BackgroundDexOptService", ec);
    //         }

    //     }

    //     mSystemServiceManager.startService(LauncherAppsService.class);
    // }

    // if (!disableNonCoreServices) {
    //     mSystemServiceManager.startService(MediaProjectionManagerService.class);
    // }

    // Before things start rolling, be sure we have decided whether
    // we are in safe mode.
    Boolean safeMode = FALSE;
    // wm->DetectSafeMode(&safeMode);
    // if (safeMode) {
    //     mActivityManagerService.enterSafeMode();
    //     // Disable the JIT for the system_server process
    //     VMRuntime.getRuntime().disableJitCompilation();
    // } else {
    //     // Enable the JIT for the system_server process
    //     VMRuntime.getRuntime().startJitCompilation();
    // }

    // // MMS service broker
    // mmsService = mSystemServiceManager.startService(MmsServiceBroker.class);

    // // It is now time to start up the app processes...

    // try {
    //     vibrator->SystemReady();
    // } catch (Throwable e) {
    //     ReportWtf("making Vibrator Service ready", ec);
    // }

    // if (lockSettings != NULL) {
    //     try {
    //         lockSettings->SystemReady();
    //     } catch (Throwable e) {
    //         ReportWtf("making Lock Settings Service ready", ec);
    //     }
    // }

    // // Needed by DevicePolicyManager for initialization
    // mSystemServiceManager->StartBootPhase(ISystemService::PHASE_LOCK_SETTINGS_READY);

    // mSystemServiceManager->StartBootPhase(ISystemService::PHASE_SYSTEM_SERVICES_READY);

    // try {
    //     wm->SystemReady();
    // } catch (Throwable e) {
    //     ReportWtf("making Window Manager Service ready", ec);
    // }

    // if (safeMode) {
    //     mActivityManagerService.showSafeModeOverlay();
    // }

    // // Update the configuration for this context by hand, because we're going
    // // to start using it before the config change done in wm->SystemReady() will
    // // propagate to it.
    // Configuration config = wm.computeNewConfiguration();
    // DisplayMetrics metrics = new DisplayMetrics();
    // WindowManager w = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
    // w.getDefaultDisplay().getMetrics(metrics);
    // context.getResources().updateConfiguration(config, metrics);

    // try {
    //     // TODO: use boot phase
    //     mPowerManagerService.systemReady(mActivityManagerService.getAppOpsService());
    // } catch (Throwable e) {
    //     ReportWtf("making Power Manager Service ready", ec);
    // }

    // try {
    //     mPackageManagerService->SystemReady();
    // } catch (Throwable e) {
    //     ReportWtf("making Package Manager Service ready", ec);
    // }

    // TODO: use boot phase and communicate these flags some other way
    ec = mDisplayManagerService->SystemReady(safeMode, mOnlyCore);
    if (FAILED(ec)) {
        ReportWtf("making Display Manager Service ready", ec);
    }

    // These are needed to propagate to the runnable below.
    AutoPtr<ServiceBundle> bundle = new ServiceBundle();
    // bundle->mMountServiceF = mountService;
    // bundle->mNetworkManagementF = networkManagement;
    // bundle->mNetworkStatsF = networkStats;
    // bundle->mNetworkPolicyF = networkPolicy;
    // bundle->mConnectivityF = connectivity;
    // bundle->mNetworkScoreF = networkScore;
    // bundle->mWallpaperF = wallpaper;
    // bundle->mImmF = imm;
    // bundle->mLocationF = location;
    // bundle->mCountryDetectorF = countryDetector;
    // bundle->mNetworkTimeUpdaterF = networkTimeUpdater;
    // bundle->mCommonTimeMgmtServiceF = commonTimeMgmtService;
    // bundle->mTextServiceManagerServiceF = tsms;
    // bundle->mStatusBarF = statusBar;
    // bundle->mAtlasF = atlas;
    // bundle->mInputManagerF = inputManager;
    // bundle->mTelephonyRegistryF = telephonyRegistry;
    // bundle->mMediaRouterF = mediaRouter;
    // bundle->mAudioServiceF = audioService;
    // bundle->mMmsServiceF = mmsService;

    // We now tell the activity manager it is okay to run third party
    // code.  It will call back into us once it has gotten to the state
    // where third party code can really run (but before it has actually
    // started launching the initial applications), for us to complete our
    // initialization.
    AutoPtr<IRunnable> runnable = new SystemReadyRunnable(this, bundle);
    // mActivityManagerService->SystemReady(runnable);
    return NOERROR;
}

ECode SystemServer::StartSystemUi(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    AutoPtr<IComponentName> name;
    CComponentName::New(
        String("com.android.systemui"),
        String("com.android.systemui.SystemUIService"),
        (IComponentName**)&name);
    intent->SetComponent(name);
    //Slog.d(TAG, "Starting service: " + intent);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> owner;
    helper->GetOWNER((IUserHandle**)&owner);
    AutoPtr<IComponentName> result;
    context->StartServiceAsUser(intent, owner, (IComponentName**)&result);
    return NOERROR;
}

//===============================================================================
// SystemServer::SystemReadyRunnable
//===============================================================================
SystemServer::SystemReadyRunnable::SystemReadyRunnable(
    /* [in] */ SystemServer* host,
    /* [in] */ SystemServer::ServiceBundle* bundle)
    : mHost(host)
    , mServiceBundle(bundle)
{
}

ECode SystemServer::SystemReadyRunnable::Run()
{
    Slogger::I(TAG, "Making services ready");
    assert(0 && "TODO");

    mHost->mSystemServiceManager->StartBootPhase(
        ISystemService::PHASE_ACTIVITY_MANAGER_READY);

    ECode ec = NOERROR;

    // ec = mActivityManagerService->StartObservingNativeCrashes();
    // if (FAILED(ec)) {
    //     mHost->ReportWtf("observing native crashes", ec);
    // }

    // Slogger::I(SystemServer::TAG, "WebViewFactory preparation");
    AutoPtr<IWebViewFactory> webViewFactory;
    // CWebViewFactory::AcquireSingleton((IWebViewFactory**)&webViewFactory);
    webViewFactory->PrepareWebViewInSystemServer();

    ec = StartSystemUi(mHost->mSystemContext);
    if (FAILED(ec)) {
        mHost->ReportWtf("starting System UI", ec);
    }

    // if (mServiceBundle->mMountServiceF != NULL) {
    //     ec = mServiceBundle->mMountServiceF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Mount Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mNetworkScoreF != NULL) {
    //     ec = mServiceBundle->mNetworkScoreF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Network Score Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mNetworkManagementF != NULL) {
    //     ec = mServiceBundle->mNetworkManagementF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Network Managment Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mNetworkStatsF != NULL) {
    //     ec = mServiceBundle->mNetworkStatsF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Network Stats Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mNetworkPolicyF != NULL) {
    //     ec = mServiceBundle->mNetworkPolicyF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Network Policy Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mConnectivityF != NULL) {
    //     ec = mServiceBundle->mConnectivityF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making Connectivity Service ready", ec);
    //     }
    // }

    // if (mServiceBundle->mAudioServiceF != NULL) {
    //     ec = mServiceBundle->mAudioServiceF->SystemReady();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("making AudioService ready", ec);
    //     }
    // }

    // // Watchdog.getInstance().start();

    // // It is now okay to let the various system services start their
    // // third party code...
    // mSystemServiceManager->StartBootPhase(
    //     ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START);

    // if (mServiceBundle->mWallpaperF != NULL) {
    //     ec = mServiceBundle->mWallpaperF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying WallpaperService running", ec);
    //     }
    // }

    // if (mServiceBundle->mImmF != NULL) {
    //     ec = mServiceBundle->mImmF->SystemRunning(mServiceBundle->mStatusBarF);
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying InputMethodService running", ec);
    //     }
    // }

    // if (mServiceBundle->mLocationF != NULL) {
    //     ec = mServiceBundle->mLocationF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying Location Service running", ec);
    //     }
    // }

    // if (mServiceBundle->mCountryDetectorF != NULL) {
    //     ec = mServiceBundle->mCountryDetectorF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying CountryDetectorService running", ec);
    //     }
    // }

    // if (mServiceBundle->mNetworkTimeUpdaterF != NULL) {
    //     ec = mServiceBundle->mNetworkTimeUpdaterF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying NetworkTimeService running", ec);
    //     }
    // }

    // if (mServiceBundle->mCommonTimeMgmtServiceF != NULL) {
    //     ec = mServiceBundle->mCommonTimeMgmtServiceF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying CommonTimeManagementService running", ec);
    //     }
    // }

    // if (mServiceBundle->mTextServiceManagerServiceF != NULL) {
    //     ec = mServiceBundle->mTextServiceManagerServiceF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying TextServicesManagerService running", ec);
    //     }
    // }

    // if (mServiceBundle->mAtlasF != NULL) {
    //     ec = mServiceBundle->mAtlasF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying AssetAtlasService running", ec);
    //     }
    // }

    // if (mServiceBundle->mInputManagerF != NULL) {
    //     // TODO(BT) Pass parameter to input manager
    //     ec = mServiceBundle->mInputManagerF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying InputManagerService running", ec);
    //     }
    // }

    // if (mServiceBundle->mTelephonyRegistryF != NULL) {
    //     ec = mServiceBundle->mTelephonyRegistryF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying TelephonyRegistry running", ec);
    //     }
    // }

    // if (mServiceBundle->mMediaRouterF != NULL) {
    //     ec = mServiceBundle->mMediaRouterF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying MediaRouterService running", ec);
    //     }
    // }

    // if (mServiceBundle->mMmsServiceF != NULL) {
    //     ec = mServiceBundle->mMmsServiceF->SystemRunning();
    //     if (FAILED(ec)) {
    //         mHost->ReportWtf("Notifying MmsService running", ec);
    //     }
    // }

    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
