
#include "elastos/droid/server/CSystemServer.h"
#include "elastos/droid/server/SystemServiceManager.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/server/AlarmManagerService.h"
#include "elastos/droid/server/AttributeCache.h"
#include "elastos/droid/server/Watchdog.h"
#include "elastos/droid/server/SystemConfig.h"
#include "elastos/droid/server/CConsumerIrService.h"
#include "elastos/droid/server/devicepolicy/CDevicePolicyManagerService.h"
#include "elastos/droid/server/CEntropyMixer.h"
#include "elastos/droid/server/CInputMethodManagerService.h"
#include "elastos/droid/server/CLockSettingsService.h"
#include "elastos/droid/server/CPersistentDataBlockService.h"
#include "elastos/droid/server/CTelephonyRegistry.h"
#include "elastos/droid/server/CTorchService.h"
#include "elastos/droid/server/CVibratorService.h"
#include "elastos/droid/server/CUiModeManagerService.h"
#include "elastos/droid/server/CNsdService.h"
#include "elastos/droid/server/CBatteryService.h"
#include "elastos/droid/server/CCmHardwareService.h"
#include "elastos/droid/server/CProfileManagerService.h"
#include "elastos/droid/server/accessibility/CAccessibilityManagerService.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"
#include "elastos/droid/server/appwidget/CAppWidgetService.h"
#include "elastos/droid/server/clipboard/CClipboardService.h"
#include "elastos/droid/server/content/CContentService.h"
#include "elastos/droid/server/lights/LightsService.h"
#include "elastos/droid/server/notification/CNotificationManagerService.h"
#include "elastos/droid/server/os/CSchedulingPolicyService.h"
#include "elastos/droid/server/pm/CLauncherAppsService.h"
#include "elastos/droid/server/power/ShutdownThread.h"
#include "elastos/droid/server/twilight/CTwilightService.h"
#include "elastos/droid/server/search/CSearchManagerService.h"
#include "elastos/droid/server/WiredAccessoryManager.h"
#include "elastos/droid/server/webkit/WebViewUpdateService.h"
#include "elastos/droid/server/usb/CUsbService.h"
#include "elastos/droid/server/wm/InputMonitor.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/dreams/CDreamManagerService.h"
#include "elastos/droid/server/CBluetoothManagerService.h"
#include "elastos/droid/server/ethernet/EthernetService.h"

#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Webkit.h>
#include <Elastos.Droid.Accounts.h>
#include <elastos/droid/R.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/os/FactoryTest.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/internal/os/BinderInternal.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/utility/logging/Slogger.h>

#include <SensorService.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <utils/misc.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::FactoryTest;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IISchedulingPolicyService;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Internal::Os::BinderInternal;
using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;
using Elastos::Droid::App::IContextImpl;
using Elastos::Droid::App::IIAlarmManager;
using Elastos::Droid::App::IIProfileManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IIBluetoothManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIClipboard;
using Elastos::Droid::Content::Res::IThemeConfig;
using Elastos::Droid::Content::Res::IThemeConfigHelper;
using Elastos::Droid::Content::Res::CThemeConfigHelper;
using Elastos::Droid::Media::CAudioService;
using Elastos::Droid::Media::IAudioService;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IIAssetAtlas;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::Accounts::IIAccountManager;
using Elastos::Droid::Hardware::IITorchService;
using Elastos::Droid::Hardware::IICmHardwareService;
using Elastos::Droid::Hardware::Input::IIInputManager;
using Elastos::Droid::Webkit::IWebViewFactory;
using Elastos::Droid::Webkit::CWebViewFactory;
using Elastos::Droid::Utility::CDisplayMetrics;

using Elastos::Droid::Server::Accessibility::CAccessibilityManagerService;
using Elastos::Droid::Server::Accounts::CAccountManagerService;
using Elastos::Droid::Server::AppWidget::CAppWidgetService;
using Elastos::Droid::Server::Clipboard::CClipboardService;
using Elastos::Droid::Server::Content::CContentService;
using Elastos::Droid::Server::Lights::LightsService;
using Elastos::Droid::Server::Notification::CNotificationManagerService;
using Elastos::Droid::Server::Os::CSchedulingPolicyService;
using Elastos::Droid::Server::Pm::CLauncherAppsService;
using Elastos::Droid::Server::Power::ShutdownThread;
using Elastos::Droid::Server::Search::CSearchManagerService;
using Elastos::Droid::Server::Twilight::CTwilightService;
using Elastos::Droid::Server::Usb::CUsbService;
using Elastos::Droid::Server::Webkit::WebViewUpdateService;
using Elastos::Droid::Server::Wm::InputMonitor;
using Elastos::Droid::Server::DevicePolicy::CDevicePolicyManagerService;
using Elastos::Droid::Server::Dreams::CDreamManagerService;
using Elastos::Droid::Server::CBluetoothManagerService;
using Elastos::Droid::Server::Ethernet::EthernetService;

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String SystemServer::TAG("CSystemServer");

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
const String SystemServer::WIFI_SERVICE_CLASS("Elastos.Droid.Server.Wifi.CWifiService");
const String SystemServer::WIFI_P2P_SERVICE_CLASS("Elastos.Droid.Server.Wifi.P2p.CWifiP2pService");
    //"com.android.server.wifi.p2p.WifiP2pService";
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
    Slogger::I("CSystemServer", "CSystemServer::Main");
    for (Int32 i = 0; i < args.GetLength(); ++i) {
        Slogger::I("CSystemServer", " >> arg %d: %s", i, args[i].string());
    }
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
    Environment::SetUserRequired(TRUE);

    // Ensure binder calls into the system always run at foreground priority.
    BinderInternal::DisableBackgroundScheduling(TRUE);

    // Prepare the main looper thread (this thread).
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_FOREGROUND);
    Process::SetCanSelfBackground(false);
    ec = Looper::PrepareMainLooper();
    if (FAILED(ec)) ReportWtf("failed to PrepareMainLooper", ec);

    // Initialize native services.
    // System.loadLibrary("android_servers");
    NativeInit();

    // Check whether we failed to shut down last time we tried.
    // This call may not return.
    PerformPendingShutdown();

    Slogger::I(TAG, "Initialize the system context.");
    // Initialize the system context.
    CreateSystemContext();

    Slogger::I(TAG, "Create the system service manager.");
    // Create the system service manager.
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

    Slogger::I(TAG, ">>>>> System Server Loop forever <<<<<");

    // Loop forever.
    Looper::Loop();

    Slogger::E(TAG, "Main thread loop unexpectedly exited");
    return E_RUNTIME_EXCEPTION;

_EXIT_:
    Slogger::E(TAG, "****************************************************************");
    Slogger::E(TAG, "************ Failure starting system services, error code: %08x", ec);
    Slogger::E(TAG, "****************************************************************");
    return ec;
}

void SystemServer::ReportWtf(
    /* [in] */ const char* msg,
    /* [in] */ ECode ec)
{
    Slogger::W(TAG, "***********************************************");
    Slogger::W(TAG, "BOOT FAILURE %s, error code: %08x", msg, ec);
    Slogger::W(TAG, "***********************************************");
    assert(0);
}

ECode SystemServer::PerformPendingShutdown()
{
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    String shutdownAction;
    systemProperties->Get(ShutdownThread::SHUTDOWN_ACTION_PROPERTY, String(""), &shutdownAction);
    if (!shutdownAction.IsNullOrEmpty()) {
        Boolean reboot = (shutdownAction.GetChar(0) == '1');

        String reason;
        if (shutdownAction.GetLength() > 1) {
            reason = shutdownAction.Substring(1, shutdownAction.GetLength());
        }

        ShutdownThread::RebootOrShutdown(reboot, reason);
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
    Slogger::I(TAG, " === StartBootstrapServices ===");
    AutoPtr<ISystemService> service;

    // Wait for installd to finish starting up so that it has a chance to
    // create critical directories such as /data/user with the appropriate
    // permissions.  We need this to complete before we initialize other services.
    mInstaller = new Installer(mSystemContext);
    mSystemServiceManager->StartService(mInstaller.Get());

    Slogger::I(TAG, "Activity manager");
    // Activity manager runs the show.
    AutoPtr<CActivityManagerService::Lifecycle> am = new CActivityManagerService::Lifecycle();
    ECode ec = am->constructor(mSystemContext);
    if (FAILED(ec)) ReportWtf("create Activity manager service.", ec);
    ec = mSystemServiceManager->StartService(am.Get());
    if (FAILED(ec)) ReportWtf("starting Activity manager service.", ec);
    mActivityManagerService = am->GetService();
    mActivityManagerService->SetSystemServiceManager(mSystemServiceManager);

    Slogger::I(TAG, "Power manager");
    // Power manager needs to be started early because other services need it.
    // Native daemons may be watching for it to be registered so it must be ready
    // to handle incoming binder calls immediately (including being able to verify
    // the permissions for those calls).
    service = NULL;
    mPowerManagerService = new PowerManagerService();
    ec = mPowerManagerService->constructor(mSystemContext);
    if (FAILED(ec)) ReportWtf("starting Power manager service.", ec);
    mSystemServiceManager->StartService(mPowerManagerService.Get());

    // Now that the power manager has been started, let the activity manager
    // initialize power management features.
    mActivityManagerService->InitPowerManagement();

    Slogger::I(TAG, "Display manager");
    // Display manager is needed to provide display metrics before package manager
    // starts up.
    service = NULL;
    ec = CDisplayManagerService::New(mSystemContext, (ISystemService**)&service);
    if (FAILED(ec)) ReportWtf("starting Display manager service.", ec);
    mSystemServiceManager->StartService(service);
    mDisplayManagerService = (CDisplayManagerService*)service.Get();

    // We need the default display before we can initialize the package manager.
    mSystemServiceManager->StartBootPhase(ISystemService::PHASE_WAIT_FOR_DEFAULT_DISPLAY);

    // Only run "core" apps if we're encrypting the device.
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    String cryptState;
    systemProperties->Get(String("vold.decrypt"), &cryptState);
    if (ENCRYPTING_STATE.Equals(cryptState)) {
        Slogger::W(TAG, "Detected encryption in progress - only parsing core apps");
        mOnlyCore = TRUE;
    }
    else if (ENCRYPTED_STATE.Equals(cryptState)) {
        Slogger::W(TAG, "Device encrypted - only parsing core apps");
        mOnlyCore = TRUE;
    }

    Slogger::I(TAG, "Package manager");
    // Start the package manager.
    AutoPtr<IIPackageManager> pm = CPackageManagerService::Main(mSystemContext, mInstaller,
            mFactoryTestMode != FactoryTest::FACTORY_TEST_OFF, mOnlyCore);
    mPackageManagerService = (CPackageManagerService*)pm.Get();
    mPackageManagerService->IsFirstBoot(&mFirstBoot);
    mSystemContext->GetPackageManager((IPackageManager**)&mPackageManager);

    Slogger::I(TAG, "User Service");
    AutoPtr<IInterface> svrObj = (IIUserManager*)CUserManagerService::GetInstance().Get();
    ServiceManager::AddService(IContext::USER_SERVICE, svrObj.Get());

    // Initialize attribute cache used to cache resources from packages.
    AttributeCache::Init(mSystemContext);

    Slogger::I(TAG, "set up the Application instance for the system process and get started.");
    // Set up the Application instance for the system process and get started.
    ec = mActivityManagerService->SetSystemProcess();
    if (FAILED(ec)) ReportWtf("starting the Application instance for the system process.", ec);
    return ec;
}

ECode SystemServer::StartCoreServices()
{
    Slogger::I(TAG, " === StartCoreServices ===");
    // Manages LEDs and display backlight.
    Slogger::I(TAG, "Lights Service");
    AutoPtr<LightsService> ligthService = new LightsService();
    ligthService->constructor(mSystemContext);
    mSystemServiceManager->StartService(ligthService.Get());

    // Tracks the battery level.  Requires LightService.
    Slogger::I(TAG, "Battery Service");
    AutoPtr<ISystemService> service;
    CBatteryService::New(mSystemContext, (ISystemService**)&service);
    mSystemServiceManager->StartService(service);

    // // Tracks application usage stats.
    Slogger::I(TAG, "Usage Stats Service todo");
    // service = NULL;
    // CUsageStatsService::New(mSystemContext, (ISystemService**)&service);
    // mSystemServiceManager->tartService(service.Get());
    // AutoPtr<IInterface> usm = LocalServices::GetService(EIID_IUsageStatsManagerInternal);
    // mActivityManagerService->SetUsageStatsManager(IUsageStatsManagerInternal::Probe(usm));

    // Tracks whether the updatable WebView is in a ready state and watches for update installs.
    Slogger::I(TAG, "WebView Update Service");
    AutoPtr<WebViewUpdateService> wvus = new WebViewUpdateService();
    wvus->constructor(mSystemContext);
    mSystemServiceManager->StartService(wvus.Get());
    return NOERROR;
}

ECode SystemServer::StartOtherServices()
{
    Slogger::I(TAG, " === StartOtherServices ===");
    ECode ec = NOERROR;
    Boolean bval;
    AutoPtr<IContext> context = mSystemContext;
    AutoPtr<CAccountManagerService> accountManager;
    AutoPtr<CContentService> contentService;
    AutoPtr<CVibratorService> vibrator;
    AutoPtr<IIAlarmManager> alarm;
    AutoPtr<CMountService> mountService;
    AutoPtr<CNetworkManagementService> networkManagement;
    AutoPtr<CNetworkStatsService> networkStats;
    AutoPtr<CNetworkPolicyManagerService> networkPolicy;
    AutoPtr<CConnectivityService> connectivity;
    AutoPtr<CNetworkScoreService> networkScore;
    AutoPtr<CNsdService> serviceDiscovery;
    AutoPtr<CWindowManagerService> wm;
    AutoPtr<IIBluetoothManager> bluetooth; //AutoPtr<CBluetoothManagerService> ;
    // AutoPtr<CUsbService> usb;
    // AutoPtr<CSerialService> serial;
    AutoPtr<NetworkTimeUpdateService> networkTimeUpdater;
    // AutoPtr<CCommonTimeManagementService> commonTimeMgmtService;
    AutoPtr<CInputManagerService> inputManager;
    // AutoPtr<CTelephonyRegistry> telephonyRegistry;
    AutoPtr<CConsumerIrService> consumerIr;
    AutoPtr<IAudioService> audioService;
    // AutoPtr<CMmsServiceBroker> mmsService;
    AutoPtr<IIProfileManager> profile;

    AutoPtr<CStatusBarManagerService> statusBar;
    AutoPtr<IINotificationManager> notification;
    AutoPtr<CInputMethodManagerService> imm;
    AutoPtr<CWallpaperManagerService> wallpaper;
    AutoPtr<CLocationManagerService> location;
    AutoPtr<CCountryDetectorService> countryDetector;
    AutoPtr<CTextServicesManagerService> tsms;
    AutoPtr<CLockSettingsService> lockSettings;
    AutoPtr<IIAssetAtlas> atlas;
    AutoPtr<CMediaRouterService> mediaRouter;
    // EdgeGestureService edgeGestureService = null;
    // GestureService gestureService = null;
    AutoPtr<IIThemeService> themeService;
    // KillSwitchService killSwitchService = null;

    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    AutoPtr<IInterface> service;
    AutoPtr<ISystemService> systemService;

    Boolean disableStorage, disableMedia, disableBluetooth, disableTelephony;
    Boolean disableLocation, disableSystemUI, disableNonCoreServices, disableNetwork, disableAtlas;
    systemProperties->GetBoolean(String("config.disable_storage"), FALSE, &disableStorage);
    systemProperties->GetBoolean(String("config.disable_media"), FALSE, &disableMedia);
    systemProperties->GetBoolean(String("config.disable_bluetooth"), FALSE, &disableBluetooth);
    systemProperties->GetBoolean(String("config.disable_telephony"), FALSE, &disableTelephony);
    systemProperties->GetBoolean(String("config.disable_location"), FALSE, &disableLocation);
    systemProperties->GetBoolean(String("config.disable_systemui"), FALSE, &disableSystemUI);
    systemProperties->GetBoolean(String("config.disable_noncore"), FALSE, &disableNonCoreServices);
    systemProperties->GetBoolean(String("config.disable_network"), FALSE, &disableNetwork);
    systemProperties->GetBoolean(String("config.disable_atlas"), FALSE, &disableAtlas);
    String str;
    systemProperties->Get(String("ro.kernel.qemu"), &str);
    Boolean isEmulator = str.Equals("1");

    // try {
    Slogger::I(TAG, "Reading configuration...");
    SystemConfig::GetInstance();

    Slogger::I(TAG, "Scheduling Policy");
    AutoPtr<IISchedulingPolicyService> sps;
    ec = CSchedulingPolicyService::New((IISchedulingPolicyService**)&sps);
    if (FAILED(ec)) ReportWtf("starting Scheduling Policy service", ec);
    ServiceManager::AddService(String("scheduling_policy"), sps.Get());

    Slogger::I(TAG, "Telephony Registry");
    AutoPtr<IITelephonyRegistry> telephonyRegistry;
    ec = CTelephonyRegistry::New(context, (IITelephonyRegistry**)&telephonyRegistry);
    if (FAILED(ec)) ReportWtf("starting Telephony Registry", ec);
    ServiceManager::AddService(String("telephony.registry"), telephonyRegistry.Get());

    Slogger::I(TAG, "Entropy Mixer");
    AutoPtr<IEntropyMixer> em;
    CEntropyMixer::New(context, (IEntropyMixer**)&em);
    ServiceManager::AddService(String("entropy"), em.Get());

    context->GetContentResolver((IContentResolver**)&mContentResolver);

    // The AccountManager must come before the ContentService
    // TODO: seems like this should be disable-able, but req'd by ContentService
    Slogger::I(TAG, "Account Manager");
    AutoPtr<IIAccountManager> am;
    ec = CAccountManagerService::New(context, (IIAccountManager**)&am);
    if (FAILED(ec)) ReportWtf("starting Account Manager service", ec);
    ServiceManager::AddService(IContext::ACCOUNT_SERVICE, am.Get());
    accountManager = (CAccountManagerService*)am.Get();

    Slogger::I(TAG, "Content Manager");
    AutoPtr<IIContentService> cs = CContentService::Main(context,
        mFactoryTestMode == FactoryTest::FACTORY_TEST_LOW_LEVEL);
    contentService = (CContentService*)cs.Get();

    Slogger::I(TAG, "System Content Providers");
    ec = mActivityManagerService->InstallSystemProviders();
    if (FAILED(ec)) Slogger::E(TAG, "failed to isntall System Content Providers");

    Slogger::I(TAG, "Vibrator Service");
    AutoPtr<IIVibratorService> vs;
    ec = CVibratorService::New(context, (IIVibratorService**)&vs);
    if (FAILED(ec)) ReportWtf("starting Vibrator service", ec);
    ServiceManager::AddService(String("vibrator"), vs.Get());
    vibrator = (CVibratorService*)vs.Get();

    Slogger::I(TAG, "Consumer IR Service");
    AutoPtr<IIConsumerIrService> cirs;
    ec = CConsumerIrService::New(context, (IIConsumerIrService**)&cirs);
    // if (FAILED(ec)) ReportWtf("starting Consumer IR service", ec);
    ServiceManager::AddService(IContext::CONSUMER_IR_SERVICE, cirs.Get());
    consumerIr = (CConsumerIrService*)cirs.Get();

    Slogger::I(TAG, "Alarm Manager Service");
    AutoPtr<AlarmManagerService> alarmMgr = new AlarmManagerService();
    ec = alarmMgr->constructor(context);
    if (FAILED(ec)) ReportWtf("starting Alarm Manager service", ec);
    mSystemServiceManager->StartService(alarmMgr.Get());
    service = ServiceManager::GetService(IContext::ALARM_SERVICE);
    alarm = IIAlarmManager::Probe(service);

    Slogger::I(TAG, "Init Watchdog");
    Watchdog::GetInstance()->Init(context, mActivityManagerService);

    Slogger::I(TAG, "Input Manager");
    AutoPtr<IIInputManager> inputMgr;
    ec = CInputManagerService::New(context, (IIInputManager**)&inputMgr);
    if (FAILED(ec)) ReportWtf("starting Input Manager service", ec);
    inputManager = (CInputManagerService*)inputMgr.Get();

    Slogger::I(TAG, "Window Manager");
    wm = CWindowManagerService::Main(context, inputMgr,
        mFactoryTestMode != FactoryTest::FACTORY_TEST_LOW_LEVEL,
        !mFirstBoot, mOnlyCore);
    ServiceManager::AddService(IContext::WINDOW_SERVICE, IIWindowManager::Probe(wm));
    ServiceManager::AddService(IContext::INPUT_SERVICE, inputMgr.Get());

    mActivityManagerService->SetWindowManager(wm);

    AutoPtr<InputMonitor> inputMonitor = wm->GetInputMonitor();
    inputManager->SetWindowManagerCallbacks((IWindowManagerCallbacks*)inputMonitor.Get());
    inputManager->Start();

    // TODO: Use service dependencies instead.
    mDisplayManagerService->WindowManagerAndInputReady();

    // // Skip Bluetooth if we have an emulator kernel
    // // TODO: Use a more reliable check to see if this product should
    // // support Bluetooth - see bug 988521
    // AutoPtr<IPackageManager> pm;
    // context->GetPackageManager((IPackageManager**)&pm);
    // Boolean result;
    // if (isEmulator) {
    //     Slogger::I(TAG, "No Bluetooh Service (emulator)");
    // }
    // else if (mFactoryTestMode == FactoryTest::FACTORY_TEST_LOW_LEVEL) {
    //     Slogger::I(TAG, "No Bluetooth Service (factory test)");
    // }
    // else if (pm->HasSystemFeature(IPackageManager::FEATURE_BLUETOOTH, &result), !result) {
    //     Slogger::I(TAG, "No Bluetooth Service (Bluetooth Hardware Not Present)");
    // }
    // else if (disableBluetooth) {
    //     Slogger::I(TAG, "Bluetooth Service disabled by config");
    // }
    // else {
    //     Slogger::I(TAG, "Bluetooth Manager Service");
    //     CBluetoothManagerService::New(context, (IIBluetoothManager**)&bluetooth);
    //     ServiceManager::AddService(IBluetoothAdapter::BLUETOOTH_MANAGER_SERVICE, bluetooth);
    // }
    // } catch (RuntimeException e) {
    //     Slog.e("System", "******************************************");
    //     Slog.e("System", "************ Failure starting core service", ec);
    // }


    // Bring up services needed for UI.
    if (mFactoryTestMode != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
        //if (!disableNonCoreServices) { // TODO: View depends on these; mock them?
        if (TRUE) {
            Slogger::I(TAG, "Input Method Service");
            ec = CInputMethodManagerService::NewByFriend(context, IIWindowManager::Probe(wm),
                (CInputMethodManagerService**)&imm);
            if (FAILED(ec)) ReportWtf("starting Input Manager Service", ec);

            ServiceManager::AddService(IContext::INPUT_METHOD_SERVICE, (IIInputMethodManager*)imm.Get());

            Slogger::I(TAG, "Accessibility Manager");
            AutoPtr<IIAccessibilityManager> accessManager;
            ec = CAccessibilityManagerService::New(context, (IIAccessibilityManager**)&accessManager);
            if (FAILED(ec)) ReportWtf("starting Accessibility Service", ec);
            ServiceManager::AddService(IContext::ACCESSIBILITY_SERVICE, accessManager);
        }
    }

    ec = wm->DisplayReady();
    if (FAILED(ec)) {
        ReportWtf("making display ready", ec);
    }

    if (mFactoryTestMode != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
        if (!disableStorage &&
            (systemProperties->Get(String("system_init.startmountservice"), &str), !str.Equals("0"))) {
                /*
                 * NotificationManagerService is dependant on MountService,
                 * (for media / usb notifications) so we must start MountService first.
                 */
                Slogger::I(TAG, "Mount Service");
                ec = CMountService::NewByFriend(context, (CMountService**)&mountService);
                if (FAILED(ec)) ReportWtf("starting Mount service", ec);
                ServiceManager::AddService(String("mount"), IIMountService::Probe(mountService));
        }
    }

    ec = mPackageManagerService->PerformBootDexOpt();
    if (FAILED(ec)) ReportWtf("performing boot dexopt", ec);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> bootMsg;
    res->GetText(R::string::android_upgrading_starting_apps, (ICharSequence**)&bootMsg);
    ActivityManagerNative::GetDefault()->ShowBootMessage(bootMsg, FALSE);

    if (mFactoryTestMode != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
        if (!disableNonCoreServices) {
            Slogger::I(TAG,  "Lock Settings Service");
            ec = CLockSettingsService::NewByFriend(context, (CLockSettingsService**)&lockSettings);
            if (FAILED(ec)) ReportWtf("starting Lock settings service", ec);
            ServiceManager::AddService(String("lock_settings"), TO_IINTERFACE(lockSettings));

            systemProperties->Get(PERSISTENT_DATA_BLOCK_PROP, &str);
            if (!str.Equals("")) {
                Slogger::I(TAG,  "Persistent Data Block Service todo");
                // AutoPtr<ISystemService> pdb;
                // ec = CPersistentDataBlockService::New(context, (ISystemService**)&pdb);
                // if (FAILED(ec)) ReportWtf("starting CPersistentDataBlockService", ec);
                // mSystemServiceManager->StartService(pdb.Get());
            }

            // Always start the Device Policy Manager, so that the API is compatible with
            // API8.
            AutoPtr<CDevicePolicyManagerService::Lifecycle> dpms = new CDevicePolicyManagerService::Lifecycle();
            dpms->constructor(context);
            if (FAILED(ec)) ReportWtf("starting Policy Manager Service", ec);
            mSystemServiceManager->StartService(dpms.Get());
        }

        if (!disableSystemUI) {
            Slogger::I(TAG, "Status Bar");
            ec = CStatusBarManagerService::NewByFriend(context, IIWindowManager::Probe(wm),
                (CStatusBarManagerService**)&statusBar);
            if (FAILED(ec)) Slogger::E(TAG, "failed to start CStatusBarManagerService");
            ServiceManager::AddService(IContext::STATUS_BAR_SERVICE, TO_IINTERFACE(statusBar));
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Clipboard Service");
            AutoPtr<IIClipboard> cb;
            ec = CClipboardService::New(context, (IIClipboard**)&cb);
            if (FAILED(ec)) ReportWtf("starting Clipboard Service", ec);
            ServiceManager::AddService(IContext::CLIPBOARD_SERVICE, cb.Get());
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Torch Service");
            AutoPtr<IITorchService> service;
            ec = CTorchService::New(context, (IITorchService**)&service);
            if (FAILED(ec)) ReportWtf("starting Torch Service", ec);
            ServiceManager::AddService(IContext::TORCH_SERVICE, service.Get());
        }

        if (!disableNetwork) {
            Slogger::I(TAG, "Network manager Service");
            AutoPtr<IINetworkManagementService> nms;
            ec = CNetworkManagementService::Create(context, (IINetworkManagementService**)&nms);
            if (FAILED(ec)) ReportWtf("starting NetworkManagement Service", ec);
            networkManagement = (CNetworkManagementService*)nms.Get();
            ec = ServiceManager::AddService(IContext::NETWORKMANAGEMENT_SERVICE, nms.Get());
            if (FAILED(ec)) ReportWtf("add NetworkManagement Service", ec);
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Text Services manager Service");
            ec = CTextServicesManagerService::NewByFriend(context, (CTextServicesManagerService**)&tsms);
            if (FAILED(ec)) ReportWtf("starting Text Service Manager Service", ec);
            ec = ServiceManager::AddService(IContext::TEXT_SERVICES_MANAGER_SERVICE, TO_IINTERFACE(tsms));
            if (FAILED(ec)) ReportWtf("Add Text Service Manager Service", ec);
        }

        if (!disableNetwork) {
            Slogger::I(TAG, "Network Score Service");
            ec = CNetworkScoreService::NewByFriend(context, (CNetworkScoreService**)&networkScore);
            if (FAILED(ec)) ReportWtf("starting Network Score Service", ec);
            ec = ServiceManager::AddService(IContext::NETWORK_SCORE_SERVICE, (IINetworkScoreService*)networkScore.Get());
            if (FAILED(ec)) ReportWtf("Add Network Score Service", ec);

            Slogger::I(TAG, "Network Stats Service");
            ec = CNetworkStatsService::NewByFriend(context, networkManagement, alarm, (CNetworkStatsService**)&networkStats);
            if (FAILED(ec)) ReportWtf("starting NetworkStatsService Service", ec);
            ec = ServiceManager::AddService(IContext::NETWORK_STATS_SERVICE, (IINetworkStatsService*)networkStats.Get());
            if (FAILED(ec)) ReportWtf("Add NetworkStatsService Service", ec);

            Slogger::I(TAG, "Network Policy Manager Service");
            ec = CNetworkPolicyManagerService::NewByFriend(
                context, mActivityManagerService,
                IIPowerManager::Probe(ServiceManager::GetService(IContext::POWER_SERVICE)),
                networkStats, networkManagement,
                (CNetworkPolicyManagerService**)&networkPolicy);
            if (FAILED(ec)) ReportWtf("starting NetworkPolicy Service", ec);
            ec = ServiceManager::AddService(IContext::NETWORK_POLICY_SERVICE, (IINetworkPolicyManager*)networkPolicy.Get());
            if (FAILED(ec)) ReportWtf("Add NetworkPolicy Service", ec);


            Slogger::I(TAG, "wifiP2pService");
            AutoPtr<ISystemService> wifiP2pService;
            ec = mSystemServiceManager->StartService(WIFI_P2P_SERVICE_CLASS, (ISystemService**)&wifiP2pService);
            if (FAILED(ec)) Slogger::I(TAG, "WifiP2pService start fail");
            Slogger::I(TAG, "wifiService");
            AutoPtr<ISystemService> wifiService;
            ec = mSystemServiceManager->StartService(WIFI_SERVICE_CLASS, (ISystemService**)&wifiService);
            if (FAILED(ec)) Slogger::I(TAG, "WifiService start fail");
            Slogger::I(TAG, "wifiScanningService");
            AutoPtr<ISystemService> wifiScanningService;
            ec = mSystemServiceManager->StartService(
                String("Elastos.Droid.Server.Wifi.CWifiScanningService"), (ISystemService**)&wifiScanningService);
            if (FAILED(ec)) Slogger::I(TAG, "WifiScanningService start fail");

            Slogger::I(TAG, "RttService");
            AutoPtr<ISystemService> rttService;
            ec = mSystemServiceManager->StartService(
                String("Elastos.Droid.Server.Wifi.CRttService"), (ISystemService**)&rttService);
            if (FAILED(ec)) Slogger::I(TAG, "RttService start fail");

            bval = FALSE;
            if (mPackageManager->HasSystemFeature(IPackageManager::FEATURE_ETHERNET, &bval), bval) {
                Slogger::I(TAG, "EthernetService");
                AutoPtr<EthernetService> ehternetService = new EthernetService();
                ehternetService->constructor(context);
                mSystemServiceManager->StartService(ehternetService);
            }

            Slogger::I(TAG, "Connectivity Service");
            ec = CConnectivityService::NewByFriend(context, networkManagement, networkStats, networkPolicy,
                    (CConnectivityService**)&connectivity);
            if (FAILED(ec)) ReportWtf("starting Connectivity Service", ec);
            ec = ServiceManager::AddService(IContext::CONNECTIVITY_SERVICE, (IIConnectivityManager*)connectivity.Get());
            if (FAILED(ec)) ReportWtf("Add Connectivity Service", ec);
            networkStats->BindConnectivityManager(connectivity);
            networkPolicy->BindConnectivityManager(connectivity);

            Slogger::I(TAG, "Network Service Discovery Service");
            ec = CNsdService::Create(context, (CNsdService**)&serviceDiscovery);
            if (FAILED(ec)) ReportWtf("starting Service Discovery Service", ec);
            ec = ServiceManager::AddService(IContext::NSD_SERVICE, (IINsdManager*)serviceDiscovery.Get());
            if (FAILED(ec)) ReportWtf("Add Service Discovery Service", ec);

            // try {
            //     Slogger::I(TAG, "DPM Service");
            //     startDpmService(context);
            // } catch (Throwable e) {
            //     reportWtf("starting DpmService", e);
            // }
        }

        if (!disableNonCoreServices) {
    //         try {
                Slogger::I(TAG, "UpdateLock Service todo");
    //             ServiceManager::AddService(IContext::UPDATE_LOCK_SERVICE,
    //                     new UpdateLockService(context));
    //         } catch (Throwable e) {
    //             ReportWtf("starting UpdateLockService", ec);
    //         }
        }

        /*
         * MountService has a few dependencies: Notification Manager and
         * AppWidget Provider. Make sure MountService is completely started
         * first before continuing.
         */
        if (mountService != NULL && !mOnlyCore) {
            mountService->WaitForAsecScan();
        }

        if (accountManager != NULL) {
            accountManager->SystemReady();
            //if (FAILED(ec)) ReportWtf("making Account Manager Service ready", ec);
        }

        if (contentService != NULL) {
            ec = contentService->SystemReady();
            if (FAILED(ec)) ReportWtf("making Content Service ready", ec);
        }

        systemService = NULL;
        ec = CNotificationManagerService::New(context, (ISystemService**)&systemService);
        if (FAILED(ec)) {
            ReportWtf("making Notification Manager Service ready %d", ec);
        }
        mSystemServiceManager->StartService(systemService.Get());
        service = ServiceManager::GetService(IContext::NOTIFICATION_SERVICE);
        notification = IINotificationManager::Probe(service);
        assert(notification != NULL);
        networkPolicy->BindNotificationManager(notification);

    //     mSystemServiceManager->StartService(DeviceStorageMonitorService.class);

        if (!disableLocation) {
            Slogger::I(TAG, "Location Manager");
            if (FAILED(CLocationManagerService::NewByFriend(context, (CLocationManagerService**)&location))) {
                ReportWtf("starting Location Manager 0x%08x", ec);
            }
            else
                ServiceManager::AddService(IContext::LOCATION_SERVICE, (IBinder*)location);

            Slogger::I(TAG, "Country Detector");
            if (FAILED(CCountryDetectorService::NewByFriend(context, (CCountryDetectorService**)&countryDetector))) {
                ReportWtf("starting Country Detector 0x%08x", ec);
            }
            else
                ServiceManager::AddService(IContext::COUNTRY_DETECTOR, (IBinder*)countryDetector);
        }

        if (!disableNonCoreServices) {
            // try {
            Slogger::I(TAG, "Search Service");
            AutoPtr<IISearchManager> searchManager;
            ec = CSearchManagerService::New(context, (IISearchManager**)&searchManager);
            if (FAILED(ec)) ReportWtf("starting Audio Service", ec);
            ServiceManager::AddService(IContext::SEARCH_SERVICE, searchManager.Get());
            // } catch (Throwable e) {
            //     ReportWtf("starting Search Service", ec);
            // }
        }

    //     try {
    //         Slogger::I(TAG, "DropBox Service");
    //         ServiceManager::AddService(IContext::DROPBOX_SERVICE,
    //                 new DropBoxManagerService(context, new File("/data/system/dropbox")));
    //     } catch (Throwable e) {
    //         ReportWtf("starting DropBoxManagerService", ec);
    //     }

        if (!disableNonCoreServices
            && (res->GetBoolean(R::bool_::config_enableWallpaperService, &bval), bval)) {
            Slogger::I(TAG, "Wallpaper Service");
            ec = CWallpaperManagerService::NewByFriend(context, (CWallpaperManagerService**)&wallpaper);
            if (FAILED(ec)) ReportWtf("starting Wallpaper Service", ec);
            ServiceManager::AddService(IContext::WALLPAPER_SERVICE, (IIWallpaperManager*)wallpaper.Get());
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Profile Manager");
            ec = CProfileManagerService::New(context, (IIProfileManager**)&profile);
            if (FAILED(ec)) ReportWtf("Failure starting Profile Manager", ec);
            ServiceManager::AddService(IContext::PROFILE_SERVICE, profile);
        }

        if (!disableMedia &&
            (systemProperties->Get(String("system_init.startaudioservice"), &str), !str.Equals("0"))) {
            Slogger::I(TAG, "Audio Service");
            ec = CAudioService::New(context, (IAudioService**)&audioService);
            if (FAILED(ec)) ReportWtf("starting Audio Service", ec);
            ServiceManager::AddService(IContext::AUDIO_SERVICE, audioService);
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Dock Observer todo");
            // systemService = NULL;
            // ec = CDockObserver::New((ISystemService**)&systemService);
            // if (FAILED(ec)) ReportWtf("starting Dock Observer", ec);
            // mSystemServiceManager->StartService(systemService.Get());
        }

        if (!disableMedia) {
            //try {
                Slogger::I(TAG, "Wired Accessory Manager");
                // Listen for wired headset changes
                inputManager->SetWiredAccessoryCallbacks(
                        new WiredAccessoryManager(context, inputManager));

            //} catch (Throwable e) {
            //if (FAILED(ec)) ReportWtf("starting WiredAccessoryManager", ec);
            //}
        }

        if (!disableNonCoreServices) {
            Boolean res;
            if ((mPackageManager->HasSystemFeature(IPackageManager::FEATURE_USB_HOST, &res), res)
                || (mPackageManager->HasSystemFeature(IPackageManager::FEATURE_USB_ACCESSORY, &res), res)) {
                // Manage USB host and device support
                systemService = new CUsbService::Lifecycle(context);
                mSystemServiceManager->StartService(systemService);
            }

            // try {
            //     Slogger::I(TAG, "Serial Service");
            //     // Serial port support
            //     serial = new SerialService(context);
            //     ServiceManager::AddService(IContext::SERIAL_SERVICE, serial);
            // } catch (Throwable e) {
            //     Slog.e(TAG, "Failure starting SerialService", ec);
            // }
        }

        Slogger::I(TAG, "Twilight Service");
        systemService = NULL;
        ec = CTwilightService::New(context, (ISystemService**)&systemService);
        if (FAILED(ec)) ReportWtf("making TwilightService ready", ec);
        mSystemServiceManager->StartService(systemService);

        Slogger::I(TAG, "UiMode manager Service");
        systemService = NULL;
        ec = CUiModeManagerService::New(context, (ISystemService**)&systemService);
        if (FAILED(ec)) ReportWtf("making UiMode manager Service ready", ec);
        mSystemServiceManager->StartService(systemService);

        Slogger::I(TAG, "Job Scheduler Service todo");
        // AutoPtr<JobSchedulerService> jss = new JobSchedulerService();
        // ec = jss->constructor(context);
        // if (FAILED(ec)) ReportWtf("making Job Scheduler Service ready", ec);
        //mSystemServiceManager->StartService(JobSchedulerService.Get());

        if (!disableNonCoreServices) {
            mPackageManager->HasSystemFeature(IPackageManager::FEATURE_BACKUP, &bval);
            if (bval) {
                Slogger::I(TAG, "Bacup manager Service todo");
                //mSystemServiceManager->StartService(BACKUP_MANAGER_SERVICE_CLASS);
            }

            mPackageManager->HasSystemFeature(IPackageManager::FEATURE_APP_WIDGETS, &bval);
            if (bval) {
                Slogger::I(TAG, "AppWidget Service");
                systemService = NULL;
                CAppWidgetService::New(context, (ISystemService**)&systemService);
                mSystemServiceManager->StartService(systemService);
            }

            mPackageManager->HasSystemFeature(IPackageManager::FEATURE_VOICE_RECOGNIZERS, &bval);
            if (bval) {
                Slogger::I(TAG, "Voice recognizer Service todo");
                // mSystemServiceManager->StartService(VOICE_RECOGNITION_MANAGER_SERVICE_CLASS);
            }
        }

    //     try {
    //         Slogger::I(TAG, "DiskStats Service");
    //         ServiceManager::AddService("diskstats", new DiskStatsService(context));
    //     } catch (Throwable e) {
    //         ReportWtf("starting DiskStats Service", ec);
    //     }

    //     try {
    //         // need to add this service even if SamplingProfilerIntegration.isEnabled()
    //         // is false, because it is this service that detects system property change and
    //         // turns on SamplingProfilerIntegration. Plus, when sampling profiler doesn't work,
    //         // there is little overhead for running this service.
    //         Slogger::I(TAG, "SamplingProfiler Service");
    //         ServiceManager::AddService("samplingprofiler",
    //                     new SamplingProfilerService(context));
    //     } catch (Throwable e) {
    //         ReportWtf("starting SamplingProfiler Service", ec);
    //     }

        if (!disableNetwork) {
            // try {
                Slogger::I(TAG, "NetworkTimeUpdateService");
                networkTimeUpdater = new NetworkTimeUpdateService();
                networkTimeUpdater->constructor(context);
            // } catch (Throwable e) {
            //     ReportWtf("starting NetworkTimeUpdate service", ec);
            // }
        }

    //     if (!disableMedia) {
    //         try {
    //             Slogger::I(TAG, "CommonTimeManagementService");
    //             commonTimeMgmtService = new CommonTimeManagementService(context);
    //             ServiceManager::AddService("commontime_management", commonTimeMgmtService);
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

        if (!disableNonCoreServices) {
            // Dreams (interactive idle-time views, a/k/a screen savers, and doze mode)
            systemService = NULL;
            ec = CDreamManagerService::New(context, (ISystemService**)&systemService);
            if (FAILED(ec)) ReportWtf("create DreamManager Service ready", ec);
            ec = mSystemServiceManager->StartService(systemService);
            if (FAILED(ec)) ReportWtf("making DreamManager Service ready", ec);
        }

        if (!disableNonCoreServices && !disableAtlas) {
            Slogger::I(TAG, "Assets Atlas Service");
            ec = CAssetAtlasService::New(context, (IIAssetAtlas**)&atlas);
            if (FAILED(ec)) ReportWtf("making Assets Atlas Service ready", ec);
            ServiceManager::AddService(CAssetAtlasService::ASSET_ATLAS_SERVICE, atlas);
        }

        // if (res->GetBoolean(R::bool_::config_enableGestureService, &bval), bval) {
        //     try {
        //         Slogger::I(TAG, "Gesture Sensor Service");
        //         gestureService = new GestureService(context, inputManager);
        //         ServiceManager.addService("gesture", gestureService);
        //     } catch (Throwable e) {
        //         Slog.e(TAG, "Failure starting Gesture Sensor Service", e);
        //     }
        // }

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_PRINTING)) {
    //         mSystemServiceManager->StartService(PRINT_MANAGER_SERVICE_CLASS);
    //     }

    //     mSystemServiceManager->StartService(RestrictionsManagerService.class);

    Slogger::I(TAG, "MediaSessionService");
    AutoPtr<ISystemService> mediaSessionService;
    ec = mSystemServiceManager->StartService(String("Elastos.Droid.Server.Media.CMediaSessionService"), (ISystemService**)&mediaSessionService);
    if (FAILED(ec)) Slogger::I(TAG, "MediaSessionService start fail");

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_HDMI_CEC)) {
    //         mSystemServiceManager->StartService(HdmiControlService.class);
    //     }

    //     if (mPackageManager.hasSystemFeature(PackageManager.FEATURE_LIVE_TV)) {
    //         mSystemServiceManager->StartService(TvInputManagerService.class);
    //     }

        if (!disableNonCoreServices && !mOnlyCore) {
            Slogger::I(TAG, "Theme Service");
            ec = CThemeService::New(context, (IIThemeService**)&themeService);
            if (FAILED(ec)) ReportWtf("starting Theme Service", ec);
            ServiceManager::AddService(IContext::THEME_SERVICE, themeService);
        }

    // try {
    //     Slogger::I(TAG, "KillSwitch Service");
    //     killSwitchService = new KillSwitchService(context);
    //     ServiceManager.addService(Context.KILLSWITCH_SERVICE, killSwitchService);
    // } catch (Throwable e) {
    //     reportWtf("starting KillSwitch Service", e);
    // }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Media Router Service");
            ec = CMediaRouterService::NewByFriend(context, (CMediaRouterService**)&mediaRouter);
            if (FAILED(ec)) ReportWtf("starting MediaRouterService", ec);
            ServiceManager::AddService(IContext::MEDIA_ROUTER_SERVICE, (IBinder*)mediaRouter);

            // mSystemServiceManager->StartService(TrustManagerService.class);

            // mSystemServiceManager->StartService(FingerprintService.class);

            // try {
            //     Slogger::I(TAG, "BackgroundDexOptService");
            //     BackgroundDexOptService.schedule(context);
            // } catch (Throwable e) {
            //     ReportWtf("starting BackgroundDexOptService", ec);
            // }
        }

        if (!disableNonCoreServices) {
            Slogger::I(TAG, "Launcher CmHardwareService");
            AutoPtr<IICmHardwareService> cmhs;
            ec = CCmHardwareService::New(context, (IICmHardwareService**)&cmhs);
            if (FAILED(ec)) ReportWtf("starting CMHW Service", ec);
            ServiceManager::AddService(IContext::CMHW_SERVICE, cmhs);
        }

        Slogger::I(TAG, "Launcher Apps Service");
        systemService = NULL;
        ec = CLauncherAppsService::New(context, (ISystemService**)&systemService);
        if (FAILED(ec)) ReportWtf("making Launcher Apps Service ready", ec);
        mSystemServiceManager->StartService(systemService.Get());

        // boolean isWipowerEnabled = SystemProperties.getBoolean("ro.bluetooth.wipower", false);

        // if (isWipowerEnabled) {
        //     try {
        //         final String WBC_SERVICE_NAME = "wbc_service";
        //         Slogger::I(TAG, "WipowerBatteryControl Service");

        //         PathClassLoader wbcClassLoader =
        //             new PathClassLoader("/system/framework/com.quicinc.wbc.jar:/system/framework/com.quicinc.wbcservice.jar",
        //                                 ClassLoader.getSystemClassLoader());
        //         Class wbcClass = wbcClassLoader.loadClass("com.quicinc.wbcservice.WbcService");
        //         Constructor<Class> ctor = wbcClass.getConstructor(Context.class);
        //         Object wbcObject = ctor.newInstance(context);
        //         Slog.d(TAG, "Successfully loaded WbcService class");
        //         ServiceManager.addService(WBC_SERVICE_NAME, (IBinder) wbcObject);
        //     } catch (Throwable e) {
        //         reportWtf("starting WipowerBatteryControl Service", e);
        //     }
        // } else {
        //     Slog.d(TAG, "Wipower not supported");
        // }

        // try {
        //     Slogger::I(TAG, "EdgeGesture service");
        //     edgeGestureService = new EdgeGestureService(context, inputManager);
        //     ServiceManager.addService("edgegestureservice", edgeGestureService);
        // } catch (Throwable e) {
        //     Slog.e(TAG, "Failure starting EdgeGesture service", e);
        // }
    }

    // if (!disableNonCoreServices) {
    //     mSystemServiceManager->StartService(MediaProjectionManagerService.class);
    // }

    // // make sure the ADB_ENABLED setting value matches the secure property value
    // Settings.Secure.putInt(mContentResolver, Settings.Secure.ADB_PORT,
    //         Integer.parseInt(SystemProperties.get("service.adb.tcp.port", "-1")));

    // // register observer to listen for settings changes
    // mContentResolver.registerContentObserver(
    //     Settings.Secure.getUriFor(Settings.Secure.ADB_PORT),
    //     false, new AdbPortObserver());

    // Before things start rolling, be sure we have decided whether
    // we are in safe mode.
    Boolean safeMode = wm->DetectSafeMode();
    if (safeMode) {
        mActivityManagerService->EnterSafeMode();
    //     // Disable the JIT for the system_server process
    //     VMRuntime.getRuntime().disableJitCompilation();
    } else {
    //     // Enable the JIT for the system_server process
    //     VMRuntime.getRuntime().startJitCompilation();
    }

    // // MMS service broker
    // mmsService = mSystemServiceManager->StartService(MmsServiceBroker.class);

    // It is now time to start up the app processes...

    ec = vibrator->SystemReady();
    if (FAILED(ec)) Slogger::E(TAG, "failed to making vibrator Service ready");

    if (lockSettings != NULL) {
        // ec = lockSettings->SystemReady();
        // if (FAILED(ec)) Slogger::E(TAG, "failed to making lockSettings Service ready");
    }

    // Needed by DevicePolicyManager for initialization
    mSystemServiceManager->StartBootPhase(ISystemService::PHASE_LOCK_SETTINGS_READY);
    mSystemServiceManager->StartBootPhase(ISystemService::PHASE_SYSTEM_SERVICES_READY);

    Slogger::I(TAG, "Window Manager Service ready...");
    ec = wm->SystemReady();
    if (FAILED(ec)) ReportWtf("making Window Manager Service ready", ec);

    if (safeMode) {
        mActivityManagerService->ShowSafeModeOverlay();
    }

    // Update the configuration for this context by hand, because we're going
    // to start using it before the config change done in wm->SystemReady() will
    // propagate to it.
    AutoPtr<IConfiguration> config = wm->ComputeNewConfiguration();
    AutoPtr<IDisplayMetrics> metrics;
    CDisplayMetrics::New((IDisplayMetrics**)&metrics);
    service = NULL;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IWindowManager> w = IWindowManager::Probe(service);
    AutoPtr<IDisplay> display;
    w->GetDefaultDisplay((IDisplay**)&display);
    display->GetMetrics(metrics);
    res->UpdateConfiguration(config, metrics);

    // try {
    // TODO: use boot phase
    Slogger::I(TAG, "Power Manager Service ready...");
    ec = mPowerManagerService->SystemReady(mActivityManagerService->GetAppOpsService());
    if (FAILED(ec)) ReportWtf("making Power Manager Service ready", ec);
    // } catch (Throwable e) {
    //     ReportWtf("making Power Manager Service ready", ec);
    // }

    Slogger::I(TAG, "Package Manager Service ready...");
    ec = mPackageManagerService->SystemReady();
    if (FAILED(ec)) ReportWtf("making Package Manager Service ready", ec);

    // TODO: use boot phase and communicate these flags some other way
    ec = mDisplayManagerService->SystemReady(safeMode, mOnlyCore);
    if (FAILED(ec)) ReportWtf("making Display Manager Service ready", ec);


    // if (edgeGestureService != null) {
    //     try {
    //         edgeGestureService.systemReady();
    //     } catch (Throwable e) {
    //         reportWtf("making EdgeGesture service ready", e);
    //     }
    // }

    // if (gestureService != null) {
    //     try {
    //         gestureService.systemReady();
    //     } catch (Throwable e) {
    //         reportWtf("making Gesture Sensor Service ready", e);
    //     }
    // }

    Slogger::I(TAG, "other services ready...");

    // IntentFilter filter = new IntentFilter();
    // filter.addAction(Intent.ACTION_APP_FAILURE);
    // filter.addAction(Intent.ACTION_APP_FAILURE_RESET);
    // filter.addAction(Intent.ACTION_PACKAGE_ADDED);
    // filter.addAction(Intent.ACTION_PACKAGE_REMOVED);
    // filter.addAction(ThemeUtils.ACTION_THEME_CHANGED);
    // filter.addCategory(Intent.CATEGORY_THEME_PACKAGE_INSTALLED_STATE_CHANGE);
    // filter.addDataScheme("package");
    // context.registerReceiver(new AppsFailureReceiver(), filter);

    // These are needed to propagate to the runnable below.
    AutoPtr<ServiceBundle> bundle = new ServiceBundle();
    bundle->mMountServiceF = mountService;
    bundle->mNetworkManagementF = networkManagement;
    bundle->mNetworkStatsF = networkStats;
    bundle->mNetworkPolicyF = networkPolicy;
    // bundle->mConnectivityF = connectivity;
    bundle->mNetworkScoreF = networkScore;
    bundle->mWallpaperF = wallpaper;
    bundle->mImmF = imm;
    bundle->mLocationF = location;
    bundle->mCountryDetectorF = countryDetector;
    bundle->mNetworkTimeUpdaterF = networkTimeUpdater;
    // bundle->mCommonTimeMgmtServiceF = commonTimeMgmtService;
    bundle->mTextServiceManagerServiceF = tsms;
    bundle->mStatusBarF = statusBar;
    bundle->mAtlasF = (CAssetAtlasService*)atlas.Get();
    bundle->mInputManagerF = inputManager;
    // bundle->mTelephonyRegistryF = telephonyRegistry;
    bundle->mMediaRouterF = mediaRouter;
    bundle->mAudioServiceF = audioService;
    // bundle->mMmsServiceF = mmsService;
    bundle->mThemeServiceF = (CThemeService*)themeService.Get();

    // We now tell the activity manager it is okay to run third party
    // code.  It will call back into us once it has gotten to the state
    // where third party code can really run (but before it has actually
    // started launching the initial applications), for us to complete our
    // initialization.
    AutoPtr<IRunnable> runnable = new SystemReadyRunnable(this, bundle);
    mActivityManagerService->SystemReady(runnable);
    return NOERROR;
}

ECode SystemServer::StartSystemUi(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    AutoPtr<IComponentName> name;
    CComponentName::New(
        String("Elastos.Droid.SystemUI"),
        String("Elastos.Droid.SystemUI.CSystemUIService"),
        (IComponentName**)&name);
    intent->SetComponent(name);
    //Slog.d(TAG, "Starting service: " + intent);
    AutoPtr<IComponentName> result;
    context->StartServiceAsUser(intent, UserHandle::OWNER, (IComponentName**)&result);
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

    ECode ec = NOERROR;

    mHost->mSystemServiceManager->StartBootPhase(
        ISystemService::PHASE_ACTIVITY_MANAGER_READY);

    ec = mHost->mActivityManagerService->StartObservingNativeCrashes();
    if (FAILED(ec)) mHost->ReportWtf("observing native crashes", ec);

    Slogger::I(SystemServer::TAG, "WebViewFactory preparation todo");
    AutoPtr<IWebViewFactory> webViewFactory;
    CWebViewFactory::AcquireSingleton((IWebViewFactory**)&webViewFactory);
    webViewFactory->PrepareWebViewInSystemServer();

    Slogger::I(SystemServer::TAG, "start System UI.");
    ec = StartSystemUi(mHost->mSystemContext);
    if (FAILED(ec)) {
        mHost->ReportWtf("starting System UI", ec);
    }

    if (mServiceBundle->mMountServiceF != NULL) {
        ec = mServiceBundle->mMountServiceF->SystemReady();
        if (FAILED(ec)) mHost->ReportWtf("making Mount Service ready", ec);
    }

    // if (mServiceBundle->mNetworkScoreF != NULL) {
    //     ec = mServiceBundle->mNetworkScoreF->SystemReady();
    //     if (FAILED(ec)) mHost->ReportWtf("making Network Score Service ready", ec);
    // }

    // if (mServiceBundle->mNetworkManagementF != NULL) {
    //     ec = mServiceBundle->mNetworkManagementF->SystemReady();
    //     if (FAILED(ec)) mHost->ReportWtf("making Network Managment Service ready", ec);
    // }

    // if (mServiceBundle->mNetworkStatsF != NULL) {
    //     ec = mServiceBundle->mNetworkStatsF->SystemReady();
    //     if (FAILED(ec)) mHost->ReportWtf("making Network Stats Service ready", ec);
    // }

    // if (mServiceBundle->mNetworkPolicyF != NULL) {
    //     ec = mServiceBundle->mNetworkPolicyF->SystemReady();
    //     if (FAILED(ec)) mHost->ReportWtf("making Network Policy Service ready", ec);

    // }

    // if (mServiceBundle->mConnectivityF != NULL) {
    //     ec = mServiceBundle->mConnectivityF->SystemReady();
    //     if (FAILED(ec)) mHost->ReportWtf("making Connectivity Service ready", ec);
    // }

    if (mServiceBundle->mAudioServiceF != NULL) {
        ec = mServiceBundle->mAudioServiceF->SystemReady();
        if (FAILED(ec)) mHost->ReportWtf("making AudioService ready", ec);
    }

    Watchdog::GetInstance()->Start();

    // It is now okay to let the various system services start their
    // third party code...
    mHost->mSystemServiceManager->StartBootPhase(
        ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START);

    if (mServiceBundle->mWallpaperF != NULL) {
        ec = mServiceBundle->mWallpaperF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying WallpaperService running", ec);
    }

    if (mServiceBundle->mImmF != NULL) {
        ec = mServiceBundle->mImmF->SystemRunning(mServiceBundle->mStatusBarF);
        if (FAILED(ec)) mHost->ReportWtf("Notifying InputMethodService running", ec);
    }

    if (mServiceBundle->mLocationF != NULL) {
        ec = mServiceBundle->mLocationF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying Location Service running", ec);
    }

    if (mServiceBundle->mCountryDetectorF != NULL) {
        ec = mServiceBundle->mCountryDetectorF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying CountryDetectorService running", ec);
    }

    if (mServiceBundle->mNetworkTimeUpdaterF != NULL) {
        ec = mServiceBundle->mNetworkTimeUpdaterF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying NetworkTimeService running", ec);
    }

    // if (mServiceBundle->mCommonTimeMgmtServiceF != NULL) {
    //     ec = mServiceBundle->mCommonTimeMgmtServiceF->SystemRunning();
    //     if (FAILED(ec)) mHost->ReportWtf("Notifying CommonTimeManagementService running", ec);
    // }

    if (mServiceBundle->mTextServiceManagerServiceF != NULL) {
        ec = mServiceBundle->mTextServiceManagerServiceF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying TextServicesManagerService running", ec);
    }

    if (mServiceBundle->mAtlasF != NULL) {
        ec = mServiceBundle->mAtlasF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying AssetAtlasService running", ec);
    }

    if (mServiceBundle->mInputManagerF != NULL) {
        // TODO(BT) Pass parameter to input manager
        ec = mServiceBundle->mInputManagerF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying InputManagerService running", ec);
    }

    // if (mServiceBundle->mTelephonyRegistryF != NULL) {
    //     ec = mServiceBundle->mTelephonyRegistryF->SystemRunning();
    //     if (FAILED(ec)) mHost->ReportWtf("Notifying TelephonyRegistry running", ec);

    // }

    if (mServiceBundle->mMediaRouterF != NULL) {
        ec = mServiceBundle->mMediaRouterF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying MediaRouterService running", ec);
    }

    // if (mServiceBundle->mMmsServiceF != NULL) {
    //     ec = mServiceBundle->mMmsServiceF->SystemRunning();
    //     if (FAILED(ec)) mHost->ReportWtf("Notifying MmsService running", ec);
    // }

    // now that the system is up, apply default theme if applicable
    if (mServiceBundle->mThemeServiceF != NULL){
        ec = mServiceBundle->mThemeServiceF->SystemRunning();
        if (FAILED(ec)) mHost->ReportWtf("Notifying Theme running", ec);
    }

    AutoPtr<IThemeConfigHelper> tcHelper;
    CThemeConfigHelper::AcquireSingleton((IThemeConfigHelper**)&tcHelper);
    AutoPtr<IContentResolver> cr;
    mHost->mSystemContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IThemeConfig> themeConfig;
    tcHelper->GetBootTheme(cr, (IThemeConfig**)&themeConfig);
    String iconPkg;
    themeConfig->GetIconPackPkgName(&iconPkg);
    ec = mHost->mPackageManagerService->UpdateIconMapping(iconPkg);
    if (FAILED(ec)) mHost->ReportWtf("Icon Mapping failed", ec);

    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
