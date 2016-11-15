
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ActivityContainer.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActiveServices.h"
#include "elastos/droid/server/am/AppErrorResult.h"
#include "elastos/droid/server/am/AppErrorDialog.h"
#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/CompatModeDialog.h"
#include "elastos/droid/server/am/FactoryErrorDialog.h"
#include "elastos/droid/server/am/AppNotRespondingDialog.h"
#include "elastos/droid/server/am/StrictModeViolationDialog.h"
#include "elastos/droid/server/am/BroadcastFilter.h"
#include "elastos/droid/server/am/BroadcastQueue.h"
#include "elastos/droid/server/am/BroadcastRecord.h"
#include "elastos/droid/server/am/CPendingIntentRecord.h"
#include "elastos/droid/server/am/CContentProviderConnection.h"
#include "elastos/droid/server/am/LaunchWarningWindow.h"
#include "elastos/droid/server/am/ProcessList.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ProviderMap.h"
#include "elastos/droid/server/am/ProcessMemInfo.h"
#include "elastos/droid/server/am/ReceiverList.h"
#include "elastos/droid/server/am/UserStartedState.h"
#include "elastos/droid/server/am/TaskRecord.h"
#include "elastos/droid/server/am/TaskPersister.h"
#include "elastos/droid/server/am/ActivityState.h"
#include "elastos/droid/server/am/AppWaitingForDebuggerDialog.h"
#include "elastos/droid/server/am/CPermissionController.h"
#include "elastos/droid/server/am/NativeCrashListener.h"
#include "elastos/droid/server/am/CAppTaskImpl.h"
#include "elastos/droid/server/am/UserSwitchingDialog.h"
#include "elastos/droid/server/am/CActivityManagerBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSwitchUserReceiver.h"
#include "elastos/droid/server/am/CActivityManagerNeedStartReceiver.h"
#include "elastos/droid/server/am/CActivityManagerPreBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSystemReadyReceiver.h"
#include "elastos/droid/server/am/CActivityManagerShutdownReceiver.h"
#include "elastos/droid/server/am/CActivityManagerStoppingReceiver.h"
#include "elastos/droid/server/am/CActivityManagerDispatchUserSwitchCallback.h"
#include "elastos/droid/server/wm/AppTransition.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/AttributeCache.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/server/CAppOpsService.h"
#include "elastos/droid/server/Watchdog.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/content/Context.h>
#include <elastos/droid/content/pm/ThemeUtils.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/FactoryTest.h>
#include <elastos/droid/os/FileUtils.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SELinux.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/droid/R.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/Algorithm.h>
#include <cutils/properties.h>
#include <dlfcn.h>
#include <unistd.h>
#include <elastos/droid/utility/MemoryDumper.h>
using Elastos::Droid::Utility::MemoryDumper;

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerWaitResult;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CActivityManagerMemoryInfo;
using Elastos::Droid::App::CActivityManagerProcessErrorStateInfo;
using Elastos::Droid::App::CActivityManagerRecentTaskInfo;
using Elastos::Droid::App::CActivityManagerRunningAppProcessInfo;
using Elastos::Droid::App::IActivityManagerRunningAppProcessInfoHelper;
using Elastos::Droid::App::CActivityManagerRunningAppProcessInfoHelper;
using Elastos::Droid::App::CActivityManagerTaskDescription;
using Elastos::Droid::App::CApplicationErrorReport;
using Elastos::Droid::App::CApplicationErrorReportHelper;
using Elastos::Droid::App::IApplicationErrorReportHelper;
using Elastos::Droid::App::CApplicationErrorReportAnrInfo;
using Elastos::Droid::App::IApplicationErrorReportAnrInfo;
using Elastos::Droid::App::CContextImpl;
using Elastos::Droid::App::IContextImpl;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::EIID_IActivityManagerInternal;
using Elastos::Droid::App::EIID_IIActivityManager;
using Elastos::Droid::App::EIID_IIAppTask;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CNotificationManagerHelper;
using Elastos::Droid::App::INotificationManagerHelper;
using Elastos::Droid::App::IInstrumentation;
using Elastos::Droid::App::CProfilerInfo;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::Usage::IUsageEvent;
using Elastos::Droid::App::Usage::IUsageEvents;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::EIID_IIIntentSender;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::EIID_IIntentFilter;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::CConfigurationInfo;
using Elastos::Droid::Content::Pm::CParceledListSlice;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IInstrumentationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPathPermission;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::CThemeConfigHelper;
using Elastos::Droid::Content::Res::IThemeConfigHelper;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Internal::Os::IBatteryStatsImplUidPkg;
using Elastos::Droid::Internal::Os::IBatteryStatsImplUidPkgServ;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CProcessCpuTracker;
using Elastos::Droid::Internal::Os::EIID_IBatteryCallback;
using Elastos::Droid::Internal::Os::IProcessCpuTrackerStats;
using Elastos::Droid::Internal::Os::CTransferPipe;
using Elastos::Droid::Internal::Os::ITransferPipe;
using Elastos::Droid::Internal::Os::IZygote;
using Elastos::Droid::Internal::Utility::CMemInfoReader;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Internal::Utility::CFastPrintWriter;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Telephony::Cat::IAppInterface;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBatteryStatsUidProc;
using Elastos::Droid::Os::IBatteryStatsHistoryItem;
using Elastos::Droid::Os::CDebug;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebugMemoryInfo;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::CDebugMemoryInfoHelper;
using Elastos::Droid::Os::IDebugMemoryInfoHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIRemoteCallback;
using Elastos::Droid::Os::FactoryTest;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IPermissionController;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IPatternMatcher;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::CUpdateLock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IProcessStartResult;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Server::AttributeCache;
using Elastos::Droid::Server::CAppOpsService;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Server::Wm::AppTransition;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;

using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringToIntegral;
using Elastos::Core::IStringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::CString;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IPrintWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger64;;
using Elastos::Utility::Etl::Pair;
using Libcore::IO::CIoBridge;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

// 9d9d1d7f-98d8-4b71-b75f-d455e4ce60a1
extern "C" const InterfaceID EIID_StringObjectHashMap =
    { 0x9d9d1d7f, 0x98d8, 0x4b71, { 0xb7, 0x5f, 0xd4, 0x55, 0xe4, 0xce, 0x60, 0xa1 } };

//==============================================================================
//  CAR_INTERFACE_IMPL_WITH_CPP_CAST
//==============================================================================
PInterface CActivityManagerService::StringObjectHashMap::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return this;
    }
    if (riid == EIID_StringObjectHashMap ) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}

UInt32 CActivityManagerService::StringObjectHashMap::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CActivityManagerService::StringObjectHashMap::Release()
{
    return ElLightRefBase::Release();
}

ECode CActivityManagerService::StringObjectHashMap::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == this) {
        *iid = EIID_IInterface;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//==============================================================================
//  Init statics
//==============================================================================
static AutoPtr< ArrayOf<Int64> > InitDumpMemBuckets()
{
    Int64 DUMP_MEM_BUCKETS[] = {
        5*1024, 7*1024, 10*1024, 15*1024, 20*1024, 30*1024, 40*1024, 80*1024,
        120*1024, 160*1024, 200*1024,
        250*1024, 300*1024, 350*1024, 400*1024, 500*1024, 600*1024, 800*1024,
        1*1024*1024, 2*1024*1024, 5*1024*1024, 10*1024*1024, 20*1024*1024
    };

    Int32 size = ArraySize(DUMP_MEM_BUCKETS);
    AutoPtr< ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(size);
    array->Copy(DUMP_MEM_BUCKETS, size);
    return array;
}

static AutoPtr< ArrayOf<Int32> > InitDumpMemOomAdj()
{
    Int32 DUMP_MEM_OOM_ADJ[] = {
        ProcessList::NATIVE_ADJ,
        ProcessList::SYSTEM_ADJ, ProcessList::PERSISTENT_PROC_ADJ,
        ProcessList::PERSISTENT_SERVICE_ADJ, ProcessList::FOREGROUND_APP_ADJ,
        ProcessList::VISIBLE_APP_ADJ, ProcessList::PERCEPTIBLE_APP_ADJ,
        ProcessList::BACKUP_APP_ADJ, ProcessList::HEAVY_WEIGHT_APP_ADJ,
        ProcessList::SERVICE_ADJ, ProcessList::HOME_APP_ADJ,
        ProcessList::PREVIOUS_APP_ADJ, ProcessList::SERVICE_B_ADJ, ProcessList::CACHED_APP_MAX_ADJ
    };

    Int32 size = ArraySize(DUMP_MEM_OOM_ADJ);
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(size);
    array->Copy(DUMP_MEM_OOM_ADJ, size);
    return array;
}

static AutoPtr< ArrayOf<String> > InitDumpMemOomLabel()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(14);
    (*array)[0] = String("Native");
    (*array)[1] = String("System");
    (*array)[2] = String("Persistent");
    (*array)[3] = String("Persistent Service");
    (*array)[4] = String("Foreground");
    (*array)[5] = String("Visible");
    (*array)[6] = String("Perceptible");
    (*array)[7] = String("Heavy Weight");
    (*array)[8] = String("Backup");
    (*array)[9] = String("A Services");
    (*array)[10] = String("Home");
    (*array)[11] = String("Previous");
    (*array)[12] = String("B Services");
    (*array)[13] = String("Cached");
    return array;
}

static AutoPtr< ArrayOf<String> > InitDumpMemOomCompactLabel()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(14);
    (*array)[0] = String("native");
    (*array)[1] = String("sys");
    (*array)[2] = String("pers");
    (*array)[3] = String("persvc");
    (*array)[4] = String("fore");
    (*array)[5] = String("vis");
    (*array)[6] = String("percept");
    (*array)[7] = String("heavy");
    (*array)[8] = String("backup");
    (*array)[9] = String("servicea");
    (*array)[10] = String("home");
    (*array)[11] = String("prev");
    (*array)[12] = String("serviceb");
    (*array)[13] = String("cached");
    return array;
}

static Boolean InitIsUserBuild()
{
    return Build::TYPE == "user";
}

static Int32 InitMyPid()
{
    return Process::MyPid();
}

static void IdentityDestructor(void* st)
{
    CActivityManagerService::Identity* obj = static_cast<CActivityManagerService::Identity*>(st);
    if (obj) {
        obj->Release();
    }
}

static pthread_key_t InitSCallerIdentity()
{
    pthread_key_t key;
    pthread_key_create(&key, IdentityDestructor);
    return key;
}

const String CActivityManagerService::CALLED_PRE_BOOTS_FILENAME("called_pre_boots.dat");
const String CActivityManagerService::TAG("CActivityManagerService");
const String CActivityManagerService::TAG_MU("CActivityManagerServiceMU");
const Boolean CActivityManagerService::DEBUG = FALSE;
const Boolean CActivityManagerService::localLOGV = DEBUG;
const Boolean CActivityManagerService::DEBUG_BACKUP = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_BROADCAST = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_BROADCAST_LIGHT = DEBUG_BROADCAST || FALSE;
const Boolean CActivityManagerService::DEBUG_BACKGROUND_BROADCAST = DEBUG_BROADCAST || FALSE;
const Boolean CActivityManagerService::DEBUG_CLEANUP = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_CONFIGURATION = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_FOCUS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_IMMERSIVE = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_MU = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_OOM_ADJ = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_LRU = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_PAUSE = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_POWER = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_POWER_QUICK = DEBUG_POWER || FALSE;
const Boolean CActivityManagerService::DEBUG_PROCESS_OBSERVERS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_PROCESSES = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_PROVIDER = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_RESULTS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_SERVICE = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_SERVICE_EXECUTING = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_STACK = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_SWITCH = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_TASKS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_THUMBNAILS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_TRANSITION = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_URI_PERMISSION = localLOGV || TRUE;
const Boolean CActivityManagerService::DEBUG_USER_LEAVING = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_VISBILITY = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_PSS = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_LOCKSCREEN = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_RECENTS = localLOGV || FALSE;
const Boolean CActivityManagerService::VALIDATE_TOKENS = FALSE;
const Boolean CActivityManagerService::SHOW_ACTIVITY_START_TIME = FALSE;

const Int64 CActivityManagerService::BATTERY_STATS_TIME = 30 * 60 * 1000;
const Boolean CActivityManagerService::MONITOR_CPU_USAGE = TRUE;
const Int64 CActivityManagerService::MONITOR_CPU_MIN_TIME = 5 * 1000;
const Int64 CActivityManagerService::MONITOR_CPU_MAX_TIME = 0x0fffffff;
const Boolean CActivityManagerService::MONITOR_THREAD_CPU_USAGE = FALSE;

const Int32 CActivityManagerService::STOCK_PM_FLAGS = IPackageManager::GET_SHARED_LIBRARY_FILES;
const Boolean CActivityManagerService::IS_USER_BUILD = InitIsUserBuild();

const Int32 CActivityManagerService::MAX_RECENT_BITMAPS;
const Int64 CActivityManagerService::APP_SWITCH_DELAY_TIME;
const Int32 CActivityManagerService::PROC_START_TIMEOUT;
const Int32 CActivityManagerService::PROC_START_TIMEOUT_WITH_WRAPPER;
const Int32 CActivityManagerService::GC_TIMEOUT;
const Int32 CActivityManagerService::GC_MIN_INTERVAL;
const Int32 CActivityManagerService::FULL_PSS_MIN_INTERVAL;
const Int32 CActivityManagerService::FULL_PSS_LOWERED_INTERVAL;

const Int32 CActivityManagerService::POWER_CHECK_DELAY = (CActivityManagerService::DEBUG_POWER_QUICK ? 2 : 15) * 60 * 1000;;
const Int32 CActivityManagerService::WAKE_LOCK_MIN_CHECK_DURATION = (CActivityManagerService::DEBUG_POWER_QUICK ? 1 : 5) * 60 * 1000;
const Int32 CActivityManagerService::CPU_MIN_CHECK_DURATION = (CActivityManagerService::DEBUG_POWER_QUICK ? 1 : 5) * 60 * 1000;

const Int32 CActivityManagerService::BROADCAST_FG_TIMEOUT = 10 * 1000;
const Int32 CActivityManagerService::BROADCAST_BG_TIMEOUT = 60 * 1000;

const Int32 CActivityManagerService::KEY_DISPATCHING_TIMEOUT = 5 * 1000;
const Int32 CActivityManagerService::INSTRUMENTATION_KEY_DISPATCHING_TIMEOUT = 60 * 1000;

const Int32 CActivityManagerService::USER_SWITCH_TIMEOUT = 2 * 1000;
const Int32 CActivityManagerService::MAX_RUNNING_USERS = 3;
const Int32 CActivityManagerService::PENDING_ASSIST_EXTRAS_TIMEOUT;
const Int32 CActivityManagerService::MAX_PERSISTED_URI_GRANTS;

const Int32 CActivityManagerService::MY_PID = InitMyPid();
const AutoPtr< ArrayOf<String> > CActivityManagerService::EMPTY_STRING_ARRAY = ArrayOf<String>::Alloc(0);
const Int32 CActivityManagerService::DROPBOX_MAX_SIZE;
const String CActivityManagerService::PROP_REFRESH_THEME("sys.refresh_theme");
const Int32 CActivityManagerService::ALLOW_NON_FULL;
const Int32 CActivityManagerService::ALLOW_NON_FULL_IN_PROFILE;
const Int32 CActivityManagerService::ALLOW_FULL_ONLY;
const Int32 CActivityManagerService::LAST_PREBOOT_DELIVERED_FILE_VERSION;

const Int32 CActivityManagerService::SHOW_ERROR_MSG = 1;
const Int32 CActivityManagerService::SHOW_NOT_RESPONDING_MSG = 2;
const Int32 CActivityManagerService::SHOW_FACTORY_ERROR_MSG = 3;
const Int32 CActivityManagerService::UPDATE_CONFIGURATION_MSG = 4;
const Int32 CActivityManagerService::GC_BACKGROUND_PROCESSES_MSG = 5;
const Int32 CActivityManagerService::WAIT_FOR_DEBUGGER_MSG = 6;
const Int32 CActivityManagerService::SERVICE_TIMEOUT_MSG = 12;
const Int32 CActivityManagerService::UPDATE_TIME_ZONE = 13;
const Int32 CActivityManagerService::SHOW_UID_ERROR_MSG = 14;
const Int32 CActivityManagerService::IM_FEELING_LUCKY_MSG = 15;
const Int32 CActivityManagerService::PROC_START_TIMEOUT_MSG = 20;
const Int32 CActivityManagerService::DO_PENDING_ACTIVITY_LAUNCHES_MSG = 21;
const Int32 CActivityManagerService::KILL_APPLICATION_MSG = 22;
const Int32 CActivityManagerService::FINALIZE_PENDING_INTENT_MSG = 23;
const Int32 CActivityManagerService::POST_HEAVY_NOTIFICATION_MSG = 24;
const Int32 CActivityManagerService::CANCEL_HEAVY_NOTIFICATION_MSG = 25;
const Int32 CActivityManagerService::SHOW_STRICT_MODE_VIOLATION_MSG = 26;
const Int32 CActivityManagerService::CHECK_EXCESSIVE_WAKE_LOCKS_MSG = 27;
const Int32 CActivityManagerService::CLEAR_DNS_CACHE_MSG = 28;
const Int32 CActivityManagerService::UPDATE_HTTP_PROXY_MSG = 29;
const Int32 CActivityManagerService::SHOW_COMPAT_MODE_DIALOG_MSG = 30;
const Int32 CActivityManagerService::DISPATCH_PROCESSES_CHANGED = 31;
const Int32 CActivityManagerService::DISPATCH_PROCESS_DIED = 32;
const Int32 CActivityManagerService::REPORT_MEM_USAGE_MSG = 33;
const Int32 CActivityManagerService::REPORT_USER_SWITCH_MSG = 34;
const Int32 CActivityManagerService::CONTINUE_USER_SWITCH_MSG = 35;
const Int32 CActivityManagerService::USER_SWITCH_TIMEOUT_MSG = 36;
const Int32 CActivityManagerService::IMMERSIVE_MODE_LOCK_MSG = 37;
const Int32 CActivityManagerService::PERSIST_URI_GRANTS_MSG = 38;
const Int32 CActivityManagerService::REQUEST_ALL_PSS_MSG = 39;
const Int32 CActivityManagerService::START_PROFILES_MSG = 40;
const Int32 CActivityManagerService::UPDATE_TIME = 41;
const Int32 CActivityManagerService::SYSTEM_USER_START_MSG = 42;
const Int32 CActivityManagerService::SYSTEM_USER_CURRENT_MSG = 43;
const Int32 CActivityManagerService::ENTER_ANIMATION_COMPLETE_MSG = 44;
const Int32 CActivityManagerService::FINISH_BOOTING_MSG = 45;
const Int32 CActivityManagerService::START_USER_SWITCH_MSG = 46;
const Int32 CActivityManagerService::SEND_LOCALE_TO_MOUNT_DAEMON_MSG = 47;

const Int32 CActivityManagerService::POST_PRIVACY_NOTIFICATION_MSG = 48;
const Int32 CActivityManagerService::CANCEL_PRIVACY_NOTIFICATION_MSG = 49;

const Int32 CActivityManagerService::FIRST_ACTIVITY_STACK_MSG;
const Int32 CActivityManagerService::FIRST_BROADCAST_QUEUE_MSG;
const Int32 CActivityManagerService::FIRST_COMPAT_MODE_MSG;
const Int32 CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG;

const AutoPtr<ArrayOf<Int64> > CActivityManagerService::DUMP_MEM_BUCKETS = InitDumpMemBuckets();
const AutoPtr<ArrayOf<Int32> > CActivityManagerService::DUMP_MEM_OOM_ADJ = InitDumpMemOomAdj();
const AutoPtr<ArrayOf<String> > CActivityManagerService::DUMP_MEM_OOM_LABEL = InitDumpMemOomLabel();
const AutoPtr<ArrayOf<String> > CActivityManagerService::DUMP_MEM_OOM_COMPACT_LABEL = InitDumpMemOomCompactLabel();

const String CActivityManagerService::USER_DATA_DIR("/data/user/");
const String CActivityManagerService::SYSTEM_DEBUGGABLE("ro.debuggable");

const Int32 CActivityManagerService::MAX_DUP_SUPPRESSED_STACKS = 5000;

const String CActivityManagerService::TAG_URI_GRANTS("uri-grants");
const String CActivityManagerService::TAG_URI_GRANT("uri-grant");
const String CActivityManagerService::ATTR_USER_HANDLE("userHandle");
const String CActivityManagerService::ATTR_SOURCE_USER_ID("sourceUserId");
const String CActivityManagerService::ATTR_TARGET_USER_ID("targetUserId");
const String CActivityManagerService::ATTR_SOURCE_PKG("sourcePkg");
const String CActivityManagerService::ATTR_TARGET_PKG("targetPkg");
const String CActivityManagerService::ATTR_URI("uri");
const String CActivityManagerService::ATTR_MODE_FLAGS("modeFlags");
const String CActivityManagerService::ATTR_CREATED_TIME("createdTime");
const String CActivityManagerService::ATTR_PREFIX("prefix");

pthread_key_t CActivityManagerService::sCallerIdentity = InitSCallerIdentity();

const Int32 CActivityManagerService::COLLECT_PSS_BG_MSG = 1;

//==============================================================================
// CActivityManagerService::ForegroundToken
//==============================================================================
CActivityManagerService::ForegroundToken::ForegroundToken()
    : mPid(0)
{}

ECode CActivityManagerService::ForegroundToken::ProxyDied()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(CActivityManagerService::ForegroundToken, Object, IProxyDeathRecipient)

//==============================================================================
// CActivityManagerService::ProcessChangeItem
//==============================================================================
CActivityManagerService::ProcessChangeItem::ProcessChangeItem()
    : mChanges(0)
    , mUid(0)
    , mPid(0)
    , mProcessState(0)
    , mForegroundActivities(0)
{}

//==============================================================================
// CActivityManagerService::ProcessCpuThread
//==============================================================================

CActivityManagerService::ProcessCpuThread::ProcessCpuThread(
    /* [in] */ CActivityManagerService* host)
    : mHost(host)
{
    Thread::constructor(String("CpuTracker"));
}

ECode CActivityManagerService::ProcessCpuThread::Run()
{
    while (TRUE) {
//         try {
//             try {
        {
            AutoLock lock(this);
            const Int64 now = SystemClock::GetUptimeMillis();
            Int64 ltime;
            mHost->mLastCpuTime->Get(&ltime);
            Int64 nextCpuDelay = (ltime + MONITOR_CPU_MAX_TIME) - now;
            // TODO: wait for batteryservice
            // Int64 nextWriteDelay = (mHost->mLastWriteTime + BATTERY_STATS_TIME) - now;
            Int64 nextWriteDelay = BATTERY_STATS_TIME;
            //Slogger::I(TAG, "Cpu delay=" + nextCpuDelay
            //        + ", write delay=" + nextWriteDelay);
            if (nextWriteDelay < nextCpuDelay) {
                nextCpuDelay = nextWriteDelay;
            }
            if (nextCpuDelay > 0) {
                mHost->mProcessCpuMutexFree->Set(TRUE);
                Thread::Wait(nextCpuDelay);
            }
        }
//             } catch (InterruptedException e) {
//             }
        mHost->UpdateCpuStatsNow();
//         } catch (Exception e) {
//             Slogger::E(TAG, "Unexpected exception collecting process stats", e);
//         }
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::IntentFirewallInterface
//==============================================================================

CActivityManagerService::IntentFirewallInterface::IntentFirewallInterface(
    /* [in] */ CActivityManagerService* host)
    : mHost(host)
{}

// @Override
Int32 CActivityManagerService::IntentFirewallInterface::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported)
{
    return mHost->CheckComponentPermission(permission, pid, uid,
            owningUid, exported);
}

// @Override
AutoPtr<Object> CActivityManagerService::IntentFirewallInterface::GetAMSLock()
{
    return mHost;
}

//==============================================================================
// CActivityManagerService::ThemeChangeReceiver
//==============================================================================

ECode CActivityManagerService::ThemeChangeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mUiContext = NULL;
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ScreenStatusReceiver
//==============================================================================

ECode CActivityManagerService::ScreenStatusReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent == NULL || (intent->GetAction(&action), action == NULL)) {
        return NOERROR;
    }
    if (action.Equals(IAppInterface::CHECK_SCREEN_IDLE_ACTION)) {
        Slogger::I(TAG, "ICC has requested idle screen status");
        AutoPtr<IIntent> idleScreenIntent;
        CIntent::New(IAppInterface::CAT_IDLE_SCREEN_ACTION, (IIntent**)&idleScreenIntent);
        idleScreenIntent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        Boolean isIdle = mHost->GetFocusedStack()->IsHomeStack();
        idleScreenIntent->PutExtra(String("SCREEN_IDLE"), isIdle);
        Slogger::I(TAG, "Broadcasting Home idle screen Intent SCREEN_IDLE is %d", isIdle);
        mHost->mContext->SendBroadcast(idleScreenIntent);
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ItemMatcher
//==============================================================================

CActivityManagerService::ItemMatcher::ItemMatcher()
    : mAll(TRUE)
{}

CActivityManagerService::ItemMatcher::~ItemMatcher()
{
}

void CActivityManagerService::ItemMatcher::Build(
   /* [in] */ const String& name)
{
    AutoPtr<IComponentNameHelper> componentNameHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&componentNameHelper);
    AutoPtr<IComponentName> componentName;
    componentNameHelper->UnflattenFromString(name, (IComponentName**)&componentName);
    if (componentName != NULL) {
        if (mComponents == NULL) {
            mComponents = new List<AutoPtr<IComponentName> >;
        }
        mComponents->PushBack(componentName);
        mAll = FALSE;
    }
    else {
        Int32 objectId = 0;
        // Not a '/' separated full component name; maybe an object ID?
        //try {
        ECode ec = StringToIntegral::Parse(name, 16, &objectId);
        if (mObjects == NULL) {
            mObjects = new List<Int32>;
        }
        if (SUCCEEDED(ec)) {
            mObjects->PushBack(objectId);
            mAll = FALSE;
        }
        else {
        //} catch (RuntimeException e) {
            // Not an integer; just do string match.
            if (mStrings == NULL) {
                mStrings = new List<String>;
            }
            mStrings->PushBack(name);
            mAll = FALSE;
        }
        //}
    }
}

Int32 CActivityManagerService::ItemMatcher::Build(
    /* [in] */ const ArrayOf<String>* args,
    /* [in] */ Int32 opti)
{
    for (; opti < args->GetLength(); opti++) {
        String name = (*args)[opti];
        if (name.Equals("--")) {
            return opti + 1;
        }
        Build(name);
    }
    return opti;
}

Boolean CActivityManagerService::ItemMatcher::Match(
    /* [in] */ IInterface* object,
    /* [in] */ IComponentName* comp)
{
    if (mAll) {
        return TRUE;
    }

    if (mComponents != NULL) {
        List<AutoPtr<IComponentName> >::Iterator it;
        for (it = mComponents->Begin(); it != mComponents->End(); ++it) {
            Boolean res;
            if (IObject::Probe(*it)->Equals(comp, &res), res) {
                return TRUE;
            }
        }
    }
    if (mObjects != NULL) {
        List<Int32>::Iterator it;
        for (it = mObjects->Begin(); it != mObjects->End(); ++it) {
            Int32 obj = *it;
            Int32 hashCode = Object::GetHashCode(object);
            if (/*System.identityHashCode(object)*/hashCode == obj) {
                return TRUE;
            }
        }
    }
    if (mStrings != NULL) {
        String flat;
        comp->FlattenToString(&flat);
        List<String>::Iterator it;
        for (it = mStrings->Begin(); it != mStrings->End(); ++it) {
            String str = *it;
            if (flat.Contains(str)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

//==============================================================================
// CActivityManagerService::MemItem
//==============================================================================
CActivityManagerService::MemItem::MemItem(
    /* [in] */ const String& label,
    /* [in] */ const String& shortLabel,
    /* [in] */ Int64 pss,
    /* [in] */ Int32 id,
    /* [in] */ Boolean hasActivities)
    : mIsProc(TRUE)
    , mLabel(label)
    , mShortLabel(shortLabel)
    , mPss(pss)
    , mId(id)
    , mHasActivities(hasActivities)
{}

CActivityManagerService::MemItem::MemItem(
    /* [in] */ const String& label,
    /* [in] */ const String& shortLabel,
    /* [in] */ Int64 pss,
    /* [in] */ Int32 id)
    : mIsProc(FALSE)
    , mLabel(label)
    , mShortLabel(shortLabel)
    , mPss(pss)
    , mId(id)
    , mHasActivities(FALSE)
{}

CActivityManagerService::MemItem::~MemItem()
{
}

Boolean CActivityManagerService::MemItem::Compare(
    /* [in] */ MemItem* lhs,
    /* [in] */ MemItem* rhs)
{
    if (lhs->mPss < rhs->mPss) {
        return FALSE;
    }
    else if (lhs->mPss > rhs->mPss) {
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
// NeededUriGrants
//==============================================================================
NeededUriGrants::NeededUriGrants(
    /* [in] */ const String& targetPkg,
    /* [in] */ Int32 targetUid,
    /* [in] */ Int32 flags)
    : mTargetUid(targetUid)
    , mFlags(flags)
{}

//==============================================================================
// CActivityManagerService::ReceiverResolver
//==============================================================================
Boolean CActivityManagerService::ReceiverResolver::AllowFilterResult(
    /* [in] */ BroadcastFilter* filter,
    /* [in] */ List<AutoPtr<BroadcastFilter> >* dest)
{
    IBinder* target = IBinder::Probe(filter->mReceiverList->mReceiver);
    List<AutoPtr<BroadcastFilter> >::ReverseIterator rit = dest->RBegin();
    for (; rit != dest->REnd(); ++rit) {
        BroadcastFilter* bf = *rit;
        IBinder* r = IBinder::Probe(bf->mReceiverList->mReceiver);
        if (r == target) {
            return FALSE;
        }
    }
    return TRUE;
}

AutoPtr<BroadcastFilter> CActivityManagerService::ReceiverResolver::NewResult(
    /* [in] */ BroadcastFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ Int32 userId)
{
    if (userId == IUserHandle::USER_ALL || filter->mOwningUserId == IUserHandle::USER_ALL
            || userId == filter->mOwningUserId) {
        return IntentResolver<BroadcastFilter, BroadcastFilter>::NewResult(filter, match, userId);
    }
    return NULL;
}

AutoPtr< ArrayOf<BroadcastFilter*> > CActivityManagerService::ReceiverResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr< ArrayOf<BroadcastFilter*> > array = ArrayOf<BroadcastFilter*>::Alloc(size);
    return array;
}

Boolean CActivityManagerService::ReceiverResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ BroadcastFilter* filter)
{
    return packageName.Equals(filter->mPackageName);
}

//==============================================================================
// CActivityManagerService::Identity
//==============================================================================
CActivityManagerService::Identity::Identity(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
    : mPid(pid)
    , mUid(uid)
{}

//==============================================================================
// CActivityManagerService::AppDeathRecipient
//==============================================================================
CActivityManagerService::AppDeathRecipient::AppDeathRecipient(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 pid,
    /* [in] */ IApplicationThread* thread,
    /* [in] */ CActivityManagerService* owner)
    : mApp(app)
    , mPid(pid)
    , mAppThread(thread)
    , mOwner(owner)
{}

ECode CActivityManagerService::AppDeathRecipient::ProxyDied()
{
    IBinder* binder = IBinder::Probe(mAppThread);
    String info;
    binder->ToString(&info);
    Slogger::V(TAG, "Death received in %p  for thread %p : %s",
        this, IBinder::Probe(mAppThread), info.string());

    AutoLock lock(mOwner);
    return mOwner->AppDiedLocked(mApp, mPid, mAppThread);
}

CAR_INTERFACE_IMPL(CActivityManagerService::AppDeathRecipient, Object, IProxyDeathRecipient)

//==============================================================================
// CActivityManagerService::SwitchUserReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::BootCompletedReceiver, Object, IIntentReceiver, IBinder)

ECode CActivityManagerService::BootCompletedReceiver::constructor(
    /* [in] */ IIActivityManager* host)
{
    mHost = (CActivityManagerService*)host;
    return NOERROR;
}

ECode CActivityManagerService::BootCompletedReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    AutoLock syncLock(mHost);
    mHost->RequestPssAllProcsLocked(SystemClock::GetUptimeMillis(), TRUE, FALSE);
    return NOERROR;
}

ECode CActivityManagerService::BootCompletedReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::SwitchUserReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::SwitchUserReceiver, Object, IIntentReceiver, IBinder);

ECode CActivityManagerService::SwitchUserReceiver::constructor(
    /* [in] */ IIActivityManager* host,
    /* [in] */ IUserStartedState* uss,
    /* [in] */ Boolean foreground,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 userId)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    mForeground = foreground;
    mOldUserId = oldUserId;
    mUserId = userId;
    return NOERROR;
}

ECode CActivityManagerService::SwitchUserReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return mHost->OnUserInitialized(mUss, mForeground, mOldUserId, mUserId);
}

ECode CActivityManagerService::SwitchUserReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::NeedStartReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::NeedStartReceiver, Object, IIntentReceiver, IBinder);

ECode CActivityManagerService::NeedStartReceiver::constructor()
{
    return NOERROR;
}

ECode CActivityManagerService::NeedStartReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return E_REMOTE_EXCEPTION;
}

ECode CActivityManagerService::NeedStartReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::ReportMemUsageThread
//==============================================================================

CAR_INTERFACE_IMPL(CActivityManagerService::ReportMemUsageThread::ProcessMemInfoComparator,
    Object, IComparator)

ECode CActivityManagerService::ReportMemUsageThread::ProcessMemInfoComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    ProcessMemInfo* lhs = (ProcessMemInfo*)IProcessMemInfo::Probe(_lhs);
    ProcessMemInfo* rhs = (ProcessMemInfo*)IProcessMemInfo::Probe(_rhs);
    if (lhs->mOomAdj != rhs->mOomAdj) {
        *result = lhs->mOomAdj < rhs->mOomAdj ? -1 : 1;
        return NOERROR;
    }
    if (lhs->mPss != rhs->mPss) {
        *result = lhs->mPss < rhs->mPss ? 1 : -1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

CActivityManagerService::ReportMemUsageThread::ReportMemUsageThread(
    /* [in] */ IList* memInfos,
    /* [in] */ CActivityManagerService* host)
    : mMemInfos(memInfos)
    , mHost(host)
{
    Thread::constructor();
}

ECode CActivityManagerService::ReportMemUsageThread::Run()
{
    IList* memInfos = mMemInfos;
    Int32 size;
    memInfos->GetSize(&size);
    HashMap<Int32, AutoPtr<ProcessMemInfo> > infoMap(size);
    for (Int32 i = 0, N = size; i < N; i++) {
        AutoPtr<IInterface> item;
        mMemInfos->Get(i, (IInterface**)&item);
        AutoPtr<ProcessMemInfo> mi = (ProcessMemInfo*)IProcessMemInfo::Probe(item);
        infoMap[mi->mPid] = mi;
    }
    mHost->UpdateCpuStatsNow();
    AutoPtr<IDebug> debug;
    CDebug::AcquireSingleton((IDebug**)&debug);
    {
        AutoLock lock(mHost->mProcessCpuTracker);
        Int32 N;
        mHost->mProcessCpuTracker->CountStats(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IProcessCpuTrackerStats> st;
            mHost->mProcessCpuTracker->GetStats(i, (IProcessCpuTrackerStats**)&st);
            Int64 vsize;
            st->GetVsize(&vsize);
            if (vsize > 0) {
                Int32 pid;
                st->GetPid(&pid);
                Int64 pss = 0;
                debug->GetPss(pid, NULL, &pss);
                if (pss > 0) {
                    if (infoMap.Find(pid) == infoMap.End()) {
                        String name;
                        st->GetName(&name);
                        AutoPtr<ProcessMemInfo> mi = new ProcessMemInfo(name, pid,
                            ProcessList::NATIVE_ADJ, -1, String("native"), String(NULL));
                        mi->mPss = pss;
                        memInfos->Add((IProcessMemInfo*)mi.Get());
                    }
                }
            }
        }
    }

    Int64 totalPss = 0;
    memInfos->GetSize(&size);
    for (Int32 i = 0, N = size; i < N; i++) {
        AutoPtr<IInterface> item;
        mMemInfos->Get(i, (IInterface**)&item);
        AutoPtr<ProcessMemInfo> mi = (ProcessMemInfo*)IProcessMemInfo::Probe(item);
        if (mi->mPss == 0) {
            debug->GetPss(mi->mPid, NULL, &mi->mPss);
        }
        totalPss += mi->mPss;
    }

    AutoPtr<ProcessMemInfoComparator> comparator = new ProcessMemInfoComparator();
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(memInfos, comparator);

    StringBuilder tag(128);
    StringBuilder stack(128);
    tag.Append("Low on memory -- ");
    mHost->AppendMemBucket(tag, totalPss, String("total"), FALSE);
    mHost->AppendMemBucket(stack, totalPss, String("total"), TRUE);

    StringBuilder logBuilder(1024);
    logBuilder.Append("Low on memory:\n");

    Boolean firstLine = TRUE;
    Int32 lastOomAdj = Elastos::Core::Math::INT32_MIN_VALUE;
    for (Int32 i = 0, N = size; i < N; i++) {
        AutoPtr<IInterface> item;
        mMemInfos->Get(i, (IInterface**)&item);
        AutoPtr<ProcessMemInfo> mi = (ProcessMemInfo*)IProcessMemInfo::Probe(item);

        if (mi->mOomAdj != ProcessList::NATIVE_ADJ
            && (mi->mOomAdj < ProcessList::SERVICE_ADJ
            || mi->mOomAdj == ProcessList::HOME_APP_ADJ
            || mi->mOomAdj == ProcessList::PREVIOUS_APP_ADJ)) {
            if (lastOomAdj != mi->mOomAdj) {
                lastOomAdj = mi->mOomAdj;
                if (mi->mOomAdj <= ProcessList::FOREGROUND_APP_ADJ) {
                    tag.Append(" / ");
                }
                if (mi->mOomAdj >= ProcessList::FOREGROUND_APP_ADJ) {
                    if (firstLine) {
                        stack.Append(":");
                        firstLine = FALSE;
                    }
                    stack.Append("\n\t at ");
                }
                else {
                    stack.Append("$");
                }
            }
            else {
                tag.Append(" ");
                stack.Append("$");
            }
            if (mi->mOomAdj <= ProcessList::FOREGROUND_APP_ADJ) {
                AppendMemBucket(tag, mi->mPss, mi->mName, FALSE);
            }
            AppendMemBucket(stack, mi->mPss, mi->mName, TRUE);
            AutoPtr<ProcessMemInfo> nextMi;
            if (i + 1 < N) {
                item = NULL;
                mMemInfos->Get(i + 1, (IInterface**)&item);
                nextMi = (ProcessMemInfo*)IProcessMemInfo::Probe(item);
            }
            if (mi->mOomAdj >= ProcessList::FOREGROUND_APP_ADJ
                && ((i+1) >= N || nextMi->mOomAdj != lastOomAdj)) {
                stack.Append("(");
                for (Int32 k = 0; k < DUMP_MEM_OOM_ADJ->GetLength(); k++) {
                    if ((*DUMP_MEM_OOM_ADJ)[k] == mi->mOomAdj) {
                        stack.Append((*DUMP_MEM_OOM_LABEL)[k]);
                        stack.Append(":");
                        stack.Append((*DUMP_MEM_OOM_ADJ)[k]);
                    }
                }
                stack.Append(")");
            }
        }

        logBuilder.Append("  ");
        logBuilder.Append(ProcessList::MakeOomAdjString(mi->mOomAdj));
        logBuilder.AppendChar(' ');
        logBuilder.Append(ProcessList::MakeProcStateString(mi->mProcState));
        logBuilder.AppendChar(' ');
        ProcessList::AppendRamKb(&logBuilder, mi->mPss);
        logBuilder.Append(" kB: ");
        logBuilder.Append(mi->mName);
        logBuilder.Append(" (");
        logBuilder.Append(mi->mPid);
        logBuilder.Append(") ");
        logBuilder.Append(mi->mAdjType);
        logBuilder.AppendChar('\n');
        if (mi->mAdjReason != NULL) {
            logBuilder.Append("                      ");
            logBuilder.Append(mi->mAdjReason);
            logBuilder.AppendChar('\n');
        }
    }

    logBuilder.Append("           ");
    ProcessList::AppendRamKb(&logBuilder, totalPss);
    logBuilder.Append(" kB: TOTAL\n");

    AutoPtr<ArrayOf<Int64> > infos = ArrayOf<Int64>::Alloc(IDebug::MEMINFO_COUNT);
    debug->GetMemInfo(infos);
    logBuilder.Append("  MemInfo: ");
    logBuilder.Append((*infos)[IDebug::MEMINFO_SLAB]);
    logBuilder.Append(" kB slab, ");
    logBuilder.Append((*infos)[IDebug::MEMINFO_SHMEM]);
    logBuilder.Append(" kB shmem, ");
    logBuilder.Append((*infos)[IDebug::MEMINFO_BUFFERS]);
    logBuilder.Append(" kB buffers, ");
    logBuilder.Append((*infos)[IDebug::MEMINFO_CACHED]);
    logBuilder.Append(" kB cached, ");
    logBuilder.Append((*infos)[IDebug::MEMINFO_FREE]);
    logBuilder.Append(" kB free\n");
    if ((*infos)[IDebug::MEMINFO_ZRAM_TOTAL] != 0) {
        logBuilder.Append("  ZRAM: ");
        logBuilder.Append((*infos)[IDebug::MEMINFO_ZRAM_TOTAL]);
        logBuilder.Append(" kB RAM, ");
        logBuilder.Append((*infos)[IDebug::MEMINFO_SWAP_TOTAL]);
        logBuilder.Append(" kB swap total, ");
        logBuilder.Append((*infos)[IDebug::MEMINFO_SWAP_FREE]);
        logBuilder.Append(" kB swap free\n");
    }
    Slogger::I(TAG, logBuilder.ToString());

    StringBuilder dropBuilder(1024);
    /*
    StringWriter oomSw = new StringWriter();
    PrintWriter oomPw = new FastPrintWriter(oomSw, FALSE, 256);
    StringWriter catSw = new StringWriter();
    PrintWriter catPw = new FastPrintWriter(catSw, FALSE, 256);
    String[] emptyArgs = new String[] { };
    dumpApplicationMemoryUsage(NULL, oomPw, "  ", emptyArgs, TRUE, catPw);
    oomPw.flush();
    String oomString = oomSw.toString();
    */
    dropBuilder.Append(stack.ToString());
    dropBuilder.AppendChar('\n');
    dropBuilder.AppendChar('\n');
    dropBuilder.Append(logBuilder.ToString());
    dropBuilder.AppendChar('\n');
    /*
    dropBuilder.Append(oomString);
    dropBuilder.AppendChar('\n');
    */
    AutoPtr<IWriter> catSw;
    CStringWriter::New((IWriter**)&catSw);
    {
        AutoLock syncLock(mHost);
        AutoPtr<IPrintWriter> catPw;
        CFastPrintWriter::New(catSw, FALSE, 256, (IPrintWriter**)&catPw);
        AutoPtr<ArrayOf<String> > emptyArgs = ArrayOf<String>::Alloc(0);
        catPw->Println();
        mHost->DumpProcessesLocked(NULL, catPw, emptyArgs, 0, FALSE, String(NULL));
        catPw->Println();
        mHost->mServices->DumpServicesLocked(NULL, catPw, emptyArgs, 0,
                FALSE, FALSE, String(NULL));
        catPw->Println();
        mHost->DumpActivitiesLocked(NULL, catPw, emptyArgs, 0, FALSE, FALSE, String(NULL));
        IFlushable::Probe(catPw)->Flush();
    }
    String str = Object::ToString(catSw);
    dropBuilder.Append(str);
    mHost->AddErrorToDropBox(String("lowmem"), NULL, String("system_server"), NULL,
        NULL, tag.ToString(), dropBuilder.ToString(), NULL, NULL);
    //Slogger::I(TAG, "Sent to dropbox:");
    //Slogger::I(TAG, dropBuilder.toString());
    {
        AutoLock syncLock(mHost);
        Int64 now = SystemClock::GetUptimeMillis();
        if (mHost->mLastMemUsageReportTime < now) {
            mHost->mLastMemUsageReportTime = now;
        }
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::DumpStackTracesFileObserver
//==============================================================================

ECode CActivityManagerService::DumpStackTracesFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    AutoLock lock(this);
    Notify();
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ShowRunnable
//==============================================================================

ECode CActivityManagerService::ShowRunnable::Run()
{
    AutoLock lock(mHost);
    IDialogInterface::Probe(mDialog)->Dismiss();
    mHost->mLaunchWarningShown = FALSE;
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ShowLaunchWarningLockedRunnable
//==============================================================================

ECode CActivityManagerService::ShowLaunchWarningLockedRunnable::Run()
{
    AutoLock lock(mHost);
    AutoPtr<LaunchWarningWindow> lwindow = new LaunchWarningWindow(mHost->GetUiContext(), mCur, mNext);
    AutoPtr<IDialog> d = IDialog::Probe(lwindow);
    d->Show();
    AutoPtr<IRunnable> runnable = new ShowRunnable(mHost, d);
    Boolean result;
    mHost->mHandler->PostDelayed(runnable, 4000, &result);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::LockTaskRunnable
//==============================================================================

CActivityManagerService::LockTaskRunnable::LockTaskRunnable(
    /* [in] */ TaskRecord* task,
    /* [in] */ LockToAppRequestDialog* lockToAppRequest)
    : mTaskRecord(task)
    , mLockToAppRequest(lockToAppRequest)
{}

ECode CActivityManagerService::LockTaskRunnable::Run()
{
    mLockToAppRequest->ShowLockTaskPrompt(mTaskRecord);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::InputDispatchingTimedOutRunnable
//==============================================================================

ECode CActivityManagerService::InputDispatchingTimedOutRunnable::Run()
{
    mHost->AppNotResponding(mProc, mActivity, mParent, mAboveSystem, mAnnotation);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::SetProcessForegroundToken
//==============================================================================

ECode CActivityManagerService::SetProcessForegroundToken::ProxyDied()
{
    return mHost->ForegroundTokenDied(this);
}

//==============================================================================
// CActivityManagerService::HangWhoDeathRecipient
//==============================================================================

CAR_INTERFACE_IMPL(CActivityManagerService::HangWhoDeathRecipient, Object, IProxyDeathRecipient)

ECode CActivityManagerService::HangWhoDeathRecipient::ProxyDied()
{
    AutoLock lock(this);
    return NotifyAll();
}

//==============================================================================
// CActivityManagerService::RestartBroadcastReceiver
//==============================================================================

ECode CActivityManagerService::RestartBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Now the broadcast is done, finish up the low-level shutdown.
    Logger::I(TAG, "Shutting down activity manager...");
    Boolean res;
    mHost->Shutdown(10000, &res);
    Logger::I(TAG, "Shutdown complete, restarting!");
    Process::KillProcess(Process::MyPid());
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->Exit(10);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::PreBootCompletedReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::PreBootCompletedReceiver, Object, IIntentReceiver, IBinder)

ECode CActivityManagerService::PreBootCompletedReceiver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IRunnable* onFinishCallback)
{
    mHandler = handler;
    mOnFinishCallback = onFinishCallback;
    return NOERROR;
}

ECode CActivityManagerService::PreBootCompletedReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // The raw IIntentReceiver interface is called
    // with the AM lock held, so redispatch to
    // execute our code without the lock.
    Boolean res;
    return mHandler->Post(mOnFinishCallback, &res);
}

ECode CActivityManagerService::PreBootCompletedReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::OnFinishCallback
//==============================================================================
CActivityManagerService::OnFinishCallback::OnFinishCallback(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ List<AutoPtr<IComponentName> >* doneReceivers,
    /* [in] */ IRunnable* goingCallback)
    : mHost(host)
    , mDoneReceivers(doneReceivers)
    , mGoingCallback(goingCallback)
{}

ECode CActivityManagerService::OnFinishCallback::Run()
{
    {
        AutoLock syncLock(mHost);
        mHost->mDidUpdate = TRUE;
    }
    mHost->WriteLastDonePreBootReceivers(mDoneReceivers);
    AutoPtr<ICharSequence> text;
    mHost->mContext->GetText(R::string::android_upgrading_complete,
        (ICharSequence**)&text);
    mHost->ShowBootMessage(text, FALSE);
    mHost->SystemReady(mGoingCallback);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ErrorMsgButtonOnClickListener
//==============================================================================

ECode CActivityManagerService::ErrorMsgButtonOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(
        CActivityManagerService::IM_FEELING_LUCKY_MSG, &result);
}

CAR_INTERFACE_IMPL(CActivityManagerService::ErrorMsgButtonOnClickListener, Object, IDialogInterfaceOnClickListener)

//==============================================================================
// CActivityManagerService::SystemReadyReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::SystemReadyReceiver, Object, IIntentReceiver, IBinder)

ECode CActivityManagerService::SystemReadyReceiver::constructor()
{
    return NOERROR;
}

ECode CActivityManagerService::SystemReadyReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return E_REMOTE_EXCEPTION;
}

ECode CActivityManagerService::SystemReadyReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::DropBoxTagThread
//==============================================================================

CActivityManagerService::DropBoxTagThread::DropBoxTagThread(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ StringBuilder* sb,
    /* [in] */ IDropBoxManager* dbox,
    /* [in] */ const String& tag)
    : mHost(host)
    , mSb(sb)
    , mDbox(dbox)
    , mTag(tag)
{
    Thread::constructor(String("Error dump: ") + tag);
}

ECode CActivityManagerService::DropBoxTagThread::Run()
{
    String report;
    {    AutoLock syncLock(mSb);
        report = mSb->ToString();
        mSb->Delete(0, mSb->GetLength());
        mSb->TrimToSize();
    }
    if (report.GetLength() != 0) {
        mDbox->AddText(mTag, report);
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::DropBoxTagThread2
//==============================================================================

CActivityManagerService::DropBoxTagThread2::DropBoxTagThread2(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ IDropBoxManager* dbox,
    /* [in] */ const String& tag)
    : mHost(host)
    , mDbox(dbox)
    , mTag(tag)
{
    Thread::constructor(String("Error dump: ") + tag);
}

ECode CActivityManagerService::DropBoxTagThread2::Run()
{
    // 5 second sleep to let stacks arrive and be batched together
    // try {
    Thread::Sleep(5000);  // 5 seconds
    // } catch (InterruptedException e) {}

    String errorReport;
    {
        AutoLock lock(mHost->mStrictModeBuffer);
        errorReport = mHost->mStrictModeBuffer->ToString();
        if (errorReport.GetLength() == 0) {
            return NOERROR;
        }
        mHost->mStrictModeBuffer->Delete(0, mHost->mStrictModeBuffer->GetLength());
        mHost->mStrictModeBuffer->TrimToSize();
    }
    mDbox->AddText(mTag, errorReport);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::HandleApplicationWtfRunnable
//==============================================================================

CActivityManagerService::HandleApplicationWtfRunnable::HandleApplicationWtfRunnable(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ IBinder* app,
    /* [in] */ const String& tag,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
    : mHost(host)
    , mCallingUid(callingUid)
    , mCallingPid(callingPid)
    , mApp(app)
    , mTag(tag)
    , mCrashInfo(crashInfo)
{}

ECode CActivityManagerService::HandleApplicationWtfRunnable::Run()
{
    mHost->HandleApplicationWtfInner(mCallingUid, mCallingPid, mApp, mTag, mCrashInfo);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::WorkerThread
//==============================================================================

CActivityManagerService::WorkerThread::WorkerThread(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ const String& dropboxTag,
    /* [in] */ const String& report,
    /* [in] */ StringBuilder* sb,
    /* [in] */ IFile* logFile,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
    /* [in] */ IDropBoxManager* dbox)
    : mHost(host)
    , mDropboxTag(dropboxTag)
    , mReport(report)
    , mSb(sb)
    , mLogFile(logFile)
    , mCrashInfo(crashInfo)
    , mDbox(dbox)
{
    Thread::constructor(String("Error dump: ") + dropboxTag);
}

ECode CActivityManagerService::WorkerThread::Run()
{
    if (mReport != NULL) {
         mSb->Append(mReport);
    }
    if (mLogFile != NULL) {
        // try {
        String str;
        FileUtils::ReadTextFile(mLogFile, DROPBOX_MAX_SIZE, String("\n\n[[TRUNCATED]]"), &str);
        mSb->Append(str);
        // } catch (IOException e) {
        //     Slogger::e(TAG, "Error reading " + logFile, e);
        // }
    }
    String trace;
    if (mCrashInfo != NULL) {
        mCrashInfo->GetStackTrace(&trace);
        if (!trace.IsNull()) mSb->Append(trace);
    }

    String setting = ISettingsGlobal::ERROR_LOGCAT_PREFIX + mDropboxTag;
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 lines;
    Settings::Global::GetInt32(resolver, setting, 0, &lines);
    if (lines > 0) {
        mSb->Append("\n");

        // Merge several logcat streams, and take the last N lines
        // AutoPtr<IInputStreamReader> input;
        Slogger::E(TAG, "// TODO: WorkerThread::Run() %s", mSb->ToString().string());
    //TODO:
    //     try {
    //         java.lang.Process logcat = new ProcessBuilder("/system/bin/logcat",
    //                 "-v", "time", "-b", "events", "-b", "system", "-b", "main",
    //                 "-b", "crash",
    //                 "-t", String.valueOf(lines)).redirectErrorStream(TRUE).start();

    //         try { logcat.getOutputStream().close(); } catch (IOException e) {}
    //         try { logcat.getErrorStream().close(); } catch (IOException e) {}
    //         input = new InputStreamReader(logcat.getInputStream());

    //         Int32 num;
    //         char[] buf = new char[8192];
    //         while ((num = input.read(buf)) > 0) sb.Append(buf, 0, num);
    //     } catch (IOException e) {
    //         Slogger::e(TAG, "Error running logcat", e);
    //     } finally {
    //         if (input != NULL) try { input.close(); } catch (IOException e) {}
    //     }
    }

    mDbox->AddText(mDropboxTag, mSb->ToString());
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::WriteStateRunnable
//==============================================================================

ECode CActivityManagerService::WriteStateRunnable::Run()
{
    {    AutoLock syncLock(mHost);
        mHost->mProcessStats->WriteStateAsyncLocked();
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::StopUserLockedRunnable
//==============================================================================

ECode CActivityManagerService::StopUserLockedRunnable::Run()
{
    // try {
    mCallback->UserStopped(mUserId);
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::DispatchUserSwitchCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::DispatchUserSwitchCallback, Object, IIRemoteCallback, IBinder)

ECode CActivityManagerService::DispatchUserSwitchCallback::constructor(
    /* [in] */ IIActivityManager* host,
    /* [in] */ Int32 n,
    /* [in] */ IUserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    mCount = 0;
    N = n;
    mOldUserId = oldUserId;
    mNewUserId = newUserId;
    return NOERROR;
}

CActivityManagerService::DispatchUserSwitchCallback::SendResult(
    /* [in] */ IBundle* data)
{
    AutoLock lock(mHost);
    if (mHost->mCurUserSwitchCallback.Get() == TO_IINTERFACE(this)) {
        mCount++;
        if (mCount == N) {
            mHost->SendContinueUserSwitchLocked(mUss, mOldUserId, mNewUserId);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::DispatchUserSwitchCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CActivityManagerService::DispatchUserSwitchCallback";
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::MainHandler
//==============================================================================

CActivityManagerService::MainHandler::MainHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CActivityManagerService* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{
}

ECode CActivityManagerService::MainHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case CActivityManagerService::SHOW_ERROR_MSG: {
            AutoPtr<StringObjectHashMap> data = reinterpret_cast<StringObjectHashMap*>(
                obj->Probe(EIID_StringObjectHashMap));
            assert(data != NULL);
            mHost->HandleShowErrorMsg(data);
        }
        break;
        case CActivityManagerService::SHOW_NOT_RESPONDING_MSG: {
            AutoPtr<StringObjectHashMap> data = reinterpret_cast<StringObjectHashMap*>(
                obj->Probe(EIID_StringObjectHashMap));
            assert(data != NULL);
            mHost->HandleShowNotRespondingMsg(data, arg1);
        }
        break;
        case CActivityManagerService::SHOW_STRICT_MODE_VIOLATION_MSG: {
            AutoPtr<StringObjectHashMap> data = reinterpret_cast<StringObjectHashMap*>(
                obj->Probe(EIID_StringObjectHashMap));
            assert(data != NULL);
            mHost->HandleShowStrictModeViolationMsg(data);
        }
        break;
        case CActivityManagerService::SHOW_FACTORY_ERROR_MSG: {
            AutoPtr<ICharSequence> errorInfo = ICharSequence::Probe(obj.Get());
            mHost->HandleShowFactoryErrorMsg(errorInfo);
        }
        break;
        case CActivityManagerService::UPDATE_CONFIGURATION_MSG: {
            AutoPtr<IConfiguration> config = IConfiguration::Probe(obj.Get());
            mHost->HandleUpdateConfigurationMsg(config);
        }
        break;
        case CActivityManagerService::GC_BACKGROUND_PROCESSES_MSG: {
            mHost->HandleGCBackgroundProcessesMsg();
        }
        break;
        case CActivityManagerService::WAIT_FOR_DEBUGGER_MSG: {
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj);
            mHost->HandleWaitForDebuggerMsg(pr, arg1);
        }
        break;
        case CActivityManagerService::SERVICE_TIMEOUT_MSG: {
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj);
            mHost->HandleServiceTimeoutMsg(pr);
        }
        break;
        case CActivityManagerService::UPDATE_TIME_ZONE: {
            mHost->HandleUpdateTimeZone();
        }
        break;
        case CActivityManagerService::CLEAR_DNS_CACHE_MSG: {
            mHost->HandleClearDnsCache();
        }
        break;
        case CActivityManagerService::UPDATE_HTTP_PROXY_MSG: {
            AutoPtr<IProxyInfo> proxy = IProxyInfo::Probe(obj);
            mHost->HandleUpdateHttpProxy(proxy);
        }
        break;
        case CActivityManagerService::SHOW_UID_ERROR_MSG: {
            mHost->HandleShowUidErrorMsg();
        }
        break;
        case CActivityManagerService::IM_FEELING_LUCKY_MSG: {
            mHost->HandleIMFeelingLuckyMsg();
        }
        break;
        case CActivityManagerService::PROC_START_TIMEOUT_MSG: {
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj);
            mHost->HandleProcStartTimeoutMsg(pr);
        }
        break;
        case CActivityManagerService::DO_PENDING_ACTIVITY_LAUNCHES_MSG: {
            mHost->HandleDoPendingActivityLaunchesMsg();
        }
        break;
        case CActivityManagerService::KILL_APPLICATION_MSG: {
            AutoPtr<IBundle> bundle = IBundle::Probe(obj);
            mHost->HandleKillApplicationMsg(arg1, arg2, bundle);
        }
        break;
        case CActivityManagerService::FINALIZE_PENDING_INTENT_MSG: {
            AutoPtr<IIIntentSender> pir = IIIntentSender::Probe(obj);
            assert(pir);
            ((CPendingIntentRecord*)pir.Get())->CompleteFinalize();
        }
        break;
        case CActivityManagerService::POST_HEAVY_NOTIFICATION_MSG: {
            AutoPtr<ActivityRecord> ar = (ActivityRecord*)IActivityRecord::Probe(obj);
            assert(ar != NULL);
            mHost->HandlePostHeavyNotificationMsg(ar);
        }
        break;
        case CActivityManagerService::CANCEL_HEAVY_NOTIFICATION_MSG: {
            mHost->HandleCancelHeavyNotificationMsg(arg1);
        }
        break;
        case CActivityManagerService::CHECK_EXCESSIVE_WAKE_LOCKS_MSG: {
            mHost->HandleCheckExcessiveWakeLocksMsg();
        }
        break;
        case CActivityManagerService::SHOW_COMPAT_MODE_DIALOG_MSG: {
            AutoPtr<ActivityRecord> ar = (ActivityRecord*)IActivityRecord::Probe(obj);
            mHost->HandleShowCompatModeDialogMsg(ar);
        }
        break;
        case CActivityManagerService::DISPATCH_PROCESSES_CHANGED: {
            mHost->DispatchProcessesChanged();
        }
        break;
        case CActivityManagerService::DISPATCH_PROCESS_DIED: {
            mHost->DispatchProcessDied(arg1, arg2);
        }
        break;
        case CActivityManagerService::REPORT_MEM_USAGE_MSG: {
            AutoPtr<IList> memInfos = IList::Probe(obj);
            mHost->HandleReportMemUsageMsg(memInfos);
        }
        break;
        case CActivityManagerService::START_USER_SWITCH_MSG: {
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            mHost->ShowUserSwitchDialog(arg1, str);
        }
        break;
        case CActivityManagerService::REPORT_USER_SWITCH_MSG: {
            AutoPtr<UserStartedState> state = (UserStartedState*)IUserStartedState::Probe(obj);
            mHost->DispatchUserSwitch(state, arg1, arg2);
        }
        break;
        case CActivityManagerService::CONTINUE_USER_SWITCH_MSG: {
            AutoPtr<UserStartedState> state = (UserStartedState*)IUserStartedState::Probe(obj);
            mHost->ContinueUserSwitch(state, arg1, arg2);
        }
        break;
        case CActivityManagerService::USER_SWITCH_TIMEOUT_MSG: {
            AutoPtr<UserStartedState> state = (UserStartedState*)IUserStartedState::Probe(obj);
            mHost->TimeoutUserSwitch(state, arg1, arg2);
        }
        break;
        case IMMERSIVE_MODE_LOCK_MSG: {
            Boolean nextState = (arg1 != 0);
            Boolean isHeld;
            mHost->mUpdateLock->IsHeld(&isHeld);
            if (isHeld != nextState) {
                if (DEBUG_IMMERSIVE) {
                    AutoPtr<ActivityRecord> r = (ActivityRecord*)IActivityRecord::Probe(obj);
                    Slogger::D(TAG, "Applying new update lock state '%d' for %s", nextState, TO_CSTR(r));
                }
                if (nextState) {
                    mHost->mUpdateLock->AcquireLock();
                }
                else {
                    mHost->mUpdateLock->ReleaseLock();
                }
            }
        }
        break;
        case PERSIST_URI_GRANTS_MSG: {
            mHost->WriteGrantedUriPermissions();
        }
        break;
        case REQUEST_ALL_PSS_MSG: {
            {    AutoLock syncLock(mHost);
                mHost->RequestPssAllProcsLocked(SystemClock::GetUptimeMillis(), TRUE, FALSE);
            }
        }
        break;
        case START_PROFILES_MSG: {
            {    AutoLock syncLock(mHost);
                mHost->StartProfilesLocked();
            }
        }
        break;
        case UPDATE_TIME: {
            {    AutoLock syncLock(mHost);
                List< AutoPtr<ProcessRecord> >::Iterator it = mHost->mLruProcesses.Begin();
                for (; it != mHost->mLruProcesses.End(); ++it) {
                    AutoPtr<ProcessRecord> r = *it;
                    if (r->mThread != NULL) {
                        if (FAILED(r->mThread->UpdateTimePrefs(arg1 == 0 ? FALSE : TRUE))) {
                            String processName;
                            r->mInfo->GetProcessName(&processName);
                            Slogger::W(TAG, "Failed to update preferences for: %s", processName.string());
                        }
                    }
                }
            }
        }
        break;
        case SYSTEM_USER_START_MSG: {
            mHost->mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_USER_RUNNING_START,
                StringUtils::ToString(arg1), arg1);
            mHost->mSystemServiceManager->StartUser(arg1);
        }
        break;
        case SYSTEM_USER_CURRENT_MSG: {
            mHost->mBatteryStatsService->NoteEvent(
                    IBatteryStatsHistoryItem::EVENT_USER_FOREGROUND_FINISH,
                    StringUtils::ToString(arg2), arg2);
            mHost->mBatteryStatsService->NoteEvent(
                    IBatteryStatsHistoryItem::EVENT_USER_FOREGROUND_START,
                    StringUtils::ToString(arg1), arg1);
            mHost->mSystemServiceManager->SwitchUser(arg1);
            mHost->mLockToAppRequest->ClearPrompt();
        }
        break;
        case ENTER_ANIMATION_COMPLETE_MSG: {
            {    AutoLock syncLock(mHost);
                AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(IBinder::Probe(obj));
                if (r != NULL && r->mApp != NULL && r->mApp->mThread != NULL) {
                    r->mApp->mThread->ScheduleEnterAnimationComplete(IBinder::Probe(r->mAppToken));
                }
            }
        }
        break;
        case FINISH_BOOTING_MSG: {
            if (arg1 != 0) {
                mHost->FinishBooting();
            }
            if (arg2 != 0) {
                mHost->EnableScreenAfterBoot();
            }
        }
        break;
        case SEND_LOCALE_TO_MOUNT_DAEMON_MSG: {
            AutoPtr<ILocale> l = ILocale::Probe(obj);
            AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
            AutoPtr<IIMountService> mountService = IIMountService::Probe(service);
            String tag;
            l->ToLanguageTag(&tag);
            Logger::D(TAG, "Storing locale %s for decryption UI", tag.string());
            if (FAILED(mountService->SetField(IStorageManager::SYSTEM_LOCALE_KEY, tag))) {
                Logger::E(TAG, "Error storing locale for decryption UI");
            }
        }
        break;
        case POST_PRIVACY_NOTIFICATION_MSG: {
            AutoPtr<INotificationManagerHelper> nmHelper;
            CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&nmHelper);
            AutoPtr<IINotificationManager> inm;
            nmHelper->GetService((IINotificationManager**)&inm);
            if (inm == NULL) {
                return NOERROR;
            }

            ActivityRecord* root = (ActivityRecord*)IActivityRecord::Probe(obj);
            AutoPtr<ProcessRecord> process = root->mApp;
            if (process == NULL) {
                return NOERROR;
            }

            String packageName;
            IPackageItemInfo::Probe(process->mInfo)->GetPackageName(&packageName);
            AutoPtr<IContext> context;
            if (FAILED(mHost->mContext->CreatePackageContext(packageName, 0, (IContext**)&context))) {
                Slogger::W(TAG, "Unable to create context for privacy guard notification");
            }
            else {
                AutoPtr<IPackageManager> pm;
                context->GetPackageManager((IPackageManager**)&pm);
                AutoPtr<IApplicationInfo> appInfo;
                context->GetApplicationInfo((IApplicationInfo**)&appInfo);
                AutoPtr<ICharSequence> label;
                IPackageItemInfo::Probe(appInfo)->LoadLabel(pm, (ICharSequence**)&label);
                AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
                (*formatArgs)[0] = label;
                String text;
                mHost->mContext->GetString(R::string::privacy_guard_notification_detail, formatArgs, &text);
                String title;
                mHost->mContext->GetString(R::string::privacy_guard_notification, &title);

                AutoPtr<IUriHelper> uriHelper;
                CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
                AutoPtr<IUri> uri;
                uriHelper->FromParts(String("package"), root->mPackageName, String(NULL), (IUri**)&uri);
                AutoPtr<IIntent> infoIntent;
                CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&infoIntent);

                AutoPtr<INotification> notification;
                CNotification::New((INotification**)&notification);
                notification->SetIcon(R::drawable::stat_notify_privacy_guard);
                notification->SetWhen(0);
                notification->SetFlags(INotification::FLAG_ONGOING_EVENT);
                notification->SetPriority(INotification::PRIORITY_LOW);
                notification->SetDefaults(0);
                notification->SetSound(NULL);
                notification->SetVibrate(NULL);
                AutoPtr<IUserHandle> userHandle;
                CUserHandle::New(root->mUserId, (IUserHandle**)&userHandle);
                AutoPtr<IPendingIntentHelper> helper;
                CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
                AutoPtr<IPendingIntent> pendingIntent;
                helper->GetActivityAsUser(mHost->mContext, 0, infoIntent,
                    IPendingIntent::FLAG_CANCEL_CURRENT, NULL,
                    userHandle, (IPendingIntent**)&pendingIntent);

                notification->SetLatestEventInfo(mHost->mContext, CoreUtils::Convert(title),
                    CoreUtils::Convert(text), pendingIntent);

                AutoPtr<ArrayOf<Int32> > outId = ArrayOf<Int32>::Alloc(1);
                AutoPtr<ArrayOf<Int32> > outIdTmp;
                ECode ec = inm->EnqueueNotificationWithTag(String("android"), String("android"), String(NULL),
                    R::string::privacy_guard_notification,
                    notification, outId, root->mUserId, (ArrayOf<Int32>**)&outIdTmp);
                outId = outIdTmp;
                if (FAILED(ec)) {
                    Slogger::W(TAG, "Error showing notification for privacy guard");
                }
            }
        }
        break;
        case CANCEL_PRIVACY_NOTIFICATION_MSG: {
            AutoPtr<INotificationManagerHelper> nmHelper;
            CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&nmHelper);
            AutoPtr<IINotificationManager> inm;
            nmHelper->GetService((IINotificationManager**)&inm);
            if (inm == NULL) {
                return NOERROR;
            }
            if (FAILED(inm->CancelNotificationWithTag(String("android"), String(NULL),
                R::string::privacy_guard_notification, arg1))) {
                Slogger::W(TAG, "Error canceling notification for service");
            }
        }
        break;
    }

    return NOERROR;
}

//==============================================================================
// CActivityManagerService::BgHandler
//==============================================================================

CActivityManagerService::BgHandler::BgHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CActivityManagerService* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{
}

ECode CActivityManagerService::BgHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case COLLECT_PSS_BG_MSG: {
            mHost->HandleCollectPssBgMsg();
        }
        break;
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::MyPackageMonitor
//==============================================================================
CActivityManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CActivityManagerService* host)
    : mHost(host)
{}

ECode CActivityManagerService::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    // Remove all tasks with activities in the specified package from the list of recent tasks
    Int32 eventUserId;
    GetChangingUserId(&eventUserId);
    {    AutoLock syncLock(mHost);
        List<AutoPtr<TaskRecord> >::ReverseIterator rit = mHost->mRecentTasks->RBegin();
        while (rit != mHost->mRecentTasks->REnd()) {
            AutoPtr<TaskRecord> tr = *rit;
            if (tr->mUserId != eventUserId) {
                ++rit;
                continue;
            }

            AutoPtr<IComponentName> cn;
            tr->mIntent->GetComponent((IComponentName**)&cn);
            String cnPackageName;
            cn->GetPackageName(&cnPackageName);
            if (cn != NULL && cnPackageName.Equals(packageName)) {
                // If the package name matches, remove the task and kill the process
                if (mHost->RemoveTaskByIdLocked(tr->mTaskId, IActivityManager::REMOVE_TASK_KILL_PROCESS)) {
                    continue;
                }
            }
            ++rit;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::MyPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    OnPackageModified(packageName);
    *result = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::MyPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    Int32 eventUserId;
    GetChangingUserId(&eventUserId);
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    List<AutoPtr<Pair<AutoPtr<IIntent>, Int32> > > recentTaskIntents;
    HashSet<AutoPtr<IComponentName> > componentsKnownToExist;
    List<Int32> tasksToRemove;
    // Copy the list of recent tasks so that we don't hold onto the lock on
    // ActivityManagerService for long periods while checking if components exist.
    {    AutoLock syncLock(mHost);
        List<AutoPtr<TaskRecord> >::Iterator it = mHost->mRecentTasks->Begin();
        for (; it != mHost->mRecentTasks->End(); ++it) {
            AutoPtr<TaskRecord> tr = *it;
            if (tr->mUserId != eventUserId)
                continue;
            AutoPtr<Pair<AutoPtr<IIntent>, Int32> > pair =
                new Pair<AutoPtr<IIntent>, Int32>(tr->mIntent, tr->mTaskId);
            recentTaskIntents.PushBack(pair);
        }
    }
    // Check the recent tasks and filter out all tasks with components that no longer exist.
    List<AutoPtr<Pair<AutoPtr<IIntent>, Int32> > >::Iterator rIt = recentTaskIntents.Begin();
    for (; rIt != recentTaskIntents.End(); ++ rIt) {
        AutoPtr<Pair<AutoPtr<IIntent>, Int32> > p = *rIt;
        AutoPtr<IComponentName> cn;
        p->mFirst->GetComponent((IComponentName**)&cn);
        String cnPackageName;
        cn->GetPackageName(&cnPackageName);
        if (cn != NULL && cnPackageName.Equals(packageName)) {
            if (componentsKnownToExist.Find(cn) != componentsKnownToExist.End()) {
                // If we know that the component still exists in the package, then skip
                continue;
            }
            // try {
                AutoPtr<IActivityInfo> info;
                pm->GetActivityInfo(cn, 0, eventUserId, (IActivityInfo**)&info);
                if (info != NULL) {
                    componentsKnownToExist.Insert(cn);
                }
                else {
                    tasksToRemove.PushBack(p->mSecond);
                }
            // } catch (RemoteException e) {
            //     Log.e(TAG, "Failed to query activity info for component: " + cn, e);
            // }
        }
    }
    // Prune all the tasks with removed components from the list of recent tasks
    {    AutoLock syncLock(mHost);
        List<Int32>::ReverseIterator rit = tasksToRemove.RBegin();
        for (; rit != tasksToRemove.REnd(); ++rit) {
            // Remove the task but don't kill the process (since other components in that
            // package may still be running and in the background)
            mHost->RemoveTaskByIdLocked(*rit, 0);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Force stop the specified packages
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &userId);
    if (packages != NULL) {
        for (Int32 i = 0; i < packages->GetLength(); i++) {
            String pkg = (*packages)[i];
            {
                AutoLock lock(mHost);
                if (mHost->ForceStopPackageLocked(pkg, -1, FALSE, FALSE, FALSE, FALSE, FALSE,
                        userId, String("finished booting"))) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::Lifecycle
//==============================================================================
ECode CActivityManagerService::Lifecycle::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context))
    return CActivityManagerService::NewByFriend(context, (CActivityManagerService**)&mService);
}

// @Override
ECode CActivityManagerService::Lifecycle::OnStart()
{
    return mService->Start();
}

AutoPtr<CActivityManagerService> CActivityManagerService::Lifecycle::GetService()
{
    return mService;
}

//==============================================================================
// CActivityManagerService::ShutdownReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::ShutdownReceiver, Object, IIntentReceiver, IBinder)

ECode CActivityManagerService::ShutdownReceiver::constructor(
    /* [in] */ IIActivityManager* host,
    /* [in] */ IUserStartedState* uss)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    return NOERROR;
}

ECode CActivityManagerService::ShutdownReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    mHost->FinishUserStop(mUss);
    return NOERROR;
}

ECode CActivityManagerService::ShutdownReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::StoppingReceiver
//==============================================================================

CAR_INTERFACE_IMPL_2(CActivityManagerService::StoppingReceiver, Object, IIntentReceiver, IBinder)

ECode CActivityManagerService::StoppingReceiver::constructor(
    /* [in] */ IIActivityManager* host,
    /* [in] */ IUserStartedState* uss,
    /* [in] */ IIntent* shutdownIntent,
    /* [in] */ IIntentReceiver* shutdownReceiver,
    /* [in] */ Int32 userId)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    mShutdownIntent = shutdownIntent;
    mShutdownReceiver = shutdownReceiver;
    mUserId = userId;
    return NOERROR;
}

ECode CActivityManagerService::StoppingReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // On to the next.
    {
        AutoLock lock(mHost);
        if (mUss->mState != UserStartedState::STATE_STOPPING) {
            // Whoops, we are being started back up.  Abort, abort!
            return NOERROR;
        }
        mUss->mState = UserStartedState::STATE_SHUTDOWN;
    }
    mHost->mBatteryStatsService->NoteEvent(
            IBatteryStatsHistoryItem::EVENT_USER_RUNNING_FINISH,
            StringUtils::ToString(mUserId), mUserId);
    mHost->mSystemServiceManager->StopUser(mUserId);
    Int32 result;
    String nullStr;
    mHost->BroadcastIntentLocked(NULL, nullStr, mShutdownIntent,
            nullStr, mShutdownReceiver, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
            TRUE, FALSE, CActivityManagerService::MY_PID, IProcess::SYSTEM_UID, mUserId, &result);
    return NOERROR;
}

ECode CActivityManagerService::StoppingReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService::LocalService
//==============================================================================

CAR_INTERFACE_IMPL(CActivityManagerService::LocalService, Object, IActivityManagerInternal)

ECode CActivityManagerService::LocalService::GoingToSleep()
{
    return mHost->GoingToSleep();
}

ECode CActivityManagerService::LocalService::WakingUp()
{
    return mHost->WakingUp();
}

ECode CActivityManagerService::LocalService::StartIsolatedProcess(
    /* [in] */ const String& entryPoint,
    /* [in] */ ArrayOf<String>* entryPointArgs,
    /* [in] */ const String& processName,
    /* [in] */ const String& abiOverride,
    /* [in] */ Int32 uid,
    /* [in] */ IRunnable* crashHandler,
    /* [in] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->StartIsolatedProcess(entryPoint, entryPointArgs,
            processName, abiOverride, uid, crashHandler);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::BatteryCallback
//==============================================================================
CAR_INTERFACE_IMPL(CActivityManagerService::BatteryCallback, Object, IBatteryCallback)

ECode CActivityManagerService::BatteryCallback::BatteryNeedsCpuUpdate()
{
    return mHost->BatteryNeedsCpuUpdate();
}

ECode CActivityManagerService::BatteryCallback::BatteryPowerChanged(
    /* [in] */ Boolean onBattery)
{
    return mHost->BatteryPowerChanged(onBattery);
}

//==============================================================================
// CActivityManagerService::AppTaskImpl
//==============================================================================

CAR_INTERFACE_IMPL(CActivityManagerService::AppTaskImpl, Object, IIAppTask)

CAR_OBJECT_IMPL(CAppTaskImpl)

ECode CActivityManagerService::AppTaskImpl::constructor(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 callingUid,
    /* [in] */ IIActivityManager* host)
{
    mTaskId = taskId;
    mCallingUid = callingUid;
    mHost = (CActivityManagerService*)host;
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::CheckCaller()
{
    if (mCallingUid != Binder::GetCallingUid()) {
        Slogger::E(TAG, "Caller %d does not match caller of getAppTasks(): %d",
            mCallingUid, Binder::GetCallingUid());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::FinishAndRemoveTask()
{
    FAIL_RETURN(CheckCaller());

    {    AutoLock syncLock(mHost);
        Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<TaskRecord> tr = mHost->RecentTaskForIdLocked(mTaskId);
        if (tr == NULL) {
            Slogger::E(TAG, "Unable to find task ID %d", mTaskId);
            Binder::RestoreCallingIdentity(origId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Only kill the process if we are not a new document
        Int32 flags;
        tr->GetBaseIntent()->GetFlags(&flags);
        Boolean isDocument = (flags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) ==
                IIntent::FLAG_ACTIVITY_NEW_DOCUMENT;
        mHost->RemoveTaskByIdLocked(mTaskId,
                !isDocument ? IActivityManager::REMOVE_TASK_KILL_PROCESS : 0);
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::GetTaskInfo(
    /* [out] */ IActivityManagerRecentTaskInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    FAIL_RETURN(CheckCaller());

    {    AutoLock syncLock(mHost);
        Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<TaskRecord> tr = mHost->RecentTaskForIdLocked(mTaskId);
        if (tr == NULL) {
            Slogger::E(TAG, "Unable to find task ID %d", mTaskId);
            Binder::RestoreCallingIdentity(origId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IActivityManagerRecentTaskInfo> temp = mHost->CreateRecentTaskInfoFromTaskRecord(tr);
        *info = temp;
        REFCOUNT_ADD(*info);
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::MoveToFront()
{
    FAIL_RETURN(CheckCaller());

    AutoPtr<TaskRecord> tr;
    {    AutoLock syncLock(mHost);
        tr = mHost->RecentTaskForIdLocked(mTaskId);
        if (tr == NULL) {
            Slogger::E(TAG, "Unable to find task ID %d", mTaskId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (tr->GetRootActivity() != NULL) {
            mHost->MoveTaskToFrontLocked(tr->mTaskId, 0, NULL);
            return NOERROR;
        }
    }

    Int32 res;
    mHost->StartActivityFromRecentsInner(tr->mTaskId, NULL, &res);
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::StartActivity(
    /* [in] */ IBinder* whoThread,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckCaller());

    Int32 callingUser = UserHandle::GetCallingUserId();
    AutoPtr<TaskRecord> tr;
    AutoPtr<IApplicationThread> appThread;
    {    AutoLock syncLock(mHost);
        tr = mHost->RecentTaskForIdLocked(mTaskId);
        if (tr == NULL) {
            Slogger::E(TAG, "Unable to find task ID %d", mTaskId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        appThread =IApplicationThread::Probe(whoThread);
        if (appThread == NULL) {
            Slogger::E(TAG, "Bad app thread %s", TO_CSTR(appThread));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return mHost->mStackSupervisor->StartActivityMayWait(appThread, -1, callingPackage, intent,
            resolvedType, NULL, NULL, NULL, String(NULL), 0, 0, NULL, NULL,
            NULL, options, callingUser, NULL, tr, result);
}

ECode CActivityManagerService::AppTaskImpl::SetExcludeFromRecents(
    /* [in] */ Boolean exclude)
{
    FAIL_RETURN(CheckCaller());

    {    AutoLock syncLock(mHost);
        Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<TaskRecord> tr = mHost->RecentTaskForIdLocked(mTaskId);
        if (tr == NULL) {
            Slogger::E(TAG, "Unable to find task ID %d", mTaskId);
            Binder::RestoreCallingIdentity(origId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IIntent> intent = tr->GetBaseIntent();
        if (exclude) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        }
        else {
            Int32 flags;
            intent->GetFlags(&flags);
            intent->SetFlags(flags & ~IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        }
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::AppTaskImpl::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// CActivityManagerService
//==============================================================================

CAR_INTERFACE_IMPL_4(CActivityManagerService, Object, IIActivityManager,
    IWatchdogMonitor, IBatteryCallback, IBinder)

CAR_OBJECT_IMPL(CActivityManagerService)

CActivityManagerService::CActivityManagerService()
    : mLastAddedTaskUid(0)
    , mNextIsolatedProcessUid(0)
    , mLruProcessActivityStart(0)
    , mLruProcessServiceStart(0)
    , mLastFullPssTime(SystemClock::GetUptimeMillis())
    , mFullPssPending(FALSE)
    , mConfigurationSeq(0)
    , GL_ES_VERSION(0)
    , mTopAction(IIntent::ACTION_MAIN)
    , mProcessesReady(FALSE)
    , mSystemReady(FALSE)
    , mBooting(FALSE)
    , mCallFinishBooting(FALSE)
    , mBootAnimationComplete(FALSE)
    , mWaitingUpdate(FALSE)
    , mDidUpdate(FALSE)
    , mOnBattery(FALSE)
    , mLaunchWarningShown(FALSE)
    , mFactoryTest(0)
    , mCheckedForSetup(FALSE)
    , mAppSwitchesAllowedTime(0)
    , mDidAppSwitch(FALSE)
    , mLastPowerCheckRealtime(0)
    , mShuttingDown(FALSE)
    , mAdjSeq(0)
    , mLruSeq(0)
    , mNumNonCachedProcs(0)
    , mNumCachedHiddenProcs(0)
    , mNumServiceProcs(0)
    , mNewNumAServiceProcs(0)
    , mNewNumServiceProcs(0)
    , mAllowLowerMemLevel(FALSE)
    , mLastMemoryLevel(IProcessStats::ADJ_MEM_FACTOR_NORMAL)
    , mLastNumProcesses(0)
    , mLastIdleTime(SystemClock::GetUptimeMillis())
    , mLowRamTimeSinceLastIdle(0)
    , mLowRamStartTime(0)
    , mDidDexOpt(FALSE)
    , mSafeMode(FALSE)
    , mWaitForDebugger(FALSE)
    , mDebugTransient(FALSE)
    , mOrigWaitForDebugger(FALSE)
    , mAlwaysFinishActivities(FALSE)
    , mSamplingInterval(0)
    , mAutoStopProfiler(FALSE)
    , mProfileType(0)
    , mLastWriteTime(0)
    , mBooted(FALSE)
    , mProcessLimit(ProcessList::MAX_CACHED_APPS)
    , mProcessLimitOverride(-1)
    , mCurrentUserId(0)
    , mTargetUserId(IUserHandle::USER_NULL)
    , mLastMemUsageReportTime(0)
    , mHasRecents(FALSE)
    , mThumbnailWidth(0)
    , mThumbnailHeight(0)
    , mShowDialogs(TRUE)
    , mSleeping(FALSE)
    , mRunningVoice(FALSE)
    , mWentToSleep(FALSE)
    , mLockScreenShown(FALSE)
    , mCurResumedUid(-1)
{
    CArrayList::New((IArrayList**)&mProcessesOnHold);
    CArrayList::New((IArrayList**)&mPersistentStartingProcesses);
    CArrayList::New((IArrayList**)&mRemovedProcesses);
    CAtomicInteger64::New(0, (IAtomicInteger64**)&mLastCpuTime);
    CAtomicBoolean::New(TRUE, (IAtomicBoolean**)&mProcessCpuMutexFree);
    mProcessList = new ProcessList();

    mStrictModeBuffer = new StringBuilder();
    mStringBuilder = new StringBuilder(256);
    mProcessNames = new ProcessMap< AutoPtr<ProcessRecord> >();
    mProcessCrashTimes = new ProcessMap<Int64>();
    mBadProcesses = new ProcessMap<AutoPtr<BadProcessInfo> >();
    mForegroundPackages = new ProcessMap<AutoPtr<List<AutoPtr<ProcessRecord> > > >();

    mStartedUserArray = ArrayOf<Int32>::Alloc(1);
    (*mStartedUserArray)[0] = 0;

    CRemoteCallbackList::New((IRemoteCallbackList**)&mUserSwitchObservers);

    CRemoteCallbackList::New((IRemoteCallbackList**)&mProcessObservers);

    CProcessCpuTracker::New(MONITOR_THREAD_CPU_USAGE, (IProcessCpuTracker**)&mProcessCpuTracker);

    mActiveProcessChanges = ArrayOf<ProcessChangeItem*>::Alloc(5);

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->GetInt32(String("ro.opengles.version"), IConfigurationInfo::GL_ES_VERSION_UNDEFINED, &GL_ES_VERSION);

    CConfiguration::New((IConfiguration**)&mConfiguration);

    mBroadcastQueues = ArrayOf<BroadcastQueue*>::Alloc(2);

    mReceiverResolver = new ReceiverResolver();

    CUpdateLock::New(String("immersive"), (IUpdateLock**)&mUpdateLock);

    mCurrentProfileIds = ArrayOf<Int32>::Alloc(1);
    (*mCurrentProfileIds)[0] = IUserHandle::USER_OWNER;

    AutoPtr<IBackgroundThreadHelper> btHelper;
    CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&btHelper);
    AutoPtr<IHandler> handler;
    btHelper->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mBgHandler = new BgHandler(looper, this);
    mPackageMonitor = new MyPackageMonitor(this);
}

CActivityManagerService::~CActivityManagerService()
{
    mRecentTasks->Clear();
    mIsolatedProcesses.Clear();
    mPidsSelfLocked.Clear();
    mForegroundProcesses.Clear();
    mStartingProcesses.Clear();
    mLruProcesses.Clear();
    mProcessesToGc.Clear();
    mStartedUsers.Clear();
    mUserLru.Clear();
    //mPendingResultRecords.Clear();
    mIntentSenderRecords.Clear();
    mRegisteredReceivers.Clear();

    mStickyBroadcasts.Clear();
    mLaunchingProviders.Clear();
    mPendingProcessChanges.Clear();
    mAvailProcessChanges.Clear();
    mAlreadyLoggedViolatedStacks.Clear();
    mGrantedUriPermissions.Clear();
}

ECode CActivityManagerService::constructor(
    /* [in] */ IContext* systemContext)
{
    mContext = systemContext;

    mFactoryTest = FactoryTest::GetMode();
    AutoPtr<IActivityThreadHelper> atHelper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&atHelper);
    atHelper->GetCurrentActivityThread((IActivityThread**)&mSystemThread);

    AutoPtr<IActivityManagerHelper> amhelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
    Int32 cls;
    amhelper->StaticGetMemoryClass(&cls);
    Slogger::I(TAG, "Memory class: %d", cls);

    mHandlerThread = new ServiceThread(TAG,
            IProcess::THREAD_PRIORITY_FOREGROUND, FALSE /*allowIo*/);
    mHandlerThread->Start();
    AutoPtr<ILooper> looper;
    mHandlerThread->GetLooper((ILooper**)&looper);
    mHandler = new MainHandler(looper, this);

    mFgBroadcastQueue = new BroadcastQueue(this, mHandler,
        String("foreground"), BROADCAST_FG_TIMEOUT, FALSE);
    mBgBroadcastQueue = new BroadcastQueue(this, mHandler,
        String("background"), BROADCAST_BG_TIMEOUT, TRUE);
    mBroadcastQueues->Set(0, mFgBroadcastQueue);
    mBroadcastQueues->Set(1, mBgBroadcastQueue);

    mServices = new ActiveServices(this);
    mProviderMap = new ProviderMap(this);

    // TODO: Move creation of battery stats service outside of activity manager service.
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    AutoPtr<IFile> systemDir;
    CFile::New(dataDir, String("system"), (IFile**)&systemDir);
    Boolean bval;
    systemDir->Exists(&bval);
    if (!bval) {
        systemDir->Mkdirs(&bval);
    }

    Slogger::I(TAG, " GetDataDirectory: dataDir: [%s], systemDir: [%s]", TO_CSTR(dataDir), TO_CSTR(systemDir));

    CBatteryStatsService::NewByFriend(systemDir, mHandler, (CBatteryStatsService**)&mBatteryStatsService);
    AutoPtr<IBatteryStatsImpl> bstats = mBatteryStatsService->GetActiveStatistics();
    bstats->ReadLocked();
    bstats->WriteAsyncLocked();
    if (DEBUG_POWER) {
        mOnBattery = TRUE;
    }
    else {
        bstats->IsOnBattery(&mOnBattery);
    }
    AutoPtr<BatteryCallback> cb = new BatteryCallback(this);
    bstats->SetCallback(cb);

    AutoPtr<IFile> file;
    CFile::New(systemDir, String("procstats"), (IFile**)&file);
    CProcessStatsService::NewByFriend(this, file, (CProcessStatsService**)&mProcessStats);

    file = NULL;
    CFile::New(systemDir, String("appops.xml"), (IFile**)&file);
    CAppOpsService::NewByFriend(file, mHandler, (CAppOpsService**)&mAppOpsService);

    file = NULL;
    CFile::New(systemDir, String("urigrants.xml"), (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mGrantFile);

    // User 0 is the first and only user that runs at boot.
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(0, (IUserHandle**)&userHandle);
    mStartedUsers[0] = new UserStartedState(userHandle, TRUE);
    mUserLru.PushBack(0);
    UpdateStartedUserArrayLocked();

    AutoPtr<ISystemProperties> sysProps;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
    sysProps->GetInt32(String("ro.opengles.version"),
        IConfigurationInfo::GL_ES_VERSION_UNDEFINED, &GL_ES_VERSION);

    mConfiguration->SetToDefaults();
    AutoPtr<ILocaleHelper> lHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lHelper);
    AutoPtr<ILocale> defLocal;
    lHelper->GetDefault((ILocale**)&defLocal);
    mConfiguration->SetLocale(defLocal);

    mConfigurationSeq = 1;
    mConfiguration->SetSeq(1);
    mProcessCpuTracker->Init();

    mCompatModePackages = new CompatModePackages(this, systemDir, mHandler);
    AutoPtr<IntentFirewallInterface> ifInterface = new IntentFirewallInterface(this);
    // TODO
    // mIntentFirewall = new IntentFirewall(ifInterface, mHandler);
    mStackSupervisor = new ActivityStackSupervisor(this);
    mTaskPersister = new TaskPersister(systemDir, mStackSupervisor);

    mProcessCpuThread = new ProcessCpuThread(this);

    mLockToAppRequest = new LockToAppRequestDialog(mContext, this);

    AutoPtr<Watchdog> watchdog = Watchdog::GetInstance();
    watchdog->AddMonitor(this);
    watchdog->AddThread(mHandler);

    return NOERROR;
}

AutoPtr<BroadcastQueue> CActivityManagerService::BroadcastQueueForIntent(
    /* [in] */ IIntent* intent)
{
    ELA_ASSERT_WITH_BLOCK(intent != NULL) {
        Slogger::I(TAG, "BroadcastQueueForIntent: intent is NULL.");
    }

    Int32 flags = 0;
    intent->GetFlags(&flags);
    Boolean isFg = (flags & IIntent::FLAG_RECEIVER_FOREGROUND) != 0;
    if (DEBUG_BACKGROUND_BROADCAST) {
        Slogger::I(TAG, "Broadcast %s on %s queue.",
            TO_CSTR(intent), (isFg ? "foreground" : "background"));
    }
    return (isFg) ? mFgBroadcastQueue : mBgBroadcastQueue;
}

AutoPtr<BroadcastRecord> CActivityManagerService::BroadcastRecordForReceiverLocked(
    /* [in] */ IBinder* receiver)
{
    AutoPtr<BroadcastRecord> r;
    AutoPtr<BroadcastQueue> queue;
    for (Int32 i = 0; i < mBroadcastQueues->GetLength(); ++i) {
        queue = (*mBroadcastQueues)[i];
        if (queue != NULL) {
            r = queue->GetMatchingOrderedReceiver(receiver);
            if (r != NULL) {
                break;
            }
        }
    }

    return r;
}

ECode CActivityManagerService::SetSystemProcess()
{
//    try {
    FAIL_RETURN(ServiceManager::AddService(IContext::ACTIVITY_SERVICE, TO_IINTERFACE(this), TRUE))
    FAIL_RETURN(ServiceManager::AddService(IProcessStats::SERVICE_NAME, TO_IINTERFACE(mProcessStats)))
    // ServiceManager::AddService("meminfo", new MemBinder(this));
    // ServiceManager::AddService("gfxinfo", new GraphicsBinder(this));
    // ServiceManager::AddService("dbinfo", new DbBinder(this));
    // if (MONITOR_CPU_USAGE) {
    //     ServiceManager::AddService("cpuinfo", new CpuBinder(this));
    // }

    AutoPtr<IPermissionController> permissionController;
    CPermissionController::New(this, (IPermissionController**)&permissionController);
    FAIL_RETURN(ServiceManager::AddService(String("permission"), permissionController.Get()))

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> info;
    pm->GetApplicationInfo(String("android"), STOCK_PM_FLAGS, (IApplicationInfo**)&info);
    mSystemThread->InstallSystemApplicationInfo(info, NULL);

    {
        AutoLock lock(this);

        String processName;
        info->GetProcessName(&processName);
        AutoPtr<ProcessRecord> app = NewProcessRecordLocked(info, processName, FALSE, 0);
        app->mPersistent = TRUE;
        app->mPid = MY_PID;
        app->mMaxAdj = ProcessList::SYSTEM_ADJ;
        AutoPtr<IApplicationThread> appThread;
        mSystemThread->GetApplicationThread((IApplicationThread**)&appThread);
        app->MakeActive(appThread, mProcessStats);
        mProcessNames->Put(app->mProcessName, app->mUid, app);
        {
            AutoLock lock(mPidsSelfLockedLock);
            mPidsSelfLocked[app->mPid] = app;
        }
        UpdateLruProcessLocked(app, FALSE, NULL);
        UpdateOomAdjLocked();
    }
//    } catch (PackageManager.NameNotFoundException e) {
//        throw new RuntimeException(
//                "Unable to find android system package", e);
//    }
    return NOERROR;
}

void CActivityManagerService::SetWindowManager(
    /* [in] */ CWindowManagerService* wm)
{
    mWindowManager = wm;
    mStackSupervisor->SetWindowManager(wm);
}

void CActivityManagerService::SetUsageStatsManager(
    /* [in] */ IUsageStatsManagerInternal* usageStatsManager)
{
    mUsageStatsService = usageStatsManager;
}

ECode CActivityManagerService::StartObservingNativeCrashes()
{
    AutoPtr<NativeCrashListener> ncl = new NativeCrashListener(this);
    return ncl->Start();
}

AutoPtr<CAppOpsService> CActivityManagerService::GetAppOpsService()
{
    return mAppOpsService;
}

// static class MemBinder extends Binder {
//     ActivityManagerService mActivityManagerService;
//     MemBinder(ActivityManagerService activityManagerService) {
//         mActivityManagerService = activityManagerService;
//     }

//     @Override
//     protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//         if (mActivityManagerService.checkCallingPermission(android.Manifest.permission.DUMP)
//                 != PackageManager.PERMISSION_GRANTED) {
//             pw->Println(String("Permission Denial: can't dump meminfo from from pid=)"
//                     + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                     + " without permission " + android.Manifest.permission.DUMP);
//             return;
//         }

//         mActivityManagerService.dumpApplicationMemoryUsage(fd, pw, "  ", args, FALSE, null);
//     }
// }

// static class GraphicsBinder extends Binder {
//     ActivityManagerService mActivityManagerService;
//     GraphicsBinder(ActivityManagerService activityManagerService) {
//         mActivityManagerService = activityManagerService;
//     }

//     @Override
//     protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//         if (mActivityManagerService.checkCallingPermission(android.Manifest.permission.DUMP)
//                 != PackageManager.PERMISSION_GRANTED) {
//             pw->Println(String("Permission Denial: can't dump gfxinfo from from pid=)"
//                     + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                     + " without permission " + android.Manifest.permission.DUMP);
//             return;
//         }

//         mActivityManagerService.dumpGraphicsHardwareUsage(fd, pw, args);
//     }
// }

// static class DbBinder extends Binder {
//     ActivityManagerService mActivityManagerService;
//     DbBinder(ActivityManagerService activityManagerService) {
//         mActivityManagerService = activityManagerService;
//     }

//     @Override
//     protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//         if (mActivityManagerService.checkCallingPermission(android.Manifest.permission.DUMP)
//                 != PackageManager.PERMISSION_GRANTED) {
//             pw->Println("Permission Denial: can't dump dbinfo from from pid="
//                     + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                     + " without permission " + android.Manifest.permission.DUMP);
//             return;
//         }

//         mActivityManagerService.dumpDbInfo(fd, pw, args);
//     }
// }

// static class CpuBinder extends Binder {
//     ActivityManagerService mActivityManagerService;
//     CpuBinder(ActivityManagerService activityManagerService) {
//         mActivityManagerService = activityManagerService;
//     }

//     @Override
//     protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//         if (mActivityManagerService.checkCallingPermission(android.Manifest.permission.DUMP)
//                 != PackageManager.PERMISSION_GRANTED) {
//             pw->Println("Permission Denial: can't dump cpuinfo from from pid="
//                     + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                     + " without permission " + android.Manifest.permission.DUMP);
//             return;
//         }

//         {    AutoLock syncLock(mActivityManagerService.mProcessCpuTracker);
//             pw->Print(mActivityManagerService.mProcessCpuTracker.printCurrentLoad());
//             pw->Print(mActivityManagerService.mProcessCpuTracker.printCurrentState(
//                     SystemClock.uptimeMillis()));
//         }
//     }
// }

void CActivityManagerService::SetSystemServiceManager(
    /* [in] */ ISystemServiceManager* mgr)
{
    mSystemServiceManager = mgr;
}

ECode CActivityManagerService::Start()
{
    Process::RemoveAllProcessGroups();
    mProcessCpuThread->Start();

    FAIL_RETURN(mBatteryStatsService->Publish(mContext))
    FAIL_RETURN(mAppOpsService->Publish(mContext))
    AutoPtr<LocalService> localService = new LocalService(this);
    return LocalServices::AddService(EIID_IActivityManagerInternal, localService->Probe(EIID_IInterface));
}

void CActivityManagerService::InitPowerManagement()
{
    mStackSupervisor->InitPowerManagement();
    mBatteryStatsService->InitPowerManagement();
}

ECode CActivityManagerService::UpdateCpuStats()
{
    const Int64 now = SystemClock::GetUptimeMillis();
    Int64 lastCpuTime;
    mLastCpuTime->Get(&lastCpuTime);
    if (lastCpuTime >= now - MONITOR_CPU_MIN_TIME) {
        return NOERROR;
    }
    Boolean result;
    mProcessCpuMutexFree->CompareAndSet(TRUE, FALSE, &result);
    if (result) {
        {
            AutoLock lock(mProcessCpuThread);
            mProcessCpuThread->Notify();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::UpdateCpuStatsNow()
{
    // enable for detecting memory leak
    // return NOERROR;

    AutoLock lock(mProcessCpuTracker);
    mProcessCpuMutexFree->Set(FALSE);
    const Int64 now = SystemClock::GetUptimeMillis();
    Boolean haveNewCpuStats = FALSE;

    Int64 lastCpuTime;
    if (MONITOR_CPU_USAGE &&
            (mLastCpuTime->Get(&lastCpuTime), lastCpuTime) < (now-MONITOR_CPU_MIN_TIME)) {
        mLastCpuTime->Set(now);
        haveNewCpuStats = TRUE;
        mProcessCpuTracker->Update();
        //Slogger::I(TAG, mProcessCpu.printCurrentState());
        //Slogger::I(TAG, "Total CPU usage: "
        //        + mProcessCpu.getTotalCpuPercent() + "%");

        // Slog the cpu usage if the property is set.
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String value;
        if (sysProp->Get(String("events.cpu"), &value), value.Equals("TRUE")) {
            Int32 user, system, iowait, irq, softIrq, idle;
            mProcessCpuTracker->GetLastUserTime(&user);
            mProcessCpuTracker->GetLastSystemTime(&system);
            mProcessCpuTracker->GetLastIoWaitTime(&iowait);
            mProcessCpuTracker->GetLastIrqTime(&irq);
            mProcessCpuTracker->GetLastSoftIrqTime(&softIrq);
            mProcessCpuTracker->GetLastIdleTime(&idle);

            Int32 total = user + system + iowait + irq + softIrq + idle;
            if (total == 0) total = 1;

            // EventLog.writeEvent(EventLogTags.CPU,
            //         ((user+system+iowait+irq+softIrq) * 100) / total,
            //         (user * 100) / total,
            //         (system * 100) / total,
            //         (iowait * 100) / total,
            //         (irq * 100) / total,
            //         (softIrq * 100) / total);
        }
    }

    AutoPtr<ArrayOf<Int64> > cpuSpeedTimes;
    mProcessCpuTracker->GetLastCpuSpeedTimes((ArrayOf<Int64>**)&cpuSpeedTimes);
    AutoPtr<IBatteryStatsImpl> bstats = mBatteryStatsService->GetActiveStatistics();

    {    AutoLock syncLock(bstats);
        {
            AutoLock lock(mPidsSelfLockedLock);
            if (haveNewCpuStats) {
                if (mOnBattery) {
                    Int32 perc;
                    bstats->StartAddingCpuLocked(&perc);
                    Int32 totalUTime = 0;
                    Int32 totalSTime = 0;
                    Int32 pid, utime, stime, otherUTime, otherSTime;
                    Int32 N;
                    mProcessCpuTracker->CountStats(&N);
                    for (Int32 i = 0; i < N; i++) {
                        AutoPtr<IProcessCpuTrackerStats> st;
                        mProcessCpuTracker->GetStats(i, (IProcessCpuTrackerStats**)&st);
                        Boolean isWorking;
                        st->GetWorking(&isWorking);
                        if (!isWorking) {
                            continue;
                        }

                        st->GetPid(&pid);
                        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(pid);
                        AutoPtr<ProcessRecord> pr = it != mPidsSelfLocked.End() ? it->mSecond : NULL;
                        st->GetRelUtime(&utime);
                        otherUTime = (utime * perc) / 100;
                        st->GetRelStime(&stime);
                        otherSTime = (stime * perc) / 100;
                        totalUTime += otherUTime;
                        totalSTime += otherSTime;
                        if (pr != NULL) {
                            AutoPtr<IBatteryStatsImplUidProc> ps = pr->mCurProcBatteryStats;
                            Boolean isActive;
                            if (ps == NULL || !(IBatteryStatsUidProc::Probe(ps)->IsActive(&isActive), isActive)) {
                                Int32 uid;
                                pr->mInfo->GetUid(&uid);
                                ps = NULL;
                                bstats->GetProcessStatsLocked(uid, pr->mProcessName,
                                    (IBatteryStatsImplUidProc**)&ps);
                                pr->mCurProcBatteryStats = ps;
                            }
                            ps->AddCpuTimeLocked(utime-otherUTime, stime-otherSTime);
                            ps->AddSpeedStepTimes(cpuSpeedTimes);
                            pr->mCurCpuTime += (utime + stime) * 10;
                        }
                        else {
                            AutoPtr<IBatteryStatsImplUidProc> ps;
                            st->GetBatteryStats((IBatteryStatsImplUidProc**)&ps);
                            Boolean isActive;
                            if (ps == NULL || !(IBatteryStatsUidProc::Probe(ps)->IsActive(&isActive), isActive)) {
                                Int32 uid, mapUid;
                                st->GetUid(&uid);
                                bstats->MapUid(uid, &mapUid);
                                String name;
                                st->GetName(&name);
                                ps = NULL;
                                bstats->GetProcessStatsLocked(mapUid, name,
                                    (IBatteryStatsImplUidProc**)&ps);
                                st->SetBatteryStats(ps);
                            }
                            if (ps != NULL) {
                                ps->AddCpuTimeLocked(utime-otherUTime, stime-otherSTime);
                                ps->AddSpeedStepTimes(cpuSpeedTimes);
                            }
                        }
                    }
                    bstats->FinishAddingCpuLocked(perc, totalUTime,
                            totalSTime, cpuSpeedTimes);
                }
            }
        }

        if (mLastWriteTime < (now - BATTERY_STATS_TIME)) {
            mLastWriteTime = now;
            bstats->WriteAsyncLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::BatteryNeedsCpuUpdate()
{
    return UpdateCpuStatsNow();
}

ECode CActivityManagerService::BatteryPowerChanged(
    /* [in] */ Boolean onBattery)
{
    // When plugging in, update the CPU stats first before changing
    // the plug state.
    UpdateCpuStatsNow();
    {
        AutoLock lock(this);
        {
            AutoLock lock(mPidsSelfLockedLock);
            mOnBattery = DEBUG_POWER ? TRUE : onBattery;
        }
    }
    return NOERROR;
}

AutoPtr<IContext> CActivityManagerService::GetUiContext()
{
    AutoLock lock(this);
    if (mUiContext == NULL && mBooted) {
        mUiContext = ThemeUtils::CreateUiContext(mContext);
    }
    return mUiContext != NULL ? mUiContext : mContext;
}

AutoPtr<IHashMap> CActivityManagerService::GetCommonServicesLocked()
{
    if (mAppBindArgs == NULL) {
        CHashMap::New((IHashMap**)&mAppBindArgs);

        // Setup the application init args
        AutoPtr<IInterface> keyPm = CoreUtils::Convert(String("package"));
        AutoPtr<IInterface> keyWm = CoreUtils::Convert(String("window"));
        AutoPtr<IInterface> keySm = CoreUtils::Convert(IContext::ALARM_SERVICE);

        AutoPtr<IInterface> packageManager = ServiceManager::GetService(String("package"));
        AutoPtr<IInterface> windowManager = ServiceManager::GetService(String("window"));
        AutoPtr<IInterface> alarmService = ServiceManager::GetService(IContext::ALARM_SERVICE);
        mAppBindArgs->Put(keyPm, packageManager);
        mAppBindArgs->Put(keyWm, windowManager);
        mAppBindArgs->Put(keySm, alarmService);
    }

    return mAppBindArgs;
}

ECode CActivityManagerService::SetFocusedActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    if (mFocusedActivity.Get() != r) {
        if (DEBUG_FOCUS) Slogger::D(TAG, "setFocusedActivityLocked: r=%s", TO_CSTR(r));
        mFocusedActivity = r;
        if (r->mTask != NULL && r->mTask->mVoiceInteractor != NULL) {
            StartRunningVoiceLocked();
        }
        else {
            FinishRunningVoiceLocked();
        }
        mStackSupervisor->SetFocusedStack(r);
        if (r != NULL) {
            mWindowManager->SetFocusedApp(IBinder::Probe(r->mAppToken), TRUE);
        }
        ApplyUpdateLockStateLocked(r);
    }
    return NOERROR;
}

void CActivityManagerService::ClearFocusedActivity(
    /* [in] */ ActivityRecord* r)
{
    if (mFocusedActivity.Get() == r) {
        mFocusedActivity = NULL;
    }
}

// @Override
ECode CActivityManagerService::SetFocusedStack(
    /* [in] */ Int32 stackId)
{
    if (DEBUG_FOCUS) Slogger::D(TAG, "setFocusedStack: stackId=%d", stackId);
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = mStackSupervisor->GetStack(stackId);
        if (stack != NULL) {
            AutoPtr<ActivityRecord> r = stack->TopRunningActivityLocked(NULL);
            if (r != NULL) {
                SetFocusedActivityLocked(r);
            }
        }
    }
    return NOERROR;
}

// @Override
ECode CActivityManagerService::NotifyActivityDrawn(
    /* [in] */ IBinder* token)
{
    if (DEBUG_VISBILITY) Slogger::D(TAG, "notifyActivityDrawn: token=%s", TO_CSTR(token));
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = mStackSupervisor->IsInAnyStackLocked(token);
        if (r != NULL) {
            r->mTask->mStack->NotifyActivityDrawnLocked(r);
        }
    }
    return NOERROR;
}

void CActivityManagerService::ApplyUpdateLockStateLocked(
    /* [in] */ ActivityRecord* r)
{
    // Modifications to the UpdateLock state are done on our handler, outside
    // the activity manager's locks.  The new state is determined based on the
    // state *now* of the relevant activity record.  The object is passed to
    // the handler solely for logging detail, not to be consulted/modified.
    Boolean nextState = r != NULL && r->mImmersive;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IMMERSIVE_MODE_LOCK_MSG, (nextState) ? 1 : 0, 0,
        (IActivityRecord*)r, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void CActivityManagerService::ShowAskCompatModeDialogLocked(
    /* [in] */ ActivityRecord* r)
{
    AutoPtr<IMessageHelper> msgHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&msgHelper);
    AutoPtr<IMessage> msg;
    msgHelper->Obtain((IMessage**)&msg);
    msg->SetWhat(SHOW_COMPAT_MODE_DIALOG_MSG);
    msg->SetObj(r->mTask->mAskedCompatMode ? NULL : (IActivityRecord*)r);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

Int32 CActivityManagerService::UpdateLruProcessInternalLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int64 now,
    /* [in] */ Int32 index,
    /* [in] */ const String& what,
    /* [in] */ IInterface* obj,
    /* [in] */ ProcessRecord* srcApp)
{
    app->mLastActivityTime = now;

    if (app->mActivities.GetSize() > 0) {
        // Don't want to touch dependent processes that are hosting activities.
        return index;
    }

    Int32 lrui = -1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.REnd(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }
    if (lrui < 0) {
        Slogger::W/*Wtf*/(TAG, "Adding dependent process %s not on LRU list: %s %s from %s",
            TO_CSTR(app), what.string(), TO_CSTR(obj), TO_CSTR(srcApp));
        return index;
    }

    if (lrui >= index) {
        // Don't want to cause this to move dependent processes *back* in the
        // list as if they were less frequently used.
        return index;
    }

    if (lrui >= mLruProcessActivityStart) {
        // Don't want to touch dependent processes that are hosting activities.
        return index;
    }

    mLruProcesses.Erase(--rit.GetBase());
    if (index > 0) {
        index--;
    }
    if (DEBUG_LRU) Slogger::D(TAG, "Moving dep from %d to %d in LRU list: %s",
        lrui, index, TO_CSTR(app));
    List< AutoPtr<ProcessRecord> >::Iterator it = mLruProcesses.Begin();
    for (Int32 i = 0; i < index; i++)
        ++it;
    mLruProcesses.Insert(it, app);
    return index;
}

void CActivityManagerService::RemoveLruProcessLocked(
    /* [in] */ ProcessRecord* app)
{
    Int32 lrui = -1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.REnd(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }
    if (lrui >= 0) {
        if (!app->mKilled) {
            Slogger::W/*WtfStack*/(TAG, "Removing process that hasn't been killed: %s", TO_CSTR(app));
            Process::KillProcessQuiet(app->mPid);
            Int32 uid;
            app->mInfo->GetUid(&uid);
            Process::KillProcessGroup(uid, app->mPid);
        }
        if (lrui <= mLruProcessActivityStart) {
            mLruProcessActivityStart--;
        }
        if (lrui <= mLruProcessServiceStart) {
            mLruProcessServiceStart--;
        }
        mLruProcesses.Erase(--rit.GetBase());
    }
}

void CActivityManagerService::UpdateLruProcessLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean activityChange,
    /* [in] */ ProcessRecord* client)
{
    Boolean hasActivity = app->mActivities.GetSize() > 0 || app->mHasClientActivities
            || app->mTreatLikeActivity;
    Boolean hasService = FALSE; // not impl yet. app->mServices.size() > 0;
    if (!activityChange && hasActivity && !(app->mPersistent &&
        Find(mLruProcesses.Begin(), mLruProcesses.End(), AutoPtr<ProcessRecord>(app))
        == mLruProcesses.End())) {
        // The process has activities, so we are only allowing activity-based adjustments
        // to move it.  It should be kept in the front of the list with other
        // processes that have activities, and we don't want those to change their
        // order except due to activity operations.
        // Also, do not return if the app is persistent and not found in mLruProcesses.
        // For persistent apps, service records are not cleaned up and if we return
        // here it will not be added to mLruProcesses and on its restart it might lead to
        // securityException if app is not present in mLruProcesses.
        return;
    }

    mLruSeq++;
    Int64 now = SystemClock::GetUptimeMillis();
    app->mLastActivityTime = now;

    // First a quick reject: if the app is already at the position we will
    // put it, then there is nothing to do.
    if (hasActivity) {
        Int32 N = mLruProcesses.GetSize();
        if (N > 0 && mLruProcesses.RBegin()->Get() == app) {
            if (DEBUG_LRU) Slogger::D(TAG, "Not moving, already top activity: %s", TO_CSTR(app));
            return;
        }
    }
    else {
        if (mLruProcessServiceStart > 0
            && mLruProcesses[mLruProcessServiceStart-1].Get() == app) {
            if (DEBUG_LRU) Slogger::D(TAG, "Not moving, already top other: %s", TO_CSTR(app));
            return;
        }
    }

    Int32 lrui = -1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.REnd(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }

    if (app->mPersistent && lrui >= 0) {
        // We don't care about the position of persistent processes, as long as
        // they are in the list.
        if (DEBUG_LRU) Slogger::D(TAG, "Not moving, persistent: %s", TO_CSTR(app));
        return;
    }

    /* In progress: compute new position first, so we can avoid doing work
       if the process is not actually going to move.  Not yet working.
    Int32 addIndex;
    Int32 nextIndex;
    Boolean inActivity = FALSE, inService = FALSE;
    if (hasActivity) {
        // Process has activities, put it at the very tipsy-top.
        addIndex = mLruProcesses.GetSize();
        nextIndex = mLruProcessServiceStart;
        inActivity = TRUE;
    }
    else if (hasService) {
        // Process has services, put it at the top of the service list.
        addIndex = mLruProcessActivityStart;
        nextIndex = mLruProcessServiceStart;
        inActivity = TRUE;
        inService = TRUE;
    }
    else  {
        // Process not otherwise of interest, it goes to the top of the non-service area.
        addIndex = mLruProcessServiceStart;
        if (client != NULL) {
            Int32 clientIndex = mLruProcesses.lastIndexOf(client);
            if (clientIndex < 0) Slogger::D(TAG, "Unknown client " + client + " when updating "
                    + app);
            if (clientIndex >= 0 && addIndex > clientIndex) {
                addIndex = clientIndex;
            }
        }
        nextIndex = addIndex > 0 ? addIndex-1 : addIndex;
    }

    Slogger::D(TAG, "Update LRU at " + lrui + " to " + addIndex + " (act="
            + mLruProcessActivityStart + "): %s", TO_CSTR(app));
    */

    if (lrui >= 0) {
        if (lrui < mLruProcessActivityStart) {
            mLruProcessActivityStart--;
        }
        if (lrui < mLruProcessServiceStart) {
            mLruProcessServiceStart--;
        }
        /*
        if (addIndex > lrui) {
            addIndex--;
        }
        if (nextIndex > lrui) {
            nextIndex--;
        }
        */
        mLruProcesses.Erase(--rit.GetBase());
    }

    /*
    mLruProcesses.add(addIndex, app);
    if (inActivity) {
        mLruProcessActivityStart++;
    }
    if (inService) {
        mLruProcessActivityStart++;
    }
    */

    Int32 nextIndex;
    if (hasActivity) {
        Int32 N = mLruProcesses.GetSize();
        if (app->mActivities.GetSize() == 0 && mLruProcessActivityStart < (N-1)) {
            // Process doesn't have activities, but has clients with
            // activities...  move it up, but one below the top (the top
            // should always have a real activity).
            if (DEBUG_LRU) Slogger::D(TAG, "Adding to second-top of LRU activity list: %s", TO_CSTR(app));
            List< AutoPtr<ProcessRecord> >::Iterator it = --mLruProcesses.End();
            mLruProcesses.Insert(it, app);
            // To keep it from spamming the LRU list (by making a bunch of clients),
            // we will push down any other entries owned by the app.
            Int32 uid;
            app->mInfo->GetUid(&uid);
            --it;
            for (Int32 i = N - 2; i > mLruProcessActivityStart; --it, i--) {
                AutoPtr<ProcessRecord> subProc = *it;
                Int32 subUid;
                subProc->mInfo->GetUid(&subUid);
                if (subUid == uid) {
                    // We want to push this one down the list.  If the process after
                    // it is for the same uid, however, don't do so, because we don't
                    // want them internally to be re-ordered.
                    List< AutoPtr<ProcessRecord> >::Iterator nextIt = it;
                    AutoPtr<ProcessRecord> nextSubProc = *(--nextIt);
                    Int32 nextSubUid;
                    nextSubProc->mInfo->GetUid(&nextSubUid);
                    if (nextSubUid != uid) {
                        if (DEBUG_LRU) Slogger::D(TAG, "Pushing uid %d swapping at %d: %s : %s",
                            uid, i, TO_CSTR(subProc), TO_CSTR(nextSubProc));
                        *it = nextSubProc;
                        *nextIt = subProc;
                        --it;
                        i--;
                    }
                }
                else {
                    // A gap, we can stop here.
                    break;
                }
            }
        }
        else {
            // Process has activities, put it at the very tipsy-top.
            if (DEBUG_LRU) Slogger::D(TAG, "Adding to top of LRU activity list: %s", TO_CSTR(app));
            mLruProcesses.PushBack(app);
        }
        nextIndex = mLruProcessServiceStart;
    }
    else if (hasService) {
        // Process has services, put it at the top of the service list.
        if (DEBUG_LRU) Slogger::D(TAG, "Adding to top of LRU service list: %s", TO_CSTR(app));
        List< AutoPtr<ProcessRecord> >::Iterator it = mLruProcesses.Begin();
        for (Int32 i = 0; i < mLruProcessActivityStart; ++i)
            ++it;
        mLruProcesses.Insert(it, app);
        nextIndex = mLruProcessServiceStart;
        mLruProcessActivityStart++;
    }
    else  {
        // Process not otherwise of interest, it goes to the top of the non-service area.
        Int32 index = mLruProcessServiceStart;
        if (client != NULL) {
            // If there is a client, don't allow the process to be moved up higher
            // in the list than that client.
            Int32 clientIndex = -1;
            List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
            for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.REnd(); ++rit, --i) {
                if (rit->Get() == client) {
                    clientIndex = i;
                    break;
                }
            }
            if (DEBUG_LRU && clientIndex < 0) Slogger::D(TAG, "Unknown client %s when updating %s",
                    TO_CSTR(client), TO_CSTR(app));
            if (clientIndex <= lrui) {
                // Don't allow the client index restriction to push it down farther in the
                // list than it already is.
                clientIndex = lrui;
            }
            if (clientIndex >= 0 && index > clientIndex) {
                index = clientIndex;
            }
        }
        if (DEBUG_LRU) Slogger::D(TAG, "Adding at %d of LRU list: %s", index, TO_CSTR(app));
        List< AutoPtr<ProcessRecord> >::Iterator it = mLruProcesses.Begin();
        for (Int32 i = 0; i < index; ++i)
            ++it;
        mLruProcesses.Insert(it, app);
        nextIndex = index-1;
        mLruProcessActivityStart++;
        mLruProcessServiceStart++;
    }

    // If the app is currently using a content provider or service,
    // bump those processes as well.
    // for (Int32 j = app->mConnections.size()-1; j>=0; j--) {
    HashSet< AutoPtr<ConnectionRecord> >::Iterator cit = app->mConnections.Begin();
    for (; cit != app->mConnections.End(); ++cit) {
        AutoPtr<ConnectionRecord> cr = *cit;
        if (cr->mBinding != NULL && !cr->mServiceDead && cr->mBinding->mService != NULL
                && cr->mBinding->mService->mApp != NULL
                && cr->mBinding->mService->mApp->mLruSeq != mLruSeq
                && !cr->mBinding->mService->mApp->mPersistent) {
            nextIndex = UpdateLruProcessInternalLocked(cr->mBinding->mService->mApp, now, nextIndex,
                    String("service connection"), cr->Probe(EIID_IInterface), app);
        }
    }
    // for (Int32 j=app->mConProviders.size()-1; j>=0; j--) {
    List<AutoPtr<CContentProviderConnection> >::Iterator cpcIt = app->mConProviders.Begin();
    for (; cpcIt != app->mConProviders.End(); ++cpcIt) {
        AutoPtr<ContentProviderRecord> cpr = (*cpcIt)->mProvider;
        if (cpr->mProc != NULL && cpr->mProc->mLruSeq != mLruSeq && !cpr->mProc->mPersistent) {
            nextIndex = UpdateLruProcessInternalLocked(cpr->mProc, now, nextIndex,
                    String("provider reference"), cpr->Probe(EIID_IInterface), app);
        }
    }
}

AutoPtr<ProcessRecord> CActivityManagerService::GetProcessRecordLocked(
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean keepIfLarge)
{
    AutoPtr<ProcessRecord> proc;
    if (uid == IProcess::SYSTEM_UID) {
        // The system gets to run in any process.  If there are multiple
        // processes with the same uid, just pick the first (this
        // should never happen).
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > map = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
        it = map->Find(processName);
        if (it == map->End()) {
            return proc;
        }

        AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > procs= it->mSecond;
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it2;
        for(it2 = procs->Begin(); it2 != procs->End(); ++it2) {
            Int32 procUid = it2->mFirst;
            if (UserHandle::IsApp(procUid) || !UserHandle::IsSameUser(procUid, uid)) {
                // Don't use an app process or different user process for system component.
                continue;
            }
            return it2->mSecond;
        }
    }
    proc = mProcessNames->Get(processName, uid);
    if (FALSE && proc != NULL && !keepIfLarge
        && proc->mSetProcState >= IActivityManager::PROCESS_STATE_CACHED_EMPTY
        && proc->mLastCachedPss >= 4000) {
        // Turn this condition on to cause killing to happen regularly, for testing.
        if (proc->mBaseProcessTracker != NULL) {
            proc->mBaseProcessTracker->ReportCachedKill(proc->mPkgList, proc->mLastCachedPss);
        }
        proc->Kill(StringUtils::ToString(proc->mLastCachedPss) + "k from cached", TRUE);
    }
    else if (proc != NULL && !keepIfLarge
        && mLastMemoryLevel > IProcessStats::ADJ_MEM_FACTOR_NORMAL
        && proc->mSetProcState >= IActivityManager::PROCESS_STATE_CACHED_EMPTY) {
        if (DEBUG_PSS) Slogger::D(TAG, "May not keep %s: pss=%lld", TO_CSTR(proc), proc->mLastCachedPss);
        if (proc->mLastCachedPss >= mProcessList->GetCachedRestoreThresholdKb()) {
            if (proc->mBaseProcessTracker != NULL) {
                proc->mBaseProcessTracker->ReportCachedKill(proc->mPkgList, proc->mLastCachedPss);
            }
            proc->Kill(StringUtils::ToString(proc->mLastCachedPss) + "k from cached", TRUE);
        }
    }
    return proc;
}

ECode CActivityManagerService::EnsurePackageDexOpt(
    /* [in] */ const String& packageName)
{
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
//    try {
        Boolean result;
        pm->PerformDexOptIfNeeded(packageName, String(NULL)/* instruction set */, &result);
        if (result) {
            mDidDexOpt = TRUE;
        }
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

Boolean CActivityManagerService::IsNextTransitionForward()
{
    Int32 UNUSED(transit);
    mWindowManager->GetPendingAppTransition(&transit);
    return transit == AppTransition::TRANSIT_ACTIVITY_OPEN
            || transit == AppTransition::TRANSIT_TASK_OPEN
            || transit == AppTransition::TRANSIT_TASK_TO_FRONT;
}

Int32 CActivityManagerService::StartIsolatedProcess(
    /* [in] */ const String& entryPoint,
    /* [in] */ ArrayOf<String>* entryPointArgs,
    /* [in] */ const String& processName,
    /* [in] */ const String& abiOverride,
    /* [in] */ Int32 uid,
    /* [in] */ IRunnable* crashHandler)
{
    AutoLock lock(this);
    AutoPtr<IApplicationInfo> info;
    CApplicationInfo::New((IApplicationInfo**)&info);
    // In general the ApplicationInfo.uid isn't neccesarily equal to ProcessRecord.uid.
    // For isolated processes, the former contains the parent's uid and the latter the
    // actual uid of the isolated process.
    // In the special case introduced by this method (which is, starting an isolated
    // process directly from the SystemServer without an actual parent app process) the
    // closest thing to a parent's uid is SYSTEM_UID.
    // The only important thing here is to keep AI.uid != PR.uid, in order to trigger
    // the |isolated| logic in the ProcessRecord constructor.
    info->SetUid(IProcess::SYSTEM_UID);
    info->SetProcessName(processName);
    info->SetClassName(entryPoint);
    IPackageItemInfo::Probe(info)->SetPackageName(String("android"));
    AutoPtr<ProcessRecord> proc = StartProcessLocked(processName, info /* info */,
            FALSE /* knownToBeDead */, 0 /* intentFlags */, String("")  /* hostingType */,
            NULL /* hostingName */, TRUE /* allowWhileBooting */, TRUE /* isolated */,
            uid, TRUE /* keepIfLarge */, abiOverride, entryPoint, entryPointArgs,
            crashHandler);
    return proc != NULL ? proc->mPid : 0;
}

AutoPtr<ProcessRecord> CActivityManagerService::StartProcessLocked(
    /* [in] */ const String& processName,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean knownToBeDead,
    /* [in] */ Int32 intentFlags,
    /* [in] */ const String& hostingType,
    /* [in] */ IComponentName* hostingName,
    /* [in] */ Boolean allowWhileBooting,
    /* [in] */ Boolean isolated,
    /* [in] */ Boolean keepIfLarge)
{
    return StartProcessLocked(processName, info, knownToBeDead, intentFlags, hostingType,
            hostingName, allowWhileBooting, isolated, 0 /* isolatedUid */, keepIfLarge,
            String(NULL) /* ABI override */, String(NULL) /* entryPoint */, NULL /* entryPointArgs */,
            NULL /* crashHandler */);
}

AutoPtr<ProcessRecord> CActivityManagerService::StartProcessLocked(
    /* [in] */ const String& processName,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean knownToBeDead,
    /* [in] */ Int32 intentFlags,
    /* [in] */ const String& hostingType,
    /* [in] */ IComponentName* hostingName,
    /* [in] */ Boolean allowWhileBooting,
    /* [in] */ Boolean isolated,
    /* [in] */ Int32 isolatedUid,
    /* [in] */ Boolean keepIfLarge,
    /* [in] */ const String& abiOverride,
    /* [in] */ const String& entryPoint,
    /* [in] */ ArrayOf<String>* entryPointArgs,
    /* [in] */ IRunnable* crashHandler)
{
    Int32 startTime = SystemClock::GetElapsedRealtime();

    Int32 infoUid;
    info->GetUid(&infoUid);

    AutoPtr<ProcessRecord> app;
    if (!isolated) {
        app = GetProcessRecordLocked(processName, infoUid, keepIfLarge);
        CheckTime(startTime, String("startProcess: after getProcessRecord"));
    }
    else {
        // If this is an isolated process, it can't re-use an existing process.
        app = NULL;
    }

    // We don't have to do anything more if:
    // (1) There is an existing application record; and
    // (2) The caller doesn't think it is dead, OR there is no thread
    //     object attached to it so we know it couldn't have crashed; and
    // (3) There is a pid assigned to it, so it is either starting or
    //     already running.
    if (DEBUG_PROCESSES) {
        Slogger::V(TAG, "startProcess: name=%s app=%s knownToBeDead=%d thread=%s pid=%d",
            processName.string(), TO_CSTR(app), knownToBeDead,
            (app != NULL ? TO_CSTR(app->mThread) : "NULL"), (app != NULL ? app->mPid : -1));
    }

    if (app != NULL && app->mPid > 0) {
        if (!knownToBeDead || app->mThread == NULL) {
            // We already have the app running, or are waiting for it to
            // come up (we have a pid but not yet its thread), so keep it.
            if (DEBUG_PROCESSES) {
                Slogger::V(TAG, "App already running: %s", TO_CSTR(app));
            }
            // If this is a new package in the process, add the package to the list
            String packageName;
            IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
            Int32 versionCode;
            info->GetVersionCode(&versionCode);
            app->AddPackage(packageName, versionCode, mProcessStats);
            CheckTime(startTime, String("startProcess: done, added package to proc"));
            return app;
        }
        // An application record is attached to a previous process,
        // clean it up now.
        if (DEBUG_PROCESSES || DEBUG_CLEANUP) {
            Slogger::V(TAG, "App died: %s", TO_CSTR(app));
        }
        CheckTime(startTime, String("startProcess: bad proc running, killing"));
        Int32 uid;
        app->mInfo->GetUid(&uid);
        Process::KillProcessGroup(uid, app->mPid);
        HandleAppDiedLocked(app, TRUE, TRUE);
        CheckTime(startTime, String("startProcess: done killing old proc"));
    }

    String hostingNameStr, infoProcName;
    info->GetProcessName(&infoProcName);
    if (hostingName != NULL) {
        hostingName->FlattenToShortString(&hostingNameStr);
    }

    if (!isolated) {
        if ((intentFlags & IIntent::FLAG_FROM_BACKGROUND) != 0) {
            // If we are in the background, then check to see if this process
            // is bad.  If so, we will just silently fail.
            if (mBadProcesses->Get(infoProcName, infoUid) != (Int64)NULL) {
                if (DEBUG_PROCESSES) {
                    Slogger::V(TAG, "Bad process: %d / %s", infoUid, infoProcName.string());
                }
                app = NULL;
                return app;
            }
        }
        else {
            // When the user is explicitly starting a process, then clear its
            // crash count so that we won't make it bad until they see at
            // least one crash dialog again, and make the process good again
            // if it had been bad.
            if (DEBUG_PROCESSES) {
                Slogger::V(TAG, "Clearing bad process: %d / %s", infoUid, infoProcName.string());
            }
            mProcessCrashTimes->Remove(infoProcName, infoUid);
            if (mBadProcesses->Get(infoProcName, infoUid) != (Int64)NULL) {
//                EventLog.writeEvent(EventLogTags.AM_PROC_GOOD, UserHandle::GetUserId(info.uid), info.uid,
//                        info.processName);
                mBadProcesses->Remove(infoProcName, infoUid);
                if (app != NULL) {
                    app->mBad = FALSE;
                }
            }
        }
    }

    if (app == NULL) {
        CheckTime(startTime, String("startProcess: creating new process record"));
        app = NewProcessRecordLocked(info, processName, isolated, isolatedUid);
        if (app == NULL) {
            Slogger::W(TAG, "Failed making new process record for %s / %d isolated=%d",
                processName.string(), infoUid, isolated);
            return NULL;
        }
        app->mCrashHandler = crashHandler;
        mProcessNames->Put(processName, app->mUid, app);
        if (isolated) {
            mIsolatedProcesses[app->mUid] = app;
        }
        CheckTime(startTime, String("startProcess: done creating new process record"));
    }
    else {
        String infoPackageName;
        IPackageItemInfo::Probe(info)->GetPackageName(&infoPackageName);
        Int32 versionCode;
        info->GetVersionCode(&versionCode);
        // If this is a new package in the process, add the package to the list
        app->AddPackage(infoPackageName, versionCode, mProcessStats);
        CheckTime(startTime, String("startProcess: added package to existing proc"));
    }

    // If the system is not ready yet, then hold off on starting this
    // process until it is.
    if (!mProcessesReady
            && !IsAllowedWhileBooting(info)
            && !allowWhileBooting) {
        Boolean contains;
        if (mProcessesOnHold->Contains((IProcessRecord*)app.Get(), &contains), !contains) {
            mProcessesOnHold->Add((IProcessRecord*)app.Get());
        }
        if (DEBUG_PROCESSES) {
            String appDes = app->ToString();
            Slogger::V(TAG, "System not ready, putting on hold: %s", appDes.string());
        }
        CheckTime(startTime, String("startProcess: returning with proc on hold"));
        return app;
    }

    CheckTime(startTime, String("startProcess: stepping in to startProcess"));
    StartProcessLocked(app, hostingType, hostingNameStr, abiOverride,
        entryPoint, entryPointArgs);
    CheckTime(startTime, String("startProcess: done starting proc!"));
    return (app->mPid != 0) ? app : NULL;
}

Boolean CActivityManagerService::IsAllowedWhileBooting(
    /* [in] */ IApplicationInfo* ai)
{
    assert(ai != NULL);

    Int32 flags;
    ai->GetFlags(&flags);
    return (flags & IApplicationInfo::FLAG_PERSISTENT) != 0;
}

ECode CActivityManagerService::StartProcessLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& hostingType,
    /* [in] */ const String& hostingNameStr)
{
    String nullStr;
    return StartProcessLocked(app, hostingType, hostingNameStr, nullStr /* abiOverride */,
        nullStr /* entryPoint */, NULL /* entryPointArgs */);
}

ECode CActivityManagerService::StartProcessLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& hostingType,
    /* [in] */ const String& hostingNameStr,
    /* [in] */ const String& abiOverride,
    /* [in] */ const String& _entryPoint,
    /* [in] */ ArrayOf<String>* entryPointArgs)
{
    Slogger::I(TAG, " >> StartProcessLocked: hostingType:%s, hostingName:%s, abiOverride:%s, entryPoint:%s",
        hostingType.string(), hostingNameStr.string(), abiOverride.string(), _entryPoint.string());
    ECode ec = NOERROR;
    String entryPoint = _entryPoint;
    Int64 startTime = SystemClock::GetElapsedRealtime();
    if (app->mPid > 0 && app->mPid != MY_PID) {
        CheckTime(startTime, String("startProcess: removing from pids map"));
        {
            AutoLock lock(mPidsSelfLockedLock);
            mPidsSelfLocked.Erase(app->mPid);
            mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, TO_IINTERFACE(app));
        }
        CheckTime(startTime, String("startProcess: done removing from pids map"));
        app->SetPid(0);
    }

    AutoPtr<ProcessRecord> temp = app;
    Boolean contains;
    if (DEBUG_PROCESSES &&
            (mProcessesOnHold->Contains((IProcessRecord*)app, &contains), contains)) {
        Slogger::V(TAG, "startProcessLocked removing on hold: ", TO_CSTR(app));
    }
    mProcessesOnHold->Remove((IProcessRecord*)app);

    CheckTime(startTime, String("startProcess: starting to update cpu stats"));
    UpdateCpuStats();
    CheckTime(startTime, String("startProcess: done updating cpu stats"));

//    try {
    Int32 uid = app->mUid;
    AutoPtr<ArrayOf<Int32> > gids;
    Int32 mountExternal = IZygote::MOUNT_EXTERNAL_NONE;
    if (!app->mIsolated) {
        AutoPtr<ArrayOf<Int32> > permGids;
        // try {
            CheckTime(startTime, String("startProcess: getting gids from package manager"));
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            String packageName;
            IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&packageName);
            pm->GetPackageGids(packageName, (ArrayOf<Int32>**)&permGids);

            AutoPtr<IEnvironment> env;
            CEnvironment::AcquireSingleton((IEnvironment**)&env);
            Boolean isEmulated;
            if (env->IsExternalStorageEmulated(&isEmulated), isEmulated) {
                CheckTime(startTime, String("startProcess: checking external storage perm"));
                Int32 res;
                pm->CheckPermission(
                    Manifest::permission::ACCESS_ALL_EXTERNAL_STORAGE,
                    packageName, &res);
                if (res == IPackageManager::PERMISSION_GRANTED) {
                    mountExternal = IZygote::MOUNT_EXTERNAL_MULTIUSER_ALL;
                }
                else {
                    mountExternal = IZygote::MOUNT_EXTERNAL_MULTIUSER;
                }
            }
        // } catch (PackageManager.NameNotFoundException e) {
        //     Slogger::W(TAG, "Unable to retrieve gids", e);
        // }

        /*
         * Add shared application and profile GIDs so applications can share some
         * resources like shared libraries and access user-wide resources
         */
        if (permGids == NULL) {
            gids = ArrayOf<Int32>::Alloc(2);
        }
        else {
            gids = ArrayOf<Int32>::Alloc(permGids->GetLength() + 2);
            gids->Copy(2, permGids, permGids->GetLength());
        }

        Int32 appGid = UserHandle::GetSharedAppGid(UserHandle::GetAppId(uid));
        Int32 userGid = UserHandle::GetUid(UserHandle::GetUserId(uid), IProcess::SHARED_USER_GID);
        (*gids)[0] = appGid;
        (*gids)[1] = userGid;
    }
    CheckTime(startTime, String("startProcess: building args"));
    Int32 flags;
    app->mInfo->GetFlags(&flags);
    String tPkgName;
    if (mFactoryTest != FactoryTest::FACTORY_TEST_OFF) {
        if (mFactoryTest == FactoryTest::FACTORY_TEST_LOW_LEVEL
            && mTopComponent != NULL
            && (mTopComponent->GetPackageName(&tPkgName), app->mProcessName.Equals(tPkgName))) {
            uid = 0;
        }

        if (mFactoryTest == FactoryTest::FACTORY_TEST_HIGH_LEVEL
           && (flags & IApplicationInfo::FLAG_FACTORY_TEST) != 0) {
           uid = 0;
        }
    }

    Int32 debugFlags = 0;
    if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) != 0) {
        debugFlags |= IZygote::DEBUG_ENABLE_DEBUGGER;
        // Also turn on CheckJNI for debuggable apps. It's quite
        // awkward to turn on otherwise.
        debugFlags |= IZygote::DEBUG_ENABLE_CHECKJNI;
    }
    // Run the app in safe mode if its manifest requests so or the
    // system is booted in safe mode.
    if ((flags & IApplicationInfo::FLAG_VM_SAFE_MODE) != 0 ||
        mSafeMode == TRUE) {
        debugFlags |= IZygote::DEBUG_ENABLE_SAFEMODE;
    }

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    if (sysProp->Get(String("debug.checkjni"), &value), value.Equals("1")) {
        debugFlags |= IZygote::DEBUG_ENABLE_CHECKJNI;
    }
    if (sysProp->Get(String("debug.jni.logging"), &value), value.Equals("1")) {
        debugFlags |= IZygote::DEBUG_ENABLE_JNI_LOGGING;
    }
    if (sysProp->Get(String("debug.assert"), &value), value.Equals("1")) {
        debugFlags |= IZygote::DEBUG_ENABLE_ASSERT;
    }

    //Check if zygote should refresh its fonts
    Boolean refreshTheme = FALSE;
    Boolean propValue = FALSE;
    if (sysProp->GetBoolean(PROP_REFRESH_THEME, FALSE, &propValue), propValue) {
        sysProp->Set(PROP_REFRESH_THEME, String("false"));
        refreshTheme = TRUE;
    }

    String requiredAbi, primaryCpuAbi;
    if (abiOverride != NULL) {
        requiredAbi = abiOverride;
    }
    else {
        app->mInfo->GetPrimaryCpuAbi(&primaryCpuAbi);
        requiredAbi = primaryCpuAbi;
    }

    if (requiredAbi == NULL) {
        requiredAbi = (*Build::SUPPORTED_ABIS)[0];
    }

    String instructionSet;
    if (primaryCpuAbi != NULL) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetInstructionSet(primaryCpuAbi, &instructionSet);
    }

    // Start the process.  It will either succeed and return a result containing
    // the PID of the new process, or else throw a RuntimeException.
    Boolean isActivityProcess = (entryPoint.IsNull());
    if (isActivityProcess) {
        entryPoint = "android.app.ActivityThread";
    }
    CheckTime(startTime, String("startProcess: asking zygote to start proc"));

    Int32 targetSdkVersion;
    String sourcePath, dataDir, seinfo, processClass;
    app->mInfo->GetSourceDir(&sourcePath);
    app->mInfo->GetDataDir(&dataDir);
    app->mInfo->GetTargetSdkVersion(&targetSdkVersion);
    app->mInfo->GetSeinfo(&seinfo);
    processClass = sourcePath.EndWith(".apk") && !sourcePath.Contains("framework-res.apk") ? entryPoint : String("Elastos.Droid.App.CActivityThreadHelper");
    Slogger::I(TAG, " >>> StartProcessLocked %s, ProcessName:%s, processClass: %s, dataDir: %s, seinfo: %s",
        sourcePath.string(), app->mProcessName.string(), processClass.string(),
        dataDir.string(), seinfo.string());

    AutoPtr<IProcessStartResult> startResult;
    ec = Process::Start(processClass, app->mProcessName,
        uid, uid, gids, debugFlags, mountExternal,
        targetSdkVersion, seinfo, requiredAbi, instructionSet, dataDir,
        refreshTheme, entryPointArgs, (IProcessStartResult**)&startResult);
    if (FAILED(ec)) {
        Slogger::E(TAG, "faield to StartProcessLocked %s, ProcessName:%s",
            sourcePath.string(), app->mProcessName.string());
        return ec;
    }
    CheckTime(startTime, String("startProcess: returned from zygote!"));

    Int32 infoUid;
    app->mInfo->GetUid(&infoUid);
    if (app->mIsolated) {
        mBatteryStatsService->AddIsolatedUid(app->mUid, infoUid);
    }
    mBatteryStatsService->NoteProcessStart(app->mProcessName, infoUid);
    CheckTime(startTime, String("startProcess: done updating battery stats"));

    Int32 pid = 0;
    startResult->GetPid(&pid);
    if (app->mPersistent) {
       Watchdog::GetInstance()->ProcessStarted(app->mProcessName, pid);
    }

    CheckTime(startTime, String("startProcess: building log message"));
    StringBuilder* buf = mStringBuilder;
    buf->Reset();
    buf->Append(String("Start proc "));
    buf->Append(app->mProcessName);
    if (!isActivityProcess) {
        buf->Append(" [");
        buf->Append(entryPoint);
        buf->Append("]");
    }
    buf->Append(" for ");
    buf->Append(hostingType);
    if (!hostingNameStr.IsNull()) {
        buf->Append(" ");
        buf->Append(hostingNameStr);
    }
    buf->Append(": pid=");
    buf->Append(pid);
    buf->Append(" uid=");
    buf->Append(uid);
    buf->Append(" gids={");
    if (gids != NULL) {
        for (Int32 gi = 0; gi < gids->GetLength(); gi++) {
            if (gi != 0) buf->Append(", ");
            buf->Append((*gids)[gi]);
        }
    }
    buf->Append("}");
    if (requiredAbi != NULL) {
        buf->Append(" abi=");
        buf->Append(requiredAbi);
    }
    Slogger::I(TAG, buf->ToString());

    Boolean usingWrapper;
    startResult->GetUsingWrapper(&usingWrapper);
    app->SetPid(pid);
    app->mUsingWrapper = usingWrapper;
    app->mRemoved = FALSE;
    app->mKilled = FALSE;
    app->mKilledByAm = FALSE;
    CheckTime(startTime, String("startProcess: starting to update pids map"));
    {
        AutoLock lock(mPidsSelfLockedLock);
        mPidsSelfLocked[pid] = app;
        if (isActivityProcess) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(PROC_START_TIMEOUT_MSG, (IMessage**)&msg);
            msg->SetObj(TO_IINTERFACE(app));
            Boolean result;
            mHandler->SendMessageDelayed(msg,
                usingWrapper ? PROC_START_TIMEOUT_WITH_WRAPPER : PROC_START_TIMEOUT,
                &result);
        }
        CheckTime(startTime, String("startProcess: done updating pids map"));
    }
    // } catch (RuntimeException e) {
    //     // XXX do better error recovery.
    //     app.setPid(0);
    //     mBatteryStatsService.noteProcessFinish(app.processName, app.info.uid);
    //     if (app.isolated) {
    //         mBatteryStatsService.removeIsolatedUid(app.uid, app.info.uid);
    //     }
    //     Slog.e(TAG, "Failure starting process " + app.processName, e);
    // }
    return NOERROR;
}

ECode CActivityManagerService::UpdateUsageStats(
    /* [in] */ ActivityRecord* component,
    /* [in] */ Boolean resumed)
{
    if (DEBUG_SWITCH)
        Slogger::D(TAG, "updateUsageStats: comp=%s res=%d", TO_CSTR(component), resumed);
    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    if (resumed) {
        if (mUsageStatsService != NULL) {
            mUsageStatsService->ReportEvent(component->mRealActivity, component->mUserId,
                    IUsageEvent::MOVE_TO_FOREGROUND);
        }
        {
            AutoLock syncLock(stats);
            stats->NoteActivityResumedLocked(component->mApp->mUid);
        }
    }
    else {
        if (mUsageStatsService != NULL) {
            mUsageStatsService->ReportEvent(component->mRealActivity, component->mUserId,
                    IUsageEvent::MOVE_TO_BACKGROUND);
        }
        {
            AutoLock syncLock(stats);
            stats->NoteActivityPausedLocked(component->mApp->mUid);
        }
    }

    return NOERROR;
}

AutoPtr<IIntent> CActivityManagerService::GetHomeIntent()
{
    AutoPtr<IUri> uri;
    if (mTopData != NULL) {
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        uriHelper->Parse(mTopData, (IUri**)&uri);
    }
    AutoPtr<IIntent> intent;
    CIntent::New(mTopAction, uri, (IIntent**)&intent);
    intent->SetComponent(mTopComponent);
    if (mFactoryTest != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
        intent->AddCategory(IIntent::CATEGORY_HOME);
    }
    return intent;
}

Boolean CActivityManagerService::StartHomeActivityLocked(
    /* [in] */ Int32 userId)
{
    Slogger::I(TAG, "StartHomeActivityLocked, mTopAction: %s", mTopAction.string());

    if (mFactoryTest == FactoryTest::FACTORY_TEST_LOW_LEVEL
            && mTopAction.IsNull()) {
        // We are running in factory test mode, but unable to find
        // the factory test app, so just sit around displaying the
        // error message and don't try to start anything.
        return FALSE;
    }

    AutoPtr<IIntent> intent = GetHomeIntent();
    Slogger::I(TAG, "StartHomeActivityLocked, GetHomeIntent: %s", TO_CSTR(intent));
    AutoPtr<IActivityInfo> aInfo = ResolveActivityInfo(intent, STOCK_PM_FLAGS, userId);
    Slogger::I(TAG, "StartHomeActivityLocked, ResolveActivityInfo: %s", TO_CSTR(aInfo));
    if (aInfo != NULL) {
        IComponentInfo* ci = IComponentInfo::Probe(aInfo);
        AutoPtr<IApplicationInfo> appInfo;
        ci->GetApplicationInfo((IApplicationInfo**)&appInfo);

        String cname, name, pname;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&cname);
        IPackageItemInfo::Probe(aInfo)->GetName(&name);
        AutoPtr<IComponentName> component;
        CComponentName::New(cname, name, (IComponentName**)&component);
        intent->SetComponent((IComponentName*)component);

        // Don't do this if the home app is currently being
        // instrumented.
        AutoPtr<IActivityInfo> temp = aInfo;
        aInfo = NULL;
        CActivityInfo::New(temp, (IActivityInfo**)&aInfo);
        ci = IComponentInfo::Probe(aInfo);

        appInfo = NULL;
        ci->GetApplicationInfo((IApplicationInfo**)&appInfo);

        AutoPtr<IApplicationInfo> newAppInfo = GetAppInfoForUser(appInfo, userId);
        ci->SetApplicationInfo(newAppInfo);

        Int32 uid;
        newAppInfo->GetUid(&uid);
        ci->GetProcessName(&pname);
        AutoPtr<ProcessRecord> app = GetProcessRecordLocked(pname, uid, TRUE);
        if (app == NULL || app->mInstrumentationClass == NULL) {
            Int32 flags;
            intent->GetFlags(&flags);
            intent->SetFlags(flags | IIntent::FLAG_ACTIVITY_NEW_TASK);
            mStackSupervisor->StartHomeActivity(intent, aInfo);
        }
    }

    return TRUE;
}

AutoPtr<IActivityInfo> CActivityManagerService::ResolveActivityInfo(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId)
{
    AutoPtr<IActivityInfo> ai;
    AutoPtr<IComponentName> comp;
    intent->GetComponent((IComponentName**)&comp);
//    try {
        if (comp != NULL) {
            AppGlobals::GetPackageManager()->GetActivityInfo(comp, flags, userId,
                    (IActivityInfo**)&ai);
        }
        else {
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            String resolvedType;
            intent->ResolveTypeIfNeeded(resolver, &resolvedType);
            AutoPtr<IResolveInfo> info;
            AppGlobals::GetPackageManager()->ResolveIntent(intent, resolvedType, flags,
                    userId, (IResolveInfo**)&info);

            if (info != NULL) {
                info->GetActivityInfo((IActivityInfo**)&ai);
            }
            else {
                Slogger::E(TAG, "Failed to resolve activity info for %s", TO_CSTR(intent));
            }
        }
//    } catch (RemoteException e) {
//    // ignore
//    }
    return ai;
}

ECode CActivityManagerService::StartSetupActivityLocked()
{
    // Only do this once per boot.
    if (mCheckedForSetup) {
        return NOERROR;
    }

    // We will show this screen if the current one is a different
    // version than the last one shown, and we are not running in
    // low-level factory test mode.
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);
    if (mFactoryTest != FactoryTest::FACTORY_TEST_LOW_LEVEL &&
            value != 0) {
        mCheckedForSetup = TRUE;

        // See if we should be showing the platform update setup UI.
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_UPGRADE_SETUP,(IIntent**)&intent);
        AutoPtr<IPackageManager> pkgManager;
        mContext->GetPackageManager((IPackageManager**)&pkgManager);
        AutoPtr<IList> ris;
        pkgManager->QueryIntentActivities(intent, IPackageManager::GET_META_DATA, (IList**)&ris);

        // We don't allow third party apps to replace this.
        AutoPtr<IResolveInfo> ri;
        Int32 size = 0;
        if (ris != NULL)
            ris->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            ris->Get(i, (IInterface**)&item);
            AutoPtr<IResolveInfo> rInfo = IResolveInfo::Probe(item);
            AutoPtr<IActivityInfo> aInfo;
            rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                ri = rInfo;
                break;
            }
        }

        String nullStr;
        if (ri != NULL) {
            AutoPtr<IActivityInfo> aInfo;
            ri->GetActivityInfo((IActivityInfo**)&aInfo);
            IPackageItemInfo* pi = IPackageItemInfo::Probe(aInfo);
            AutoPtr<IBundle> metaData;
            pi->GetMetaData((IBundle**)&metaData);
            String vers;
            if (metaData != NULL) {
                metaData->GetString(IIntent::METADATA_SETUP_VERSION, &vers);
            }

            if (vers.IsNull()) {
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(pi)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                AutoPtr<IBundle> appMetaData;
                IPackageItemInfo::Probe(appInfo)->GetMetaData((IBundle**)&appMetaData);
                if (appMetaData != NULL) {
                    appMetaData->GetString(IIntent::METADATA_SETUP_VERSION, &vers);
                }
            }

            String lastVers;
            Settings::Secure::GetString(resolver, ISettingsSecure::LAST_SETUP_SHOWN, &lastVers);
            if (!vers.IsNull() && !vers.Equals(lastVers)) {
                intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                String pkgName, name;
                pi->GetPackageName(&pkgName);
                pi->GetName(&name);
                AutoPtr<IComponentName> component;
                CComponentName::New(pkgName, name, (IComponentName**)&component);
                intent->SetComponent(component);
                Int32 status;
                mStackSupervisor->StartActivityLocked(NULL, intent, nullStr, aInfo,
                   NULL, NULL, NULL, nullStr, 0, 0, 0, nullStr, 0, 0, 0, NULL,
                   FALSE, NULL, NULL, NULL, &status);
            }
        }
    }
    return NOERROR;
}

AutoPtr<ICompatibilityInfo> CActivityManagerService::CompatibilityInfoForPackageLocked(
    /* [in] */ IApplicationInfo* ai)
{
    return mCompatModePackages->CompatibilityInfoForPackageLocked(ai);
}

ECode CActivityManagerService::EnforceNotIsolatedCaller(
    /* [in] */ const String& caller)
{
    Boolean isIsolated = UserHandle::IsIsolated(Binder::GetCallingUid());
    if (isIsolated) {
        //throw new SecurityException("Isolated process not allowed to call " + caller);
        Slogger::E(TAG, "Isolated process not allowed to call %s", caller.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityManagerService::EnforceShellRestriction(
    /* [in] */ const String& restriction,
    /* [in] */ Int32 userHandle)
{
    if (Binder::GetCallingUid() == IProcess::SHELL_UID) {
        Boolean res;
        if (userHandle < 0 ||
            (mUserManager->HasUserRestriction(restriction, userHandle, &res), res)) {
            Slogger::E(TAG, "Shell does not have permission to access user %d", userHandle);
            return E_SECURITY_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetFrontActivityScreenCompatMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = IActivityManager::COMPAT_MODE_UNKNOWN;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getFrontActivityScreenCompatMode")));
    {
        AutoLock lock(this);
        return mCompatModePackages->GetFrontActivityScreenCompatModeLocked();
    }

    return NOERROR;
}

ECode CActivityManagerService::SetFrontActivityScreenCompatMode(
    /* [in] */ Int32 mode)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_SCREEN_COMPATIBILITY,
            String("setFrontActivityScreenCompatMode")));
    {
        AutoLock lock(this);
        mCompatModePackages->SetFrontActivityScreenCompatModeLocked(mode);
    }
    return NOERROR;
}


ECode CActivityManagerService::GetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = 0;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getPackageScreenCompatMode")));
    {
        AutoLock lock(this);
        *mode = mCompatModePackages->GetPackageScreenCompatModeLocked(packageName);
    }
    return NOERROR;
}


ECode CActivityManagerService::SetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_SCREEN_COMPATIBILITY,
            String("setPackageScreenCompatMode")));
    {
        AutoLock lock(this);
        mCompatModePackages->SetPackageScreenCompatModeLocked(packageName, mode);
    }
    return NOERROR;
}

ECode CActivityManagerService::GetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* compat)
{
    VALIDATE_NOT_NULL(compat);
    *compat = FALSE;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getPackageAskScreenCompat")));
    {
        AutoLock lock(this);
        *compat = mCompatModePackages->GetPackageAskCompatModeLocked(packageName);
    }
    return NOERROR;
}

ECode CActivityManagerService::SetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean ask)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_SCREEN_COMPATIBILITY,
            String("setPackageAskScreenCompat")));
    {
        AutoLock lock(this);
        mCompatModePackages->SetPackageAskCompatModeLocked(packageName, ask);
    }
    return NOERROR;
}

ECode CActivityManagerService::DispatchProcessesChanged()
{
    Int32 N;
    {
        AutoLock lock(this);
        N = mPendingProcessChanges.GetSize();
        if (mActiveProcessChanges->GetLength() < N) {
            mActiveProcessChanges = ArrayOf<ProcessChangeItem*>::Alloc(N);
        }
        List< AutoPtr<ProcessChangeItem> >::Iterator it = mPendingProcessChanges.Begin();
        for (Int32 i = 0; it != mPendingProcessChanges.End(); ++it, ++i) {
            mActiveProcessChanges->Set(i, *it);
        }
        mAvailProcessChanges.Merge(mPendingProcessChanges);
        mPendingProcessChanges.Clear();
        if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "*** Delivering %d process changes", N);
    }
    Int32 i;
    mProcessObservers->BeginBroadcast(&i);
    while (i > 0) {
        i--;
        AutoPtr<IInterface> bitem;
        mProcessObservers->GetBroadcastItem(i, (IInterface**)&bitem);
        AutoPtr<IIProcessObserver> observer = IIProcessObserver::Probe(bitem);
        if (observer != NULL) {
//             try {
            for (Int32 j = 0; j < N; j++) {
                AutoPtr<ProcessChangeItem> item = (*mActiveProcessChanges)[j];
                if ((item->mChanges & ProcessChangeItem::CHANGE_ACTIVITIES) != 0) {
                    if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "ACTIVITIES CHANGED pid=%d uid=%d: %d",
                            item->mPid, item->mUid, item->mForegroundActivities);
                    observer->OnForegroundActivitiesChanged(item->mPid, item->mUid,
                            item->mForegroundActivities);
                }
                if ((item->mChanges & ProcessChangeItem::CHANGE_PROCESS_STATE) != 0) {
                    if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "PROCSTATE CHANGED pid=%d uid=%d: %d",
                            item->mPid, item->mUid, item->mProcessState);
                    observer->OnProcessStateChanged(item->mPid, item->mUid,
                            item->mProcessState);
                }
            }
//             } catch (RemoteException e) {
//             }
        }
    }
    mProcessObservers->FinishBroadcast();
    return NOERROR;
}

ECode CActivityManagerService::DispatchProcessDied(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    Int32 i;
    mProcessObservers->BeginBroadcast(&i);
    while (i > 0) {
        i--;
        AutoPtr<IInterface> item;
        mProcessObservers->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IIProcessObserver> observer = IIProcessObserver::Probe(item);
        if (observer != NULL) {
//             try {
            observer->OnProcessDied(pid, uid);
//             } catch (RemoteException e) {
//             }
        }
    }
    mProcessObservers->FinishBroadcast();
    return NOERROR;
}

ECode CActivityManagerService::StartActivity(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    return StartActivityAsUser(caller, callingPackage, intent, resolvedType, resultTo,
        resultWho, requestCode, startFlags, profilerInfo, options, UserHandle::GetCallingUserId(), result);
}

ECode CActivityManagerService::StartActivityAsUser(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    // Slogger::I(TAG, " >> memory leak StartActivityAsUser: %s, intent: %s", callingPackage.string(), TO_CSTR(intent));
    // MemoryDumper::Dump();

    VALIDATE_NOT_NULL(result);
    *result = 0;

    String nullStr;
    String name("startActivity");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));
    return mStackSupervisor->StartActivityMayWait(caller, -1, callingPackage, intent,
        resolvedType, NULL, NULL, resultTo, resultWho, requestCode, startFlags,
        profilerInfo, NULL, NULL, options, userId, NULL, NULL, result);
}

ECode CActivityManagerService::StartActivityAsCaller(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    // This is very dangerous -- it allows you to perform a start activity (including
    // permission grants) as any app that may launch one of your own activities.  So
    // we will only allow this to be done from activities that are part of the core framework,
    // and then only when they are running as the system.
    AutoPtr<ActivityRecord> sourceRecord;
    Int32 targetUid = 0;
    String targetPackage;
    {    AutoLock syncLock(this);
        if (resultTo == NULL) {
            Slogger::E(TAG, "Must be called from an activity");
            return E_SECURITY_EXCEPTION;
        }
        sourceRecord = mStackSupervisor->IsInAnyStackLocked(resultTo);
        if (sourceRecord == NULL) {
            Slogger::E(TAG, "Called with bad activity token: %s", TO_CSTR(resultTo));
            return E_SECURITY_EXCEPTION;
        }
        String packageName;
        IPackageItemInfo::Probe(sourceRecord->mInfo)->GetPackageName(&packageName);
        if (!packageName.Equals("android")) {
            Slogger::E(TAG,
                    "Must be called from an activity that is declared in the android package");
            return E_SECURITY_EXCEPTION;
        }
        if (sourceRecord->mApp == NULL) {
            Slogger::E(TAG, "Called without a process attached to activity");
            return E_SECURITY_EXCEPTION;
        }
        if (UserHandle::GetAppId(sourceRecord->mApp->mUid) != IProcess::SYSTEM_UID) {
            // This is still okay, as long as this activity is running under the
            // uid of the original calling activity.
            if (sourceRecord->mApp->mUid != sourceRecord->mLaunchedFromUid) {
                Slogger::E(TAG,
                    "Calling activity in uid %d must be system uid or original calling uid %d",
                    sourceRecord->mApp->mUid, sourceRecord->mLaunchedFromUid);
                return E_SECURITY_EXCEPTION;
            }
        }
        targetUid = sourceRecord->mLaunchedFromUid;
        targetPackage = sourceRecord->mLaunchedFromPackage;
    }

    if (userId == IUserHandle::USER_NULL) {
        userId = UserHandle::GetUserId(sourceRecord->mApp->mUid);
    }

    // TODO: Switch to user app stacks here.
    // try {
        return mStackSupervisor->StartActivityMayWait(NULL, targetUid, targetPackage, intent,
                resolvedType, NULL, NULL, resultTo, resultWho, requestCode, startFlags, NULL,
                NULL, NULL, options, userId, NULL, NULL, result);
    // } catch (SecurityException e) {
    //     // XXX need to figure out how to propagate to original app.
    //     // A SecurityException here is generally actually a fault of the original
    //     // calling activity (such as a fairly granting permissions), so propagate it
    //     // back to them.
    //     /*
    //     StringBuilder msg = new StringBuilder();
    //     msg.append("While launching");
    //     msg.append(intent.toString());
    //     msg.append(": ");
    //     msg.append(e.getMessage());
    //     */
    //     throw e;
    // }
}

ECode CActivityManagerService::StartActivityAndWait(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ IActivityManagerWaitResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    String nullStr;
    String name("startActivityAndWait");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));
    FAIL_RETURN(CActivityManagerWaitResult::New(result));
    Int32 status;
    return mStackSupervisor->StartActivityMayWait(caller, -1, callingPackage, intent, resolvedType,
        NULL, NULL, resultTo, resultWho, requestCode, startFlags, profilerInfo,
        *result, NULL, options, userId, NULL, NULL, &status);
}

ECode CActivityManagerService::StartActivityWithConfig(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IConfiguration* config,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    String nullStr;
    String name("startActivityWithConfig");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));
    return mStackSupervisor->StartActivityMayWait(caller, -1, callingPackage, intent,
        resolvedType, NULL, NULL, resultTo, resultWho, requestCode, startFlags,
        NULL, NULL, config, options, userId, NULL, NULL, result);
}

ECode CActivityManagerService::StartActivityIntentSender(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    String name("startActivityIntentSender");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));

    // Refuse possible leaked file descriptors
    Boolean hasFd;
    if (fillInIntent != NULL &&
            (fillInIntent->HasFileDescriptors(&hasFd), hasFd)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IIIntentSender> sender;
    intent->GetTarget((IIIntentSender**)&sender);
    if (IPendingIntentRecord::Probe(sender) == NULL) {
        Slogger::E(TAG, "Bad PendingIntent object");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CPendingIntentRecord* pir = (CPendingIntentRecord*)IPendingIntentRecord::Probe(sender);

    {
        AutoLock lock(this);

        // If this is coming from the currently resumed activity, it is
        // effectively saying that app switches are allowed at this point.
        AutoPtr<ActivityStack> stack = GetFocusedStack();
        if (stack->mResumedActivity != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(stack->mResumedActivity->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            if (uid == Binder::GetCallingUid()) {
                mAppSwitchesAllowedTime = 0;
            }
        }
    }

    String nullStr;
    *status = pir->SendInner(0, fillInIntent, resolvedType, NULL,
            nullStr, resultTo, resultWho, requestCode, flagsMask, flagsValues, options, NULL);
    return NOERROR;
}

ECode CActivityManagerService::StartVoiceActivity(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIVoiceInteractionSession* session,
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (CheckCallingPermission(Manifest::permission::BIND_VOICE_INTERACTION)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg;
        msg += "Permission Denial: startVoiceActivity() from pid=";
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += Manifest::permission::BIND_VOICE_INTERACTION;
        Slogger::W(TAG, msg.ToString());
        return E_SECURITY_EXCEPTION;
    }
    if (session == NULL || interactor == NULL) {
        Slogger::E(TAG, "NULL session or interactor");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(HandleIncomingUser(callingPid, callingUid, userId, FALSE,
        ALLOW_FULL_ONLY, String("startVoiceActivity"), String(NULL), &userId));
    // TODO: Switch to user app stacks here.
    return mStackSupervisor->StartActivityMayWait(NULL, callingUid, callingPackage, intent,
        resolvedType, session, interactor, NULL, String(NULL), 0, startFlags, profilerInfo, NULL,
        NULL, options, userId, NULL, NULL, result);
}

ECode CActivityManagerService::StartNextMatchingActivity(
    /* [in] */ IBinder* callingActivity,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // Refuse possible leaked file descriptors
    Boolean hasFD;

    if (intent != NULL && (intent->HasFileDescriptors(&hasFD),hasFD)) {
        Slogger::E(TAG, "File descriptors passed in Intent.\n");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock lock(this);

        AutoPtr<IActivityOptionsHelper> helper;
        FAIL_RETURN(CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper));

        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(callingActivity);
        if (r == NULL) {
            return helper->Abort(options);
        }
        if (r->mApp == NULL || r->mApp->mThread == NULL) {
            // The caller is not running...  d'oh!
            return helper->Abort(options);
        }
        AutoPtr<IIntent> newIntent;
        FAIL_RETURN(CIntent::New(intent, (IIntent**)&newIntent));
        // The caller is not allowed to change the data.
        String intentType;
        FAIL_RETURN(r->mIntent->GetType(&intentType));
        AutoPtr<IUri> uriData;
        FAIL_RETURN(r->mIntent->GetData((IUri**)&uriData));
        FAIL_RETURN(newIntent->SetDataAndType(uriData, intentType));
        // And we are resetting to find the next component...
        FAIL_RETURN(newIntent->SetComponent(NULL));

        Int32 flags;
        intent->GetFlags(&flags);
        Boolean debug = ((flags & IIntent::FLAG_DEBUG_LOG_RESOLUTION) != 0);

        AutoPtr<IActivityInfo> aInfo;
        // try {
        AutoPtr<IList> resolves;
        AppGlobals::GetPackageManager()->QueryIntentActivities(
            intent, r->mResolvedType,
            IPackageManager::MATCH_DEFAULT_ONLY | STOCK_PM_FLAGS,
            UserHandle::GetCallingUserId(), (IList**)&resolves);

        // Look for the original activity in the list...
        AutoPtr<IResolveInfo> rInfo;
        String cName, name, rName;
        Int32 N = 0;
        if (resolves != NULL)
            resolves->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> item;
            resolves->Get(i, (IInterface**)&item);
            rInfo = IResolveInfo::Probe(item);
            aInfo = NULL;
            rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
            IPackageItemInfo::Probe(aInfo)->GetPackageName(&cName);
            IPackageItemInfo::Probe(aInfo)->GetName(&name);
            IPackageItemInfo::Probe(r->mInfo)->GetName(&rName);
            if (cName.Equals(r->mPackageName) && name.Equals(rName)) {
                // We found the current one...  the next matching is
                // after it.
                i++;
                if (i < N) {
                    item = NULL;
                    resolves->Get(i, (IInterface**)&item);
                    rInfo = IResolveInfo::Probe(item);
                    aInfo = NULL;
                    rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
                }
                if (debug) {
                    Slogger::V(TAG, "Next matching activity: found current %s/%s",
                        r->mPackageName.string(), rName.string());
                    Slogger::V(TAG, "Next matching activity: next is %s/%s",
                        cName.string(), name.string());
                }
                break;
            }
        }
        // } catch (RemoteException e) {
        // }

        if (aInfo == NULL) {
            // Nobody who is next!
            helper->Abort(options);
            if (debug) Slogger::D(TAG, "Next matching activity: nothing found");
            return NOERROR;
        }

        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String appCName, aName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&appCName);
        IPackageItemInfo::Probe(aInfo)->GetName(&aName);
        AutoPtr<IComponentName> component;
        CComponentName::New(appCName, aName,(IComponentName**)&component);
        newIntent->SetComponent(component);
        newIntent->GetFlags(&flags);
        newIntent->SetFlags(flags&~(
            IIntent::FLAG_ACTIVITY_FORWARD_RESULT|
            IIntent::FLAG_ACTIVITY_CLEAR_TOP|
            IIntent::FLAG_ACTIVITY_MULTIPLE_TASK|
            IIntent::FLAG_ACTIVITY_NEW_TASK));

        // Okay now we need to start the new activity, replacing the
        // currently running activity.  This is a little tricky because
        // we want to start the new one as if the current one is finished,
        // but not finish the current one first so that there is no flicker.
        // And thus...
        const Boolean wasFinishing = r->mFinishing;
        r->mFinishing = TRUE;

        // Propagate reply information over to the new activity.
        AutoPtr<ActivityRecord> resultTo = r->mResultTo;
        const String resultWho = r->mResultWho;
        const Int32 requestCode = r->mRequestCode;
        r->mResultTo = NULL;
        if (resultTo != NULL) {
            resultTo->RemoveResultsLocked(r, resultWho, requestCode);
        }

        const Int64 origId = Binder::ClearCallingIdentity();
        Int32 res;
        mStackSupervisor->StartActivityLocked(r->mApp->mThread, newIntent,
                r->mResolvedType, aInfo, NULL, NULL,
                resultTo != NULL ? IBinder::Probe(resultTo->mAppToken) : NULL,
                resultWho, requestCode, -1, r->mLaunchedFromUid, r->mLaunchedFromPackage, -1,
                r->mLaunchedFromUid,  0, options, FALSE, NULL, NULL, NULL, &res);
        Binder::RestoreCallingIdentity(origId);

        r->mFinishing = wasFinishing;
        if (res != IActivityManager::START_SUCCESS) {
            return NOERROR;
        }
        *result = TRUE;
    }
    return NOERROR;
}

ECode CActivityManagerService::StartActivityFromRecents(
    /* [in] */ Int32 taskId,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (CheckCallingPermission(Manifest::permission::START_TASKS_FROM_RECENTS)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: startActivityFromRecents called without ");
        msg += Manifest::permission::START_TASKS_FROM_RECENTS;
        Slogger::W(TAG, msg.ToString());
        return E_SECURITY_EXCEPTION;
    }
    return StartActivityFromRecentsInner(taskId, options, result);
}

ECode CActivityManagerService::StartActivityFromRecentsInner(
    /* [in] */ Int32 taskId,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    AutoPtr<TaskRecord> task;
    Int32 callingUid = 0;
    String callingPackage;
    AutoPtr<IIntent> intent;
    Int32 userId = 0;
    {    AutoLock syncLock(this);
        task = RecentTaskForIdLocked(taskId);
        if (task == NULL) {
            Slogger::E(TAG, "Task %d not found.", taskId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        callingUid = task->mCallingUid;
        callingPackage = task->mCallingPackage;
        intent = task->mIntent;
        intent->AddFlags(IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY);
        userId = task->mUserId;
    }
    return StartActivityInPackage(callingUid, callingPackage, intent, String(NULL),
        NULL, String(NULL), 0, 0, options, userId, NULL, task, result);
}

ECode CActivityManagerService::StartActivityInPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [in] */ IIActivityContainer* container,
    /* [in] */ TaskRecord* inTask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    String nullStr;
    String name("startActivityInPackage");
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));

    return mStackSupervisor->StartActivityMayWait(NULL, uid, callingPackage, intent,
        resolvedType, NULL, NULL, resultTo, resultWho, requestCode, startFlags,
        NULL, NULL, NULL, options, userId, container, inTask, result);
}

ECode CActivityManagerService::StartActivities(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ IBinder* resultTo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    String nullStr;
    String name("startActivity");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));
    return mStackSupervisor->StartActivities(caller, -1, callingPackage, intents,
        resolvedTypes, resultTo, options, userId, result);
}

ECode CActivityManagerService::StartActivitiesInPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ IBinder* resultTo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    String nullStr;
    String name("startActivityInPackage");
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, ALLOW_FULL_ONLY, name, nullStr, &userId));
    return mStackSupervisor->StartActivities(NULL, uid, callingPackage, intents, resolvedTypes,
        resultTo, options, userId, status);
}

//explicitly remove thd old information in mRecentTasks when removing existing user.
void CActivityManagerService::RemoveRecentTasksForUserLocked(
    /* [in] */ Int32 userId)
{
    if(userId <= 0) {
        Slogger::I(TAG, "Can't remove recent task on user %d", userId);
        return;
    }

    List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks->RBegin();
    while (rit != mRecentTasks->REnd()) {
        AutoPtr<TaskRecord> tr = *rit;
        if (tr->mUserId == userId) {
            if(DEBUG_TASKS) Slogger::I(TAG, "remove RecentTask %s when finishing user %d",
                TO_CSTR(tr), userId);
            List<AutoPtr<TaskRecord> >::ReverseIterator tmpIt(rit);
            mRecentTasks->Erase(--tmpIt.GetBase());
            tr->RemovedFromRecents(mTaskPersister);
        }
        else
            ++rit;
    }

    // Remove tasks from persistent storage.
    mTaskPersister->Wakeup(NULL, TRUE);
}

// Sort by taskId
Int32 CActivityManagerService::TaskRecordCompare(
    /* [in] */ TaskRecord* lhs,
    /* [in] */ TaskRecord* rhs)
{
    return rhs->mTaskId - lhs->mTaskId;
}

// Extract the affiliates of the chain containing mRecentTasks[start].
Int32 CActivityManagerService::ProcessNextAffiliateChain(
    /* [in] */ Int32 start)
{
    List<AutoPtr<TaskRecord> >::Iterator startIt = mRecentTasks->Begin();
    for (Int32 i = 0; i < start; i++)
        ++startIt;
    AutoPtr<TaskRecord> startTask = *startIt;
    Int32 affiliateId = startTask->mAffiliatedTaskId;

    // Quick identification of isolated tasks. I.e. those not launched behind.
    if (startTask->mTaskId == affiliateId && startTask->mPrevAffiliate == NULL &&
            startTask->mNextAffiliate == NULL) {
        // There is still a slim chance that there are other tasks that point to this task
        // and that the chain is so messed up that this task no longer points to them but
        // the gain of this optimization outweighs the risk.
        startTask->mInRecents = TRUE;
        return start + 1;
    }

    // Remove all tasks that are affiliated to affiliateId and put them in mTmpRecents.
    mTmpRecents.Clear();
    List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks->RBegin();
    while (rit != mRecentTasks->REnd()) {
        AutoPtr<TaskRecord> task = *rit;
        if (task->mAffiliatedTaskId == affiliateId) {
            List<AutoPtr<TaskRecord> >::ReverseIterator tmpIt(rit);
            mRecentTasks->Erase(--tmpIt.GetBase());
            mTmpRecents.PushBack(task);
        }
        else
            ++rit;
    }

    // Sort them all by taskId. That is the order they were create in and that order will
    // always be correct.
    mTmpRecents.Sort(TaskRecordCompare);

    // Go through and fix up the linked list.
    // The first one is the end of the chain and has no next.
    AutoPtr<TaskRecord> first = mTmpRecents.GetFront();
    first->mInRecents = TRUE;
    if (first->mNextAffiliate != NULL) {
        Slogger::W(TAG, "Link error 1 first->next=%s", TO_CSTR(first->mNextAffiliate));
        first->SetNextAffiliate(NULL);
        mTaskPersister->Wakeup(first, FALSE);
    }
    // Everything in the middle is doubly linked from next to prev.
    Int32 tmpSize = mTmpRecents.GetSize();
    List<AutoPtr<TaskRecord> >::Iterator it = mTmpRecents.Begin();
    for (Int32 i = 0; i < tmpSize - 1; ++it, ++i) {
        AutoPtr<TaskRecord> next = *it;
        AutoPtr<TaskRecord> prev = *(++it);
        --it;
        if (next->mPrevAffiliate != prev) {
            Slogger::W(TAG, "Link error 2 next=%s prev=%s setting prev=%s",
                TO_CSTR(next), TO_CSTR(next->mPrevAffiliate), TO_CSTR(prev));
            next->SetPrevAffiliate(prev);
            mTaskPersister->Wakeup(next, FALSE);
        }
        if (prev->mNextAffiliate != next) {
            Slogger::W(TAG, "Link error 3 prev=%s next=%s setting next=",
                TO_CSTR(prev), TO_CSTR(prev->mPrevAffiliate), TO_CSTR(next));
            prev->SetNextAffiliate(next);
            mTaskPersister->Wakeup(prev, FALSE);
        }
        prev->mInRecents = TRUE;
    }
    // The last one is the beginning of the list and has no prev.
    AutoPtr<TaskRecord> last = mTmpRecents.GetBack();
    if (last->mPrevAffiliate != NULL) {
        Slogger::W(TAG, "Link error 4 last.prev=%s", TO_CSTR(last->mPrevAffiliate));
        last->SetPrevAffiliate(NULL);
        mTaskPersister->Wakeup(last, FALSE);
    }

    // Insert the group back into mRecentTasks at start.
    mRecentTasks->Insert(startIt, mTmpRecents.Begin(), mTmpRecents.End());

    // Let the caller know where we left off.
    return start + tmpSize;
}

void CActivityManagerService::CleanupRecentTasksLocked(
    /* [in] */ Int32 userId)
{
    if (mRecentTasks == NULL) {
        // Happens when called from the packagemanager broadcast before boot.
        return;
    }

    HashMap<AutoPtr<IComponentName>, AutoPtr<IActivityInfo> > availActCache;
    HashMap<String, AutoPtr<IApplicationInfo> > availAppCache;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    AutoPtr<IActivityInfo> dummyAct;
    CActivityInfo::New((IActivityInfo**)&dummyAct);
    AutoPtr<IApplicationInfo> dummyApp;
    CApplicationInfo::New((IApplicationInfo**)&dummyApp);

    Int32 N = mRecentTasks->GetSize();

    AutoPtr<ArrayOf<Int32> > users;
    if (userId == IUserHandle::USER_ALL) {
        users = GetUsersLocked();
    }
    else {
        users = ArrayOf<Int32>::Alloc(1);
        (*users)[0] = userId;
    }

    for (Int32 ui = 0; ui < users->GetLength(); ui++) {
        Int32 user = (*users)[ui];
        List<AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
        while (it != mRecentTasks->End()) {
            AutoPtr<TaskRecord> task = *it;
            if (task->mUserId != user) {
                // Only look at tasks for the user ID of interest.
                ++it;
                continue;
            }
            if (task->mAutoRemoveRecents && task->GetTopActivity() == NULL) {
                // This situation is broken, and we should just get rid of it now.
                it = mRecentTasks->Erase(it);
                task->RemovedFromRecents(mTaskPersister);
                N--;
                Slogger::W(TAG, "Removing auto-remove without activity: %s", TO_CSTR(task));
                continue;
            }
            // Check whether this activity is currently available.
            if (task->mRealActivity != NULL) {
                AutoPtr<IActivityInfo> ai = availActCache[task->mRealActivity];
                if (ai == NULL) {
                    // try {
                        pm->GetActivityInfo(task->mRealActivity,
                            IPackageManager::GET_UNINSTALLED_PACKAGES
                            | IPackageManager::GET_DISABLED_COMPONENTS,
                            user, (IActivityInfo**)&ai);
                    // } catch (RemoteException e) {
                    //     // Will never happen.
                    //     continue;
                    // }
                    if (ai == NULL) {
                        ai = dummyAct;
                    }
                    availActCache[task->mRealActivity] = ai;
                }
                if (ai == dummyAct) {
                    // This could be either because the activity no longer exists, or the
                    // app is temporarily gone.  For the former we want to remove the recents
                    // entry; for the latter we want to mark it as unavailable.
                    String pkgName;
                    task->mRealActivity->GetPackageName(&pkgName);
                    AutoPtr<IApplicationInfo> app = availAppCache[pkgName];
                    if (app == NULL) {
                        // try {
                            pm->GetApplicationInfo(pkgName,
                                IPackageManager::GET_UNINSTALLED_PACKAGES
                                | IPackageManager::GET_DISABLED_COMPONENTS,
                                user, (IApplicationInfo**)&app);
                        // } catch (RemoteException e) {
                        //     // Will never happen.
                        //     continue;
                        // }
                        if (app == NULL) {
                            app = dummyApp;
                        }
                        availAppCache[pkgName] = app;
                    }
                    Int32 flags;
                    if (app == dummyApp || (app->GetFlags(&flags), flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
                        // Doesn't exist any more!  Good-bye.
                        it = mRecentTasks->Erase(it);
                        task->RemovedFromRecents(mTaskPersister);
                        N--;
                        Slogger::W(TAG, "Removing no longer valid recent: %s", TO_CSTR(task));
                        continue;
                    }
                    else {
                        // Otherwise just not available for now.
                        if (task->mIsAvailable) {
                            if (DEBUG_RECENTS) {
                                Slogger::D(TAG, "Making recent unavailable: %s", TO_CSTR(task));
                            }
                        }
                        task->mIsAvailable = FALSE;
                    }
                }
                else {
                    Boolean enabled;
                    IComponentInfo::Probe(ai)->GetEnabled(&enabled);
                    AutoPtr<IApplicationInfo> appInfo;
                    IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                    Boolean aEnabled;
                    appInfo->GetEnabled(&aEnabled);
                    Int32 flags;
                    appInfo->GetFlags(&flags);
                    if (!enabled || !aEnabled || (flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
                        if (task->mIsAvailable) {
                            if (DEBUG_RECENTS)
                                Slogger::D(TAG, "Making recent unavailable: %s (enabled=%d/%d flags=0x%08x)",
                                    TO_CSTR(task), enabled, aEnabled, flags);
                        }
                        task->mIsAvailable = FALSE;
                    }
                    else {
                        if (!task->mIsAvailable) {
                            if (DEBUG_RECENTS) Slogger::D(TAG, "Making recent available: %s",
                                    TO_CSTR(task));
                        }
                        task->mIsAvailable = TRUE;
                    }
                }
            }
            ++it;
        }
    }

    // Verify the affiliate chain for each task.
    for (Int32 i = 0; i < N; i = ProcessNextAffiliateChain(i)) {
    }

    mTmpRecents.Clear();
    // mRecentTasks is now in sorted, affiliated order.
}

Boolean CActivityManagerService::MoveAffiliatedTasksToFront(
    /* [in] */ TaskRecord* task,
    /* [in] */ Int32 taskIndex)
{
    Int32 N = mRecentTasks->GetSize();
    AutoPtr<TaskRecord> top = task;
    Int32 topIndex = taskIndex;
    while (top->mNextAffiliate != NULL && topIndex > 0) {
        top = top->mNextAffiliate;
        topIndex--;
    }
    if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: adding affilliates starting"
        " at %d from intial %d", topIndex, taskIndex);
    // Find the end of the chain, doing a sanity check along the way.
    Boolean sane = top->mAffiliatedTaskId == task->mAffiliatedTaskId;
    Int32 endIndex = topIndex;
    List<AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
    for (Int32 i = 0; i < endIndex; i++)
        ++it;
    AutoPtr<TaskRecord> prev = top;
    while (endIndex < N) {
        AutoPtr<TaskRecord> cur = *it;
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: looking at next chain @%d %s",
                endIndex, TO_CSTR(cur));
        if (cur == top) {
            // Verify start of the chain.
            if (cur->mNextAffiliate != NULL || cur->mNextAffiliateTaskId != -1) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d: first task has next affiliate: %s",
                    endIndex, TO_CSTR(prev));
                sane = FALSE;
                break;
            }
        }
        else {
            // Verify middle of the chain's next points back to the one before.
            if (cur->mNextAffiliate != prev
                    || cur->mNextAffiliateTaskId != prev->mTaskId) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d middle task %s @%d has bad next"
                    " affiliate %s id %d, expected %s", endIndex, TO_CSTR(cur), endIndex,
                    TO_CSTR(cur->mNextAffiliate), cur->mNextAffiliateTaskId, TO_CSTR(prev));
                sane = FALSE;
                break;
            }
        }
        if (cur->mPrevAffiliateTaskId == -1) {
            // Chain ends here.
            if (cur->mPrevAffiliate != NULL) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d: last task %s has previous affiliate %s",
                        endIndex, TO_CSTR(cur), TO_CSTR(cur->mPrevAffiliate));
                sane = FALSE;
            }
            if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: end of chain @%d", endIndex);
            break;
        }
        else {
            // Verify middle of the chain's prev points to a valid item.
            if (cur->mPrevAffiliate == NULL) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d: task %s has previous affiliate NULL but should be id %d",
                    endIndex, TO_CSTR(cur), cur->mPrevAffiliateTaskId);
                sane = FALSE;
                break;
            }
        }
        if (cur->mAffiliatedTaskId != task->mAffiliatedTaskId) {
            Slogger::W/*Wtf*/(TAG, "Bad chain @%d: task %s has affiliated id %d but should be %d",
                    endIndex, TO_CSTR(cur), cur->mAffiliatedTaskId, task->mAffiliatedTaskId);
            sane = FALSE;
            break;
        }
        prev = cur;
        endIndex++;
        ++it;
        if (endIndex >= N) {
            Slogger::W/*Wtf*/(TAG, "Bad chain ran off index %d: last task %s",
                endIndex, TO_CSTR(prev));
            sane = FALSE;
            break;
        }
    }
    if (sane) {
        if (endIndex < taskIndex) {
            Slogger::W/*Wtf*/(TAG, "Bad chain @%d: did not extend to task %s @%d",
                endIndex, TO_CSTR(task), taskIndex);
            sane = FALSE;
        }
    }
    if (sane) {
        // All looks good, we can just move all of the affiliated tasks
        // to the top.
        List<AutoPtr<TaskRecord> >::Iterator topIt = mRecentTasks->Begin();
        List<AutoPtr<TaskRecord> >::Iterator endIt = mRecentTasks->Begin();
        for (Int32 i = 0; i < topIndex; i++)
            ++topIt;
        for (Int32 i = 0; i <= endIndex; i++)
            ++endIt;
        mRecentTasks->Splice(mRecentTasks->Begin(), *mRecentTasks, topIt, endIt);
        // for (Int32 i = topIndex; i <= endIndex; i++) {
        //     if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: moving affiliated " + task
        //             + " from " + i + " to " + (i-topIndex));
        //     TaskRecord cur = mRecentTasks->remove(i);
        //     mRecentTasks->add(i-topIndex, cur);
        // }
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: done moving tasks  %d to %d", topIndex, endIndex);
        return TRUE;
    }

    // Whoops, couldn't do it.
    return FALSE;
}

ECode CActivityManagerService::AddRecentTaskLocked(
    /* [in] */ TaskRecord* task)
{
    // Slogger::I(TAG, " >> AddRecentTaskLocked MemoryDumper::Dump()");
    // MemoryDumper::Dump();

    // enable for detecting memory leak
    // return NOERROR;

    Boolean isAffiliated = task->mAffiliatedTaskId != task->mTaskId
            || task->mNextAffiliateTaskId != -1 || task->mPrevAffiliateTaskId != -1;

    Int32 N = mRecentTasks->GetSize();
    // Quick case: check if the top-most recent task is the same.
    if (!isAffiliated && N > 0 && mRecentTasks->Begin()->Get() == task) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: already at top: %s", TO_CSTR(task));
        return NOERROR;
    }
    // Another quick case: check if this is part of a set of affiliated
    // tasks that are at the top.
    if (isAffiliated && N > 0 && task->mInRecents
        && task->mAffiliatedTaskId == (*mRecentTasks->Begin())->mAffiliatedTaskId) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: affiliated %s at top when adding %s",
            TO_CSTR(*mRecentTasks->Begin()), TO_CSTR(task));
        return NOERROR;
    }
    // Another quick case: never add voice sessions.
    if (task->mVoiceSession != NULL) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: not adding voice interaction %s", TO_CSTR(task));
        return NOERROR;
    }

    Boolean needAffiliationFix = FALSE;

    // Slightly less quick case: the task is already in recents, so all we need
    // to do is move it.
    if (task->mInRecents) {
        Int32 taskIndex = -1;
        List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
        for (Int32 i = 0; it != mRecentTasks->End(); ++it, i++) {
            if (it->Get() == task) {
                taskIndex = i;
                break;
            }
        }
        if (taskIndex >= 0) {
            if (!isAffiliated) {
                // Simple case: this is not an affiliated task, so we just move it to the front.
                mRecentTasks->Erase(it);
                mRecentTasks->Insert(mRecentTasks->Begin(), task);
                NotifyTaskPersisterLocked(task, FALSE);
                if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: moving to top %s from %d",
                    TO_CSTR(task), taskIndex);
                return NOERROR;
            }
            else {
                // More complicated: need to keep all affiliated tasks together.
                if (MoveAffiliatedTasksToFront(task, taskIndex)) {
                    // All went well.
                    return NOERROR;
                }

                // Uh oh...  something bad in the affiliation chain, try to rebuild
                // everything and then go through our general path of adding a new task.
                needAffiliationFix = TRUE;
            }
        }
        else {
            Slogger::W/*Wtf*/(TAG, "Task with inRecent not in recents: %s", TO_CSTR(task));
            needAffiliationFix = TRUE;
        }
    }

    if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: trimming tasks for %s", TO_CSTR(task));
    TrimRecentsForTask(task, TRUE);

    N = mRecentTasks->GetSize();
    AutoPtr<IActivityManagerHelper> amhelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
    Int32 maxRecentTasks;
    amhelper->GetMaxRecentTasksStatic(&maxRecentTasks);
    while (N >= maxRecentTasks) {
        AutoPtr<TaskRecord> tr = mRecentTasks->GetBack();
        mRecentTasks->PopBack();
        tr->RemovedFromRecents(mTaskPersister);
        N--;
    }
    task->mInRecents = TRUE;
    if (!isAffiliated || needAffiliationFix) {
        // If this is a simple non-affiliated task, or we had some failure trying to
        // handle it as part of an affilated task, then just place it at the top.
        mRecentTasks->Insert(mRecentTasks->Begin(), task);
    }
    else if (isAffiliated) {
        // If this is a new affiliated task, then move all of the affiliated tasks
        // to the front and insert this new one.
        AutoPtr<TaskRecord> other = task->mNextAffiliate;
        if (other == NULL) {
            other = task->mPrevAffiliate;
        }
        if (other != NULL) {
            Int32 otherIndex = -1;
            List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
            for (Int32 i = 0; it != mRecentTasks->End(); ++it, i++) {
                if (*it == other) {
                    otherIndex = i;
                    break;
                }
            }
            if (otherIndex >= 0) {
                // Insert new task at appropriate location.
                Int32 taskIndex;
                List< AutoPtr<TaskRecord> >::Iterator insertIt = it;
                if (other == task->mNextAffiliate) {
                    // We found the index of our next affiliation, which is who is
                    // before us in the list, so add after that point.
                    taskIndex = otherIndex + 1;
                    ++insertIt;
                }
                else {
                    // We found the index of our previous affiliation, which is who is
                    // after us in the list, so add at their position.
                    taskIndex = otherIndex;
                }
                if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: new affiliated task added at%d: %s",
                    taskIndex, TO_CSTR(task));
                mRecentTasks->Insert(insertIt, task);

                // Now move everything to the front.
                if (MoveAffiliatedTasksToFront(task, taskIndex)) {
                    // All went well.
                    return NOERROR;
                }

                // Uh oh...  something bad in the affiliation chain, try to rebuild
                // everything and then go through our general path of adding a new task->m
                needAffiliationFix = TRUE;
            }
            else {
                if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: couldn't find other affiliation %s",
                        TO_CSTR(other));
                needAffiliationFix = TRUE;
            }
        }
        else {
            if (DEBUG_RECENTS) Slogger::D(TAG,
                    "addRecent: adding affiliated task without next/prev:%s", TO_CSTR(task));
            needAffiliationFix = TRUE;
        }
    }
    if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: adding %s", TO_CSTR(task));

    if (needAffiliationFix) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: regrouping affiliations");
        CleanupRecentTasksLocked(task->mUserId);
    }
    return NOERROR;
}

/**
 * If needed, remove oldest existing entries in recents that are for the same kind
 * of task as the given one.
 */
Int32 CActivityManagerService::TrimRecentsForTask(
    /* [in] */ TaskRecord* _task,
    /* [in] */ Boolean doTrim)
{
    AutoPtr<TaskRecord> task = _task;
    AutoPtr<IIntent> intent = task->mIntent;
    Boolean document = FALSE;
    if (intent != NULL)
        intent->IsDocument(&document);

    Int32 maxRecents = task->mMaxRecents - 1;
    List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
    for (Int32 i = 0; it != mRecentTasks->End(); ++i) {
        AutoPtr<TaskRecord> tr = *it;
        if (task != tr) {
            if (task->mUserId != tr->mUserId) {
                ++it;
                continue;
            }
            if (i > MAX_RECENT_BITMAPS) {
                tr->FreeLastThumbnail();
            }
            AutoPtr<IIntent> trIntent = tr->mIntent;
            Boolean equals;
            if ((task->mAffinity == NULL || !task->mAffinity.Equals(tr->mAffinity)) &&
                (intent == NULL || !(intent->FilterEquals(trIntent, &equals), equals))) {
                ++it;
                continue;
            }
            Boolean trIsDocument;
            if (trIntent != NULL)
                trIntent->IsDocument(&trIsDocument);
            if (document && trIsDocument) {
                // These are the same document activity (not necessarily the same doc).
                if (maxRecents > 0) {
                    --maxRecents;
                    ++it;
                    continue;
                }
                // Hit the maximum number of documents for this task. Fall through
                // and remove this document from recents.
            }
            else if (document || trIsDocument) {
                // Only one of these is a document. Not the droid we're looking for.
                ++it;
                continue;
            }
        }

        if (!doTrim) {
            // If the caller is not actually asking for a trim, just tell them we reached
            // a point where the trim would happen.
            return i;
        }

        // Either task and tr are the same or, their affinities match or their intents match
        // and neither of them is a document, or they are documents using the same activity
        // and their maxRecents has been reached.
        tr->DisposeThumbnail();
        it = mRecentTasks->Erase(it);
        if (task != tr) {
            tr->RemovedFromRecents(mTaskPersister);
        }
        i--;
        if (task->mIntent == NULL) {
            // If the new recent task we are adding is not fully
            // specified, then replace it with the existing recent task.
            task = tr;
        }
        NotifyTaskPersisterLocked(tr, FALSE);
    }

    return -1;
}

ECode CActivityManagerService::ReportActivityFullyDrawn(
    /* [in] */ IBinder* token)
{
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            r->ReportFullyDrawnLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::SetRequestedOrientation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 requestedOrientation)
{
    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r == NULL) {
        return NOERROR;;
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    mWindowManager->SetAppOrientation(r->mAppToken, requestedOrientation);
    AutoPtr<IConfiguration> config;
    mWindowManager->UpdateOrientationFromAppTokens(
        mConfiguration, r->MayFreezeScreenLocked(r->mApp)
            ? IBinder::Probe(r->mAppToken) : NULL, (IConfiguration**)&config);
    if (config != NULL) {
        r->mFrozenBeforeDestroy = TRUE;
        if (!UpdateConfigurationLocked(config, r, FALSE, FALSE)) {
            mStackSupervisor->ResumeTopActivitiesLocked();
        }
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::GetRequestedOrientation(
    /* [in] */ IBinder* token,
    /* [out] */ Int32* requestedOrientation)
{
    VALIDATE_NOT_NULL(requestedOrientation)
    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r == NULL) {
        *requestedOrientation = IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED;
        return NOERROR;
    }
    return mWindowManager->GetAppOrientation(r->mAppToken, requestedOrientation);
}

ECode CActivityManagerService::FinishActivity(
    /* [in] */ IBinder *token,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *resultData,
    /* [in] */ Boolean finishTask,
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);
    *finished = FALSE;

    // Refuse possible leaked file descriptors
    Boolean hasFd;
    if (resultData != NULL &&
            (resultData->HasFileDescriptors(&hasFd), hasFd)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock lock(this);

        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r == NULL) {
            return TRUE;
        }
        // Keep track of the root activity of the task before we finish it
        AutoPtr<TaskRecord> tr = r->mTask;
        AutoPtr<ActivityRecord> rootR = tr->GetRootActivity();
        // Do not allow task to finish in Lock Task mode.
        if (tr == mStackSupervisor->mLockTaskModeTask) {
            if (rootR == r) {
                mStackSupervisor->ShowLockTaskToast();
                return FALSE;
            }
        }

        if (mController != NULL) {
            // Find the first activity that is not finishing.
            AutoPtr<ActivityRecord> next = r->mTask->mStack->TopRunningActivityLocked(token, 0);
            if (next != NULL) {
                // ask watcher if this is allowed
                Boolean resumeOK = TRUE;
                if (FAILED(mController->ActivityResuming(next->mPackageName, &resumeOK))) {
                    mController = NULL;
                    Watchdog::GetInstance()->SetActivityController(NULL);
                }

                if (!resumeOK) {
                    return NOERROR;
                }
            }
        }

        const Int64 origId = Binder::ClearCallingIdentity();
        if (finishTask && r == rootR) {
            // If requested, remove the task that is associated to this activity only if it
            // was the root activity in the task.  The result code and data is ignored because
            // we don't support returning them across task boundaries.
            *finished = RemoveTaskByIdLocked(tr->mTaskId, 0);
        }
        else {
            *finished = tr->mStack->RequestFinishActivityLocked(token, resultCode,
                    resultData, String("app-request"), TRUE);
        }
        Binder::RestoreCallingIdentity(origId);
        return NOERROR;
    }
}

ECode CActivityManagerService::FinishHeavyWeightApp()
{
    if (CheckCallingPermission(Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: finishHeavyWeightApp() from pid=%d, uid=%d"
            " requires Manifest::permission::FORCE_STOP_PACKAGES",
            Binder::GetCallingPid(), Binder::GetCallingUid());
        // throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(this);
        if (mHeavyWeightProcess == NULL) {
            return NOERROR;
        }

        List<AutoPtr<ActivityRecord> > activities = mHeavyWeightProcess->mActivities;
        List<AutoPtr<ActivityRecord> >::Iterator it;
        for (it = activities.Begin(); it != activities.End(); ++it) {
            AutoPtr<ActivityRecord> r = *it;
            if (!r->mFinishing) {
                r->mTask->mStack->FinishActivityLocked(r, IActivity::RESULT_CANCELED,
                    NULL, String("finish-heavy"), TRUE);
           }
        }

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG,
            mHeavyWeightProcess->mUserId, 0, (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessage(msg, &result);

        mHeavyWeightProcess = NULL;
    }
    return NOERROR;
}

ECode CActivityManagerService::CrashApplication(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& packageName,
    /* [in] */ const String& message)
{
    if (CheckCallingPermission(Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: crashApplication() from pid=%d, uid=%d requires"
            " Manifest::permission::FORCE_STOP_PACKAGES",
            Binder::GetCallingPid(), Binder::GetCallingUid());
        // throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(this);
        AutoPtr<ProcessRecord> proc;

        // Figure out which process to kill.  We don't trust that initialPid
        // still has any relation to current pids, so must scan through the
        // list.
        {
            AutoLock lock(mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
            for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
                AutoPtr<ProcessRecord> p = it->mSecond;
                if (p->mUid != uid) {
                    continue;
                }
                if (p->mPid == initialPid) {
                    proc = p;
                    break;
                }
                Boolean res;
                if ((p->mPkgList->ContainsKey(CoreUtils::Convert(packageName), &res), res)) {
                    proc = p;
                }
            }
        }

        if (proc == NULL) {
            Slogger::W(TAG, "crashApplication: nothing for uid=%d initialPid=%d packageName=%s", uid,
                    initialPid, packageName.string());
            return NOERROR;
        }

        if (proc->mThread != NULL) {
            if (proc->mPid == Process::MyPid()) {
                Logger::W(TAG, "crashApplication: trying to crash self!");
                return NOERROR;
            }
            Int64 ident = Binder::ClearCallingIdentity();
           // try {
            proc->mThread->ScheduleCrash(message);
           // } catch (RemoteException e) {
           // }
            Binder::RestoreCallingIdentity(ident);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::FinishSubActivity(
    /* [in] */ IBinder* token,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode)
{
    AutoLock lock(this);
    const Int64 origId = Binder::ClearCallingIdentity();
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r != NULL) {
        r->mTask->mStack->FinishSubActivityLocked(r, resultWho, requestCode);
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::FinishActivityAffinity(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* finished)
 {
    VALIDATE_NOT_NULL(finished);
    *finished = FALSE;

    AutoLock lock(this);
    const Int64 origId = Binder::ClearCallingIdentity();

    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    AutoPtr<ActivityRecord> rootR = r->mTask->GetRootActivity();
    ECode ec = NOERROR;
    // Do not allow task to finish in Lock Task mode.
    if (r->mTask == mStackSupervisor->mLockTaskModeTask) {
        if (rootR == r) {
            ec = mStackSupervisor->ShowLockTaskToast();
            goto _EXIT_;
        }
    }

    if (r != NULL) {
        *finished = r->mTask->mStack->FinishActivityAffinityLocked(r);
    }
_EXIT_:
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::FinishVoiceTask(
    /* [in] */ IIVoiceInteractionSession* session)
{
    {    AutoLock syncLock(this);
        const Int64 origId = Binder::ClearCallingIdentity();
        ECode ec = mStackSupervisor->FinishVoiceTask(session);
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }
    return NOERROR;
}

ECode CActivityManagerService::ReleaseActivityInstance(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        const Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r->mTask == NULL || r->mTask->mStack == NULL) {
            *result = FALSE;
        }
        else
            *result = r->mTask->mStack->SafelyDestroyActivityLocked(
                r, String("app-req"));
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::ReleaseSomeActivities(
    /* [in] */ IApplicationThread* appInt)
{
    {    AutoLock syncLock(this);
        const Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<ProcessRecord> app = GetRecordForAppLocked(appInt);
        ECode ec = mStackSupervisor->ReleaseSomeActivitiesLocked(app, String("low-mem"));
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }
    return NOERROR;
}

ECode CActivityManagerService::WillActivityBeVisible(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);

    AutoLock lock(this);

    AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
    if (stack != NULL) {
        *visible = stack->WillActivityBeVisibleLocked(token);
        return NOERROR;
    }
    *visible = FALSE;
    return NOERROR;
}

ECode CActivityManagerService::OverridePendingTransition(
    /* [in] */ IBinder* token,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 enterAnim,
    /* [in] */ Int32 exitAnim)
{
    AutoLock lock(this);
    AutoPtr<ActivityRecord> self = ActivityRecord::IsInStackLocked(token);
        if (self == NULL) {
            return NOERROR;
        }

    const Int64 origId = Binder::ClearCallingIdentity();

    if (self->mState == ActivityState_RESUMED
            || self->mState == ActivityState_PAUSING) {
        mWindowManager->OverridePendingAppTransition(packageName,
                enterAnim, exitAnim, NULL);
    }

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::HandleAppDiedLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean restarting,
    /* [in] */ Boolean allowRestart)
{
    Int32 pid = app->mPid;
    Boolean kept = CleanUpApplicationRecordLocked(app, restarting, allowRestart, -1);
    if (!kept && !restarting) {
        RemoveLruProcessLocked(app);
        if (pid > 0) {
            ProcessList::Remove(pid);
        }
    }

    if (mProfileProc.Get() == app) {
        ClearProfilerLocked();
    }

    // Remove this application's activities from active lists.
    Boolean hasVisibleActivities = mStackSupervisor->HandleAppDiedLocked(app);

    app->mActivities.Clear();

    if (app->mInstrumentationClass != NULL) {
        String instrStr;
        app->mInstrumentationClass->ToString(&instrStr);
        Slogger::W(TAG, "Crash of app %s running instrumentation",
                app->mProcessName.string(), instrStr.string());

        AutoPtr<IBundle> info;
        CBundle::New((IBundle**)&info);
        info->PutString(String("shortMsg"), String("Process crashed."));
        FinishInstrumentationLocked(app, IActivity::RESULT_CANCELED, info);
    }

    if (!restarting) {
        if (!mStackSupervisor->ResumeTopActivitiesLocked()) {
            // If there was nothing to resume, and we are not already
            // restarting this process, but there is a visible activity that
            // is hosted by the process...  then make sure all visible
            // activities are running, taking care of restarting this
            // process.
            if (hasVisibleActivities) {
                mStackSupervisor->EnsureActivitiesVisibleLocked(NULL, 0);
            }
        }
    }
    return NOERROR;
}

Int32 CActivityManagerService::GetLRURecordIndexForAppLocked(
    /* [in] */ IApplicationThread* thread)
{
    // Find the application record.
    Int32 idx = mLruProcesses.GetSize() - 1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator it;
    for (it = mLruProcesses.RBegin(); it != mLruProcesses.REnd(); ++it, --idx) {
        AutoPtr<ProcessRecord> rec = *it;
        if (rec->mThread != NULL
            && IBinder::Probe(rec->mThread.Get()) == IBinder::Probe(thread)) {
            return idx;
        }
    }
    return -1;
}

AutoPtr<ProcessRecord> CActivityManagerService::GetRecordForAppLocked(
    /* [in] */ IApplicationThread* thread)
{
    AutoPtr<ProcessRecord> proc;
    if (thread == NULL) return proc;

    List< AutoPtr<ProcessRecord> >::Iterator it;
    for (it = mLruProcesses.Begin(); it != mLruProcesses.End(); ++it) {
        AutoPtr<ProcessRecord> rec = *it;
        if (rec->mThread != NULL
            && IBinder::Probe(rec->mThread.Get()) == IBinder::Probe(thread)) {
            proc = rec;
            break;
        }
    }
    return proc;
}

void CActivityManagerService::DoLowMemReportIfNeededLocked(
    /* [in] */ ProcessRecord* dyingProc)
{
    // If there are no longer any background processes running,
    // and the app that died was not running instrumentation,
    // then tell everyone we are now low on memory.
    Boolean haveBg = FALSE;
    List< AutoPtr<ProcessRecord> >::Iterator it;
    for (it = mLruProcesses.Begin(); it != mLruProcesses.End(); ++it) {
        AutoPtr<ProcessRecord> rec = *it;
        if (rec->mThread != NULL
            && rec->mSetProcState >= IActivityManager::PROCESS_STATE_CACHED_ACTIVITY) {
            haveBg = TRUE;
            break;
        }
    }

    if (!haveBg) {
        AutoPtr<ISystemProperties> sysProps;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
        String value;
        sysProps->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
        Boolean doReport = value.Equals("1");
        if (doReport) {
            Int64 now = SystemClock::GetUptimeMillis();
            if (now < (mLastMemUsageReportTime + 5*60*1000)) {
                doReport = FALSE;
            }
            else {
                mLastMemUsageReportTime = now;
            }
        }
        AutoPtr<IList> memInfos;
        if (doReport) {
            CArrayList::New(mLruProcesses.GetSize(), (IList**)&memInfos);
        }
        // EventLog.writeEvent(EventLogTags.AM_LOW_MEMORY, mLruProcesses.size());
        Int64 now = SystemClock::GetUptimeMillis();
        List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> rec = *rit;
            if (rec.Get() == dyingProc || rec->mThread == NULL) {
                continue;
            }
            if (doReport) {
                AutoPtr<IProcessMemInfo> memInfo = new ProcessMemInfo(rec->mProcessName,
                    rec->mPid, rec->mSetAdj, rec->mSetProcState, rec->mAdjType, rec->MakeAdjReason());
                memInfos->Add(memInfo);
            }
            if ((rec->mLastLowMemory + GC_MIN_INTERVAL) <= now) {
                // The low memory report is overriding any current
                // state for a GC request.  Make sure to do
                // heavy/important/visible/foreground processes first.
                if (rec->mSetAdj <= ProcessList::HEAVY_WEIGHT_APP_ADJ) {
                    rec->mLastRequestedGc = 0;
                }
                else {
                    rec->mLastRequestedGc = rec->mLastLowMemory;
                }
                rec->mReportLowMemory = TRUE;
                rec->mLastLowMemory = now;
                mProcessesToGc.Remove(rec);
                AddProcessToGcListLocked(rec);
            }
        }
        if (doReport) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(REPORT_MEM_USAGE_MSG, memInfos, (IMessage**)&msg);
            Boolean res;
            mHandler->SendMessage(msg, &res);
        }
        ScheduleAppGcsLocked();
    }
}

ECode CActivityManagerService::AppDiedLocked(
    /* [in] */ ProcessRecord* app)
{
    return AppDiedLocked(app, app->mPid, app->mThread);
}

ECode CActivityManagerService::AppDiedLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 pid,
    /* [in] */ IApplicationThread* thread)
{
    // First check if this ProcessRecord is actually active for the pid.
    {    AutoLock syncLock(mPidsSelfLockedLock);
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator find = mPidsSelfLocked.Find(pid);
        AutoPtr<ProcessRecord> curProc = find != mPidsSelfLocked.End() ? find->mSecond : NULL;
        if (curProc.Get() != app) {
            Slogger::W(TAG, "Spurious death for %s, curProc for %d: %s",
                TO_CSTR(app), pid, TO_CSTR(curProc));
            return NOERROR;
        }
    }

    Int32 uid;
    app->mInfo->GetUid(&uid);
    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    {    AutoLock syncLock(stats);
        stats->NoteProcessDiedLocked(uid, pid);
    }

    Process::KillProcessQuiet(pid);
    Process::KillProcessGroup(uid, pid);
    app->mKilled = TRUE;

    // Clean up already done if the process has been re-started.
    if (app->mPid == pid && app->mThread != NULL &&
        IBinder::Probe(app->mThread) == IBinder::Probe(thread)) {
        Boolean doLowMem = app->mInstrumentationClass == NULL;
        Boolean doOomAdj = doLowMem;
        if (!app->mKilledByAm) {
            Slogger::I(TAG, "Process %s (pid %d) has died", app->mProcessName.string(), pid);
            mAllowLowerMemLevel = TRUE;
        }
        else {
            // Note that we always want to do oom adj to update our state with the
            // new number of procs.
            mAllowLowerMemLevel = FALSE;
            doLowMem = FALSE;
        }
        // EventLog.writeEvent(EventLogTags.AM_PROC_DIED, app->mUserId, app->mPid, app->mProcessName);
        if (DEBUG_CLEANUP)
            Slogger::V(TAG, "Dying app: %s, pid: %d, thread: %s",
                TO_CSTR(app), pid, TO_CSTR(thread));
        HandleAppDiedLocked(app, FALSE, TRUE);

        if (doOomAdj) {
            UpdateOomAdjLocked();
        }

        if (doLowMem) {
            DoLowMemReportIfNeededLocked(app);
        }
    }
    else if (app->mPid != pid) {
        // A new process has already been started.
        Slogger::I(TAG, "Process %s (pid %d) has died and restarted (pid %d).",
            app->mProcessName.string(), pid, app->mPid);
        // EventLog.writeEvent(EventLogTags.AM_PROC_DIED, app->mUserId, app->mPid, app->mProcessName);
    }
    else if (DEBUG_PROCESSES) {
        Slogger::D(TAG, "Received spurious death notification for thread %s"
            , TO_CSTR(thread));
    }

    return NOERROR;
}

AutoPtr<IFile> CActivityManagerService::DumpStackTraces(
    /* [in] */ Boolean clearTraces,
    /* [in] */ List<Int32>* firstPids,
    /* [in] */ IProcessCpuTracker* processCpuTracker,
    /* [in] */ HashMap<Int32, Boolean>* lastPids,
    /* [in] */ ArrayOf<String>* nativeProcs)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String tracesPath;
    sysProp->Get(String("dalvik.vm.stack-trace-file"), String(NULL), &tracesPath);
    if (tracesPath.IsNullOrEmpty()) {
        return NULL;
    }

    AutoPtr<IFile> tracesFile;
    CFile::New(tracesPath, (IFile**)&tracesFile);
//     try {
    AutoPtr<IFile> tracesDir;
    tracesFile->GetParentFile((IFile**)&tracesDir);
    Boolean dirExist;
    tracesDir->Exists(&dirExist);
    if (!dirExist) {
        Boolean mked;
        tracesDir->Mkdirs(&mked);
        if (!SELinux::Restorecon(tracesDir)) {
            return NULL;
        }
    }
    String dirPath;
    tracesDir->GetPath(&dirPath);
    FileUtils::SetPermissions(dirPath, 0775, -1, -1);  // drwxrwxr-x

    Boolean fileExist;
    tracesFile->Exists(&fileExist);
    if (clearTraces && fileExist) {
        Boolean deleted;
        tracesFile->Delete(&deleted);
    }
    Boolean created;
    tracesFile->CreateNewFile(&created);
    String filePath;
    tracesFile->GetPath(&filePath);
    FileUtils::SetPermissions(filePath, 0666, -1, -1); // -rw-rw-rw-
//     } catch (IOException e) {
//         Slogger::W(TAG, "Unable to prepare ANR traces file: " + tracesPath, e);
//         return NULL;
//     }
    DumpStackTraces(tracesPath, firstPids, processCpuTracker, lastPids, nativeProcs);
    return tracesFile;
}

void CActivityManagerService::DumpStackTraces(
    /* [in] */ const String& tracesPath,
    /* [in] */ List<Int32>* firstPids,
    /* [in] */ IProcessCpuTracker* processCpuTracker,
    /* [in] */ HashMap<Int32, Boolean>* lastPids,
    /* [in] */ ArrayOf<String>* nativeProcs)
 {
    // Use a FileObserver to detect when traces finish writing.
    // The order of traces is considered important to maintain for legibility.
    AutoPtr<DumpStackTracesFileObserver> observer =
        new DumpStackTracesFileObserver(tracesPath, IFileObserver::CLOSE_WRITE);

    // try {
    observer->StartWatching();

    // First collect all of the stacks of the most important pids.
    if (firstPids != NULL) {
        // try {
        List<Int32>::Iterator it;
        for (it = firstPids->Begin(); it != firstPids->End(); ++it) {
            AutoLock lock(observer);
            Int32 pid = *it;
            Process::SendSignal(pid, IProcess::SIGNAL_QUIT);
            observer->Wait(200);  // Wait for write-close, give up after 200msec
        }
        // } catch (InterruptedException e) {
        //     Slog.wtf(TAG, e);
        // }
    }

    // Next collect the stacks of the native pids
    if (nativeProcs != NULL) {
        AutoPtr<ArrayOf<Int32> > pids;
        Process::GetPidsForCommands(*nativeProcs, (ArrayOf<Int32>**)&pids);
        if (pids != NULL) {
            AutoPtr<IDebug> debug;
            CDebug::AcquireSingleton((IDebug**)&debug);
            for (Int32 i = 0; i < pids->GetLength(); ++i) {
                debug->DumpNativeBacktraceToFile((*pids)[i], tracesPath);
            }
        }
    }

    // Lastly, measure CPU usage.
    if (processCpuTracker != NULL) {
        processCpuTracker->Init();
        processCpuTracker->Update();
        // try {
            {
                AutoLock syncLock(processCpuTracker);
                ((Object*)IObject::Probe(processCpuTracker))->Wait(500); // measure over 1/2 second.
            }
        // } catch (InterruptedException e) {
        // }
        processCpuTracker->Update();

        // We'll take the stack crawls of just the top apps using CPU.
        Int32 N;
        processCpuTracker->CountWorkingStats(&N);
        Int32 pid;
        Int32 numProcs = 0;
        for (Int32 i = 0; i < N && numProcs < 5; i++) {
            AutoPtr<IProcessCpuTrackerStats> stats;
            processCpuTracker->GetWorkingStats(i, (IProcessCpuTrackerStats**)&stats);
            stats->GetPid(&pid);
            if (lastPids->Find(pid) != lastPids->End()) {
                numProcs++;
                {
                    AutoLock lock(observer);
                // try {
                    Process::SendSignal(pid, IProcess::SIGNAL_QUIT);
                    observer->Wait(200);  // Wait for write-close, give up after 200msec
                }
                // } catch (InterruptedException e) {
                //     Log.wtf(TAG, e);
                // }
            }
        }
    }

    // } finally {
    observer->StopWatching();
    // }
}

ECode CActivityManagerService::LogAppTooSlow(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int64 startTime,
    /* [in] */ const String& msg)
 {
    if (TRUE || IS_USER_BUILD) {
        return NOERROR;
    }
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String tracesPath;
    sysProp->Get(String("dalvik.vm.stack-trace-file"), String(NULL), &tracesPath);
    if (tracesPath.IsNullOrEmpty()) {
        return NOERROR;
    }

    AutoPtr<IStrictMode> helper;
    CStrictMode::AcquireSingleton((IStrictMode**)&helper);
    AutoPtr<IStrictModeThreadPolicy> oldPolicy, tmpPolicy;
    helper->AllowThreadDiskReads((IStrictModeThreadPolicy**)&oldPolicy);
    helper->AllowThreadDiskWrites((IStrictModeThreadPolicy**)&tmpPolicy);
//     try {
    AutoPtr<IFile> tracesFile;
    CFile::New(tracesPath, (IFile**)&tracesFile);
    AutoPtr<IFile> tracesDir;
    tracesFile->GetParentFile((IFile**)&tracesDir);
    AutoPtr<IFile> tracesTmp;
    CFile::New(tracesDir, String("__tmp__"), (IFile**)&tracesTmp);
//         try {
    Boolean dirExist;
    tracesDir->Exists(&dirExist);
    String dirPath;
    tracesDir->GetPath(&dirPath);
    if (!dirExist) {
        Boolean mked;
        tracesDir->Mkdirs(&mked);
        if (!SELinux::Restorecon(dirPath)) {
            return NOERROR;
        }
    }
    FileUtils::SetPermissions(dirPath, 0775, -1, -1);  // drwxrwxr-x
    Boolean fileExist;
    tracesFile->Exists(&fileExist);
    Boolean renamed;
    if (fileExist) {
        Boolean deleted;
        tracesTmp->Delete(&deleted);
        tracesFile->RenameTo(tracesTmp, &renamed);
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    StringBuilder sb;
    AutoPtr<ITime> tobj;
    CTime::New((ITime**)&tobj);
    tobj->Set(now);
    String fString;
    tobj->Format(String("%Y-%m-%d %H:%M:%S"), &fString);
    sb.Append(fString);
    sb.Append(String(": "));
    TimeUtils::FormatDuration(SystemClock::GetUptimeMillis()-startTime, sb);
    sb.Append(String(" since "));
    sb.Append(msg);
    AutoPtr<IOutputStream> fos;
    CFileOutputStream::New(tracesFile, (IOutputStream**)&fos);
    fos->Write(sb.ToString().GetBytes());
    if (app == NULL) {
        String str("\n*** No application process!");
        fos->Write(str.GetBytes());
    }
    ICloseable::Probe(fos)->Close();
    String filePath;
    tracesFile->GetPath(&filePath);
    FileUtils::SetPermissions(filePath, 0666, -1, -1); // -rw-rw-rw-
//         } catch (IOException e) {
//             Slogger::W(TAG, "Unable to prepare slow app traces file: " + tracesPath, e);
//             return;
//         }

    if (app != NULL) {
        List<Int32> firstPids;
        firstPids.PushBack(app->mPid);
        DumpStackTraces(tracesPath, &firstPids, NULL, NULL, NULL);
    }

    AutoPtr<IFile> lastTracesFile;
    AutoPtr<IFile> curTracesFile;
    for (Int32 i=9; i>=0; i--) {
        // String name = String.format(Locale.US, "slow%02d.txt", i);
        String name(NULL);
        name.AppendFormat("slow%02d.txt", i);
        curTracesFile = NULL;
        CFile::New(tracesDir, name, (IFile**)&curTracesFile);
        Boolean curFileExist;
        curTracesFile->Exists(&curFileExist);
        if (curFileExist) {
            if (lastTracesFile != NULL) {
                curTracesFile->RenameTo(lastTracesFile, &renamed);
            }
            else {
                Boolean deleted;
                curTracesFile->Delete(&deleted);
            }
        }
        lastTracesFile = curTracesFile;
    }

    tracesFile->RenameTo(curTracesFile, &renamed);
    Boolean tmpExist;
    tracesTmp->Exists(&tmpExist);
    if (tmpExist) {
        tracesTmp->RenameTo(tracesFile, &renamed);
    }
    //} finally {
    helper->SetThreadPolicy(oldPolicy);
    //}
    return NOERROR;
}

ECode CActivityManagerService::AppNotResponding(
    /* [in] */ ProcessRecord* app,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ ActivityRecord* parent,
    /* [in] */ Boolean aboveSystem,
    /* [in] */ const String& annotation)
{
    List<Int32> firstPids;
    HashMap<Int32, Boolean> lastPids(20);

    if (mController != NULL) {
        // try {
        // 0 == continue, -1 = kill process immediately
        Int32 res;
        if (SUCCEEDED(mController->AppEarlyNotResponding(app->mProcessName, app->mPid, annotation, &res))) {
            if (res < 0 && app->mPid != MY_PID) {
                app->Kill(String("anr"), TRUE);
            }
        }
        else {
            mController = NULL;
            Watchdog::GetInstance()->SetActivityController(NULL);
        }
    }

    Int64 anrTime = SystemClock::GetUptimeMillis();
    if (MONITOR_CPU_USAGE) {
        UpdateCpuStatsNow();
    }

    {
        AutoLock lock(this);
        // PowerManager.reboot() can block for a long time, so ignore ANRs while shutting down.
        if (mShuttingDown) {
            Slogger::I(TAG, "During shutdown skipping ANR: %s %s", TO_CSTR(app), annotation.string());
            return NOERROR;
        }
        else if (app->mNotResponding) {
            Slogger::I(TAG, "Skipping duplicate ANR: %s %s", TO_CSTR(app), annotation.string());
            return NOERROR;
        }
        else if (app->mCrashing) {
            Slogger::I(TAG, "Crashing app skipping ANR: %s %s", TO_CSTR(app), annotation.string());
            return NOERROR;
        }

        // In case we come through here for the same app before completing
        // this one, mark as anring now so we will bail out.
        app->mNotResponding = TRUE;

        // Log the ANR to the event log.
//TODO:
//            EventLog.writeEvent(EventLogTags.AM_ANR, app.userId, app.pid, app.processName, app.info.flags,
//                    annotation);

        // Dump thread traces as quickly as we can, starting with "interesting" processes.
        firstPids.PushBack(app->mPid);

        Int32 parentPid = app->mPid;
        if (parent != NULL && parent->mApp != NULL && parent->mApp->mPid > 0) parentPid = parent->mApp->mPid;
        if (parentPid != app->mPid) {
            firstPids.PushBack(parentPid);
        }

        if (MY_PID != app->mPid && MY_PID != parentPid) {
            firstPids.PushBack(MY_PID);
        }

        List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
           AutoPtr<ProcessRecord> r = *rit;
           if (r != NULL && r->mThread != NULL) {
               Int32 pid = r->mPid;
               if (pid > 0 && pid != app->mPid && pid != parentPid && pid != MY_PID) {
                    if (r->mPersistent) {
                        firstPids.PushBack(pid);
                    }
                    else {
                        lastPids[pid] = TRUE;
                    }
                }
            }
        }
    }

    // Log the ANR to the main log.
    StringBuilder info(0);
    info.Append(String("ANR in "));
    info.Append(app->mProcessName);
    if (activity != NULL && activity->mShortComponentName != NULL) {
        info.Append(String(" ("));
        info.Append(activity->mShortComponentName);
        info.Append(String(")"));
    }
    info.Append(String("\n"));
    info.Append("PID: ");
    info.Append(app->mPid);
    info.Append("\n");
    if (!annotation.IsNull()) {
        info.Append(String("Reason: "));
        info.Append(annotation);
        info.Append(String("\n"));
    }
    if (parent != NULL && parent != activity) {
        info.Append(String("Parent: "));
        info.Append(parent->mShortComponentName);
        info.Append(String("\n"));
    }

    AutoPtr<IProcessCpuTracker> processCpuTracker;
    CProcessCpuTracker::New(TRUE, (IProcessCpuTracker**)&processCpuTracker);

    AutoPtr<IFile> tracesFile = DumpStackTraces(TRUE, &firstPids, processCpuTracker, &lastPids,
        Watchdog::NATIVE_STACKS_OF_INTEREST);

    String cpuInfo;
    if (MONITOR_CPU_USAGE) {
        UpdateCpuStatsNow();
        {    AutoLock syncLock(mProcessCpuTracker);
            mProcessCpuTracker->PrintCurrentState(anrTime, &cpuInfo);
        }
        String loadStr;
        processCpuTracker->PrintCurrentLoad(&loadStr);
        info.Append(loadStr);
        info.Append(cpuInfo);
    }

    String statsStr;
    processCpuTracker->PrintCurrentState(anrTime, &statsStr);
    info.Append(statsStr);

    Slogger::E(TAG, "%s", info.ToString().string());
    if (tracesFile == NULL) {
        // There is no trace file, so dump (only) the alleged culprit's threads to the log
        Process::SendSignal(app->mPid, IProcess::SIGNAL_QUIT);
    }

    AddErrorToDropBox(String("anr"), app, app->mProcessName, activity, parent, annotation, cpuInfo, tracesFile, NULL);

    if (mController != NULL) {
//            try {
        // 0 == show dialog, 1 = keep waiting, -1 = kill process immediately
        Int32 res;
        if (SUCCEEDED(mController->AppNotResponding(app->mProcessName, app->mPid, info.ToString(), &res))) {
            if (res != 0) {
                if (res < 0 && app->mPid != MY_PID) {
                    app->Kill(String("anr"), TRUE);
                }
                else {
                    {    AutoLock syncLock(this);
                        mServices->ScheduleServiceTimeoutLocked(app);
                    }
                }
                return NOERROR;
            }
        }
        else {
            mController = NULL;
            Watchdog::GetInstance()->SetActivityController(NULL);
        }
    }

    // Unless configured otherwise, swallow ANRs in background processes & kill the process.
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    Settings::Secure::GetInt32(resolver, ISettingsSecure::ANR_SHOW_BACKGROUND, 0, &value);
    Boolean showBackground = value != 0;

    {
        AutoLock lock(this);
        if (!showBackground && !app->IsInterestingToUserLocked() && app->mPid != MY_PID) {
            app->Kill(String("bg anr"), TRUE);
            return NOERROR;
        }

        // Set the app's notResponding state, and look up the errorReportReceiver
        MakeAppNotRespondingLocked(app,
            activity != NULL ? activity->mShortComponentName : String(NULL),
            !annotation.IsNull() ? String("ANR ") + annotation : String("ANR"),
            info.ToString());

        AutoPtr<ISystemProperties> sysProps;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
        String tracesPath;
        sysProps->Get(String("dalvik.vm.stack-trace-file"), String(NULL), &tracesPath);
        if (tracesPath != NULL && tracesPath.GetLength() != 0) {
            AutoPtr<IFile> traceRenameFile;
            CFile::New(tracesPath, (IFile**)&traceRenameFile);
            String newTracesPath;
            Int32 lpos = tracesPath.LastIndexOf(".");
            if (-1 != lpos)
                newTracesPath = tracesPath.Substring(0, lpos) + "_" + app->mProcessName + tracesPath.Substring(lpos);
            else
                newTracesPath = tracesPath + "_" + app->mProcessName;
            AutoPtr<IFile> file;
            CFile::New(newTracesPath, (IFile**)&file);
            Boolean res;
            traceRenameFile->RenameTo(file, &res);
        }

        // Bring up the infamous App Not Responding dialog
        AutoPtr<StringObjectHashMap> data = new StringObjectHashMap();
        (*data)[String("app")] = app->Probe(EIID_IInterface);
        if (activity != NULL) {
            (*data)[String("activity")] = activity->Probe(EIID_IInterface);
        }

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(SHOW_NOT_RESPONDING_MSG, (IMessage**)&msg);
        msg->SetObj(data->Probe(EIID_IInterface));
        msg->SetArg1(aboveSystem ? 1 : 0);
        Boolean bval;
        mHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

ECode CActivityManagerService::ShowLaunchWarningLocked(
    /* [in] */ ActivityRecord* cur,
    /* [in] */ ActivityRecord* next)
{
    if (!mLaunchWarningShown) {
        mLaunchWarningShown = TRUE;
        AutoPtr<IRunnable> runnable = new ShowLaunchWarningLockedRunnable(this, cur, next);
        Boolean result;
        mHandler->Post(runnable, &result);
    }
    return NOERROR;
}

ECode CActivityManagerService::ClearApplicationUserData(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("clearApplicationUserData")));
    Int32 uid = Binder::GetCallingUid();
    Int32 pid = Binder::GetCallingPid();
    HandleIncomingUser(pid, uid, userId, FALSE, ALLOW_FULL_ONLY,
        String("clearApplicationUserData"), String(NULL), &userId);
    Int64 callingId = Binder::ClearCallingIdentity();
//    try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 pkgUid = -1;
    {
        AutoLock lock(this);
//            try {
        pm->GetPackageUid(packageName, userId, &pkgUid);
//            } catch (RemoteException e) {
//            }
        if (pkgUid == -1) {
            Slogger::W(TAG, "Invalid packageName:%s", packageName.string());
            if (observer != NULL) {
                if (FAILED(observer->OnRemoveCompleted(packageName, FALSE))) {
                    Slogger::I(TAG, "Observer no longer exists.");
                }
            }
            Binder::RestoreCallingIdentity(callingId);
            return FALSE;
        }
        if (uid == pkgUid || CheckComponentPermission(
               Manifest::permission::CLEAR_APP_USER_DATA,
               pid, uid, -1, TRUE)
               == IPackageManager::PERMISSION_GRANTED) {
            ForceStopPackageLocked(packageName, pkgUid, String("clear data"));
        }
        else {
            Binder::RestoreCallingIdentity(callingId);
            Slogger::E(TAG, "PID %d does not have permission %s to clear data of package %s",
                pid, Manifest::permission::CLEAR_APP_USER_DATA.string(), packageName.string());
            return E_SECURITY_EXCEPTION;
        }

        // / Remove all tasks match the cleared application package and user
        List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks->RBegin();
        while (rit != mRecentTasks->REnd()) {
            AutoPtr<TaskRecord> tr = *rit;
            AutoPtr<IComponentName> component;
            tr->GetBaseIntent()->GetComponent((IComponentName**)&component);
            String taskPackageName;
            component->GetPackageName(&taskPackageName);
            if (tr->mUserId != userId) {
                ++rit;
                continue;
            }
            if (!taskPackageName.Equals(packageName)) {
                ++rit;
                continue;
            }
            RemoveTaskByIdLocked(tr->mTaskId, 0);
        }
    }

    // try {
    //clear application user data
    String nullStr;
    pm->ClearApplicationUserData(packageName, observer, userId);

    {    AutoLock syncLock(this);
        // Remove all permissions granted from/to this package
        RemoveUriPermissionsForPackageLocked(packageName, userId, TRUE);
    }

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->FromParts(String("package"), packageName, nullStr, (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_PACKAGE_DATA_CLEARED,
            uri, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_UID, pkgUid);
    Int32 value;
    BroadcastIntentInPackage(String("android"), IProcess::SYSTEM_UID, intent,
            nullStr, NULL, 0, nullStr, NULL, nullStr, FALSE, FALSE, userId, &value);
    //     } catch (RemoteException e) {
    //     }
    // } finally {
    Binder::RestoreCallingIdentity(callingId);
    // }
    *result = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::KillBackgroundProcesses(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    if (CheckCallingPermission(Manifest::permission::KILL_BACKGROUND_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED &&
            CheckCallingPermission(Manifest::permission::RESTART_PACKAGES)
                != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: killBackgroundProcesses() from pid=%d, uid=%d requires Manifest::permission::KILL_BACKGROUND_PROCESSES",
                Binder::GetCallingPid(), Binder::GetCallingUid());
       // throw new SecurityException(msg);
       return E_SECURITY_EXCEPTION;
    }

    HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
        userId, TRUE, ALLOW_FULL_ONLY, String("killBackgroundProcesses"), String(NULL), &userId);
    Int64 callingId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    {
        AutoLock lock(this);
        Int32 appId = -1;
        // try {
        Int32 uid;
        pm->GetPackageUid(packageName, 0, &uid);
        appId = UserHandle::GetAppId(uid);
        // } catch (RemoteException e) {
        // }
        if (appId == -1) {
            Slogger::W(TAG, "Invalid packageName: %s", packageName.string());
            Binder::RestoreCallingIdentity(callingId);
            return NOERROR;
        }
        KillPackageProcessesLocked(packageName, appId, userId,
            ProcessList::SERVICE_ADJ, FALSE, TRUE, TRUE, FALSE, String("kill background"));
    }
    // } finally {
    Binder::RestoreCallingIdentity(callingId);
    // }
    return NOERROR;
}

ECode CActivityManagerService::KillAllBackgroundProcesses()
{
    if (CheckCallingPermission(Manifest::permission::KILL_BACKGROUND_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: killAllBackgroundProcesses() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::KILL_BACKGROUND_PROCESSES";
        Slogger::W(TAG, msg.ToString().string());
        // throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    Int64 callingId = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(this);
        List<AutoPtr<ProcessRecord> > procs;
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > processMap;
        processMap = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
        for (it = processMap->Begin(); it != processMap->End(); ++it) {
            AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > apps = it->mSecond;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator appIt;
            for (appIt = apps->Begin(); appIt != apps->End(); ++appIt) {
                AutoPtr<ProcessRecord> app = appIt->mSecond;
                if (app->mPersistent) {
                    // we don't kill persistent processes
                    continue;
                }
                if (app->mRemoved) {
                    procs.PushBack(app);
                }
                else if (app->mSetAdj >= ProcessList::CACHED_APP_MIN_ADJ) {
                    app->mRemoved = TRUE;
                    procs.PushBack(app);
                }
            }
        }

        List<AutoPtr<ProcessRecord> >::Iterator procIt;
        for (procIt = procs.Begin(); procIt != procs.End(); ++procIt) {
            RemoveProcessLocked(*procIt, FALSE, TRUE, String("kill all background"));
        }
        mAllowLowerMemLevel = TRUE;
        UpdateOomAdjLocked();
        DoLowMemReportIfNeededLocked(NULL);
    }
    // } finally {
    Binder::RestoreCallingIdentity(callingId);
    // }
    return NOERROR;
}

ECode CActivityManagerService::ForceStopPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    if (CheckCallingPermission(Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: forceStopPackage() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::FORCE_STOP_PACKAGES";
        Slogger::W(TAG, msg.ToString().string());
        // throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }
    Int32 callingPid = Binder::GetCallingPid();
    HandleIncomingUser(callingPid, Binder::GetCallingUid(),
        userId, TRUE, ALLOW_FULL_ONLY, String("forceStopPackage"), String(NULL), &userId);
    Int64 callingId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    {
        AutoLock lock(this);
        AutoPtr<ArrayOf<Int32> > users;
        if (userId == IUserHandle::USER_ALL) {
            users = GetUsersLocked();
        }
        else {
            users = ArrayOf<Int32>::Alloc(1);
            users->Set(0, userId);
        }
        Int32 size = users->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            Int32 user = (*users)[i];
            Int32 pkgUid = -1;
            // try {
            pm->GetPackageUid(packageName, user, &pkgUid);
            // } catch (RemoteException e) {
            // }
            if (pkgUid == -1) {
                Slogger::W(TAG, "Invalid packageName: %s", packageName.string());
                continue;
            }
            // try {
            pm->SetPackageStoppedState(packageName, TRUE, user);
            // } catch (RemoteException e) {
            // } catch (IllegalArgumentException e) {
            //     Slogger::W(TAG, "Failed trying to unstop package "
            //             + packageName + ": " + e);
            // }
            if (IsUserRunningLocked(user, FALSE)) {
                StringBuilder sb("from pid");
                sb += callingPid;
                ForceStopPackageLocked(packageName, pkgUid, sb.ToString());
            }
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(callingId);
    // }
    return NOERROR;
}

ECode CActivityManagerService::AddPackageDependency(
    /* [in] */ const String& packageName)
{
    {
        AutoLock syncLock(this);
        Int32 callingPid = Binder::GetCallingPid();
        if (callingPid == Process::MyPid()) {
            //  Yeah, um, no.
            Slogger::W(TAG, "Can't addPackageDependency on system process");
            return NOERROR;
        }
        AutoPtr<ProcessRecord> proc;
        {
            AutoLock syncLock(mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                mPidsSelfLocked.Find(Binder::GetCallingPid());
            if (it != mPidsSelfLocked.End()) {
                proc = it->mSecond;
            }
        }
        if (proc != NULL) {
            if (proc->mPkgDeps == NULL) {
                proc->mPkgDeps = new HashSet<String>(1);
            }
            proc->mPkgDeps->Insert(packageName);
        }
    }
    return NOERROR;
}

/*
 * The pkg name and app id have to be specified.
 */
ECode CActivityManagerService::KillApplicationWithAppId(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 appid,
    /* [in] */ const String& reason)
{
    if (pkg == NULL) {
        return NOERROR;
    }
    // Make sure the uid is valid.
    if (appid < 0) {
        Slogger::W(TAG, "Invalid appid specified for pkg : %s", pkg.string());
        return NOERROR;
    }
    Int32 callingUid = Binder::GetCallingUid();
    // Only the system server can kill an application
    if (callingUid == IProcess::SYSTEM_UID) {
        // Post an aysnc message to kill the application
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(KILL_APPLICATION_MSG, appid, 0, (IMessage**)&msg);
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        bundle->PutString(String("pkg"), pkg);
        bundle->PutString(String("reason"), reason);
        msg->SetObj(bundle);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
    else {
        Slogger::E(TAG, "%d cannot kill pkg: %s", callingUid, pkg.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityManagerService::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("CloseSystemDialogs")));

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    Int64 origId = Binder::ClearCallingIdentity();
    // try
    {
        AutoLock lock(this);
        // Only allow this from foreground processes, so that background
        // applications can't abuse it to prevent system UI from being shown.
        if (uid >= IProcess::FIRST_APPLICATION_UID) {
            AutoPtr<ProcessRecord> proc;
            {
                AutoLock lock(mPidsSelfLockedLock);
                proc = mPidsSelfLocked[pid];
            }
            if (proc->mCurRawAdj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                // Slogger::W(TAG, "Ignoring closeSystemDialogs " + reason
                //         + " from background process " + proc);
                Binder::RestoreCallingIdentity(origId);
                return NOERROR;
            }
        }
        CloseSystemDialogsLocked(reason);
    }
    // finally {
    Binder::RestoreCallingIdentity(origId);
    // }
    return NOERROR;
}

ECode CActivityManagerService::CloseSystemDialogsLocked(
    /* [in] */ const String& reason)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_FOREGROUND);
    if (reason != NULL) {
        intent->PutExtra(String("reason"), reason);
    }
    mWindowManager->CloseSystemDialogs(reason);

    mStackSupervisor->CloseSystemDialogsLocked();

    String nullStr;
    Int32 result;
    BroadcastIntentLocked(NULL, nullStr, intent, nullStr,
        NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE, FALSE, FALSE, -1,
        IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
    return NOERROR;
}

ECode CActivityManagerService::GetProcessMemoryInfo(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out, callee] */ ArrayOf<IDebugMemoryInfo*>** retInfos)
{
    VALIDATE_NOT_NULL(retInfos);
    *retInfos = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessMemoryInfo")));
    Int32 length = pids->GetLength();
    AutoPtr<ArrayOf<IDebugMemoryInfo*> > infos = ArrayOf<IDebugMemoryInfo*>::Alloc(length);
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    for (Int32 i = length - 1; i >= 0; i--) {
        AutoPtr<ProcessRecord> proc;
        Int32 oomAdj = 0;
        {
            AutoLock lock(this);
            {
                AutoLock innerLock(mPidsSelfLockedLock);
                HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                    mPidsSelfLocked.Find(Binder::GetCallingPid());
                if (it != mPidsSelfLocked.End()) {
                    proc = it->mSecond;
                }
                oomAdj = proc != NULL ? proc->mSetAdj : 0;
            }
        }
        AutoPtr<IDebugMemoryInfo> info;
        CDebugMemoryInfo::New((IDebugMemoryInfo**)&info);
        infos->Set(i, info);
        dbg->GetMemoryInfo((*pids)[i], (IDebugMemoryInfo**)&info);
        if (proc != NULL) {
            {
                AutoLock lock(this);
                if (proc->mThread != NULL && proc->mSetAdj == oomAdj) {
                    // Record this for posterity if the process has been stable.
                    Int32 pss, uss;
                    info->GetTotalPss(&pss);
                    info->GetTotalUss(&uss);
                    proc->mBaseProcessTracker->AddPss(pss, uss, FALSE, proc->mPkgList);
                }
            }
        }
    }
    *retInfos = infos;
    REFCOUNT_ADD(*retInfos);
    return NOERROR;
}

ECode CActivityManagerService::GetProcessPss(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out, callee] */ ArrayOf<Int64>** processPss)
{
    VALIDATE_NOT_NULL(processPss);
    *processPss = NULL;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessPss")));
    Int32 length = pids->GetLength();
    AutoPtr<ArrayOf<Int64> > pss = ArrayOf<Int64>::Alloc(length);
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    for (Int32 i = length - 1; i >= 0; i--) {
        AutoPtr<ProcessRecord> proc;
        Int32 oomAdj = 0;
        {
            AutoLock lock(this);
            {
                AutoLock innerLock(mPidsSelfLockedLock);
                HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                    mPidsSelfLocked.Find((*pids)[i]);
                if (it != mPidsSelfLocked.End()) {
                    proc = it->mSecond;
                }
                oomAdj = proc != NULL ? proc->mSetAdj : 0;
            }
        }
        AutoPtr<ArrayOf<Int64> > tmpUss;
        dbg->GetPss((*pids)[i], (ArrayOf<Int64>**)&tmpUss, &(*pss)[i]);
        if (proc != NULL) {
            AutoLock lock(this);
            if (proc->mThread != NULL && proc->mSetAdj == oomAdj) {
                // Record this for posterity if the process has been stable.
                proc->mBaseProcessTracker->AddPss((*pss)[i], (*tmpUss)[0], FALSE, proc->mPkgList);
            }
        }
    }
    *processPss = pss;
    REFCOUNT_ADD(*processPss);
    return NOERROR;
}

ECode CActivityManagerService::KillApplicationProcess(
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid)
{
    if (processName.IsNull()) {
        return NOERROR;
    }

    Int32 callingUid = Binder::GetCallingUid();
    // Only the system server can kill an application
    if (callingUid == IProcess::SYSTEM_UID) {
        {
            AutoLock lock(this);
            AutoPtr<ProcessRecord> app = GetProcessRecordLocked(processName, uid, TRUE);
            if (app != NULL && app->mThread != NULL) {
                // try {
                app->mThread->ScheduleSuicide();
                // } catch (RemoteException e) {
                //     // If the other end already died, then our work here is done.
                // }
            }
            else {
                Slogger::W(TAG, "Process/uid not found attempting kill of %s / %d",
                        processName.string(), uid);
            }
        }
    }
    else {
        // throw new SecurityException(callingUid + " cannot kill app process: " +
        //         processName);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityManagerService::ForceStopPackageLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ const String& reason)
{
    String nullStr;
    ForceStopPackageLocked(packageName, UserHandle::GetAppId(uid), FALSE,
            FALSE, TRUE, FALSE, FALSE, UserHandle::GetUserId(uid), reason);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->FromParts(String("package"), packageName, nullStr, (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_PACKAGE_RESTARTED,
            uri, (IIntent**)&intent);
    if (!mProcessesReady) {
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                | IIntent::FLAG_RECEIVER_FOREGROUND);
    }
    intent->PutExtra(IIntent::EXTRA_UID, uid);
    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, UserHandle::GetUserId(uid));
    Int32 result;
    return BroadcastIntentLocked(NULL, nullStr, intent,
        nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
        FALSE, FALSE,
        MY_PID, IProcess::SYSTEM_UID, UserHandle::GetUserId(uid), &result);
}

ECode CActivityManagerService::ForceStopUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ const String& reason)
{
    String nullStr;
    ForceStopPackageLocked(nullStr, -1, FALSE, FALSE, TRUE, FALSE, FALSE, userId, reason);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_USER_STOPPED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
    Int32 result;
    BroadcastIntentLocked(NULL, nullStr, intent,
        nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
        FALSE, FALSE,
        MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
    return NOERROR;
}

Boolean CActivityManagerService::KillPackageProcessesLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appId,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 minOomAdj,
    /* [in] */ Boolean callerWillRestart,
    /* [in] */ Boolean allowRestart,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ const String& reason)
{
    List< AutoPtr<ProcessRecord> > procs;

    // Remove all processes this package may have touched: all with the
    // same UID (except for the system or root user), and all whose name
    // matches the package name.
    AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > map;
    map = mProcessNames->GetMap();
    HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
    it = map->Begin();
    for (; it != map->End(); ++it) {
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator iit = it->mSecond->Begin();
        for (; iit != it->mSecond->End(); ++iit) {
            AutoPtr<ProcessRecord> app = iit->mSecond;
            if (app->mPersistent && !evenPersistent) {
                // we don't kill persistent processes
                continue;
            }
            if (app->mRemoved) {
                if (doit) {
                    procs.PushBack(app);
                }
                continue;
            }

            // Skip process if it doesn't meet our oom adj requirement.
            if (app->mSetAdj < minOomAdj) {
                 continue;
            }

            // If no package is specified, we call all processes under the
            // give user id.
            if (packageName.IsNull()) {
                if (app->mUserId != userId) {
                    continue;
                }
                if (appId >= 0 && UserHandle::GetAppId(app->mUid) != appId) {
                    continue;
                }
            // Package has been specified, we want to hit all processes
            // that match it.  We need to qualify this by the processes
            // that are running under the specified app and user ID.
            }
            else {
                Boolean isDep = app->mPkgDeps != NULL
                    && app->mPkgDeps->Find(packageName) != app->mPkgDeps->End();
                if (!isDep && UserHandle::GetAppId(app->mUid) != appId) {
                    continue;
                }
                if (userId != IUserHandle::USER_ALL && app->mUserId != userId) {
                    continue;
                }

                Boolean res;
                if (!(app->mPkgList->ContainsKey(CoreUtils::Convert(packageName), &res), res) && !isDep) {
                    continue;
                }
            }

            // Process has passed all conditions, kill it!
            if (!doit) {
                return TRUE;
            }
            app->mRemoved = TRUE;
            procs.PushBack(app);
        }
    }

    List< AutoPtr<ProcessRecord> >::Iterator iter = procs.Begin();
    for (; iter != procs.End(); ++iter) {
        RemoveProcessLocked(*iter, callerWillRestart, allowRestart, reason);
    }
    UpdateOomAdjLocked();
    return !procs.IsEmpty();
}

Boolean CActivityManagerService::ForceStopPackageLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 appId,
    /* [in] */ Boolean callerWillRestart,
    /* [in] */ Boolean purgeCache,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Boolean uninstalling,
    /* [in] */ Int32 userId,
    /* [in] */ const String& reason)
{
    if (userId == IUserHandle::USER_ALL && name.IsNull()) {
        Slogger::W(TAG, "Can't force stop all processes of all users, that is insane!");
    }

    if (appId < 0 && !name.IsNull()) {
        Int32 uid;
        AppGlobals::GetPackageManager()->GetPackageUid(name, 0, &uid);
        appId = UserHandle::GetAppId(uid);
    }

    if (doit) {
        if (!name.IsNull()) {
            Slogger::I(TAG, "Force stopping %s appId=%d user=%d: %s",
                    name.string(), appId, userId, reason.string());
        }
        else {
            Slogger::I(TAG, "Force stopping user %d: %s", userId, reason.string());
        }
        HashMap<String, AutoPtr<HashMap<Int32, Int64> > >::Iterator it = mProcessCrashTimes->GetMap()->Begin();
        for (; it != mProcessCrashTimes->GetMap()->End();) {
            AutoPtr<HashMap<Int32, Int64> > ba = it->mSecond;
            HashMap<Int32, Int64>::Iterator it2 = ba->Begin();
            for (; it2 != ba->End();) {
                Boolean remove = FALSE;
                Int32 entUid = it2->mFirst;
                if (!name.IsNull()) {
                    if (userId == IUserHandle::USER_ALL) {
                        if (UserHandle::GetAppId(entUid) == appId) {
                            remove = TRUE;
                        }
                    }
                    else {
                        Int32 uid = UserHandle::GetUid(userId, appId);
                        if (entUid == uid) {
                            remove = TRUE;
                        }
                    }
                }
                else if (UserHandle::GetUserId(entUid) == userId) {
                    remove = TRUE;
                }

                if (remove) {
                    ba->Erase(it2++);
                }
                else {
                    ++it2;
                }
            }

            if (ba->IsEmpty()) {
                mProcessCrashTimes->GetMap()->Erase(it++);
            }
            else {
                ++it;
            }
        }
    }

    StringBuilder sb("stop ");
    if (name.IsNull()) {
        sb += "user ";
        sb += userId;
    }
    else {
        sb += name;
    }

    Boolean didSomething = KillPackageProcessesLocked(name, appId, userId,
            -100, callerWillRestart, TRUE, doit, evenPersistent,
            sb.ToString());

    if (mStackSupervisor->ForceStopPackageLocked(name, doit, evenPersistent, userId)) {
        if (!doit) {
            return TRUE;
        }
        didSomething = TRUE;
    }

    if (mServices->ForceStopLocked(name, userId, evenPersistent, doit)) {
        if (!doit) {
            return TRUE;
        }
        didSomething = TRUE;
    }

    if (name.IsNull()) {
        // Remove all sticky broadcasts from this user.
        StickyBroadcastIterator sbit = mStickyBroadcasts.Find(userId);
        if (sbit != mStickyBroadcasts.End()) {
            mStickyBroadcasts.Erase(sbit);
        }
    }

    List<AutoPtr<ContentProviderRecord> > providers;
    if (mProviderMap->CollectForceStopProviders(name, appId, doit, evenPersistent,
        userId, &providers)) {
        if (!doit) {
            return TRUE;
        }
        didSomething = TRUE;
    }

    List<AutoPtr<ContentProviderRecord> >::Iterator it;
    for (it = providers.Begin(); it != providers.End(); ++it) {
        RemoveDyingProviderLocked(NULL, *it, TRUE);
    }

    // Remove transient permissions granted from/to this package/user
    RemoveUriPermissionsForPackageLocked(name, userId, FALSE);

    if (name.IsNull() || uninstalling) {
        // Remove pending intents.  For now we only do this when force
        // stopping users, because we have some problems when doing this
        // for packages -- app widgets are not currently cleaned up for
        // such packages, so they can be left with bad pending intents.
        if (!mIntentSenderRecords.IsEmpty()) {
            PendingIntentRecordHashMap::Iterator it = mIntentSenderRecords.Begin();
            for (; it != mIntentSenderRecords.End();) {
                AutoPtr<IWeakReference> wpir = it->mSecond;
                if (wpir == NULL) {
                    mIntentSenderRecords.Erase(it++);
                    continue;
                }
                AutoPtr<IIIntentSender> _pir;
                wpir->Resolve(EIID_IIIntentSender, (IInterface**)&_pir);
                if (_pir == NULL) {
                    mIntentSenderRecords.Erase(it++);
                    continue;
                }

                CPendingIntentRecord* pir = (CPendingIntentRecord*)_pir.Get();
                if (name.IsNull()) {
                    // Stopping user, remove all objects for the user.
                    if (pir->mKey->mUserId != userId) {
                        ++it;
                        // Not the same user, skip it.
                        continue;
                    }
                }
                else {
                    Int32 uappId = UserHandle::GetAppId(pir->mUid);
                    if (uappId != appId) {
                        ++it;
                        // Different app id, skip it.
                        continue;
                    }
                    if (userId != IUserHandle::USER_ALL && pir->mKey->mUserId != userId) {
                        ++it;
                        // Different user, skip it.
                        continue;
                    }
                    if (!pir->mKey->mPackageName.Equals(name)) {
                        ++it;
                        // Different package, skip it.
                        continue;
                    }
                }
                if (!doit) {
                    return TRUE;
                }

                didSomething = TRUE;
                mIntentSenderRecords.Erase(it++);
                pir->mCanceled = TRUE;
                if (pir->mKey->mActivity != NULL) {
                    pir->mKey->mActivity->mPendingResults->Erase(pir->mRef);
                }
            }
        }
    }

    if (doit) {
        if (purgeCache && !name.IsNull()) {
            AutoPtr<AttributeCache> ac = AttributeCache::GetInstance();
            if (ac != NULL) {
                ac->RemovePackage(name);
            }
        }
        if (mBooted) {
            mStackSupervisor->ResumeTopActivitiesLocked();
            mStackSupervisor->ScheduleIdleLocked();
        }
    }

    return didSomething;
}

Boolean CActivityManagerService::RemoveProcessLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean callerWillRestart,
    /* [in] */ Boolean allowRestart,
    /* [in] */ const String& reason)
{
    if (app == NULL)
        return TRUE;

    String name = app->mProcessName;
    Int32 uid = app->mUid;
    if (DEBUG_PROCESSES) {
        String appDes = app->ToString();
        Slogger::D(TAG, "Force removing proc %s (%s/%d)",
                appDes.string(), name.string(), uid);
    }

    mProcessNames->Remove(name, uid);
    mIsolatedProcesses.Erase(uid);
    if (mHeavyWeightProcess.Get() == app) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG,
            mHeavyWeightProcess->mUserId, 0, (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessage(msg, &result);

        mHeavyWeightProcess = NULL;
    }
    Boolean needRestart = FALSE;
    if (app->mPid > 0 && app->mPid != MY_PID) {
        Int32 pid = app->mPid;
        {
            AutoLock lock(mPidsSelfLockedLock);
            mPidsSelfLocked.Erase(pid);
            mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, app->Probe(EIID_IInterface));
        }
        Int32 infoUid;
        app->mInfo->GetUid(&infoUid);
        mBatteryStatsService->NoteProcessFinish(app->mProcessName, infoUid);
        if (app->mIsolated) {
            mBatteryStatsService->RemoveIsolatedUid(app->mUid, infoUid);
        }
        Boolean willRestart = FALSE;
        if (app->mPersistent && !app->mIsolated) {
            if (!callerWillRestart) {
                willRestart = TRUE;
            }
            else {
                needRestart = TRUE;
            }
        }
        app->Kill(reason, TRUE);
        HandleAppDiedLocked(app, willRestart, allowRestart);
        if (willRestart) {
            RemoveLruProcessLocked(app);
            AddAppLocked(app->mInfo, FALSE, String(NULL) /* ABI override */);
        }
    }
    else {
        mRemovedProcesses->Add((IProcessRecord*)app);
    }

    return needRestart;
}

ECode CActivityManagerService::ProcessStartTimedOutLocked(
    /* [in] */ ProcessRecord* app)
{
    const Int32 pid = app->mPid;
    Boolean gone = FALSE;
    {
        AutoLock lock(mPidsSelfLockedLock);
        AutoPtr<ProcessRecord> knownApp;
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(pid);
        if (it != mPidsSelfLocked.End()) {
            knownApp = it->mSecond;
        }
        if (knownApp != NULL && knownApp->mThread == NULL) {
            mPidsSelfLocked.Erase(pid);
            gone = TRUE;
        }
    }

    if (gone) {
        Slogger::W(TAG, "Process %p failed to attach", app);
//         EventLog.writeEvent(EventLogTags.AM_PROCESS_START_TIMEOUT, app.userId,
//                 pid, app.uid, app.processName);
        mProcessNames->Remove(app->mProcessName, app->mUid);
        mIsolatedProcesses.Erase(app->mUid);
        if (mHeavyWeightProcess.Get() == app) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG,
                mHeavyWeightProcess->mUserId, 0, (IMessage**)&msg);
            Boolean result;
            mHandler->SendMessage(msg, &result);

            mHeavyWeightProcess = NULL;
        }
        Int32 infoUid;
        app->mInfo->GetUid(&infoUid);
        mBatteryStatsService->NoteProcessFinish(app->mProcessName, infoUid);
        if (app->mIsolated) {
            mBatteryStatsService->RemoveIsolatedUid(app->mUid, infoUid);
        }
        // Take care of any launching providers waiting for this process.
        CheckAppInLaunchingProvidersLocked(app, TRUE);
        // Take care of any services that are waiting for the process.
        mServices->ProcessStartTimedOutLocked(app);
        app->Kill(String("start timeout"), TRUE);
        RemoveLruProcessLocked(app);
        if (mBackupTarget != NULL && mBackupTarget->mApp->mPid == pid) {
            Slogger::W(TAG, "Unattached app died before backup, skipping");
            // try {
            AutoPtr<IInterface> backupService = ServiceManager::GetService(IContext::BACKUP_SERVICE);
            AutoPtr<IIBackupManager> bm = IIBackupManager::Probe(backupService);
            String pkgName;
            IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&pkgName);
            bm->AgentDisconnected(pkgName);
            // } catch (RemoteException e) {
            //     // Can't happen; the backup manager is local
            // }
        }
        if (IsPendingBroadcastProcessLocked(pid)) {
            Slogger::W(TAG, "Unattached app died before broadcast acknowledged, skipping");
            SkipPendingBroadcastLocked(pid);
        }
    }
    else {
        Slogger::W(TAG, "Spurious process start timeout - pid not known for %p", app);
    }
    return NOERROR;
}

Boolean CActivityManagerService::AttachApplicationLocked(
    /* [in] */ IApplicationThread* thread,
    /* [in] */ Int32 pid)
{
    // Find the application record that is being attached...  either via
    // the pid if we are running in multiple processes, or just pull the
    // next app record if we are emulating process with anonymous threads.
    AutoPtr<ProcessRecord> app;
    if (pid != MY_PID && pid >= 0) {
        AutoLock lock(mPidsSelfLockedLock);
        app = mPidsSelfLocked[pid];
    }
    else {
        app = NULL;
    }

    if (app == NULL) {
        Slogger::W(TAG, "No pending application record for pid %d (IApplicationThread %s); dropping process",
                pid, TO_CSTR(thread));
        //EventLog.writeEvent(EventLogTags.AM_DROP_PROCESS, pid);
        if (pid > 0 && pid != MY_PID) {
            // Int32 uid;
            // app->mInfo->GetUid(&uid);
            Process::KillProcessQuiet(pid);
            // Process::KillProcessGroup(uid, pid);
        }
        else {
            thread->ScheduleExit();
        }
        return FALSE;
    }

    // If this application record is still attached to a previous
    // process, clean it up now.
    if (app->mThread != NULL) {
        HandleAppDiedLocked(app, TRUE, TRUE);
    }

    // Tell the process all about itself.

    if (localLOGV) {
        Slogger::V(TAG, "Binding process pid %d to record %s", pid, TO_CSTR(app));
    }

    String processName = app->mProcessName;

    AutoPtr<AppDeathRecipient> adr = new AppDeathRecipient(app, pid, thread, this);
    AutoPtr< ::IProxy > proxy = (::IProxy*)thread->Probe(::EIID_IProxy);
    assert(proxy != NULL);
    if (FAILED(proxy->LinkToDeath(adr.Get(), 0))) {
        app->ResetPackageList(mProcessStats);
        StartProcessLocked(app, String("link fail"), processName);
        return FALSE;
    }
    app->mDeathRecipient = adr;
//    EventLog.writeEvent(EventLogTags.AM_PROC_BOUND, app.userId, app.pid, app.processName);

    app->MakeActive(thread, mProcessStats);
    app->mCurAdj = app->mSetAdj = -100;
    app->mCurSchedGroup = IProcess::THREAD_GROUP_DEFAULT;
    app->mSetSchedGroup = IProcess::THREAD_GROUP_DEFAULT;
    app->mForcingToForeground = NULL;
    UpdateProcessForegroundLocked(app, FALSE, FALSE);
    app->mHasShownUi = FALSE;
    app->mDebugging = FALSE;
    app->mCached = FALSE;
    app->mKilledByAm = FALSE;

    mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, app->Probe(EIID_IInterface));

    AutoPtr<IList> providers;
    Boolean normalMode = mProcessesReady || IsAllowedWhileBooting(app->mInfo);
    if (normalMode) {
        providers = GenerateApplicationProvidersLocked(app);
    }

    if (!normalMode) {
        Slogger::I(TAG, "Launching preboot mode app: %s", TO_CSTR(app));
    }

    if (localLOGV) {
        Slogger::V(TAG, "New app %s record %s, thread=%s pid=%d",
            processName.string(), TO_CSTR(app), TO_CSTR(thread), pid);
    }
//    try {
    Int32 testMode = IApplicationThread::DEBUG_OFF;
    if (!mDebugApp.IsNull() && !mDebugApp.Compare(processName)) {
        testMode = mWaitForDebugger
            ? IApplicationThread::DEBUG_WAIT
            : IApplicationThread::DEBUG_ON;
        app->mDebugging = TRUE;
        if (mDebugTransient) {
            mDebugApp = mOrigDebugApp;
            mWaitForDebugger = mOrigWaitForDebugger;
        }
    }

    String profileFile = app->mInstrumentationProfileFile;
    AutoPtr<IParcelFileDescriptor> profileFd;
    Int32 samplingInterval = 0;
    Boolean profileAutoStop = FALSE;
    if (!mProfileApp.IsNull() && mProfileApp.Equals(processName)) {
        mProfileProc = app;
        profileFile = mProfileFile;
        profileFd = mProfileFd;
        samplingInterval = mSamplingInterval;
        profileAutoStop = mAutoStopProfiler;
    }

    Boolean enableOpenGlTrace = FALSE;
    if (!mOpenGlTraceApp.IsNull() && mOpenGlTraceApp.Equals(processName)) {
        enableOpenGlTrace = TRUE;
        mOpenGlTraceApp = NULL;
    }

    // If the app is being launched for restore or full backup, set it up specially
    Boolean isRestrictedBackupMode = FALSE;
    if (mBackupTarget != NULL && mBackupAppName.Equals(processName)) {
        isRestrictedBackupMode = (mBackupTarget->mBackupMode == BackupRecord::RESTORE)
                || (mBackupTarget->mBackupMode == BackupRecord::RESTORE_FULL)
                || (mBackupTarget->mBackupMode == BackupRecord::BACKUP_FULL);
    }

    String insPkgName, infoPkgName;
    if (app->mInstrumentationInfo != NULL) {
        IPackageItemInfo::Probe(app->mInstrumentationInfo)->GetPackageName(&insPkgName);
        EnsurePackageDexOpt(insPkgName);
    }
    else {
        IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&infoPkgName);
        EnsurePackageDexOpt(infoPkgName);
    }
    if (app->mInstrumentationClass != NULL) {
        String classPkgName;
        app->mInstrumentationClass->GetPackageName(&classPkgName);
        EnsurePackageDexOpt(classPkgName);
    }
    if (DEBUG_CONFIGURATION) {
        Slogger::V(TAG, "Binding proc %s with config %s", processName.string(), TO_CSTR(mConfiguration));
    }
    AutoPtr<IApplicationInfo> appInfo = app->mInstrumentationInfo != NULL
            ? app->mInstrumentationInfo : app->mInfo;
    app->mCompat = CompatibilityInfoForPackageLocked(appInfo);
    if (profileFd != NULL) {
        AutoPtr<IParcelFileDescriptor> pfd;
        profileFd->Dup((IParcelFileDescriptor**)&pfd);
        profileFd = pfd;
    }
    AutoPtr<IProfilerInfo> profilerInfo;
    if (profileFile != NULL) {
        CProfilerInfo::New(profileFile, profileFd, samplingInterval,
            profileAutoStop, (IProfilerInfo**)&profilerInfo);
    }
    ECode ec = NOERROR;
    AutoPtr<IHashMap> services = GetCommonServicesLocked();
    AutoPtr<IConfiguration> configuration;
    CConfiguration::New(mConfiguration, (IConfiguration**)&configuration);
    AutoPtr<IBundle> coreSettings = mCoreSettingsObserver->GetCoreSettingsLocked();
    ec = thread->BindApplication(processName, appInfo, providers,
        app->mInstrumentationClass, profilerInfo, app->mInstrumentationArguments,
        app->mInstrumentationWatcher, app->mInstrumentationUiAutomationConnection, testMode,
        enableOpenGlTrace, isRestrictedBackupMode || !normalMode, app->mPersistent,
        configuration, app->mCompat, IMap::Probe(services), coreSettings);
    if (FAILED(ec)) {
        // todo: Yikes!  What should we do?  For now we will try to
        // start another process, but that could easily get us in
        // an infinite loop of restarting processes...
        Slogger::W(TAG, "Exception thrown during bind of %s, ec=%08x", TO_CSTR(app), ec);

        app->ResetPackageList(mProcessStats);
        app->UnlinkDeathRecipient();
        StartProcessLocked(app, String("bind fail"), processName);
        return FALSE;
    }

    UpdateLruProcessLocked(app, FALSE, NULL);
    app->mLastRequestedGc = app->mLastLowMemory = SystemClock::GetUptimeMillis();

    // Remove this record from the list of starting applications.
    mPersistentStartingProcesses->Remove((IProcessRecord*)app.Get());
    Boolean contains;
    if (DEBUG_PROCESSES && (mProcessesOnHold->Contains((IProcessRecord*)app.Get(), &contains), contains)) {
        Slogger::V(TAG, "Attach application locked removing on hold: %s", TO_CSTR(app));
    }
    mProcessesOnHold->Remove((IProcessRecord*)app.Get());

    Boolean badApp = FALSE, didSomething = FALSE, bval = FALSE;

    // See if the top visible activity is waiting to run in this process...
    if (normalMode) {
        ec = mStackSupervisor->AttachApplicationLocked(app, &bval);
        if (bval) {
            didSomething = TRUE;
        }
        if (FAILED(ec)) {
            Slogger::W(TAG, "Exception thrown starting activities in %s", TO_CSTR(app));
            badApp = TRUE;
        }
    }

    // Find any services that should be running in this process...
    if (!badApp) {
        ec = mServices->AttachApplicationLocked(app, processName, &bval);
        didSomething |= bval;
        if (FAILED(ec)) {
            Slogger::W/*wtf*/(TAG, "Exception thrown starting services in %s", TO_CSTR(app));
            badApp = TRUE;
        }
    }

    // Check if a next-broadcast receiver is in this process...
    if (!badApp && IsPendingBroadcastProcessLocked(pid)) {
        // try {
        didSomething |= SendPendingBroadcastsLocked(app);
        //} catch (Exception e) {
            // If the app died trying to launch the receiver we declare it 'bad'
            // Slog.wtf(TAG, "Exception thrown dispatching broadcasts in " + app, e);
            // badApp = TRUE;
        //}
    }
    // Check whether the next backup agent is in this process...
    Int32 backupUid;
    if (!badApp && mBackupTarget != NULL
        && (mBackupTarget->mAppInfo->GetUid(&backupUid), backupUid == app->mUid)) {
        if (DEBUG_BACKUP) Slogger::V(TAG, "New app is backup target, launching agent for %s", TO_CSTR(app));
        String pkgName;
        IPackageItemInfo::Probe(mBackupTarget->mAppInfo)->GetPackageName(&pkgName);
        EnsurePackageDexOpt(pkgName);
        if (FAILED(thread->ScheduleCreateBackupAgent(mBackupTarget->mAppInfo,
            CompatibilityInfoForPackageLocked(mBackupTarget->mAppInfo),
            mBackupTarget->mBackupMode))) {
            Slogger::W/*wtf*/(TAG, "Exception thrown creating backup agent in %s", TO_CSTR(app));
            badApp = TRUE;
        }
    }

    if (badApp) {
        // todo: Also need to kill application to deal with all
        // kinds of exceptions.
        app->Kill(String("error during init"), TRUE);
        HandleAppDiedLocked(app, FALSE, TRUE);
        return FALSE;
    }

    if (!didSomething) {
        UpdateOomAdjLocked();
    }

    return TRUE;
}

ECode CActivityManagerService::AttachApplication(
    /* [in] */ IApplicationThread* thread)
{
    AutoLock lock(this);

    Int32 callingPid = Binder::GetCallingPid();
    const Int64 origId = Binder::ClearCallingIdentity();
    AttachApplicationLocked(thread, callingPid);
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ActivityIdle(
    /* [in] */ IBinder* token,
    /* [in] */ IConfiguration* config,
    /* [in] */ Boolean stopProfiling)
{
    const Int64 origId = Binder::ClearCallingIdentity();
    {
        AutoLock lock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            AutoPtr<ActivityRecord> r;
            mStackSupervisor->ActivityIdleInternalLocked(token, FALSE, config, (ActivityRecord**)&r);
            if (stopProfiling) {
                if ((mProfileProc == r->mApp) && (mProfileFd != NULL)) {
                    mProfileFd->Close();
                    ClearProfilerLocked();
                }
            }
        }
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::PostFinishBooting(
    /* [in] */ Boolean finishBooting,
    /* [in] */ Boolean enableScreen)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(FINISH_BOOTING_MSG,
        finishBooting? 1 : 0, enableScreen ? 1 : 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CActivityManagerService::EnableScreenAfterBoot()
{
//    EventLog.writeEvent(EventLogTags.BOOT_PROGRESS_ENABLE_SCREEN,
//            SystemClock.uptimeMillis());
    mWindowManager->EnableScreenAfterBoot();
    AutoLock lock(this);
    UpdateEventDispatchingLocked();

    return NOERROR;
}

ECode CActivityManagerService::ShowBootMessage(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean always)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("showBootMessage")));
    mWindowManager->ShowBootMessage(msg, always);
    return NOERROR;
}

ECode CActivityManagerService::KeyguardWaitingForActivityDrawn()
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("KeyguardWaitingForActivityDrawn")));
    const Int64 token = Binder::ClearCallingIdentity();
//     try {
        {
            AutoLock lock(this);
            if (DEBUG_LOCKSCREEN) LogLockScreen(String(""));
                mWindowManager->KeyguardWaitingForActivityDrawn();
            if (mLockScreenShown) {
                mLockScreenShown = FALSE;
                ComeOutOfSleepIfNeededLocked();
            }
        }
//     } finally {
        Binder::RestoreCallingIdentity(token);
//     }
    return NOERROR;
}

ECode CActivityManagerService::FinishBooting()
{
    {
        AutoLock syncLock(this);
        if (!mBootAnimationComplete) {
            mCallFinishBooting = TRUE;
            return NOERROR;
        }
        mCallFinishBooting = FALSE;
    }

    // Register receivers to handle package update events

    AutoPtr<ILooper> looper = Looper::GetMainLooper();
    mPackageMonitor->Register(mContext, looper, UserHandle::ALL, FALSE);

    AutoPtr<BroadcastReceiver> receiver = new ThemeChangeReceiver(this);
    ThemeUtils::RegisterThemeChangeReceiver(mContext, receiver);

    // Let system services know.
    mSystemServiceManager->StartBootPhase(SystemService::PHASE_BOOT_COMPLETED);

    {
        AutoLock lock(this);
        String nullStr;

        // Ensure that any processes we had put on hold are now started
        // up.
        Int32 NP;
        mProcessesOnHold->GetSize(&NP);
        if (NP > 0) {
            AutoPtr<IArrayList> proces;
            CArrayList::New(ICollection::Probe(mProcessesOnHold), (IArrayList**)&proces);
            for (Int32 ip = 0; ip < NP; ip++) {
                AutoPtr<IInterface> obj;
                proces->Get(ip, (IInterface**)&obj);
                ProcessRecord* app = (ProcessRecord*)IProcessRecord::Probe(obj);
                if (DEBUG_PROCESSES) Slogger::V(TAG, "Starting process on hold: %s", TO_CSTR(app));
                StartProcessLocked(app, String("onHold"), nullStr);
            }
        }

        if (mFactoryTest != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
            // Start looking for apps that are abusing wake locks.
            Boolean result;
            mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);

            // Tell anyone interested that we are done booting!
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            sysProp->Set(String("sys.boot_completed"), String("1"));
            // And trigger dev.bootcomplete if we are not showing encryption progress
            String str1, str2;
            sysProp->Get(String("vold.decrypt"), &str1);
            sysProp->Get(String("vold.encrypt_progress"), &str2);
            if (!str1.Equals("trigger_restart_min_framework") || str2.Equals("")) {
                sysProp->Set(String("dev.bootcomplete"), String("1"));
            }

            Int32 userId, ival;
            HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it;
            for (it = mStartedUsers.Begin(); it != mStartedUsers.End(); ++it) {
                AutoPtr<UserStartedState> uss = it->mSecond;
                if (uss->mState == UserStartedState::STATE_BOOTING) {
                    uss->mState = UserStartedState::STATE_RUNNING;
                    userId = it->mFirst;
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_BOOT_COMPLETED, NULL, (IIntent**)&intent);
                    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                    intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
                    AutoPtr<IIntentReceiver> receiver;
                    CActivityManagerBootCompletedReceiver::New(this, (IIntentReceiver**)&receiver);
                    //TODO:
                    BroadcastIntentLocked(NULL, nullStr, intent,
                        nullStr, receiver, 0, nullStr, NULL,
                        Manifest::permission::RECEIVE_BOOT_COMPLETED, IAppOpsManager::OP_BOOT_COMPLETED,
                        TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &ival);
                }
            }
            ScheduleStartProfilesLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::BootAnimationComplete()
{
    Boolean callFinishBooting = FALSE;
    {
        AutoLock syncLock(this);
        callFinishBooting = mCallFinishBooting;
        mBootAnimationComplete = TRUE;
    }
    if (callFinishBooting) {
        FinishBooting();
    }
    return NOERROR;
}

ECode CActivityManagerService::EnsureBootCompleted()
{
   Boolean booting;
   Boolean enableScreen;
   {
       AutoLock lock(this);
       booting = mBooting;
       mBooting = FALSE;
       enableScreen = !mBooted;
       mBooted = TRUE;
   }

   if (booting) {
       FinishBooting();
   }

   if (enableScreen) {
       EnableScreenAfterBoot();
   }
    return NOERROR;
}

ECode CActivityManagerService::ActivityResumed(
    /* [in] */ IBinder* token)
{
    const Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            ActivityRecord::ActivityResumedLocked(token);
        }
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ActivityPaused(
    /* [in] */ IBinder* token)
{
    const Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            stack->ActivityPausedLocked(token, FALSE);
        }
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ActivityStopped(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* icicle,
    /* [in] */ IPersistableBundle* persistentState,
    /* [in] */ ICharSequence* description)
{
    if (localLOGV) {
        Slogger::V(TAG, "Activity stopped: token=%s", TO_CSTR(token));
    }
    // Refuse possible leaked file descriptors
    Boolean hasFD = FALSE;
    if (icicle != NULL && (icicle->HasFileDescriptors(&hasFD), hasFD)) {
        Slogger::E(TAG, "File descriptors passed in Bundle");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const Int64 origId = Binder::ClearCallingIdentity();

    {
        AutoLock lock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            r->mTask->mStack->ActivityStoppedLocked(r, icicle, persistentState, description);
        }
    }

    TrimApplications();

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ActivityDestroyed(
    /* [in] */ IBinder* token)
{
    if (DEBUG_SWITCH) {
        Slogger::V(TAG, "ACTIVITY DESTROYED: %s", TO_CSTR(token));
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            stack->ActivityDestroyedLocked(token);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::BackgroundResourcesReleased(
    /* [in] */ IBinder* token)
{
    const Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            stack->BackgroundResourcesReleased(token);
        }
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::NotifyLaunchTaskBehindComplete(
    /* [in] */ IBinder* token)
{
    return mStackSupervisor->ScheduleLaunchTaskBehindComplete(token);
}

ECode CActivityManagerService::NotifyEnterAnimationComplete(
    /* [in] */ IBinder* token)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(ENTER_ANIMATION_COMPLETE_MSG, token, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CActivityManagerService::GetCallingPackage(
    /* [in] */ IBinder* token,
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package);
    *package = NULL;

    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = GetCallingRecordLocked(token);
    if (r != NULL && r->mInfo != NULL) {
        return IPackageItemInfo::Probe(r->mInfo)->GetPackageName(package);
    }

    return NOERROR;
}

ECode CActivityManagerService::GetCallingActivity(
    /* [in] */ IBinder* token,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName);
    AutoLock lock(this);

    AutoPtr<ActivityRecord> r = GetCallingRecordLocked(token);
    if (r != NULL){
        return r->mIntent->GetComponent(componentName);
    }

    *componentName = NULL;
    return NOERROR;
}

ECode CActivityManagerService::GetCallingPackageForBroadcast(
    /* [in] */ Boolean foreground,
    /* [out] */ String* pkg)
{
    AutoPtr<BroadcastQueue> queue = foreground ? mFgBroadcastQueue : mBgBroadcastQueue;
    AutoPtr<BroadcastRecord> r = queue->GetProcessingBroadcast();
    if (r != NULL) {
        *pkg = r->mCallerPackage;
        return NOERROR;
    }
    else {
        Logger::E(TAG, "Broadcast sender is only retrievable in the onReceive");
    }
    *pkg = NULL;
    return NOERROR;
}

AutoPtr<ActivityRecord> CActivityManagerService::GetCallingRecordLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ActivityRecord> resultTo;
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r != NULL) {
        resultTo = r->mResultTo;
    }
    return resultTo;
}

ECode CActivityManagerService::GetActivityClassForToken(
    /* [in] */ IBinder* token,
    /* [out] */ IComponentName** activityCls)
{
    VALIDATE_NOT_NULL(activityCls);
    AutoLock lock(this);

    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r != NULL) {
        return r->mIntent->GetComponent(activityCls);
    }

    *activityCls = NULL;
    return NOERROR;
}

ECode CActivityManagerService::GetPackageForToken(
    /* [in] */ IBinder* token,
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = NULL;

    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r != NULL) {
        *packageName = r->mPackageName;
    }
    return NOERROR;
}

ECode CActivityManagerService::GetIntentSender(
    /* [in] */ Int32 type,
    /* [in] */ const String& packageName,
    /* [in] */ IBinder* token,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ IIIntentSender** intentSender)
{
    VALIDATE_NOT_NULL(intentSender);
    *intentSender = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getIntentSender")));

    Boolean bval;
    // Refuse possible leaked file descriptors
    if (intents != NULL) {
        Int32 length = intents->GetLength();
        if (length < 1) {
            Logger::E(TAG, "Intents array length must be >= 1");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IIntent> intent = (*intents)[i];
            if (intent != NULL) {
                if (intent->HasFileDescriptors(&bval), bval) {
                    Logger::E(TAG, "File descriptors passed in Intent");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                Int32 flags;
                intent->GetFlags(&flags);
                if (type == IActivityManager::INTENT_SENDER_BROADCAST &&
                    (flags & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) != 0) {
                    Logger::E(TAG, "Can't use FLAG_RECEIVER_BOOT_UPGRADE here");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                AutoPtr<IIntent> tmp;
                CIntent::New(intent, (IIntent**)&tmp);
                intents->Set(i, tmp);
            }
        }

        if (resolvedTypes != NULL && resolvedTypes->GetLength() != length) {
            Logger::E(TAG, "Intent array length does not match resolvedTypes length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (options != NULL) {
        if (options->HasFileDescriptors(&bval), bval) {
            Logger::E(TAG, "File descriptors passed in options");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    {
        AutoLock lock(this);
        Int32 callingUid = Binder::GetCallingUid();
        Int32 origUserId = userId;
        Int32 result;
        userId = HandleIncomingUser(Binder::GetCallingPid(), callingUid, userId,
                type == IActivityManager::INTENT_SENDER_BROADCAST, ALLOW_NON_FULL,
                String("getIntentSender"), String(NULL), &result);
        if (origUserId == IUserHandle::USER_CURRENT) {
            // We don't want to evaluate this until the pending intent is
            // actually executed.  However, we do want to always do the
            // security checking for it above.
            userId = IUserHandle::USER_CURRENT;
        }
        // try {
        if (callingUid != 0 && callingUid != IProcess::SYSTEM_UID) {
            AutoPtr<IIPackageManager> pkgMgr = AppGlobals::GetPackageManager();
            Int32 uid;
            pkgMgr->GetPackageUid(packageName, UserHandle::GetUserId(callingUid), &uid);
            Boolean isSameApp = UserHandle::IsSameApp(callingUid, uid);
            if (!isSameApp) {
                StringBuilder msg;
                msg += "Permission Denial: getIntentSender() from pid=";
                msg += Binder::GetCallingPid();
                msg += ", uid=";
                msg += Binder::GetCallingUid();
                msg += ", (need uid=";
                msg += uid;
                msg += ")";
                msg += " is not allowed to send as package ";
                msg += packageName;
                Slogger::W(TAG, msg.ToString());
                return E_SECURITY_EXCEPTION;
            }
        }

        return GetIntentSenderLocked(type, packageName, callingUid, userId,
                token, resultWho, requestCode, intents, resolvedTypes, flags, options, intentSender);
    }
}

ECode CActivityManagerService::GetIntentSenderLocked(
    /* [in] */ Int32 type,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ IBinder* token,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options,
    /* [out] */ IIIntentSender** sender)
{
    VALIDATE_NOT_NULL(sender);
    *sender = NULL;

    if (DEBUG_MU)
        Slogger::V(TAG_MU, "getIntentSenderLocked(): uid=%d, packageName: %s",
            callingUid, packageName.string());

    AutoPtr<ActivityRecord> activity;
    if (type == IActivityManager::INTENT_SENDER_ACTIVITY_RESULT) {
        activity = ActivityRecord::IsInStackLocked(token);
        if (activity == NULL) {
            return NOERROR;
        }
        if (activity->mFinishing) {
            return NOERROR;
        }
    }

    Boolean noCreate = (flags & IPendingIntent::FLAG_NO_CREATE) != 0;
    Boolean cancelCurrent = (flags & IPendingIntent::FLAG_CANCEL_CURRENT) != 0;
    Boolean updateCurrent = (flags & IPendingIntent::FLAG_UPDATE_CURRENT) != 0;
    flags &= ~(IPendingIntent::FLAG_NO_CREATE | IPendingIntent::FLAG_CANCEL_CURRENT
            | IPendingIntent::FLAG_UPDATE_CURRENT);

    AutoPtr<CPendingIntentRecord::Key> key = new CPendingIntentRecord::Key(type, packageName, activity, resultWho,
            requestCode, intents, resolvedTypes, flags, options, userId);
    typename PendingIntentRecordHashMap::Iterator it = mIntentSenderRecords.Find(key);
    AutoPtr<IWeakReference> ref = (it != mIntentSenderRecords.End()) ? it->mSecond : NULL;
    AutoPtr<IIIntentSender> _rec;
    if (ref != NULL) ref->Resolve(EIID_IIIntentSender, (IInterface**)&_rec);
    AutoPtr<CPendingIntentRecord> rec = _rec != NULL ? (CPendingIntentRecord*)_rec.Get() : NULL;
    if (rec != NULL) {
        if (!cancelCurrent) {
            if (updateCurrent) {
                if (rec->mKey->mRequestIntent != NULL) {
                    rec->mKey->mRequestIntent->ReplaceExtras(intents != NULL ?
                                (*intents)[intents->GetLength() - 1] : NULL);
                }
                if (intents != NULL) {
                    intents->Set(intents->GetLength() - 1, rec->mKey->mRequestIntent);
                    rec->mKey->mAllIntents = intents;
                    rec->mKey->mAllResolvedTypes = resolvedTypes;
                }
                else {
                    rec->mKey->mAllIntents = NULL;
                    rec->mKey->mAllResolvedTypes = NULL;
                }
            }
            *sender = rec;
            REFCOUNT_ADD(*sender);
            return NOERROR;
        }
        rec->mCanceled = TRUE;
        mIntentSenderRecords.Erase(key);
    }
    if (noCreate) {
        *sender = rec;
        REFCOUNT_ADD(*sender);
        return NOERROR;
    }

    rec = NULL;
    CPendingIntentRecord::NewByFriend(this, (Handle32)key.Get(), callingUid, (CPendingIntentRecord**)&rec);
    mIntentSenderRecords[key] = rec->mRef;
    if (type == IActivityManager::INTENT_SENDER_ACTIVITY_RESULT) {
        if (activity->mPendingResults == NULL) {
            activity->mPendingResults = new HashSet< AutoPtr<IWeakReference> >(9);
        }
        activity->mPendingResults->Insert(rec->mRef);
    }
    *sender = rec;
    REFCOUNT_ADD(*sender);
    return NOERROR;
}

ECode CActivityManagerService::CancelIntentSender(
    /* [in] */ IIIntentSender* sender)
{
    if (IPendingIntentRecord::Probe(sender) == NULL) {
        return NOERROR;
    }

    {
        AutoLock lock(this);
        CPendingIntentRecord* rec = (CPendingIntentRecord*)sender;
        // try {
        AutoPtr<IIPackageManager> pm =  AppGlobals::GetPackageManager();
        Int32 uid;
        pm->GetPackageUid(rec->mKey->mPackageName, UserHandle::GetCallingUserId(), &uid);
        Boolean isSameApp = UserHandle::IsSameApp(uid, Binder::GetCallingUid());
        if (!isSameApp) {
            Slogger::W(TAG, "Permission Denial: cancelIntentSender() from pid=%d, uid=%d is not allowed to cancel packages %s",
                Binder::GetCallingPid(), Binder::GetCallingUid(),
                rec->mKey->mPackageName.string());
           // throw new SecurityException(msg);
           return E_SECURITY_EXCEPTION;
       }
       // } catch (RemoteException e) {
       //     throw new SecurityException(e);
       // }

       CancelIntentSenderLocked(sender, TRUE);
    }
    return NOERROR;
}

ECode CActivityManagerService::CancelIntentSenderLocked(
    /* [in] */ IIIntentSender* pir,
    /* [in] */ Boolean cleanActivity)
{
    VALIDATE_NOT_NULL(pir);

    CPendingIntentRecord* rec = (CPendingIntentRecord*)pir;
    rec->mCanceled = TRUE;
    mIntentSenderRecords.Erase(rec->mKey);
    if (cleanActivity && rec->mKey->mActivity != NULL) {
        rec->mKey->mActivity->mPendingResults->Erase(rec->mRef);
    }
    return NOERROR;
}

ECode CActivityManagerService::GetPackageForIntentSender(
    /* [in] */ IIIntentSender* pendingResult,
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = NULL;

    if (IPendingIntentRecord::Probe(pendingResult) == NULL) {
        return NOERROR;
    }

   // try {
   CPendingIntentRecord* res = (CPendingIntentRecord*)pendingResult;
   *packageName = res->mKey->mPackageName;
   // } catch (ClassCastException e) {
   // }
    return NOERROR;
}

ECode CActivityManagerService::GetUidForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = -1;

    if (IPendingIntentRecord::Probe(sender) == NULL) {
        return NOERROR;
    }

    // try {
    CPendingIntentRecord* res = (CPendingIntentRecord*)sender;
    *uid = res->mUid;
    // } catch (ClassCastException e) {
    // }

    return NOERROR;
}

ECode CActivityManagerService::IsIntentSenderTargetedToPackage(
    /* [in] */ IIIntentSender* pendingResult,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (IPendingIntentRecord::Probe(pendingResult) == NULL) {
        return NOERROR;
    }

    // try {
    CPendingIntentRecord* res = (CPendingIntentRecord*)pendingResult;
    if (res->mKey->mAllIntents == NULL) {
        return NOERROR;
    }

    String pkgName;
    for (Int32 i = 0; i < res->mKey->mAllIntents->GetLength(); i++) {
        AutoPtr<IIntent> intent = (*res->mKey->mAllIntents)[i];
        if (intent != NULL) {
            intent->GetPackage(&pkgName);
            AutoPtr<IComponentName> compName;
            intent->GetComponent((IComponentName**)&compName);
            if (!pkgName.IsNull() && compName != NULL) {
                return NOERROR;
            }
        }
    }

    *result = TRUE;
    // } catch (ClassCastException e) {
    // }

    return NOERROR;
}

ECode CActivityManagerService::IsIntentSenderAnActivity(
    /* [in] */ IIIntentSender* pendingResult,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (IPendingIntentRecord::Probe(pendingResult) == NULL) {
        return NOERROR;
    }

    // try {
    CPendingIntentRecord* res = (CPendingIntentRecord*)pendingResult;
    if (res->mKey->mType == IActivityManager::INTENT_SENDER_ACTIVITY) {
        *result = TRUE;
    }
    // } catch (ClassCastException e) {
    // }

    return NOERROR;
}

ECode CActivityManagerService::GetIntentForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;

    if (IPendingIntentRecord::Probe(sender) == NULL) {
        return NOERROR;
    }

//     try {
    CPendingIntentRecord* res = (CPendingIntentRecord*)sender;
    if (res->mKey->mRequestIntent == NULL) {
        return NOERROR;
    }

    return CIntent::New(res->mKey->mRequestIntent, intent);
//     } catch (ClassCastException e) {
//     }
//     return null;
}

ECode CActivityManagerService::GetTagForIntentSender(
    /* [in] */ IIIntentSender* pendingResult,
    /* [in] */ const String& prefix,
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = NULL;

    if (IPendingIntentRecord::Probe(pendingResult) == NULL) {
        return NOERROR;
    }
    // try {
        CPendingIntentRecord* res = (CPendingIntentRecord*)pendingResult;
        AutoPtr<IIntent> intent = res->mKey->mRequestIntent;
        if (intent != NULL) {
            if (res->mLastTag != NULL && res->mLastTagPrefix == prefix && (res->mLastTagPrefix == NULL
                || res->mLastTagPrefix.Equals(prefix))) {
                *tag = res->mLastTag;
                return NOERROR;
            }
            res->mLastTagPrefix = prefix;
            StringBuilder sb(128);
            if (prefix != NULL) {
                sb.Append(prefix);
            }
            String action;
            intent->GetAction(&action);
            AutoPtr<IComponentName> component;
            if (action != NULL) {
                sb.Append(action);
            }
            else if (intent->GetComponent((IComponentName**)&component), component != NULL) {
                component->AppendShortString(&sb);
            }
            else {
                sb.Append("?");
            }
            *tag = res->mLastTag = sb.ToString();
            return NOERROR;
        }
    // } catch (ClassCastException e) {
    // }
    return NOERROR;
}

ECode CActivityManagerService::SetProcessLimit(
    /* [in] */ Int32 max)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_PROCESS_LIMIT,
            String("setProcessLimit()")));
    {
        AutoLock lock(this);
        mProcessLimit = max < 0 ? ProcessList::MAX_CACHED_APPS : max;
        mProcessLimitOverride = max;
    }
    TrimApplications();
    return NOERROR;
}

ECode CActivityManagerService::GetProcessLimit(
    /* [out] */ Int32* limit)
{
    VALIDATE_NOT_NULL(limit);

    AutoLock lock(this);
    *limit = mProcessLimitOverride;
    return NOERROR;
}

ECode CActivityManagerService::ForegroundTokenDied(
    /* [in] */ ForegroundToken* token)
{
    AutoLock lock(this);
    {
        AutoLock lock(mPidsSelfLockedLock);
        AutoPtr<ForegroundToken> cur
            = mForegroundProcesses[token->mPid];
        if (cur.Get() != token) {
            return NOERROR;
        }
        mForegroundProcesses.Erase(token->mPid);
        AutoPtr<ProcessRecord> pr = mPidsSelfLocked[token->mPid];
        if (pr == NULL) {
            return NOERROR;
        }
        pr->mForcingToForeground = NULL;
        UpdateProcessForegroundLocked(pr, FALSE, FALSE);
    }
    UpdateOomAdjLocked();
    return NOERROR;
}

ECode CActivityManagerService::SetProcessForeground(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 pid,
    /* [in] */ Boolean isForeground)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_PROCESS_LIMIT,
            String("setProcessForeground()")));
    {
        AutoLock lock(this);
        Boolean changed = FALSE;

    {
        AutoLock lock(mPidsSelfLockedLock);
        AutoPtr<ProcessRecord> pr = mPidsSelfLocked[pid];
            if (pr == NULL && isForeground) {
                Slogger::W(TAG, "setProcessForeground called on unknown pid: %d", pid);
                return NOERROR;
            }
            AutoPtr<ForegroundToken> oldToken = mForegroundProcesses[pid];
            if (oldToken != NULL) {
                AutoPtr< ::IProxy> proxy = (::IProxy*)oldToken->mToken->Probe(EIID_IProxy);
                if (proxy) {
                    Boolean result;
                    proxy->UnlinkToDeath(oldToken, 0, &result);
                }
                mForegroundProcesses.Erase(pid);
                if (pr != NULL) {
                    pr->mForcingToForeground = NULL;
                }
                changed = TRUE;
            }
            if (isForeground && token != NULL) {
                AutoPtr<ForegroundToken> newToken = new SetProcessForegroundToken(this);
                newToken->mPid = pid;
                newToken->mToken = token;
                // try {
                AutoPtr< ::IProxy> proxy = (::IProxy*)token->Probe(EIID_IProxy);
                if (proxy) {
                    proxy->LinkToDeath(newToken, 0);
                }
                mForegroundProcesses[pid] = newToken;
                pr->mForcingToForeground = token;
                changed = TRUE;
                // } catch (RemoteException e) {
                //     // If the process died while doing this, we will later
                //     // do the cleanup with the process death link.
                // }
            }
        }

        if (changed) {
            UpdateOomAdjLocked();
        }
    }
    return NOERROR;
}

//=========================================================
// PERMISSIONS
//=========================================================

Int32 CActivityManagerService::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported)
{
    // We might be performing an operation on behalf of an indirect binder
    // invocation, e.g. via {@link #openContentUri}.  Check and adjust the
    // client identity accordingly before proceeding.
    AutoPtr<Identity> tlsIdentity = (Identity*)pthread_getspecific(sCallerIdentity);
    if (tlsIdentity != NULL) {
        Slogger::D(TAG, "  adjusting {pid,uid} to {%d, %d}", tlsIdentity->mPid, tlsIdentity->mUid);

        uid = tlsIdentity->mUid;
        pid = tlsIdentity->mPid;
    }

    if (pid == MY_PID) {
        return IPackageManager::PERMISSION_GRANTED;
    }

    AutoPtr<IActivityManagerHelper> amhelper;
    Int32 intPermission = IPackageManager::PERMISSION_GRANTED;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
    amhelper->CheckComponentPermission(permission, uid, owningUid, exported, &intPermission);
    return intPermission;
}

ECode CActivityManagerService::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (permission.IsNull()) {
        *result = IPackageManager::PERMISSION_DENIED;
    }
    else {
        *result = CheckComponentPermission(permission, pid, UserHandle::GetAppId(uid), -1, TRUE);
    }

    return NOERROR;
}

Int32 CActivityManagerService::CheckCallingPermission(
    /* [in] */ const String& permission)
{
    Int32 result;
    CheckPermission(permission, Binder::GetCallingPid(),
        UserHandle::GetAppId(Binder::GetCallingUid()), &result);
    return result;
}

ECode CActivityManagerService::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& func)
{
    if (CheckCallingPermission(permission) == IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }

    Slogger::W(TAG, "Permission Denial: %s from pid=%d, uid=%d requires %s",
        func.string(), Binder::GetCallingPid(), Binder::GetCallingUid(),
        permission.string());
    return E_SECURITY_EXCEPTION;
}

Boolean CActivityManagerService::CheckHoldingPermissionsLocked(
    /* [in] */ IIPackageManager* pm,
    /* [in] */ IProviderInfo* pi,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags)
{
    if (DEBUG_URI_PERMISSION) {
        Slogger::V(TAG, "checkHoldingPermissionsLocked: uri=%s uid=%d", TO_CSTR(grantUri), uid);
    }
    if (UserHandle::GetUserId(uid) != grantUri->mSourceUserId) {
        AutoPtr<IActivityManagerHelper> amhelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
        Int32 result;
        amhelper->CheckComponentPermission(Manifest::permission::INTERACT_ACROSS_USERS,
            uid, -1, TRUE, &result);
        if (result != IPackageManager::PERMISSION_GRANTED) {
            return FALSE;
        }
    }
    return CheckHoldingPermissionsInternalLocked(pm, pi, grantUri, uid, modeFlags, TRUE);
}

Boolean CActivityManagerService::CheckHoldingPermissionsInternalLocked(
    /* [in] */ IIPackageManager* pm,
    /* [in] */ IProviderInfo* pi,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Boolean considerUidPermissions)
{
    AutoPtr<IApplicationInfo> piAppInfo;
    IComponentInfo::Probe(pi)->GetApplicationInfo((IApplicationInfo**)&piAppInfo);
    Int32 piUid;
    piAppInfo->GetUid(&piUid);
    Boolean piExported;
    IComponentInfo::Probe(pi)->GetExported(&piExported);
    if (piUid == uid) {
        return TRUE;
    }
    else if (!piExported) {
        return FALSE;
    }

    Boolean readMet = (modeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) == 0;
    Boolean writeMet = (modeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) == 0;
    // try {
    // check if target holds top-level <provider> permissions
    String piReadPermission;
    pi->GetReadPermission(&piReadPermission);
    Int32 perm;
    ASSERT_SUCCEEDED(pm->CheckUidPermission(piReadPermission, uid, &perm));
    if (!readMet && !piReadPermission.IsNull() && considerUidPermissions
        && perm == IPackageManager::PERMISSION_GRANTED) {
        readMet = TRUE;
    }
    String piWritePermission;
    pi->GetWritePermission(&piWritePermission);
    Int32 perm2;
    ASSERT_SUCCEEDED(pm->CheckUidPermission(piWritePermission, uid, &perm2));
    if (!writeMet && !piWritePermission.IsNull() && considerUidPermissions
        && perm2 == IPackageManager::PERMISSION_GRANTED) {
        writeMet = TRUE;
    }

    // track if unprotected read/write is allowed; any denied
    // <path-permission> below removes this ability
    Boolean allowDefaultRead = piReadPermission == NULL;
    Boolean allowDefaultWrite = piWritePermission == NULL;

    // check if target holds any <path-permission> that match uri
    AutoPtr<ArrayOf<IPathPermission*> > pps;
    pi->GetPathPermissions((ArrayOf<IPathPermission*>**)&pps);
    if (pps != NULL) {
        String path;
        grantUri->mUri->GetPath(&path);
        Int32 i = pps->GetLength();
        while (i > 0 && (!readMet || !writeMet)) {
            i--;
            AutoPtr<IPathPermission> pp = (*pps)[i];
            Boolean isMatch;
            IPatternMatcher::Probe(pp)->Match(path, &isMatch);
            if (isMatch) {
                if (!readMet) {
                    String pprperm;
                    pp->GetReadPermission(&pprperm);
                    Int32 perm;
                    ASSERT_SUCCEEDED(pm->CheckUidPermission(pprperm, uid, &perm));

                    if (DEBUG_URI_PERMISSION) {
                        String pathLog;
                        IPatternMatcher::Probe(pp)->GetPath(&pathLog);
                        Slogger::V(TAG, "Checking read perm for %s for %s: match=%d check=%d", pprperm.string(), pathLog.string(),
                                isMatch, perm);
                    }
                    if (!pprperm.IsNull()) {
                        if (considerUidPermissions && perm == IPackageManager::PERMISSION_GRANTED) {
                            readMet = TRUE;
                        }
                        else {
                            allowDefaultRead = FALSE;
                        }
                    }
                }
                if (!writeMet) {
                    String ppwperm;
                    pp->GetWritePermission(&ppwperm);
                    Int32 perm;
                    ASSERT_SUCCEEDED(pm->CheckUidPermission(ppwperm, uid, &perm));
                    Boolean isMatch;
                    IPatternMatcher::Probe(pp)->Match(path, &isMatch);
                    if (DEBUG_URI_PERMISSION) {
                        String pathLog;
                        IPatternMatcher::Probe(pp)->GetPath(&pathLog);
                        Slogger::V(TAG, "Checking write perm %s for %s: match=%d check=%d",
                            ppwperm.string(), pathLog.string(), isMatch, perm);
                    }
                    if (!ppwperm.IsNull()) {
                        if (considerUidPermissions && perm == IPackageManager::PERMISSION_GRANTED) {
                            writeMet = TRUE;
                        }
                        else {
                            allowDefaultWrite = FALSE;
                        }
                    }
                }
            }
        }
    }

    // grant unprotected <provider> read/write, if not blocked by
    // <path-permission> above
    if (allowDefaultRead) readMet = TRUE;
    if (allowDefaultWrite) writeMet = TRUE;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }

    return readMet && writeMet;
}

AutoPtr<IProviderInfo> CActivityManagerService::GetProviderInfoLocked(
    /* [in] */ const String& authority,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IProviderInfo> pi;
    AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByName(authority, userHandle);
    if (cpr != NULL) {
        pi = cpr->mInfo;
    }
    else {
        AppGlobals::GetPackageManager()->ResolveContentProvider(
            authority, IPackageManager::GET_URI_PERMISSION_PATTERNS, userHandle,
            (IProviderInfo**)&pi);
    }
    return pi;
}

AutoPtr<UriPermission> CActivityManagerService::FindUriPermissionLocked(
    /* [in] */ Int32 targetUid,
    /* [in] */ GrantUri* grantUri)
{
    GrantUriPermissionMapMap::Iterator find = mGrantedUriPermissions.Find(targetUid);
    AutoPtr<GrantUriPermissionMap> targetUris;
    if (find != mGrantedUriPermissions.End())
        targetUris = find->mSecond;
    if (targetUris != NULL) {
        GrantUriPermissionMap::Iterator it = targetUris->Find(grantUri);
        if (it != targetUris->End())
            return it->mSecond;
    }
    return NULL;
}

AutoPtr<UriPermission> CActivityManagerService::FindOrCreateUriPermissionLocked(
    /* [in] */ const String& sourcePkg,
    /* [in] */ const String& targetPkg,
    /* [in] */ Int32 targetUid,
    /* [in] */ GrantUri* grantUri)
{
    AutoPtr<GrantUriPermissionMap> targetUris = mGrantedUriPermissions[targetUid];
    if (targetUris == NULL) {
        targetUris = new GrantUriPermissionMap;
        mGrantedUriPermissions[targetUid] = targetUris;
    }

    AutoPtr<UriPermission> perm = (*targetUris)[grantUri];
    if (perm == NULL) {
        perm = new UriPermission(sourcePkg, targetPkg, targetUid, grantUri);
        (*targetUris)[grantUri] = perm;
    }

    return perm;
}

Boolean CActivityManagerService::CheckUriPermissionLocked(
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags)
{
    Boolean persistable = (modeFlags & IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION) != 0;
    Int32 minStrength = persistable ? UriPermission::STRENGTH_PERSISTABLE
            : UriPermission::STRENGTH_OWNED;

    // Root gets to do everything.
    if (uid == 0) {
        return TRUE;
    }

    GrantUriPermissionMapMap::Iterator it = mGrantedUriPermissions.Find(uid);
    AutoPtr<GrantUriPermissionMap> perms;
    if (it != mGrantedUriPermissions.End())
        perms = it->mSecond;
    if (perms == NULL)
        return FALSE;

    AutoPtr<UriPermission> exactPerm;
    GrantUriPermissionMap::Iterator it2 = perms->Find(grantUri);
    if (it2 != perms->End())
        exactPerm = it2->mSecond;
    if (exactPerm != NULL && exactPerm->GetStrength(modeFlags) >= minStrength) {
        return TRUE;
    }

    // No exact match, look for prefixes
    it2 = perms->Begin();
    for (; it2 != perms->End(); ++it2) {
        AutoPtr<UriPermission> perm = it2->mSecond;
        Boolean res;
        if (perm->mUri->mPrefix && (grantUri->mUri->IsPathPrefixMatch(perm->mUri->mUri, &res), res)
                && perm->GetStrength(modeFlags) >= minStrength) {
            return TRUE;
        }
    }

    return FALSE;
}

ECode CActivityManagerService::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* permission)
{
    VALIDATE_NOT_NULL(permission);
    *permission = IPackageManager::PERMISSION_DENIED;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("checkUriPermission")));

   // Another redirected-binder-call permissions check as in
   // {@link checkComponentPermission}.
   AutoPtr<Identity> tlsIdentity = (Identity*)pthread_getspecific(sCallerIdentity);

   if (tlsIdentity != NULL) {
      uid = tlsIdentity->mUid;
      pid = tlsIdentity->mPid;
   }

   // Our own process gets to do everything.
   if (pid == MY_PID) {
       *permission = IPackageManager::PERMISSION_GRANTED;
       return NOERROR;
   }

   {
       AutoLock lock(this);
       AutoPtr<GrantUri> guri = new GrantUri(userId, uri, FALSE);
       *permission = CheckUriPermissionLocked(guri, uid, modeFlags)
              ? IPackageManager::PERMISSION_GRANTED
              : IPackageManager::PERMISSION_DENIED;
        return NOERROR;
   }
}

ECode CActivityManagerService::CheckGrantUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 lastTargetUid,
    /* [out] */ Int32* permission)
{
    VALIDATE_NOT_NULL(permission);
    *permission = IPackageManager::PERMISSION_DENIED;
    AutoPtr<IIntentHelper> iHelper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
    Boolean res;
    iHelper->IsAccessUriMode(modeFlags, &res);
    if (!res) {
        *permission = -1;
        return NOERROR;
    }

    if (!targetPkg.IsNull()) {
        if (DEBUG_URI_PERMISSION) {
            Slogger::V(TAG, "Checking grant %s permission to %s", targetPkg.string(), TO_CSTR(grantUri));
        }
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    // If this is not a content: uri, we can't do anything with it.
    String scheme;
    grantUri->mUri->GetScheme(&scheme);
    if (!scheme.Equals(IContentResolver::SCHEME_CONTENT)) {
        if (DEBUG_URI_PERMISSION) {
           Slogger::V(TAG, "Can't grant URI permission for non-content URI: %s", TO_CSTR(grantUri));
        }
        *permission = -1;
        return NOERROR;
    }

    String authority;
    grantUri->mUri->GetAuthority(&authority);
    AutoPtr<IProviderInfo> pi = GetProviderInfoLocked(authority, grantUri->mSourceUserId);
    if (pi == NULL) {
        String uriString;
        grantUri->mUri->ToSafeString(&uriString);
        Slogger::W(TAG, "No content provider found for permission check: %s", uriString.string());
        *permission = -1;
        return NOERROR;
    }

    Int32 targetUid = lastTargetUid;
    if (targetUid < 0 && !targetPkg.IsNull()) {
        if (SUCCEEDED(pm->GetPackageUid(targetPkg, UserHandle::GetUserId(callingUid), &targetUid))) {
            if (targetUid < 0) {
                if (DEBUG_URI_PERMISSION) {
                    Slogger::V(TAG, "Can't grant URI permission no uid for: %s", targetPkg.string());
                    *permission = -1;
                    return NOERROR;
                }
            }
        }
        else {
            *permission = -1;
            return NOERROR;
        }
    }

    if (targetUid >= 0) {
        // First...  does the target actually need this permission?
        if (CheckHoldingPermissionsLocked(pm, pi, grantUri, targetUid, modeFlags)) {
            // No need to grant the target this permission.
            if (DEBUG_URI_PERMISSION) {
                Slogger::V(TAG, "Target %s already has full permission to %s", targetPkg.string(), TO_CSTR(grantUri));
            }
            *permission = -1;
            return NOERROR;
        }
    }
    else {
        // First...  there is no target package, so can anyone access it?
        Boolean allowed;
        IComponentInfo::Probe(pi)->GetExported(&allowed);
        if ((modeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
            String piReadPermission;
            pi->GetReadPermission(&piReadPermission);
            if (!piReadPermission.IsNull()) {
                allowed = FALSE;
            }
        }
        if ((modeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
            String piWritePermission;
            pi->GetWritePermission(&piWritePermission);
            if (!piWritePermission.IsNull()) {
                allowed = FALSE;
            }
        }
        if (allowed) {
            *permission = -1;
            return NOERROR;
        }
    }

    /* There is a special cross user grant if:
     * - The target is on another user.
     * - Apps on the current user can access the uri without any uid permissions.
     * In this case, we grant a uri permission, even if the ContentProvider does not normally
     * grant uri permissions.
     */
    Boolean specialCrossUserGrant = UserHandle::GetUserId(targetUid) != grantUri->mSourceUserId
            && CheckHoldingPermissionsInternalLocked(pm, pi, grantUri, callingUid,
            modeFlags, FALSE /*without considering the uid permissions*/);


    // Second...  is the provider allowing granting of URI permissions?
    if (!specialCrossUserGrant) {
        Boolean grant;
        pi->GetGrantUriPermissions(&grant);
        if (!grant) {
            String piCName, piName;
            IPackageItemInfo::Probe(pi)->GetPackageName(&piCName);
            IPackageItemInfo::Probe(pi)->GetName(&piName);
            Slogger::E(TAG, "Provider %s/%s does not allow granting of Uri permissions (uri %s)",
                    piCName.string(), piName.string(), TO_CSTR(grantUri));
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<ArrayOf<IPatternMatcher*> > patterns;
        pi->GetUriPermissionPatterns((ArrayOf<IPatternMatcher*>**)&patterns);
        if (patterns != NULL) {
            const Int32 N = patterns->GetLength();
            Boolean allowed = FALSE;
            for (Int32 i=0; i<N; i++) {
                String uriPath;
                grantUri->mUri->GetPath(&uriPath);
                Boolean matched;
                if ((*patterns)[i] != NULL && ((*patterns)[i]->Match(uriPath, &matched), matched)) {
                    allowed = TRUE;
                    break;
                }
            }
            if (!allowed) {
                String piCName, piName;
                IPackageItemInfo::Probe(pi)->GetPackageName(&piCName);
                IPackageItemInfo::Probe(pi)->GetName(&piName);
                Slogger::E(TAG, "Provider %s/%s does not allow granting of permission to path of Uri %s",
                    piCName.string(), piName.string(), TO_CSTR(grantUri));
                return E_SECURITY_EXCEPTION;
            }
        }
    }

    // Third...  does the caller itself have permission to access
    // this uri?
    if (UserHandle::GetAppId(callingUid) != IProcess::SYSTEM_UID) {
        if (!CheckHoldingPermissionsLocked(pm, pi, grantUri, callingUid, modeFlags)) {
            if (!CheckUriPermissionLocked(grantUri, callingUid, modeFlags)) {
               Slogger::E(TAG, "Uid %d does not have permission to uri %s",
                    callingUid, TO_CSTR(grantUri));
                return E_SECURITY_EXCEPTION;
            }
        }
    }
    *permission = targetUid;
    return NOERROR;
}

ECode CActivityManagerService::CheckGrantUriPermission(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* granted)
{
    VALIDATE_NOT_NULL(granted);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("checkGrantUriPermission")));
    {
        AutoLock lock(this);
        AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);
        return CheckGrantUriPermissionLocked(callingUid, targetPkg, grantUri, modeFlags, -1, granted);
    }
}

ECode CActivityManagerService::GrantUriPermissionUncheckedLocked(
    /* [in] */ Int32 targetUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ UriPermissionOwner* owner)
{
    AutoPtr<IIntentHelper> iHelper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
    Boolean res;
    iHelper->IsAccessUriMode(modeFlags, &res);
    if (!res) {
        return NOERROR;
    }

    // So here we are: the caller has the assumed permission
    // to the uri, and the target doesn't.  Let's now give this to
    // the target.

    if (DEBUG_URI_PERMISSION) {
        Slogger::V(TAG, "Granting %s/%d permission to %s", targetPkg.string(),
            targetUid, TO_CSTR(grantUri));
    }

    String authority;
    grantUri->mUri->GetAuthority(&authority);
    AutoPtr<IProviderInfo> pi = GetProviderInfoLocked(authority, grantUri->mSourceUserId);
    if (pi == NULL) {
        Slogger::W(TAG, "No content provider found for grant: %s", grantUri->ToSafeString().string());
        return NOERROR;
    }

    if ((modeFlags & IIntent::FLAG_GRANT_PREFIX_URI_PERMISSION) != 0) {
        grantUri->mPrefix = TRUE;
    }
    String packageName;
    IPackageItemInfo::Probe(pi)->GetPackageName(&packageName);
    AutoPtr<UriPermission> perm = FindOrCreateUriPermissionLocked(
            packageName, targetPkg, targetUid, grantUri);
    perm->GrantModes(modeFlags, owner);
    return NOERROR;
}

ECode CActivityManagerService::GrantUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ UriPermissionOwner* owner,
    /* [in] */ Int32 targetUserId)
{
    if (targetPkg.IsNull()) {
       // throw new NullPointerException("targetPkg");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 targetUid;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    if (FAILED(pm->GetPackageUid(targetPkg, targetUserId, &targetUid))) {
        return NOERROR;
    }

    CheckGrantUriPermissionLocked(callingUid, targetPkg, grantUri, modeFlags,
        targetUid, &targetUid);
    if (targetUid < 0) {
        return NOERROR;
    }

    GrantUriPermissionUncheckedLocked(targetUid, targetPkg, grantUri, modeFlags, owner);
    return NOERROR;
}

AutoPtr<NeededUriGrants>
CActivityManagerService::CheckGrantUriPermissionFromIntentLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 mode,
    /* [in] */ NeededUriGrants* needed,
    /* [in] */ Int32 targetUserId)
{
    if (DEBUG_URI_PERMISSION) {
        StringBuilder sb("Checking URI perm to data ");
        AutoPtr<IUri> uri;
        AutoPtr<IClipData> clipData;
        Int32 flags;
        if (intent != NULL) {
            intent->GetFlags(&flags);
            intent->GetData((IUri**)&uri);
            intent->GetClipData((IClipData**)&clipData);
        }
        sb += uri;
        sb += " clip=";
        sb += clipData;
        sb += " from ";
        sb += intent;
        sb += "; flags=0x";
        sb += StringUtils::ToString(flags, 16);
        Slogger::V(TAG, sb.ToString().string());
    }

    if (targetPkg.IsNull()) {
        // throw new NullPointerException("targetPkg");
        return NULL;
    }

    if (intent == NULL) {
        return NULL;
    }
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    AutoPtr<IClipData> clip;
    intent->GetClipData((IClipData**)&clip);
    if (data == NULL && clip == NULL) {
        return NULL;
    }
    // Default userId for uris in the intent (if they don't specify it themselves)
    Int32 contentUserHint;
    intent->GetContentUserHint(&contentUserHint);
    if (contentUserHint == IUserHandle::USER_CURRENT) {
        contentUserHint = UserHandle::GetUserId(callingUid);
    }
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 targetUid;
    if (needed != NULL) {
        targetUid = needed->mTargetUid;
    }
    else {
        if (FAILED(pm->GetPackageUid(targetPkg, targetUserId, &targetUid))) {
            return NULL;
        }
        if (targetUid < 0) {
            if (DEBUG_URI_PERMISSION) {
                Slogger::V(TAG, "Can't grant URI permission no uid for: %s on user %d",
                    targetPkg.string(), targetUserId);
            }
            return NULL;
        }
    }
    if (data != NULL) {
        AutoPtr<GrantUri> grantUri = GrantUri::Resolve(contentUserHint, data);
        CheckGrantUriPermissionLocked(callingUid, targetPkg, grantUri,
                mode, targetUid, &targetUid);
        if (targetUid > 0) {
            if (needed == NULL) {
                needed = new NeededUriGrants(targetPkg, targetUid, mode);
            }
            needed->PushBack(grantUri);
        }
    }
    if (clip != NULL) {
        Int32 count;
        clip->GetItemCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IClipDataItem> clipItem;
            clip->GetItemAt(i, (IClipDataItem**)&clipItem);
            AutoPtr<IUri> uri;
            clipItem->GetUri((IUri**)&uri);
            if (uri != NULL) {
                AutoPtr<GrantUri> grantUri = GrantUri::Resolve(contentUserHint, uri);
                CheckGrantUriPermissionLocked(callingUid, targetPkg, grantUri,
                        mode, targetUid, &targetUid);
                if (targetUid > 0) {
                    if (needed == NULL) {
                        needed = new NeededUriGrants(targetPkg, targetUid, mode);
                    }
                    needed->PushBack(grantUri);
                }
            }
            else {
                AutoPtr<IIntent> clipIntent;
                clipItem->GetIntent((IIntent**)&clipIntent);
                if (clipIntent != NULL) {
                    AutoPtr<NeededUriGrants> newNeeded = CheckGrantUriPermissionFromIntentLocked(
                            callingUid, targetPkg, clipIntent, mode, needed, targetUserId);
                    if (newNeeded != NULL) {
                        needed = newNeeded;
                    }
                }
            }
        }
    }

   return needed;
}

ECode CActivityManagerService::GrantUriPermissionUncheckedFromIntentLocked(
    /* [in] */ NeededUriGrants* needed,
    /* [in] */ UriPermissionOwner* owner)
{
    if (needed != NULL) {
        NeededUriGrants::Iterator it;
        for (it = needed->Begin(); it != needed->End(); ++it) {
            GrantUriPermissionUncheckedLocked(needed->mTargetUid, needed->mTargetPkg,
                    *it, needed->mFlags, owner);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GrantUriPermissionFromIntentLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IIntent* intent,
    /* [in] */ UriPermissionOwner* owner,
    /* [in] */ Int32 targetUserId)
{
    Int32 flags;
    AutoPtr<NeededUriGrants> needed = CheckGrantUriPermissionFromIntentLocked(callingUid, targetPkg,
            intent, intent != NULL ? (intent->GetFlags(&flags), flags) : 0, NULL, targetUserId);
    if (needed == NULL) {
       return NOERROR;
    }

    GrantUriPermissionUncheckedFromIntentLocked(needed, owner);
    return NOERROR;
}

ECode CActivityManagerService::GrantUriPermission(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("grantUriPermission")));
    AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);

    AutoLock lock(this);
    AutoPtr<ProcessRecord> r = GetRecordForAppLocked(caller);
    if (r == NULL) {
        Slogger::E(TAG, "Unable to find app for caller %s when granting permission to uri %s",
            TO_CSTR(caller), TO_CSTR(grantUri));
        return E_SECURITY_EXCEPTION;
    }
    if (targetPkg.IsNull()) {
        // throw new IllegalArgumentException("null target");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (uri == NULL) {
        // throw new IllegalArgumentException("null uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    FAIL_RETURN(preconditions->CheckFlagsArgument(modeFlags, IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION
            | IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION
            | IIntent::FLAG_GRANT_PREFIX_URI_PERMISSION));

    GrantUriPermissionLocked(r->mUid, targetPkg, grantUri, modeFlags, NULL,
        UserHandle::GetUserId(r->mUid));
    return NOERROR;
}

ECode CActivityManagerService::RemoveUriPermissionIfNeededLocked(
    /* [in] */ UriPermission* perm)
{
    if (perm->mModeFlags == 0) {
        GrantUriPermissionMapMap::Iterator it = mGrantedUriPermissions.Find(perm->mTargetUid);
        AutoPtr<GrantUriPermissionMap> perms;
        if (it != mGrantedUriPermissions.End())
            perms = it->mSecond;
        if (perms != NULL) {
            if (DEBUG_URI_PERMISSION) {
                Slogger::V(TAG, "Removing %d permission to %s", perm->mTargetUid, TO_CSTR(perm->mUri));
            }
            perms->Erase(perm->mUri);
            if (perms->IsEmpty()) {
                mGrantedUriPermissions.Erase(perm->mTargetUid);
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::RevokeUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ GrantUri* grantUri,
    /* [in] */ Int32 modeFlags)
{
    if (DEBUG_URI_PERMISSION) {
        Slogger::V(TAG, "Revoking all granted permissions to %s", TO_CSTR(grantUri));
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    String authority;
    grantUri->mUri->GetAuthority(&authority);
    AutoPtr<IProviderInfo> pi = GetProviderInfoLocked(authority, grantUri->mSourceUserId);
    if (pi == NULL) {
        Slogger::W(TAG, "No content provider found for permission revoke: %s",
            grantUri->ToSafeString().string());
        return NOERROR;
    }

    // Does the caller have this permission on the URI?
    if (!CheckHoldingPermissionsLocked(pm, pi, grantUri, callingUid, modeFlags)) {
        // If they don't have direct access to the URI, then revoke any
        // ownerless URI permissions that have been granted to them.
        GrantUriPermissionMapMap::Iterator find = mGrantedUriPermissions.Find(callingUid);
        AutoPtr<GrantUriPermissionMap> perms;
        if (find != mGrantedUriPermissions.End())
            perms = find->mSecond;
        if (perms != NULL) {
            Boolean persistChanged = FALSE;
            GrantUriPermissionMap::Iterator it = perms->Begin();
            while (it != perms->End()) {
                AutoPtr<UriPermission> perm = it->mSecond;
                Boolean res;
                if (perm->mUri->mSourceUserId == grantUri->mSourceUserId
                    && (perm->mUri->mUri->IsPathPrefixMatch(grantUri->mUri, &res), res)) {
                    if (DEBUG_URI_PERMISSION)
                        Slogger::V(TAG, "Revoking non-owned %d permission to %s",
                            perm->mTargetUid, TO_CSTR(perm->mUri));
                    persistChanged |= perm->RevokeModes(
                            modeFlags | IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION, FALSE);
                    if (perm->mModeFlags == 0) {
                        perms->Erase(it++);
                        continue;
                    }
                }
                ++it;
            }
            if (perms->IsEmpty()) {
                mGrantedUriPermissions.Erase(callingUid);
            }
            if (persistChanged) {
                SchedulePersistUriGrants();
            }
        }
        return NOERROR;
    }

    Boolean persistChanged = FALSE;

    // Go through all of the permissions and remove any that match.
    GrantUriPermissionMapMap::Iterator mit = mGrantedUriPermissions.Begin();
    while (mit != mGrantedUriPermissions.End()) {
        AutoPtr<GrantUriPermissionMap> perms = mit->mSecond;

        GrantUriPermissionMap::Iterator it = perms->Begin();
        while (it != perms->End()) {
            AutoPtr<UriPermission> perm = it->mSecond;
            Boolean res;
            if (perm->mUri->mSourceUserId == grantUri->mSourceUserId
                && (perm->mUri->mUri->IsPathPrefixMatch(grantUri->mUri, &res), res)) {
                if (DEBUG_URI_PERMISSION)
                    Slogger::V(TAG, "Revoking %d permission to %s",
                        perm->mTargetUid, TO_CSTR(perm->mUri));
                persistChanged |= perm->RevokeModes(
                        modeFlags | IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION, TRUE);
                if (perm->mModeFlags == 0) {
                    perms->Erase(it++);
                    continue;
                }
            }
            ++it;
        }

        if (perms->IsEmpty()) {
            mGrantedUriPermissions.Erase(mit++);
        }
    }

    if (persistChanged) {
        SchedulePersistUriGrants();
    }
    return NOERROR;
}

ECode CActivityManagerService::RevokeUriPermission(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("revokeUriPermission")));
    AutoLock lock(this);

    AutoPtr<ProcessRecord> r = GetRecordForAppLocked(caller);
    if (r == NULL) {
        Slogger::E(TAG, "Unable to find app for caller %s when revoking permission to uri %s",
            TO_CSTR(caller), TO_CSTR(uri));
        return E_SECURITY_EXCEPTION;
    }
    if (uri == NULL) {
        Slogger::W(TAG, "revokeUriPermission: null uri");
        return NOERROR;;
    }

    AutoPtr<IIntentHelper> iHelper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
    Boolean res;
    iHelper->IsAccessUriMode(modeFlags, &res);
    if (!res) {
        return NOERROR;
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    String authority;
    uri->GetAuthority(&authority);
    AutoPtr<IProviderInfo> pi = GetProviderInfoLocked(authority, userId);
    if (pi == NULL) {
        String uriString;
        uri->ToSafeString(&uriString);
        Slogger::W(TAG, "No content provider found for permission revoke: %s",
            uriString.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);
    RevokeUriPermissionLocked(r->mUid, grantUri, modeFlags);

    return NOERROR;
}

ECode CActivityManagerService::RemoveUriPermissionsForPackageLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [in] */ Boolean persistable)
{
    if (userHandle == IUserHandle::USER_ALL && packageName == NULL) {
        Slogger::E(TAG, "Must narrow by either package or user");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean persistChanged = FALSE;

    GrantUriPermissionMapMap::Iterator mit = mGrantedUriPermissions.Begin();
    while (mit != mGrantedUriPermissions.End()) {
        Int32 targetUid = mit->mFirst;
        AutoPtr<GrantUriPermissionMap> perms = mit->mSecond;

        // Only inspect grants matching user
        if (userHandle == IUserHandle::USER_ALL
            || userHandle == UserHandle::GetUserId(targetUid)) {
            GrantUriPermissionMap::Iterator it = perms->Begin();
            while (it != perms->End()) {
                AutoPtr<UriPermission> perm = it->mSecond;
                // Only inspect grants matching package
                if (packageName == NULL || perm->mSourcePkg.Equals(packageName)
                        || perm->mTargetPkg.Equals(packageName)) {
                    persistChanged |= perm->RevokeModes(persistable
                            ? ~0 : ~IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION, TRUE);

                    // Only remove when no modes remain; any persisted grants
                    // will keep this alive.
                    if (perm->mModeFlags == 0) {
                        perms->Erase(it++);
                        continue;
                    }
                }
                ++it;
            }

            if (perms->IsEmpty()) {
                mGrantedUriPermissions.Erase(mit++);
            }
        }
    }

    if (persistChanged) {
        SchedulePersistUriGrants();
    }

    return NOERROR;
}

ECode CActivityManagerService::NewUriPermissionOwner(
    /* [in] */ const String& name,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("newUriPermissionOwner")));
    AutoLock lock(this);
    AutoPtr<UriPermissionOwner> owner = new UriPermissionOwner(
        this, IObject::Probe(CoreUtils::Convert(name)), TRUE);
    AutoPtr<IBinder> temp = owner->GetExternalTokenLocked();
    *token = temp;
    REFCOUNT_ADD(*token);

    return NOERROR;
}

ECode CActivityManagerService::GrantUriPermissionFromOwner(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 fromUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 sourceUserId,
    /* [in] */ Int32 targetUserId)
{
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
        targetUserId, FALSE, ALLOW_FULL_ONLY, String("grantUriPermissionFromOwner"),
        String(NULL), &targetUserId));

    AutoLock lock(this);
    AutoPtr<UriPermissionOwner> owner = UriPermissionOwner::FromExternalToken(token);
    if (owner == NULL) {
        // throw new IllegalArgumentException("Unknown owner: " + token);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (fromUid != Binder::GetCallingUid()) {
        if (Binder::GetCallingUid() != Process::MyUid()) {
            // Only system code can grant URI permissions on behalf
            // of other users.
            // throw new SecurityException("nice try");
            return E_SECURITY_EXCEPTION;
        }
    }
    if (targetPkg.IsNull()) {
        // throw new IllegalArgumentException("null target");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (uri == NULL) {
        // throw new IllegalArgumentException("null uri");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<GrantUri> grantUri = new GrantUri(sourceUserId, uri, FALSE);
    return GrantUriPermissionLocked(fromUid, targetPkg, grantUri, modeFlags, owner, targetUserId);
}

ECode CActivityManagerService::RevokeUriPermissionFromOwner(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId)
{
    AutoLock lock(this);
    AutoPtr<UriPermissionOwner> owner = UriPermissionOwner::FromExternalToken(token);
    if (owner == NULL) {
        // throw new IllegalArgumentException("Unknown owner: " + token);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (uri == NULL) {
        owner->RemoveUriPermissionsLocked(mode);
    }
    else{
        AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);
        owner->RemoveUriPermissionLocked(grantUri, mode);
    }
    return NOERROR;
}

void CActivityManagerService::SchedulePersistUriGrants()
{
    Boolean hasMessages;
    mHandler->HasMessages(PERSIST_URI_GRANTS_MSG, &hasMessages);
    if (!hasMessages) {
        Boolean res;
        mHandler->SendEmptyMessageDelayed(PERSIST_URI_GRANTS_MSG,
            10 * IDateUtils::SECOND_IN_MILLIS, &res);
    }
}

void CActivityManagerService::WriteGrantedUriPermissions()
{
    if (DEBUG_URI_PERMISSION) Slogger::V(TAG, "writeGrantedUriPermissions()");

    // Snapshot permissions so we can persist without lock
    List<AutoPtr<UriPermission::Snapshot> > persist;
    {    AutoLock syncLock(this);
        GrantUriPermissionMapMap::Iterator mit = mGrantedUriPermissions.Begin();
        for (; mit != mGrantedUriPermissions.End(); ++mit) {
            AutoPtr<GrantUriPermissionMap> perms = mit->mSecond;
            GrantUriPermissionMap::Iterator it = perms->Begin();
            for (; it != perms->End(); ++it) {
                AutoPtr<UriPermission> perm = it->mSecond;
                if (perm->mPersistedModeFlags != 0) {
                    persist.PushBack(perm->GetSnapshot());
                }
            }
        }
    }

    AutoPtr<IFileOutputStream> fos;
    ECode ec;
    do {
        ec = mGrantFile->StartWrite((IFileOutputStream**)&fos);
        if (FAILED(ec))
            break;

        AutoPtr<IXmlSerializer> out;
        CFastXmlSerializer::New((IXmlSerializer**)&out);
        ec = out->SetOutput(IOutputStream::Probe(fos), String("utf-8"));
        if (FAILED(ec))
            break;
        ec = out->StartDocument(String(NULL), TRUE);
        if (FAILED(ec))
            break;
        ec = out->WriteStartTag(String(NULL), TAG_URI_GRANTS);
        if (FAILED(ec))
            break;
        List<AutoPtr<UriPermission::Snapshot> >::Iterator sit;
        for (sit = persist.Begin(); sit != persist.End(); ++sit) {
            AutoPtr<UriPermission::Snapshot> perm = *sit;
            ec = out->WriteStartTag(String(NULL), TAG_URI_GRANT);
            if (FAILED(ec))
                break;
            ec = XmlUtils::WriteInt32Attribute(out, ATTR_SOURCE_USER_ID, perm->mUri->mSourceUserId);
            if (FAILED(ec))
                break;
            ec = XmlUtils::WriteInt32Attribute(out, ATTR_TARGET_USER_ID, perm->mTargetUserId);
            if (FAILED(ec))
                break;
            ec = out->WriteAttribute(String(NULL), ATTR_SOURCE_PKG, perm->mSourcePkg);
            if (FAILED(ec))
                break;
            ec = out->WriteAttribute(String(NULL), ATTR_TARGET_PKG, perm->mTargetPkg);
            if (FAILED(ec))
                break;
            String str;
            IObject::Probe(perm->mUri->mUri)->ToString(&str);
            ec = out->WriteAttribute(String(NULL), ATTR_URI, str);
            if (FAILED(ec))
                break;
            ec = XmlUtils::WriteBooleanAttribute(out, ATTR_PREFIX, perm->mUri->mPrefix);
            if (FAILED(ec))
                break;
            ec = XmlUtils::WriteInt32Attribute(out, ATTR_MODE_FLAGS, perm->mPersistedModeFlags);
            if (FAILED(ec))
                break;
            ec = XmlUtils::WriteInt64Attribute(out, ATTR_CREATED_TIME, perm->mPersistedCreateTime);
            if (FAILED(ec))
                break;
            ec = out->WriteEndTag(String(NULL), TAG_URI_GRANT);
            if (FAILED(ec))
                break;
        }
        ec = out->WriteEndTag(String(NULL), TAG_URI_GRANTS);
        if (FAILED(ec))
            break;
        ec = out->EndDocument();
        if (FAILED(ec))
            break;

        ec = mGrantFile->FinishWrite(fos);
    } while (0);

    if (FAILED(ec)) {
        if (fos != NULL) {
            mGrantFile->FailWrite(fos);
        }
    }
}

void CActivityManagerService::ReadGrantedUriPermissionsLocked()
{
    if (DEBUG_URI_PERMISSION) Slogger::V(TAG, "readGrantedUriPermissions()");
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    AutoPtr<IFileInputStream> fis;
    ECode ec;
    do {
        ec = mGrantFile->OpenRead((IFileInputStream**)&fis);
        if (FAILED(ec))
            break;
        AutoPtr<IXmlPullParser> in;
        ec = Xml::NewPullParser((IXmlPullParser**)&in);
        if (FAILED(ec))
            break;
        ec = in->SetInput(IInputStream::Probe(fis), String(NULL));
        if (FAILED(ec))
            break;

        Int32 type;
        while ((in->Next(&type), type) != IXmlPullParser::END_DOCUMENT) {
            String tag;
            ec = in->GetName(&tag);
            if (FAILED(ec))
                break;
            if (type == IXmlPullParser::START_TAG) {
                if (TAG_URI_GRANT.Equals(tag)) {
                    Int32 sourceUserId;
                    Int32 targetUserId;
                    Int32 userHandle;
                    ec = XmlUtils::ReadInt32Attribute(in, ATTR_USER_HANDLE, IUserHandle::USER_NULL, &userHandle);
                    if (FAILED(ec))
                        break;
                    if (userHandle != IUserHandle::USER_NULL) {
                        // For backwards compatibility.
                        sourceUserId = userHandle;
                        targetUserId = userHandle;
                    }
                    else {
                        ec = XmlUtils::ReadInt32Attribute(in, ATTR_SOURCE_USER_ID, &sourceUserId);
                        if (FAILED(ec))
                            break;
                        ec = XmlUtils::ReadInt32Attribute(in, ATTR_TARGET_USER_ID, &targetUserId);
                        if (FAILED(ec))
                            break;
                    }
                    String sourcePkg;
                    ec = in->GetAttributeValue(String(NULL), ATTR_SOURCE_PKG, &sourcePkg);
                    if (FAILED(ec))
                        break;
                    String targetPkg;
                    ec = in->GetAttributeValue(String(NULL), ATTR_TARGET_PKG, &targetPkg);
                    if (FAILED(ec))
                        break;
                    String strUri;
                    ec = in->GetAttributeValue(String(NULL), ATTR_URI, &strUri);
                    if (FAILED(ec))
                        break;
                    AutoPtr<IUriHelper> uriHelper;
                    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
                    AutoPtr<IUri> uri;
                    uriHelper->Parse(strUri, (IUri**)&uri);
                    Boolean prefix;
                    ec = XmlUtils::ReadBooleanAttribute(in, ATTR_PREFIX, &prefix);
                    if (FAILED(ec))
                        break;
                    Int32 modeFlags;
                    ec = XmlUtils::ReadInt32Attribute(in, ATTR_MODE_FLAGS, &modeFlags);
                    if (FAILED(ec))
                        break;
                    Int64 createdTime;
                    ec = XmlUtils::ReadInt64Attribute(in, ATTR_CREATED_TIME, now, &createdTime);
                    if (FAILED(ec))
                        break;

                    // Sanity check that provider still belongs to source package
                    String authority;
                    uri->GetAuthority(&authority);
                    AutoPtr<IProviderInfo> pi = GetProviderInfoLocked(authority, sourceUserId);
                    String packageName;
                    if (pi != NULL && sourcePkg.Equals((IPackageItemInfo::Probe(pi)->GetPackageName(&packageName), packageName))) {
                        Int32 targetUid = -1;
                        AppGlobals::GetPackageManager()->GetPackageUid(
                            targetPkg, targetUserId, &targetUid);
                        if (targetUid != -1) {
                            AutoPtr<GrantUri> grantUri = new GrantUri(sourceUserId, uri, prefix);
                            AutoPtr<UriPermission> perm = FindOrCreateUriPermissionLocked(
                                    sourcePkg, targetPkg, targetUid, grantUri);
                            perm->InitPersistedModes(modeFlags, createdTime);
                        }
                    }
                    else {
                        Slogger::W(TAG, "Persisted grant for %s had source %s but instead found %s",
                            TO_CSTR(uri), sourcePkg.string(), TO_CSTR(pi));
                    }
                }
            }
        }
    } while (0);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        // Missing grants is okay
    }
    else if (FAILED(ec)) {
        Slogger::W/*wtf*/(TAG, "Failed reading Uri grants ec = 0x%08x", ec);
    }
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(fis));
}

ECode CActivityManagerService::TakePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("takePersistableUriPermission")));

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    FAIL_RETURN(preconditions->CheckFlagsArgument(modeFlags,
        IIntent::FLAG_GRANT_READ_URI_PERMISSION | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION));

    {    AutoLock syncLock(this);
        Int32 callingUid = Binder::GetCallingUid();
        Boolean persistChanged = FALSE;
        AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);
        AutoPtr<GrantUri> grantUri2 = new GrantUri(userId, uri, TRUE);
        AutoPtr<UriPermission> exactPerm = FindUriPermissionLocked(callingUid, grantUri);
        AutoPtr<UriPermission> prefixPerm = FindUriPermissionLocked(callingUid, grantUri2);

        Boolean exactValid = (exactPerm != NULL)
            && ((modeFlags & exactPerm->mPersistableModeFlags) == modeFlags);
        Boolean prefixValid = (prefixPerm != NULL)
            && ((modeFlags & prefixPerm->mPersistableModeFlags) == modeFlags);

        if (!(exactValid || prefixValid)) {
            Slogger::E(TAG, "No persistable permission grants found for UID %d and Uri %s",
                callingUid, grantUri->ToSafeString().string());
            return E_SECURITY_EXCEPTION;
        }

        if (exactValid) {
            persistChanged |= exactPerm->TakePersistableModes(modeFlags);
        }
        if (prefixValid) {
            persistChanged |= prefixPerm->TakePersistableModes(modeFlags);
        }

        persistChanged |= MaybePrunePersistedUriGrantsLocked(callingUid);

        if (persistChanged) {
            SchedulePersistUriGrants();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::ReleasePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("releasePersistableUriPermission")));

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    FAIL_RETURN(preconditions->CheckFlagsArgument(modeFlags,
        IIntent::FLAG_GRANT_READ_URI_PERMISSION | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION));

    {    AutoLock syncLock(this);
        Int32 callingUid = Binder::GetCallingUid();
        Boolean persistChanged = FALSE;
        AutoPtr<GrantUri> grantUri = new GrantUri(userId, uri, FALSE);
        AutoPtr<GrantUri> grantUri2 = new GrantUri(userId, uri, TRUE);
        AutoPtr<UriPermission> exactPerm = FindUriPermissionLocked(callingUid, grantUri);
        AutoPtr<UriPermission> prefixPerm = FindUriPermissionLocked(callingUid, grantUri2);

        if (exactPerm == NULL && prefixPerm == NULL) {
            String str;
            uri->ToSafeString(&str);
            Slogger::E(TAG, "No permission grants found for UID %d and Uri %s",
                callingUid, str.string());
            return E_SECURITY_EXCEPTION;
        }

        if (exactPerm != NULL) {
            persistChanged |= exactPerm->ReleasePersistableModes(modeFlags);
            RemoveUriPermissionIfNeededLocked(exactPerm);
        }
        if (prefixPerm != NULL) {
            persistChanged |= prefixPerm->ReleasePersistableModes(modeFlags);
            RemoveUriPermissionIfNeededLocked(prefixPerm);
        }

        if (persistChanged) {
            SchedulePersistUriGrants();
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::MaybePrunePersistedUriGrantsLocked(
    /* [in] */ Int32 uid)
{
    GrantUriPermissionMapMap::Iterator find = mGrantedUriPermissions.Find(uid);
    AutoPtr<GrantUriPermissionMap> perms;
    if (find != mGrantedUriPermissions.End())
        perms = find->mSecond;
    if (perms != NULL)
        return FALSE;
    if (perms->GetSize() < MAX_PERSISTED_URI_GRANTS)
        return FALSE;

    List<AutoPtr<UriPermission> > persisted;
    GrantUriPermissionMap::Iterator it = perms->Begin();
    for (; it != perms->End(); ++it) {
        AutoPtr<UriPermission> perm = it->mSecond;
        if (perm->mPersistedModeFlags != 0) {
            persisted.PushBack(perm);
        }
    }

    Int32 trimCount = persisted.GetSize() - MAX_PERSISTED_URI_GRANTS;
    if (trimCount <= 0)
        return FALSE;

    persisted.Sort(UriPermission::PersistedTimeCompare);
    List<AutoPtr<UriPermission> >::Iterator pit;
    for (pit = persisted.Begin(); pit != persisted.End(); ++pit) {
        AutoPtr<UriPermission> perm = *pit;

        if (DEBUG_URI_PERMISSION) {
            Slogger::V(TAG, "Trimming grant created at %lld", perm->mPersistedCreateTime);
        }

        perm->ReleasePersistableModes(~0);
        RemoveUriPermissionIfNeededLocked(perm);
    }

    return TRUE;
}

ECode CActivityManagerService::GetPersistedUriPermissions(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean incoming,
    /* [out] */ IParceledListSlice** list)
{
    VALIDATE_NOT_NULL(list);
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getPersistedUriPermissions")));
    if (packageName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 packageUid;
    if (SUCCEEDED(pm->GetPackageUid(packageName, UserHandle::GetUserId(callingUid), &packageUid))) {
        if (packageUid != callingUid) {
            Slogger::E(TAG, "Package %s does not belong to calling UID %d",
                packageName.string(), callingUid);
            return E_SECURITY_EXCEPTION;
        }
    }
    else {
        Slogger::E(TAG, "Failed to verify package name ownership");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    {    AutoLock syncLock(this);
        if (incoming) {
            GrantUriPermissionMapMap::Iterator find = mGrantedUriPermissions.Find(callingUid);
            AutoPtr<GrantUriPermissionMap> perms;
            if (find != mGrantedUriPermissions.End())
                perms = find->mSecond;
            if (perms == NULL) {
                Slogger::W(TAG, "No permission grants found for %s", packageName.string());
            }
            else {
                GrantUriPermissionMap::Iterator it = perms->Begin();
                for (; it != perms->End(); ++it) {
                    AutoPtr<UriPermission> perm = it->mSecond;
                    if (packageName.Equals(perm->mTargetPkg) && perm->mPersistedModeFlags != 0) {
                        result->Add(perm->BuildPersistedPublicApiObject());
                    }
                }
            }
        }
        else {
            GrantUriPermissionMapMap::Iterator mit = mGrantedUriPermissions.Begin();
            for (; mit != mGrantedUriPermissions.End(); ++mit) {
                AutoPtr<GrantUriPermissionMap> perms = mit->mSecond;
                GrantUriPermissionMap::Iterator it = perms->Begin();
                for (; it != perms->End(); ++it) {
                    AutoPtr<UriPermission> perm = it->mSecond;
                    if (packageName.Equals(perm->mSourcePkg) && perm->mPersistedModeFlags != 0) {
                        result->Add(perm->BuildPersistedPublicApiObject());
                    }
                }
            }
        }
    }
    return CParceledListSlice::New(result, list);
}

ECode CActivityManagerService::ShowWaitingForDebugger(
    /* [in] */ IApplicationThread* who,
    /* [in] */ Boolean waiting)
{
    AutoLock lock(this);
    AutoPtr<ProcessRecord> app =
        who != NULL ? GetRecordForAppLocked(who) : NULL;
    if (app == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(WAIT_FOR_DEBUGGER_MSG, (IMessage**)&msg);
    msg->SetObj(app != NULL ? app->Probe(EIID_IInterface) : NULL);
    msg->SetArg1(waiting ? 1 : 0);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode CActivityManagerService::GetMemoryInfo(
    /* [out] */ IActivityManagerMemoryInfo** outInfo)
{
    VALIDATE_NOT_NULL(outInfo);
    *outInfo = NULL;

    FAIL_RETURN(CActivityManagerMemoryInfo::New(outInfo));
    const Int64 homeAppMem = mProcessList->GetMemLevel(ProcessList::HOME_APP_ADJ);
    const Int64 cachedAppMem = mProcessList->GetMemLevel(ProcessList::CACHED_APP_MIN_ADJ);
    (*outInfo)->SetAvailMem(Process::GetFreeMemory());
    (*outInfo)->SetTotalMem(Process::GetTotalMemory());
    (*outInfo)->SetThreshold(homeAppMem);
    Int64 aMem;
    (*outInfo)->GetAvailMem(&aMem);
    (*outInfo)->SetLowMemory(aMem < (homeAppMem + ((cachedAppMem-homeAppMem)/2)));
    (*outInfo)->SetHiddenAppThreshold(cachedAppMem);
    (*outInfo)->SetSecondaryServerThreshold(mProcessList->GetMemLevel(ProcessList::SERVICE_ADJ));
    (*outInfo)->SetVisibleAppThreshold(mProcessList->GetMemLevel(
            ProcessList::VISIBLE_APP_ADJ));
    (*outInfo)->SetForegroundAppThreshold(mProcessList->GetMemLevel(
            ProcessList::FOREGROUND_APP_ADJ));
    return NOERROR;
}

// =========================================================
// TASK MANAGEMENT
// =========================================================

ECode CActivityManagerService::GetAppTasks(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    Int32 callingUid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();

    {
        AutoLock syncLock(this);
        CParcelableList::New(list);
        // try {
            if (localLOGV) Slogger::V(TAG, "getAppTasks");

            List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
            for (; it != mRecentTasks->End(); ++it) {
                AutoPtr<TaskRecord> tr = *it;
                // Skip tasks that do not match the caller.  We don't need to verify
                // callingPackage, because we are also limiting to callingUid and know
                // that will limit to the correct security sandbox.
                if (tr->mEffectiveUid != callingUid) {
                    continue;
                }
                AutoPtr<IIntent> intent = tr->GetBaseIntent();
                String packageName;
                if (intent != NULL) {
                    AutoPtr<IComponentName> component;
                    intent->GetComponent((IComponentName**)&component);
                    component->GetPackageName(&packageName);
                }
                if (intent == NULL || !callingPackage.Equals(packageName)) {
                    continue;
                }
                AutoPtr<IActivityManagerRecentTaskInfo> taskInfo = CreateRecentTaskInfoFromTaskRecord(tr);
                Int32 persistentId;
                taskInfo->GetPersistentId(&persistentId);
                AutoPtr<IIAppTask> taskImpl;
                CAppTaskImpl::New(persistentId, callingUid, this, (IIAppTask**)&taskImpl);
                (*list)->Add(taskImpl);
            }
        // } finally {
            Binder::RestoreCallingIdentity(ident);
        // }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    Int32 callingUid = Binder::GetCallingUid();
    CParcelableList::New(list);

    {
        AutoLock syncLock(this);
        if (localLOGV) Slogger::V(TAG, "getTasks: max=%d, flags=%d", maxNum, flags);

        Boolean allowed = IsGetTasksAllowed(String("getTasks"), Binder::GetCallingPid(),
                callingUid);

        // TODO: Improve with MRU list from all ActivityStacks.
        mStackSupervisor->GetTasksLocked(maxNum, *list, callingUid, allowed);
    }

    return NOERROR;
}

AutoPtr<TaskRecord> CActivityManagerService::GetMostRecentTask()
{
    return mRecentTasks->GetFront();
}

/**
 * Creates a new RecentTaskInfo from a TaskRecord.
 */
AutoPtr<IActivityManagerRecentTaskInfo> CActivityManagerService::CreateRecentTaskInfoFromTaskRecord(
    /* [in] */ TaskRecord* tr)
{
    // Update the task description to reflect any changes in the task stack
    tr->UpdateTaskDescription();

    // Compose the recent task info
    AutoPtr<IActivityManagerRecentTaskInfo> rti;
    CActivityManagerRecentTaskInfo::New((IActivityManagerRecentTaskInfo**)&rti);
    rti->SetId(tr->GetTopActivity() == NULL ? -1 : tr->mTaskId);
    rti->SetPersistentId(tr->mTaskId);
    AutoPtr<IIntent> intent;
    CIntent::New(tr->GetBaseIntent(), (IIntent**)&intent);
    rti->SetBaseIntent(intent);
    rti->SetOrigActivity(tr->mOrigActivity);
    rti->SetDescription(tr->mLastDescription);
    rti->SetStackId(tr->mStack != NULL ? tr->mStack->mStackId : -1);
    rti->SetUserId(tr->mUserId);
    AutoPtr<IActivityManagerTaskDescription> td;
    CActivityManagerTaskDescription::New(tr->mLastTaskDescription,
        (IActivityManagerTaskDescription**)&td);
    rti->SetTaskDescription(td);
    rti->SetFirstActiveTime(tr->mFirstActiveTime);
    rti->SetLastActiveTime(tr->mLastActiveTime);
    rti->SetAffiliatedTaskId(tr->mAffiliatedTaskId);
    rti->SetAffiliatedTaskColor(tr->mAffiliatedTaskColor);
    return rti;
}

Boolean CActivityManagerService::IsGetTasksAllowed(
    /* [in] */ const String& caller,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid)
{
    Int32 permission;
    CheckPermission(Manifest::permission::REAL_GET_TASKS,
            callingPid, callingUid, &permission);
    Boolean allowed = permission == IPackageManager::PERMISSION_GRANTED;
    if (!allowed) {
        CheckPermission(Manifest::permission::GET_TASKS,
            callingPid, callingUid, &permission);
        if (permission == IPackageManager::PERMISSION_GRANTED) {
            // Temporary compatibility: some existing apps on the system image may
            // still be requesting the old permission and not switched to the new
            // one; if so, we'll still allow them full access.  This means we need
            // to see if they are holding the old permission and are a system app.
            Boolean res;
            AppGlobals::GetPackageManager()->IsUidPrivileged(callingUid, &res);
            if (res) {
                allowed = TRUE;
                Slogger::W(TAG, "%s: caller %d is using old GET_TASKS"
                    " but privileged; allowing", caller.string(), callingUid);
            }
        }
    }
    if (!allowed) {
        Slogger::W(TAG, "%s: caller %d does not hold REAL_GET_TASKS;"
            " limiting output", caller.string(), callingUid);
    }
    return allowed;
}

ECode CActivityManagerService::GetRecentTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = NULL;

    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    FAIL_RETURN(HandleIncomingUser(callingPid, callingUid, userId,
        FALSE, ALLOW_FULL_ONLY, String("GetRecentTasks"), String(NULL), &userId));

    Boolean includeProfiles = (flags & IActivityManager::RECENT_INCLUDE_PROFILES) != 0;
    Boolean withExcluded = (flags & IActivityManager::RECENT_WITH_EXCLUDED) != 0;
    AutoLock lock(this);
    Boolean allowed = IsGetTasksAllowed(String("GetRecentTasks"), callingPid, callingUid);
    Boolean detailed = CheckCallingPermission(Manifest::permission::GET_DETAILED_TASKS)
        == IPackageManager::PERMISSION_GRANTED;

    Int32 N = mRecentTasks->GetSize();
    CParcelableList::New(maxNum < N ? maxNum : N, tasks);
    if (DEBUG_RECENTS) Slogger::D(TAG, " RecentTasks size: N:%d, return count:%d", N, maxNum < N ? maxNum : N);

    AutoPtr<HashSet<Int32> > includedUsers;
    if (includeProfiles) {
        includedUsers = GetProfileIdsLocked(userId);
    }
    else {
        includedUsers = new HashSet<Int32>();
    }
    includedUsers->Insert(userId);

    List< AutoPtr<TaskRecord> >::Iterator it;
    for (it = mRecentTasks->Begin(); it != mRecentTasks->End() && maxNum > 0; ++it) {
        AutoPtr<TaskRecord> tr = *it;
        // Only add calling user or related users recent tasks
        if (includedUsers->Find(tr->mUserId) == includedUsers->End()) {
            if (DEBUG_RECENTS) Slogger::D(TAG, "Skipping, not user: %s", TO_CSTR(tr));
            continue;
        }

        // Return the entry if desired by the caller.  We always return
        // the first entry, because callers always expect this to be the
        // foreground app.  We may filter others if the caller has
        // not supplied RECENT_WITH_EXCLUDED and there is some reason
        // we should exclude the entry.

        Int32 flg;
        if (it == mRecentTasks->Begin()
            || withExcluded
            || (tr->mIntent == NULL)
            || ((tr->mIntent->GetFlags(&flg), flg & IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS) == 0)) {
            if (!allowed) {
                // If the caller doesn't have the GET_TASKS permission, then only
                // allow them to see a small subset of tasks -- their own and home.
                if (!tr->IsHomeTask() && tr->mEffectiveUid != callingUid) {
                    if (DEBUG_RECENTS) Slogger::D(TAG, "Skipping, not allowed: %s", TO_CSTR(tr));
                    continue;
                }
            }
            if ((flags & IActivityManager::RECENT_IGNORE_HOME_STACK_TASKS) != 0) {
                if (tr->mStack != NULL && tr->mStack->IsHomeStack()) {
                    if (DEBUG_RECENTS) Slogger::D(TAG, "Skipping, home stack task: %s", TO_CSTR(tr));
                    continue;
                }
            }
            if (tr->mAutoRemoveRecents && tr->GetTopActivity() == NULL) {
                // Don't include auto remove tasks that are finished or finishing.
                if (DEBUG_RECENTS) Slogger::D(TAG, "Skipping, auto-remove without activity: %s", TO_CSTR(tr));
                continue;
            }
            if ((flags & IActivityManager::RECENT_IGNORE_UNAVAILABLE) != 0
                    && !tr->mIsAvailable) {
                if (DEBUG_RECENTS) Slogger::D(TAG, "Skipping, unavail real act: %s", TO_CSTR(tr));
                continue;
            }

            AutoPtr<IActivityManagerRecentTaskInfo> rti = CreateRecentTaskInfoFromTaskRecord(tr);
            if (!detailed) {
                AutoPtr<IIntent> baseIntent;
                rti->GetBaseIntent((IIntent**)&baseIntent);
                baseIntent->ReplaceExtras((IBundle*)NULL);
            }

            if (DEBUG_RECENTS) Slogger::D(TAG, " > recentTasks: %s", TO_CSTR(rti));
            (*tasks)->Add(rti);
            maxNum--;
        }
    }
    return NOERROR;
}

AutoPtr<TaskRecord> CActivityManagerService::RecentTaskForIdLocked(
    /* [in] */ Int32 id)
{
    List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
    for(; it != mRecentTasks->End(); it++) {
        if((*it)->mTaskId == id)
            return *it;
    }
    return NULL;
 }

ECode CActivityManagerService::GetTaskThumbnail(
    /* [in] */ Int32 id,
    /* [out] */ IActivityManagerTaskThumbnail** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail);
    *thumbnail = NULL;

    {
        AutoLock lock(this);
        FAIL_RETURN(EnforceCallingPermission(Manifest::permission::READ_FRAME_BUFFER,
                String("getTaskThumbnail()")));
        AutoPtr<TaskRecord> tr = RecentTaskForIdLocked(id);
        if (tr != NULL) {
            AutoPtr<IActivityManagerTaskThumbnail> temp = tr->GetTaskThumbnailLocked();;
            *thumbnail = temp;
            REFCOUNT_ADD(*thumbnail);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::AddAppTask(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityManagerTaskDescription* description,
    /* [in] */ IBitmap* thumbnail,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    Int32 callingUid = Binder::GetCallingUid();
    Int64 callingIdent = Binder::ClearCallingIdentity();

    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(activityToken);
        if (r == NULL) {
            Binder::RestoreCallingIdentity(callingIdent);
            Slogger::E(TAG, "Activity does not exist; token=%s", TO_CSTR(activityToken));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IComponentName> comp;
        intent->GetComponent((IComponentName**)&comp);
        if (comp == NULL) {
            Binder::RestoreCallingIdentity(callingIdent);
            Slogger::E(TAG, "Intent %s must specify explicit component", TO_CSTR(intent));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 w, h;
        thumbnail->GetWidth(&w);
        thumbnail->GetHeight(&h);
        if (w != mThumbnailWidth || h != mThumbnailHeight) {
            Binder::RestoreCallingIdentity(callingIdent);
            Slogger::E(TAG, "Bad thumbnail size: got %dx%d, require %dx%d",
                w, h, mThumbnailWidth, mThumbnailHeight);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        intent->SetSelector(NULL);
        intent->SetSourceBounds(NULL);
        Int32 flags;
        intent->GetFlags(&flags);
        if ((flags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) != 0) {
            if ((flags & IIntent::FLAG_ACTIVITY_RETAIN_IN_RECENTS) == 0) {
                // The caller has added this as an auto-remove task...  that makes no
                // sense, so turn off auto-remove.
                intent->AddFlags(IIntent::FLAG_ACTIVITY_RETAIN_IN_RECENTS);
            }
        }
        else if ((flags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
            // Must be a new task.
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        }
        Boolean equals = Object::Equals(comp, mLastAddedTaskComponent);
        if (!equals || callingUid != mLastAddedTaskUid) {
            mLastAddedTaskActivity = NULL;
        }
        AutoPtr<IActivityInfo> ainfo = mLastAddedTaskActivity;
        if (ainfo == NULL) {
            AppGlobals::GetPackageManager()->GetActivityInfo(
                comp, 0, UserHandle::GetUserId(callingUid), (IActivityInfo**)&ainfo);
            mLastAddedTaskActivity = ainfo;
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(ainfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            if (uid != callingUid) {
                Binder::RestoreCallingIdentity(callingIdent);
                Slogger::E(TAG, "Can't add task for another application: target uid= %d"
                    ", calling uid=%d", uid, callingUid);
                return E_SECURITY_EXCEPTION;
            }
        }

        AutoPtr<TaskRecord> task = new TaskRecord();
        task->constructor(this, mStackSupervisor->GetNextTaskId(), ainfo, intent, description);

        Int32 trimIdx = TrimRecentsForTask(task, FALSE);
        if (trimIdx >= 0) {
            // If this would have caused a trim, then we'll abort because that
            // means it would be added at the end of the list but then just removed.
            Binder::RestoreCallingIdentity(callingIdent);
            return NOERROR;
        }

        Int32 N = mRecentTasks->GetSize();
        AutoPtr<IActivityManagerHelper> amhelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
        Int32 maxRecentTasks;
        amhelper->GetMaxRecentTasksStatic(&maxRecentTasks);
        if (N >= (maxRecentTasks - 1)) {
            AutoPtr<TaskRecord> tr = mRecentTasks->GetBack();
            mRecentTasks->PopBack();
            tr->RemovedFromRecents(mTaskPersister);
        }

        // disable for detecting memory leak
        task->mInRecents = TRUE;
        mRecentTasks->PushBack(task);
        r->mTask->mStack->AddTask(task, FALSE, FALSE);

        task->SetLastThumbnail(thumbnail);
        task->FreeLastThumbnail();
        Binder::RestoreCallingIdentity(callingIdent);
        *result = task->mTaskId;
    }

    return NOERROR;
}

ECode CActivityManagerService::GetAppTaskThumbnailSize(
    /* [out] */ IPoint** point)
{
    AutoLock syncLock(this);
    return CPoint::New(mThumbnailWidth, mThumbnailHeight, point);
}

ECode CActivityManagerService::SetTaskDescription(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityManagerTaskDescription* td)
{
    AutoLock syncLock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r != NULL) {
        r->SetTaskDescription(td);
        r->mTask->UpdateTaskDescription();
    }

    return NOERROR;
}

ECode CActivityManagerService::GetTaskDescriptionIcon(
    /* [in] */ const String& filename,
    /* [out] */ IBitmap** icon)
{
    VALIDATE_NOT_NULL(icon);
    if (!FileUtils::IsValidExtFilename(filename)
        || !filename.Contains(ActivityRecord::ACTIVITY_ICON_SUFFIX)) {
        Slogger::E(TAG, "Bad filename: %s", filename.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBitmap> temp = mTaskPersister->GetTaskDescriptionIcon(filename);
    *icon = temp;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode CActivityManagerService::CleanUpRemovedTaskLocked(
    /* [in] */ TaskRecord* tr,
    /* [in] */ Int32 flags)
{
    mRecentTasks->Remove(tr);
    tr->RemovedFromRecents(mTaskPersister);
    Boolean killProcesses = (flags & IActivityManager::REMOVE_TASK_KILL_PROCESS) != 0;
    AutoPtr<IIntent> baseIntent;
    CIntent::New(tr->mIntent != NULL ? tr->mIntent : tr->mAffinityIntent, (IIntent**)&baseIntent);
    AutoPtr<IComponentName> component;
    baseIntent->GetComponent((IComponentName**)&component);
    if (component == NULL) {
        Slogger::W(TAG, "Now component for base intent of task: %p", tr);
        return NOERROR;
    }

    // Find any running services associated with this app.
    mServices->CleanUpRemovedTaskLocked(tr, component, baseIntent);

    if (killProcesses) {
        // Find any running processes associated with this app.
        String pkg;
        component->GetPackageName(&pkg);
        AutoPtr<ICharSequence> pkgKey = CoreUtils::Convert(pkg);
        List<AutoPtr<ProcessRecord> > procs;
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > pmap;
        pmap = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
        for (it = pmap->Begin(); it != pmap->End(); ++it) {
            AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > uids = it->mSecond;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator uit;
            for (uit = uids->Begin(); uit != uids->End(); ++uit) {
                AutoPtr<ProcessRecord> proc = uit->mSecond;
                if (proc->mUserId != tr->mUserId) {
                    continue;
                }

                Boolean res;
                proc->mPkgList->ContainsKey(pkgKey, &res);
                if (!res) {
                    continue;
                }
                procs.PushBack(proc);
            }
        }

        // Kill the running processes.
        List<AutoPtr<ProcessRecord> >::Iterator procsIt;
        for (procsIt = procs.Begin(); procsIt != procs.End(); ++procsIt) {
            AutoPtr<ProcessRecord> pr = *procsIt;
            if (pr == mHomeProcess) {
                // Don't kill the home process along with tasks from the same package.
                continue;
            }
            if (pr->mSetSchedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE) {
                pr->Kill(String("remove task"), TRUE);
            }
            else {
                pr->mWaitingToKill = "remove task";
            }
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::RemoveTaskByIdLocked(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags)
{
    AutoPtr<TaskRecord> tr = RecentTaskForIdLocked(taskId);
    if (tr != NULL) {
        tr->RemoveTaskActivitiesLocked();
        CleanUpRemovedTaskLocked(tr, flags);
        if (tr->mIsPersistable) {
            NotifyTaskPersisterLocked(NULL, TRUE);
        }
        return TRUE;
    }
    return FALSE;
}

ECode CActivityManagerService::RemoveTask(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* removed)
{
    VALIDATE_NOT_NULL(removed);
    *removed = FALSE;

    {
        AutoLock lock(this);
        FAIL_RETURN(EnforceCallingPermission(Manifest::permission::REMOVE_TASKS,
                String("removeTask()")));
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        *removed = RemoveTaskByIdLocked(taskId, flags);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }

    return NOERROR;
}

/**
 * TODO: Add mController hook
 */
ECode CActivityManagerService::MoveTaskToFront(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::REORDER_TASKS,
            String("moveTaskToFront()")));
    if (DEBUG_STACK) Slogger::D(TAG, "moveTaskToFront: moving taskId=%d", taskId);
    {    AutoLock syncLock(this);
        MoveTaskToFrontLocked(taskId, flags, options);
    }
    return NOERROR;
}

ECode CActivityManagerService::MoveTaskToFrontLocked(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    AutoPtr<IActivityOptionsHelper> aoHelper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
    if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
           Binder::GetCallingUid(), -1, -1, String("Task to front"))) {
        aoHelper->Abort(options);
        return NOERROR;
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<TaskRecord> task = mStackSupervisor->AnyTaskForIdLocked(taskId);
    if (task == NULL) {
        return NOERROR;
    }
    if (mStackSupervisor->IsLockTaskModeViolation(task)) {
        mStackSupervisor->ShowLockTaskToast();
        Slogger::E(TAG, "moveTaskToFront: Attempt to violate Lock Task Mode");
        return NOERROR;
    }
    AutoPtr<ActivityRecord> prev = mStackSupervisor->TopRunningActivityLocked();
    if (prev != NULL && prev->IsRecentsActivity()) {
        task->SetTaskToReturnTo(ActivityRecord::RECENTS_ACTIVITY_TYPE);
    }
    mStackSupervisor->FindTaskToMoveToFrontLocked(task, flags, options);
    // } finally {
    Binder::RestoreCallingIdentity(origId);
    // }
    aoHelper->Abort(options);
    return NOERROR;
}

ECode CActivityManagerService::MoveTaskToBack(
    /* [in] */ Int32 taskId)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::REORDER_TASKS,
           String("moveTaskToBack()")));

    AutoLock lock(this);
    AutoPtr<TaskRecord> tr = RecentTaskForIdLocked(taskId);
    if (tr != NULL) {
        if (tr == mStackSupervisor->mLockTaskModeTask) {
            mStackSupervisor->ShowLockTaskToast();
            return NOERROR;
        }
        if (DEBUG_STACK) Slogger::D(TAG, "moveTaskToBack: moving task=%s", TO_CSTR(tr));
        AutoPtr<ActivityStack> stack = tr->mStack;
        if (stack->mResumedActivity != NULL && stack->mResumedActivity->mTask == tr) {
            if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
                Binder::GetCallingUid(), -1, -1, String("Task to back"))) {
                return NOERROR;
            }
        }
        const Int64 origId = Binder::ClearCallingIdentity();
        stack->MoveTaskToBackLocked(taskId, NULL);
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

/**
 * Moves an activity, and all of the other activities within the same task, to the bottom
 * of the history stack.  The activity's order within the task is unchanged.
 *
 * @param token A reference to the activity we wish to move
 * @param nonRoot If FALSE then this only works if the activity is the root
 *                of a task; if TRUE it will work for any activity in a task.
 * @return Returns TRUE if the move completed, FALSE if not.
 */
ECode CActivityManagerService::MoveActivityTaskToBack(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean nonRoot,
    /* [out] */ Boolean* completed)
{
    VALIDATE_NOT_NULL(completed);
    *completed = FALSE;

    String name("moveActivityTaskToBack");
    FAIL_RETURN(EnforceNotIsolatedCaller(name));

    {
        AutoLock lock(this);
        const Int64 origId = Binder::ClearCallingIdentity();
        Int32 taskId = ActivityRecord::GetTaskForActivityLocked(token, !nonRoot);
        if (taskId >= 0) {
            if ((mStackSupervisor->mLockTaskModeTask != NULL)
                && (mStackSupervisor->mLockTaskModeTask->mTaskId == taskId)) {
                mStackSupervisor->ShowLockTaskToast();
                Binder::RestoreCallingIdentity(origId);
                return NOERROR;
            }
            *completed = ActivityRecord::GetStackLocked(token)->MoveTaskToBackLocked(taskId, NULL);
        }
        Binder::RestoreCallingIdentity(origId);
    }

    return NOERROR;
}

ECode CActivityManagerService::MoveTaskBackwards(
    /* [in] */ Int32 task)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::REORDER_TASKS,
            String("moveTaskBackwards()")));

    AutoLock lock(this);
    if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
        Binder::GetCallingUid(), -1, -1, String("Task backwards"))) {
        return NOERROR;
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    MoveTaskBackwardsLocked(task);
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::MoveTaskBackwardsLocked(
    /* [in] */ Int32 task)
{
    Slogger::E(TAG, "moveTaskBackwards not yet implemented!");
    return NOERROR;
}

ECode CActivityManagerService::GetHomeActivityToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("getHomeActivityToken()")));
    {    AutoLock syncLock(this);
        AutoPtr<IBinder> temp = mStackSupervisor->GetHomeActivityToken();
        *token = temp;
        REFCOUNT_ADD(*token);
    }
    return NOERROR;
}

ECode CActivityManagerService::CreateActivityContainer(
    /* [in] */ IBinder* parentActivityToken,
    /* [in] */ IActivityContainerCallback* callback,
    /* [out] */ IIActivityContainer** container)
{
    VALIDATE_NOT_NULL(container);
    *container = NULL;
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("createActivityContainer()")));
    {    AutoLock syncLock(this);
        if (parentActivityToken == NULL) {
            Slogger::E(TAG, "parent token must not be NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(parentActivityToken);
        if (r == NULL) {
            return NOERROR;
        }
        if (callback == NULL) {
            Slogger::E(TAG, "callback must not be NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IIActivityContainer> temp = mStackSupervisor->CreateActivityContainer(r, callback);
        *container = temp;
        REFCOUNT_ADD(*container);
    }
    return NOERROR;
}

ECode CActivityManagerService::DeleteActivityContainer(
    /* [in] */ IIActivityContainer* container)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("deleteActivityContainer()")));
    {    AutoLock syncLock(this);
        mStackSupervisor->DeleteActivityContainer(container);
    }
    return NOERROR;
}

ECode CActivityManagerService::GetEnclosingActivityContainer(
    /* [in] */ IBinder* activityToken,
    /* [out] */ IIActivityContainer** container)
{
    VALIDATE_NOT_NULL(container);
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(activityToken);
        if (stack != NULL) {
            *container = stack->mActivityContainer;
            REFCOUNT_ADD(*container);
            return NOERROR;
        }
        *container = NULL;
    }
    return NOERROR;
}

ECode CActivityManagerService::GetActivityDisplayId(
    /* [in] */ IBinder* activityToken,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(activityToken);
        if (stack != NULL && stack->mActivityContainer->IsAttachedLocked()) {
            return stack->mActivityContainer->GetDisplayId(result);
        }
        *result = IDisplay::DEFAULT_DISPLAY;
    }
    return NOERROR;
}

ECode CActivityManagerService::MoveTaskToStack(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 stackId,
    /* [in] */ Boolean toTop)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("moveTaskToStack()")));
    if (stackId == ActivityStackSupervisor::HOME_STACK_ID) {
        Slogger::E(TAG, "moveTaskToStack: Attempt to move task %d to home stack", taskId);
            // new RuntimeException("here").fillInStackTrace());
    }
    {    AutoLock syncLock(this);
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
            if (DEBUG_STACK) Slogger::D(TAG, "moveTaskToStack: moving task=%d to stackId=%d toTop=%d",
                    taskId, stackId, toTop);
            mStackSupervisor->MoveTaskToStack(taskId, stackId, toTop);
        // } finally {
            Binder::RestoreCallingIdentity(ident);
        // }
    }
    return NOERROR;
}

ECode CActivityManagerService::ResizeStack(
    /* [in] */ Int32 stackBoxId,
    /* [in] */ IRect* bounds)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("resizeStackBox()")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        mWindowManager->ResizeStack(stackBoxId, bounds);
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::GetAllStackInfos(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("getAllStackInfos()")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {
            AutoLock syncLock(this);
            AutoPtr<IList> temp = IList::Probe(mStackSupervisor->GetAllStackInfosLocked());
            *list = temp;
            REFCOUNT_ADD(*list);
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::GetStackInfo(
    /* [in] */ Int32 stackId,
    /* [out] */ IActivityManagerStackInfo** info)
{
    VALIDATE_NOT_NULL(info);
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("getStackInfo()")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {    AutoLock syncLock(this);
            AutoPtr<IActivityManagerStackInfo> temp = mStackSupervisor->GetStackInfoLocked(stackId);
            *info = temp;
            REFCOUNT_ADD(*info);
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::IsInHomeStack(
    /* [in] */ Int32 taskId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("getStackInfo()")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {    AutoLock syncLock(this);
            AutoPtr<TaskRecord> tr = RecentTaskForIdLocked(taskId);
            *result = tr != NULL && tr->mStack != NULL && tr->mStack->IsHomeStack();
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::GetTaskForActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyRoot,
    /* [out] */ Int32* taskId)
{
    VALIDATE_NOT_NULL(taskId)
    {    AutoLock syncLock(this);
        *taskId = ActivityRecord::GetTaskForActivityLocked(token, onlyRoot);
    }
    return NOERROR;
}

Boolean CActivityManagerService::IsLockTaskAuthorized(
    /* [in] */ const String& pkg)
{
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&service);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(service);

    AutoPtr<IUserHandle> h = Binder::GetCallingUserHandle();
    Int32 id;
    h->GetIdentifier(&id);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 uid;
    if (FAILED(pm->GetPackageUid(pkg, id, &uid)))
        return FALSE;
    Boolean res;
    return (uid == Binder::GetCallingUid()) && dpm != NULL
        && (dpm->IsLockTaskPermitted(pkg, &res), res);
}

ECode CActivityManagerService::StartLockTaskMode(
    /* [in] */ TaskRecord* task)
{
    String pkg;
    {    AutoLock syncLock(this);
        AutoPtr<IComponentName> component;
        task->mIntent->GetComponent((IComponentName**)&component);
        component->GetPackageName(&pkg);
    }
    Boolean isSystemInitiated = Binder::GetCallingUid() == IProcess::SYSTEM_UID;
    if (!isSystemInitiated && !IsLockTaskAuthorized(pkg)) {
        AutoPtr<Runnable> r = new LockTaskRunnable(task, mLockToAppRequest);
        Boolean res;
        mHandler->Post(r, &res);
        return NOERROR;
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {    AutoLock syncLock(this);
            // Since we lost lock on task, make sure it is still there.
            task = mStackSupervisor->AnyTaskForIdLocked(task->mTaskId);
            if (task != NULL) {
                if (!isSystemInitiated && ((mFocusedActivity == NULL)
                    || (task != mFocusedActivity->mTask))) {
                    Slogger::E(TAG, "Invalid task, not in foreground");
                    Binder::RestoreCallingIdentity(ident);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                mStackSupervisor->SetLockTaskModeLocked(task, !isSystemInitiated);
            }
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

ECode CActivityManagerService::StartLockTaskMode(
    /* [in] */ Int32 taskId)
{
    AutoPtr<TaskRecord> task;
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {    AutoLock syncLock(this);
            task = mStackSupervisor->AnyTaskForIdLocked(taskId);
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    if (task != NULL) {
        StartLockTaskMode(task);
    }
    return NOERROR;
}

ECode CActivityManagerService::StartLockTaskMode(
    /* [in] */ IBinder* token)
{
    AutoPtr<TaskRecord> task;
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {    AutoLock syncLock(this);
            AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(token);
            if (r == NULL) {
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }
            task = r->mTask;
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    if (task != NULL) {
        StartLockTaskMode(task);
    }
    return NOERROR;
}

ECode CActivityManagerService::StartLockTaskModeOnCurrent()
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("startLockTaskModeOnCurrent")));
    AutoPtr<ActivityRecord> r;
    {    AutoLock syncLock(this);
        r = mStackSupervisor->TopRunningActivityLocked();
    }
    StartLockTaskMode(r->mTask);
    return NOERROR;
}

ECode CActivityManagerService::StopLockTaskMode()
{
    // Verify that the user matches the package of the intent for the TaskRecord
    // we are locked to or systtem.  This will ensure the same caller for startLockTaskMode
    // and stopLockTaskMode.
    Int32 callingUid = Binder::GetCallingUid();
    if (callingUid != IProcess::SYSTEM_UID) {
        AutoPtr<IComponentName> component;
        mStackSupervisor->mLockTaskModeTask->mIntent->GetComponent(
            (IComponentName**)&component);
        String pkg;
        component->GetPackageName(&pkg);
        AutoPtr<IUserHandle> h = Binder::GetCallingUserHandle();
        Int32 id;
        h->GetIdentifier(&id);
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        Int32 uid;
        if (FAILED(pm->GetPackageUid(pkg, id, &uid))) {
            Logger::D(TAG, "stopLockTaskMode ");
            return NOERROR;
        }
        if (uid != callingUid) {
            Slogger::E(TAG, "Invalid uid, expected %d", uid);
            return E_SECURITY_EXCEPTION;
        }
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        Logger::D(TAG, "stopLockTaskMode");
        // Stop lock task
        {    AutoLock syncLock(this);
            mStackSupervisor->SetLockTaskModeLocked(NULL, FALSE);
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::StopLockTaskModeOnCurrent()
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::MANAGE_ACTIVITY_STACKS,
            String("stopLockTaskModeOnCurrent")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        StopLockTaskMode();
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::IsInLockTaskMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        *result = mStackSupervisor->IsInLockTaskMode();
    }
    return NOERROR;
}

// =========================================================
// CONTENT PROVIDERS
// =========================================================

AutoPtr<IList> CActivityManagerService::GenerateApplicationProvidersLocked(
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<IList> providers;

    AutoPtr<IIPackageManager> pkgMgr = AppGlobals::GetPackageManager();
    pkgMgr->QueryContentProviders(
            app->mProcessName, app->mUid,
            STOCK_PM_FLAGS | IPackageManager::GET_URI_PERMISSION_PATTERNS,
            (IList**)&providers);

    if (DEBUG_MU) {
        Slogger::V(TAG_MU, "generateApplicationProvidersLocked, app.info.uid = %d", app->mUid);
    }
    Int32 userId = app->mUserId;
    if (providers != NULL) {
        Int32 N;
        providers->GetSize(&N);
        // app->mPubProviders->EnsureCapacity(N + app.pubProviders.size());
        String proName, cpiPkgName, cpiName, cpiCName;
        Boolean singleton, multiprocess;
        Int32 cpiflags, versionCode;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> item;
            providers->Get(i, (IInterface**)&item);
            AutoPtr<IProviderInfo> cpi = IProviderInfo::Probe(item);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(cpi)->GetProcessName(&proName);
            IComponentInfo::Probe(cpi)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            IPackageItemInfo::Probe(cpi)->GetPackageName(&cpiPkgName);
            IPackageItemInfo::Probe(cpi)->GetName(&cpiName);
            cpi->GetFlags(&cpiflags);

            IsSingleton(proName, appInfo, cpiName, cpiflags, &singleton);
            if (singleton && UserHandle::GetUserId(app->mUid) != 0) {
                // This is a singleton provider, but a user besides the
                // default user is asking to initialize a process it runs
                // in...  well, no, it doesn't actually run in this process,
                // it runs in the process of the default user.  Get rid of it.
                providers->Remove(i);
                N--;
                i--;
                continue;
            }
            AutoPtr<IComponentName> comp;
            CComponentName::New(cpiPkgName, cpiName, (IComponentName**)&comp);
            AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByClass(comp, userId);
            if (cpr == NULL) {
                cpr = new ContentProviderRecord(this, cpi, app->mInfo, comp, singleton);
                mProviderMap->PutProviderByClass(comp, cpr);
            }
            if (DEBUG_MU) {
                Slogger::V(TAG_MU, "generateApplicationProvidersLocked, cpi.uid = %d", cpr->mUid);
            }
            (app->mPubProviders)[cpiName] = cpr;

            IPackageItemInfo::Probe(appInfo)->GetPackageName(&cpiCName);
            cpi->GetMultiprocess(&multiprocess);
            if (!multiprocess || !cpiPkgName.Equals("android")) {
                // Don't add this if it is a platform component that is marked
                // to run in multiple processes, because this is actually
                // part of the framework so doesn't make sense to track as a
                // separate apk in the process.
                appInfo->GetVersionCode(&versionCode);
                app->AddPackage(cpiCName, versionCode, mProcessStats);
            }
            EnsurePackageDexOpt(cpiCName);
        }
    }

    return providers;
}

/**
 * Check if {@link ProcessRecord} has a possible chance at accessing the
 * given {@link ProviderInfo}. Final permission checking is always done
 * in {@link ContentProvider}.
 */
String CActivityManagerService::CheckContentProviderPermissionLocked(
    /* [in] */ IProviderInfo* cpi,
    /* [in] */ ProcessRecord* r,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean checkUser)
{
    String nullStr;
    Int32 callingPid = (r != NULL) ? r->mPid : Binder::GetCallingPid();
    Int32 callingUid = (r != NULL) ? r->mUid : Binder::GetCallingUid();
    Boolean checkedGrants = FALSE;
    if (checkUser) {
        // Looking for cross-user grants before enforcing the typical cross-users permissions
        Int32 tmpTargetUserId = UnsafeConvertIncomingUser(userId);
        if (tmpTargetUserId != UserHandle::GetUserId(callingUid)) {
            if (CheckAuthorityGrants(callingUid, cpi, tmpTargetUserId, checkUser)) {
                return nullStr;
            }
            checkedGrants = TRUE;
        }
        String authority;
        cpi->GetAuthority(&authority);
        HandleIncomingUser(callingPid, callingUid, userId, FALSE, ALLOW_NON_FULL,
            String("checkContentProviderPermissionLocked ") + authority, nullStr, &userId);
        if (userId != tmpTargetUserId) {
            // When we actually went to determine the final targer user ID, this ended
            // up different than our initial check for the authority.  This is because
            // they had asked for USER_CURRENT_OR_SELF and we ended up switching to
            // SELF.  So we need to re-check the grants again.
            checkedGrants = FALSE;
        }
    }

    Boolean cpiExported;
    String cpiReadPerm, cpiWritePerm;
    cpi->GetReadPermission(&cpiReadPerm);
    cpi->GetWritePermission(&cpiWritePerm);
    IComponentInfo::Probe(cpi)->GetExported(&cpiExported);
    AutoPtr<IApplicationInfo> cpiAppInfo;
    IComponentInfo::Probe(cpi)->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
    Int32 cpiUid;
    cpiAppInfo->GetUid(&cpiUid);
    if (CheckComponentPermission(cpiReadPerm, callingPid, callingUid,
            cpiUid, cpiExported)
            == IPackageManager::PERMISSION_GRANTED) {
        return nullStr;
    }
    if (CheckComponentPermission(cpiWritePerm, callingPid, callingUid,
            cpiUid, cpiExported)
            == IPackageManager::PERMISSION_GRANTED) {
        return nullStr;
    }

    AutoPtr< ArrayOf<IPathPermission*> > pps;
    cpi->GetPathPermissions((ArrayOf<IPathPermission*>**)&pps);
    if (pps != NULL) {
        Int32 i = pps->GetLength();
        while (i > 0) {
            i--;
            IPathPermission* pp = (*pps)[i];

            String readPerm;
            pp->GetReadPermission(&readPerm);
            if (readPerm != NULL && (CheckComponentPermission(readPerm, callingPid, callingUid,
                    cpiUid, cpiExported) == IPackageManager::PERMISSION_GRANTED)) {
                return nullStr;
            }

            String writePerm;
            pp->GetWritePermission(&writePerm);
            if (writePerm != NULL && (CheckComponentPermission(writePerm, callingPid, callingUid,
                    cpiUid, cpiExported) == IPackageManager::PERMISSION_GRANTED)) {
                return nullStr;
            }
        }
    }

    if (!checkedGrants && CheckAuthorityGrants(callingUid, cpi, userId, checkUser)) {
        return nullStr;
    }

    String cpiName;
    IPackageItemInfo::Probe(cpi)->GetName(&cpiName);
    String appDes = r->ToString();
    StringBuilder sb;
    if (!cpiWritePerm) {
        sb += "Permission Denial: opening provider ";
        sb += cpiName;
        sb += " from ";
        sb += (r != NULL ? appDes : "(null)" );
        sb += " (pid=";
        sb += callingPid;
        sb += ", uid=";
        sb += callingUid;
        sb += ") that is not exported from uid ";
        sb += cpiUid;
    }
    else {
        sb += "Permission Denial: opening provider ";
        sb += cpiName;
        sb += " from ";
        sb += (r != NULL ? appDes : "(null)" );
        sb += " (pid=";
        sb += callingPid;
        sb += ", uid=";
        sb += callingUid;
        sb += ") requires ";
        sb += cpiReadPerm;
        sb += " or ";
        sb += cpiWritePerm;
    }

    String msg = sb.ToString();
    Slogger::W(TAG, msg.string());;
    return msg;
}

/**
 * Returns if the ContentProvider has granted a uri to callingUid
 */
Boolean CActivityManagerService::CheckAuthorityGrants(
    /* [in] */ Int32 callingUid,
    /* [in] */ IProviderInfo* cpi,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean checkUser)
{
    GrantUriPermissionMapMap::Iterator it = mGrantedUriPermissions.Find(callingUid);
    AutoPtr<GrantUriPermissionMap> perms;
    if (it != mGrantedUriPermissions.End())
        perms = it->mSecond;

    if (perms != NULL) {
        GrantUriPermissionMap::Iterator pIt = perms->Begin();
        for (; pIt != perms->End(); ++pIt) {
            AutoPtr<GrantUri> grantUri = pIt->mFirst;
            if (grantUri->mSourceUserId == userId || !checkUser) {
                if (MatchesProvider(grantUri->mUri, cpi)) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/**
 * Returns TRUE if the uri authority is one of the authorities specified in the provider.
 */
Boolean CActivityManagerService::MatchesProvider(
    /* [in] */ IUri* uri,
    /* [in] */ IProviderInfo* cpi)
{
    String uriAuth;
    uri->GetAuthority(&uriAuth);
    String cpiAuth;
    cpi->GetAuthority(&cpiAuth);
    if (cpiAuth.IndexOf(';') == -1) {
        return cpiAuth.Equals(uriAuth);
    }
    AutoPtr<ArrayOf<String> > cpiAuths;
    StringUtils::Split(cpiAuth, ";", (ArrayOf<String>**)&cpiAuths);
    Int32 length = cpiAuths->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*cpiAuths)[i].Equals(uriAuth))
            return TRUE;
    }
    return FALSE;
}

AutoPtr<CContentProviderConnection> CActivityManagerService::IncProviderCountLocked(
    /* [in] */ ProcessRecord* r,
    /* [in] */ ContentProviderRecord* cpr,
    /* [in] */ IBinder* externalProcessToken,
    /* [in] */ Boolean stable)
{
    if (r != NULL) {
        List< AutoPtr<CContentProviderConnection> >::Iterator it;
        for (it = r->mConProviders.Begin(); it != r->mConProviders.End(); ++it) {
            AutoPtr<CContentProviderConnection> conn = *it;
            if (conn->mProvider.Get() == cpr) {
                if (DEBUG_PROVIDER) {
                    String pName;
                    IComponentInfo::Probe(cpr->mInfo)->GetProcessName(&pName);
                    String nameString;
                    cpr->mName->FlattenToShortString(&nameString);
                    Slogger::V(TAG,
                        "Adding provider requested by %s from process %s: %s scnt=%d uscnt=%d",
                        r->mProcessName.string(), pName.string(), nameString.string(),
                        conn->mStableCount, conn->mUnstableCount);
                }
                if (stable) {
                    conn->mStableCount++;
                    conn->mNumStableIncs++;
                }
                else {
                    conn->mUnstableCount++;
                    conn->mNumUnstableIncs++;
                }
                return conn;
            }
        }
        AutoPtr<CContentProviderConnection> conn;
        CContentProviderConnection::NewByFriend((Handle32)cpr, (Handle32)r, (CContentProviderConnection**)&conn);
        if (stable) {
            conn->mStableCount = 1;
            conn->mNumStableIncs = 1;
        }
        else {
            conn->mUnstableCount = 1;
            conn->mNumUnstableIncs = 1;
        }
        cpr->mConnections.PushBack(conn);
        r->mConProviders.PushBack(conn);
        return conn;
    }
    cpr->AddExternalProcessHandleLocked(externalProcessToken);
    return NULL;
}

Boolean CActivityManagerService::DecProviderCountLocked(
    /* [in] */ CContentProviderConnection* conn,
    /* [in] */ ContentProviderRecord* cpr,
    /* [in] */ IBinder* externalProcessToken,
    /* [in] */ Boolean stable)
{
    if (conn != NULL) {
        cpr = conn->mProvider;
        if (DEBUG_PROVIDER) {
            String pName;
            IComponentInfo::Probe(cpr->mInfo)->GetProcessName(&pName);
            String nameString;
            cpr->mName->FlattenToShortString(&nameString);
            Slogger::V(TAG,
                "Removing provider requested by %s from process %s: %s scnt=%d uscnt=",
                conn->mClient->mProcessName.string(), pName.string(),
                nameString.string(), conn->mStableCount, conn->mUnstableCount);
        }
        if (stable) {
            conn->mStableCount--;
        }
        else {
            conn->mUnstableCount--;
        }
        if (conn->mStableCount == 0 && conn->mUnstableCount == 0) {
            cpr->mConnections.Remove(conn);
            conn->mClient->mConProviders.Remove(conn);
            return TRUE;
        }
        return FALSE;
    }
    cpr->RemoveExternalProcessHandleLocked(externalProcessToken);
    return FALSE;
}

void CActivityManagerService::CheckTime(
    /* [in] */ Int64 startTime,
    /* [in] */ const String& where)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if ((now - startTime) > 1000) {
        // If we are taking more than a second, log about it.
        Slogger::W(TAG, "Slow operation: %lld ms so far, now at %s", (now-startTime), where.string());
    }

    // if (DEBUG) {
    //     Slogger::I(TAG, where.string());
    // }
}

ECode CActivityManagerService::GetContentProviderImpl(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& name,
    /* [in] */ IBinder* token,
    /* [in] */ Boolean stable,
    /* [in] */ Int32 userId,
    /* [out] */ IContentProviderHolder** providerHolder)
{
    VALIDATE_NOT_NULL(providerHolder);
    *providerHolder = NULL;

    AutoPtr<ContentProviderRecord> cpr;
    AutoPtr<CContentProviderConnection> conn;
    AutoPtr<IProviderInfo> cpi;

    {
        AutoLock lock(this);
        Int64 startTime = SystemClock::GetElapsedRealtime();

        AutoPtr<ProcessRecord> r;
        if (caller != NULL) {
            r = GetRecordForAppLocked(caller);
            if (r == NULL) {
                String callerDes;
                IBinder::Probe(caller)->ToString(&callerDes);
                Slogger::E(TAG, "Unable to find app for caller %s (pid=%d) when getting content provider %s",
                    callerDes.string(), Binder::GetCallingPid(), name.string());
                return E_SECURITY_EXCEPTION;
            }
        }

        Boolean checkCrossUser = TRUE;

        CheckTime(startTime, String("getContentProviderImpl: getProviderByName"));

        // First check if this content provider has been published...
        cpr = mProviderMap->GetProviderByName(name, userId);
        // If that didn't work, check if it exists for user 0 and then
        // verify that it's a singleton provider before using it.
        if (cpr == NULL && userId != IUserHandle::USER_OWNER) {
            cpr = mProviderMap->GetProviderByName(name, IUserHandle::USER_OWNER);
            if (cpr != NULL) {
                cpi = cpr->mInfo;
                String proName, cpiName;
                Int32 cpiflags, uid;
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(cpi)->GetProcessName(&proName);
                IComponentInfo::Probe(cpi)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                IPackageItemInfo::Probe(cpi)->GetName(&cpiName);
                cpi->GetFlags(&cpiflags);
                appInfo->GetUid(&uid);
                Boolean isSingleton;
                FAIL_RETURN(IsSingleton(proName, appInfo, cpiName, cpiflags, &isSingleton));
                if (isSingleton && IsValidSingletonCall(r->mUid, uid)) {
                    userId = IUserHandle::USER_OWNER;
                    checkCrossUser = FALSE;
                }
                else {
                    cpr = NULL;
                    cpi = NULL;
                }
            }
        }

        Boolean providerRunning = cpr != NULL;
        if (providerRunning) {
            cpi = cpr->mInfo;
            CheckTime(startTime, String("getContentProviderImpl: before checkContentProviderPermission"));
            String msg = CheckContentProviderPermissionLocked(cpi, r, userId, checkCrossUser);
            if (!msg.IsNull()) {
                Slogger::E(TAG, msg.string());
                return E_SECURITY_EXCEPTION;
            }
            CheckTime(startTime, String("getContentProviderImpl: after checkContentProviderPermission"));

            if (r != NULL && cpr->CanRunHere(r)) {
                // This provider has been published or is in the process
                // of being published...  but it is also allowed to run
                // in the caller's process, so don't make a connection
                // and just let the caller instantiate its own instance.
                AutoPtr<IContentProviderHolder> holder = cpr->NewHolder(NULL);
                // don't give caller the provider object, it needs
                // to make its own.
                holder->SetContentProvider(NULL);
                *providerHolder = holder;
                REFCOUNT_ADD(*providerHolder);
                return NOERROR;
            }

            const Int64 origId = Binder::ClearCallingIdentity();

            CheckTime(startTime, String("getContentProviderImpl: incProviderCountLocked"));

            // In this case the provider instance already exists, so we can
            // return it right away.
            conn = IncProviderCountLocked(r, cpr, token, stable);
            if (conn != NULL && (conn->mStableCount+conn->mUnstableCount) == 1) {
                if (cpr->mProc != NULL && r->mSetAdj <= ProcessList::PERCEPTIBLE_APP_ADJ) {
                    // If this is a perceptible app accessing the provider,
                    // make sure to count it as being accessed and thus
                    // back up on the LRU list.  This is good because
                    // content providers are often expensive to start.
                    CheckTime(startTime, String("getContentProviderImpl: before updateLruProcess"));
                    UpdateLruProcessLocked(cpr->mProc, FALSE, NULL);
                    CheckTime(startTime, String("getContentProviderImpl: after updateLruProcess"));
                }
            }

            if (cpr->mProc != NULL) {
                String cnflattenString;
                cpr->mName->FlattenToShortString(&cnflattenString);

                if (FALSE) {
                     if (cnflattenString.Equals(
                             String("com.android.providers.calendar/.CalendarProvider2"))) {
                         Slogger::V(TAG, "****************** KILLING %s", cnflattenString.string());
                         Process::KillProcess(cpr->mProc->mPid);
                     }
                 }
                 CheckTime(startTime, String("getContentProviderImpl: before updateOomAdj"));
                 Boolean success = UpdateOomAdjLocked(cpr->mProc);
                 CheckTime(startTime, String("getContentProviderImpl: after updateOomAdj"));
                 if (DEBUG_PROVIDER) Slogger::I(TAG, "Adjust success: %d", success);
                 // NOTE: there is still a race here where a signal could be
                 // pending on the process even though we managed to update its
                 // adj level.  Not sure what to do about this, but at least
                 // the race is now smaller.
                 if (!success) {
                     // Uh oh...  it looks like the provider's process
                     // has been killed on us.  We need to wait for a new
                     // process to be started, and make sure its death
                     // doesn't kill our process.
                     Slogger::I(TAG, "Existing provider %s is crashing; detaching %s",
                        cnflattenString.string(), r->ToString().string());
                     Boolean lastRef = DecProviderCountLocked(conn, cpr, token, stable);
                     CheckTime(startTime, String("getContentProviderImpl: before appDied"));
                     AppDiedLocked(cpr->mProc);
                     CheckTime(startTime, String("getContentProviderImpl: after appDied"));
                     if (!lastRef) {
                        // This wasn't the last ref our process had on
                        // the provider...  we have now been killed, bail.
                        Binder::RestoreCallingIdentity(origId);
                        return NOERROR;
                     }
                     providerRunning = FALSE;
                     conn = NULL;
                 }
            }

            Binder::RestoreCallingIdentity(origId);
        }

        Boolean singleton;
        if (!providerRunning) {
            cpi = NULL;
            CheckTime(startTime, String("getContentProviderImpl: before resolveContentProvider"));
            AppGlobals::GetPackageManager()->ResolveContentProvider(name,
                STOCK_PM_FLAGS | IPackageManager::GET_URI_PERMISSION_PATTERNS, userId,
                (IProviderInfo**)&cpi);
            CheckTime(startTime, String("getContentProviderImpl: after resolveContentProvider"));
            if (cpi == NULL) {
                return NOERROR;
            }

            String cpiProcName, cpiName, cpiPkgName, cpiAppInfoPkgName;
            IComponentInfo* ci = IComponentInfo::Probe(cpi);
            IPackageItemInfo* pi = IPackageItemInfo::Probe(cpi);
            ci->GetProcessName(&cpiProcName);
            pi->GetPackageName(&cpiPkgName);
            pi->GetName(&cpiName);
            Int32 flags, uid;
            cpi->GetFlags(&flags);
            AutoPtr<IApplicationInfo> cpiAppInfo;
            ci->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
            IPackageItemInfo::Probe(cpiAppInfo)->GetPackageName(&cpiAppInfoPkgName);
            cpiAppInfo->GetUid(&uid);

            // If the provider is a singleton AND
            // (it's a call within the same user || the provider is a
            // privileged app)
            // Then allow connecting to the singleton provider
            Boolean isSingleton;
            FAIL_RETURN(IsSingleton(cpiProcName, cpiAppInfo, cpiName, flags, &isSingleton));
            singleton = isSingleton && IsValidSingletonCall(r->mUid, uid);;
            if (singleton) {
                userId = IUserHandle::USER_OWNER;
            }
            ci->SetApplicationInfo(GetAppInfoForUser(cpiAppInfo, userId));
            CheckTime(startTime, String("getContentProviderImpl: got app info for user"));

            CheckTime(startTime, String("getContentProviderImpl: before checkContentProviderPermission"));
            String msg = CheckContentProviderPermissionLocked(cpi, r, userId, !singleton);
            if (!msg.IsNull()) {
               Slogger::E(TAG, msg.string());
               return E_SECURITY_EXCEPTION;
            }
            CheckTime(startTime, String("getContentProviderImpl: after checkContentProviderPermission"));

            if (!mProcessesReady && !mDidUpdate && !mWaitingUpdate
                    && !cpiProcName.Equals("system")) {
                // If this content provider does not run in the system
                // process, and the system is not yet ready to run other
                // processes, then fail fast instead of hanging.
                Slogger::E(TAG, "Attempt to launch content provider before system ready");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // Make sure that the user who owns this provider is running.  If not,
            // we don't want to allow it to run.
            if (!IsUserRunningLocked(userId, FALSE)) {
                Slogger::W(TAG, "Unable to launch app %s/%d for provider %s:user %d is stopped.",
                        cpiAppInfoPkgName.string(), uid, name.string(), userId);
                return NOERROR;
            }

            AutoPtr<IComponentName> comp;
            CComponentName::New(cpiPkgName, cpiName, (IComponentName**)&comp);
            CheckTime(startTime, String("getContentProviderImpl: before getProviderByClass"));
            cpr = mProviderMap->GetProviderByClass(comp, userId);
            CheckTime(startTime, String("getContentProviderImpl: after getProviderByClass"));
            Boolean firstClass = cpr == NULL;
            if (firstClass) {
                Int64 ident = Binder::ClearCallingIdentity();
                CheckTime(startTime, String("getContentProviderImpl: before getApplicationInfo"));
                AutoPtr<IApplicationInfo> ai;
                AppGlobals::GetPackageManager()->GetApplicationInfo(
                        cpiAppInfoPkgName, STOCK_PM_FLAGS, userId, (IApplicationInfo**)&ai);
                CheckTime(startTime, String("getContentProviderImpl: after getApplicationInfo"));
                if (ai == NULL) {
                    Slogger::W(TAG, "No package info for content provider %s", cpiName.string());
                    Binder::RestoreCallingIdentity(ident);
                    return NOERROR;
                }
                ai = GetAppInfoForUser(ai, userId);
                cpr = new ContentProviderRecord(this, cpi, ai, comp, singleton);
                Binder::RestoreCallingIdentity(ident);
            }

            CheckTime(startTime, String("getContentProviderImpl: now have ContentProviderRecord"));

            if (r != NULL && cpr->CanRunHere(r)) {
                // If this is a multiprocess provider, then just return its
                // info and allow the caller to instantiate it.  Only do
                // this if the provider is the same user as the caller's
                // process, or can run as root (so can be in any process).
                AutoPtr<IContentProviderHolder> holder = cpr->NewHolder(NULL);
                *providerHolder = holder.Get();
                REFCOUNT_ADD(*providerHolder);
                return NOERROR;
            }

            if (DEBUG_PROVIDER) {
                Int32 cprUid;
                cpr->mAppInfo->GetUid(&cprUid);
                String cprName;
                IPackageItemInfo::Probe(cpr->mAppInfo)->GetName(&cprName);
                Slogger::W(TAG, "launching remote provider (myuid %d pruid %d): %s",
                    (r != NULL ? r->mUid : NULL), cprUid, cprName.string());
            }

            // This is single process, and our app is now connecting to it.
            // See if we are already in the process of launching this
            // provider.
            List<AutoPtr<ContentProviderRecord> >::Iterator it = mLaunchingProviders.Begin();
            for (; it != mLaunchingProviders.End(); ++it) {
                if (*it == cpr) {
                    break;
                }
            }
            // If the provider is not already being launched, then get it
            // started.
            if (it == mLaunchingProviders.End()) {
                const Int64 origId = Binder::ClearCallingIdentity();

                CheckTime(startTime, String("getContentProviderImpl: before set stopped state"));
                AppGlobals::GetPackageManager()->SetPackageStoppedState(
                        cpiAppInfoPkgName, FALSE, userId);
                CheckTime(startTime, String("getContentProviderImpl: after set stopped state"));

                // Use existing process if already started
                CheckTime(startTime, String("getContentProviderImpl: looking for process record"));
                AutoPtr<ProcessRecord> proc = GetProcessRecordLocked(cpiProcName, uid, FALSE);
                if (proc != NULL && proc->mThread != NULL) {
                    if (DEBUG_PROVIDER) {
                        Slogger::D(TAG, "Installing in existing process %s", TO_CSTR(proc));
                    }
                    if (proc->mPubProviders.Find(cpiName) == proc->mPubProviders.End()) {
                        CheckTime(startTime, String("getContentProviderImpl: scheduling install"));
                        proc->mPubProviders[cpiName] = cpr;
                        proc->mThread->ScheduleInstallProvider(cpi);
                    }
                }
                else {
                    CheckTime(startTime, String("getContentProviderImpl: before start process"));
                    AutoPtr<IComponentName> component;
                    CComponentName::New(cpiAppInfoPkgName, cpiName, (IComponentName**)&component);
                    proc = StartProcessLocked(cpiProcName,
                        cpr->mAppInfo, FALSE, 0, String("content provider"),
                        component, FALSE, FALSE, FALSE);
                    CheckTime(startTime, String("getContentProviderImpl: after start process"));
                    if (proc == NULL) {
                        Slogger::W(TAG, "Unable to launch app %s/%d for provider %s: process is bad",
                            cpiAppInfoPkgName.string(), uid, name.string());
                        Binder::RestoreCallingIdentity(origId);
                        return NOERROR;
                    }
                }

                cpr->mLaunchingApp = proc;
                mLaunchingProviders.PushBack(cpr);
                Binder::RestoreCallingIdentity(origId);
            }

            CheckTime(startTime, String("getContentProviderImpl: updating data structures"));

            // Make sure the provider is published (the same provider class
            // may be published under multiple names).
            if (firstClass) {
                mProviderMap->PutProviderByClass(comp, cpr);
            }

            mProviderMap->PutProviderByName(name, cpr);
            conn = IncProviderCountLocked(r, cpr, token, stable);
            if (conn != NULL) {
                conn->mWaiting = TRUE;
            }
        }
        CheckTime(startTime, String("getContentProviderImpl: done!"));
    }

    // Wait for the provider to be published...
    {
        AutoPtr<IThread> thread;
        Thread::Attach((IThread**)&thread);
        cpr->Lock();
        IComponentInfo* ci = IComponentInfo::Probe(cpi);
        String pkgName;
        Int32 uid;

        while (cpr->mProvider == NULL) {
            if (cpr->mLaunchingApp == NULL) {
                cpr->Unlock();

                AutoPtr<IApplicationInfo> cpiAppInfo;
                ci->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
                IPackageItemInfo::Probe(cpiAppInfo)->GetPackageName(&pkgName);
                cpiAppInfo->GetUid(&uid);
                String callerDes;
                IBinder::Probe(caller)->ToString(&callerDes);
                Slogger::W(TAG, "Unable to launch app %s/%d for provider [%s] caller [%s]: launching app became null",
                    pkgName.string(), uid, name.string(), callerDes.string());
                *providerHolder = NULL;
                return E_LAUNCH_EXCEPTION;
            }
            if (DEBUG_MU) {
                Slogger::V(TAG_MU, "Waiting to start provider %s launchingApp= %s",
                    cpr->ToString().string(), cpr->mLaunchingApp->ToString().string());
            }
            if (conn != NULL) {
                conn->mWaiting = TRUE;
            }
            cpr->Wait();
            if (conn != NULL) {
                conn->mWaiting = FALSE;
            }
        }

        cpr->Unlock();
    }
    AutoPtr<IContentProviderHolder> holder = cpr != NULL ? cpr->NewHolder(conn) : NULL;
    *providerHolder = holder.Get();
    REFCOUNT_ADD(*providerHolder);
    return NOERROR;
}

ECode CActivityManagerService::GetContentProvider(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean stable,
    /* [out] */ IContentProviderHolder** providerHolder)
{
    VALIDATE_NOT_NULL(providerHolder);
    *providerHolder = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getContentProvider")));
    if (caller == NULL) {
        Slogger::W(TAG, "null IApplicationThread when getting content provider %s", name.string());
        return E_SECURITY_EXCEPTION;
    }

    // The incoming user check is now handled in checkContentProviderPermissionLocked() to deal
    // with cross-user grant.
    return GetContentProviderImpl(caller, name, NULL, stable, userId, providerHolder);
}

ECode CActivityManagerService::GetContentProviderExternal(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ IBinder* token,
    /* [out] */ IContentProviderHolder** providerHolder)
{
    VALIDATE_NOT_NULL(providerHolder);
    *providerHolder = NULL;

    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::ACCESS_CONTENT_PROVIDERS_EXTERNALLY,
            String("Do not have permission in call getContentProviderExternal()")));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, ALLOW_FULL_ONLY, String("getContentProvider"), String(NULL), &userId));
    return GetContentProviderExternalUnchecked(name, token, userId, providerHolder);
}

ECode CActivityManagerService::GetContentProviderExternalUnchecked(
    /* [in] */ const String& name,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 userId,
    /* [out] */ IContentProviderHolder** providerHolder)
{
    VALIDATE_NOT_NULL(providerHolder);
    return GetContentProviderImpl(NULL, name, token, TRUE, userId, providerHolder);
}

/**
 * Drop a content provider from a ProcessRecord's bookkeeping
 */
ECode CActivityManagerService::RemoveContentProvider(
    /* [in] */ IBinder* connection,
    /* [in] */ Boolean stable)
{
    VALIDATE_NOT_NULL(connection);
    FAIL_RETURN(EnforceNotIsolatedCaller(String("removeContentProvider")));
    Int64 ident = Binder::ClearCallingIdentity();
    {
        AutoLock lock(this);
        AutoPtr<CContentProviderConnection> conn = (CContentProviderConnection*)connection;
        if (conn == NULL) {
            String conDes;
            connection->ToString(&conDes);
            Slogger::W(TAG, "removeContentProvider: %s not a CContentProviderConnection",
                    conDes.string());
            //throw new IllegalArgumentException(msg);
            Binder::RestoreCallingIdentity(ident);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (DecProviderCountLocked(conn, NULL, NULL, stable)) {
            UpdateOomAdjLocked();
        }
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CActivityManagerService::RemoveContentProviderExternal(
    /* [in] */ const String& name,
    /* [in] */ IBinder* token)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::ACCESS_CONTENT_PROVIDERS_EXTERNALLY,
           String("Do not have permission in call removeContentProviderExternal()")));
    Int32 userId = UserHandle::GetCallingUserId();
    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = RemoveContentProviderExternalUnchecked(name, token, userId);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CActivityManagerService::RemoveContentProviderExternalUnchecked(
    /* [in] */ const String& name,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 userId)
{
    VALIDATE_NOT_NULL(token);

    AutoLock lock(this);

    AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByName(name, userId);
    if (cpr == NULL) {
        //remove from mProvidersByClass
        if(localLOGV) {
            Slogger::V(TAG, "%s content provider not found in providers list", name.string());
        }
        return NOERROR;
    }

    //update content provider record entry info
    String cprPkgName, cprName;
    IPackageItemInfo::Probe(cpr->mInfo)->GetPackageName(&cprPkgName);
    IPackageItemInfo::Probe(cpr->mInfo)->GetName(&cprName);
    AutoPtr<IComponentName> comp;
    CComponentName::New(cprPkgName, cprName, (IComponentName**)&comp);

    AutoPtr<ContentProviderRecord> localCpr = mProviderMap->GetProviderByClass(comp, userId);
    if (localCpr->HasExternalProcessHandles()) {
        if (localCpr->RemoveExternalProcessHandleLocked(token)) {
            return UpdateOomAdjLocked();
        }
        else {
            String tokenDes;
            token->ToString(&tokenDes);
            Slogger::E(TAG, "Attmpt to remove content provider %s with no external reference for token: %s.",
                    localCpr->ToString().string(), tokenDes.string());
        }
    }
    else {
        Slogger::E(TAG, "Attmpt to remove content provider: %s with no external references.",
                localCpr->ToString().string());
    }
    return NOERROR;
}

ECode CActivityManagerService::PublishContentProviders(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IList* providers)
{
    if (providers == NULL) {
        return NOERROR;
    }

    FAIL_RETURN(EnforceNotIsolatedCaller(String("publishContentProviders")));
    AutoLock lock(this);

    AutoPtr<ProcessRecord> r = GetRecordForAppLocked(caller);
    if (DEBUG_MU) {
        Slogger::V(TAG_MU, "ProcessRecord uid = %d", r->mUid);
    }
    if (r == NULL) {
        String callerDes;
        IBinder::Probe(caller)->ToString(&callerDes);
        Slogger::W(TAG, "Unable to find app for caller %s (pid=%d) when publishing content providers",
                callerDes.string(), Binder::GetCallingPid());
        return E_SECURITY_EXCEPTION;
    }

    const Int64 origId = Binder::ClearCallingIdentity();
    // for dst->Notify();
    AutoPtr<IThread> thread;
    Thread::Attach((IThread**)&thread);

    Int32 N;
    providers->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> item;
        providers->Get(i, (IInterface**)&item);
        AutoPtr<IContentProviderHolder> src = IContentProviderHolder::Probe(item);
        if (src == NULL) {
            continue;
        }
        AutoPtr<IProviderInfo> srcProviderInfo;
        src->GetProviderInfo((IProviderInfo**)&srcProviderInfo);
        AutoPtr<IIContentProvider> srcProvider;
        src->GetContentProvider((IIContentProvider**)&srcProvider);
        if (srcProviderInfo == NULL || srcProvider == NULL) {
            continue;
        }

        String srcName;
        IPackageItemInfo::Probe(srcProviderInfo)->GetName(&srcName);
        AutoPtr<ContentProviderRecord> dst;
        HashMap<String, AutoPtr<ContentProviderRecord> >::Iterator it =
                r->mPubProviders.Find(srcName);
        if (it != r->mPubProviders.End()) {
            dst = it->mSecond;
        }
        if (DEBUG_MU) {
            Slogger::V(TAG_MU, "ContentProviderRecord uid = %d", dst->mUid);
        }
        if (dst != NULL) {
            String dstPkgName, dstName;
            IPackageItemInfo::Probe(dst->mInfo)->GetPackageName(&dstPkgName);
            IPackageItemInfo::Probe(dst->mInfo)->GetName(&dstName);
            AutoPtr<IComponentName> comp;
            CComponentName::New(dstPkgName, dstName, (IComponentName**)&comp);
            mProviderMap->PutProviderByClass(comp, dst);
            String dstAuth;
            dst->mInfo->GetAuthority(&dstAuth);
            AutoPtr< ArrayOf<String> > names;
            StringUtils::Split(dstAuth, String(";"), (ArrayOf<String>**)&names);
            for (Int32 i = 0; i < names->GetLength(); ++i) {
                mProviderMap->PutProviderByName((*names)[i], dst);
            }

            List< AutoPtr<ContentProviderRecord> >::Iterator it = mLaunchingProviders.Begin();
            while (it != mLaunchingProviders.End()) {
                if (*it == dst) {
                    it = mLaunchingProviders.Erase(it);
                }
                else ++it;
            }
            dst->Lock();
            dst->mProvider = srcProvider;
            dst->mProc = r;
            dst->NotifyAll();
            dst->Unlock();
            UpdateOomAdjLocked(r);
        }
    }

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::RefContentProvider(
    /* [in] */ IBinder* connection,
    /* [in] */ Int32 stable,
    /* [in] */ Int32 unstable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<CContentProviderConnection> conn;
    // try {
    conn = (CContentProviderConnection*)connection;
    // } catch (ClassCastException e) {
    //     String msg ="refContentProvider: " + connection
    //             + " not a CContentProviderConnection";
    //     Slogger::W(TAG, msg);
    //     throw new IllegalArgumentException(msg);
    // }
    if (conn == NULL) {
        // throw new NullPointerException("connection is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    if (stable > 0) {
        conn->mNumStableIncs += stable;
    }
    stable = conn->mStableCount + stable;
    if (stable < 0) {
        // throw new IllegalStateException("stableCount < 0: " + stable);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (unstable > 0) {
        conn->mNumUnstableIncs += unstable;
    }
    unstable = conn->mUnstableCount + unstable;
    if (unstable < 0) {
        // throw new IllegalStateException("unstableCount < 0: " + unstable);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if ((stable+unstable) <= 0) {
        // throw new IllegalStateException("ref counts can't go to zero here: stable="
        //         + stable + " unstable=" + unstable);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    conn->mStableCount = stable;
    conn->mUnstableCount = unstable;
    *result = !conn->mDead;
    return NOERROR;
}

ECode CActivityManagerService::UnstableProviderDied(
    /* [in] */ IBinder* connection)
{
    AutoPtr<CContentProviderConnection> conn;
    // try {
    conn = (CContentProviderConnection*)connection;
    // } catch (ClassCastException e) {
    //     String msg ="refContentProvider: " + connection
    //             + " not a CContentProviderConnection";
    //     Slogger::W(TAG, msg);
    //     throw new IllegalArgumentException(msg);
    // }
    if (conn == NULL) {
        // throw new NullPointerException("connection is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    // Safely retrieve the content provider associated with the connection.
    AutoPtr<IIContentProvider> provider;
    {
        AutoLock lock(this);
        provider = conn->mProvider->mProvider;
    }

    if (provider == NULL) {
        // Um, yeah, we're way ahead of you.
        return NOERROR;
    }

//TODO:
//     // Make sure the caller is being honest with us.
//     if (provider.asBinder().pingBinder()) {
//         // Er, no, still looks good to us.
//         {
//             AutoLock lock(this);
//             Slogger::W(TAG, "unstableProviderDied: caller %d says %p died, but we don't agree", Binder::GetCallingUid(),
//                     conn.Get());
//             return NOERROR;
//         }
//     }

    // Well look at that!  It's dead!
    AutoLock lock(this);
    if (conn->mProvider->mProvider != provider) {
        // But something changed...  good enough.
        return NOERROR;
    }

    AutoPtr<ProcessRecord> proc = conn->mProvider->mProc;
    if (proc == NULL || proc->mThread == NULL) {
        // Seems like the process is already cleaned up.
        return NOERROR;
    }

    // As far as we're concerned, this is just like receiving a
    // death notification...  just a bit prematurely.
    Slogger::I(TAG, "Process %s (pid %d) early provider death", proc->mProcessName.string(), proc->mPid);
    const Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AppDiedLocked(proc);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::AppNotRespondingViaProvider(
    /* [in] */ IBinder* connection)
{
    FAIL_RETURN(EnforceCallingPermission(
        Manifest::permission::REMOVE_TASKS, String("appNotRespondingViaProvider()")));

    AutoPtr<CContentProviderConnection> conn = (CContentProviderConnection*)connection;
    if (conn == NULL) {
        Slogger::W(TAG, "ContentProviderConnection is null");
        return NOERROR;;
    }

    AutoPtr<ProcessRecord> host = conn->mProvider->mProc;
    if (host == NULL) {
        Slogger::W(TAG, "Failed to find hosting ProcessRecord");
        return NOERROR;
    }

    Int64 token = Binder::ClearCallingIdentity();
    AppNotResponding(host, NULL, NULL, FALSE, String("ContentProvider not responding"));
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CActivityManagerService::InstallSystemProviders()
{
    AutoPtr<IList> providers;
    {
        AutoLock lock(this);
        AutoPtr<ProcessRecord> app = mProcessNames->Get(String("system"), IProcess::SYSTEM_UID);
        providers = GenerateApplicationProvidersLocked(app);
        if (providers != NULL) {
            Int32 size, flags;
            String name;
            providers->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> item;
                providers->Get(i, (IInterface**)&item);
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(item)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                appInfo->GetFlags(&flags);
                IPackageItemInfo::Probe(item)->GetName(&name);
                if ((flags&IApplicationInfo::FLAG_SYSTEM) == 0) {
                    Slogger::W(TAG, "Not installing system proc provider %s: not system .apk", name.string());
                    providers->Remove(item);
                }
                else {
                    Slogger::I(TAG, "installing system proc provider %s", name.string());
                }
            }
        }
    }

    if (providers != NULL) {
        mSystemThread->InstallSystemProviders(providers);
    }

    mCoreSettingsObserver = new CoreSettingsObserver();
    mCoreSettingsObserver->constructor(this);

    // TODO
    // mUsageStatsService->MonitorPackages();
    return NOERROR;
}

/**
 * Allows apps to retrieve the MIME type of a URI.
 * If an app is in the same user as the ContentProvider, or if it is allowed to interact across
 * users, then it does not need permission to access the ContentProvider.
 * Either, it needs cross-user uri grants.
 *
 * CTS tests for this functionality can be run with "runtest cts-appsecurity".
 *
 * Test cases are at cts/tests/appsecurity-tests/test-apps/UsePermissionDiffCert/
 *     src/com/android/cts/usespermissiondiffcertapp/AccessPermissionWithDiffSigTest.java
 */
ECode CActivityManagerService::GetProviderMimeType(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = NULL;
    VALIDATE_NOT_NULL(uri);

    String nullStr;
    String funcName("getProviderMimeType");
    FAIL_RETURN(EnforceNotIsolatedCaller(funcName));
    String name;
    uri->GetAuthority(&name);
    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    Int64 ident = 0;
    Boolean clearedIdentity = FALSE;
    userId = UnsafeConvertIncomingUser(userId);
    if (CanClearIdentity(callingPid, callingUid, userId)) {
        clearedIdentity = TRUE;
        ident = Binder::ClearCallingIdentity();
    }
    AutoPtr<IContentProviderHolder> holder;

    if (SUCCEEDED(GetContentProviderExternalUnchecked(name, NULL, userId,
            (IContentProviderHolder**)&holder))) {
        if (holder != NULL) {
            AutoPtr<IIContentProvider> provider;
            holder->GetContentProvider((IIContentProvider**)&provider);
            provider->GetType(uri, type);
            if (!clearedIdentity) {
                ident = Binder::ClearCallingIdentity();
            }
            RemoveContentProviderExternalUnchecked(name, NULL, userId);
            Binder::RestoreCallingIdentity(ident);
            return NOERROR;
        }
    }
    else {
        Slogger::W(TAG, "Content provider dead retrieving %s", TO_CSTR(uri));
    }

    // We need to clear the identity to call removeContentProviderExternalUnchecked
    if (!clearedIdentity) {
        ident = Binder::ClearCallingIdentity();
    }

    if (holder != NULL) {
        RemoveContentProviderExternalUnchecked(name, NULL, userId);
    }

    Binder::RestoreCallingIdentity(ident);
    *type = NULL;
    return NOERROR;
}

Boolean CActivityManagerService::CanClearIdentity(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId)
{
    if (UserHandle::GetUserId(callingUid) == userId) {
        return TRUE;
    }
    if (CheckComponentPermission(Manifest::permission::INTERACT_ACROSS_USERS, callingPid,
        callingUid, -1, TRUE) == IPackageManager::PERMISSION_GRANTED
        || CheckComponentPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL, callingPid,
        callingUid, -1, TRUE) == IPackageManager::PERMISSION_GRANTED) {
            return TRUE;
    }
    return FALSE;
}

// =========================================================
// GLOBAL MANAGEMENT
// =========================================================

AutoPtr<ProcessRecord> CActivityManagerService::NewProcessRecordLocked(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ const String& customProcess,
    /* [in] */ Boolean isolated,
    /* [in] */ Int32 isolatedUid)
{
    String proc;
    if (!customProcess.IsNull()) {
        proc = customProcess;
    }
    else {
        info->GetProcessName(&proc);
    }

    Int32 uid;
    info->GetUid(&uid);
    if (isolated) {
        if (isolatedUid == 0) {
            Int32 userId = UserHandle::GetUserId(uid);
            Int32 stepsLeft = IProcess::LAST_ISOLATED_UID - IProcess::FIRST_ISOLATED_UID + 1;

            while (TRUE) {
                if (mNextIsolatedProcessUid < IProcess::FIRST_ISOLATED_UID
                        || mNextIsolatedProcessUid > IProcess::LAST_ISOLATED_UID) {
                    mNextIsolatedProcessUid = IProcess::FIRST_ISOLATED_UID;
                }
                uid = UserHandle::GetUid(userId, mNextIsolatedProcessUid);
                mNextIsolatedProcessUid++;

                if (mIsolatedProcesses.Find(uid) == mIsolatedProcesses.End()) {
                    // No process for this uid, use it.
                    break;
                }
                stepsLeft--;
                if (stepsLeft <= 0) {
                    Slogger::W(TAG, " NewProcessRecordLocked return null");
                    return NULL;
                }
            }
        }
        else {
            // Special case for startIsolatedProcess (internal only), where
            // the uid of the isolated process is specified by the caller.
            uid = isolatedUid;
        }
    }

    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    AutoPtr<ProcessRecord> result = new ProcessRecord(stats, info, proc, uid);
    return result;
}

AutoPtr<ProcessRecord> CActivityManagerService::AddAppLocked(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean isolated,
    /* [in] */ const String& abiOverride)
{
    String pname, pkgName;
    info->GetProcessName(&pname);
    IPackageItemInfo::Probe(info)->GetPackageName(&pkgName);
    Int32 uid;
    info->GetUid(&uid);
    AutoPtr<ProcessRecord> app;
    if (!isolated) {
        app = GetProcessRecordLocked(pname, uid, TRUE);
    }
    else {
        app = NULL;
    }

    if (app == NULL) {
        String nullStr;
        app = NewProcessRecordLocked(info, nullStr, isolated, 0);
        mProcessNames->Put(pname, app->mUid, app);
        if (isolated) {
            mIsolatedProcesses[app->mUid] = app;
        }
        UpdateLruProcessLocked(app, FALSE, NULL);
        UpdateOomAdjLocked();
    }
    // This package really, really can not be stopped.
//    try {
        ECode ec = AppGlobals::GetPackageManager()->SetPackageStoppedState(
                pkgName, FALSE, UserHandle::GetUserId(app->mUid));
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            Slogger::W(TAG, "Failed trying to unstop package %s : %08x",
                    pkgName.string(), ec);
        }
//    } catch (RemoteException e) {
//    } catch (IllegalArgumentException e) {
//        Slogger::W(TAG, "Failed trying to unstop package "
//                + info.packageName + ": " + e);
//    }

    Int32 flags;
    info->GetFlags(&flags);
    if ((flags & (IApplicationInfo::FLAG_SYSTEM | IApplicationInfo::FLAG_PERSISTENT))
            == (IApplicationInfo::FLAG_SYSTEM | IApplicationInfo::FLAG_PERSISTENT)) {
        app->mPersistent = TRUE;

        // The Adj score defines an order of processes to be killed.
        // If a process is shared by multiple apps, maxAdj must be set by the highest
        // prioritized app to avoid being killed.
        if (app->mMaxAdj >= ProcessList::PERSISTENT_PROC_ADJ) {
            app->mMaxAdj = ProcessList::PERSISTENT_PROC_ADJ;
        }
    }

    Int32 index;
    if (app->mThread == NULL &&
        (mPersistentStartingProcesses->IndexOf((IProcessRecord*)app.Get(), &index), index < 0)) {
        mPersistentStartingProcesses->Add((IProcessRecord*)app.Get());
        StartProcessLocked(app, String("added application"), app->mProcessName, abiOverride,
                String(NULL) /* entryPoint */, NULL /* entryPointArgs */);
    }

    return app;
}

ECode CActivityManagerService::UnhandledBack()
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::FORCE_BACK,
           String("unhandledBack()")));

    AutoLock lock(this);
    const Int64 origId = Binder::ClearCallingIdentity();
    GetFocusedStack()->UnhandledBackLocked();
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::OpenContentUri(
    /* [in] */ IUri* uri,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("openContentUri")));
    const Int32 userId = UserHandle::GetCallingUserId();
    String name;
    uri->GetAuthority(&name);
    AutoPtr<IContentProviderHolder> cph;
    GetContentProviderExternalUnchecked(name, NULL, userId, (IContentProviderHolder**)&cph);
    AutoPtr<IParcelFileDescriptor> pfd;
    if (cph != NULL) {
        // We record the binder invoker's uid in thread-local storage before
        // going to the content provider to open the file.  Later, in the code
        // that handles all permissions checks, we look for this uid and use
        // that rather than the Activity Manager's own uid.  The effect is that
        // we do the check against the caller's permissions even though it looks
        // to the content provider like the Activity Manager itself is making
        // the request.
        Identity* oldIdentity = (Identity*)pthread_getspecific(sCallerIdentity);
        if (oldIdentity != NULL) oldIdentity->Release();
        AutoPtr<Identity> identity = new Identity(Binder::GetCallingPid(), Binder::GetCallingUid());
        pthread_setspecific(sCallerIdentity, identity.Get());
        identity->AddRef();
//        try {
        AutoPtr<IIContentProvider> contentProvider;
        cph->GetContentProvider((IIContentProvider**)&contentProvider);
        contentProvider->OpenFile(String(NULL), uri, String("r"), NULL, (IParcelFileDescriptor**)&pfd);
//        } catch (FileNotFoundException e) {
//            // do nothing; pfd will be returned null
//        } finally {
        // Ensure that whatever happens, we clean up the identity state
        pthread_setspecific(sCallerIdentity, NULL);
        identity->Release();
        // Ensure we're done with the provider.
        RemoveContentProviderExternalUnchecked(name, NULL, userId);
//        }
    }
    else {
        Slogger::D(TAG, "Failed to get provider for authority '%s'", name.string());
    }
    *result = pfd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// Actually is sleeping or shutting down or whatever else in the future
// is an inactive state.
Boolean CActivityManagerService::IsSleepingOrShuttingDown()
{
    return IsSleeping() || mShuttingDown;
}

// Actually is sleeping or shutting down or whatever else in the future
// is an inactive state.
Boolean CActivityManagerService::IsSleeping()
{
    return mSleeping;
}

ECode CActivityManagerService::GoingToSleep()
{
    {    AutoLock syncLock(this);
        mWentToSleep = TRUE;
        GoToSleepIfNeededLocked();
    }
    return NOERROR;
}


ECode CActivityManagerService::FinishRunningVoiceLocked()
{
    if (mRunningVoice) {
        mRunningVoice = FALSE;
        GoToSleepIfNeededLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::GoToSleepIfNeededLocked()
{
    if (mWentToSleep && !mRunningVoice) {
        if (!mSleeping) {
            mSleeping = TRUE;
            mStackSupervisor->GoingToSleepLocked();

            // Initialize the wake times of all processes.
            CheckExcessivePowerUsageLocked(FALSE);

            mHandler->RemoveMessages(CHECK_EXCESSIVE_WAKE_LOCKS_MSG);
            Boolean result;
            mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::NotifyTaskPersisterLocked(
    /* [in]*/ TaskRecord* task,
    /* [in]*/ Boolean flush)
{
    if (task != NULL && task->mStack != NULL && task->mStack->IsHomeStack()) {
        // Never persist the home stack.
        return NOERROR;
    }
    mTaskPersister->Wakeup(task, flush);
    return NOERROR;
}

ECode CActivityManagerService::Shutdown(
    /* [in]*/ Int32 timeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckCallingPermission(Manifest::permission::SHUTDOWN)
          != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::SHUTDOWN);
        return E_SECURITY_EXCEPTION;
    }

    Boolean timedout = FALSE;

    {
        AutoLock lock(this);
        mShuttingDown = TRUE;
        UpdateEventDispatchingLocked();
        timedout = mStackSupervisor->ShutdownLocked(timeout);
    }

    mAppOpsService->Shutdown();
    if (mUsageStatsService != NULL) {
        mUsageStatsService->PrepareShutdown();
    }
    mBatteryStatsService->Shutdown();
    {    AutoLock syncLock(this);
        mProcessStats->ShutdownLocked();
    }
    NotifyTaskPersisterLocked(NULL, TRUE);

    *result = timedout;
    return NOERROR;
}

ECode CActivityManagerService::ActivitySlept(
    /* [in] */ IBinder* token)
{
    if (localLOGV) {
        String info;
        token->ToString(&info);
        Slogger::V(TAG, "Activity slept: token= %s", info.string());
    }

    const Int64 origId = Binder::ClearCallingIdentity();

    {
        AutoLock lock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            mStackSupervisor->ActivitySleptLocked(r);
        }
    }

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::LogLockScreen(
    /* [in] */ const String& msg)
{
    if (DEBUG_LOCKSCREEN) {
        // AutoPtr<IDebug> debug;
        // assert(0 && "TODO");
        // // CDebug::AcquireSingleton((IDebug**)&debug);
        // String callers;
        // // debug->GetCallers(2, &callers);
        // Slogger::D(TAG, "%s:%s mLockScreenShown=%d mWentToSleep=%d mSleeping=%d",
        //     callers.string(), msg.string(), mLockScreenShown, mWentToSleep, mSleeping);
    }
    return NOERROR;
}

ECode CActivityManagerService::ComeOutOfSleepIfNeededLocked()
{
    if ((!mWentToSleep && !mLockScreenShown) || mRunningVoice) {
        if (mSleeping) {
            mSleeping = FALSE;
            mStackSupervisor->ComeOutOfSleepIfNeededLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::WakingUp()
{
    {    AutoLock syncLock(this);
        mWentToSleep = FALSE;
        ComeOutOfSleepIfNeededLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::StartRunningVoiceLocked()
{
    if (!mRunningVoice) {
        mRunningVoice = TRUE;
        ComeOutOfSleepIfNeededLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::UpdateEventDispatchingLocked()
{
    mWindowManager->SetEventDispatching(mBooted && !mShuttingDown);
    return NOERROR;
}

ECode CActivityManagerService::SetLockScreenShown(
    /* [in] */ Boolean shown)
{
    if (CheckCallingPermission(Manifest::permission::DEVICE_POWER)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::DEVICE_POWER);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    Int64 ident = Binder::ClearCallingIdentity();
    if (DEBUG_LOCKSCREEN) {
        StringBuilder sb("shown=");
        sb += shown;
        LogLockScreen(sb.ToString());
    }
    mLockScreenShown = shown;
    ComeOutOfSleepIfNeededLocked();
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CActivityManagerService::StopAppSwitches()
{
    if (CheckCallingPermission(Manifest::permission::STOP_APP_SWITCHES)
           != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::STOP_APP_SWITCHES);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    mAppSwitchesAllowedTime = SystemClock::GetUptimeMillis() + APP_SWITCH_DELAY_TIME;
    mDidAppSwitch = FALSE;

    Boolean result;
    mHandler->RemoveMessages(DO_PENDING_ACTIVITY_LAUNCHES_MSG);
    return mHandler->SendEmptyMessageDelayed(DO_PENDING_ACTIVITY_LAUNCHES_MSG,
        APP_SWITCH_DELAY_TIME, &result);
}

ECode CActivityManagerService::ResumeAppSwitches()
{
    if (CheckCallingPermission(Manifest::permission::STOP_APP_SWITCHES)
          != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //        + Manifest::permission::STOP_APP_SWITCHES);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    // Note that we don't execute any pending app switches... we will
    // let those wait until either the timeout, or the next start
    // activity request.
    mAppSwitchesAllowedTime = 0;
    return NOERROR;
}

Boolean CActivityManagerService::CheckAppSwitchAllowedLocked(
    /* [in] */ Int32 sourcePid,
    /* [in] */ Int32 sourceUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& name)
{
    if (mAppSwitchesAllowedTime < SystemClock::GetUptimeMillis()) {
        return TRUE;
    }

    Int32 perm = CheckComponentPermission(
            Manifest::permission::STOP_APP_SWITCHES, sourcePid, sourceUid, -1, TRUE);
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }

    // If the actual IPC caller is different from the logical source, then
    // also see if they are allowed to control app switches.
    if (callingUid != -1 && callingUid != sourceUid) {
        perm = CheckComponentPermission(
            Manifest::permission::STOP_APP_SWITCHES, callingPid,
            callingUid, -1, TRUE);
        if (perm == IPackageManager::PERMISSION_GRANTED) {
            return TRUE;
        }
    }

    Slogger::W(TAG, "%s request from %d stopped", name.string(), sourceUid);
    return FALSE;
}

ECode CActivityManagerService::SetDebugApp(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean waitForDebugger,
    /* [in] */ Boolean persistent)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_DEBUG_APP,
           String("setDebugApp()")));

    Int64 ident = Binder::ClearCallingIdentity();
    // Note that this is not really thread safe if there are multiple
    // callers into it at the same time, but that's not a situation we
    // care about.
    if (persistent) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        Boolean result;
        Settings::Global::PutString(
            resolver, ISettingsGlobal::DEBUG_APP,
            packageName, &result);
        Settings::Global::PutInt32(
            resolver, ISettingsGlobal::WAIT_FOR_DEBUGGER,
            waitForDebugger ? 1 : 0, &result);
    }

    AutoLock lock(this);
    if (!persistent) {
        mOrigDebugApp = mDebugApp;
        mOrigWaitForDebugger = mWaitForDebugger;
    }
    mDebugApp = packageName;
    mWaitForDebugger = waitForDebugger;
    mDebugTransient = !persistent;
    if (packageName != NULL) {
        ForceStopPackageLocked(packageName, -1, FALSE, FALSE, TRUE, TRUE,
            FALSE, IUserHandle::USER_ALL, String("set debug app"));
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CActivityManagerService::SetOpenGlTraceApp(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ const String& processName)
{
    AutoLock lock(this);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
    Boolean isDebuggable = value.Equals("1");
    if (!isDebuggable) {
        Int32 flags;
        app->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) == 0) {
            // throw new SecurityException("Process not debuggable: " + app.packageName);
            return E_SECURITY_EXCEPTION;
        }
    }

    mOpenGlTraceApp = processName;
    return NOERROR;
}

ECode CActivityManagerService::SetProfileApp(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ const String& processName,
    /* [in] */ IProfilerInfo* profilerInfo)
{
    AutoLock lock(this);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
    Boolean isDebuggable = value.Equals("1");
    if (!isDebuggable) {
        Int32 flags;
        app->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) == 0) {
            // throw new SecurityException("Process not debuggable: " + app.packageName);
            return E_SECURITY_EXCEPTION;
        }
    }
    mProfileApp = processName;
    profilerInfo->GetProfileFile(&mProfileFile);
    if (mProfileFd != NULL) {
        // try {
        mProfileFd->Close();
        // } catch (IOException e) {
        // }
        mProfileFd = NULL;
    }
    profilerInfo->GetProfileFd((IParcelFileDescriptor**)&mProfileFd);
    profilerInfo->GetSamplingInterval(&mSamplingInterval);
    profilerInfo->IsAutoStopProfiler(&mAutoStopProfiler);
    mProfileType = 0;
    return NOERROR;
}

ECode CActivityManagerService::SetAlwaysFinish(
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_ALWAYS_FINISH,
        String("setAlwaysFinish()")));

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    Settings::Global::PutInt32(resolver,
        ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES, enabled ? 1 : 0, &result);

    AutoLock lock(this);
    mAlwaysFinishActivities = enabled;
    return NOERROR;
}

ECode CActivityManagerService::SetActivityController(
    /* [in] */ IIActivityController* controller)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER,
            String("SetActivityController()")));

    AutoLock lock(this);
    mController = controller;
    Watchdog::GetInstance()->SetActivityController(controller);
    return NOERROR;
}

ECode CActivityManagerService::SetUserIsMonkey(
    /* [in] */ Boolean userIsMonkey)
{
    {    AutoLock syncLock(this);
        {    AutoLock syncLock(mPidsSelfLockedLock);
            Int32 callingPid = Binder::GetCallingPid();
            AutoPtr<ProcessRecord> precessRecord;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(callingPid);
            if (it != mPidsSelfLocked.End()) {
                precessRecord = it->mSecond;
            }
            if (precessRecord == NULL) {
                Slogger::E(TAG, "Unknown process: %d", callingPid);
                return E_SECURITY_EXCEPTION;
            }
            if (precessRecord->mInstrumentationUiAutomationConnection == NULL) {
                Slogger::E(TAG, "Only an instrumentation process "
                        "with a UiAutomation can call setUserIsMonkey");
                return E_SECURITY_EXCEPTION;
            }
        }
        mUserIsMonkey = userIsMonkey;
    }
    return NOERROR;
}

ECode CActivityManagerService::IsUserAMonkey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(this);
    // If there is a controller also implies the user is a monkey.
    *result = (mUserIsMonkey || mController != NULL);
    return NOERROR;
}

ECode CActivityManagerService::RequestBugReport()
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::DUMP,
        String("requestBugReport")));
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Set(String("ctl.start"), String("bugreport"));
    return NOERROR;
}

Int64 CActivityManagerService::GetInputDispatchingTimeoutLocked(
    /* [in] */ ActivityRecord* r)
{
    return r != NULL ? GetInputDispatchingTimeoutLocked(r->mApp) : KEY_DISPATCHING_TIMEOUT;
}

Int64 CActivityManagerService::GetInputDispatchingTimeoutLocked(
    /* [in] */ ProcessRecord* r)
{
    if (r != NULL && (r->mInstrumentationClass != NULL || r->mUsingWrapper)) {
        return INSTRUMENTATION_KEY_DISPATCHING_TIMEOUT;
    }
    return KEY_DISPATCHING_TIMEOUT;
}

ECode CActivityManagerService::InputDispatchingTimedOut(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean aboveSystem,
    /* [in] */ const String& reason,
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    if (CheckCallingPermission(Manifest::permission::FILTER_EVENTS)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::FILTER_EVENTS);
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<ProcessRecord> proc;
    {
        AutoLock lock(this);
        {
            AutoLock lock(mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(pid);
            if (it != mPidsSelfLocked.End()) {
                proc = it->mSecond;
            }
            *timeout = GetInputDispatchingTimeoutLocked(proc);
        }
        if (!InputDispatchingTimedOut(proc, NULL, NULL, aboveSystem, reason)) {
            *timeout =-1;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::InputDispatchingTimedOut(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ ActivityRecord* parent,
    /* [in] */ Boolean aboveSystem,
    /* [in] */ const String& reason)
{
    if (CheckCallingPermission(Manifest::permission::FILTER_EVENTS)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::FILTER_EVENTS);
        return E_SECURITY_EXCEPTION;
    }

    String annotation;
    if (reason == NULL) {
        annotation = "Input dispatching timed out";
    }
    else {
        annotation.AppendFormat("Input dispatching timed out (%s)", reason.string());
    }

    if (proc != NULL) {
        {    AutoLock syncLock(this);
            if (proc->mDebugging) {
                return FALSE;
            }

            if (mDidDexOpt) {
                // Give more time since we were dexopting.
                mDidDexOpt = FALSE;
                return FALSE;
            }

            if (proc->mInstrumentationClass != NULL) {
                AutoPtr<IBundle> info;
                CBundle::New((IBundle**)&info);
                info->PutString(String("shortMsg"), String("keyDispatchingTimedOut"));
                info->PutString(String("longMsg"), annotation);
                FinishInstrumentationLocked(proc, IActivity::RESULT_CANCELED, info);
                return TRUE;
            }
        }
        AutoPtr<InputDispatchingTimedOutRunnable> runnable = new InputDispatchingTimedOutRunnable(
            this, proc, activity, parent, aboveSystem, annotation);
        Boolean res;
        mHandler->Post(runnable, &res);
    }

    return TRUE;
}

ECode CActivityManagerService::GetAssistContextExtras(
    /* [in] */ Int32 requestType,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    AutoPtr<CPendingAssistExtras> pae;
    FAIL_RETURN(EnqueueAssistContext(requestType, NULL, String(NULL),
            UserHandle::GetCallingUserId(), (CPendingAssistExtras**)&pae));
    if (pae == NULL) {
        *bundle = NULL;
        return NOERROR;
    }
    {    AutoLock syncLock(pae);
        while (!pae->mHaveResult) {
            pae->Wait();
        }
        if (pae->mResult != NULL) {
            pae->mExtras->PutBundle(IIntent::EXTRA_ASSIST_CONTEXT, pae->mResult);
        }
    }
    {    AutoLock syncLock(this);
        mPendingAssistExtras.Remove(pae);
        mHandler->RemoveCallbacks(pae);
    }
    *bundle = pae->mExtras;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

ECode CActivityManagerService::EnqueueAssistContext(
    /* [in] */ Int32 requestType,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle,
    /* [out] */ CPendingAssistExtras** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::GET_TOP_ACTIVITY_INFO,
            String("getAssistContextExtras()")));
    AutoPtr<CPendingAssistExtras> pae;
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> activity = GetFocusedStack()->mResumedActivity;
        if (activity == NULL) {
            Slogger::W(TAG, "getAssistContextExtras failed: no resumed activity");
            return NOERROR;
        }
        extras->PutString(IIntent::EXTRA_ASSIST_PACKAGE, activity->mPackageName);
        if (activity->mApp == NULL || activity->mApp->mThread == NULL) {
            Slogger::W(TAG, "getAssistContextExtras failed: no process for %s", TO_CSTR(activity));
            return NOERROR;
        }
        if (activity->mApp->mPid == Binder::GetCallingPid()) {
            Slogger::W(TAG, "getAssistContextExtras failed: request process same as %s", TO_CSTR(activity));
            return NOERROR;
        }
        CPendingAssistExtras::NewByFriend(activity, extras, intent, hint, userHandle,
            (CPendingAssistExtras**)&pae);
        if (FAILED(activity->mApp->mThread->RequestAssistContextExtras(
            IBinder::Probe(activity->mAppToken), pae, requestType))) {
            Slogger::W(TAG, "getAssistContextExtras failed: crash calling %s", TO_CSTR(activity));
            return NOERROR;
        }
        mPendingAssistExtras.PushBack(pae);
        Boolean res;
        mHandler->PostDelayed(pae, PENDING_ASSIST_EXTRAS_TIMEOUT, &res);
        *result = pae;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode CActivityManagerService::ReportAssistContextExtras(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* extras)
{
    CPendingAssistExtras* pae = (CPendingAssistExtras*)token;
    {    AutoLock syncLock(pae);
        pae->mResult = extras;
        pae->mHaveResult = TRUE;
        pae->NotifyAll();
        if (pae->mIntent == NULL) {
            // Caller is just waiting for the result.
            return NOERROR;
        }
    }

    // We are now ready to launch the assist activity.
    {    AutoLock syncLock(this);
        List<AutoPtr<CPendingAssistExtras> >::Iterator it = Find(
            mPendingAssistExtras.Begin(), mPendingAssistExtras.End(), AutoPtr<CPendingAssistExtras>(pae));
        Boolean exists = it != mPendingAssistExtras.End();
        if (exists)
            mPendingAssistExtras.Erase(it);
        mHandler->RemoveCallbacks(pae);
        if (!exists) {
            // Timed out.
            return NOERROR;
        }
    }
    pae->mIntent->ReplaceExtras(extras);
    if (pae->mHint != NULL) {
        pae->mIntent->PutBooleanExtra(pae->mHint, TRUE);
    }
    pae->mIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_SINGLE_TOP
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    CloseSystemDialogs(String("assist"));
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(pae->mUserHandle, (IUserHandle**)&userHandle);
    if (FAILED(mContext->StartActivityAsUser(pae->mIntent, userHandle))) {
        Slogger::W(TAG, "No activity to handle assist action.");
    }
    return NOERROR;
}

ECode CActivityManagerService::LaunchAssistIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestType,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CPendingAssistExtras> extras;
    FAIL_RETURN(EnqueueAssistContext(requestType, intent, hint,
        userHandle, (CPendingAssistExtras**)&extras));
    *result = extras != NULL;
    return NOERROR;
}

ECode CActivityManagerService::RegisterProcessObserver(
    /* [in] */ IIProcessObserver* observer)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER,
            String("registerProcessObserver()")));
    AutoLock lock(this);
    Boolean registed;
    mProcessObservers->Register(observer, &registed);
    return NOERROR;
}

ECode CActivityManagerService::UnregisterProcessObserver(
    /* [in] */ IIProcessObserver* observer)
{
    AutoLock lock(this);
    Boolean unregisted;
    mProcessObservers->Unregister(observer, &unregisted);
    return NOERROR;
}

ECode CActivityManagerService::ConvertFromTranslucent(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r == NULL) {
            *result = FALSE;
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }
        Boolean translucentChanged = r->ChangeWindowTranslucency(TRUE);
        if (translucentChanged) {
            r->mTask->mStack->ReleaseBackgroundResources();
            mStackSupervisor->EnsureActivitiesVisibleLocked(NULL, 0);
        }
        mWindowManager->SetAppFullscreen(token, TRUE);
        *result = translucentChanged;
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ConvertToTranslucent(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityOptions* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r == NULL) {
            *result = FALSE;
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }
        Int32 index;
        r->mTask->mActivities->LastIndexOf((IActivityRecord*)r, &index);
        if (index > 0) {
            AutoPtr<IInterface> obj;
            r->mTask->mActivities->Get(index - 1, (IInterface**)&obj);
            ActivityRecord* under = (ActivityRecord*)IActivityRecord::Probe(obj);
            under->mReturningOptions = options;
        }
        Boolean translucentChanged = r->ChangeWindowTranslucency(FALSE);
        if (translucentChanged) {
            r->mTask->mStack->ConvertToTranslucent(r);
        }
        mStackSupervisor->EnsureActivitiesVisibleLocked(NULL, 0);
        mWindowManager->SetAppFullscreen(token, FALSE);
        *result = translucentChanged;
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::RequestVisibleBehind(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean visible,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            *result = mStackSupervisor->RequestVisibleBehindLocked(r, visible);
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }
    }
    *result = FALSE;
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::IsBackgroundVisibleBehind(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        Boolean visible = stack == NULL ? FALSE : stack->HasVisibleBehindActivity();
        if (ActivityStackSupervisor::DEBUG_VISIBLE_BEHIND)
            Slogger::D(TAG, "isBackgroundVisibleBehind: stack=%s visible=%d",
                TO_CSTR(stack), visible);
        *result = visible;
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::GetActivityOptions(
    /* [in] */ IBinder* token,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options);
    Int64 origId = Binder::ClearCallingIdentity();
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            *options = r->mPendingOptions;
            REFCOUNT_ADD(*options);
            r->mPendingOptions = NULL;
        }
        else
            *options = NULL;
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::SetImmersive(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean immersive)
{
    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r == NULL) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    r->mImmersive = immersive;

    // update associated state if we're frontmost
    if (r == mFocusedActivity) {
        if (DEBUG_IMMERSIVE) {
            Slogger::D(TAG, "Frontmost changed immersion: %s", TO_CSTR(r));
        }
        ApplyUpdateLockStateLocked(r);
    }

    return NOERROR;
}

ECode CActivityManagerService::IsImmersive(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* isImmersived)
{
    VALIDATE_NOT_NULL(isImmersived);

    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
    if (r == NULL) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *isImmersived = r->mImmersive;
    return NOERROR;
}

ECode CActivityManagerService::IsTopActivityImmersive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("startActivity")));
    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = GetFocusedStack()->TopRunningActivityLocked(NULL);
    *result = (r != NULL) ? r->mImmersive : FALSE;
    return NOERROR;
}

ECode CActivityManagerService::IsTopOfTask(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        if (r == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *result = r->mTask->GetTopActivity() == r;
    }
    return NOERROR;
}

ECode CActivityManagerService::EnterSafeMode()
{
    AutoLock lock(this);
    // It only makes sense to do this before the system is ready
    // and started launching other packages.
    if (!mSystemReady) {
        // try {
        AppGlobals::GetPackageManager()->EnterSafeMode();
        // } catch (RemoteException e) {
        // }
    }
    mSafeMode = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::ShowSafeModeOverlay()
{
    AutoPtr<ILayoutInflater> lInflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&lInflater);
    AutoPtr<IView> v;
    lInflater->Inflate(
            R::layout::safe_mode, NULL, (IView**)&v);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&lp);
    lp->SetType(IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY);
    IViewGroupLayoutParams::Probe(lp)->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    IViewGroupLayoutParams::Probe(lp)->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
    lp->SetGravity(IGravity::BOTTOM | IGravity::START);
    AutoPtr<IDrawable> background;
    v->GetBackground((IDrawable**)&background);
    Int32 opacity;
    background->GetOpacity(&opacity);
    lp->SetFormat(opacity);
    lp->SetFlags(IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);
    Int32 pFlags;
    lp->GetPrivateFlags(&pFlags);
    lp->SetPrivateFlags(pFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(
            IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IViewManager> wm = IViewManager::Probe(service);
    wm->AddView(v, IViewGroupLayoutParams::Probe(lp));
    return NOERROR;
}

ECode CActivityManagerService::NoteWakeupAlarm(
    /* [in] */ IIIntentSender* sender,
    /* [in] */ Int32 sourceUid,
    /* [in] */ const String& sourcePkg)
{
    if (IPendingIntentRecord::Probe(sender) == NULL) {
        return NOERROR;
    }

    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();

    {
        AutoLock syncLock(stats);
        if (mBatteryStatsService->IsOnBattery()) {
            mBatteryStatsService->EnforceCallingPermission();
            AutoPtr<CPendingIntentRecord> rec = (CPendingIntentRecord*)sender;
            Int32 MY_UID = Binder::GetCallingUid();
            Int32 uid = rec->mUid == MY_UID ? IProcess::SYSTEM_UID : rec->mUid;
            AutoPtr<IBatteryStatsImplUidPkg> pkg;
            stats->GetPackageStatsLocked(sourceUid >= 0 ? sourceUid : uid,
                sourcePkg != NULL ? sourcePkg : rec->mKey->mPackageName,
                (IBatteryStatsImplUidPkg**)&pkg);
            if (pkg) {
                pkg->IncWakeupsLocked();
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::KillPids(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [in] */ const String& pReason,
    /* [in] */ Boolean secure,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        // throw new SecurityException("killPids only available to the system");
        return E_SECURITY_EXCEPTION;
    }
    String reason = (pReason == NULL) ? String("Unknown") : pReason;
    // XXX Note: don't acquire main activity lock here, because the window
    // manager calls in with its locks held.

    Boolean killed = FALSE;
    {
        AutoLock lock(mPidsSelfLockedLock);
        Int32 pidsCount = pids->GetLength();
        AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(pidsCount);
        Int32 worstType = 0;
        for (Int32 i = 0; i < pidsCount; i++) {
            AutoPtr<ProcessRecord> proc;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find((*pids)[i]);
            if (it != mPidsSelfLocked.End()) {
                proc = it->mSecond;
            }
            if (proc != NULL) {
                Int32 type = proc->mSetAdj;
                (*types)[i] = type;
                if (type > worstType) {
                    worstType = type;
                }
            }
        }

        // If the worst oom_adj is somewhere in the cached proc LRU range,
        // then constrain it so we will kill all cached procs.
        if (worstType < ProcessList::CACHED_APP_MAX_ADJ
                && worstType > ProcessList::CACHED_APP_MIN_ADJ) {
            worstType = ProcessList::CACHED_APP_MIN_ADJ;
        }

        // If this is not a secure call, don't let it kill processes that
        // are important.
        if (!secure && worstType < ProcessList::SERVICE_ADJ) {
            worstType = ProcessList::SERVICE_ADJ;
        }

        Slogger::W(TAG, "Killing processes %s at adjustment %d", reason.string(), worstType);
        for (Int32 i = 0; i < pidsCount; i++) {
            AutoPtr<ProcessRecord> proc;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find((*pids)[i]);
            if (it != mPidsSelfLocked.End()) {
                proc = it->mSecond;
            }
            if (proc == NULL) {
                continue;
            }
            Int32 adj = proc->mSetAdj;
            if (adj >= worstType && !proc->mKilledByAm) {
                proc->Kill(reason, TRUE);
                killed = TRUE;
            }
        }
    }
    *result = killed;
    return NOERROR;
}

ECode CActivityManagerService::KillUid(
    /* [in] */ Int32 uid,
    /* [in] */ const String& reason)
{
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "killUid only available to the system");
        return E_SECURITY_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        KillPackageProcessesLocked(String(NULL), UserHandle::GetAppId(uid), UserHandle::GetUserId(uid),
            ProcessList::FOREGROUND_APP_ADJ - 1, FALSE, TRUE, TRUE, FALSE,
            reason != NULL ? reason : String("kill uid"));
    }
    return NOERROR;
}

ECode CActivityManagerService::KillProcessesBelowForeground(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* killed)
{
    VALIDATE_NOT_NULL(killed);

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "killProcessesBelowForeground() only available to system");
        return E_SECURITY_EXCEPTION;
    }

    return KillProcessesBelowAdj(ProcessList::FOREGROUND_APP_ADJ, reason, killed);
}

ECode CActivityManagerService::KillProcessesBelowAdj(
    /* [in] */ Int32 belowAdj,
    /* [in] */ const String& reason,
    /* [out] */ Boolean* killed)
{
    VALIDATE_NOT_NULL(killed);
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "killProcessesBelowAdj() only available to system");
        return E_SECURITY_EXCEPTION;
    }

    *killed = FALSE;
    {
        AutoLock lock(mPidsSelfLockedLock);
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
            AutoPtr<ProcessRecord> proc = it->mSecond;
            if (proc == NULL) continue;

            const Int32 adj = proc->mSetAdj;
            if (adj > belowAdj && !proc->mKilledByAm) {
                proc->Kill(reason, TRUE);
                *killed = TRUE;
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::Hang(
    /* [in] */ IBinder* who,
    /* [in] */ Boolean allowRestart)
{
    if (CheckCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Requires permission %s", Manifest::permission::SET_ACTIVITY_WATCHER.string());
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<HangWhoDeathRecipient> death = new HangWhoDeathRecipient();

    AutoPtr< ::IProxy> proxy = (::IProxy*)who->Probe(EIID_IProxy);
    if (proxy == NULL || FAILED(proxy->LinkToDeath(death, 0))) {
        Slogger::W(TAG, "hang: given caller IBinder is already dead.");
        return NOERROR;
    }

    {    AutoLock syncLock(this);
        Watchdog::GetInstance()->SetAllowRestart(allowRestart);
        Slogger::I(TAG, "Hanging system process at request of pid %s", Binder::GetCallingPid());
        {    AutoLock syncLock(death);
            Boolean alive;
            while (proxy->IsStubAlive(&alive), alive) {
                death->Wait();
            }
        }
        Watchdog::GetInstance()->SetAllowRestart(TRUE);
    }
    return NOERROR;
}

ECode CActivityManagerService::Restart()
{
    if (CheckCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Requires permission %s", Manifest::permission::SET_ACTIVITY_WATCHER.string());
        return E_SECURITY_EXCEPTION;
    }

    Logger::I(TAG, "Sending shutdown broadcast...");

    AutoPtr<BroadcastReceiver> br = new RestartBroadcastReceiver(this);

    // First send the high-level shut down broadcast.
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SHUTDOWN, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->PutBooleanExtra(IIntent::EXTRA_SHUTDOWN_USERSPACE_ONLY, TRUE);
    /* For now we are not doing a clean shutdown, because things seem to get unhappy.
    mContext.sendOrderedBroadcastAsUser(intent,
            UserHandle.ALL, null, br, mHandler, 0, null, null);
    */
    br->OnReceive(mContext, intent);
    return NOERROR;
}

Int64 CActivityManagerService::GetLowRamTimeSinceIdle(
    /* [in] */ Int64 now)
{
    return mLowRamTimeSinceLastIdle + (mLowRamStartTime > 0 ? (now-mLowRamStartTime) : 0);
}

ECode CActivityManagerService::PerformIdleMaintenance()
{
    if (CheckCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Requires permission %s", Manifest::permission::SET_ACTIVITY_WATCHER.string());
        return E_SECURITY_EXCEPTION;
    }

    {    AutoLock syncLock(this);
        const Int64 now = SystemClock::GetUptimeMillis();
        const Int64 timeSinceLastIdle = now - mLastIdleTime;
        const Int64 lowRamSinceLastIdle = GetLowRamTimeSinceIdle(now);
        mLastIdleTime = now;
        mLowRamTimeSinceLastIdle = 0;
        if (mLowRamStartTime != 0) {
            mLowRamStartTime = now;
        }

        StringBuilder sb(128);
        sb.Append("Idle maintenance over ");
        TimeUtils::FormatDuration(timeSinceLastIdle, sb);
        sb.Append(" low RAM for ");
        TimeUtils::FormatDuration(lowRamSinceLastIdle, sb);
        Slogger::I(TAG, sb.ToString());

        // If at least 1/3 of our time since the last idle period has been spent
        // with RAM low, then we want to kill processes.
        Boolean doKilling = lowRamSinceLastIdle > (timeSinceLastIdle/3);

        List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
           AutoPtr<ProcessRecord> proc = *rit;
            if (proc->mNotCachedSinceIdle) {
                if (proc->mSetProcState > IActivityManager::PROCESS_STATE_TOP
                    && proc->mSetProcState <= IActivityManager::PROCESS_STATE_SERVICE) {
                    if (doKilling && proc->mInitialIdlePss != 0
                            && proc->mLastPss > ((proc->mInitialIdlePss*3)/2)) {
                        StringBuilder sb;
                        sb += "idle maint (pss ";
                        sb += proc->mLastPss;
                        sb += " from ";
                        sb += proc->mInitialIdlePss;
                        sb += ")";
                        proc->Kill(sb.ToString(), TRUE);
                    }
                }
            }
            else if (proc->mSetProcState < IActivityManager::PROCESS_STATE_HOME) {
                proc->mNotCachedSinceIdle = TRUE;
                proc->mInitialIdlePss = 0;
                proc->mNextPssTime = ProcessList::ComputeNextPssTime(
                    proc->mCurProcState, TRUE, IsSleeping(), now);
            }
        }

        mHandler->RemoveMessages(REQUEST_ALL_PSS_MSG);
        Boolean res;
        mHandler->SendEmptyMessageDelayed(REQUEST_ALL_PSS_MSG, 2*60*1000, &res);
    }
    return NOERROR;
}

ECode CActivityManagerService::RetrieveSettings()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String debugApp;
    Settings::Global::GetString(resolver, ISettingsGlobal::DEBUG_APP, &debugApp);
    Int32 value;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::WAIT_FOR_DEBUGGER, 0, &value);
    Boolean waitForDebugger = value != 0;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES, 0, &value);
    Boolean alwaysFinishActivities = value != 0;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::DEVELOPMENT_FORCE_RTL, 0, &value);
    Boolean forceRtl = value != 0;
    // Transfer any global setting for forcing RTL layout, into a System Property
    AutoPtr<ISystemProperties> sysProps;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
    sysProps->Set(ISettingsGlobal::DEVELOPMENT_FORCE_RTL, String(forceRtl ? "1":"0"));

    AutoPtr<IConfiguration> configuration;
    CConfiguration::New((IConfiguration**)&configuration);
    Settings::System::GetConfiguration(resolver, configuration);
    if (forceRtl) {
        // This will take care of setting the correct layout direction flags
        AutoPtr<ILocale> locale;
        configuration->GetLocale((ILocale**)&locale);
        configuration->SetLayoutDirection(locale);
    }

    {
        AutoLock lock(this);
        mDebugApp = mOrigDebugApp = debugApp;
        mWaitForDebugger = mOrigWaitForDebugger = waitForDebugger;
        mAlwaysFinishActivities = alwaysFinishActivities;
        // This happens before any activities are started, so we can
        // change mConfiguration in-place.
        UpdateConfigurationLocked(configuration, NULL, FALSE, TRUE);
        if (DEBUG_CONFIGURATION) Slogger::V(TAG, "Initial config: %s", TO_CSTR(mConfiguration));
    }
    return NOERROR;
}

/** Loads resources after the current configuration has been set. */
ECode CActivityManagerService::LoadResourcesOnSystemReady()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_hasRecents, &mHasRecents);
    // enable for detecting memory leak.
    // mHasRecents = FALSE;
    res->GetDimensionPixelSize(R::dimen::thumbnail_width, &mThumbnailWidth);
    res->GetDimensionPixelSize(R::dimen::thumbnail_height, &mThumbnailHeight);
    return NOERROR;
}

ECode CActivityManagerService::TestIsSystemReady(
    /* [out] */ Boolean* isSystemReady)
{
    VALIDATE_NOT_NULL(isSystemReady);

    // no need to synchronize(this) just to read & return the value
    *isSystemReady = mSystemReady;
    return NOERROR;
}

AutoPtr<IFile> CActivityManagerService::GetCalledPreBootReceiversFile()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    AutoPtr<IFile> systemDir;
    CFile::New(dataDir, String("system"), (IFile**)&systemDir);
    AutoPtr<IFile> fname;
    CFile::New(systemDir, CALLED_PRE_BOOTS_FILENAME, (IFile**)&fname);
    return fname;
}

AutoPtr<IList> CActivityManagerService::ReadLastDonePreBootReceivers()
{
    AutoPtr<IList> lastDoneReceivers;
    CArrayList::New((IList**)&lastDoneReceivers);
    AutoPtr<IFile> file = GetCalledPreBootReceiversFile();
    AutoPtr<IInputStream> fis;
    // try {
    CFileInputStream::New(file, (IInputStream**)&fis);
    AutoPtr<IInputStream> bufferStream;
    CBufferedInputStream::New(fis, 2048, (IInputStream**)&bufferStream);
    AutoPtr<IDataInput> dis;
    CDataInputStream::New(bufferStream, (IDataInput**)&dis);
    Int32 fvers;
    ECode ec = dis->ReadInt32(&fvers);
    if (FAILED(ec)) {
        if (fis != NULL) {
            fis->Close();
        }
        return lastDoneReceivers;
    }
    if (fvers == LAST_PREBOOT_DELIVERED_FILE_VERSION) {
        String vers;
        dis->ReadUTF(&vers);
        String codename;
        dis->ReadUTF(&codename);
        String build;
        dis->ReadUTF(&build);
        if (Build::VERSION::RELEASE.Equals(vers)
                    && Build::VERSION::CODENAME.Equals(codename)
                    && Build::VERSION::INCREMENTAL.Equals(build)) {
                Int32 num;
                dis->ReadInt32(&num);
                while (num > 0) {
                    num--;
                    String pkg;
                    dis->ReadUTF(&pkg);
                    String cls;
                    dis->ReadUTF(&cls);
                    AutoPtr<IComponentName> component;
                    CComponentName::New(pkg, cls, (IComponentName**)&component);
                    lastDoneReceivers->Add(component);
                }
            }
        }
    // } catch (FileNotFoundException e) {
    // } catch (IOException e) {
    //     Slogger::W(TAG, "Failure reading last done pre-boot receivers", e);
    // } finally {
    if (fis != NULL) {
        // try {
        fis->Close();
        // } catch (IOException e) {
        // }
    }
    // }
    return lastDoneReceivers;
}

ECode CActivityManagerService::WriteLastDonePreBootReceivers(
    /* [in] */ const List< AutoPtr<IComponentName> >* list)
{
    AutoPtr<IFile> file = GetCalledPreBootReceiversFile();
    AutoPtr<IOutputStream> fos;
    AutoPtr<IDataOutput> dos;
    // try {
    CFileOutputStream::New(file, (IOutputStream**)&fos);
    AutoPtr<IOutputStream> buffer;
    CBufferedOutputStream::New(fos, 2048, (IOutputStream**)&buffer);
    CDataOutputStream::New(buffer, (IDataOutput**)&dos);
    dos->WriteInt32(LAST_PREBOOT_DELIVERED_FILE_VERSION);
    dos->WriteUTF(Build::VERSION::RELEASE);
    dos->WriteUTF(Build::VERSION::CODENAME);
    dos->WriteUTF(Build::VERSION::INCREMENTAL);
    dos->WriteInt32(list->GetSize());
    List< AutoPtr<IComponentName> >::ConstIterator it;
    for (it = list->Begin(); it != list->End(); ++it) {
        String pkgName, className;
        (*it)->GetPackageName(&pkgName);
        (*it)->GetClassName(&className);
        dos->WriteUTF(pkgName);
        dos->WriteUTF(className);
    }
    // } catch (IOException e) {
    //     Slogger::W(TAG, "Failure writing last done pre-boot receivers", e);
    //     file.delete();
    // } finally {
    FileUtils::Sync(IFileOutputStream::Probe(fos));
    if (dos != NULL) {
        // try {
        FAIL_RETURN(ICloseable::Probe(dos)->Close());
        // } catch (IOException e) {
        //     // TODO Auto-generated catch block
        //     e.printStackTrace();
        // }
    }
    // }
    return NOERROR;
}

Boolean CActivityManagerService::DeliverPreBootCompleted(
    /* [in] */ IRunnable* onFinishCallback,
    /* [in] */ List<AutoPtr<IComponentName> >* doneReceivers,
    /* [in] */ Int32 userId)
{
    Boolean waitingUpdate = FALSE;
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_PRE_BOOT_COMPLETED, (IIntent**)&intent);
    AutoPtr<IList> ris;
    AppGlobals::GetPackageManager()->QueryIntentReceivers(
            intent, String(NULL), 0, userId, (IList**)&ris);

    if (ris != NULL) {
        Int32 size;
        ris->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> item;
            ris->Get(i, (IInterface**)&item);
            AutoPtr<IActivityInfo> activityInfo;
            IResolveInfo::Probe(item)->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                ris->Remove(i);
            }
        }
        intent->AddFlags(IIntent::FLAG_RECEIVER_BOOT_UPGRADE);

        // For User 0, load the version number. When delivering to a new user, deliver
        // to all receivers.
        if (userId == IUserHandle::USER_OWNER) {
            AutoPtr<IList> lastDoneReceivers = ReadLastDonePreBootReceivers();
            ris->GetSize(&size);
            String packageName, name;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                ris->Get(i, (IInterface**)&item);
                AutoPtr<IActivityInfo> ai;
                IResolveInfo::Probe(item)->GetActivityInfo((IActivityInfo**)&ai);
                IPackageItemInfo* pi = IPackageItemInfo::Probe(ai);
                pi->GetPackageName(&packageName);
                pi->GetName(&name);
                AutoPtr<IComponentName> comp;
                CComponentName::New(packageName, name, (IComponentName**)&comp);
                Boolean contains;
                if (lastDoneReceivers->Contains(comp, &contains), contains) {
                    // We already did the pre boot receiver for this app with the current
                    // platform version, so don't do it again...
                    ris->Remove(i);
                    i--;
                    // ...however, do keep it as one that has been done, so we don't
                    // forget about it when rewriting the file of last done receivers.
                    doneReceivers->PushBack(comp);
                }
            }
        }

        // If primary user, send broadcast to all available users, else just to userId
        AutoPtr<ArrayOf<Int32> > users = userId == IUserHandle::USER_OWNER ? GetUsersLocked()
                : AutoPtr<ArrayOf<Int32> >(ArrayOf<Int32>::Alloc(1));
        if (userId != IUserHandle::USER_OWNER) {
            (*users)[0] = userId;
        }

        ris->GetSize(&size);
        String packageName, name;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            ris->Get(i, (IInterface**)&item);
            AutoPtr<IActivityInfo> ai;
            IResolveInfo::Probe(item)->GetActivityInfo((IActivityInfo**)&ai);
            IPackageItemInfo* pi = IPackageItemInfo::Probe(ai);
            pi->GetPackageName(&packageName);
            pi->GetName(&name);
            AutoPtr<IComponentName> comp;
            CComponentName::New(packageName, name, (IComponentName**)&comp);
            doneReceivers->PushBack(comp);
            intent->SetComponent(comp);
            for (Int32 j = 0; j < users->GetLength(); j++) {
                AutoPtr<IIntentReceiver> finisher;
                // On last receiver and user, set up a completion callback
                if (i == size - 1 && j == users->GetLength() - 1 && onFinishCallback != NULL) {
                    CActivityManagerPreBootCompletedReceiver::New(
                        mHandler.Get(), onFinishCallback, (IIntentReceiver**)&finisher);
                }
                Slogger::I(TAG, "Sending system update to %s for user %d, finisher:%s",
                    TO_CSTR(comp), (*users)[j], TO_CSTR(finisher));
                Int32 res;
                BroadcastIntentLocked(NULL, String(NULL), intent, String(NULL), finisher,
                    0, String(NULL), NULL, String(NULL), IAppOpsManager::OP_NONE,
                    TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, (*users)[j], &res);
                if (finisher != NULL) {
                    waitingUpdate = TRUE;
                }
            }
        }
    }

    return waitingUpdate;
}

ECode CActivityManagerService::SystemReady(
    /* [in] */ IRunnable* goingCallback)
{
    Logger::I(TAG, " >>>>>>>>>> CActivityManagerService::SystemReady:%s", TO_CSTR(goingCallback));
    {
        AutoLock lock(this);

        if (mSystemReady) {
            // If we're done calling all the receivers, run the next "boot phase" passed in
            // by the SystemServer
            if (goingCallback != NULL)
                goingCallback->Run();
            return NOERROR;
        }
        // Make sure we have the current profile info, since it is needed for
        // security checks.
        UpdateCurrentProfileIdsLocked();

        if (mRecentTasks == NULL) {
            mRecentTasks = mTaskPersister->RestoreTasksLocked();
            if (!mRecentTasks->IsEmpty()) {
                mStackSupervisor->CreateStackForRestoredTaskHistory(mRecentTasks);
            }
            CleanupRecentTasksLocked(IUserHandle::USER_ALL);
            mTaskPersister->StartPersisting();
        }

        // Check to see if there are any update receivers to run.
        if (!mDidUpdate) {
            if (mWaitingUpdate) {
                return NOERROR;
            }

            AutoPtr<List<AutoPtr<IComponentName> > > doneReceivers = new List<AutoPtr<IComponentName> >();
            AutoPtr<IRunnable> runnable = new OnFinishCallback(this, doneReceivers.Get(), goingCallback);
            mWaitingUpdate = DeliverPreBootCompleted(runnable, doneReceivers, IUserHandle::USER_OWNER);

            if (mWaitingUpdate) {
                return NOERROR;
            }
            mDidUpdate = TRUE;
        }

        mAppOpsService->SystemReady();
        mSystemReady = TRUE;
    }

    AutoPtr<List<AutoPtr<ProcessRecord> > > procsToKill;
    {
        AutoLock lock(mPidsSelfLockedLock);

        List<AutoPtr<ProcessRecord> > pidsList;
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
            pidsList.PushBack(it->mSecond);
        }


        List<AutoPtr<ProcessRecord> >::ReverseIterator lrit;
        for (lrit = pidsList.RBegin(); lrit != pidsList.REnd(); ++lrit) {
                AutoPtr<ProcessRecord> proc = *lrit;
            if (!IsAllowedWhileBooting(proc->mInfo)){
                if (procsToKill == NULL) {
                    procsToKill = new List<AutoPtr<ProcessRecord> >();
                }
                procsToKill->PushBack(proc);
            }
        }
    }

    {
        AutoLock lock(this);

        if (procsToKill != NULL) {
            List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
            for (rit = procsToKill->RBegin(); rit != procsToKill->REnd(); ++rit) {
                AutoPtr<ProcessRecord> proc = *rit;
                Slogger::I(TAG, "Removing system update proc: %p", proc.Get());
                RemoveProcessLocked(proc, TRUE, FALSE, String("system update done"));
            }
        }

        // Now that we have cleaned up any update processes, we
        // are ready to start launching real processes and know that
        // we won't trample on them any more.
        mProcessesReady = TRUE;
    }

    Slogger::I(TAG, "System now ready");
//    EventLog.writeEvent(EventLogTags.BOOT_PROGRESS_AMS_READY,
//        SystemClock.uptimeMillis());
    AutoPtr<IIntentFilter> bootFilter;
    CIntentFilter::New(IAppInterface::CHECK_SCREEN_IDLE_ACTION, (IIntentFilter**)&bootFilter);
    AutoPtr<ScreenStatusReceiver> receiver = new ScreenStatusReceiver(this);
    AutoPtr<IIntent> outIntent;
    mContext->RegisterReceiver(receiver, bootFilter, (IIntent**)&outIntent);

    {
        AutoLock lock(this);
        // Make sure we have no pre-ready processes sitting around.

        if (mFactoryTest == FactoryTest::FACTORY_TEST_LOW_LEVEL) {
            AutoPtr<IPackageManager> pkgManager;
            mContext->GetPackageManager((IPackageManager**)&pkgManager);
            AutoPtr<IIntent> tIntent;
            CIntent::New(IIntent::ACTION_FACTORY_TEST, (IIntent**)&tIntent);
            AutoPtr<IResolveInfo> ri;
            pkgManager->ResolveActivity(tIntent,
                    STOCK_PM_FLAGS, (IResolveInfo**)&ri);
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            AutoPtr<ICharSequence> errorMsg;
            if (ri != NULL) {
                AutoPtr<IActivityInfo> ai;
                ri->GetActivityInfo((IActivityInfo**)&ai);
                AutoPtr<IApplicationInfo> app;
                IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&app);
                Int32 appFlags;
                app->GetFlags(&appFlags);
                if ((appFlags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                    mTopAction = IIntent::ACTION_FACTORY_TEST;
                    mTopData = NULL;
                    String appPkgName, aiName;
                    IPackageItemInfo::Probe(app)->GetPackageName(&appPkgName);
                    IPackageItemInfo::Probe(ai)->GetName(&aiName);
                    mTopComponent = NULL;
                    CComponentName::New(appPkgName, aiName, (IComponentName**)&mTopComponent);
                }
                else {
                    resources->GetText(
                            R::string::factorytest_not_system, (ICharSequence**)&errorMsg);
                }
            }
            else {
                resources->GetText(
                        R::string::factorytest_no_action, (ICharSequence**)&errorMsg);
            }
            if (errorMsg != NULL) {
                mTopAction = NULL;
                mTopData = NULL;
                mTopComponent = NULL;

                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(SHOW_FACTORY_ERROR_MSG, (IMessage**)&msg);
                msg->SetObj(errorMsg->Probe(EIID_IInterface));
                Boolean result;
                mHandler->SendMessage(msg, &result);
            }
        }
    }

    RetrieveSettings();
    LoadResourcesOnSystemReady();

    {
        AutoLock syncLock(this);
        ReadGrantedUriPermissionsLocked();
    }

    if (goingCallback != NULL) goingCallback->Run();

    mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_USER_RUNNING_START,
            StringUtils::ToString(mCurrentUserId), mCurrentUserId);
    mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_USER_FOREGROUND_START,
            StringUtils::ToString(mCurrentUserId), mCurrentUserId);
    mSystemServiceManager->StartUser(mCurrentUserId);

    {
        AutoLock lock(this);
        if (mFactoryTest != FactoryTest::FACTORY_TEST_LOW_LEVEL) {
            // try {
            AutoPtr<IList> apps;
            AppGlobals::GetPackageManager()->GetPersistentApplications(STOCK_PM_FLAGS, (IList**)&apps);
            if (apps != NULL) {
                Int32 N;
                apps->GetSize(&N);
                for (Int32 i = 0; i < N; i++) {
                     AutoPtr<IInterface> obj;
                     apps->Get(i, (IInterface**)&obj);
                     AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
                     String infoPkgName;
                     if (info != NULL) {
                         IPackageItemInfo::Probe(info)->GetPackageName(&infoPkgName);
                         if (!infoPkgName.Equals("android"))
                            AddAppLocked(info, FALSE, String(NULL) /* ABI override */);
                     }
                 }
            }
            // } catch (RemoteException ex) {
            //     // pm is in same process, this will never happen.
            // }
        }

        // Start up initial activity.
        mBooting = TRUE;
        StartHomeActivityLocked(mCurrentUserId);

        // try {
        Boolean hasUidErrors;
        AppGlobals::GetPackageManager()->HasSystemUidErrors(&hasUidErrors);
        if (hasUidErrors) {
            Boolean result;
            mHandler->SendEmptyMessage(SHOW_UID_ERROR_MSG, &result);
        }
        // } catch (RemoteException e) {
        // }

        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_USER_STARTED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, mCurrentUserId);
        Int32 result;

        BroadcastIntentLocked(NULL, String(NULL), intent,
            String(NULL), NULL, 0, String(NULL), NULL, String(NULL), IAppOpsManager::OP_NONE,
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, mCurrentUserId, &result);

        AutoPtr<IIntent> newIntent;
        CIntent::New(IIntent::ACTION_USER_STARTING, (IIntent**)&newIntent);
        newIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        newIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, mCurrentUserId);
        AutoPtr<IIntentReceiver> receiver;
        CActivityManagerSystemReadyReceiver::New((IIntentReceiver**)&receiver);
        BroadcastIntentLocked(NULL, String(NULL), newIntent,
            String(NULL), receiver, 0, String(NULL), NULL,
            Manifest::permission::INTERACT_ACROSS_USERS, IAppOpsManager::OP_NONE,
            TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
        // catch (Throwable t) {
        //     Slog.wtf(TAG, "Failed sending first user broadcasts", t);
        // }
        // } finally {
        Binder::RestoreCallingIdentity(ident);

        // }
        mStackSupervisor->ResumeTopActivitiesLocked();
        SendUserSwitchBroadcastsLocked(-1, mCurrentUserId);
    }

    return NOERROR;
}

Boolean CActivityManagerService::MakeAppCrashingLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ const String& stackTrace)
{
    app->mCrashing = TRUE;
    app->mCrashingReport = GenerateProcessError(app,
            IActivityManagerProcessErrorStateInfo::CRASHED, String(NULL), shortMsg, longMsg, stackTrace);
    StartAppProblemLocked(app);
    app->StopFreezingAllLocked();
    return HandleAppCrashLocked(app, shortMsg, longMsg, stackTrace);
}

ECode CActivityManagerService::MakeAppNotRespondingLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& activity,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg)
{
    app->mNotResponding = TRUE;
    app->mNotRespondingReport = GenerateProcessError(app,
            IActivityManagerProcessErrorStateInfo::NOT_RESPONDING,
            activity, shortMsg, longMsg, String(NULL));
    StartAppProblemLocked(app);
    app->StopFreezingAllLocked();
    return NOERROR;
}

///**
// * Generate a process error record, suitable for attachment to a ProcessRecord.
// *
// * @param app The ProcessRecord in which the error occurred.
// * @param condition Crashing, Application Not Responding, etc.  Values are defined in
// *                      IActivityManager::AppErrorStateInfo
// * @param activity The activity associated with the crash, if known.
// * @param shortMsg Short message describing the crash.
// * @param longMsg Long message describing the crash.
// * @param stackTrace Full crash stack trace, may be null.
// *
// * @return Returns a fully-formed AppErrorStateInfo record.
// */
AutoPtr<IActivityManagerProcessErrorStateInfo> CActivityManagerService::GenerateProcessError(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 condition,
    /* [in] */ const String& activity,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ const String& stackTrace)
{
   AutoPtr<IActivityManagerProcessErrorStateInfo> report;
   CActivityManagerProcessErrorStateInfo::New((IActivityManagerProcessErrorStateInfo**)&report);

   report->SetCondition(condition);
   report->SetProcessName(app->mProcessName);
   report->SetPid(app->mPid);
   Int32 appUid;
   app->mInfo->GetUid(&appUid);
   report->SetUid(appUid);
   report->SetTag(activity);
   report->SetShortMsg(shortMsg);
   report->SetLongMsg(longMsg);
   report->SetStackTrace(stackTrace);

    return report;
}

ECode CActivityManagerService::KillAppAtUsersRequest(
    /* [in] */ ProcessRecord* app,
    /* [Int32] */ IDialog* fromDialog )
{
    AutoLock lock(this);
    app->mCrashing = FALSE;
    app->mCrashingReport = NULL;
    app->mNotResponding = FALSE;
    app->mNotRespondingReport = NULL;
    if (app->mAnrDialog.Get() == fromDialog) {
        app->mAnrDialog = NULL;
    }
    if (app->mWaitDialog.Get() == fromDialog) {
       app->mWaitDialog = NULL;
    }
    if (app->mPid > 0 && app->mPid != MY_PID) {
        HandleAppCrashLocked(app, String(NULL), String(NULL), String(NULL));
        app->Kill(String("user request after error"), TRUE);
    }
    return NOERROR;
}

void CActivityManagerService::SendAppFailureBroadcast(
    /* [in[ */ const String& pkgName)
{
    AutoPtr<IUri> uri;
    if (pkgName != NULL) {
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        AutoPtr<IUri> uri;
        uriHelper->FromParts(String("package"), pkgName, String(NULL), (IUri**)&uri);
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_APP_FAILURE, uri, (IIntent**)&intent);
    mContext->SendBroadcastAsUser(intent, UserHandle::CURRENT_OR_SELF);
}

Boolean CActivityManagerService::IsPossibleThemeCrash(
    /* [in] */ const String& exceptionClassName)
{
    // if (Resources.NotFoundException.class.getName().equals(exceptionClassName) ||
    //     InflateException.class.getName().equals(exceptionClassName)) {
    if (exceptionClassName.Equals("NotFoundException") ||
        exceptionClassName.Equals("InflateException")) {
        return TRUE;
    }
    return FALSE;
}

Boolean CActivityManagerService::HandleAppCrashLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ const String& stackTrace)
{
    Int64 now = SystemClock::GetUptimeMillis();

    Int64 crashTime;
    String processName;
    app->mInfo->GetProcessName(&processName);
    if (!app->mIsolated) {
        crashTime = mProcessCrashTimes->Get(processName, app->mUid);
    }
    else {
        crashTime = 0;
    }

    if (IsPossibleThemeCrash(shortMsg)) {
        String packageName;
        IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&packageName);
        SendAppFailureBroadcast(packageName);
    }

    if (crashTime != 0 && now < crashTime + ProcessList::MIN_CRASH_INTERVAL) {
        // This process loses!
        Slogger::W(TAG, "Process %s has crashed too many times: killing!", processName.string());
    //     EventLog.writeEvent(EventLogTags.AM_PROCESS_CRASHED_TOO_MUCH,
    //             app.userId, app.info.processName, app.uid);
        mStackSupervisor->HandleAppCrashLocked(app);
        if (!app->mPersistent) {
            // We don't want to start this process again until the user
            // explicitly does so...  but for persistent process, we really
            // need to keep it running.  If a persistent process is actually
            // repeatedly crashing, then badness for everyone.
    //         EventLog.writeEvent(EventLogTags.AM_PROC_BAD, app.userId, app.uid,
    //                 app.info.processName);
            if (!app->mIsolated) {
                // XXX We don't have a way to mark isolated processes
                // as bad, since they don't have a peristent identity.
                String processName;
                app->mInfo->GetProcessName(&processName);
                AutoPtr<BadProcessInfo> badProcessInfo = new BadProcessInfo(
                    now, shortMsg, longMsg, stackTrace);
                mBadProcesses->Put(processName, app->mUid, badProcessInfo);
                mProcessCrashTimes->Remove(processName, app->mUid);
            }
            app->mBad = TRUE;
            app->mRemoved = TRUE;
            // Don't let services in this process be restarted and potentially
            // annoy the user repeatedly.  Unless it is persistent, since those
            // processes run critical code.
            RemoveProcessLocked(app, FALSE, FALSE, String("crash"));
            mStackSupervisor->ResumeTopActivitiesLocked();
            return FALSE;
        }
        mStackSupervisor->ResumeTopActivitiesLocked();
    }
    else {
        mStackSupervisor->FinishTopRunningActivityLocked(app);
    }

    // Bump up the crash count of any services currently running in the proc.
    HashSet< AutoPtr<CServiceRecord> >::Iterator it;
    for (it = app->mServices.Begin(); it != app->mServices.End(); ++it) {
        // Any services running in the application need to be placed
        // back in the pending list.
        AutoPtr<CServiceRecord> sr = *it;
        sr->mCrashCount++;
    }

    // If the crashing process is what we consider to be the "home process" and it has been
    // replaced by a third-party app, clear the package preferred activities from packages
    // with a home activity running in the process to prevent a repeatedly crashing app
    // from blocking the user to manually clear the list.
    Int32 homeFlags;
    List< AutoPtr<ActivityRecord> > activities = app->mActivities;
    if (app == mHomeProcess && activities.IsEmpty() == FALSE
        && ((mHomeProcess->mInfo->GetFlags(&homeFlags), homeFlags & IApplicationInfo::FLAG_SYSTEM) == 0)) {
        List< AutoPtr<ActivityRecord> >::Iterator it;
        for (it = activities.Begin(); it != activities.End(); ++it) {
            AutoPtr<ActivityRecord> r = *it;
            if (r->IsHomeActivity()) {
                Logger::I(TAG, "Clearing package preferred activities from %s", r->mPackageName.string());
                // try {
                AutoPtr<IActivityThreadHelper> helper;
                CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
                AutoPtr<IIPackageManager> pkgManager;
                helper->GetPackageManager((IIPackageManager**)&pkgManager);
                pkgManager->ClearPackagePreferredActivities(r->mPackageName);
                // } catch (RemoteException c) {
                //     // pm is in same process, this will never happen.
                // }
            }
        }
    }

    if (!app->mIsolated) {
        // XXX Can't keep track of crash times for isolated processes,
        // because they don't have a perisistent identity.
        String appProcessName;
        app->mInfo->GetProcessName(&appProcessName);
        mProcessCrashTimes->Put(appProcessName, app->mUid, now);
    }

    if (app->mCrashHandler != NULL) {
        Boolean res;
        mHandler->Post(app->mCrashHandler, &res);
    }
    return TRUE;
}

ECode CActivityManagerService::StartAppProblemLocked(
    /* [in] */ ProcessRecord* app)
{
    // If this app is not running under the current user, then we
    // can't give it a report button because that would require
    // launching the report UI under a different user.
    app->mErrorReportReceiver = NULL;

    for (Int32 i = 0; i < mCurrentProfileIds->GetLength(); i++) {
        if (app->mUserId == (*mCurrentProfileIds)[i]) {
            AutoPtr<IApplicationErrorReportHelper> aerHelper;
            CApplicationErrorReportHelper::AcquireSingleton((IApplicationErrorReportHelper**)&aerHelper);
            app->mErrorReportReceiver = NULL;
            String packageName;
            IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&packageName);
            Int32 flags;
            app->mInfo->GetFlags(&flags);
            aerHelper->GetErrorReportReceiver(mContext, packageName, flags,
                (IComponentName**)&app->mErrorReportReceiver);
        }
    }
    SkipCurrentReceiverLocked(app);
    return NOERROR;
}

ECode CActivityManagerService::SkipCurrentReceiverLocked(
    /* [in] */ ProcessRecord* app)
{
    Int32 size = mBroadcastQueues->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<BroadcastQueue> queue = (*mBroadcastQueues)[i];
        queue->SkipCurrentReceiverLocked(app);
    }
    return NOERROR;
}

/**
 * Used by {@link com.android.internal.os.RuntimeInit} to report when an application crashes.
 * The application process will exit immediately after this call returns.
 * @param app object of the crashing app, null for the system server
 * @param crashInfo describing the exception
 */
ECode CActivityManagerService::HandleApplicationCrash(
    /* [in] */ IBinder* app,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    AutoPtr<ProcessRecord> r = FindAppProcess(app, String("Crash"));
    String processName = app == NULL ? String("system_server")
            : (r == NULL ? String("unknown") : r->mProcessName);
    return HandleApplicationCrashInner(String("crash"), r, processName, crashInfo);
}

/* Native crash reporting uses this inner version because it needs to be somewhat
 * decoupled from the AM-managed cleanup lifecycle
 */
ECode CActivityManagerService::HandleApplicationCrashInner(
    /* [in] */ const String& eventType,
    /* [in] */ ProcessRecord* r,
    /* [in] */ const String& processName,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
//     EventLog.writeEvent(EventLogTags.AM_CRASH, Binder::GetCallingPid(),
//             UserHandle::GetUserId(Binder::GetCallingUid()), processName,
//             r == null ? -1 : r.info.flags,
//             crashInfo.exceptionClassName,
//             crashInfo.exceptionMessage,
//             crashInfo.throwFileName,
//             crashInfo.throwLineNumber);

    AddErrorToDropBox(eventType, r, processName, NULL, NULL, String(NULL), String(NULL), NULL, crashInfo);

    CrashApplication(r, crashInfo);
    return NOERROR;
}

ECode CActivityManagerService::HandleApplicationStrictModeViolation(
    /* [in] */ IBinder* app,
    /* [in] */ Int32 violationMask,
    /* [in] */ IStrictModeViolationInfo* info)
{
    AutoPtr<ProcessRecord> r = FindAppProcess(app, String("StrictMode"));
    if (r == NULL) {
        return NOERROR;
    }

   if ((violationMask & IStrictMode::PENALTY_DROPBOX) != 0) {
       Int32 stackFingerprint = Object::GetHashCode(info);
       Boolean logIt = TRUE;
       AutoLock lock(mAlreadyLoggedViolatedStacksLock);
       HashSet<Int32>::Iterator it = mAlreadyLoggedViolatedStacks.Find(stackFingerprint);
        if (it != mAlreadyLoggedViolatedStacks.End()) {
            logIt = FALSE;
            // TODO: sub-sample into EventLog for these, with
            // the info.durationMillis?  Then we'd get
            // the relative pain numbers, without logging all
            // the stack traces repeatedly.  We'd want to do
            // likewise in the client code, which also does
            // dup suppression, before the Binder call.
        }
        else {
            if (mAlreadyLoggedViolatedStacks.GetSize() >= MAX_DUP_SUPPRESSED_STACKS) {
                mAlreadyLoggedViolatedStacks.Clear();
            }
            mAlreadyLoggedViolatedStacks.Insert(stackFingerprint);
        }
        if (logIt) {
            LogStrictModeViolationToDropBox(r, info);
        }
    }

    if ((violationMask & IStrictMode::PENALTY_DIALOG) != 0) {
        AutoPtr<AppErrorResult> result = new AppErrorResult();
        {
            AutoLock lock(this);
            const Int64 origId = Binder::ClearCallingIdentity();

            AutoPtr<IInteger32> imask;
            CInteger32::New(violationMask, (IInteger32**)&imask);
            AutoPtr<StringObjectHashMap> data = new StringObjectHashMap();
            (*data)[String("result")] = result->Probe(EIID_IInterface);
            (*data)[String("app")] = r->Probe(EIID_IInterface);
            (*data)[String("violationMask")] = imask->Probe(EIID_IInterface);
            (*data)[String("info")] = info ? info->Probe(EIID_IInterface) : NULL;

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(
                SHOW_STRICT_MODE_VIOLATION_MSG,
                data->Probe(EIID_IInterface), (IMessage**)&msg);
            Boolean bval;
            mHandler->SendMessage(msg, &bval);

            Binder::RestoreCallingIdentity(origId);
        }

        Int32 res = result->GetResult();
        Slogger::W(TAG, "handleApplicationStrictModeViolation; res=%d", res);
    }
    return NOERROR;
}

ECode CActivityManagerService::LogStrictModeViolationToDropBox(
    /* [in] */ ProcessRecord* process,
    /* [in] */ IStrictModeViolationInfo* info)
{
    if (info == NULL) {
        return NOERROR;
    }
    Int32 flags;
    const Boolean isSystemApp = process == NULL ||
           ((process->mInfo->GetFlags(&flags), flags & (IApplicationInfo::FLAG_SYSTEM |
                                  IApplicationInfo::FLAG_UPDATED_SYSTEM_APP))) != 0;
    String processName = process == NULL ? String("unknown") : process->mProcessName;
    String dropboxTag = isSystemApp ? String("system_app_strictmode") : String("data_app_strictmode");
    AutoPtr<IInterface> dboxService;
    mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&dboxService);
    AutoPtr<IDropBoxManager> dbox = IDropBoxManager::Probe(dboxService);

   // Exit early if the dropbox isn't configured to accept this report type.
    Boolean enabled;
    if (dbox == NULL || (dbox->IsTagEnabled(dropboxTag, &enabled), !enabled)) return NOERROR;

    Boolean bufferWasEmpty = FALSE;
    Boolean needsFlush = FALSE;
    AutoPtr<StringBuilder> sb;
    if (isSystemApp)
        sb = mStrictModeBuffer;
    else
        sb = new StringBuilder(1024);
    {    AutoLock syncLock(sb);
        bufferWasEmpty = sb->GetLength() == 0;
        AppendDropBoxProcessHeaders(process, processName, *sb);
        sb->Append("Build: ");
        sb->Append(Build::FINGERPRINT);
        sb->Append("\n");
        sb->Append("System-App: ");
        sb->Append(isSystemApp);
        sb->Append("\n");
        sb->Append("Uptime-Millis: ");

        Int64 violationUptimeMillis;
        info->GetViolationUptimeMillis(&violationUptimeMillis);
        sb->Append(violationUptimeMillis);
        sb->Append("\n");
        Int32 violationNumThisLoop;
        info->GetViolationNumThisLoop(&violationNumThisLoop);
        if (violationNumThisLoop != 0) {
            sb->Append(String("Loop-Violation-Number: "));
            sb->Append(violationNumThisLoop);
            sb->Append("\n");
        }
        Int32 numAnimationsRunning;
        info->GetNumAnimationsRunning(&numAnimationsRunning);
        if (numAnimationsRunning != 0) {
            sb->Append(String("Animations-Running: "));
            sb->Append(numAnimationsRunning);
            sb->Append("\n");
        }
        String broadcastIntentAction;
        info->GetBroadcastIntentAction(&broadcastIntentAction);
        if (!broadcastIntentAction.IsNull()) {
            sb->Append(String("Broadcast-Intent-Action: "));
            sb->Append(broadcastIntentAction);
            sb->Append("\n");
        }
        Int32 durationMillis;
        info->GetDurationMillis(&durationMillis);
        if (durationMillis != -1) {
            sb->Append(String("Duration-Millis: "));
            sb->Append(durationMillis);
            sb->Append("\n");
        }
        Int64 numInstances;
        info->GetNumInstances(&numInstances);
        if (numInstances != -1) {
            sb->Append(String("Instance-Count: "));
            sb->Append(numInstances);
            sb->Append("\n");
        }
        AutoPtr<ArrayOf<String> > tags;
        info->GetTags((ArrayOf<String>**)&tags);
        if (tags != NULL) {
            for(Int32 i = 0; i < tags->GetLength(); i++)
            {
                sb->Append(String("Span-Tag: "));
                sb->Append((*tags)[i]);
                sb->Append("\n");
            }
        }
        sb->Append("\n");
        AutoPtr<IApplicationErrorReportCrashInfo> crashInfo;
        info->GetCrashInfo((IApplicationErrorReportCrashInfo**)&crashInfo);
        String trace;
        if (crashInfo != NULL && (crashInfo->GetStackTrace(&trace), !trace.IsNull())) {
            sb->Append(trace);
        }
        sb->Append("\n");

        // Only buffer up to ~64k.  Various logging bits truncate
        // things at 128k.
        needsFlush = (sb->GetLength() > 64 * 1024);
    }

    // Flush immediately if the buffer's grown too large, or this
    // is a non-system app.  Non-system apps are isolated with a
    // different tag & policy and not batched.
    //
    // Batching is useful during internal testing with
    // StrictMode settings turned up high.  Without batching,
    // thousands of separate files could be created on boot.
    if (!isSystemApp || needsFlush) {
        AutoPtr<DropBoxTagThread> thread = new DropBoxTagThread(
                this, sb, dbox, dropboxTag);
        thread->Start();
        return NOERROR;
    }

    // System app batching:
    if (!bufferWasEmpty) {
        // An existing dropbox-writing thread is outstanding, so
        // we don't need to start it up.  The existing thread will
        // catch the buffer appends we just did.
        return NOERROR;
    }

    // Worker thread to both batch writes and to avoid blocking the caller on I/O.
    // (After this point, we shouldn't access AMS internal data structures.)
    AutoPtr<DropBoxTagThread2> thread = new DropBoxTagThread2(this, dbox, dropboxTag);
    thread->Start();
    return NOERROR;
}

ECode CActivityManagerService::HandleApplicationWtf(
    /* [in] */ IBinder* app,
    /* [in] */ const String& tag,
    /* [in] */ Boolean system,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();

    if (system) {
        // If this is coming from the system, we could very well have low-level
        // system locks held, so we want to do this all asynchronously.  And we
        // never want this to become fatal, so there is that too.
        AutoPtr<IRunnable> runnable = new HandleApplicationWtfRunnable(
            this, callingUid, callingPid, app, tag, crashInfo);
        Boolean res;
        mHandler->Post(runnable, &res);
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ProcessRecord> r = HandleApplicationWtfInner(
        callingUid, callingPid, app, tag, crashInfo);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::WTF_IS_FATAL, 0, &value);
    if (r != NULL && r->mPid != Process::MyPid() && value != 0) {
        CrashApplication(r, crashInfo);
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

AutoPtr<ProcessRecord> CActivityManagerService::HandleApplicationWtfInner(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ IBinder* app,
    /* [in] */ const String& tag,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    AutoPtr<ProcessRecord> r = FindAppProcess(app, String("WTF"));
    String processName = app == NULL ? String("system_server")
            : (r == NULL ? String("unknown") : r->mProcessName);

    // EventLog.writeEvent(EventLogTags.AM_WTF, UserHandle.getUserId(callingUid), callingPid,
    //         processName, r == NULL ? -1 : r.info.flags, tag, crashInfo.exceptionMessage);

    AddErrorToDropBox(String("wtf"), r, processName, NULL, NULL, tag, String(NULL), NULL, crashInfo);

    return r;
}

AutoPtr<ProcessRecord> CActivityManagerService::FindAppProcess(
    /* [in] */ IBinder* app,
    /* [in] */ const String& reason)
{
    if (app == NULL) {
        return NULL;
    }

    {
        AutoLock lock(this);

        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > map;
        map = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
        for (it = map->Begin(); it != map->End(); ++it) {
            AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > apps = it->mSecond;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator appsIt;
            for (appsIt = apps->Begin(); appsIt != apps->End(); ++appsIt) {
                AutoPtr<ProcessRecord> p = appsIt->mSecond;
                if (p->mThread != NULL && IBinder::Probe(p->mThread) == app) {
                    return p;
                }
            }
        }

        Slogger::W(TAG, "Can't find mystery application for %s from pid=%d uid=%d: %p", reason.string(), Binder::GetCallingPid(),
                Binder::GetCallingUid(), app);
       return NULL;
    }
}

ECode CActivityManagerService::AppendDropBoxProcessHeaders(
    /* [in] */ ProcessRecord* process,
    /* [in] */ const String& processName,
    /* [in] */ StringBuilder& sb)
{
    // Watchdog thread ends up invoking this function (with
    // a null ProcessRecord) to add the stack file to dropbox.
    // Do not acquire a lock on this (am) in such cases, as it
    // could cause a potential deadlock, if and when watchdog
    // is invoked due to unavailability of lock on am and it
    // would prevent watchdog from killing system_server.
    if (process == NULL) {
        sb.Append("Process: ");
        sb.Append(processName);
        sb.Append("\n");
        return NOERROR;
    }
    // Note: ProcessRecord 'process' is guarded by the service
    // instance.  (notably process.pkgList, which could otherwise change
    // concurrently during execution of this method)
    {
        AutoLock lock(this);
        sb.Append("Process: ");
        sb.Append(processName);
        sb.Append("\n");
        Int32 flags;
        process->mInfo->GetFlags(&flags);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        sb.Append("Flags: 0x");
        String fstr = StringUtils::ToString(flags, 16);
        sb.Append(fstr);
        sb.Append("\n");
        Int32 size;
        process->mPkgList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            process->mPkgList->GetKeyAt(i, (IInterface**)&item);
            String pkg;
            ICharSequence::Probe(item)->ToString(&pkg);
            sb.Append("Package: ");
            sb.Append(pkg);
            // try {
            AutoPtr<IPackageInfo> pi;
            FAIL_RETURN(pm->GetPackageInfo(pkg, 0, UserHandle::GetCallingUserId(), (IPackageInfo**)&pi));
            if (pi != NULL) {
                sb.Append(" v");
                Int32 code;
                pi->GetVersionCode(&code);
                sb.Append(code);
                String name;
                pi->GetVersionName(&name);
                if (!name.IsNull()) {
                    sb.Append(" (");
                    sb.Append(name);
                    sb.Append(")");
                }
            }
            // } catch (RemoteException e) {
            //     Slogger::e(TAG, "Error getting package info: " + pkg, e);
            // }
            sb.Append("\n");
        }
    }
    return NOERROR;
}

String CActivityManagerService::ProcessClass(
    /* [in] */ ProcessRecord* process)
{
    Int32 pFlags;
    if (process == NULL || process->mPid == MY_PID) {
        return String("system_server");
    }
    else if (process->mInfo->GetFlags(&pFlags), (pFlags & IApplicationInfo::FLAG_SYSTEM) != 0)  {
        return String("system_app");
    }
    else {
        return String("data_app");
    }
}

ECode CActivityManagerService::AddErrorToDropBox(
    /* [in] */ const String& eventType,
    /* [in] */ ProcessRecord* process,
    /* [in] */ const String& processName,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ ActivityRecord* parent,
    /* [in] */ const String& subject,
    /* [in] */ const String& report,
    /* [in] */ IFile* logFile,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    // NOTE -- this must never acquire the ActivityManagerService lock,
    // otherwise the watchdog may be prevented from resetting the system.

    const String dropboxTag = ProcessClass(process) + "_" + eventType;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&service);
    AutoPtr<IDropBoxManager> dbox = IDropBoxManager::Probe(service);

    // Exit early if the dropbox isn't configured to accept this report type.
    Boolean enabled;
    if (dbox == NULL || (dbox->IsTagEnabled(dropboxTag, &enabled), !enabled)) return NOERROR;

    AutoPtr<StringBuilder> sb = new StringBuilder(1024);
    AppendDropBoxProcessHeaders(process, processName, *sb);
    if (activity != NULL) {
        sb->Append("Activity: ");
        sb->Append(activity->mShortComponentName);
        sb->Append("\n");
    }
    if (parent != NULL && parent->mApp != NULL && parent->mApp->mPid != process->mPid) {
        sb->Append("Parent-Process: ");
        sb->Append(parent->mApp->mProcessName);
        sb->Append("\n");
    }
    if (parent != NULL && parent != activity) {
        sb->Append("Parent-Activity: ");
        sb->Append(parent->mShortComponentName);
        sb->Append("\n");
    }
    if (subject != NULL) {
        sb->Append("Subject: ");
        sb->Append(subject);
        sb->Append("\n");
    }
    sb->Append("Build: ");
    sb->Append(Build::FINGERPRINT);
    sb->Append("\n");
//TODO:
//    if (Debug.isDebuggerConnected()) {
//        sb.Append("Debugger: Connected\n");
//    }
    sb->Append("\n");

    // Do the rest in a worker thread to avoid blocking the caller on I/O
    // (After this point, we shouldn't access AMS internal data structures.)
    AutoPtr<WorkerThread> worker = new WorkerThread(this, dropboxTag, report, sb, logFile,
            crashInfo, dbox);

    if (process == NULL) {
        // If process is null, we are being called from some internal code
        // and may be about to die -- run this synchronously.
        worker->Run();
    }
    else {
        worker->Start();
    }
    return NOERROR;
}

ECode  CActivityManagerService::CrashApplication(
    /* [in] */ ProcessRecord* r,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 timeMillis;
    system->GetCurrentTimeMillis(&timeMillis);
    String shortMsg;
    crashInfo->GetExceptionClassName(&shortMsg);
    String longMsg;
    crashInfo->GetExceptionMessage(&longMsg);
    String stackTrace;
    crashInfo->GetStackTrace(&stackTrace);
    if (!shortMsg.IsNull() && !longMsg.IsNull()) {
        longMsg = shortMsg + ": " + longMsg;
    }
    else if (!shortMsg.IsNull()) {
        longMsg = shortMsg;
    }

    AutoPtr<AppErrorResult> result = new AppErrorResult();
    {
        AutoLock lock(this);
        if (mController != NULL) {
            String name = r != NULL ? r->mProcessName : String(NULL);
            Int32 pid = r != NULL ? r->mPid : Binder::GetCallingPid();
            Int32 uid;
            if (r != NULL)
                r->mInfo->GetUid(&uid);
            else
                uid = Binder::GetCallingUid();
            Boolean crashed;
            if (FAILED(mController->AppCrashed(name, pid,
                    shortMsg, longMsg, timeMillis, stackTrace, &crashed))) {
                mController = NULL;
                Watchdog::GetInstance()->SetActivityController(NULL);
            }
            else if (!crashed) {
                AutoPtr<ISystemProperties> sysProps;
                CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
                String value;
                sysProps->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
                String className;
                if (value.Equals("1") && (crashInfo->GetExceptionClassName(&className),
                        className.Equals("Native crash"))) {
                    Slogger::W(TAG, "Skip killing native crashed app %s(%d) during testing",
                        name.string(), pid);
                }
                else {
                    Slogger::W(TAG, "Force-killing crashed app %s at watcher's request", name.string());
                    if (r != NULL) {
                        r->Kill(String("crash"), TRUE);
                    }
                    else {
                        // Huh.
                        Process::KillProcess(pid);
                        Process::KillProcessGroup(uid, pid);
                    }
                }

                Process::KillProcess(pid);
                return NOERROR;
            }
        }

        const Int64 origId = Binder::ClearCallingIdentity();

        // If this process is running instrumentation, finish it.
        if (r != NULL && r->mInstrumentationClass != NULL) {
            Slogger::W(TAG, "Error in app %s running instrumentation %p:", r->mProcessName.string(), r->mInstrumentationClass.Get());
            if (!shortMsg.IsNull()) Slogger::W(TAG, "  %s", shortMsg.string());
            if (!longMsg.IsNull()) Slogger::W(TAG, "  %s", longMsg.string());
            AutoPtr<IBundle> info;
            CBundle::New((IBundle**)&info);
            info->PutString(String("shortMsg"), shortMsg);
            info->PutString(String("longMsg"), longMsg);
            FinishInstrumentationLocked(r, IActivity::RESULT_CANCELED, info);
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }

        // If we can't identify the process or it's already exceeded its crash quota,
        // quit right away without showing a crash dialog.
        if (r == NULL || !MakeAppCrashingLocked(r, shortMsg, longMsg, stackTrace)) {
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }

        AutoPtr<StringObjectHashMap> data = new StringObjectHashMap();
        (*data)[String("result")] = result ? result->Probe(EIID_IInterface) : NULL;
        (*data)[String("app")] = r ? r->Probe(EIID_IInterface) : NULL;

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(SHOW_ERROR_MSG, data->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean bval;
        mHandler->SendMessage(msg, &bval);

        Binder::RestoreCallingIdentity(origId);
    }

    Int32 res = result->GetResult();

    AutoPtr<IIntent> appErrorIntent;
    {
        AutoLock lock(this);
        if (r != NULL && !r->mIsolated) {
            // XXX Can't keep track of crash time for isolated processes,
            // since they don't have a persistent identity.
            String processName;
            r->mInfo->GetProcessName(&processName);
            mProcessCrashTimes->Put(processName, r->mUid,
                    SystemClock::GetUptimeMillis());
        }
        if (res == AppErrorDialog::FORCE_QUIT_AND_REPORT) {
            appErrorIntent = CreateAppErrorIntentLocked(r, timeMillis, crashInfo);
        }
    }

    if (appErrorIntent != NULL) {
        // try {
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(r->mUserId, (IUserHandle**)&userHandle);
        FAIL_RETURN(mContext->StartActivityAsUser(appErrorIntent, userHandle));
        // } catch (ActivityNotFoundException e) {
        //     Slogger::W(TAG, "bug report receiver dissappeared", e);
        // }
    }
    return NOERROR;
}

AutoPtr<IIntent> CActivityManagerService::CreateAppErrorIntentLocked(
    /* [in] */ ProcessRecord* r,
    /* [in] */ Int64 timeMillis,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    AutoPtr<IApplicationErrorReport> report = CreateAppErrorReportLocked(r, timeMillis, crashInfo);
    if (report == NULL) {
        return NULL;
    }
    AutoPtr<IIntent> result;
    CIntent::New(IIntent::ACTION_APP_ERROR, (IIntent**)&result);
    result->SetComponent(r->mErrorReportReceiver);
    result->PutExtra(IIntent::EXTRA_BUG_REPORT, IParcelable::Probe(report));
    result->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return result;
}

AutoPtr<IApplicationErrorReport> CActivityManagerService::CreateAppErrorReportLocked(
    /* [in] */ ProcessRecord* r,
    /* [in] */ Int64 timeMillis,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    if (r->mErrorReportReceiver == NULL) {
        return NULL;
    }

    if (!r->mCrashing && !r->mNotResponding  && !r->mForceCrashReport) {
        return NULL;
    }

    AutoPtr<IApplicationErrorReport> report;
    CApplicationErrorReport::New((IApplicationErrorReport**)&report);
    String rPkgName;
    IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&rPkgName);
    report->SetPackageName(rPkgName);
    String rErrorPkgName;
    r->mErrorReportReceiver->GetPackageName(&rErrorPkgName);
    report->SetInstallerPackageName(rErrorPkgName);
    report->SetProcessName(r->mProcessName);
    report->SetTime(timeMillis);
    Int32 rFlags;
    r->mInfo->GetFlags(&rFlags);
    report->SetSystemApp((rFlags & IApplicationInfo::FLAG_SYSTEM) != 0);

    if (r->mCrashing || r->mForceCrashReport) {
        report->SetType(IApplicationErrorReport::TYPE_CRASH);
        report->SetCrashInfo(crashInfo);
    }
    else if (r->mNotResponding) {
        report->SetType(IApplicationErrorReport::TYPE_ANR);
        AutoPtr<IApplicationErrorReportAnrInfo> anrInfo;
        CApplicationErrorReportAnrInfo::New((IApplicationErrorReportAnrInfo**)&anrInfo);
        report->SetAnrInfo(anrInfo);

        String tag;
        r->mNotRespondingReport->GetTag(&tag);
        anrInfo->SetActivity(tag);
        String shortMsg;
        r->mNotRespondingReport->GetShortMsg(&shortMsg);
        anrInfo->SetCause(shortMsg);
        String longMsg;
        r->mNotRespondingReport->GetLongMsg(&longMsg);
        anrInfo->SetInfo(longMsg);
    }

    return report;
}

ECode CActivityManagerService::GetProcessesInErrorState(
    /* [out] */ IList** procs)
{
    VALIDATE_NOT_NULL(procs);
    *procs = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessesInErrorState")));
    // assume our apps are happy - lazy create the list
    CParcelableList::New(procs);

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 permission;
    helper->CheckUidPermission(
           Manifest::permission::INTERACT_ACROSS_USERS_FULL,
           Binder::GetCallingUid(), &permission);
    const Boolean allUsers = permission == IPackageManager::PERMISSION_GRANTED;
    Int32 userId = UserHandle::GetUserId(Binder::GetCallingUid());

    {
        AutoLock lock(this);

       // iterate across all processes
        List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if (!allUsers && app->mUserId != userId) {
                continue;
            }
            if ((app->mThread != NULL) && (app->mCrashing || app->mNotResponding)) {
                // This one's in trouble, so we'll generate a report for it
                // crashes are higher priority (in case there's a crash *and* an anr)
                AutoPtr<IActivityManagerProcessErrorStateInfo> report;
                if (app->mCrashing) {
                   report = app->mCrashingReport;
                }
                else if (app->mNotResponding) {
                    report = app->mNotRespondingReport;
                }

                if (report != NULL) {
                    (*procs)->Add(report);
                }
                else {
                    Slogger::W(TAG, "Missing app error report, app = %s crashing = %d notResponding = %d", app->mProcessName.string(),
                            app->mCrashing, app->mNotResponding);
                }
            }
        }
    }

    return NOERROR;
}

Int32 CActivityManagerService::ProcStateToImportance(
    /* [in] */ Int32 procState,
    /* [in] */ Int32 memAdj,
    /* [in] */ IActivityManagerRunningAppProcessInfo* currApp)
{
    AutoPtr<IActivityManagerRunningAppProcessInfoHelper> amrapHelper;
    CActivityManagerRunningAppProcessInfoHelper::AcquireSingleton(
        (IActivityManagerRunningAppProcessInfoHelper**)&amrapHelper);
    Int32 ival = 0;
    amrapHelper->ProcStateToImportance(procState, &ival);
    if (ival == IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND) {
        currApp->SetLru(memAdj);
    }
    else {
        currApp->SetLru(0);
    }
    return ival;
}

ECode CActivityManagerService::FillInProcMemInfo(
    /* [in] */ ProcessRecord* app,
    /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo)
{
    outInfo->SetPid(app->mPid);
    Int32 uid = 0;
    app->mInfo->GetUid(&uid);
    outInfo->SetUid(uid);

    Int32 flag = 0;
    if (mHeavyWeightProcess.Get() == app) {
        outInfo->GetFlags(&flag);
        outInfo->SetFlags(flag | IActivityManagerRunningAppProcessInfo::FLAG_CANT_SAVE_STATE);
    }

    if (app->mPersistent) {
        outInfo->GetFlags(&flag);
        outInfo->SetFlags(flag | IActivityManagerRunningAppProcessInfo::FLAG_PERSISTENT);
    }

    if (app->mActivities.GetSize() > 0) {
        outInfo->GetFlags(&flag);
        outInfo->SetFlags(flag | IActivityManagerRunningAppProcessInfo::FLAG_HAS_ACTIVITIES);
    }

    outInfo->SetLastTrimLevel(app->mTrimMemoryLevel);
    Int32 adj = app->mCurAdj;
    Int32 procState = app->mCurProcState;

    outInfo->SetImportance(ProcStateToImportance(procState, adj, outInfo));
    outInfo->SetImportanceReasonCode(app->mAdjTypeCode);
    outInfo->SetProcessState(app->mCurProcState);

    return NOERROR;
}

ECode CActivityManagerService::GetRunningAppProcesses(
    /* [out] */ IList** appProcs)
{
    VALIDATE_NOT_NULL(appProcs);

    *appProcs = NULL;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getRunningAppProcesses")));

    Int32 callingUid = Binder::GetCallingUid();

    // Lazy instantiation of lists
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 permission;
    FAIL_RETURN(helper->CheckUidPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL,
            callingUid, &permission));
    Boolean allUsers = permission == IPackageManager::PERMISSION_GRANTED;
    Int32 userId = UserHandle::GetUserId(callingUid);
    Boolean allUids = IsGetTasksAllowed(String("getRunningAppProcesses"),
        Binder::GetCallingPid(), callingUid);
    {
        AutoLock lock(this);
        // Iterate across all processes
        List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if ((!allUsers && app->mUserId != userId)
                || (!allUids && app->mUid != callingUid)) {
                continue;
            }
            if ((app->mThread != NULL) && (!app->mCrashing && !app->mNotResponding)) {
                // Generate process state info for running application
                AutoPtr<IActivityManagerRunningAppProcessInfo> currApp;
                CActivityManagerRunningAppProcessInfo::New(app->mProcessName,
                        app->mPid, app->GetPackageList(), (IActivityManagerRunningAppProcessInfo**)&currApp);
                FillInProcMemInfo(app, currApp);
                if (app->mAdjSource != NULL) {
                    if (IProcessRecord::Probe(app->mAdjSource) != NULL) {
                        ProcessRecord* pr = (ProcessRecord*)IProcessRecord::Probe(app->mAdjSource);
                        currApp->SetImportanceReasonPid(pr->mPid);
                        Int32 imp = 0;
                        AutoPtr<IActivityManagerRunningAppProcessInfoHelper> amrapHelper;
                        CActivityManagerRunningAppProcessInfoHelper::AcquireSingleton(
                            (IActivityManagerRunningAppProcessInfoHelper**)&amrapHelper);
                        amrapHelper->ProcStateToImportance(app->mAdjSourceProcState, &imp);
                        currApp->SetImportanceReasonImportance(imp);
                    }
                    else if (IActivityRecord::Probe(app->mAdjSource) != NULL) {
                        ActivityRecord* r = (ActivityRecord*)IActivityRecord::Probe(app->mAdjSource);
                        if (r->mApp != NULL) currApp->SetImportanceReasonPid(r->mApp->mPid);
                    }
                }
                if (IComponentName::Probe(app->mAdjTarget) != NULL) {
                    currApp->SetImportanceReasonComponent(IComponentName::Probe(app->mAdjTarget));
                }
                //Slogger::v(TAG, "Proc " + app->processName + ": imp=" + currApp.importance
                //        + " lru=" + currApp.lru);
                if (*appProcs == NULL) {
                     FAIL_RETURN(CParcelableList::New(appProcs));
                }
                (*appProcs)->Add(currApp);
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetRunningExternalApplications(
    /* [out] */ IList** apps)
{
    VALIDATE_NOT_NULL(apps);
    *apps = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getRunningExternalApplications")));
    AutoPtr<IList> runningApps;
    GetRunningAppProcesses((IList**)&runningApps);
    CParcelableList::New(apps);
    Int32 appsCount;
    if (runningApps != NULL && (runningApps->GetSize(&appsCount), appsCount > 0)) {
        HashSet<String> extList;
        for (Int32 i = 0; i < appsCount; i++) {
            AutoPtr<IInterface> obj;
            runningApps->Get(i, (IInterface**)&obj);
            AutoPtr<IActivityManagerRunningAppProcessInfo> app = IActivityManagerRunningAppProcessInfo::Probe(obj);
            AutoPtr<ArrayOf<String> > appPkgList;
            app->GetPkgList((ArrayOf<String>**)&appPkgList);
            if (appPkgList != NULL) {
                for (Int32 i = 0; i < appPkgList->GetLength(); ++i) {
                    String pkg = (*appPkgList)[i];
                    extList.Insert(pkg);
                }
            }
        }
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        HashSet<String>::Iterator it;
        for (it = extList.Begin(); it != extList.End(); ++it) {
            String pkg = *it;
            // try {
            AutoPtr<IApplicationInfo> info;
            pm->GetApplicationInfo(pkg, 0, UserHandle::GetCallingUserId(), (IApplicationInfo**)&info);
            Int32 infoFlags;
            info->GetFlags(&infoFlags);
            if ((infoFlags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
                (*apps)->Add(info);
            }
           // } catch (RemoteException e) {
           // }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetMyMemoryState(
    /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getMyMemoryState")));
    {
        AutoLock lock(this);
        AutoPtr<ProcessRecord> proc;
        {
            AutoLock lock(mPidsSelfLockedLock);
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(Binder::GetCallingPid());
            if (it != mPidsSelfLocked.End())
                proc = it->mSecond;
        }
        FillInProcMemInfo(proc, outInfo);
    }
    return NOERROR;
}

ECode CActivityManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    if (CheckCallingPermission(Manifest::permission::DUMP)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("Permission Denial: can't dump ActivityManager from from pid=");
        sb += Binder::GetCallingPid();
        sb += ", uid=";
        sb += Binder::GetCallingUid();
        sb += " without permission ";
        sb += "Manifest::permission::DUMP";
        pw->Println(sb.ToString());
        return NOERROR;
    }

    Boolean dumpAll = FALSE;
    Boolean dumpClient = FALSE;
    String dumpPackage;

    Int32 opti = 0;
    while (opti < args->GetLength()) {
        String opt = (*args)[opti];
        if (opt.IsNull() || opt.GetLength() <= 0 || opt.GetChar(0) != '-') {
            break;
        }
        opti++;
        if (opt.Equals("-a")) {
            dumpAll = TRUE;
        }
        else if (opt.Equals("-c")) {
            dumpClient = TRUE;
        }
        else if (opt.Equals("-h")) {
            pw->Println(String("Activity manager dump options:"));
            pw->Println(String("  [-a] [-c] [-h] [cmd] ..."));
            pw->Println(String("  cmd may be one of:"));
            pw->Println(String("    a[ctivities]: activity stack state"));
            pw->Println(String("    r[recents]: recent activities state"));
            pw->Println(String("    b[roadcasts] [PACKAGE_NAME] [history [-s]]: broadcast state"));
            pw->Println(String("    i[ntents] [PACKAGE_NAME]: pending intent state"));
            pw->Println(String("    p[rocesses] [PACKAGE_NAME]: process state"));
            pw->Println(String("    o[om]: out of memory management"));
            pw->Println(String("    prov[iders] [COMP_SPEC ...]: content provider state"));
            pw->Println(String("    provider [COMP_SPEC]: provider client-side state"));
            pw->Println(String("    s[ervices] [COMP_SPEC ...]: service state"));
            pw->Println(String("    service [COMP_SPEC]: service client-side state"));
            pw->Println(String("    package [PACKAGE_NAME]: all state related to given package"));
            pw->Println(String("    all: dump all activities"));
            pw->Println(String("    top: dump the top activity"));
            pw->Println(String("    write: write all pending state to storage"));
            pw->Println(String("  cmd may also be a COMP_SPEC to dump activities."));
            pw->Println(String("  COMP_SPEC may be a component name (com.foo/.myApp),"));
            pw->Println(String("    a partial substring in a component name, a"));
            pw->Println(String("    hex object identifier."));
            pw->Println(String("  -a: include all available server state."));
            pw->Println(String("  -c: include client state."));
            return NOERROR;
        }
        else {
            pw->Println(String("Unknown argument: ") + opt + "; use -h for help");
        }
    }

    Int64 origId = Binder::ClearCallingIdentity();
    Boolean more = FALSE;
    // Is the caller requesting to dump a particular piece of data?
    if (opti < args->GetLength()) {
        String cmd = (*args)[opti];
        opti++;
        if (cmd.Equals("activities") || cmd.Equals("a")) {
            AutoLock lock(this);
            DumpActivitiesLocked(fd, pw, args, opti, TRUE, dumpClient, String(NULL));
        }
         else if (cmd.Equals("recents") || cmd.Equals("r")) {
            {    AutoLock syncLock(this);
                DumpRecentsLocked(fd, pw, args, opti, TRUE, String(NULL));
            }
        }
        else if (cmd.Equals("broadcasts") || cmd.Equals("b")) {
            AutoPtr<ArrayOf<String> > newArgs;
            String name;
            if (opti >= args->GetLength()) {
                name = NULL;
                newArgs = EMPTY_STRING_ARRAY;
            }
            else {
                name = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
            }
            {
                AutoLock lock(this);
                DumpBroadcastsLocked(fd, pw, args, opti, TRUE, name);
            }
        }
        else if (cmd.Equals("intents") || cmd.Equals("i")) {
            AutoPtr<ArrayOf<String> > newArgs;
            String name;
            if (opti >= args->GetLength()) {
                name = NULL;
                newArgs = EMPTY_STRING_ARRAY;
            }
            else {
                name = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
            }
            {
                AutoLock lock(this);
                DumpPendingIntentsLocked(fd, pw, args, opti, TRUE, name);
            }
        }
        else if (cmd.Equals("processes") || cmd.Equals("p")) {
            AutoPtr<ArrayOf<String> > newArgs;
            String name;
            if (opti >= args->GetLength()) {
                name = NULL;
                newArgs = EMPTY_STRING_ARRAY;
            }
            else {
                name = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
            }
            {
                AutoLock lock(this);
                DumpProcessesLocked(fd, pw, args, opti, TRUE, name);
            }
        }
        else if (cmd.Equals("oom") || cmd.Equals("o")) {
            {
                AutoLock lock(this);
                DumpOomLocked(fd, pw, args, opti, TRUE);
            }
        }
        else if (cmd.Equals("provider")) {
            AutoPtr<ArrayOf<String> > newArgs;
            String name;
            if (opti >= args->GetLength()) {
                name = NULL;
                newArgs = EMPTY_STRING_ARRAY;
            }
            else {
                name = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
            }
            if (!DumpProvider(fd, pw, name, newArgs, 0, dumpAll)) {
                pw->Println(String("No providers match: ") + name);
                pw->Println(String("Use -h for help."));
            }
        }
        else if (cmd.Equals("providers") || cmd.Equals("prov")) {
            AutoLock lock(this);
            DumpProvidersLocked(fd, pw, args, opti, TRUE, String(NULL));
        }
        else if (cmd.Equals("service")) {
            AutoPtr<ArrayOf<String> > newArgs;
            String name;
            if (opti >= args->GetLength()) {
                name = NULL;
                newArgs = EMPTY_STRING_ARRAY;
            }
            else {
                name = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
            }
            if (!mServices->DumpService(fd, pw, name, newArgs, 0, dumpAll)) {
                pw->Println(String("No services match: ") + name);
                pw->Println(String("Use -h for help."));
            }
        }
        else if (cmd.Equals("package")) {
            AutoPtr<ArrayOf<String> > newArgs;
            if (opti >= args->GetLength()) {
                pw->Println(String("package: no package name specified"));
                pw->Println(String("Use -h for help."));
            }
            else {
                dumpPackage = (*args)[opti];
                opti++;
                newArgs = ArrayOf<String>::Alloc(args->GetLength() - opti);
                if (args->GetLength() > 2) newArgs->Copy(args, opti, args->GetLength() - opti);
                args = newArgs;
                opti = 0;
                more = TRUE;
            }
        }
        else if (cmd.Equals("services") || cmd.Equals("s")) {
            AutoLock lock(this);
            mServices->DumpServicesLocked(fd, pw, args, opti, TRUE, dumpClient, String(NULL));
        }
        else if (cmd.Equals("write")) {
            mTaskPersister->Flush();
            pw->Println(String("All tasks persisted."));
            return NOERROR;
        }
        else {
            // Dumping a single activity?
            if (!DumpActivity(fd, pw, cmd, args, opti, dumpAll)) {
                pw->Println(String("Bad activity command, or no activities match: ") + cmd);
                pw->Println(String("Use -h for help."));
            }
        }
        if (!more) {
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }
    }

    // No piece of data specified, dump everything.
    {
        AutoLock lock(this);
        DumpPendingIntentsLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        DumpBroadcastsLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        DumpProvidersLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        mServices->DumpServicesLocked(fd, pw, args, opti, dumpAll, dumpClient, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        DumpRecentsLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        DumpActivitiesLocked(fd, pw, args, opti, dumpAll, dumpClient, dumpPackage);
        pw->Println();
        if (dumpAll) {
            pw->Println(String("-------------------------------------------------------------------------------"));
        }
        DumpProcessesLocked(fd, pw, args, opti, dumpAll, dumpPackage);
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

void CActivityManagerService::DumpActivitiesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean dumpClient,
    /* [in] */ const String& dumpPackage)
{
    pw->Println(String("ACTIVITY MANAGER ACTIVITIES (dumpsys activity activities)"));

    Boolean printedAnything = mStackSupervisor->DumpActivitiesLocked(
        fd, pw, dumpAll, dumpClient, dumpPackage);
    Boolean needSep = printedAnything;

    Boolean printed = ActivityStackSupervisor::PrintThisActivity(pw, mFocusedActivity,
            dumpPackage, needSep, String("  mFocusedActivity: "));
    if (printed) {
        printedAnything = TRUE;
        needSep = FALSE;
    }

    if (dumpPackage == NULL) {
        if (needSep) {
            pw->Println();
        }
        needSep = TRUE;
        printedAnything = TRUE;
        mStackSupervisor->Dump(pw, String("  "));
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

void CActivityManagerService::DumpRecentsLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    pw->Println(String("ACTIVITY MANAGER ACTIVITIES (dumpsys activity recents)"));

    Boolean printedAnything = FALSE;

    if (mRecentTasks != NULL && mRecentTasks->GetSize() > 0) {
        Boolean printedHeader = FALSE;

        List<AutoPtr<TaskRecord> >::Iterator it = mRecentTasks->Begin();
        for (Int32 i = 0; it != mRecentTasks->End(); ++it, ++i) {
            AutoPtr<TaskRecord> tr = *it;
            if (dumpPackage != NULL) {
                String packageName;
                if (tr->mRealActivity == NULL || (tr->mRealActivity->GetPackageName(&packageName),
                    !dumpPackage.Equals(packageName))) {
                    continue;
                }
            }
            if (!printedHeader) {
                pw->Println(String("  Recent tasks:"));
                printedHeader = TRUE;
                printedAnything = TRUE;
            }
            pw->Print(String("  * Recent #"));
            pw->Print(i);
            pw->Print(String(": "));
            pw->Println(tr->ToString());
            if (dumpAll) {
                tr->Dump(pw, String("    "));
            }
        }
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

void CActivityManagerService::DumpProcessesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Boolean printedAnything = FALSE;
    Int32 numPers = 0;

    pw->Println(String("ACTIVITY MANAGER RUNNING PROCESSES (dumpsys activity processes)"));

    if (dumpAll) {
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > map;
        map = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator mapIt;
        for (mapIt = map->Begin(); mapIt != map->End(); ++mapIt) {
            AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > procs = mapIt->mSecond;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator procsIt;
            for (procsIt = procs->Begin(); procsIt != procs->End(); ++procsIt) {
                AutoPtr<ProcessRecord> r = procsIt->mSecond;
                Boolean res;
                if (!dumpPackage.IsNull() && !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res)) {
                    continue;
                }
                if (!needSep) {
                    pw->Println(String("  All known processes:"));
                    needSep = TRUE;
                    printedAnything = TRUE;
                }
                pw->Print(r->mPersistent ? String("  *PERS*") : String("  *APP*"));
                pw->Print(String(" UID "));
                pw->Print(procsIt->mFirst);
                pw->Print(String(" "));
                pw->Println(r->ToString());
                r->Dump(pw, String("    "));
                if (r->mPersistent) {
                    numPers++;
                }
            }
        }
    }

    if (mIsolatedProcesses.IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        Int32 i = 0;
        for (it = mIsolatedProcesses.Begin(); it != mIsolatedProcesses.End(); ++it, ++i) {
            AutoPtr<ProcessRecord> r = it->mSecond;
            Boolean res;
            if (!dumpPackage.IsNull() && !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res)) {
                continue;
            }
            if (!printed) {
                if (needSep) {
                    pw->Println();
                }
                pw->Println(String("  Isolated process list (sorted by uid):"));
                printedAnything = TRUE;
                printed = TRUE;
                needSep = TRUE;
            }
            String s;
            s.AppendFormat("    Isolated #%2d: %s", i, r->ToString().string());
            pw->Println(s);
        }
    }

    if (mLruProcesses.IsEmpty() == FALSE) {
        if (needSep) {
            pw->Println();
        }
        pw->Print(String("  Process LRU list (sorted by oom_adj, "));
        pw->Print((Int32)mLruProcesses.GetSize());
        pw->Print(String(" total, non-act at "));
        pw->Print((Int32)mLruProcesses.GetSize()-mLruProcessActivityStart);
        pw->Print(String(", non-svc at "));
        pw->Print((Int32)mLruProcesses.GetSize()-mLruProcessServiceStart);
        pw->Println(String("):"));
        DumpProcessOomList(pw, this, &mLruProcesses, String("    "),
                String("Proc"), String("PERS"), FALSE, dumpPackage);
        needSep = TRUE;
        printedAnything = TRUE;
    }

    if (dumpAll || dumpPackage != NULL) {
        AutoLock lock(mPidsSelfLockedLock);
        Boolean printed = FALSE;
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
            AutoPtr<ProcessRecord> r = it->mSecond;
            String rPkgName;
            IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&rPkgName);
            Boolean res;
            if (!dumpPackage.IsNull() && !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res)) {
                continue;
            }
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  PID mappings:"));
                printed = TRUE;
                printedAnything = TRUE;
            }
            pw->Print(String("    PID #"));
            pw->Print(it->mFirst);
            pw->Print(String(": "));
            pw->Println(r->ToString());
        }
    }

    if (mForegroundProcesses.IsEmpty() == FALSE) {
        AutoLock lock(mPidsSelfLockedLock);
        Boolean printed = FALSE;
        HashMap<Int32, AutoPtr<ForegroundToken> >::Iterator it;
        for (it = mForegroundProcesses.Begin(); it != mForegroundProcesses.End(); ++it) {
            AutoPtr<ProcessRecord> r;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator pit =
                    mPidsSelfLocked.Find(it->mSecond->mPid);
            if (pit != mPidsSelfLocked.End()) {
                r = pit->mSecond;
            }
            Boolean res;
            if (!dumpPackage.IsNull() && (r == NULL
                || !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
                continue;
            }
            if (!printed) {
                if (needSep)
                    pw->Println();
                needSep = TRUE;
                pw->Println(String("  Foreground Processes:"));
                printed = TRUE;
                printedAnything = TRUE;
            }
            pw->Print(String("    PID #"));
            pw->Print(it->mFirst);
            pw->Print(String(": "));
            pw->Println(it->mSecond->Probe(EIID_IInterface));
        }
    }

    Int32 size;
    if (mPersistentStartingProcesses->GetSize(&size), size > 0) {
        if (needSep) pw->Println();
        needSep = TRUE;
        printedAnything = TRUE;
        pw->Println(String("  Persisent processes that are starting:"));
        DumpProcessList(pw, this, mPersistentStartingProcesses, String("    "),
                String("Starting Norm"), String("Restarting PERS"), dumpPackage);
    }

    if (mRemovedProcesses->GetSize(&size), size > 0) {
        if (needSep) pw->Println();
        needSep = TRUE;
        printedAnything = TRUE;
        pw->Println(String("  Processes that are being removed:"));
        DumpProcessList(pw, this, mRemovedProcesses, String("    "),
                String("Removed Norm"), String("Removed PERS"), dumpPackage);
    }

    if (mProcessesOnHold->GetSize(&size), size > 0) {
        if (needSep) pw->Println();
        needSep = TRUE;
        printedAnything = TRUE;
        pw->Println(String("  Processes that are on old until the system is ready:"));
        DumpProcessList(pw, this, mProcessesOnHold, String("    "),
                String("OnHold Norm"), String("OnHold PERS"), dumpPackage);
    }

    needSep = DumpProcessesToGc(fd, pw, args, opti, needSep, dumpAll, dumpPackage);

    AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, Int64> > > > tMap = mProcessCrashTimes->GetMap();
    if (tMap->IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        Int64 now = SystemClock::GetUptimeMillis();
        HashMap<String, AutoPtr<HashMap<Int32, Int64> > >::Iterator mapIt;
        for (mapIt = tMap->Begin(); mapIt != tMap->End(); ++mapIt) {
            String pname = mapIt->mFirst;
            AutoPtr<HashMap<Int32, Int64> > uids = mapIt->mSecond;
            HashMap<Int32, Int64>::Iterator uidsIt;
            for (uidsIt = uids->Begin(); uidsIt != uids->End(); ++uidsIt) {
                Int32 puid = uidsIt->mFirst;
                AutoPtr<ProcessRecord> r = mProcessNames->Get(pname, puid);
                Boolean res;
                if (!dumpPackage.IsNull() && (r == NULL
                    || !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->Println();
                    needSep = TRUE;
                    pw->Println(String("  Time since processes crashed:"));
                    printed = TRUE;
                    printedAnything = TRUE;
                }
                pw->Print(String("    Process "));
                pw->Print(pname);
                pw->Print(String(" uid "));
                pw->Print(puid);
                pw->Print(String(": last crashed "));
                TimeUtils::FormatDuration(now - uidsIt->mSecond, pw);
                pw->Println(String(" ago"));
            }
        }
    }

    AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<BadProcessInfo> > > > > badMap = mBadProcesses->GetMap();
    if (badMap->IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<BadProcessInfo> > > >::Iterator badMapIt;
        for (badMapIt = badMap->Begin(); badMapIt != badMap->End(); ++badMapIt) {
            String pname = badMapIt->mFirst;
            AutoPtr<HashMap<Int32, AutoPtr<BadProcessInfo> > > uids = badMapIt->mSecond;
            HashMap<Int32, AutoPtr<BadProcessInfo> >::Iterator uidsIt;
            for (uidsIt = uids->Begin(); uidsIt != uids->End(); ++uidsIt) {
                Int32 puid = uidsIt->mFirst;
                AutoPtr<ProcessRecord> r = mProcessNames->Get(pname, puid);
                Boolean res;
                if (!dumpPackage.IsNull() && (r == NULL
                    || !(r->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->Println();
                    needSep = TRUE;
                    pw->Println(String("  Bad processes:"));
                    printedAnything = TRUE;
                }
                AutoPtr<BadProcessInfo> info = uidsIt->mSecond;
                pw->Print(String("    Bad process "));
                pw->Print(pname);
                pw->Print(String(" uid "));
                pw->Print(puid);
                pw->Print(String(": crashed at time "));
                pw->Println(info->mTime);
                if (info->mShortMsg != NULL) {
                    pw->Print(String("      Short msg: "));
                    pw->Println(info->mShortMsg);
                }
                if (info->mLongMsg != NULL) {
                    pw->Print(String("      Long msg: "));
                    pw->Println(info->mLongMsg);
                }
                if (info->mStack != NULL) {
                    pw->Println(String("      Stack:"));
                    Int32 lastPos = 0;
                    for (Int32 pos = 0; pos < info->mStack.GetLength(); pos++) {
                        if (info->mStack.GetChar(pos) == '\n') {
                            pw->Print(String("        "));
                            IWriter::Probe(pw)->Write(info->mStack.GetChars(), lastPos, pos-lastPos);
                            pw->Println();
                            lastPos = pos + 1;
                        }
                    }
                    if (lastPos < info->mStack.GetLength()) {
                        pw->Print(String("        "));
                        IWriter::Probe(pw)->Write(info->mStack.GetChars(), lastPos, info->mStack.GetLength()-lastPos);
                        pw->Println();
                    }
                }
            }
        }
    }

    if (dumpPackage == NULL) {
        pw->Println();
        needSep = FALSE;
        pw->Println(String("  mStartedUsers:"));
        HashMap<Int32, AutoPtr<UserStartedState> >::Iterator uIt;
        for (uIt = mStartedUsers.Begin(); uIt != mStartedUsers.End(); ++uIt) {
            AutoPtr<UserStartedState> uss = uIt->mSecond;
            pw->Print(String("    User #"));
            Int32 id;
            uss->mHandle->GetIdentifier(&id);
            pw->Print(id);
            pw->Print(String(": "));
            uss->Dump(String(""), pw);
        }
        pw->Print(String("  mStartedUserArray: ["));
        for (Int32 i = 0; i < mStartedUserArray->GetLength(); i++) {
            if (i > 0)
                pw->Print(String(", "));
            pw->Print((*mStartedUserArray)[i]);
        }
        pw->Println(String("]"));
        pw->Print(String("  mUserLru: ["));
        List<Int32>::Iterator userIt;
        for (userIt = mUserLru.Begin(); userIt != mUserLru.End(); ++userIt) {
            if (userIt != mUserLru.Begin())
                pw->Print(String(", "));
            pw->Print(*userIt);
        }
        pw->Println(String("]"));
        if (dumpAll) {
            pw->Print(String("  mStartedUserArray: "));
            StringBuilder sb;
            for (Int32 i = 0; i < mStartedUserArray->GetLength(); ++i) {
                sb += (*mStartedUserArray)[i];
            }
            pw->Println(sb.ToString());
        }

        {    AutoLock syncLock(mUserProfileGroupIdsSelfLockedLock);
            if (mUserProfileGroupIdsSelfLocked.GetSize() > 0) {
                pw->Println(String("  mUserProfileGroupIds:"));
                HashMap<Int32, Int32>::Iterator it = mUserProfileGroupIdsSelfLocked.Begin();
                for (; it != mUserProfileGroupIdsSelfLocked.End(); ++it) {
                    pw->Print(String("    User #"));
                    pw->Print(it->mFirst);
                    pw->Print(String(" -> profile #"));
                    pw->Println(it->mSecond);
                }
            }
        }
    }

    Boolean res;
    if (mHomeProcess != NULL && (dumpPackage == NULL
        || (mHomeProcess->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
        if (needSep) {
            pw->Println();
            needSep = FALSE;
        }
        pw->Print(String("  mHomeProcess: "));
        pw->Println(mHomeProcess->ToString());
    }
    if (mPreviousProcess != NULL && (dumpPackage == NULL
        || (mPreviousProcess->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
        if (needSep) {
            pw->Println();
            needSep = FALSE;
        }
        pw->Println(String("  mPreviousProcess: "));
        pw->Println(mPreviousProcess->ToString());
    }
    if (dumpAll) {
        StringBuilder sb(128);
        sb.Append("  mPreviousProcessVisibleTime: ");
        TimeUtils::FormatDuration(mPreviousProcessVisibleTime, sb);
        pw->Println(sb.ToString());
    }
    if (mHeavyWeightProcess != NULL && (dumpPackage == NULL
        || (mHeavyWeightProcess->mPkgList->ContainsKey(CoreUtils::Convert(dumpPackage), &res), res))) {
         if (needSep) {
            pw->Println();
            needSep = FALSE;
        }
        pw->Println(String("  mHeavyWeightProcess: "));
        pw->Println(mHeavyWeightProcess->ToString());
    }
    if (dumpPackage == NULL) {
        String configString;
        mConfiguration->ToString(&configString);
        pw->Println(String("  mConfiguration: ") + configString);
    }
    if (dumpAll) {
        StringBuilder sb("  mConfigWillChange: ");
        sb += GetFocusedStack()->mConfigWillChange;
        pw->Println(sb.ToString());
        HashMap<String, Int32>& packages = mCompatModePackages->GetPackages();
        if (packages.IsEmpty() == FALSE) {
            Boolean printed = FALSE;
            HashMap<String, Int32>::Iterator it;
            for (it = packages.Begin(); it != packages.End(); ++it) {
                String pkg = it->mFirst;
                Int32 mode = it->mSecond;
                if (!dumpPackage.IsNull() && !dumpPackage.Equals(pkg)) {
                    continue;
                }
                if (!printed) {
                    pw->Println(String("  mScreenCompatPackages:"));
                    printed = TRUE;
                }
                pw->Print(String("    "));
                pw->Print(pkg);
                pw->Print(String(": "));
                pw->Print(mode);
                pw->Println();
            }
        }
    }
    if (dumpPackage == NULL) {
        if (mSleeping || mWentToSleep || mLockScreenShown) {
            StringBuilder sb("  mSleeping=");
            sb += mSleeping;
            sb += " mWentToSleep=";
            sb += mWentToSleep;
            sb += " mLockScreenShown ";
            sb += mLockScreenShown;
            pw->Println(sb.ToString());
        }
        if (mShuttingDown || mRunningVoice) {
            StringBuilder sb("  mShuttingDown=");
            sb += mShuttingDown;
            sb += "  mRunningVoice=";
            sb += mRunningVoice;
            pw->Println(sb.ToString());
        }
    }
    if (!mDebugApp.IsNull() || !mOrigDebugApp.IsNull() || mDebugTransient
            || mOrigWaitForDebugger) {
        if (dumpPackage == NULL || dumpPackage.Equals(mDebugApp)
            || dumpPackage.Equals(mOrigDebugApp)) {
            if (needSep) {
                pw->Println();
                needSep = FALSE;
            }
            StringBuilder sb("  mDebugApp=");
            sb += mDebugApp + "/orig=" + mOrigDebugApp + " mDebugTransient=";
            sb += mDebugTransient;
            sb += " mOrigWaitForDebugger=";
            sb += mOrigWaitForDebugger;
            pw->Println(sb.ToString());
        }
    }
    if (!mOpenGlTraceApp.IsNull()) {
        if (dumpPackage == NULL || dumpPackage.Equals(mOpenGlTraceApp)) {
            if (needSep) {
                pw->Println();
                needSep = FALSE;
            }
            pw->Println(String("  mOpenGlTraceApp=") + mOpenGlTraceApp);
        }
    }
    if (!mProfileApp.IsNull() || mProfileProc != NULL || !mProfileFile.IsNull()
            || mProfileFd != NULL) {
        if (dumpPackage == NULL || dumpPackage.Equals(mProfileApp)) {
            if (needSep) {
                pw->Println();
                needSep = FALSE;
            }
            pw->Println(String("  mProfileApp=") + mProfileApp + " mProfileProc=" + mProfileProc->ToString());
            StringBuilder sb("  mProfileFile=");
            sb += mProfileFile + " mProfileFd=";
            sb += mProfileFd;
            pw->Println(sb.ToString());
            StringBuilder sb1("  mSamplingInterval=");
            sb1 += mSamplingInterval;
            sb1 += " mAutoStopProfiler=";
            sb1 += mAutoStopProfiler;
            pw->Println(sb1.ToString());
            StringBuilder sb2("  mProfileType=");
            sb2 += mProfileType;
            pw->Println(sb2.ToString());
        }
    }
    if (dumpPackage == NULL) {
        if (mAlwaysFinishActivities || mController != NULL) {
            StringBuilder sb("  mAlwaysFinishActivities=");
            sb += mAlwaysFinishActivities;
            sb += " mController=";
            sb += mController;
            pw->Println(sb.ToString());
        }
        if (dumpAll) {
            StringBuilder sb("  Total persistent processes: ");
            sb += numPers;
            pw->Println(sb.ToString());
            StringBuilder sb1;
            sb1 += " mProcessesReady=";
            sb1 += mProcessesReady;
            sb1 += " mSystemReady=";
            sb1 += mSystemReady;
            sb1 += " mBooted=";
            sb1 += mBooted;
            sb1 += " mFactoryTest=";
            sb1 += mFactoryTest;
            pw->Println(sb1.ToString());
            StringBuilder sb2("  mBooting=");
            sb2 += mBooting;
            sb2 += " mCallFinishBooting=";
            sb2 += mCallFinishBooting;
            sb2 += " mBootAnimationComplete=";
            sb2 += mBootAnimationComplete;
            pw->Println(sb2.ToString());
            pw->Print(String("  mLastPowerCheckRealtime="));
            TimeUtils::FormatDuration(mLastPowerCheckRealtime, pw);
            pw->Println(String(""));
            pw->Print(String("  mLastPowerCheckUptime="));
            TimeUtils::FormatDuration(mLastPowerCheckUptime, pw);
            pw->Println(String(""));
            StringBuilder sb3("  mGoingToSleep=");
            sb3 += mStackSupervisor->mGoingToSleep;
            pw->Println(sb3.ToString());
            StringBuilder sb4("  mLaunchingActivity=");
            sb4 += mStackSupervisor->mLaunchingActivity;
            pw->Println(sb4.ToString());
            StringBuilder sb5("  mAdjSeq=");
            sb5 += mAdjSeq;
            sb5 += " mLruSeq=";
            sb5 += mLruSeq;
            pw->Println(sb5.ToString());
            StringBuilder sb6("  mNumNonCachedProcs=");
            sb6 += mNumNonCachedProcs;
            sb6 += " (";
            sb6 += mLruProcesses.GetSize();
            sb6 += " total)";
            sb6 += " mNumCachedHiddenProcs=";
            sb6 += mNumCachedHiddenProcs;
            sb6 += " mNumServiceProcs=";
            sb6 += mNumServiceProcs;
            sb6 += " mNewNumServiceProcs=";
            sb6 += mNewNumServiceProcs;
            pw->Println(sb6.ToString());

            StringBuilder sb7("  mAllowLowerMemLevel=");
            sb7 += mAllowLowerMemLevel;
            sb7 += " mLastMemoryLevel=";
            sb7 += mLastMemoryLevel;
            sb7 += " mLastNumProcesses=";
            sb7 += mLastNumProcesses;
            pw->Println(sb7.ToString());
            Int64 now = SystemClock::GetUptimeMillis();
            pw->Print(String("  mLastIdleTime="));
            TimeUtils::FormatDuration(now, mLastIdleTime, pw);
            pw->Print(String(" mLowRamSinceLastIdle="));
            TimeUtils::FormatDuration(GetLowRamTimeSinceIdle(now), pw);
            pw->Println();
        }
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

Boolean CActivityManagerService::DumpProcessesToGc(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean needSep,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    if (mProcessesToGc.IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        Int64 now = SystemClock::GetUptimeMillis();
        List< AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mProcessesToGc.Begin(); it != mProcessesToGc.End(); ++it) {
            AutoPtr<ProcessRecord> proc = *it;
            String procPkgName;
            IPackageItemInfo::Probe(proc->mInfo)->GetPackageName(&procPkgName);
            if (!dumpPackage.IsNull() && !dumpPackage.Equals(procPkgName)) {
                continue;
            }
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  Processes that are waiting to GC:"));
                printed = TRUE;
            }
            pw->Print(String("    Process "));
            pw->Println(proc.Get()->ToString());
            pw->Print(String("      lowMem="));
            pw->Print(proc->mReportLowMemory);
            pw->Print(String(", last gced="));
            pw->Print(now - proc->mLastRequestedGc);
            pw->Print(String(" ms ago, last lowMem="));
            pw->Print(now - proc->mLastLowMemory);
            pw->Println(String(" ms ago"));

        }
    }
    return needSep;
}

void CActivityManagerService::PrintOomLevel(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& name,
    /* [in] */ Int32 adj)
{
    pw->Print(String("    "));
    if (adj >= 0) {
        pw->PrintChar(' ');
        if (adj < 10) pw->PrintChar(' ');
    }
    else {
        if (adj > -10) pw->PrintChar(' ');
    }
    pw->Print(adj);
    pw->Print(String(": "));
    pw->Print(name);
    pw->Print(String(" ("));
    pw->Print(mProcessList->GetMemLevel(adj) / 1024);
    pw->Println(String(" kB)"));
}

Boolean CActivityManagerService::DumpOomLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    Boolean needSep = FALSE;

    if (mLruProcesses.IsEmpty() == FALSE) {
        if (needSep) pw->Println();
        needSep = TRUE;
        pw->Println(String("  OOM levels:"));
        PrintOomLevel(pw, String("SYSTEM_ADJ"), ProcessList::SYSTEM_ADJ);
        PrintOomLevel(pw, String("PERSISTENT_PROC_ADJ"), ProcessList::PERSISTENT_PROC_ADJ);
        PrintOomLevel(pw, String("PERSISTENT_SERVICE_ADJ"), ProcessList::PERSISTENT_SERVICE_ADJ);
        PrintOomLevel(pw, String("FOREGROUND_APP_ADJ"), ProcessList::FOREGROUND_APP_ADJ);
        PrintOomLevel(pw, String("VISIBLE_APP_ADJ"), ProcessList::VISIBLE_APP_ADJ);
        PrintOomLevel(pw, String("PERCEPTIBLE_APP_ADJ"), ProcessList::PERCEPTIBLE_APP_ADJ);
        PrintOomLevel(pw, String("BACKUP_APP_ADJ"), ProcessList::BACKUP_APP_ADJ);
        PrintOomLevel(pw, String("HEAVY_WEIGHT_APP_ADJ"), ProcessList::HEAVY_WEIGHT_APP_ADJ);
        PrintOomLevel(pw, String("SERVICE_ADJ"), ProcessList::SERVICE_ADJ);
        PrintOomLevel(pw, String("HOME_APP_ADJ"), ProcessList::HOME_APP_ADJ);
        PrintOomLevel(pw, String("PREVIOUS_APP_ADJ"), ProcessList::PREVIOUS_APP_ADJ);
        PrintOomLevel(pw, String("SERVICE_B_ADJ"), ProcessList::SERVICE_B_ADJ);
        PrintOomLevel(pw, String("CACHED_APP_MIN_ADJ"), ProcessList::CACHED_APP_MIN_ADJ);
        PrintOomLevel(pw, String("CACHED_APP_MAX_ADJ"), ProcessList::CACHED_APP_MAX_ADJ);

        if (needSep) pw->Println();
        pw->Print(String("  Process OOM control ("));
        pw->Print((Int32)mLruProcesses.GetSize());
        pw->Print(String(" total, non-act at "));
        pw->Print((Int32)mLruProcesses.GetSize()-mLruProcessActivityStart);
        pw->Print(String(", non-svc at "));
        pw->Print((Int32)mLruProcesses.GetSize()-mLruProcessServiceStart);
        pw->Println(String("):"));
        DumpProcessOomList(pw, this, &mLruProcesses, String("    "),
                String("Proc"), String("PERS"), TRUE, String(NULL));
        needSep = TRUE;
    }

    DumpProcessesToGc(fd, pw, args, opti, needSep, dumpAll, String(NULL));

    pw->Println();
    pw->Println(String("  mHomeProcess: ") + mHomeProcess->ToString());
    pw->Println(String("  mPreviousProcess: ") + mPreviousProcess->ToString());
    if (mHeavyWeightProcess != NULL) {
        pw->Println(String("  mHeavyWeightProcess: ") + mHeavyWeightProcess->ToString());
    }

    return TRUE;
 }

Boolean CActivityManagerService::DumpProvider(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    return mProviderMap->DumpProvider(fd, pw, name, args, opti, dumpAll);
}

Boolean CActivityManagerService::DumpActivity(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    AutoPtr<IArrayList> activities;
    {    AutoLock syncLock(this);
        activities = mStackSupervisor->GetDumpActivitiesLocked(name);
    }

    Int32 size;
    activities->GetSize(&size);
    if (size <= 0) {
        return FALSE;
    }

    Int32 argsLength = args->GetLength();
    AutoPtr<ArrayOf<String> > newArgs = ArrayOf<String>::Alloc(argsLength - opti);
    newArgs->Copy(args, opti, argsLength- opti);

    AutoPtr<TaskRecord> lastTask;
    Boolean needSep = FALSE;
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> item;
        activities->Get(i, (IInterface**)&item);
        AutoPtr<ActivityRecord> r = (ActivityRecord*)IActivityRecord::Probe(item);
        if (needSep) {
            pw->Println();
        }
        needSep = TRUE;
        {
            AutoLock lock(this);
            if (lastTask != r->mTask) {
                lastTask = r->mTask;
                pw->Print(String("TASK "));
                pw->Print(lastTask->mAffinity);
                pw->Print(String(" id="));
                pw->Println(lastTask->mTaskId);
                if (dumpAll) {
                    lastTask->Dump(pw, String("  "));
                }
            }
        }
        DumpActivity(String("  "), fd, pw, r, newArgs, dumpAll);
    }
    return TRUE;
}

ECode CActivityManagerService::DumpActivity(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ActivityRecord* r,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean dumpAll)
{
    String innerPrefix = prefix + "  ";
    {
        AutoLock lock(this);
        pw->Print(prefix);
        pw->Print(String("ACTIVITY "));
        pw->Print(r->mShortComponentName);
        pw->Print(String(" "));
        pw->Print((Int32)r);
        pw->Print(String(" pid="));
        if (r->mApp != NULL) pw->Println(r->mApp->mPid);
        else pw->Println(String("(not running)"));
        if (dumpAll) {
            r->Dump(pw, innerPrefix);
        }
    }
    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        // flush anything that is already in the PrintWriter since the thread is going
        // to write to the file descriptor directly
        IFlushable::Probe(pw)->Flush();
        // try {
            AutoPtr<ITransferPipe> tp;
            CTransferPipe::New((ITransferPipe**)&tp);
            // try {
            AutoPtr<IFileDescriptor> fileDes;
            AutoPtr<IParcelFileDescriptor> writeFd;
            tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
            FAIL_RETURN(r->mApp->mThread->DumpActivity(fileDes, IBinder::Probe(r->mAppToken), innerPrefix, args));
            tp->Go(fd);
            // } finally {
            tp->Kill();
            // }
        // } catch (IOException e) {
        //     pw->Println(innerPrefix + "Failure while dumping the activity: " + e);
        // } catch (RemoteException e) {
        //     pw->Println(innerPrefix + "Got a RemoteException while dumping the activity");
        // }
    }
    return NOERROR;
}

void CActivityManagerService::DumpBroadcastsLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Boolean onlyHistory = FALSE;
    Boolean printedAnything = FALSE;
    String newDumpPackage = dumpPackage;

    if (newDumpPackage.Equals("history")) {
        if (opti < args->GetLength() && (*args)[opti].Equals("-s")) {
            dumpAll = FALSE;
        }
        onlyHistory = TRUE;
        newDumpPackage = NULL;
    }

    pw->Println(String("ACTIVITY MANAGER BROADCAST STATE (dumpsys activity broadcasts)"));
    if (!onlyHistory && dumpAll) {
        if (mRegisteredReceivers.IsEmpty() == FALSE) {
            Boolean printed = FALSE;
            HashMap<AutoPtr<IBinder>, AutoPtr<ReceiverList> >::Iterator it;
            for (it = mRegisteredReceivers.Begin(); it != mRegisteredReceivers.End(); ++it) {
                AutoPtr<ReceiverList> r = it->mSecond;
                String rPkgName;
                IPackageItemInfo::Probe(r->mApp->mInfo)->GetPackageName(&rPkgName);
                if (newDumpPackage != NULL && (r->mApp == NULL ||
                        !newDumpPackage.Equals(rPkgName))) {
                    continue;
                }
                if (!printed) {
                    pw->Println(String("  Registered Receivers:"));
                    needSep = TRUE;
                    printed = TRUE;
                    printedAnything = TRUE;
                }
                pw->Print(String("  * "));
                pw->Println(r->ToString());
                r->Dump(pw, String("    "));
            }
        }

        // TODO:
        // if (mReceiverResolver->Dump(pw, String(needSep ?
        //         "\n  Receiver Resolver Table:" : "  Receiver Resolver Table:"),
        //         String("    "), newDumpPackage, FALSE)) {
        //     needSep = TRUE;
        //     printedAnything = TRUE;
        // }
    }

    for (Int32 i = 0; i < mBroadcastQueues->GetLength(); ++i) {
        AutoPtr<BroadcastQueue> q = (*mBroadcastQueues)[i];
        needSep = q->DumpLocked(fd, pw, args, opti, dumpAll, newDumpPackage, needSep);
        printedAnything |= needSep;
    }

    needSep = TRUE;

    if (!onlyHistory && !mStickyBroadcasts.IsEmpty() && newDumpPackage == NULL) {
        StickyBroadcastIterator it;
        for (it = mStickyBroadcasts.Begin(); it != mStickyBroadcasts.End(); ++it) {
            if (needSep) {
                pw->Println();
            }
            needSep = TRUE;
            printedAnything = TRUE;
            pw->Print(String("  Sticky broadcasts for user "));
            pw->Print(it->mFirst);
            pw->Println(String(":"));
            AutoPtr<StringBuilder> sb = new StringBuilder(128);
            AutoPtr<StringIntentMap> ent = it->mSecond;
            typename StringIntentMap::Iterator entIt;
            for (entIt = ent->Begin(); entIt != ent->End(); ++entIt) {
                pw->Print(String("  * Sticky action "));
                pw->Print(entIt->mFirst);
                if (dumpAll) {
                    pw->Println(String(":"));
                    AutoPtr<StringIntentList> intents = entIt->mSecond;
                    List< AutoPtr<IIntent> >::Iterator intentsIt;
                    for (intentsIt = intents->Begin(); intentsIt != intents->End(); ++intentsIt) {
//                         sb.setLength(0);
                        sb->Append("    Intent: ");
                        String str;
                        (*intentsIt)->ToShortString(sb, FALSE, TRUE, FALSE, FALSE);
                        pw->Println(sb->ToString());
                        AutoPtr<IBundle> bundle;
                        (*intentsIt)->GetExtras((IBundle**)&bundle);
                        if (bundle != NULL) {
                            pw->Print(String("      "));
                            pw->Println(bundle);
                        }
                    }
                }
                else {
                    pw->Println(String(""));
                }
            }
        }
    }

    if (!onlyHistory && dumpAll) {
        pw->Println();
        for (Int32 i = 0; i < mBroadcastQueues->GetLength(); ++i) {
            AutoPtr<BroadcastQueue> queue = (*mBroadcastQueues)[i];
            StringBuilder sb("  mBroadcastsScheduled [");
            sb += queue->mQueueName;
            sb += "]=";
            sb += queue->mBroadcastsScheduled;
            pw->Println(sb.ToString());
        }
        pw->Println(String("  mHandler:"));
//         mHandler.dump(new PrintWriterPrinter(pw), "    ");
        needSep = TRUE;
        printedAnything = TRUE;
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

void CActivityManagerService::DumpProvidersLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Boolean printedAnything = FALSE;

    AutoPtr<ItemMatcher> matcher = new ItemMatcher();
    matcher->Build(args, opti);

    pw->Println(String("ACTIVITY MANAGER CONTENT PROVIDERS (dumpsys activity providers)"));

    needSep = mProviderMap->DumpProvidersLocked(pw, dumpAll, dumpPackage);
    printedAnything |= needSep;

    if (mLaunchingProviders.IsEmpty() == FALSE) {
        Boolean printed = FALSE;
        List< AutoPtr<ContentProviderRecord> >::ReverseIterator rit;
        Int32 i=mLaunchingProviders.GetSize()-1;
        for (rit = mLaunchingProviders.RBegin(); rit != mLaunchingProviders.REnd(); ++rit, --i) {
            AutoPtr<ContentProviderRecord> r = *rit;
            String rPkgName;
            r->mName->GetPackageName(&rPkgName);
            if (dumpPackage != NULL && !dumpPackage.Equals(rPkgName)) {
                continue;
            }
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  Launching content providers:"));
                printed = TRUE;
                printedAnything = TRUE;
            }
            pw->Print(String("  Launching #"));
            pw->Print(i);
            pw->Print(String(": "));
            pw->Println(r->ToString());
        }
    }

    if (!mGrantedUriPermissions.IsEmpty()) {
        Boolean printed = FALSE;
        Int32 dumpUid = -2;
        if (dumpPackage != NULL) {
            AutoPtr<IPackageManager> pm;
            mContext->GetPackageManager((IPackageManager**)&pm);
            if (FAILED(pm->GetPackageUid(dumpPackage, 0, &dumpUid))) {
                dumpUid = -1;
            }
        }
        GrantUriPermissionMapMap::Iterator it = mGrantedUriPermissions.Begin();
        GrantUriPermissionMap::Iterator pIt;
        for (; it != mGrantedUriPermissions.End(); ++it) {
            Int32 uid = it->mFirst;
            if (dumpUid >= -1 && UserHandle::GetAppId(uid) != dumpUid) {
                continue;
            }
            AutoPtr<GrantUriPermissionMap> perms = it->mSecond;
            if (!printed) {
                if (needSep) pw->Println();
                needSep = TRUE;
                pw->Println(String("  Granted Uri Permissions:"));
                printed = TRUE;
                printedAnything = TRUE;
            }
            pw->Print(String("  * UID "));
            pw->Print(uid);
            pw->Println(String(" holds:"));
            for (pIt = perms->Begin(); pIt != perms->End(); ++pIt) {
                AutoPtr<UriPermission> perm = pIt->mSecond;
                pw->Print(String("    "));
                pw->Println(perm->ToString());
                if (dumpAll) {
                    perm->Dump(pw, String("      "));
                }
            }
        }
    }

    if (!printedAnything) {
        pw->Println(String("  (nothing)"));
    }
}

void CActivityManagerService::DumpPendingIntentsLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean printed = FALSE;

    pw->Println(String("ACTIVITY MANAGER PENDING INTENTS (dumpsys activity intents)"));

    if (mIntentSenderRecords.IsEmpty() == FALSE) {
        PendingIntentRecordHashMap::Iterator it;
        for (it = mIntentSenderRecords.Begin(); it != mIntentSenderRecords.End(); ++it) {
            AutoPtr<IWeakReference> ref = it->mSecond;
            AutoPtr<IIIntentSender> _rec;
            ref->Resolve(EIID_IIIntentSender, (IInterface**)&_rec);
            CPendingIntentRecord* rec = _rec != NULL ? (CPendingIntentRecord*)_rec.Get() : NULL;
            if (!dumpPackage.IsNull() && (rec == NULL
                    || !dumpPackage.Equals(rec->mKey->mPackageName))) {
                continue;
            }
            printed = TRUE;
            if (rec != NULL) {
                pw->Print(String("  * "));
                pw->Println(rec->Probe(EIID_IInterface));
                if (dumpAll) {
                    rec->Dump(pw, String("    "));
                }
            }
            else {
                pw->Print(String("  * "));
                pw->Println(ref);
            }
        }
    }

    if (!printed) {
        pw->Println(String("  (nothing)"));
    }
}

Int32 CActivityManagerService::DumpProcessList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IArrayList* list,
    /* [in] */ const String& prefix,
    /* [in] */ const String& normalLabel,
    /* [in] */ const String& persistentLabel,
    /* [in] */ const String& dumpPackage)
{
    Int32 numPers = 0;
    Int32 N;
    list->GetSize(&N);
    N = N - 1;
    for (Int32 i = N; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        ProcessRecord* r = (ProcessRecord*)IProcessRecord::Probe(obj);
        String rPkgName;
        IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&rPkgName);
        if (dumpPackage != NULL && !dumpPackage.Equals(rPkgName)) {
            continue;
        }
        String s;
        s.AppendFormat("%s%s #%2d: %s",
                prefix.string(), (r->mPersistent ? persistentLabel.string() : normalLabel.string()),
                i, r->ToString().string());
        pw->Println(s);
        if (r->mPersistent) {
            numPers++;
        }
    }
    return numPers;
}

Boolean CActivityManagerService::DumpProcessOomList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ CActivityManagerService* service,
    /* [in] */ List< AutoPtr<ProcessRecord> >* origList,
    /* [in] */ const String& prefix,
    /* [in] */ const String& normalLabel,
    /* [in] */ const String& persistentLabel,
    /* [in] */ Boolean inclDetails,
    /* [in] */ const String& dumpPackage)
{
    // ArrayList<Pair<ProcessRecord, Integer>> list
    //         = new ArrayList<Pair<ProcessRecord, Integer>>(origList.size());
    // for (Int32 i=0; i<origList.size(); i++) {
    //     ProcessRecord r = origList.get(i);
    //     if (dumpPackage != null && !r.pkgList.containsKey(dumpPackage)) {
    //         continue;
    //     }
    //     list.add(new Pair<ProcessRecord, Integer>(origList.get(i), i));
    // }

    // if (list.size() <= 0) {
    //     return FALSE;
    // }

    // Comparator<Pair<ProcessRecord, Integer>> comparator
    //         = new Comparator<Pair<ProcessRecord, Integer>>() {
    //     @Override
    //     public Int32 compare(Pair<ProcessRecord, Integer> object1,
    //             Pair<ProcessRecord, Integer> object2) {
    //         if (object1.first.setAdj != object2.first.setAdj) {
    //             return object1.first.setAdj > object2.first.setAdj ? -1 : 1;
    //         }
    //         if (object1.second.intValue() != object2.second.intValue()) {
    //             return object1.second.intValue() > object2.second.intValue() ? -1 : 1;
    //         }
    //         return 0;
    //     }
    // };

    // Collections.sort(list, comparator);

    // final long curRealtime = SystemClock.elapsedRealtime();
    // final long realtimeSince = curRealtime - service.mLastPowerCheckRealtime;
    // final long curUptime = SystemClock.uptimeMillis();
    // final long uptimeSince = curUptime - service.mLastPowerCheckUptime;

    // for (Int32 i=list.size()-1; i>=0; i--) {
    //     ProcessRecord r = list.get(i).first;
    //     String oomAdj = ProcessList.makeOomAdjString(r.setAdj);
    //     char schedGroup;
    //     switch (r.setSchedGroup) {
    //         case Process.THREAD_GROUP_BG_NONINTERACTIVE:
    //             schedGroup = 'B';
    //             break;
    //         case Process.THREAD_GROUP_DEFAULT:
    //             schedGroup = 'F';
    //             break;
    //         default:
    //             schedGroup = '?';
    //             break;
    //     }
    //     char foreground;
    //     if (r.foregroundActivities) {
    //         foreground = 'A';
    //     } else if (r.foregroundServices) {
    //         foreground = 'S';
    //     } else {
    //         foreground = ' ';
    //     }
    //     String procState = ProcessList.MakeProcStateString(r.curProcState);
    //     pw->Print(prefix);
    //     pw->Print(r.persistent ? persistentLabel : normalLabel);
    //     pw->Print(" #");
    //     Int32 num = (origList.size()-1)-list.get(i).second;
    //     if (num < 10) pw->Print(' ');
    //     pw->Print(num);
    //     pw->Print(": ");
    //     pw->Print(oomAdj);
    //     pw->Print(' ');
    //     pw->Print(schedGroup);
    //     pw->Print('/');
    //     pw->Print(foreground);
    //     pw->Print('/');
    //     pw->Print(procState);
    //     pw->Print(" trm:");
    //     if (r.trimMemoryLevel < 10) pw->Print(' ');
    //     pw->Print(r.trimMemoryLevel);
    //     pw->Print(' ');
    //     pw->Print(r.toShortString());
    //     pw->Print(" (");
    //     pw->Print(r.adjType);
    //     pw->Println(')');
    //     if (r.adjSource != null || r.adjTarget != null) {
    //         pw->Print(prefix);
    //         pw->Print("    ");
    //         if (r.adjTarget instanceof ComponentName) {
    //             pw->Print(((ComponentName)r.adjTarget).flattenToShortString());
    //         } else if (r.adjTarget != null) {
    //             pw->Print(r.adjTarget.toString());
    //         } else {
    //             pw->Print("{null}");
    //         }
    //         pw->Print("<=");
    //         if (r.adjSource instanceof ProcessRecord) {
    //             pw->Print("Proc{");
    //             pw->Print(((ProcessRecord)r.adjSource).toShortString());
    //             pw->Println(String("}"));
    //         } else if (r.adjSource != null) {
    //             pw->Println(r.adjSource.toString());
    //         } else {
    //             pw->Println("{null}");
    //         }
    //     }
    //     if (inclDetails) {
    //         pw->Print(prefix);
    //         pw->Print("    ");
    //         pw->Print("oom: max="); pw->Print(r.maxAdj);
    //         pw->Print(" curRaw="); pw->Print(r.curRawAdj);
    //         pw->Print(" setRaw="); pw->Print(r.setRawAdj);
    //         pw->Print(" cur="); pw->Print(r.curAdj);
    //         pw->Print(" set="); pw->Println(r.setAdj);
    //         pw->Print(prefix);
    //         pw->Print("    ");
    //         pw->Print("state: cur="); pw->Print(ProcessList.MakeProcStateString(r.curProcState));
    //         pw->Print(" set="); pw->Print(ProcessList.MakeProcStateString(r.setProcState));
    //         pw->Print(" lastPss="); pw->Print(r.lastPss);
    //         pw->Print(" lastCachedPss="); pw->Println(r.lastCachedPss);
    //         pw->Print(prefix);
    //         pw->Print("    ");
    //         pw->Print("cached="); pw->Print(r.cached);
    //         pw->Print(" empty="); pw->Print(r.empty);
    //         pw->Print(" hasAboveClient="); pw->Println(r.hasAboveClient);

    //         if (r.setProcState >= IActivityManager::PROCESS_STATE_SERVICE) {
    //             if (r.lastWakeTime != 0) {
    //                 long wtime;
    //                 BatteryStatsImpl stats = service.mBatteryStatsService.getActiveStatistics();
    //                 {    AutoLock syncLock(stats);
    //                     wtime = stats.getProcessWakeTime(r.info.uid,
    //                             r.pid, curRealtime);
    //                 }
    //                 long timeUsed = wtime - r.lastWakeTime;
    //                 pw->Print(prefix);
    //                 pw->Print("    ");
    //                 pw->Print("keep awake over ");
    //                 TimeUtils.formatDuration(realtimeSince, pw);
    //                 pw->Print(" used ");
    //                 TimeUtils.formatDuration(timeUsed, pw);
    //                 pw->Print(" (");
    //                 pw->Print((timeUsed*100)/realtimeSince);
    //                 pw->Println("%)");
    //             }
    //             if (r.lastCpuTime != 0) {
    //                 long timeUsed = r.curCpuTime - r.lastCpuTime;
    //                 pw->Print(prefix);
    //                 pw->Print("    ");
    //                 pw->Print("run cpu over ");
    //                 TimeUtils.formatDuration(uptimeSince, pw);
    //                 pw->Print(" used ");
    //                 TimeUtils.formatDuration(timeUsed, pw);
    //                 pw->Print(" (");
    //                 pw->Print((timeUsed*100)/uptimeSince);
    //                 pw->Println("%)");
    //             }
    //         }
    //     }
    // }
    // return TRUE;
    return FALSE;
}

AutoPtr<List<AutoPtr<ProcessRecord> > > CActivityManagerService::CollectProcesses(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 start,
    /* [in] */ Boolean allPkgs,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<List<AutoPtr<ProcessRecord> > > procs;
    {
        AutoLock lock(this);
        if (args != NULL && args->GetLength() > start
                && (*args)[start].GetChar(0) != '-') {
            procs = new List<AutoPtr<ProcessRecord> >();
            Int32 pid = -1;
            // try {
            pid = StringUtils::ParseInt32((*args)[start]);
            // } catch (NumberFormatException e) {
            // }
            List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
            for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
                AutoPtr<ProcessRecord> proc = *rit;
                Boolean res;
                if (proc->mPid == pid) {
                    procs->PushBack(proc);
                }
                else if (allPkgs && proc->mPkgList != NULL
                    && (proc->mPkgList->ContainsKey(CoreUtils::Convert((*args)[start]), &res), res)) {
                    procs->PushBack(proc);
                }
                else if (proc->mProcessName.Equals((*args)[start])) {
                    procs->PushBack(proc);
                }
            }
            if (procs->IsEmpty()) {
                return NULL;
            }
        }
        else {
            procs = new List<AutoPtr<ProcessRecord> >(mLruProcesses.Begin(), mLruProcesses.End());;
        }
    }
    return procs;
}

ECode CActivityManagerService::DumpGraphicsHardwareUsage(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr< List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, 0, FALSE, args);
    if (procs == NULL) {
        pw->Println(String("No process found for: ") + (*args)[0]);
        return NOERROR;
    }

    Int64 uptime = SystemClock::GetUptimeMillis();
    Int64 realtime = SystemClock::GetElapsedRealtime();
    pw->Println(String("Applications Graphics Acceleration Info:"));
    StringBuilder sb("Uptime: ");
    sb += uptime;
    sb += " Realtime: ";
    sb += realtime;
    pw->Println(sb.ToString());

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            StringBuilder sb("\n** Graphics info for pid ");
            sb += r->mPid;
            sb += " [";
            sb += r->mProcessName;
            sb += "] **";
            pw->Println(sb.ToString());
            IFlushable::Probe(pw)->Flush();
            // try {
            AutoPtr<ITransferPipe> tp;
            CTransferPipe::New((ITransferPipe**)&tp);
                // try {
                AutoPtr<IParcelFileDescriptor> writeFd;
                tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
                AutoPtr<IFileDescriptor> fileDes;
                writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
                r->mThread->DumpGfxInfo(fileDes, args);
                tp->Go(fd);
                // } finally {
                tp->Kill();
                // }
            // } catch (IOException e) {
            //     pw->Println("Failure while dumping the app: " + r);
            //     pw->Flush();
            // } catch (RemoteException e) {
            //     pw->Println("Got a RemoteException while dumping the app " + r);
            //     pw->Flush();
            // }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::DumpDbInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, 0, FALSE, args);
    if (procs == NULL) {
        pw->Println(String("No process found for: ") + (*args)[0]);
        return NOERROR;
    }

    pw->Println(String("Applications Database Info:"));

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            StringBuilder sb("\n** Database info for pid ");
            sb += r->mPid;
            sb += " [";
            sb += r->mProcessName;
            sb += "] **";
            pw->Println(sb.ToString());
            IFlushable::Probe(pw)->Flush();
            // try {
            AutoPtr<ITransferPipe> tp;
            CTransferPipe::New((ITransferPipe**)&tp);
                // try {
                AutoPtr<IParcelFileDescriptor> writeFd;
                tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
                AutoPtr<IFileDescriptor> fileDes;
                writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
                r->mThread->DumpDbInfo(fileDes, args);
                tp->Go(fd);
                // } finally {
                tp->Kill();
                // }
            // } catch (IOException e) {
            //     pw->Println("Failure while dumping the app: " + r);
            //     pw->Flush();
            // } catch (RemoteException e) {
            //     pw->Println("Got a RemoteException while dumping the app " + r);
            //     pw->Flush();
            // }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::DumpMemItems(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const String& tag,
    /* [in] */ List< AutoPtr<MemItem> >* items,
    /* [in] */ Boolean sort,
    /* [in] */ Boolean isCompact)
{
    List< AutoPtr<MemItem> >::Iterator it;
    if (sort && !isCompact) {
        items->Sort(MemItem::Compare);
    }

    AutoPtr<MemItem> mi;
    for (it = items->Begin(); it != items->End(); ++it) {
        mi = *it;
        if (!isCompact) {
            pw->Print(prefix);
            pw->Print(mi->mPss);
            pw->Print(String(" kB: "));
            pw->Println(mi->mLabel);
        }
        else if (mi->mIsProc) {
            pw->Print(String("proc,"));
            pw->Print(tag);
            pw->Print(String(","));
            pw->Print(mi->mShortLabel);
            pw->Print(String(","));
            pw->Print(mi->mId);
            pw->Print(String(","));
            pw->Print(mi->mPss);
            pw->Println(String(mi->mHasActivities ? ",a" : ",e"));
        }
        else {
            pw->Print(tag);
            pw->Print(String(","));
            pw->Print(mi->mShortLabel);
            pw->Print(String(","));
            pw->Println(mi->mPss);
        }
        if (mi->mSubitems != NULL) {
            DumpMemItems(pw, prefix + "           ",  mi->mShortLabel, mi->mSubitems, TRUE, isCompact);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::AppendMemBucket(
    /* [in] */ StringBuilder& out,
    /* [in] */ Int64 memKB,
    /* [in] */ const String& label,
    /* [in] */ Boolean stackLike)
{
    Int32 start = label.LastIndexOf('.');
    if (start >= 0)
        start++;
    else
        start = 0;
    Int32 end = label.GetLength();
    for (Int32 i=0; i<DUMP_MEM_BUCKETS->GetLength(); i++) {
        if ((*DUMP_MEM_BUCKETS)[i] >= memKB) {
            Int64 bucket = (*DUMP_MEM_BUCKETS)[i]/1024;
            out.Append(bucket);
            out.Append(stackLike ? "MB." : "MB ");
            AutoPtr<ICharSequence> cs;
            CString::New(label, (ICharSequence**)&cs);
            out.Append(cs, start, end);
            return NOERROR;
        }
    }
    out.Append(memKB/1024);
    out.Append(stackLike ? "MB." : "MB ");
    AutoPtr<ICharSequence> cs;
    CString::New(label, (ICharSequence**)&cs);
    out.Append(cs, start, end);
    return NOERROR;
}

void CActivityManagerService::DumpApplicationMemoryUsageHeader(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime,
    /* [in] */ Boolean isCheckinRequest,
    /* [in] */ Boolean isCompact)
{
    if (isCheckinRequest || isCompact) {
        // short checkin version
        pw->Print(String("time,"));
        pw->Print(uptime);
        pw->Print(String(","));
        pw->Println(realtime);
    }
    else {
        pw->Println(String("Applications Memory Usage (kB):"));
        StringBuilder sb("Uptime: ");
        sb += uptime;
        sb += " Realtime: ";
        sb += realtime;
        pw->Println(sb.ToString());
    }
}

ECode CActivityManagerService::DumpApplicationMemoryUsage(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean brief,
    /* [in] */ IPrintWriter* categoryPw)
{
    if (args == NULL) {
        return NOERROR;
    }

    Boolean dumpDetails = FALSE;
    Boolean dumpFullDetails = FALSE;
    Boolean dumpDalvik = FALSE;
    Boolean oomOnly = FALSE;
    Boolean isCompact = FALSE;
    Boolean localOnly = FALSE;
    Boolean packages = FALSE;

    Int32 opti = 0;
    String opt;
    while (opti < args->GetLength()) {
        opt = (*args)[opti];
        if (opt.IsNullOrEmpty() || opt.GetChar(0) != '-') {
            break;
        }
        opti++;
        if (opt.Equals("-a")) {
            dumpDetails = TRUE;
            dumpFullDetails = TRUE;
            dumpDalvik = TRUE;
        }
        else if (opt.Equals("-d")) {
            dumpDalvik = TRUE;
        }
        else if (opt.Equals("-c")) {
            isCompact = TRUE;
        }
        else if (opt.Equals("--oom")) {
            oomOnly = TRUE;
        }
        else if (opt.Equals("--local")) {
            localOnly = TRUE;
        }
        else if (opt.Equals("--package")) {
            packages = TRUE;
        }
        else if (opt.Equals("-h")) {
            pw->Println(String("meminfo dump options: [-a] [-d] [-c] [--oom] [process]"));
            pw->Println(String("  -a: include all available information for each process."));
            pw->Println(String("  -d: include dalvik details when dumping process details."));
            pw->Println(String("  -c: dump in a compact machine-parseable representation."));
            pw->Println(String("  --oom: only show processes organized by oom adj."));
            pw->Println(String("  --local: only collect details locally, don't call process."));
            pw->Println(String("  --package: interpret process arg as package, dumping all"));
            pw->Println(String("             processes that have loaded that package."));
            pw->Println(String("If [process] is specified it can be the name or "));
            pw->Println(String("pid of a specific process to dump."));
            return NOERROR;
        }
        else {
            pw->Println(String("Unknown argument: ") + opt + "; use -h for help");
        }
    }

    const Boolean isCheckinRequest = ScanArgs(args, String("--checkin"));
    Int64 uptime = SystemClock::GetUptimeMillis();
    Int64 realtime = SystemClock::GetElapsedRealtime();
    AutoPtr<ArrayOf<Int64> > tmpLong;

    AutoPtr<List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, opti, packages, args);
    if (procs == NULL) {
        // No Java processes.  Maybe they want to print a native process.
        if (args != NULL && args->GetLength() > opti
                && (*args)[opti].GetChar(0) != '-') {
            List<AutoPtr<IProcessCpuTrackerStats> > nativeProcs;
            UpdateCpuStatsNow();
            Int32 findPid = StringUtils::ParseInt32((*args)[opti], 10, -1);
            {    AutoLock syncLock(mProcessCpuTracker);
                Int32 N;
                mProcessCpuTracker->CountStats(&N);
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<IProcessCpuTrackerStats> st;
                    mProcessCpuTracker->GetStats(i, (IProcessCpuTrackerStats**)&st);
                    Int32 pid;
                    st->GetPid(&pid);
                    String baseName;
                    st->GetBaseName(&baseName);
                    if (pid == findPid || (baseName != NULL
                            && baseName.Equals((*args)[opti]))) {
                        nativeProcs.PushBack(st);
                    }
                }
            }
            if (nativeProcs.GetSize() > 0) {
                DumpApplicationMemoryUsageHeader(pw, uptime, realtime, isCheckinRequest,
                        isCompact);
                AutoPtr<IDebugMemoryInfo> mi;
                List<AutoPtr<IProcessCpuTrackerStats> >::ReverseIterator rit;
                for (rit = nativeProcs.RBegin(); rit != nativeProcs.REnd(); ++rit) {
                    AutoPtr<IProcessCpuTrackerStats> r = *rit;
                    Int32 pid;
                    r->GetPid(&pid);
                    String baseName;
                    r->GetBaseName(&baseName);
                    if (!isCheckinRequest && dumpDetails) {
                        String str;
                        str.AppendFormat("\n** MEMINFO in pid %d [%s] **", pid, baseName.string());
                        pw->Println(str);
                    }
                    if (mi == NULL) {
                        CDebugMemoryInfo::New((IDebugMemoryInfo**)&mi);
                    }
                    AutoPtr<IDebug> dbg;
                    CDebug::AcquireSingleton((IDebug**)&dbg);
                    if (dumpDetails || (!brief && !oomOnly)) {
                        dbg->GetMemoryInfo(pid, (IDebugMemoryInfo**)&mi);
                    }
                    else {
                        Int64 pss = 0;
                        tmpLong = NULL;
                        dbg->GetPss(pid, (ArrayOf<Int64>**)&tmpLong, &pss);
                        mi->SetDalvikPss((Int32)pss);
                        mi->SetDalvikPrivateDirty((Int32)(*tmpLong)[0]);
                    }
                    // TODO:
                    // AutoPtr<IActivityThreadHelper> atHelper;
                    // CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&atHelper);
                    // atHelper->DumpMemInfoTable(pw, mi, isCheckinRequest, dumpFullDetails,
                    //         dumpDalvik, pid, baseName, 0, 0, 0, 0, 0, 0);
                    if (isCheckinRequest) {
                        pw->Println();
                    }
                }
                return NOERROR;
            }
        }
        pw->Println(String("No process found for: ") + (*args)[opti]);
        return NOERROR;
    }

    if (!brief && !oomOnly && (procs->GetSize() == 1 || isCheckinRequest || packages)) {
        dumpDetails = TRUE;
    }

    DumpApplicationMemoryUsageHeader(pw, uptime, realtime, isCheckinRequest, isCompact);

    AutoPtr<ArrayOf<String> > innerArgs = ArrayOf<String>::Alloc(args->GetLength()-opti);
    innerArgs->Copy(args, opti, args->GetLength()-opti);

    typedef List<AutoPtr<MemItem> > MemItemList;
    AutoPtr<MemItemList> procMems = new MemItemList();
    HashMap<Int32, AutoPtr<MemItem> > procMemsMap;
    Int64 nativePss=0, dalvikPss=0, otherPss=0;
    AutoPtr<ArrayOf<Int64> > miscPss = ArrayOf<Int64>::Alloc(IDebugMemoryInfo::NUM_OTHER_STATS);

    AutoPtr<ArrayOf<Int64> > oomPss = ArrayOf<Int64>::Alloc(DUMP_MEM_OOM_LABEL->GetLength());
    AutoPtr<ArrayOf<MemItemList* > > oomProcs = ArrayOf<MemItemList* >::Alloc(DUMP_MEM_OOM_LABEL->GetLength());

    Int64 totalPss = 0;
    Int64 cachedPss = 0;

    AutoPtr<IDebugMemoryInfo> mi;
    Int32 mem = 0;
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        AutoPtr<IApplicationThread> thread;
        Int32 pid = 0;
        Int32 oomAdj = 0;
        Boolean hasActivities = FALSE;
        {
            AutoLock lock(this);
            thread = r->mThread;
            pid = r->mPid;
            oomAdj = r->GetSetAdjWithServices();
            hasActivities = r->mActivities.GetSize() > 0;
        }
        if (thread != NULL) {
            if (!isCheckinRequest && dumpDetails) {
                String str;
                str.AppendFormat("\n** MEMINFO in pid %d [%s] **", pid, r->mProcessName.string());
                pw->Println(str);
            }
            if (mi == NULL) {
                CDebugMemoryInfo::New((IDebugMemoryInfo**)&mi);
            }
            if (dumpDetails || (!brief && !oomOnly)) {
                dbg->GetMemoryInfo(pid, (IDebugMemoryInfo**)&mi);
            }
            else {
                Int64 pss = 0;
                tmpLong = NULL;
                dbg->GetPss(pid, (ArrayOf<Int64>**)&tmpLong, &pss);
                mi->SetDalvikPss((Int32)pss);
                mi->SetDalvikPrivateDirty((Int32)(*tmpLong)[0]);
            }
            if (dumpDetails) {
                if (localOnly) {
                    // TODO:
                    // AutoPtr<IActivityThreadHelper> atHelper;
                    // CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&atHelper);
                    // atHelper->DumpMemInfoTable(pw, mi, isCheckinRequest, dumpFullDetails,
                    //         dumpDalvik, pid, r->mProcessName, 0, 0, 0, 0, 0, 0);
                    if (isCheckinRequest) {
                        pw->Println();
                    }
                }
                else {
                    IFlushable::Probe(pw)->Flush();
                    if (FAILED(thread->DumpMemInfo(fd, mi, isCheckinRequest, dumpFullDetails,
                        dumpDalvik, innerArgs))) {
                        if (!isCheckinRequest) {
                            pw->Println(String("Got RemoteException!"));
                            IFlushable::Probe(pw)->Flush();
                        }
                    }
                }
            }

            Int32 myTotalPss;
            mi->GetTotalPss(&myTotalPss);
            Int32 myTotalUss;
            mi->GetTotalUss(&myTotalUss);

            {
                AutoLock lock(this);
                if (r->mThread != NULL && oomAdj == r->GetSetAdjWithServices()) {
                    // Record this for posterity if the process has been stable.
                    r->mBaseProcessTracker->AddPss(myTotalPss, myTotalUss, TRUE, r->mPkgList);
                }
            }

            if (!isCheckinRequest && mi != NULL) {
                totalPss += myTotalPss;
                StringBuilder sb(r->mProcessName);
                sb += " (pid ";
                sb += pid;
                sb += (hasActivities ? " / activities)" : ")");
                AutoPtr<MemItem> pssItem = new MemItem(sb.ToString(),
                    r->mProcessName, myTotalPss, pid, hasActivities);
                procMems->PushBack(pssItem);
                procMemsMap[pid] = pssItem;

                Int32 _nativePss;
                mi->GetNativePss(&_nativePss);
                nativePss += _nativePss;
                Int32 _dalvikPss;
                mi->GetDalvikPss(&_dalvikPss);
                dalvikPss += _dalvikPss;
                Int32 _otherPss;
                mi->GetOtherPss(&_otherPss);
                otherPss += _otherPss;
                for (Int32 j = 0; j < IDebugMemoryInfo::NUM_OTHER_STATS; j++) {
                    mi->GetOtherPss(j, &mem);
                    (*miscPss)[j] += mem;
                    otherPss -= mem;
                }

                if (oomAdj >= ProcessList::CACHED_APP_MIN_ADJ) {
                    cachedPss += myTotalPss;
                }

                for (Int32 oomIndex = 0; oomIndex < oomPss->GetLength(); oomIndex++) {
                    if (oomAdj <= (*DUMP_MEM_OOM_ADJ)[oomIndex]
                            || oomIndex == (oomPss->GetLength()-1)) {
                        (*oomPss)[oomIndex] += myTotalPss;
                        if ((*oomProcs)[oomIndex] == NULL) {
                            AutoPtr<MemItemList> list = new MemItemList();
                            oomProcs->Set(oomIndex, list);
                        }
                        (*oomProcs)[oomIndex]->PushBack(pssItem);
                        break;
                    }
                }
            }
        }
    }

    Int64 nativeProcTotalPss = 0;

    if (!isCheckinRequest && procs->GetSize() > 1 && !packages) {
        // If we are showing aggregations, also look for native processes to
        // include so that our aggregations are more accurate.
        UpdateCpuStatsNow();
        {    AutoLock syncLock(mProcessCpuTracker);
            Int32 N;
            mProcessCpuTracker->CountStats(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IProcessCpuTrackerStats> st;
                mProcessCpuTracker->GetStats(i, (IProcessCpuTrackerStats**)&st);
                Int64 vsize;
                st->GetVsize(&vsize);
                Int32 pid;
                st->GetPid(&pid);
                if (vsize > 0 && procMemsMap.Find(pid) == procMemsMap.End()) {
                    if (mi == NULL) {
                        CDebugMemoryInfo::New((IDebugMemoryInfo**)&mi);
                    }
                    if (!brief && !oomOnly) {
                        dbg->GetMemoryInfo(pid, (IDebugMemoryInfo**)&mi);
                    }
                    else {
                        Int64 pss = 0;
                        tmpLong = NULL;
                        dbg->GetPss(pid, (ArrayOf<Int64>**)&tmpLong, &pss);
                        mi->SetNativePss((Int32)pss);
                        mi->SetNativePrivateDirty((Int32)(*tmpLong)[0]);
                    }

                    Int32 myTotalPss;
                    mi->GetTotalPss(&myTotalPss);
                    totalPss += myTotalPss;
                    nativeProcTotalPss += myTotalPss;

                    String name;
                    st->GetName(&name);
                    StringBuilder sb;
                    sb += name;
                    sb +=  " (pid ";
                    sb += pid;
                    sb += ")";
                    AutoPtr<MemItem> pssItem = new MemItem(sb.ToString(),
                            name, myTotalPss, pid, FALSE);
                    procMems->PushBack(pssItem);

                    Int32 pss;
                    mi->GetNativePss(&pss);
                    nativePss += pss;
                    mi->GetDalvikPss(&pss);
                    dalvikPss += pss;
                    mi->GetOtherPss(&pss);
                    otherPss += pss;
                    for (Int32 j = 0; j < IDebugMemoryInfo::NUM_OTHER_STATS; j++) {
                        Int32 mem;
                        mi->GetOtherPss(j, &mem);
                        (*miscPss)[j] += mem;
                        otherPss -= mem;
                    }
                    (*oomPss)[0] += myTotalPss;
                    if ((*oomProcs)[0] == NULL) {
                        AutoPtr<MemItemList> list = new MemItemList();
                        oomProcs->Set(0, list);
                    }
                    (*oomProcs)[0]->PushBack(pssItem);
                }
            }
        }

        AutoPtr<MemItemList> catMems = new MemItemList();

        AutoPtr<MemItem> nPssItem = new MemItem(String("Native"), String("Native"), nativePss, -1);
        AutoPtr<MemItem> dPssItem = new MemItem(String("Dalvik"), String("Dalvik"), dalvikPss, -2);
        AutoPtr<MemItem> uPssItem = new MemItem(String("Unknown"), String("Unknown"), otherPss, -3);
        catMems->PushBack(nPssItem);
        catMems->PushBack(dPssItem);
        catMems->PushBack(uPssItem);

        String label;
        AutoPtr<IDebugMemoryInfoHelper> helper;
        CDebugMemoryInfoHelper::AcquireSingleton((IDebugMemoryInfoHelper**)&helper);
        for (Int32 j = 0; j < IDebugMemoryInfo::NUM_OTHER_STATS; j++) {
            helper->GetOtherLabel(j, &label);
            AutoPtr<MemItem> _pssItem = new MemItem(label, label, (*miscPss)[j], j);
            catMems->PushBack(_pssItem);
        }

        AutoPtr<MemItemList> oomMems = new MemItemList();
        for (Int32 j = 0; j < oomPss->GetLength(); j++) {
            if ((*oomPss)[j] != 0) {
                label = isCompact ? (*DUMP_MEM_OOM_COMPACT_LABEL)[j]
                    : (*DUMP_MEM_OOM_LABEL)[j];
                AutoPtr<MemItem> item = new MemItem(label, label, (*oomPss)[j], (*DUMP_MEM_OOM_ADJ)[j]);
                item->mSubitems = (*oomProcs)[j];
                oomMems->PushBack(item);
            }
        }

        if (!brief && !oomOnly && !isCompact) {
            pw->Println();
            pw->Println(String("Total PSS by process:"));
            DumpMemItems(pw, String("  "), String("proc"), procMems, TRUE, isCompact);
            pw->Println();
        }
        if (!isCompact) {
            pw->Println(String("Total PSS by OOM adjustment:"));
        }
        DumpMemItems(pw, String("  "), String("oom"), oomMems, FALSE, isCompact);
        if (!brief && !oomOnly) {
            AutoPtr<IPrintWriter> out = categoryPw != NULL ? categoryPw : pw;
            if (!isCompact) {
                out->Println();
                out->Println(String("Total PSS by category:"));
            }
            DumpMemItems(out, String("  "), String("cat"), catMems, TRUE, isCompact);
        }
        if (!isCompact) {
            pw->Println();
        }
        AutoPtr<IMemInfoReader> memInfo;
        CMemInfoReader::New((IMemInfoReader**)&memInfo);
        memInfo->ReadMemInfo();
        Int64 cachedSizeKb, freeSizeKb, totalSizeKb, zramTotalSizeKb,
            buffersSizeKb, shmemSizeKb, slabSizeKb;
        memInfo->GetCachedSizeKb(&cachedSizeKb);
        memInfo->GetFreeSizeKb(&freeSizeKb);
        memInfo->GetTotalSizeKb(&totalSizeKb);
        memInfo->GetZramTotalSizeKb(&zramTotalSizeKb);
        memInfo->GetBuffersSizeKb(&buffersSizeKb);
        memInfo->GetShmemSizeKb(&shmemSizeKb);
        memInfo->GetSlabSizeKb(&slabSizeKb);
        if (nativeProcTotalPss > 0) {
            {    AutoLock syncLock(this);
                mProcessStats->AddSysMemUsageLocked(cachedSizeKb, freeSizeKb, zramTotalSizeKb,
                    buffersSizeKb + shmemSizeKb + slabSizeKb, nativeProcTotalPss);
            }
        }
        if (!brief) {
            if (!isCompact) {
                pw->Print(String("Total RAM: "));
                pw->Print(totalSizeKb);
                pw->Print(String(" kB (status "));
                switch (mLastMemoryLevel) {
                    case IProcessStats::ADJ_MEM_FACTOR_NORMAL:
                        pw->Println(String("normal)"));
                        break;
                    case IProcessStats::ADJ_MEM_FACTOR_MODERATE:
                        pw->Println(String("moderate)"));
                        break;
                    case IProcessStats::ADJ_MEM_FACTOR_LOW:
                        pw->Println(String("low)"));
                        break;
                    case IProcessStats::ADJ_MEM_FACTOR_CRITICAL:
                        pw->Println(String("critical)"));
                        break;
                    default:
                        pw->Print(mLastMemoryLevel);
                        pw->Println(String(")"));
                        break;
                }
                pw->Print(String(" Free RAM: "));
                pw->Print(cachedPss + cachedSizeKb + freeSizeKb);
                pw->Print(String(" kB ("));
                pw->Print(cachedPss);
                pw->Print(String(" cached pss + "));
                pw->Print(cachedSizeKb);
                pw->Print(String(" cached + "));
                pw->Print(freeSizeKb);
                pw->Println(String(" free)"));
            }
            else {
                pw->Print(String("ram,"));
                pw->Print(totalSizeKb);
                pw->Print(String(","));
                pw->Print(cachedPss + cachedSizeKb + freeSizeKb);
                pw->Print(String(","));
                pw->Println(totalPss - cachedPss);
            }
        }
        if (!isCompact) {
            pw->Print(String(" Used RAM: "));
            pw->Print(totalPss - cachedPss
                + buffersSizeKb + shmemSizeKb + slabSizeKb);
            pw->Print(String(" kB ("));
            pw->Print(totalPss - cachedPss);
            pw->Print(String(" used pss + "));
            pw->Print(buffersSizeKb);
            pw->Print(String(" buffers + "));
            pw->Print(shmemSizeKb);
            pw->Print(String(" shmem + "));
            pw->Print(slabSizeKb);
            pw->Println(String(" slab)"));
            pw->Print(String(" Lost RAM: "));
            pw->Print(totalSizeKb
                - totalPss - freeSizeKb - cachedSizeKb
                - buffersSizeKb - shmemSizeKb - slabSizeKb);
            pw->Println(String(" kB"));
        }
        if (!brief) {
            if (zramTotalSizeKb != 0) {
                Int64 swapTotalSizeKb, swapFreeSizeKb;
                memInfo->GetSwapTotalSizeKb(&swapTotalSizeKb);
                memInfo->GetSwapFreeSizeKb(&swapFreeSizeKb);
                if (!isCompact) {
                    pw->Print(String("     ZRAM: "));
                    pw->Print(zramTotalSizeKb);
                    pw->Print(String(" kB physical used for "));
                    pw->Print(swapTotalSizeKb - swapFreeSizeKb);
                    pw->Print(String(" kB in swap ("));
                    pw->Print(swapTotalSizeKb);
                    pw->Println(String(" kB total swap)"));
                }
                else {
                    pw->Print(String("zram,"));
                    pw->Print(zramTotalSizeKb);
                    pw->Print(String(","));
                    pw->Print(swapTotalSizeKb);
                    pw->Print(String(","));
                    pw->Println(swapFreeSizeKb);
                }
            }
             AutoPtr<ArrayOf<Int32> > SINGLE_LONG_FORMAT = ArrayOf<Int32>::Alloc(1);
            (*SINGLE_LONG_FORMAT)[0] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;
            AutoPtr<ArrayOf<Int64> > int64Out = ArrayOf<Int64>::Alloc(1);
            Boolean result;
            Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_shared"),
                    *SINGLE_LONG_FORMAT, NULL, int64Out, NULL, &result);
            Int64 shared = (*int64Out)[0] * ProcessList::MEMORY_PAGE_SIZE / 1024;
            (*int64Out)[0] = 0;
            Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_sharing"),
                    *SINGLE_LONG_FORMAT, NULL, int64Out, NULL, &result);
            Int64 sharing = (*int64Out)[0] * ProcessList::MEMORY_PAGE_SIZE / 1024;
            (*int64Out)[0] = 0;
            Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_unshared"),
                    *SINGLE_LONG_FORMAT, NULL, int64Out, NULL, &result);
            Int64 unshared = (*int64Out)[0] * ProcessList::MEMORY_PAGE_SIZE / 1024;
            (*int64Out)[0] = 0;
            Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_volatile"),
                    *SINGLE_LONG_FORMAT, NULL, int64Out, NULL, &result);
            Int64 voltile = (*int64Out)[0] * ProcessList::MEMORY_PAGE_SIZE / 1024;
            AutoPtr<IActivityManagerHelper> amhelper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
            Int32 staticGetMemoryClass, staticGetLargeMemoryClass;
            Boolean isLowRamDeviceStatic, isHighEndGfx;
            amhelper->StaticGetMemoryClass(&staticGetMemoryClass);
            amhelper->StaticGetLargeMemoryClass(&staticGetLargeMemoryClass);
            amhelper->IsLowRamDeviceStatic(&isLowRamDeviceStatic);
            amhelper->IsHighEndGfx(&isHighEndGfx);
            if (!isCompact) {
                if (sharing != 0 || shared != 0 || unshared != 0 || voltile != 0) {
                    pw->Print(String("      KSM: "));
                    pw->Print(sharing);
                    pw->Print(String(" kB saved from shared "));
                    pw->Print(shared);
                    pw->Println(String(" kB"));
                    pw->Print(String("           "));
                    pw->Print(unshared);
                    pw->Print(String(" kB unshared; "));
                    pw->Print(voltile);
                    pw->Println(String(" kB volatile"));
                }
                pw->Print(String("   Tuning: "));
                pw->Print(staticGetMemoryClass);
                pw->Print(String(" (large "));
                pw->Print(staticGetLargeMemoryClass);
                pw->Print(String("), oom "));
                pw->Print(mProcessList->GetMemLevel(ProcessList::CACHED_APP_MAX_ADJ)/1024);
                pw->Print(String(" kB"));
                pw->Print(String(", restore limit "));
                pw->Print(mProcessList->GetCachedRestoreThresholdKb());
                pw->Print(String(" kB"));
                if (isLowRamDeviceStatic) {
                    pw->Print(String(" (low-ram)"));
                }
                if (isHighEndGfx) {
                    pw->Print(String(" (high-end-gfx)"));
                }
                pw->Println();
            }
            else {
                pw->Print(String("ksm,"));
                pw->Print(sharing);
                pw->Print(String(","));
                pw->Print(shared);
                pw->Print(String(","));
                pw->Print(unshared);
                pw->Print(String(","));
                pw->Println(voltile);
                pw->Print(String("tuning,"));
                pw->Print(staticGetMemoryClass);
                pw->Print(',');
                pw->Print(staticGetLargeMemoryClass);
                pw->Print(',');
                pw->Print(mProcessList->GetMemLevel(ProcessList::CACHED_APP_MAX_ADJ)/1024);
                if (isLowRamDeviceStatic) {
                    pw->Print(String(",low-ram"));
                }
                if (isHighEndGfx) {
                    pw->Print(String(",high-end-gfx"));
                }
                pw->Println();
            }
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::ScanArgs(
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& value)
{
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            String arg = (*args)[i];
            if (value.Equals(arg)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean CActivityManagerService::RemoveDyingProviderLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ ContentProviderRecord* acpr,
    /* [in] */ Boolean always)
{
    assert(acpr != NULL);
    AutoPtr<ContentProviderRecord> cpr = acpr;

    Boolean inLaunching = mLaunchingProviders.End()
            != Find(mLaunchingProviders.Begin(), mLaunchingProviders.End(), cpr);

    if (!inLaunching || always) {
        {
            //    {    AutoLock syncLock(cpr);
            AutoPtr<IThread> thread;
            Thread::Attach((IThread**)&thread);
            cpr->Lock();
            cpr->mLaunchingApp = NULL;
            cpr->NotifyAll();
            cpr->Unlock();
        }

        mProviderMap->RemoveProviderByClass(cpr->mName, UserHandle::GetUserId(cpr->mUid));

        String cprAuth;
        cpr->mInfo->GetAuthority(&cprAuth);
        AutoPtr<ArrayOf<String> > names;
        StringUtils::Split(cprAuth, String(";"), (ArrayOf<String>**)&names);
        for (Int32 j = 0; j < names->GetLength(); j++) {
            mProviderMap->RemoveProviderByName((*names)[j], UserHandle::GetUserId(cpr->mUid));
        }
    }

    List< AutoPtr<CContentProviderConnection> >::Iterator cit = cpr->mConnections.Begin();
    while (cit != cpr->mConnections.End()) {
        AutoPtr<CContentProviderConnection> conn = *cit;
        if (conn->mWaiting) {
            // If this connection is waiting for the provider, then we don't
            // need to mess with its process unless we are always removing
            // or for some reason the provider is not currently launching.
            if (inLaunching && !always) {
                ++cit;
                continue;
            }
        }
        AutoPtr<ProcessRecord> capp = conn->mClient;
        conn->mDead = TRUE;
        if (conn->mStableCount > 0) {
            if (!capp->mPersistent && capp->mThread != NULL
                    && capp->mPid != 0 && capp->mPid != MY_PID) {
                String cprName;
                cpr->mName->FlattenToShortString(&cprName);
                StringBuilder sb;
                sb += "depends on provider ";
                sb += cprName;
                sb += " in dying proc ";
                sb += proc != NULL ? proc->mProcessName : "??";
                capp->Kill(sb.ToString(), TRUE);
            }
        }
        else if (capp->mThread != NULL && conn->mProvider->mProvider != NULL) {
            // try {
            FAIL_RETURN(capp->mThread->UnstableProviderDied(IBinder::Probe(conn->mProvider->mProvider)));
            // } catch (RemoteException e) {
            // }
            // In the protocol here, we don't expect the client to correctly
            // clean up this connection, we'll just remove it.
            cit = cpr->mConnections.Erase(cit);
            conn->mClient->mConProviders.Remove(conn);
            continue;
        }
        ++cit;
    }

    if (inLaunching && always) {
        mLaunchingProviders.Remove(cpr);
    }
    return inLaunching;
}

Boolean CActivityManagerService::CleanUpApplicationRecordLocked(
    /* [in] */ ProcessRecord* aApp,
    /* [in] */ Boolean restarting,
    /* [in] */ Boolean allowRestart,
    /* [in] */ Int32 index)
{
    AutoPtr<ProcessRecord> app = aApp;

    if (index >= 0) {
        RemoveLruProcessLocked(app);
        ProcessList::Remove(app->mPid);
    }

    mProcessesToGc.Remove(app);
    mPendingPssProcesses.Remove(app);

    // Dismiss any open dialogs.
    if (app->mCrashDialog != NULL && !app->mForceCrashReport) {
        IDialogInterface::Probe(app->mCrashDialog)->Dismiss();
        app->mCrashDialog = NULL;
    }
    if (app->mAnrDialog != NULL) {
        IDialogInterface::Probe(app->mAnrDialog)->Dismiss();
        app->mAnrDialog = NULL;
    }
    if (app->mWaitDialog != NULL) {
        IDialogInterface::Probe(app->mWaitDialog)->Dismiss();
        app->mWaitDialog = NULL;
    }

    app->mCrashing = FALSE;
    app->mNotResponding = FALSE;

    app->ResetPackageList(mProcessStats);
    app->UnlinkDeathRecipient();
    app->MakeInactive(mProcessStats);
    app->mWaitingToKill = NULL;
    app->mForcingToForeground = NULL;
    UpdateProcessForegroundLocked(app, FALSE, FALSE);
    app->mForegroundActivities = FALSE;
    app->mHasShownUi = FALSE;
    app->mTreatLikeActivity = FALSE;
    app->mHasAboveClient = FALSE;
    app->mHasClientActivities = FALSE;

    mServices->KillServicesLocked(app, allowRestart);

    Boolean restart = FALSE;

    // Remove published content providers.
    HashMap<String, AutoPtr<ContentProviderRecord> >::Iterator it =
            app->mPubProviders.Begin();
    for (; it != app->mPubProviders.End(); ++it) {
        AutoPtr<ContentProviderRecord> cpr = it->mSecond;

        const Boolean always = app->mBad || !allowRestart;
        if (RemoveDyingProviderLocked(app, cpr, always) || always) {
            // We left the provider in the launching list, need to
            // restart it.
            restart = TRUE;
        }

        cpr->mProvider = NULL;
        cpr->mProc = NULL;
    }
    app->mPubProviders.Clear();

    // Take care of any launching providers waiting for this process.
    if (CheckAppInLaunchingProvidersLocked(app, FALSE)) {
        restart = TRUE;
    }

    // Unregister from connected content providers.
    if (!app->mConProviders.IsEmpty()) {
        List<AutoPtr<CContentProviderConnection> >::Iterator it = app->mConProviders.Begin();
        for (; it != app->mConProviders.End(); ++it) {
            AutoPtr<CContentProviderConnection> conn = *it;
            List< AutoPtr<CContentProviderConnection> >::Iterator cpcIt =
                Find(conn->mProvider->mConnections.Begin(), conn->mProvider->mConnections.End(), conn);
            if (cpcIt != conn->mProvider->mConnections.End())
                conn->mProvider->mConnections.Erase(cpcIt);
        }
        app->mConProviders.Clear();
    }

    // At this point there may be remaining entries in mLaunchingProviders
    // where we were the only one waiting, so they are no longer of use.
    // Look for these and clean up if found.
    // XXX Commented out for now.  Trying to figure out a way to reproduce
    // the actual situation to identify what is actually going on.
    if (FALSE) {
        List< AutoPtr<ContentProviderRecord> >::Iterator it = mLaunchingProviders.Begin();
        for (; it != mLaunchingProviders.End(); ++it) {
            AutoPtr<ContentProviderRecord> cpr = *it;
            if (cpr->mConnections.IsEmpty() && !cpr->HasExternalProcessHandles()) {
                {
                    cpr->Lock();
                    cpr->mLaunchingApp = NULL;
                    cpr->NotifyAll();
                    cpr->Unlock();
                }
            }
        }
    }

    SkipCurrentReceiverLocked(app);

    // Unregister any receivers.
    HashSet< AutoPtr<ReceiverList> >::Iterator rit = app->mReceivers.Begin();
    for (; rit != app->mReceivers.End(); ++rit) {
        RemoveReceiverLocked(*rit);
    }
    app->mReceivers.Clear();

    // If the app is undergoing backup, tell the backup manager about it
    if (mBackupTarget != NULL && app->mPid == mBackupTarget->mApp->mPid) {
        if (DEBUG_BACKUP || DEBUG_CLEANUP) {
            String aiDes;
            mBackupTarget->mAppInfo->ToString(&aiDes);
            Slogger::D(TAG, "App %s died during backup", aiDes.string());
        }
        // try {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::BACKUP_SERVICE);
        AutoPtr<IIBackupManager> bm = IIBackupManager::Probe(service);
        String appCName;
        IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&appCName);
        bm->AgentDisconnected(appCName);
        // } catch (RemoteException e) {
        //     // can't happen; backup manager is local
        // }
    }

    List< AutoPtr<ProcessChangeItem> >::Iterator cit = mPendingProcessChanges.Begin();
    for (; cit != mPendingProcessChanges.End();) {
        AutoPtr<ProcessChangeItem> item = *cit;
        if (item->mPid == app->mPid) {
            cit = mPendingProcessChanges.Erase(cit);
            mAvailProcessChanges.PushBack(item);
        }
        else {
            ++cit;
        }
    }

    Int32 uid;
    app->mInfo->GetUid(&uid);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DISPATCH_PROCESS_DIED, app->mPid, uid, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    // If the caller is restarting this app, then leave it in its
    // current lists and let the caller take care of it.
    if (restarting) {
        return FALSE;
    }

    if (!app->mPersistent || app->mIsolated) {
        if (DEBUG_PROCESSES || DEBUG_CLEANUP) {
            String appDes = app->ToString();
            Slogger::V(TAG, "Removing non-persistent process during cleanup: %s", appDes.string());
        }
        mProcessNames->Remove(app->mProcessName, app->mUid);
        mIsolatedProcesses.Erase(app->mUid);
        if (mHeavyWeightProcess == app) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG,
                mHeavyWeightProcess->mUserId, 0, (IMessage**)&msg);
            Boolean result;
            mHandler->SendMessage(msg, &result);

            mHeavyWeightProcess = NULL;
        }
    }
    else if (!app->mRemoved) {
        // This app is persistent, so we need to keep its record around.
        // If it is not already on the pending app list, add it there
        // and start a new process for it.
        Int32 index;
        if (mPersistentStartingProcesses->IndexOf((IProcessRecord*)app.Get(), &index), index < 0) {
            mPersistentStartingProcesses->Add((IProcessRecord*)app.Get());
            restart = TRUE;
        }
    }
    Boolean contains;
    if ((DEBUG_PROCESSES || DEBUG_CLEANUP)
            && (mProcessesOnHold->Contains((IProcessRecord*)app, &contains), contains)) {
        String appDes = app->ToString();
        Slogger::V(TAG, "Clean-up removing on hold: %s", appDes.string());
    }
    mProcessesOnHold->Remove((IProcessRecord*)app);

    if (app == mHomeProcess) {
        mHomeProcess = NULL;
    }
    if (app == mPreviousProcess) {
        mPreviousProcess = NULL;
    }

    if (restart && !app->mIsolated) {
        // We have components that still need to be running in the
        // process, so re-launch it.
        if (index < 0) {
            ProcessList::Remove(app->mPid);
        }
        mProcessNames->Put(app->mProcessName, app->mUid, app);
        StartProcessLocked(app, String("restart"), app->mProcessName);
        return TRUE;
    }
    else if (app->mPid > 0 && app->mPid != MY_PID) {
        // Goodbye!
        {
            AutoLock lock(mPidsSelfLockedLock);

            mPidsSelfLocked.Erase(app->mPid);
            mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, app->Probe(EIID_IInterface));
        }
        mBatteryStatsService->NoteProcessFinish(app->mProcessName, uid);
        if (app->mIsolated) {
            mBatteryStatsService->RemoveIsolatedUid(app->mUid, uid);
        }
        app->SetPid(0);
    }
    return FALSE;
}

Boolean CActivityManagerService::CheckAppInLaunchingProvidersLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean alwaysBad)
{
    // Look through the content providers we are waiting to have launched,
    // and if any run in this process then either schedule a restart of
    // the process or kill the client waiting for it if this process has
    // gone bad.
    Int32 NL = mLaunchingProviders.GetSize();
    Boolean restart = FALSE;
    for (Int32 i = 0; i < NL; i++) {
        AutoPtr<ContentProviderRecord> cpr = mLaunchingProviders[i];
        if (cpr->mLaunchingApp.Get() == app) {
            if (!alwaysBad && !app->mBad) {
                restart = TRUE;
            }
            else {
                RemoveDyingProviderLocked(app, cpr, TRUE);
                // cpr should have been removed from mLaunchingProviders
                NL = mLaunchingProviders.GetSize();
                i--;
            }
        }
    }
    return restart;
}

// =========================================================
// SERVICES
// =========================================================

ECode CActivityManagerService::GetServices(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [out] */ IList** services)
{
    VALIDATE_NOT_NULL(services);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getServices")));
    AutoLock lock(this);
    AutoPtr<IList> infoList = mServices->GetRunningServiceInfoLocked(maxNum, flags);
    *services = infoList;
    REFCOUNT_ADD(*services);
    return NOERROR;
}

ECode CActivityManagerService::GetRunningServiceControlPanel(
    /* [in] */ IComponentName* name,
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getRunningServiceControlPanel")));

    AutoLock lock(this);
    return mServices->GetRunningServiceControlPanelLocked(name, intent);
}

ECode CActivityManagerService::StartService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("startService")));

    // Refuse possible leaked file descriptors
    Boolean result;
    if (service != NULL &&
        (service->HasFileDescriptors(&result), result == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DEBUG_SERVICE) {
        String serviceDes;
        service->ToString(&serviceDes);
        Slogger::V(TAG, "startService: %s type=%s",
                serviceDes.string(), resolvedType.string());
    }

    AutoLock lock(this);

    const Int32 callingPid = Binder::GetCallingPid();
    const Int32 callingUid = Binder::GetCallingUid();
    const Int64 origId = Binder::ClearCallingIdentity();
    ECode ec = mServices->StartServiceLocked(caller, service, resolvedType,
            callingPid, callingUid, userId, name);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::StartServiceInPackage(
    /* [in] */ Int32 uid,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName);
    *componentName = NULL;

    AutoPtr<IComponentName> name;
    AutoLock lock(this);
    if (DEBUG_SERVICE) {
        String serviceDes;
        service->ToString(&serviceDes);
        Slogger::V(TAG, "startServiceInPackage: %s type=%s",
                serviceDes.string(), resolvedType.string());
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    ECode ec = mServices->StartServiceLocked(NULL, service, resolvedType, -1,
            uid, userId, (IComponentName**)&name);
    Binder::RestoreCallingIdentity(origId);

    *componentName = name;
    REFCOUNT_ADD(*componentName);
    return ec;
}

ECode CActivityManagerService::StopService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("stopService")));
    // Refuse possible leaked file descriptors
    Boolean hasFD;
    if (service != NULL && (service->HasFileDescriptors(&hasFD), hasFD == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);
    return mServices->StopServiceLocked(caller, service, resolvedType, userId, result);
}

ECode CActivityManagerService::PeekService(
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("peekService")));

    // Refuse possible leaked file descriptors
    Boolean hasFD;
    if (service != NULL && (service->HasFileDescriptors(&hasFD), hasFD == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);
    return mServices->PeekServiceLocked(service, resolvedType, token);
}

ECode CActivityManagerService::StopServiceToken(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 startId,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;

    AutoLock lock(this);
    return mServices->StopServiceTokenLocked(className, token, startId, succeeded);
}

ECode CActivityManagerService::SetServiceForeground(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ Boolean removeNotification)
{
    AutoLock lock(this);
    return mServices->SetServiceForegroundLocked(
            className, token, id, notification, removeNotification);
}

ECode CActivityManagerService::HandleIncomingUser(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean allowAll,
    /* [in] */ Boolean requireFull,
    /* [in] */ const String& name,
    /* [in] */ const String& callerPackage,
    /* [out] */ Int32* result)
{
    return HandleIncomingUser(callingPid, callingUid, userId, allowAll,
            requireFull ? ALLOW_FULL_ONLY : ALLOW_NON_FULL, name, callerPackage, result);
}

Int32 CActivityManagerService::UnsafeConvertIncomingUser(
    /* [in] */ Int32 userId)
{
    return (userId == IUserHandle::USER_CURRENT || userId == IUserHandle::USER_CURRENT_OR_SELF)
            ? mCurrentUserId : userId;
}

ECode CActivityManagerService::HandleIncomingUser(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean allowAll,
    /* [in] */ Int32 allowMode,
    /* [in] */ const String& name,
    /* [in] */ const String& callerPackage,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IUserHandle::USER_NULL;
    Int32 callingUserId = UserHandle::GetUserId(callingUid);
    if (callingUserId == userId) {
        *result = userId;
        return NOERROR;
    }

    // Note that we may be accessing mCurrentUserId outside of a lock...
    // shouldn't be a big deal, if this is being called outside
    // of a locked context there is intrinsically a race with
    // the value the caller will receive and someone else changing it.
    // We assume that USER_CURRENT_OR_SELF will use the current user; later
    // we will switch to the calling user if access to the current user fails.
    Int32 targetUserId = UnsafeConvertIncomingUser(userId);

    if (callingUid != 0 && callingUid != IProcess::SYSTEM_UID) {
        Boolean allow = FALSE;
        if (CheckComponentPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL, callingPid,
                callingUid, -1, TRUE) == IPackageManager::PERMISSION_GRANTED) {
            // If the caller has this permission, they always pass go.  And collect $200.
            allow = TRUE;
        }
        else if (allowMode == ALLOW_FULL_ONLY) {
            // We require full access, sucks to be you.
            allow = FALSE;
        }
        else if (CheckComponentPermission(Manifest::permission::INTERACT_ACROSS_USERS, callingPid,
                callingUid, -1, TRUE) != IPackageManager::PERMISSION_GRANTED) {
            // If the caller does not have either permission, they are always doomed.
            allow = FALSE;
        }
        else if (allowMode == ALLOW_NON_FULL) {
            // We are blanket allowing non-full access, you lucky caller!
            allow = TRUE;
        }
        else if (allowMode == ALLOW_NON_FULL_IN_PROFILE) {
            // We may or may not allow this depending on whether the two users are
            // in the same profile.
            {    AutoLock syncLock(mUserProfileGroupIdsSelfLockedLock);
                HashMap<Int32, Int32>::Iterator it = mUserProfileGroupIdsSelfLocked.Find(callingUserId);
                Int32 callingProfile = it != mUserProfileGroupIdsSelfLocked.End() ?
                        it->mSecond : IUserInfo::NO_PROFILE_GROUP_ID;
                it = mUserProfileGroupIdsSelfLocked.Find(targetUserId);
                Int32 targetProfile = it != mUserProfileGroupIdsSelfLocked.End() ?
                        it->mSecond : IUserInfo::NO_PROFILE_GROUP_ID;
                allow = callingProfile != IUserInfo::NO_PROFILE_GROUP_ID
                        && callingProfile == targetProfile;
            }
        }
        else {
            Slogger::E(TAG, "Unknown mode: %d", allowMode);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (!allow) {
            if (userId == IUserHandle::USER_CURRENT_OR_SELF) {
                // In this case, they would like to just execute as their
                // owner user instead of failing.
                targetUserId = callingUserId;
            }
            else {
                StringBuilder builder(128);
                builder += "Permission Denial: ";
                builder += name;
                if (callerPackage.IsNull()) {
                    builder += " from ";
                    builder += callerPackage;
                }
                builder += " asks to run as user ";
                builder += userId;
                builder += " but is calling from user ";
                builder += UserHandle::GetUserId(callingUid);
                builder += "; this requires ";
                builder += Manifest::permission::INTERACT_ACROSS_USERS_FULL;
                if (allowMode != ALLOW_FULL_ONLY) {
                    builder += " or ";
                    builder += Manifest::permission::INTERACT_ACROSS_USERS;
                }
                String msg = builder.ToString();
                Slogger::W(TAG, msg.string());
                return E_SECURITY_EXCEPTION;
            }
        }
    }

    if (!allowAll && targetUserId < 0) {
        Slogger::E(TAG, "Call does not support special user #%d", targetUserId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Check shell permission
    if (callingUid == IProcess::SHELL_UID && targetUserId >= IUserHandle::USER_OWNER) {
        Boolean res;
        mUserManager->HasUserRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES,
            targetUserId, &res);
        if (res) {
            Slogger::E(TAG, "Shell does not have permission to access user %d\n", targetUserId);
        }
    }

    *result = targetUserId;
    return NOERROR;
}

ECode CActivityManagerService::IsSingleton(
    /* [in] */ const String& componentProcessName,
    /* [in] */ IApplicationInfo* aInfo,
    /* [in] */ const String& className,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (aInfo == NULL) {
        return NOERROR;

    }
    Int32 aUid, infoFlags;
    aInfo->GetUid(&aUid);
    aInfo->GetFlags(&infoFlags);
    String pkgName;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
    // For apps that don't have pre-defined UIDs, check for permission
    if (UserHandle::GetAppId(aUid) >= IProcess::FIRST_APPLICATION_UID) {
        if ((flags & IServiceInfo::FLAG_SINGLE_USER) != 0) {
            AutoPtr<IActivityManagerHelper> helper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
            Int32 permission;
            helper->CheckUidPermission(Manifest::permission::INTERACT_ACROSS_USERS, aUid, &permission);
            if (permission != IPackageManager::PERMISSION_GRANTED) {
                AutoPtr<IComponentName> comp;
                CComponentName::New(pkgName, className, (IComponentName**)&comp);
                String compString;
                comp->FlattenToShortString(&compString);
                StringBuilder msg("Permission Denial: Component ");
                msg += compString;
                msg += " requests FLAG_SINGLE_USER, but app does not hold ";
                msg += "Manifest::permission::INTERACT_ACROSS_USERS";
                Slogger::W(TAG, msg.ToString().string());
                return E_SECURITY_EXCEPTION;
            }
            // Permission passed
            *result = TRUE;
        }
    }
    else if (componentProcessName.Equals("system")) {
        *result = TRUE;
    }
    else if ((flags & IServiceInfo::FLAG_SINGLE_USER) != 0) {
        // Phone app and persistent apps are allowed to export singleuser providers.
        Boolean isSameApp = UserHandle::IsSameApp(aUid, IProcess::PHONE_UID);
        *result = isSameApp || (infoFlags & IApplicationInfo::FLAG_PERSISTENT) != 0;
    }

    if (DEBUG_MU) {
        String infoDes = Object::ToString(aInfo);
        Slogger::V(TAG, "isSingleton(%s, %s, %s, 0x%s) = %d",
            componentProcessName.string(), infoDes.string(), className.string(),
            StringUtils::ToString(flags, 16).string(), *result);
    }
    return NOERROR;
}

/**
 * Checks to see if the caller is in the same app as the singleton
 * component, or the component is in a special app. It allows special apps
 * to export singleton components but prevents exporting singleton
 * components for regular apps.
 */
Boolean CActivityManagerService::IsValidSingletonCall(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 componentUid)
{
    Int32 componentAppId = UserHandle::GetAppId(componentUid);
    Boolean isSameApp = UserHandle::IsSameApp(callingUid, componentUid);
    AutoPtr<IActivityManagerHelper> amhelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
    Int32 perm;
    return isSameApp || componentAppId == IProcess::SYSTEM_UID
        || componentAppId == IProcess::PHONE_UID
        || (amhelper->CheckUidPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL,
            componentUid, &perm), perm) == IPackageManager::PERMISSION_GRANTED;
}

ECode CActivityManagerService::BindService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIServiceConnection* connection,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("bindService")));

    // Refuse possible leaked file descriptors
    Boolean hasFD;
    if (service != NULL && service->HasFileDescriptors(&hasFD), (hasFD == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);
    return mServices->BindServiceLocked(caller, token, service, resolvedType,
            connection, flags, userId, result);
}

ECode CActivityManagerService::UnbindService(
    /* [in] */ IIServiceConnection* connection,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoLock lock(this);
    return mServices->UnbindServiceLocked(connection, succeeded);
}

ECode CActivityManagerService::PublishService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* service)
{
    // Refuse possible leaked file descriptors
    Boolean hasFD;
    if (intent != NULL && (intent->HasFileDescriptors(&hasFD), hasFD == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);

    AutoPtr<IObject> tokenObj = (IObject*)token->Probe(EIID_IObject);
    assert(tokenObj != NULL);
    ClassID clsid;
    tokenObj->GetClassID(&clsid);
    if (clsid != ECLSID_CServiceRecord) {
        Slogger::E(TAG, "Invalid service token");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<CServiceRecord> r = (CServiceRecord*)token;
    return mServices->PublishServiceLocked(r, intent, service);
}

ECode CActivityManagerService::UnbindFinished(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean doRebind)
{
    // Refuse possible leaked file descriptors
    Boolean hasFD;
    if (intent != NULL && (intent->HasFileDescriptors(&hasFD), hasFD == TRUE)) {
        Slogger::E(TAG, "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IObject> tokenObj = (IObject*)token->Probe(EIID_IObject);
    assert(tokenObj != NULL);
    ClassID clsid;
    tokenObj->GetClassID(&clsid);
    if (clsid != ECLSID_CServiceRecord) {
        Slogger::E(TAG, "Invalid service token");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<CServiceRecord> r = (CServiceRecord*)token;
    return mServices->UnbindFinishedLocked(r, intent, doRebind);
}

ECode CActivityManagerService::ServiceDoneExecuting(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 type,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 res)
{
    VALIDATE_NOT_NULL(token);

    AutoLock lock(this);

    AutoPtr<IObject> tokenObj = (IObject*)token->Probe(EIID_IObject);
    assert(tokenObj != NULL);
    ClassID clsid;
    tokenObj->GetClassID(&clsid);
    if (clsid != ECLSID_CServiceRecord) {
        Slogger::E(TAG, "Invalid service token");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<CServiceRecord> r = (CServiceRecord*)token;
    return mServices->ServiceDoneExecutingLocked(r, type, startId, res);
}

// =========================================================
// BACKUP AND RESTORE
// =========================================================

// Cause the target app to be launched if necessary and its backup agent
// instantiated.  The backup agent will invoke backupAgentCreated() on the
// activity manager to announce its creation.
ECode CActivityManagerService::BindBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ Int32 backupMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG_BACKUP) Slogger::V(TAG, "bindBackupAgent: app=%p mode=%d", app, backupMode);
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::CONFIRM_FULL_BACKUP, String("bindBackupAgent")));

    {
        AutoLock lock(this);
        // !!! TODO: currently no check here that we're already bound
        AutoPtr<IBatteryStatsImplUidPkgServ> ss;
        Int32 uid;
        app->GetUid(&uid);
        String pkgName,name;
        IPackageItemInfo::Probe(app)->GetPackageName(&pkgName);
        IPackageItemInfo::Probe(app)->GetName(&name);
        AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
        {    AutoLock syncLock(stats);
            stats->GetServiceStatsLocked(uid, pkgName, name,
                (IBatteryStatsImplUidPkgServ**)&ss);
        }

        // Backup agent is now in use, its package can't be stopped.
        // try {
        AppGlobals::GetPackageManager()->SetPackageStoppedState(
                pkgName, FALSE, UserHandle::GetUserId(uid));
        // } catch (RemoteException e) {
        // } catch (IllegalArgumentException e) {
        //     Slogger::W(TAG, "Failed trying to unstop package "
        //             + app.packageName + ": " + e);
        // }

        AutoPtr<BackupRecord> r = new BackupRecord(ss, app, backupMode);
        String backupAgentName;
        app->GetBackupAgentName(&backupAgentName);
        AutoPtr<IComponentName> hostingName;
        if (backupMode == IApplicationThread::BACKUP_MODE_INCREMENTAL) {
            CComponentName::New(pkgName, backupAgentName, (IComponentName**)&hostingName);
        }
        else {
            CComponentName::New(String("android"), String("FullBackupAgent"), (IComponentName**)&hostingName);
        }
        // startProcessLocked() returns existing proc's record if it's already running
        String processName;
        app->GetProcessName(&processName);
        AutoPtr<ProcessRecord> proc = StartProcessLocked(processName, app,
                FALSE, 0, String("backup"), hostingName, FALSE, FALSE, FALSE);
        if (proc == NULL) {
            Slogger::E(TAG, "Unable to start backup agent process %p", r.Get());
            return FALSE;
        }

        r->mApp = proc;
        mBackupTarget = r;
        IPackageItemInfo::Probe(app)->GetPackageName(&mBackupAppName);

        // Try not to kill the process during backup
        UpdateOomAdjLocked(proc);

        // If the process is already attached, schedule the creation of the backup agent now.
        // If it is not yet live, this will be done when it attaches to the framework.
        if (proc->mThread != NULL) {
            if (DEBUG_BACKUP) Slogger::V(TAG, "Agent proc already running: %p", proc.Get());
    //         try {
            proc->mThread->ScheduleCreateBackupAgent(app,
                        CompatibilityInfoForPackageLocked(app), backupMode);
    //         } catch (RemoteException e) {
    //             // Will time out on the backup manager side
    //         }
        }
        else {
            if (DEBUG_BACKUP) Slogger::V(TAG, "Agent proc not running, waiting for attach");
        }
        // Invariants: at this point, the target app process exists and the application
        // is either already running or in the process of coming up.  mBackupTarget and
        // mBackupAppName describe the app, so that when it binds back to the AM we
        // know that it's scheduled for a backup-agent operation.
    }

    *result = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::ClearPendingBackup()
{
    if (DEBUG_BACKUP) Slogger::V(TAG, "ClearPendingBackup");
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::BACKUP, String("clearPendingBackup")));

    AutoLock lock(this);
    mBackupTarget = NULL;
    mBackupAppName = NULL;
    return NOERROR;
}

// A backup agent has just come up
ECode CActivityManagerService::BackupAgentCreated(
    /* [in] */ const String& agentPackageName,
    /* [in] */ IBinder* agent)
{
    if (DEBUG_BACKUP) Slogger::V(TAG, "BackupAgentCreated: %s = %p", agentPackageName.string(), agent);

    {
        AutoLock lock(this);
        if (!agentPackageName.Equals(mBackupAppName)) {
            Slogger::E(TAG, "Backup agent created for %s but not requested!", agentPackageName.string());
            return NOERROR;
        }
    }

    Int64 oldIdent = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::BACKUP_SERVICE);
    AutoPtr<IIBackupManager> bm = IIBackupManager::Probe(service);
    bm->AgentConnected(agentPackageName, agent);
    // } catch (RemoteException e) {
    //     // can't happen; the backup manager service is local
    // } catch (Exception e) {
    //     Slogger::W(TAG, "Exception trying to deliver BackupAgent binding: ");
    //     e.printStackTrace();
    // } finally {
    Binder::RestoreCallingIdentity(oldIdent);
    // }
    return NOERROR;
}

ECode CActivityManagerService::UnbindBackupAgent(
    /* [in] */ IApplicationInfo* appInfo)
{
    if (DEBUG_BACKUP) Slogger::V(TAG, "unbindBackupAgent: %p", appInfo);
    if (appInfo == NULL) {
        Slogger::W(TAG, "unbind backup agent for null app");
        return NOERROR;
    }

    {
        AutoLock lock(this);
        // try {
        if (mBackupAppName == NULL) {
            Slogger::W(TAG, "Unbinding backup agent with no active backup");
            return NOERROR;
        }

        String appInfoPkgName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&appInfoPkgName);
        if (!mBackupAppName.Equals(appInfoPkgName)) {
            Slogger::E(TAG, "Unbind of %p but is not the current backup target", appInfo);
            return NOERROR;
        }

        // Not backing this app up any more; reset its OOM adjustment
        AutoPtr<ProcessRecord> proc = mBackupTarget->mApp;
        UpdateOomAdjLocked(proc);

        // If the app crashed during backup, 'thread' will be null here
        if (proc->mThread != NULL) {
            // try {
            FAIL_RETURN(proc->mThread->ScheduleDestroyBackupAgent(appInfo,
                    CompatibilityInfoForPackageLocked(appInfo)));
            // } catch (Exception e) {
            //     Slogger::e(TAG, "Exception when unbinding backup agent:");
            //     e.printStackTrace();
            // }
        }
        // } finally {
        mBackupTarget = NULL;
        mBackupAppName = NULL;
        // }
    }
    return NOERROR;
}

// =========================================================
// BROADCASTS
// =========================================================

AutoPtr< List<AutoPtr<IIntent> > > CActivityManagerService::GetStickiesLocked(
    /* [in] */ const String& action,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ List<AutoPtr<IIntent> >* current,
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr< List<AutoPtr<IIntent> > > cur = current;
    StickyBroadcastIterator sit = mStickyBroadcasts.Find(userId);
    if (sit == mStickyBroadcasts.End()) {
        return cur;
    }

    if (action.IsNull()) {
        return cur;
    }

    AutoPtr<StringIntentMap> stickies = sit->mSecond;
    typename StringIntentMap::Iterator ite = stickies->Find(action);
    if(ite == stickies->End()) {
        return cur;
    }
    AutoPtr<StringIntentList> list = ite->mSecond;
    List< AutoPtr<IIntent> >::Iterator iit = list->Begin();
    for (; iit != list->End(); ++iit) {
       AutoPtr<IIntent> intent = *iit;
       Int32 result;
       filter->Match(resolver, intent, TRUE, TAG, &result);
       if (result >= 0) {
           if (cur == NULL) {
               cur = new List<AutoPtr<IIntent> >();
           }
           cur->PushBack(intent);
       }
    }

    return cur;
}

Boolean CActivityManagerService::IsPendingBroadcastProcessLocked(
    /* [in] */ Int32 pid)
{
    return mFgBroadcastQueue->IsPendingBroadcastProcessLocked(pid)
        || mBgBroadcastQueue->IsPendingBroadcastProcessLocked(pid);
}

void CActivityManagerService::SkipPendingBroadcastLocked(
    /* [in] */ Int32 pid)
{
    Slogger::W(TAG, "Unattached app died before broadcast acknowledged, skipping");
    Int32 size = mBroadcastQueues->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<BroadcastQueue> queue = (*mBroadcastQueues)[i];
        queue->SkipPendingBroadcastLocked(pid);
    }
}

Boolean CActivityManagerService::SendPendingBroadcastsLocked(
    /* [in] */ ProcessRecord* app)
{
    Boolean didSomething = FALSE;

    Int32 size = mBroadcastQueues->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<BroadcastQueue> queue = (*mBroadcastQueues)[i];
        Boolean result;
        queue->SendPendingBroadcastsLocked(app, &result);
        didSomething |= result;
    }

    return didSomething;
}

ECode CActivityManagerService::RegisterReceiver(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& aCallerPackage,
    /* [in] */ IIntentReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& permission,
    /* [in] */ Int32 userId,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;
    VALIDATE_NOT_NULL(filter);

    String nullStr;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("registerReceiver")));
    AutoPtr<List<AutoPtr<IIntent> > > stickyIntents;
    AutoPtr<ProcessRecord> callerApp;
    Int32 callingUid, callingPid;

    String callerPackage(aCallerPackage);
    {
        AutoLock lock(this);

        if (caller != NULL) {
            callerApp = GetRecordForAppLocked(caller);
            if (callerApp == NULL) {
                Slogger::E(TAG, "Unable to find app for caller %s (pid=%d) when registering receiver %s",
                    TO_CSTR(caller), Binder::GetCallingPid(), TO_CSTR(receiver));
                return E_SECURITY_EXCEPTION;
            }

            Int32 uid;
            callerApp->mInfo->GetUid(&uid);
            Boolean res;
            if (uid != IProcess::SYSTEM_UID
                && !(callerApp->mPkgList->ContainsKey(CoreUtils::Convert(callerPackage), &res), res)
                && !callerPackage.Equals("android")) {
                Slogger::E(TAG, "Given caller package %s is not running in process %s",
                    callerPackage.string(), TO_CSTR(callerApp));
                return E_SECURITY_EXCEPTION;
            }

            callingUid = uid;
            callingPid = callerApp->mPid;
        }
        else {
            callerPackage = NULL;
            callingUid = Binder::GetCallingUid();
            callingPid = Binder::GetCallingPid();
        }

        FAIL_RETURN(HandleIncomingUser(callingPid, callingUid, userId,
            TRUE, ALLOW_FULL_ONLY, String("registerReceiver"), callerPackage, &userId));

        Int32 count ;
        filter->CountActions(&count);
        AutoPtr<ArrayOf<String> > actions;
        if (count > 0){
            actions = ArrayOf<String>::Alloc(count);
            String action;
            for (Int32 i = 0; i < count; i++) {
                filter->GetAction(i, &action);
                (*actions)[i] = action;
            }
        }
        else {
            count = 1;
            actions = ArrayOf<String>::Alloc(1);
            (*actions)[0] = String(NULL);
        }

        // Collect stickies of users
        AutoPtr<ArrayOf<Int32> > userIds = ArrayOf<Int32>::Alloc(2);
        (*userIds)[0] = IUserHandle::USER_ALL;
        (*userIds)[1] = UserHandle::GetUserId(callingUid);
        for (Int32 i = 0; i < count; i++) {
            String action = (*actions)[i];
            for (Int32 j = 0; j < userIds->GetLength(); j++) {
                StickyBroadcastIterator it = mStickyBroadcasts.Find((*userIds)[j]);
                if (it != mStickyBroadcasts.End()) {
                    AutoPtr<StringIntentMap> stickies = it->mSecond;
                    StringIntentMap::Iterator it2 = stickies->Find(action);
                    if (it2 != stickies->End()) {
                        if (stickyIntents == NULL) {
                            stickyIntents = new List<AutoPtr<IIntent> >();
                        }
                        AutoPtr<List<AutoPtr<IIntent> > > intents = it2->mSecond;
                        List<AutoPtr<IIntent> >::Iterator it3 = intents->Begin();
                        for (; it3 != intents->End(); ++it3) {
                            stickyIntents->PushBack(*it3);
                        }
                    }
                }
            }
        }
    }

    AutoPtr<List<AutoPtr<IIntent> > > allSticky;
    if (stickyIntents != NULL) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        // Look for any matching sticky broadcasts...
        List<AutoPtr<IIntent> >::Iterator it = stickyIntents->Begin();
        for (; it != stickyIntents->End(); ++it) {
            AutoPtr<IIntent> intent = *it;
            // If intent has scheme "content", it will need to acccess
            // provider that needs to lock mProviderMap in ActivityThread
            // and also it may need to wait application response, so we
            // cannot lock ActivityManagerService here.
            Int32 res;
            filter->Match(resolver, intent, TRUE, TAG, &res);
            if (res >= 0) {
                if (allSticky == NULL) {
                    allSticky = new List<AutoPtr<IIntent> >();
                }
                allSticky->PushBack(intent);
            }
        }
    }

    // The first sticky in the list is returned directly back to the client.
    AutoPtr<IIntent> sticky = allSticky != NULL ? *allSticky->Begin() : NULL;
    if (DEBUG_BROADCAST) {
        Slogger::V(TAG, "Register receiver: %s, filter: %s, sticky: %s",
            TO_CSTR(receiver), TO_CSTR(filter), TO_CSTR(sticky));
    }
    if (receiver == NULL) {
        *intent = sticky;
        REFCOUNT_ADD(*intent)
        return NOERROR;
    }

    AutoLock lock(this);
    if (callerApp != NULL && callerApp->mPid == 0) {
        // Caller already died
        return NOERROR;
    }

    AutoPtr<ReceiverList> rl;
    HashMap<AutoPtr<IBinder>, AutoPtr<ReceiverList> >::Iterator rrIt =
        mRegisteredReceivers.Find(IBinder::Probe(receiver));
    if (rrIt != mRegisteredReceivers.End()) {
        rl = rrIt->mSecond;
    }
    if (rl == NULL) {
        rl = new ReceiverList(this, callerApp, callingPid, callingUid, userId, receiver);
        if (rl->mApp != NULL) {
            rl->mApp->mReceivers.Insert(rl);
        }
        else {
            AutoPtr< ::IProxy > proxy = (::IProxy*)receiver->Probe(::EIID_IProxy);
            assert(proxy != NULL);
            if (FAILED(proxy->LinkToDeath(rl.Get(), 0))) {
                *intent = sticky;
                REFCOUNT_ADD(*intent);
                return NOERROR;
            }
            rl->mLinkedToDeath = TRUE;
        }
        mRegisteredReceivers[IBinder::Probe(receiver)] = rl;
    }
    else if (rl->mUid != callingUid) {
        // throw new IllegalArgumentException(
        //         "Receiver requested to register for uid " + callingUid
        //         + " was previously registered for uid " + rl.uid);
        Slogger::E(TAG, "Receiver requested to register for uid %d was previously registered for uid %d", callingUid, rl->mUid);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (rl->mPid != callingPid) {
        // throw new IllegalArgumentException(
        //         "Receiver requested to register for pid " + callingPid
        //         + " was previously registered for pid " + rl.pid);
        Slogger::E(TAG, "Receiver requested to register for pid %d was previously registered for pid %d", callingPid, rl->mPid);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (rl->mUserId != userId) {
        // throw new IllegalArgumentException(
        //         "Receiver requested to register for user " + userId
        //         + " was previously registered for user " + rl.userId);
        Slogger::E(TAG, "Receiver requested to register for user %d was previously registered for user %d", userId, rl->mUserId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSystem = FALSE;
    if (callerApp != NULL && callerApp->mInfo != NULL) {
        Int32 flags;
        callerApp->mInfo->GetFlags(&flags);
        isSystem = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
    }
    AutoPtr<BroadcastFilter> bf = new BroadcastFilter(
        rl, callerPackage, permission, callingUid, userId, isSystem);
    bf->constructor(filter);
    rl->PushBack(bf);
    if (!bf->DebugCheck()) {
        Slogger::W(TAG, "==> For Dynamic broadast");
    }

    mReceiverResolver->AddFilter(bf);

    if (DEBUG_BROADCAST) {
        Slogger::V(TAG, "Register receiver %s, filter: %s, ReceiverList: %s",
            TO_CSTR(receiver), TO_CSTR(bf), TO_CSTR(rl));
    }

    // Enqueue broadcasts for all existing stickies that match
    // this filter.
    if (allSticky != NULL) {
        AutoPtr<IInterface> bfObj = bf->Probe(EIID_IInterface);
        AutoPtr<IList> receivers;
        CArrayList::New((IList**)&receivers);
        receivers->Add(bfObj);

        List<AutoPtr<IIntent> >::Iterator it = allSticky->Begin();
        for (; it != allSticky->End(); ++it) {
            AutoPtr<IIntent> intent = *it;
            AutoPtr<BroadcastQueue> queue = BroadcastQueueForIntent(intent);
            AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, NULL,
               nullStr, -1, -1, nullStr, nullStr, IAppOpsManager::OP_NONE, receivers, NULL, 0,
               nullStr, NULL, FALSE, TRUE, TRUE, -1);
            queue->EnqueueParallelBroadcastLocked(r);
            queue->ScheduleBroadcastsLocked();
        }
    }

    *intent = sticky;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CActivityManagerService::UnregisterReceiver(
    /* [in] */ IIntentReceiver* receiver)
{
    if (DEBUG_BROADCAST) Slogger::V(TAG, "Unregister receiver: %p", receiver);

    const Int64 origId = Binder::ClearCallingIdentity();
    //try {
    Boolean doTrim = FALSE;
    {
        AutoLock lock(this);
        AutoPtr<ReceiverList> rl;
        HashMap<AutoPtr<IBinder>, AutoPtr<ReceiverList> >::Iterator ite =
                mRegisteredReceivers.Find(IBinder::Probe(receiver));
        if (ite != mRegisteredReceivers.End()){
            rl = ite->mSecond;
        }

        if (rl != NULL){
            AutoPtr<BroadcastRecord> r = rl->mCurBroadcast;
            if (r != NULL && r == r->mQueue->GetMatchingOrderedReceiver(r->mReceiver)) {
                Boolean doNext = r->mQueue->FinishReceiverLocked(r, r->mResultCode, r->mResultData,
                        r->mResultExtras, r->mResultAbort, FALSE);
                if (doNext) {
                    doTrim = TRUE;
                    r->mQueue->ProcessNextBroadcast(FALSE);
                }
            }
            if (rl->mApp != NULL) {
                rl->mApp->mReceivers.Erase(rl);
            }
            RemoveReceiverLocked(rl);
            if (rl->mLinkedToDeath) {
                rl->mLinkedToDeath = FALSE;
                AutoPtr< ::IProxy > proxy = (::IProxy*)rl->mReceiver->Probe(::EIID_IProxy);
                assert(proxy != NULL);
                Boolean res;
                proxy->UnlinkToDeath(rl.Get(), 0, &res);
            }
        }
    }
    if (doTrim){
        TrimApplications();
    }
    //} finally {
    Binder::RestoreCallingIdentity(origId);
    //}
    return NOERROR;
}

void CActivityManagerService::RemoveReceiverLocked(
    /* [in] */ ReceiverList* rl)
{
   mRegisteredReceivers.Erase(IBinder::Probe(rl->mReceiver));
   List<AutoPtr<BroadcastFilter> >::Iterator it = rl->Begin();
   for (; it != rl->End(); ++it) {
       mReceiverResolver->RemoveFilter(*it);
   }
}

void CActivityManagerService::SendPackageBroadcastLocked(
    /* [in] */ Int32 cmd,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 userId)
{
   List<AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
   for (; rit != mLruProcesses.REnd(); ++rit) {
       AutoPtr<ProcessRecord> r = *rit;
       if (r->mThread != NULL && (userId == IUserHandle::USER_ALL || r->mUserId == userId)) {
           r->mThread->DispatchPackageBroadcast(cmd, packages);
       }
   }
}

AutoPtr<IList> CActivityManagerService::CollectReceiverComponents(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 callingUid,
    /* [in] */ ArrayOf<Int32>* users)
{
    AutoPtr<IList> receivers;
    // try {
        HashSet<AutoPtr<IComponentName> > singleUserReceivers;
        Boolean scannedFirstReceivers = FALSE;
        Int32 flags;
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        for (Int32 i = 0; i < users->GetLength(); i++) {
            Int32 user = (*users)[i];
            // Skip users that have Shell restrictions
            Boolean res;
            if (callingUid == IProcess::SHELL_UID
                && (GetUserManagerLocked()->HasUserRestriction(
                IUserManager::DISALLOW_DEBUGGING_FEATURES, user, &res), res)) {
                continue;
            }

            AutoPtr<IList> newReceivers;
            pm->QueryIntentReceivers(intent, resolvedType, STOCK_PM_FLAGS, user, (IList**)&newReceivers);
            if (user != 0 && newReceivers != NULL) {
                // If this is not the primary user, we need to check for
                // any receivers that should be filtered out.
                Int32 size;
                newReceivers->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> item;
                    newReceivers->Get(i, (IInterface**)&item);
                    AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);

                    AutoPtr<IActivityInfo> ai;
                    ri->GetActivityInfo((IActivityInfo**)&ai);
                    ai->GetFlags(&flags);
                    if ((flags & IActivityInfo::FLAG_PRIMARY_USER_ONLY) != 0) {
                        newReceivers->Remove(i);
                        i--;
                        size--;
                    }
                }
            }

            if (newReceivers != NULL) {
                Int32 size;
                newReceivers->GetSize(&size);
                if (size == 0) {
                    newReceivers = NULL;
                }
            }

            if (receivers == NULL) {
                receivers = newReceivers;
            }
            else if (newReceivers != NULL) {
                // We need to concatenate the additional receivers
                // found with what we have do far.  This would be easy,
                // but we also need to de-dup any receivers that are
                // singleUser.
                if (!scannedFirstReceivers) {
                    // Collect any single user receivers we had already retrieved.
                    scannedFirstReceivers = TRUE;
                    Int32 flags;
                    String pkgName, name;
                    Int32 size;
                    receivers->GetSize(&size);
                    for (Int32 i = 0; i < size; i++) {
                        AutoPtr<IInterface> item;
                        receivers->Get(i, (IInterface**)&item);
                        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);
                        AutoPtr<IActivityInfo> ai;
                        ri->GetActivityInfo((IActivityInfo**)&ai);
                        ai->GetFlags(&flags);
                        if ((flags & IActivityInfo::FLAG_SINGLE_USER) != 0) {
                            AutoPtr<IComponentName> cn;
                            IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
                            IPackageItemInfo::Probe(ai)->GetName(&name);
                            CComponentName::New(pkgName, name, (IComponentName**)&cn);
                            singleUserReceivers.Insert(cn);
                        }
                    }
                }

                // Add the new results to the existing results, tracking
                // and de-dupping single user receivers.
                Int32 flags;
                String pkgName, name;
                Int32 size;
                newReceivers->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> item;
                    newReceivers->Get(i, (IInterface**)&item);
                    AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);
                    AutoPtr<IActivityInfo> ai;
                    ri->GetActivityInfo((IActivityInfo**)&ai);
                    ai->GetFlags(&flags);
                    if ((flags & IActivityInfo::FLAG_SINGLE_USER) != 0) {
                        AutoPtr<IComponentName> cn;
                        IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
                        IPackageItemInfo::Probe(ai)->GetName(&name);
                        CComponentName::New(pkgName, name, (IComponentName**)&cn);

                        HashSet<AutoPtr<IComponentName> >::Iterator ite = singleUserReceivers.Find(cn);
                        if(ite == singleUserReceivers.End()) {
                            singleUserReceivers.Insert(cn);
                            receivers->Add(ri);
                        }
                    }
                    else {
                        receivers->Add(ri);
                    }
                }
            }
        }
    // } catch (RemoteException ex) {
    //     // pm is in same process, this will never happen.
    // }
    return receivers;
}

ECode CActivityManagerService::BroadcastIntentLocked(
    /* [in] */ ProcessRecord* callerApp,
    /* [in] */ const String& callerPackage,
    /* [in] */ IIntent* inIntent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* resultTo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* map,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IActivityManager::BROADCAST_STICKY_CANT_HAVE_PERMISSION;

    AutoPtr<IIntent> intent;
    CIntent::New(inIntent, (IIntent**)&intent);

    // By default broadcasts do not go to stopped apps.
    intent->AddFlags(IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES);

    if (DEBUG_BROADCAST_LIGHT) {
        Slogger::D(TAG, "Send %s ordered=%d, userId=%d, %s",
            (sticky ? "broadcast sticky: " : "broadcast: "), ordered, userId, TO_CSTR(intent));
    }
    if ((resultTo != NULL) && !ordered) {
        Slogger::W(TAG, "Broadcast %s not ordered but result callback requested!", TO_CSTR(intent));
    }

   FAIL_RETURN(HandleIncomingUser(callingPid, callingUid, userId,
        TRUE, ALLOW_NON_FULL, String("broadcast"), callerPackage, &userId));

    // Make sure that the user who is receiving this broadcast is running.
    // If not, we will just skip it.
    if (userId != IUserHandle::USER_ALL && !IsUserRunningLocked(userId, FALSE)) {
        Int32 flags;
        intent->GetFlags(&flags);
        if (callingUid != IProcess::SYSTEM_UID || (flags & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) == 0) {
            Slogger::W(TAG, "Skipping broadcast of %s: user:%d is stopped", TO_CSTR(intent), userId);
            *result = IActivityManager::BROADCAST_SUCCESS;
            return NOERROR;
        }
    }

    /*
    * Prevent non-system code (defined here to be non-persistent
    * processes) from sending protected broadcasts.
    */
    Int32 callingAppId = UserHandle::GetAppId(callingUid);
    if (callingAppId == IProcess::SYSTEM_UID || callingAppId == IProcess::PHONE_UID
        || callingAppId == IProcess::SHELL_UID || callingAppId == IProcess::BLUETOOTH_UID
        || callingAppId == IProcess::NFC_UID || callingUid == 0) {
        // Always okay.
    }
    else if (callerApp == NULL || !callerApp->mPersistent) {
        //try {
        String action;
        intent->GetAction(&action);
        Boolean b;
        AppGlobals::GetPackageManager()->IsProtectedBroadcast(action, &b);
        if (b) {
            Slogger::W(TAG, "Permission Denial: not allowed to send broadcast %s from pid=%d, uid=%d",
                action.string(), callingPid, callingUid);
            //throw new SecurityException(msg);
            return E_SECURITY_EXCEPTION;
        }
        else if (IAppWidgetManager::ACTION_APPWIDGET_CONFIGURE.Equals(action)) {
            // Special case for compatibility: we don't want apps to send this,
            // but historically it has not been protected and apps may be using it
            // to poke their own app widget.  So, instead of making it protected,
            // just limit it to the caller.
            AutoPtr<IComponentName> comp;
            if (callerApp == NULL) {
                Slogger::W(TAG, "Permission Denial: not allowed to send broadcast %s"
                    " from unknown caller.", action.string());
                return E_SECURITY_EXCEPTION;
            }
            else if ((intent->GetComponent((IComponentName**)&comp), comp != NULL)) {
                // They are good enough to send to an explicit component...  verify
                // it is being sent to the calling app.
                String pkgName, infoPkgName;
                comp->GetPackageName(&pkgName);
                IPackageItemInfo::Probe(callerApp->mInfo)->GetPackageName(&infoPkgName);
                if (!pkgName.Equals(infoPkgName)) {
                    Slogger::W(TAG, "Permission Denial: not allowed to send broadcast %s to %s from %s",
                        action.string(), pkgName.string(), infoPkgName.string());
                    return E_SECURITY_EXCEPTION;
                }
            }
            else {
                String infoPkgName;
                IPackageItemInfo::Probe(callerApp->mInfo)->GetPackageName(&infoPkgName);
                // Limit broadcast to their own package.
                intent->SetPackage(infoPkgName);
            }
        }
        //} catch (RemoteException e) {
        //    Slogger::W(TAG, "Remote exception", e);
        //    return IActivityManager::BROADCAST_SUCCESS;
        //}
    }

    // Handle special intents: if this broadcast is from the package
    // manager about a package being removed, we need to remove all of
    // its activities from the history stack.
    String action;
    intent->GetAction(&action);
    Boolean uidRemoved = action.Equals(IIntent::ACTION_UID_REMOVED);
    if (action.Equals(IIntent::ACTION_PACKAGE_REMOVED)
            || action.Equals(IIntent::ACTION_PACKAGE_CHANGED)
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE)
            || uidRemoved) {
        if (CheckComponentPermission(
                    Manifest::permission::BROADCAST_PACKAGE_REMOVED,
                    callingPid, callingUid, -1, TRUE)
                == IPackageManager::PERMISSION_GRANTED) {
            if (uidRemoved) {
                AutoPtr<IBundle> intentExtras;
                intent->GetExtras((IBundle**)&intentExtras);
                Int32 uid = -1;
                if (intentExtras != NULL) {
                    intentExtras->GetInt32(IIntent::EXTRA_UID, &uid);
                }
                if (uid >= 0) {
                    AutoPtr<IBatteryStatsImpl> bs = mBatteryStatsService->GetActiveStatistics();
                    {
                        AutoLock lock(bs);
                        bs->RemoveUidStatsLocked(uid);
                    }

                    mAppOpsService->UidRemoved(uid);
                }
            }
            else {
                // If resources are unvailble just force stop all
                // those packages and flush the attribute cache as well.
                if (action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
                    AutoPtr< ArrayOf<String> > list;
                    intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST,
                            (ArrayOf<String>**)&list);
                    if (list != NULL && (list->GetLength() > 0)) {
                        for (Int32 i = 0; i < list->GetLength(); i++) {
                            ForceStopPackageLocked((*list)[i], -1, FALSE, TRUE, TRUE,
                                FALSE, FALSE, userId, String("storage unmount"));
                        }
                        CleanupRecentTasksLocked(IUserHandle::USER_ALL);
                        SendPackageBroadcastLocked(
                            IApplicationThread::EXTERNAL_STORAGE_UNAVAILABLE, list, userId);
                    }
                }
                else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(action)) {
                    CleanupRecentTasksLocked(IUserHandle::USER_ALL);
                }
                else {
                    AutoPtr<IUri> data;
                    intent->GetData((IUri**)&data);
                    String ssp;
                    if (data != NULL && (data->GetSchemeSpecificPart(&ssp), !ssp.IsNull())) {
                        Boolean removed = IIntent::ACTION_PACKAGE_REMOVED.Equals(action);
                        Boolean extraReplacing = FALSE;
                        intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &extraReplacing);
                        Boolean fullUninstall = removed && !extraReplacing;
                        Boolean dontKillApp = FALSE;
                        intent->GetBooleanExtra(IIntent::EXTRA_DONT_KILL_APP, FALSE, &dontKillApp);
                        if (!dontKillApp) {
                            Int32 uid = -1;
                            intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &uid);
                            ForceStopPackageLocked(ssp, UserHandle::GetAppId(uid), FALSE, TRUE, TRUE,
                                FALSE, fullUninstall, userId, String(removed ? "pkg removed" : "pkg changed"));
                        }
                        if (removed) {
                            AutoPtr< ArrayOf<String> > sa = ArrayOf<String>::Alloc(1);
                            (*sa)[0] = ssp;
                            SendPackageBroadcastLocked(IApplicationThread::PACKAGE_REMOVED, sa, userId);
                            if (!extraReplacing) {
                                Int32 uid = -1;
                                intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &uid);
                                mAppOpsService->PackageRemoved(uid, ssp);

                                // Remove all permissions granted from/to this package
                                RemoveUriPermissionsForPackageLocked(ssp, userId, TRUE);
                            }
                        }
                    }
                }
            }
        }
        else {
            String action;
            intent->GetAction(&action);
            Slogger::W(TAG, "Permission Denial: %s broadcast from %s (pid=%d, uid=%d)"
                " requires Manifest::permission::BROADCAST_PACKAGE_REMOVED",
                action.string(), callerPackage.string(), callingPid, callingUid);
            return E_SECURITY_EXCEPTION;
        }

    // Special case for adding a package: by default turn on compatibility
    // mode.
    }
    else if (action.Equals(IIntent::ACTION_PACKAGE_ADDED)) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String ssp;
        if (data != NULL && (data->GetSchemeSpecificPart(&ssp), !ssp.IsNull())) {
            Boolean value;
            intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &value);
            mCompatModePackages->HandlePackageAddedLocked(ssp, value);
        }
    }

    /*
    * If this is the time zone changed action, queue up a message that will reset the timezone
    * of all currently running processes. This message will get queued up before the broadcast
    * happens.
    */
    Boolean bval;
    if (action.Equals(IIntent::ACTION_TIMEZONE_CHANGED)) {
        mHandler->SendEmptyMessage(UPDATE_TIME_ZONE, &bval);
    }

    /*
     * If the user set the time, let all running processes know.
     */
    if (action.Equals(IIntent::ACTION_TIME_CHANGED)) {
        Boolean value;
        intent->GetBooleanExtra(
            IIntent::EXTRA_TIME_PREF_24_HOUR_FORMAT, FALSE, &value);
        Int32 is24Hour = value ? 1 : 0;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(UPDATE_TIME, is24Hour, 0, (IMessage**)&msg);
        mHandler->SendMessage(msg, &bval);
        AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
        {    AutoLock syncLock(stats);
            stats->NoteCurrentTimeChangedLocked();
        }
    }

    if (action.Equals(IIntent::ACTION_CLEAR_DNS_CACHE)) {
        mHandler->SendEmptyMessage(CLEAR_DNS_CACHE_MSG, &bval);
    }

    if (action.Equals(Elastos::Droid::Net::IProxy::PROXY_CHANGE_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(Elastos::Droid::Net::IProxy::EXTRA_PROXY_INFO,
            (IParcelable**)&parcelable);
        AutoPtr<IProxyInfo> proxy = IProxyInfo::Probe(parcelable);

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(UPDATE_HTTP_PROXY_MSG, proxy->Probe(EIID_IInterface), (IMessage**)&msg);
        mHandler->SendMessage(msg, &bval);
    }

    // Add to the sticky list if requested.
    if (sticky) {
        Int32 permission;
        CheckPermission(Manifest::permission::BROADCAST_STICKY, callingPid, callingUid, &permission);
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            Slogger::W(TAG, "Permission Denial: broadcastIntent() requesting a sticky broadcast" \
                " from pid=%d, uid=%d requires Manifest::permission::BROADCAST_STICKY", callingPid, callingUid);
            return E_SECURITY_EXCEPTION;
        }
        if (!requiredPermission.IsNull()) {
            Slogger::W(TAG, "Can't broadcast sticky intent %s and enforce permission %s",
                TO_CSTR(intent), requiredPermission.string());
            *result = IActivityManager::BROADCAST_STICKY_CANT_HAVE_PERMISSION;
            return NOERROR;
        }
        AutoPtr<IComponentName> component;
        intent->GetComponent((IComponentName**)&component);
        if (component != NULL) {
            Slogger::E(TAG, "Sticky broadcasts can't target a specific component");
            return E_SECURITY_EXCEPTION;
        }

        // We use userId directly here, since the "all" target is maintained
        // as a separate set of sticky broadcasts.
        if (userId != IUserHandle::USER_ALL) {
            // But first, if this is not a broadcast to all users, then
            // make sure it doesn't conflict with an existing broadcast to
            // all users.
            AutoPtr<StringIntentMap> stickies;
            typename StickyBroadcastHashMap::Iterator it = mStickyBroadcasts.Find(IUserHandle::USER_ALL);
            if (it != mStickyBroadcasts.End()) {
                stickies = it->mSecond;
            }
            if (stickies != NULL) {
                String a;
                intent->GetAction(&a);
                AutoPtr<StringIntentList> list;
                typename StringIntentMap::Iterator ite = stickies->Find(a);
                if (ite != stickies->End()) {
                    list = ite->mSecond;
                }
                if (list != NULL) {
                    List<AutoPtr<IIntent> >::Iterator i;
                    for (i = list->Begin(); i != list->End(); i++) {
                        intent->FilterEquals(*i, &bval);
                        if (bval) {
                            Slogger::E(TAG, "Sticky broadcast %s for user %d conflicts with existing global broadcast",
                                TO_CSTR(intent), userId);
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                        }
                    }
                }
            }
        }

        AutoPtr<StringIntentMap> stickies = mStickyBroadcasts[userId];
        if (stickies == NULL) {
            stickies = new StringIntentMap();
            mStickyBroadcasts[userId] = stickies;
        }

        AutoPtr<List<AutoPtr<IIntent> > > list = (*stickies)[action];
        if (list == NULL) {
            list = new List<AutoPtr<IIntent> >();
            (*stickies)[action] = list;
        }
        List<AutoPtr<IIntent> >::Iterator iter;
        for (iter = list->Begin(); iter != list->End(); ++iter) {
            Boolean equals;
            intent->FilterEquals((IIntent *)*iter, &equals);
            if (equals) {
                // This sticky already exists, replace it.
                AutoPtr<IIntent> newIntent;
                CIntent::New(intent, (IIntent**)&newIntent);
                *iter = newIntent;
                break;
            }
        }
        if (iter == list->End()) {
            AutoPtr<IIntent> newIntent;
            CIntent::New(intent, (IIntent**)&newIntent);
            list->PushBack(newIntent);
        }
    }

    AutoPtr< ArrayOf<Int32> > users;
    if (userId == IUserHandle::USER_ALL) {
        // Caller wants broadcast to go to all started users.
        users = mStartedUserArray;
    }
    else {
        // Caller wants broadcast to go to one specific user.
        users = ArrayOf<Int32>::Alloc(1);
        (*users)[0] = userId;
    }

    // Figure out who all will receive this broadcast.
    AutoPtr<IList> receivers;
    AutoPtr<List<AutoPtr<BroadcastFilter> > > registeredReceivers;
    // Need to resolve the intent to interested receivers...
    Int32 flags;
    intent->GetFlags(&flags);
    if ((flags & IIntent::FLAG_RECEIVER_REGISTERED_ONLY) == 0) {
        receivers = CollectReceiverComponents(intent, resolvedType, callingUid, users);
    }

    AutoPtr<IComponentName> name;
    intent->GetComponent((IComponentName**)&name);
    if (name == NULL) {
        if (userId == IUserHandle::USER_ALL && callingUid == IProcess::SHELL_UID) {
            // Query one target user at a time, excluding shell-restricted users
            AutoPtr<CUserManagerService> ums = GetUserManagerLocked();
            for (Int32 i = 0; i < users->GetLength(); i++) {
                Boolean res;
                ums->HasUserRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES, (*users)[i], &res);
                if (res) {
                    continue;
                }
                AutoPtr<List<AutoPtr<BroadcastFilter> > > registeredReceiversForUser =
                     mReceiverResolver->QueryIntent(intent, resolvedType, FALSE, (*users)[i]);
                if (registeredReceivers == NULL) {
                    registeredReceivers = registeredReceiversForUser;
                }
                else if (registeredReceiversForUser != NULL) {
                    registeredReceivers->Insert(registeredReceivers->End(),
                        registeredReceiversForUser->Begin(), registeredReceiversForUser->End());
                }
            }

        }
        else {
            registeredReceivers = mReceiverResolver->QueryIntent(
                intent, resolvedType, FALSE, userId);
        }

        if (registeredReceivers != NULL && DEBUG_BROADCAST) {
            Slogger::I(TAG, " == registeredReceivers has %d items:", registeredReceivers->GetSize());
            List<AutoPtr<BroadcastFilter> >::Iterator it = registeredReceivers->Begin();
            Int32 i = 0;
            for (; it != registeredReceivers->End(); ++it) {
                Slogger::I(TAG, " >> item %d: %s", i++, TO_CSTR(*it));
            }
        }
    }

    Boolean replacePending = (flags & IIntent::FLAG_RECEIVER_REPLACE_PENDING) !=  0;

    if (DEBUG_BROADCAST) {
        String action;
        intent->GetAction(&action);
        Slogger::V(TAG, "Enqueing broadcast: %s replacePending= %d", action.string(), replacePending);
    }

    if (!ordered && registeredReceivers != NULL && !registeredReceivers->IsEmpty()) {
        AutoPtr<BroadcastQueue> queue = BroadcastQueueForIntent(intent);
        AutoPtr<IList> rlist;
        CArrayList::New((IList**)&rlist);
        List< AutoPtr<BroadcastFilter> >::Iterator it;
        for (it = registeredReceivers->Begin(); it != registeredReceivers->End(); ++it) {
            rlist->Add((*it)->Probe(EIID_IInterface));
        }

        // If we are not serializing this broadcast, then send the
        // registered receivers separately so they don't wait for the
        // components to be launched.
        AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, callerApp,
            callerPackage, callingPid, callingUid, resolvedType, requiredPermission,
            appOp, rlist, resultTo, resultCode, resultData, map,
            ordered, sticky, FALSE, userId);
        if (DEBUG_BROADCAST) {
            Slogger::V(TAG, "Enqueueing parallel broadcast %s, receivers %d", TO_CSTR(r), registeredReceivers->GetSize());
        }
        Boolean replaced = replacePending && queue->ReplaceParallelBroadcastLocked(r);
        if (!replaced) {
            if (DEBUG_BROADCAST) Slogger::V(TAG, "Enqueue and Schedule parallel broadcast %s", TO_CSTR(r));
            queue->EnqueueParallelBroadcastLocked(r);
            queue->ScheduleBroadcastsLocked();
        }

        registeredReceivers = NULL;
    }

    // Merge into one list.
    List< AutoPtr<BroadcastFilter> >::Iterator ir;
    if (registeredReceivers != NULL) {
        ir = registeredReceivers->Begin();
    }
    if (receivers != NULL) {
        // A special case for PACKAGE_ADDED: do not allow the package
        // being added to see this broadcast.  This prevents them from
        // using this as a back door to get run as soon as they are
        // installed.  Maybe in the future we want to have a special install
        // broadcast or such for apps, but we'd like to deliberately make
        // this decision.
        AutoPtr< ArrayOf<String> > skipPackages;
        if (action.Equals(IIntent::ACTION_PACKAGE_ADDED)
                || action.Equals(IIntent::ACTION_PACKAGE_RESTARTED)
                || action.Equals(IIntent::ACTION_PACKAGE_DATA_CLEARED)) {
            AutoPtr<IUri> data;
            intent->GetData((IUri**)&data);
            if (data != NULL) {
                String capName;
                data->GetSchemeSpecificPart(&capName);
                if (!capName.IsNull()) {
                    skipPackages = ArrayOf<String>::Alloc(1);
                    (*skipPackages)[0] = capName;
                }
            }
        }
        else if (action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE)) {
            intent->GetStringArrayExtra(
                IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String> **)&skipPackages);
        }
        if (skipPackages != NULL && (skipPackages->GetLength() > 0)) {
            for (Int32 i = 0; i < skipPackages->GetLength(); ++i) {
                String skipPackage = (*skipPackages)[i];
                if (!skipPackage.IsNull()) {
                    Int32 NT;
                    receivers->GetSize(&NT);
                    for (Int32 it = 0; it < NT; it++) {
                        AutoPtr<IInterface> item;
                        receivers->Get(it, (IInterface**)&item);
                        AutoPtr<IResolveInfo> curt = IResolveInfo::Probe(item);
                        AutoPtr<IActivityInfo> aInfo;
                        curt->GetActivityInfo((IActivityInfo**)&aInfo);
                        String cname;
                        IPackageItemInfo::Probe(aInfo)->GetPackageName(&cname);
                        if (cname.Equals(skipPackage)) {
                            receivers->Remove(it);
                            it--;
                            NT--;
                        }
                    }
                }
            }
        }

        Int32 NT = 0;
        if (receivers != NULL) {
            receivers->GetSize(&NT);
        }
        Int32 it = 0;
        AutoPtr<IResolveInfo> curt;
        AutoPtr<BroadcastFilter> curr;
        while (it < NT  && registeredReceivers != NULL && ir != registeredReceivers->End()) {
            if (curt == NULL) {
                AutoPtr<IInterface> item;
                receivers->Get(it, (IInterface**)&item);
                curt = IResolveInfo::Probe(item);
            }
            if (curr == NULL) {
                curr = *ir;
            }
            Int32 curtPriority, currPriority;
            curt->GetPriority(&curtPriority);
            curr->GetPriority(&currPriority);
            if (currPriority >= curtPriority) {
                // Insert this broadcast record into the final list.
                receivers->Add(it, curr->Probe(EIID_IInterface));
                ++ir;
                curr = NULL;
                it++;
                NT++;
            }
            else {
                // Skip to the next ResolveInfo in the final list.
                it++;
                curt = NULL;
            }
        }
    }

    if (registeredReceivers) {
        while(ir != registeredReceivers->End()) {
            if (receivers == NULL) {
                CArrayList::New((IList**)&receivers);
            }
            receivers->Add((*ir)->Probe(EIID_IInterface));
            ir++;
        }
    }

    if (receivers != NULL && DEBUG_BROADCAST) {
        Int32 NT;
        receivers->GetSize(&NT);
        Slogger::V(TAG, " == receivers has %d items:", NT);
        for (Int32 it = 0; it < NT; it++) {
            AutoPtr<IInterface> item;
            receivers->Get(it, (IInterface**)&item);
            Slogger::V(TAG, " >> item %d: %s", it, TO_CSTR(item));
        }
    }

    Int32 size;
    if ((receivers != NULL && (receivers->GetSize(&size), size > 0)) || resultTo != NULL)  {
        AutoPtr<BroadcastQueue> queue = BroadcastQueueForIntent(intent);
        AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, callerApp,
            callerPackage, callingPid, callingUid, resolvedType,
            requiredPermission, appOp, receivers, resultTo, resultCode,
            resultData, map, ordered, sticky, FALSE, userId);
        if (DEBUG_BROADCAST) {
            Int32 seq = -1;
            r->mIntent->GetInt32Extra(String("seq"), -1, &seq);
            Slogger::V(TAG, "Enqueueing ordered broadcast %s seq=%d: prev had %d",
                TO_CSTR(r), seq, queue->mOrderedBroadcasts.GetSize());
        }

        Boolean replaced = replacePending && queue->ReplaceOrderedBroadcastLocked(r);
        if (!replaced) {
            if (DEBUG_BROADCAST) Slogger::V(TAG, "Enqueue and Schedule ordered broadcast %s", TO_CSTR(r));
            queue->EnqueueOrderedBroadcastLocked(r);
            queue->ScheduleBroadcastsLocked();
        }
    }

    *result = IActivityManager::BROADCAST_SUCCESS;
    return NOERROR;
}

ECode CActivityManagerService::VerifyBroadcastLocked(
    /* [in] */ IIntent* intent,
    /* [out] */ IIntent** newIntent)
{
    VALIDATE_NOT_NULL(newIntent);
    *newIntent = NULL;

    // Refuse possible leaked file descriptors
    if (intent != NULL) {
        Boolean hasFD;
        if (intent->HasFileDescriptors(&hasFD), hasFD) {
            Slogger::E(TAG, "File descriptors passed in Intent.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    VALIDATE_NOT_NULL(intent);
    Int32 flags;
    intent->GetFlags(&flags);

    if (!mProcessesReady) {
        // if the caller really truly claims to know what they're doing, go
        // ahead and allow the broadcast without launching any receivers
        if ((flags & IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT) != 0) {
            CIntent::New(intent, newIntent);
            (*newIntent)->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        }
        else if ((flags & IIntent::FLAG_RECEIVER_REGISTERED_ONLY) == 0){
            String intDes;
            intent->ToString(&intDes);
            Slogger::E(TAG, "Attempt to launch receivers of broadcast intent %s before boot completion",
                intDes.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    if ((flags & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) != 0) {
        Slogger::E(TAG, "Can't use FLAG_RECEIVER_BOOT_UPGRADE here");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (*newIntent == NULL) {
        *newIntent = intent;
        REFCOUNT_ADD(*newIntent);
    }
    return NOERROR;
}

ECode CActivityManagerService::BroadcastIntent(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* resultTo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* map,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ Boolean serialized,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("broadcastIntent")));

    AutoLock lock(this);
    AutoPtr<IIntent> newIntent;
    FAIL_RETURN(VerifyBroadcastLocked(intent, (IIntent**)&newIntent));
    AutoPtr<ProcessRecord> callerApp = GetRecordForAppLocked(caller);

    Int32 callingPid = Binder::GetCallingPid();
    Int32 callingUid = Binder::GetCallingUid();
    Int64 origId = Binder::ClearCallingIdentity();
    String callerPackage;
    if (callerApp != NULL) {
        IPackageItemInfo::Probe(callerApp->mInfo)->GetPackageName(&callerPackage);
    }
    ECode ec = BroadcastIntentLocked(callerApp,
        callerPackage, newIntent, resolvedType, resultTo,
        resultCode, resultData, map, requiredPermission, appOp, serialized,
        sticky, callingPid, callingUid, userId, result);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::BroadcastIntentInPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ IIntent* inIntent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* resultTo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* map,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Boolean serialized,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)

    AutoLock lock(this);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(VerifyBroadcastLocked(inIntent, (IIntent**)&intent));

    Int64 origId = Binder::ClearCallingIdentity();
    BroadcastIntentLocked(NULL, packageName, intent, resolvedType,
        resultTo, resultCode, resultData, map, requiredPermission,
        IAppOpsManager::OP_NONE, serialized, sticky, -1, uid, userId, status);
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::UnbroadcastIntent(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 _userId)
{
    // Refuse possible leaked file descriptors
    if (intent != NULL) {
        Boolean b;
        intent->HasFileDescriptors(&b);
        if(b == TRUE) {
            // throw new IllegalArgumentException("File descriptors passed in Intent");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    Int32 userId;
    HandleIncomingUser(Binder::GetCallingPid(),
            Binder::GetCallingUid(), _userId, TRUE, ALLOW_NON_FULL,
            String("removeStickyBroadcast"), String(NULL), &userId);

    {
        AutoLock lock(this);
        if (CheckCallingPermission(Manifest::permission::BROADCAST_STICKY)
                != IPackageManager::PERMISSION_GRANTED) {
            // String msg = "Permission Denial: unbroadcastIntent() from pid="
            //         + Binder::GetCallingPid()
            //         + ", uid=" + Binder::GetCallingUid()
            //         + " requires " + Manifest::permission::BROADCAST_STICKY;
            // Slogger::W(TAG, msg);
            // throw new SecurityException(msg);
            return E_SECURITY_EXCEPTION;
        }

        StickyBroadcastIterator it = mStickyBroadcasts.Find(userId);
        if (it != mStickyBroadcasts.End()) {
            AutoPtr<StringIntentMap> stickies = it->mSecond;
            String action;
            intent->GetAction(&action);
            typename StringIntentMap::Iterator ite = stickies->Find(action);
            if (ite != stickies->End()) {
                AutoPtr<StringIntentList> list = ite->mSecond;
                List< AutoPtr<IIntent> >::Iterator iter = list->Begin();
                Boolean b;
                for (; iter != list->End(); ++iter) {
                    intent->FilterEquals(*iter, &b);
                    if (b) {
                        list->Erase(iter);
                        break;
                    }
                }
                if (list->IsEmpty()) {
                    stickies->Erase(ite);
                }
            }
            if (stickies->IsEmpty()) {
                mStickyBroadcasts.Erase(it);
            }
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::FinishReceiverLocked(
    /* [in] */ IBinder* receiver,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean resultAbort)
{
    AutoPtr<BroadcastRecord> r = BroadcastRecordForReceiverLocked(receiver);
    if (r == NULL) {
        Slogger::W(TAG, "finishReceiver called but not found on queue");
        return FALSE;
    }

    return r->mQueue->FinishReceiverLocked(r, resultCode, resultData,
        resultExtras, resultAbort, FALSE);
}

ECode CActivityManagerService::BackgroundServicesFinishedLocked(
    /* [in] */ Int32 userId)
{
    Int32 size = mBroadcastQueues->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        (*mBroadcastQueues)[i]->BackgroundServicesFinishedLocked(userId);
    }
    return NOERROR;
}

ECode CActivityManagerService::FinishReceiver(
    /* [in] */ IBinder* who,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean resultAbort,
    /* [in] */ Int32 flags)
{
    if (DEBUG_BROADCAST) {
        Slogger::V(TAG, "Finish receiver: %s", TO_CSTR(who));
    }

    // Refuse possible leaked file descriptors
    if (resultExtras != NULL) {
        Boolean hasFD;
        resultExtras->HasFileDescriptors(&hasFD);
        if (hasFD) {
            Slogger::E(TAG, "File descriptors passed in Bundle");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    Int64 origId = Binder::ClearCallingIdentity();

    //try {
    Boolean doNext = FALSE;
    AutoPtr<BroadcastRecord> r;
    {
        AutoLock lock(this);
        AutoPtr<BroadcastQueue> queue = (flags & IIntent::FLAG_RECEIVER_FOREGROUND) != 0
            ? mFgBroadcastQueue : mBgBroadcastQueue;
        r = queue->GetMatchingOrderedReceiver(who);
        if (r != NULL) {
            doNext = r->mQueue->FinishReceiverLocked(
                    r, resultCode, resultData, resultExtras, resultAbort, TRUE);
        }
    }

    if (doNext) {
        r->mQueue->ProcessNextBroadcast(FALSE);
    }
    TrimApplications();
    //} finally {
    Binder::RestoreCallingIdentity(origId);
    //}
    return NOERROR;
}

// =========================================================
// INSTRUMENTATION
// =========================================================

ECode CActivityManagerService::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* arguments,
    /* [in] */ IInstrumentationWatcher* watcher,
    /* [in] */ IIUiAutomationConnection* uiAutomationConnection,
    /* [in] */ Int32 userId,
    /* [in] */ const String& abiOverride,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("startInstrumentation")));
    HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, FALSE, ALLOW_FULL_ONLY, String("startInstrumentation"), String(NULL), &userId);
    // Refuse possible leaked file descriptors
    Boolean hasDes;
    if (arguments != NULL && (arguments->HasFileDescriptors(&hasDes), hasDes)) {
        // throw new IllegalArgumentException("File descriptors passed in Bundle");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock lock(this);
        AutoPtr<IInstrumentationInfo> ii;
        AutoPtr<IApplicationInfo> ai;
    //     try {
        AutoPtr<IPackageManager> pkgManager;
        mContext->GetPackageManager((IPackageManager**)&pkgManager);
        ECode ec = pkgManager->GetInstrumentationInfo(
                className, STOCK_PM_FLAGS, (IInstrumentationInfo**)&ii);
        String tPackage;
        if (SUCCEEDED(ec)) {
            ii->GetTargetPackage(&tPackage);
            AppGlobals::GetPackageManager()->GetApplicationInfo(
                    tPackage, STOCK_PM_FLAGS, userId, (IApplicationInfo**)&ai);
        }
    //     } catch (PackageManager.NameNotFoundException e) {
    //     } catch (RemoteException e) {
    //     }
        if (ii == NULL) {
            StringBuilder sb("Unable to find instrumentation info for: ");
            sb += className;
            ReportStartInstrumentationFailure(watcher, className,
                    sb.ToString());
            *res = FALSE;
            return NOERROR;
        }
        if (ai == NULL) {
            ReportStartInstrumentationFailure(watcher, className,
                    String("Unable to find instrumentation target package: ") + tPackage);
            *res = FALSE;
            return NOERROR;
        }

        String iiPkgName;
        IPackageItemInfo::Probe(ii)->GetPackageName(&iiPkgName);
        Int32 match;
        pkgManager->CheckSignatures(
                tPackage, iiPkgName, &match);
        if (match < 0 && match != IPackageManager::SIGNATURE_FIRST_NOT_SIGNED) {
            StringBuilder msg("Permission Denial: starting instrumentation ");
            msg += className;
            msg += " from pid=";
            msg += Binder::GetCallingPid();
            msg += ", uid=";
            msg += Binder::GetCallingUid();
            msg += " not allowed because package ";
            msg += iiPkgName;
            msg += " does not have a signature matching the target ";
            msg += tPackage;
            ReportStartInstrumentationFailure(watcher, className, msg.ToString());
            // throw new SecurityException(msg);
            return E_SECURITY_EXCEPTION;
        }

        const Int64 origId = Binder::ClearCallingIdentity();
        // Instrumentation can kill and relaunch even persistent processes
        ForceStopPackageLocked(tPackage, -1, TRUE, FALSE, TRUE, TRUE, FALSE, userId,
            String("start instr"));
        AutoPtr<ProcessRecord> app = AddAppLocked(ai, FALSE, abiOverride);
        app->mInstrumentationClass = className;
        app->mInstrumentationInfo = ai;
        app->mInstrumentationProfileFile = profileFile;
        app->mInstrumentationArguments = arguments;
        app->mInstrumentationWatcher = watcher;
        app->mInstrumentationUiAutomationConnection = uiAutomationConnection;
        app->mInstrumentationResultClass = className;
        Binder::RestoreCallingIdentity(origId);
    }

    *res = TRUE;
    return NOERROR;
}


/**
 * Report errors that occur while attempting to start Instrumentation.  Always writes the
 * error to the logs, but if somebody is watching, send the report there too.  This enables
 * the "am" command to report errors with more information.
 *
 * @param watcher The IInstrumentationWatcher.  Null if there isn't one.
 * @param cn The component name of the instrumentation.
 * @param report The error report.
 */
ECode CActivityManagerService::ReportStartInstrumentationFailure(
    /* [in] */ IInstrumentationWatcher* watcher,
    /* [in] */ IComponentName* cn,
    /* [in] */ const String& report)
{
    Slogger::W(TAG, report);
    // try {
    if (watcher != NULL) {
        AutoPtr<IBundle> results;
        CBundle::New((IBundle**)&results);
        results->PutString(IInstrumentation::REPORT_KEY_IDENTIFIER, String("ActivityManagerService"));
        results->PutString(String("Error"), report);
        watcher->InstrumentationStatus(cn, -1, results);
    }
    // } catch (RemoteException e) {
    //     Slogger::W(TAG, e);
    // }
    return NOERROR;
}

ECode CActivityManagerService::FinishInstrumentationLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    if (app->mInstrumentationWatcher != NULL) {
        // NOTE:  IInstrumentationWatcher *must* be oneway here
        app->mInstrumentationWatcher->InstrumentationFinished(
                app->mInstrumentationClass,
                resultCode,
                results);
    }
    if (app->mInstrumentationUiAutomationConnection != NULL) {
        app->mInstrumentationUiAutomationConnection->Shutdown();
        // Only a UiAutomation can set this flag and now that
        // it is finished we make sure it is reset to its default.
        mUserIsMonkey = FALSE;
    }
    app->mInstrumentationWatcher = NULL;
    app->mInstrumentationClass = NULL;
    app->mInstrumentationInfo = NULL;
    app->mInstrumentationProfileFile = NULL;
    app->mInstrumentationArguments = NULL;

    String pName;
    app->mInfo->GetProcessName(&pName);
    ForceStopPackageLocked(pName, -1, FALSE, FALSE, TRUE, TRUE, FALSE, app->mUserId,
        String("finished inst"));
    return NOERROR;
}

ECode CActivityManagerService::FinishInstrumentation(
    /* [in] */ IApplicationThread* target,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    // Refuse possible leaked file descriptors
    Boolean hasDes;
    if (results != NULL && (results->HasFileDescriptors(&hasDes), hasDes)) {
        // throw new IllegalArgumentException("File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);
    AutoPtr<ProcessRecord> app = GetRecordForAppLocked(target);
    if (app == NULL) {
        Slogger::W(TAG, "finishInstrumentation: no app for %p", target);
        return NOERROR;
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    FinishInstrumentationLocked(app, resultCode, results);
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

// =========================================================
// CONFIGURATION
// =========================================================

ECode CActivityManagerService::GetDeviceConfigurationInfo(
    /* [out] */ IConfigurationInfo** config)
{
    VALIDATE_NOT_NULL(config);
    FAIL_RETURN(CConfigurationInfo::New(config));
    {
        AutoLock lock(this);

        Int32 touchScreen;
        mConfiguration->GetTouchscreen(&touchScreen);
        (*config)->SetReqTouchScreen(touchScreen);
        Int32 keyboard;
        mConfiguration->GetKeyboard(&keyboard);
        (*config)->SetReqKeyboardType(keyboard);
        Int32 navigation;
        mConfiguration->GetNavigation(&navigation);
        (*config)->SetReqNavigation(navigation);
        if (navigation == IConfiguration::NAVIGATION_DPAD
                || navigation == IConfiguration::NAVIGATION_TRACKBALL) {
            Int32 reqInputFeatures;
            (*config)->GetReqInputFeatures(&reqInputFeatures);
            reqInputFeatures |= IConfigurationInfo::INPUT_FEATURE_FIVE_WAY_NAV;
            (*config)->SetReqInputFeatures(reqInputFeatures);
        }
        if (keyboard != IConfiguration::KEYBOARD_UNDEFINED
                && keyboard != IConfiguration::KEYBOARD_NOKEYS) {
            Int32 reqInputFeatures;
            (*config)->GetReqInputFeatures(&reqInputFeatures);
            reqInputFeatures |= IConfigurationInfo::INPUT_FEATURE_HARD_KEYBOARD;
            (*config)->SetReqInputFeatures(reqInputFeatures);
        }
        (*config)->SetReqGlEsVersion(GL_ES_VERSION);
    }
    return NOERROR;
}

AutoPtr<ActivityStack> CActivityManagerService::GetFocusedStack()
{
    return mStackSupervisor->GetFocusedStack();
}

ECode CActivityManagerService::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config);
    AutoLock lock(this);
    CConfiguration::New(mConfiguration, config);
    AutoPtr<IThemeConfig> themeConfig;
    (*config)->GetThemeConfig((IThemeConfig**)&themeConfig);
    if (themeConfig == NULL) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IThemeConfigHelper> tcHelper;
        CThemeConfigHelper::AcquireSingleton((IThemeConfigHelper**)&tcHelper);
        tcHelper->GetBootTheme(cr, (IThemeConfig**)&themeConfig);
        (*config)->SetThemeConfig(themeConfig);
    }
    return NOERROR;
}

ECode CActivityManagerService::UpdatePersistentConfiguration(
    /* [in] */ IConfiguration* values)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::CHANGE_CONFIGURATION,
            String("updateConfiguration()")));
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::WRITE_SETTINGS,
            String("updateConfiguration()")));
    if (values == NULL) {
        // throw new NullPointerException("Configuration must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    {
        AutoLock lock(this);
        const Int64 origId = Binder::ClearCallingIdentity();
        UpdateConfigurationLocked(values, NULL, TRUE, FALSE);
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::UpdateConfiguration(
    /* [in] */ IConfiguration* _values)
{
    FAIL_RETURN(EnforceCallingPermission(Manifest::permission::CHANGE_CONFIGURATION,
           String("UpdateConfiguration()")));

    AutoPtr<IConfiguration> values = _values;
    {
        AutoLock lock(this);
        if (values == NULL && mWindowManager != NULL) {
            // sentinel: fetch the current configuration from the window manager
            values = mWindowManager->ComputeNewConfiguration();
        }

        if (mWindowManager != NULL) {
            mProcessList->ApplyDisplaySize(mWindowManager);
        }

        const Int64 origId = Binder::ClearCallingIdentity();
        if (values != NULL) {
            Settings::System::ClearConfiguration(values);
        }
        UpdateConfigurationLocked(values, NULL, FALSE, FALSE);
        Binder::RestoreCallingIdentity(origId);
    }

    return NOERROR;
}

Boolean CActivityManagerService::UpdateConfigurationLocked(
    /* [in] */ IConfiguration* values,
    /* [in] */ ActivityRecord* starting,
    /* [in] */ Boolean persistent,
    /* [in] */ Boolean initLocale)
{
    Int32 changes = 0;

    if (values != NULL) {
        AutoPtr<IConfiguration> newConfig;
        CConfiguration::New(mConfiguration, (IConfiguration**)&newConfig);
        newConfig->UpdateFrom(values, &changes);
        if (changes != 0) {
            if (DEBUG_SWITCH || DEBUG_CONFIGURATION) {
                Slogger::I(TAG, "Updating configuration to: %s", TO_CSTR(values));
            }

//            EventLog.writeEvent(EventLogTags.CONFIGURATION_CHANGED, changes);

            AutoPtr<ILocale> locale;
            values->GetLocale((ILocale**)&locale);
            if (locale != NULL && !initLocale) {
                AutoPtr<ILocale> curLocale;
                mConfiguration->GetLocale((ILocale**)&curLocale);
                Boolean equals, userSetLocale;
                locale->Equals(curLocale, &equals);
                values->IsUserSetLocale(&userSetLocale);
                SaveLocaleLocked(locale, !equals, userSetLocale);
            }

            AutoPtr<IThemeConfig> themeConfig;
            values->GetThemeConfig((IThemeConfig**)&themeConfig);
            if (themeConfig != NULL) {
                AutoPtr<IThemeConfig> otherTC;
                mConfiguration->GetThemeConfig((IThemeConfig**)&otherTC);
                Boolean equals = Object::Equals(themeConfig, otherTC);
                SaveThemeResourceLocked(themeConfig, !equals);
            }

            mConfigurationSeq++;
            if (mConfigurationSeq <= 0) {
                mConfigurationSeq = 1;
            }
            newConfig->SetSeq(mConfigurationSeq);
            mConfiguration = newConfig;
            Slogger::I(TAG, "Config changed:0x%x %s", changes, TO_CSTR(mConfiguration));
            if (mUsageStatsService != NULL) {
                mUsageStatsService->ReportConfigurationChange(newConfig, mCurrentUserId);
                //mUsageStatsService.noteStartConfig(newConfig);
            }

            AutoPtr<IConfiguration> configCopy;
            CConfiguration::New(mConfiguration, (IConfiguration**)&configCopy);

            // TODO: If our config changes, should we auto dismiss any currently
            // showing dialogs?
            mShowDialogs = ShouldShowDialogs(newConfig);

            AutoPtr<AttributeCache> ac = AttributeCache::GetInstance();
            if (ac != NULL) {
                ac->UpdateConfiguration(configCopy);
            }

            // Make sure all resources in our process are updated
            // right now, so that anyone who is going to retrieve
            // resource values after we return will be sure to get
            // the new ones.  This is especially important during
            // boot, where the first config change needs to guarantee
            // all resources have that config before following boot
            // code is executed.
            mSystemThread->ApplyConfigurationToResources(configCopy);

            Boolean hasChanges = Settings::System::HasInterestingConfigurationChanges(changes);
            if (persistent && hasChanges) {
                if (DEBUG_CONFIGURATION) {
                    Slogger::V(TAG, "Sending UPDATE_CONFIGURATION_MSG with new config %s", TO_CSTR(configCopy));
                }

                AutoPtr<IConfiguration> config;
                CConfiguration::New(configCopy, (IConfiguration**)&config);

                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG, config.Get(), (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessage(msg, &result);
            }

            List<AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
            for (; rit != mLruProcesses.REnd(); ++rit) {
                AutoPtr<ProcessRecord> app = *rit;
                if (app->mThread != NULL) {
                    if (DEBUG_CONFIGURATION) {
                        Slogger::V(TAG, "Sending to proc %s new config %s",
                            app->mProcessName.string(), TO_CSTR(configCopy));
                    }
                    app->mThread->ScheduleConfigurationChanged(configCopy);
                }
            }

            if (DEBUG_CONFIGURATION) {
                Slogger::V(TAG, "Sending Broadcast with ACTION_CONFIGURATION_CHANGED config %s", TO_CSTR(configCopy));
            }

            AutoPtr<IIntent> intent;
            CIntent::New((IIntent::ACTION_CONFIGURATION_CHANGED), (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                    | IIntent::FLAG_RECEIVER_REPLACE_PENDING
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
            Int32 result;
            String nullStr;
            BroadcastIntentLocked(NULL, nullStr, intent, nullStr, NULL, 0,
                nullStr, NULL, nullStr, IAppOpsManager::OP_NONE, FALSE, FALSE, MY_PID,
                IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
            if ((changes & IActivityInfo::CONFIG_LOCALE) != 0) {
                AutoPtr<IIntent> newIntent;
                CIntent::New((IIntent::ACTION_LOCALE_CHANGED), (IIntent**)&newIntent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                BroadcastIntentLocked(NULL, nullStr, newIntent,
                    nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
                    FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
            }
        }
    }

    Boolean kept = TRUE;
    AutoPtr<ActivityStack> mainStack = mStackSupervisor->GetFocusedStack();
    // mainStack is null during startup.
    if (mainStack != NULL) {
        if (changes != 0 && starting == NULL) {
            // If the configuration changed, and the caller is not already
            // in the process of starting an activity, then find the top
            // activity to check if its configuration needs to change.
            starting = mainStack->TopRunningActivityLocked(NULL);
        }

        if (starting != NULL) {
            kept = mainStack->EnsureActivityConfigurationLocked(starting, changes);
            // And we need to make sure at this point that all other activities
            // are made visible with the correct configuration.
            mStackSupervisor->EnsureActivitiesVisibleLocked(starting, changes);
        }
    }

    if (values != NULL && mWindowManager != NULL) {
        mWindowManager->SetNewConfiguration(mConfiguration);
    }

    return kept;
}

Boolean CActivityManagerService::ShouldShowDialogs(
    /* [in] */ IConfiguration* config)
{
    if (config) {
        Int32 keyboard, touchscreen;
        config->GetKeyboard(&keyboard);
        config->GetTouchscreen(&touchscreen);

        return !(keyboard == IConfiguration::KEYBOARD_NOKEYS
                && touchscreen == IConfiguration::TOUCHSCREEN_NOTOUCH);
    }

    return FALSE;
}

///**
// * Save the locale.  You must be inside a AutoLock lock(this); block.
// */
ECode CActivityManagerService::SaveLocaleLocked(
    /* [in] */ ILocale* l,
    /* [in] */ Boolean isDiff,
    /* [in] */ Boolean isPersist)
{
    VALIDATE_NOT_NULL(l);

    String language, region;
    l->GetLanguage(&language);
    l->GetCountry(&region);
    if(isDiff) {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("user.language"), language);
        sysProp->Set(String("user.region"), region);
    }

    if(isPersist) {
        String variant;
        l->GetVariant(&variant);
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("persist.sys.language"), language);
        sysProp->Set(String("persist.sys.country"), region);
        sysProp->Set(String("persist.sys.localevar"), variant);

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(SEND_LOCALE_TO_MOUNT_DAEMON_MSG, l, (IMessage**)&msg);
        Boolean res;
        mHandler->SendMessage(msg, &res);
    }
    return NOERROR;
}

ECode CActivityManagerService::ShouldUpRecreateTask(
    /* [in] */ IBinder* token,
    /* [in] */ const String& destAffinity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    {    AutoLock syncLock(this);
        AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(token);
        if (srec->mTask != NULL && srec->mTask->mStack != NULL) {
            *result = srec->mTask->mStack->ShouldUpRecreateTaskLocked(srec, destAffinity);
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CActivityManagerService::NavigateUpTo(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* destIntent,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
        if (stack != NULL) {
            *result = stack->NavigateUpToLocked(token, destIntent, resultCode, resultData);
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CActivityManagerService::GetLaunchedFromUid(
    /* [in] */ IBinder* activityToken,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(activityToken);
    if (srec == NULL) {
        *result = -1;
    }
    else{
        *result = srec->mLaunchedFromUid;
    }
    return NOERROR;
}

ECode CActivityManagerService::GetLaunchedFromPackage(
    /* [in] */ IBinder* activityToken,
    /* [out] */ String* launchedFromPackage)
{
    VALIDATE_NOT_NULL(launchedFromPackage)
    AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(activityToken);
    if (srec == NULL) {
        *launchedFromPackage = NULL;
    }
    else
        *launchedFromPackage = srec->mLaunchedFromPackage;

    return NOERROR;
}

void CActivityManagerService::SaveThemeResourceLocked(
    /* [in] */ IThemeConfig* t,
    /* [in] */ Boolean isDiff)
{
    if (isDiff) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        String json;
        t->ToJson(&json);
        Boolean res;
        Settings::Secure::PutStringForUser(resolver,
            IConfiguration::THEME_PKG_CONFIGURATION_PERSISTENCE_PROPERTY, json,
            UserHandle::USER_CURRENT, &res);
    }
}

// =========================================================
// LIFETIME MANAGEMENT
// =========================================================

// Returns which broadcast queue the app is the current [or imminent] receiver
// on, or 'null' if the app is not an active broadcast recipient.
AutoPtr<BroadcastQueue> CActivityManagerService::IsReceivingBroadcast(
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<BroadcastRecord> r = app->mCurReceiver;
    if (r != NULL) {
        return r->mQueue;
    }

    // It's not the current receiver, but it might be starting up to become one
    {
        // xihao: lock isn't needed
        // AutoLock lock(this);
        Int32 size = mBroadcastQueues->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<BroadcastQueue> queue = (*mBroadcastQueues)[i];
            assert(queue != NULL);

            r = queue->mPendingBroadcast;
            if (r != NULL && r->mCurApp.Get() == app) {
                // found it; report which queue it's in
                return queue;
            }
        }
    }

    return NULL;
}

Int32 CActivityManagerService::ComputeOomAdjLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 cachedAdj,
    /* [in] */ ProcessRecord* TOP_APP,
    /* [in] */ Boolean doingAll,
    /* [in] */ Int64 now)
{
    assert(app != NULL && "CActivityManagerService::ComputeOomAdjLocked: app can not be NULL.");

    if (mAdjSeq == app->mAdjSeq) {
        // This adjustment has already been computed.
        return app->mCurRawAdj;
    }

    if (app->mThread == NULL) {
        app->mAdjSeq = mAdjSeq;
        app->mCurSchedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mCurProcState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
        app->mCurAdj = app->mCurRawAdj = ProcessList::CACHED_APP_MAX_ADJ;
        return app->mCurAdj;
    }

    app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo::REASON_UNKNOWN;
    app->mAdjSource = NULL;
    app->mAdjTarget = NULL;
    app->mEmpty = FALSE;
    app->mCached = FALSE;

    Int32 activitiesSize = app->mActivities.GetSize();

    if (app->mMaxAdj <= ProcessList::FOREGROUND_APP_ADJ) {
        // The max adjustment doesn't allow this app to be anything
        // below foreground, so it is not worth doing work for it.
        app->mAdjType = "fixed";
        app->mAdjSeq = mAdjSeq;
        app->mCurRawAdj = app->mMaxAdj;
        app->mForegroundActivities = FALSE;
        app->mCurSchedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mCurProcState = IActivityManager::PROCESS_STATE_PERSISTENT;
        // System processes can do UI, and when they do we want to have
        // them trim their memory after the user leaves the UI.  To
        // facilitate this, here we need to determine whether or not it
        // is currently showing UI.
        app->mSystemNoUi = TRUE;
        if (app == TOP_APP) {
            app->mSystemNoUi = FALSE;
        }
        else if (activitiesSize > 0) {
            List<AutoPtr<ActivityRecord> >::Iterator it = app->mActivities.Begin();
            for (; it != app->mActivities.End(); ++it) {
                AutoPtr<ActivityRecord> r = *it;
                if (r->mVisible) {
                    app->mSystemNoUi = FALSE;
                }
            }
        }
        if (!app->mSystemNoUi) {
            app->mCurProcState = IActivityManager::PROCESS_STATE_PERSISTENT_UI;
        }
        app->mCurAdj = app->mMaxAdj;
        return app->mCurAdj;
    }

    app->mSystemNoUi = FALSE;

    // Determine the importance of the process, starting with most
    // important to least, and assign an appropriate OOM adjustment.
    Int32 adj;
    Int32 schedGroup;
    Int32 procState;
    Boolean foregroundActivities = FALSE;
    AutoPtr<BroadcastQueue> queue;

    if (app == TOP_APP) {
        // The last app on the list is the foreground app.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mAdjType = "top-activity";
        foregroundActivities = TRUE;
        procState = IActivityManager::PROCESS_STATE_TOP;
    }
    else if (app->mInstrumentationClass != NULL) {
        // Don't want to kill running instrumentation.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mAdjType = "instrumentation";
        procState = IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
    }
    else if ((queue = IsReceivingBroadcast(app)) != NULL) {
        // An app that is currently receiving a broadcast also
        // counts as being in the foreground for OOM killer purposes.
        // It's placed in a sched group based on the nature of the
        // broadcast as reflected by which queue it's active in.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = (queue == mFgBroadcastQueue)
                ? IProcess::THREAD_GROUP_DEFAULT : IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mAdjType = "broadcast";
        procState = IActivityManager::PROCESS_STATE_RECEIVER;
    }
    else if (app->mExecutingServices.IsEmpty() == FALSE) {
        // An app that is currently executing a service callback also
        // counts as being in the foreground.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = app->mExecServicesFg ?
                IProcess::THREAD_GROUP_DEFAULT : IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mAdjType = "exec-service";
        procState = IActivityManager::PROCESS_STATE_SERVICE;
    }
    else {
        // As far as we know the process is empty.  We may change our mind later.
        schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        // Assume process is hidden (has activities); we will correct
        // later if this is not the case.
        adj = cachedAdj;
        procState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
        app->mCached = TRUE;
        app->mEmpty = TRUE;
        app->mAdjType = "cch-empty";
    }

    // Examine all activities if not already foreground.
    if (!foregroundActivities && activitiesSize > 0) {
        List<AutoPtr<ActivityRecord> >::Iterator it = app->mActivities.Begin();
        for (; it != app->mActivities.End(); ++it) {
            AutoPtr<ActivityRecord> r = *it;
            if (r->mApp.Get() != app) {
                Slogger::W(TAG, "Wtf, activity %s in proc activity list not using proc %s?!?",
                    TO_CSTR(r), TO_CSTR(app));
                continue;
            }
            if (r->mVisible) {
                // App has a visible activity; only upgrade adjustment.
                if (adj > ProcessList::VISIBLE_APP_ADJ) {
                    adj = ProcessList::VISIBLE_APP_ADJ;
                    app->mAdjType = "visible";
                }
                if (procState > IActivityManager::PROCESS_STATE_TOP) {
                    procState = IActivityManager::PROCESS_STATE_TOP;
                }
                schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                app->mCached = FALSE;
                app->mEmpty = TRUE;
                foregroundActivities = TRUE;
                break;
            }
            else if (r->mState == ActivityState_PAUSING
                    || r->mState == ActivityState_PAUSED) {
                if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                    adj = ProcessList::PERCEPTIBLE_APP_ADJ;
                    app->mAdjType = "pausing";
                }
                if (procState > IActivityManager::PROCESS_STATE_TOP) {
                    procState = IActivityManager::PROCESS_STATE_TOP;
                }
                schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                app->mCached = FALSE;
                app->mEmpty = FALSE;
                foregroundActivities = TRUE;
            }
            else if (r->mState == ActivityState_STOPPING) {
                if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                    adj = ProcessList::PERCEPTIBLE_APP_ADJ;
                    app->mAdjType = "stopping";
                }
                // For the process state, we will at this point consider the
                // process to be cached.  It will be cached either as an activity
                // or empty depending on whether the activity is finishing.  We do
                // this so that we can treat the process as cached for purposes of
                // memory trimming (determing current memory level, trim command to
                // send to process) since there can be an arbitrary number of stopping
                // processes and they should soon all go into the cached state.
                if (!r->mFinishing) {
                    if (procState > IActivityManager::PROCESS_STATE_LAST_ACTIVITY) {
                        procState = IActivityManager::PROCESS_STATE_LAST_ACTIVITY;
                    }
                }
                app->mCached = FALSE;
                app->mEmpty = FALSE;
                foregroundActivities = TRUE;
            }
            else {
                if (procState > IActivityManager::PROCESS_STATE_CACHED_ACTIVITY) {
                    procState = IActivityManager::PROCESS_STATE_CACHED_ACTIVITY;
                    app->mAdjType = "cch-act";
                }
            }
        }
    }

    if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
        if (app->mForegroundServices) {
            // The user is aware of this app, so make it visible.
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
            procState = IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
            app->mCached = FALSE;
            app->mAdjType = "fg-service";
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
        else if (app->mForcingToForeground != NULL) {
            // The user is aware of this app, so make it visible.
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
            procState = IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
            app->mCached = FALSE;
            app->mAdjType = "force-fg";
            app->mAdjSource = app->mForcingToForeground;
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
    }

    if (app == mHeavyWeightProcess) {
        if (adj > ProcessList::HEAVY_WEIGHT_APP_ADJ) {
            // We don't want to kill the current heavy-weight process.
            adj = ProcessList::HEAVY_WEIGHT_APP_ADJ;
            schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
            app->mCached = FALSE;
            app->mAdjType = "heavy";
        }
        if (procState > IActivityManager::PROCESS_STATE_HEAVY_WEIGHT) {
            procState = IActivityManager::PROCESS_STATE_HEAVY_WEIGHT;
        }
    }

    if (app == mHomeProcess) {
        if (adj > ProcessList::HOME_APP_ADJ) {
            // This process is hosting what we currently consider to be the
            // home app, so we don't want to let it go into the background.
            adj = ProcessList::HOME_APP_ADJ;
            schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
            app->mCached = FALSE;
            app->mAdjType = "home";
        }
        if (procState > IActivityManager::PROCESS_STATE_HOME) {
            procState = IActivityManager::PROCESS_STATE_HOME;
        }
    }

    if (app == mPreviousProcess && app->mActivities.GetSize() > 0) {
        if (adj > ProcessList::PREVIOUS_APP_ADJ) {
            // This was the previous process that showed UI to the user.
            // We want to try to keep it around more aggressively, to give
            // a good experience around switching between two apps.
            adj = ProcessList::PREVIOUS_APP_ADJ;
            schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
            app->mCached = FALSE;
            app->mAdjType = "previous";
        }
        if (procState > IActivityManager::PROCESS_STATE_LAST_ACTIVITY) {
            procState = IActivityManager::PROCESS_STATE_LAST_ACTIVITY;
        }
    }

    if (FALSE) Slogger::I(TAG, "OOM %s: initial adj=%d reason=%s",
            app->ToString().string(), adj, app->mAdjType.string());

    // By default, we use the computed adjustment.  It may be changed if
    // there are applications dependent on our services or providers, but
    // this gives us a baseline and makes sure we don't get into an
    // infinite recursion.
    app->mAdjSeq = mAdjSeq;
    app->mCurRawAdj = adj;
    app->mHasStartedServices = FALSE;

    if (mBackupTarget != NULL && app == mBackupTarget->mApp) {
        // If possible we want to avoid killing apps while they're being backed up
        if (adj > ProcessList::BACKUP_APP_ADJ) {
            if (DEBUG_BACKUP) {
                String appDes = app->ToString();
                Slogger::V(TAG, "oom BACKUP_APP_ADJ for %s", appDes.string());
            }
            if (procState > IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND) {
                procState = IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND;
            }
            adj = ProcessList::BACKUP_APP_ADJ;
            app->mAdjType = "backup";
            app->mCached = FALSE;
        }
        if (procState > IActivityManager::PROCESS_STATE_BACKUP) {
            procState = IActivityManager::PROCESS_STATE_BACKUP;
        }
    }

    Boolean mayBeTop = FALSE;

    HashSet<AutoPtr<CServiceRecord> >::Iterator sit = app->mServices.Begin();
    for (; sit != app->mServices.End() && (adj > ProcessList::FOREGROUND_APP_ADJ
        || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE
        || procState > IActivityManager::PROCESS_STATE_TOP); ++sit) {
        AutoPtr<CServiceRecord> s = *sit;
        if (s->mStartRequested) {
            app->mHasStartedServices = TRUE;
            if (procState > IActivityManager::PROCESS_STATE_SERVICE) {
                procState = IActivityManager::PROCESS_STATE_SERVICE;
            }
            if (app->mHasShownUi && app != mHomeProcess) {
                // If this process has shown some UI, let it immediately
                // go to the LRU list because it may be pretty heavy with
                // UI stuff.  We'll tag it with a label just to help
                // debug and understand what is going on.
                if (adj > ProcessList::SERVICE_ADJ) {
                    app->mAdjType = "cch-started-ui-services";
                }
            }
            else {
                if (now < (s->mLastActivity + ActiveServices::MAX_SERVICE_INACTIVITY)) {
                    // This service has seen some activity within
                    // recent memory, so we will keep its process ahead
                    // of the background processes.
                    if (adj > ProcessList::SERVICE_ADJ) {
                        adj = ProcessList::SERVICE_ADJ;
                        app->mAdjType = "started-services";
                        app->mCached = FALSE;
                    }
                }
                // If we have let the service slide into the background
                // state, still have some text describing what it is doing
                // even though the service no longer has an impact.
                if (adj > ProcessList::SERVICE_ADJ) {
                    app->mAdjType = "cch-started-services";
                }
            }
        }

        CServiceRecord::ConnectionIterator connit = s->mConnections.Begin();
        for (; connit != s->mConnections.End() && (adj > ProcessList::FOREGROUND_APP_ADJ
            || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE
            || procState > IActivityManager::PROCESS_STATE_TOP); ++connit) {
            AutoPtr<List<AutoPtr<ConnectionRecord> > > clist = connit->mSecond;
            List<AutoPtr<ConnectionRecord> >::Iterator it;
            for (it = clist->Begin(); it != clist->End() && (adj > ProcessList::FOREGROUND_APP_ADJ
                || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE
                || procState > IActivityManager::PROCESS_STATE_TOP); ++it) {
                // XXX should compute this based on the max of
                // all connected clients.
                AutoPtr<ConnectionRecord> cr = *it;
                if (cr->mBinding->mClient == app) {
                    // Binding to ourself is not interesting.
                    continue;
                }
                if ((cr->mFlags & IContext::BIND_WAIVE_PRIORITY) == 0) {
                    AutoPtr<ProcessRecord> client = cr->mBinding->mClient;
                    Int32 clientAdj = ComputeOomAdjLocked(client, cachedAdj,
                            TOP_APP, doingAll, now);
                    Int32 clientProcState = client->mCurProcState;
                    if (clientProcState >= IActivityManager::PROCESS_STATE_CACHED_ACTIVITY) {
                        // If the other app is cached for any reason, for purposes here
                        // we are going to consider it empty.  The specific cached state
                        // doesn't propagate except under certain conditions.
                        clientProcState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
                    }
                    String adjType;
                    if ((cr->mFlags & IContext::BIND_ALLOW_OOM_MANAGEMENT) != 0) {
                        // Not doing bind OOM management, so treat
                        // this guy more like a started service.
                        if (app->mHasShownUi && app != mHomeProcess) {
                            // If this process has shown some UI, let it immediately
                            // go to the LRU list because it may be pretty heavy with
                            // UI stuff.  We'll tag it with a label just to help
                            // debug and understand what is going on.
                            if (adj > clientAdj) {
                                adjType = "cch-bound-ui-services";
                            }
                            app->mCached = FALSE;
                            clientAdj = adj;
                            clientProcState = procState;
                        }
                        else {
                            if (now >= (s->mLastActivity
                                    + ActiveServices::MAX_SERVICE_INACTIVITY)) {
                                // This service has not seen activity within
                                // recent memory, so allow it to drop to the
                                // LRU list if there is no other reason to keep
                                // it around.  We'll also tag it with a label just
                                // to help debug and undertand what is going on.
                                if (adj > clientAdj) {
                                    adjType = "cch-bound-services";
                                }
                                clientAdj = adj;
                            }
                        }
                    }
                    if (adj > clientAdj) {
                        // If this process has recently shown UI, and
                        // the process that is binding to it is less
                        // important than being visible, then we don't
                        // care about the binding as much as we care
                        // about letting this process get into the LRU
                        // list to be killed and restarted if needed for
                        // memory.
                        if (app->mHasShownUi && app != mHomeProcess
                                && clientAdj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                            adjType = "cch-bound-ui-services";
                        }
                        else {
                            if ((cr->mFlags & (IContext::BIND_ABOVE_CLIENT
                                    |IContext::BIND_IMPORTANT)) != 0) {
                                adj = clientAdj >= ProcessList::PERSISTENT_SERVICE_ADJ
                                        ? clientAdj : ProcessList::PERSISTENT_SERVICE_ADJ;
                            }
                            else if ((cr->mFlags & IContext::BIND_NOT_VISIBLE) != 0
                                    && clientAdj < ProcessList::PERCEPTIBLE_APP_ADJ
                                    && adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                                adj = ProcessList::PERCEPTIBLE_APP_ADJ;
                            }
                            else if (clientAdj > ProcessList::VISIBLE_APP_ADJ) {
                                adj = clientAdj;
                            }
                            else {
                                if (adj > ProcessList::VISIBLE_APP_ADJ) {
                                    adj = ProcessList::VISIBLE_APP_ADJ;
                                }
                            }
                            if (!client->mCached) {
                                app->mCached = FALSE;
                            }
                            adjType = "service";
                        }
                    }
                    if ((cr->mFlags & IContext::BIND_NOT_FOREGROUND) == 0) {
                        if (client->mCurSchedGroup == IProcess::THREAD_GROUP_DEFAULT) {
                            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                        }
                        if (clientProcState <= IActivityManager::PROCESS_STATE_TOP) {
                            if (clientProcState == IActivityManager::PROCESS_STATE_TOP) {
                                // Special handling of clients who are in the top state.
                                // We *may* want to consider this process to be in the
                                // top state as well, but only if there is not another
                                // reason for it to be running.  Being on the top is a
                                // special state, meaning you are specifically running
                                // for the current top app->m  If the process is already
                                // running in the background for some other reason, it
                                // is more important to continue considering it to be
                                // in the background state.
                                mayBeTop = TRUE;
                                clientProcState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
                            }
                            else {
                                // Special handling for above-top states (persistent
                                // processes).  These should not bring the current process
                                // into the top state, since they are not on top.  Instead
                                // give them the best state after that.
                                clientProcState =
                                        IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
                            }
                        }
                    }
                    else {
                        if (clientProcState <
                                IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND) {
                            clientProcState =
                                    IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND;
                        }
                    }
                    if (procState > clientProcState) {
                        procState = clientProcState;
                    }
                    if (procState < IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND
                            && (cr->mFlags & IContext::BIND_SHOWING_UI) != 0) {
                        app->mPendingUiClean = TRUE;
                    }
                    if (adjType != NULL) {
                        app->mAdjType = adjType;
                        app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo
                                ::REASON_SERVICE_IN_USE;
                        app->mAdjSource = cr->mBinding->mClient->Probe(EIID_IInterface);
                        app->mAdjSourceProcState = clientProcState;
                        app->mAdjTarget = s->mName;
                    }
                }
                if ((cr->mFlags & IContext::BIND_TREAT_LIKE_ACTIVITY) != 0) {
                    app->mTreatLikeActivity = TRUE;
                }
                AutoPtr<ActivityRecord> a = cr->mActivity;
                if ((cr->mFlags & IContext::BIND_ADJUST_WITH_ACTIVITY) != 0) {
                    if (a != NULL && adj > ProcessList::FOREGROUND_APP_ADJ &&
                        (a->mVisible || a->mState == ActivityState_RESUMED
                         || a->mState == ActivityState_PAUSING)) {
                        adj = ProcessList::FOREGROUND_APP_ADJ;
                        if ((cr->mFlags & IContext::BIND_NOT_FOREGROUND) == 0) {
                            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                        }
                        app->mCached = FALSE;
                        app->mAdjType = "service";
                        app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo
                                ::REASON_SERVICE_IN_USE;
                        app->mAdjSource = a->Probe(EIID_IInterface);
                        app->mAdjSourceProcState = procState;
                        app->mAdjTarget = s->mName;
                    }
                }
            }
        }
    }

    HashMap<String, AutoPtr<ContentProviderRecord> >::Iterator provIt;
    for (provIt = app->mPubProviders.Begin(); provIt != app->mPubProviders.End()
            && (adj > ProcessList::FOREGROUND_APP_ADJ
            || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE
            || procState > IActivityManager::PROCESS_STATE_TOP); ++provIt) {
        AutoPtr<ContentProviderRecord> cpr = provIt->mSecond;
        List<AutoPtr<CContentProviderConnection> >::Iterator it;
        for (it = cpr->mConnections.Begin(); it != cpr->mConnections.End()
                && (adj > ProcessList::FOREGROUND_APP_ADJ
                || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE
                || procState > IActivityManager::PROCESS_STATE_TOP); ++it) {
            AutoPtr<CContentProviderConnection> conn = *it;
            AutoPtr<ProcessRecord> client = conn->mClient;
            if (client.Get() == app) {
                // Being our own client is not interesting.
                continue;
            }
            Int32 clientAdj = ComputeOomAdjLocked(client, cachedAdj, TOP_APP, doingAll, now);
            Int32 clientProcState = client->mCurProcState;
            if (clientProcState >= IActivityManager::PROCESS_STATE_CACHED_ACTIVITY) {
                // If the other app is cached for any reason, for purposes here
                // we are going to consider it empty.
                clientProcState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
            }
            if (adj > clientAdj) {
                if (app->mHasShownUi && app != mHomeProcess
                        && clientAdj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                    app->mAdjType = "cch-ui-provider";
                }
                else {
                    adj = clientAdj > ProcessList::FOREGROUND_APP_ADJ
                            ? clientAdj : ProcessList::FOREGROUND_APP_ADJ;
                    app->mAdjType = "provider";
                }
                app->mCached &= client->mCached;
                app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo
                        ::REASON_PROVIDER_IN_USE;
                app->mAdjSource = client->Probe(EIID_IInterface);
                app->mAdjSourceProcState = clientProcState;
                app->mAdjTarget = cpr->mName;
            }
            if (clientProcState <= IActivityManager::PROCESS_STATE_TOP) {
                if (clientProcState == IActivityManager::PROCESS_STATE_TOP) {
                    // Special handling of clients who are in the top state.
                    // We *may* want to consider this process to be in the
                    // top state as well, but only if there is not another
                    // reason for it to be running.  Being on the top is a
                    // special state, meaning you are specifically running
                    // for the current top app->m  If the process is already
                    // running in the background for some other reason, it
                    // is more important to continue considering it to be
                    // in the background state.
                    mayBeTop = TRUE;
                    clientProcState = IActivityManager::PROCESS_STATE_CACHED_EMPTY;
                }
                else {
                    // Special handling for above-top states (persistent
                    // processes).  These should not bring the current process
                    // into the top state, since they are not on top.  Instead
                    // give them the best state after that.
                    clientProcState =
                            IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
                }
            }
            if (procState > clientProcState) {
                procState = clientProcState;
            }
            if (client->mCurSchedGroup == IProcess::THREAD_GROUP_DEFAULT) {
                schedGroup = IProcess::THREAD_GROUP_DEFAULT;
            }
        }
        // If the provider has external (non-framework) process
        // dependencies, ensure that its adjustment is at least
        // FOREGROUND_APP_ADJ.
        if (cpr->HasExternalProcessHandles()) {
            if (adj > ProcessList::FOREGROUND_APP_ADJ) {
                adj = ProcessList::FOREGROUND_APP_ADJ;
                schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                app->mCached = FALSE;
                app->mAdjType = "provider";
                app->mAdjTarget = cpr->mName;
            }
            if (procState > IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND) {
                procState = IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
            }
        }
    }

    if (mayBeTop && procState > IActivityManager::PROCESS_STATE_TOP) {
        // A client of one of our services or providers is in the top state.  We
        // *may* want to be in the top state, but not if we are already running in
        // the background for some other reason.  For the decision here, we are going
        // to pick out a few specific states that we want to remain in when a client
        // is top (states that tend to be longer-term) and otherwise allow it to go
        // to the top state.
        switch (procState) {
            case IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND:
            case IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND:
            case IActivityManager::PROCESS_STATE_SERVICE:
                // These all are longer-term states, so pull them up to the top
                // of the background states, but not all the way to the top state.
                procState = IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND;
                break;
            default:
                // Otherwise, top is a better choice, so take it.
                procState = IActivityManager::PROCESS_STATE_TOP;
                break;
        }
    }

    if (procState >= IActivityManager::PROCESS_STATE_CACHED_EMPTY) {
        if (app->mHasClientActivities) {
            // This is a cached process, but with client activities.  Mark it so.
            procState = IActivityManager::PROCESS_STATE_CACHED_ACTIVITY_CLIENT;
            app->mAdjType = "cch-client-act";
        }
        else if (app->mTreatLikeActivity) {
            // This is a cached process, but somebody wants us to treat it like it has
            // an activity, okay!
            procState = IActivityManager::PROCESS_STATE_CACHED_ACTIVITY;
            app->mAdjType = "cch-as-act";
        }
    }

    if (adj == ProcessList::SERVICE_ADJ) {
        if (doingAll) {
            app->mServiceb = mNewNumAServiceProcs > (mNumServiceProcs/3);
            mNewNumServiceProcs++;
            //Slog.i(TAG, "ADJ " + app + " serviceb=" + app->mServiceb);
            if (!app->mServiceb) {
                // This service isn't far enough down on the LRU list to
                // normally be a B service, but if we are low on RAM and it
                // is large we want to force it down since we would prefer to
                // keep launcher over it.
                if (mLastMemoryLevel > IProcessStats::ADJ_MEM_FACTOR_NORMAL
                        && app->mLastPss >= mProcessList->GetCachedRestoreThresholdKb()) {
                    app->mServiceHighRam = TRUE;
                    app->mServiceb = TRUE;
                    //Slog.i(TAG, "ADJ " + app + " high ram!");
                }
                else {
                    mNewNumAServiceProcs++;
                    //Slog.i(TAG, "ADJ " + app + " not high ram!");
                }
            }
            else {
                app->mServiceHighRam = FALSE;
            }
        }
        if (app->mServiceb) {
            adj = ProcessList::SERVICE_B_ADJ;
        }
    }

    app->mCurRawAdj = adj;

    //Slog.i(TAG, "OOM ADJ " + app + ": pid=" + app->mPid +
    //      " adj=" + adj + " curAdj=" + app->mCurAdj + " maxAdj=" + app->mMaxAdj);
    if (adj > app->mMaxAdj) {
        adj = app->mMaxAdj;
        if (app->mMaxAdj <= ProcessList::PERCEPTIBLE_APP_ADJ) {
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
    }

    // Do final modification to adj.  Everything we do between here and applying
    // the final setAdj must be done in this function, because we will also use
    // it when computing the final cached adj later.  Note that we don't need to
    // worry about this for max adj above, since max adj will always be used to
    // keep it out of the cached vaues.
    app->mCurAdj = app->ModifyRawOomAdj(adj);
    app->mCurSchedGroup = schedGroup;
    app->mCurProcState = procState;
    app->mForegroundActivities = foregroundActivities;

    return app->mCurRawAdj;
}

/**
 * Schedule PSS collection of a process.
 */
ECode CActivityManagerService::RequestPssLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ Int32 procState)
{
    if (Find(mPendingPssProcesses.Begin(), mPendingPssProcesses.End(),
        AutoPtr<ProcessRecord>(proc)) != mPendingPssProcesses.End()) {
        return NOERROR;
    }
    if (mPendingPssProcesses.GetSize() == 0) {
        Boolean res;
        mBgHandler->SendEmptyMessage(COLLECT_PSS_BG_MSG, &res);
    }
    if (DEBUG_PSS) Slogger::D(TAG, "Requesting PSS of: %s", TO_CSTR(proc));
    proc->mPssProcState = procState;
    mPendingPssProcesses.PushBack(proc);
    return NOERROR;
}

/**
 * Schedule PSS collection of all processes.
 */
ECode CActivityManagerService::RequestPssAllProcsLocked(
    /* [in] */ Int64 now,
    /* [in] */ Boolean always,
    /* [in] */ Boolean memLowered)
{
    if (!always) {
        if (now < (mLastFullPssTime +
            (memLowered ? FULL_PSS_LOWERED_INTERVAL : FULL_PSS_MIN_INTERVAL))) {
            return NOERROR;
        }
    }
    if (DEBUG_PSS) Slogger::D(TAG, "Requesting PSS of all procs!  memLowered=%d", memLowered);
    mLastFullPssTime = now;
    mFullPssPending = TRUE;
    mPendingPssProcesses.Clear();
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> app = *rit;
        if (app->mThread == NULL || app->mCurProcState < 0) {
            continue;
        }
        if (memLowered || now > (app->mLastStateTime + ProcessList::PSS_ALL_INTERVAL)) {
            app->mPssProcState = app->mSetProcState;
            app->mNextPssTime = ProcessList::ComputeNextPssTime(app->mCurProcState, TRUE,
                    IsSleeping(), now);
            mPendingPssProcesses.PushBack(app);
        }
    }
    Boolean res;
    return mBgHandler->SendEmptyMessage(COLLECT_PSS_BG_MSG, &res);
}

/**
 * Ask a given process to GC right now.
 */
ECode CActivityManagerService::PerformAppGcLocked(
    /* [in] */ ProcessRecord* app)
{
    VALIDATE_NOT_NULL(app);

    // try {
    app->mLastRequestedGc = SystemClock::GetUptimeMillis();
    if (app->mThread != NULL) {
        if (app->mReportLowMemory) {
            app->mReportLowMemory = FALSE;
            app->mThread->ScheduleLowMemory();
        }
        else {
            app->mThread->ProcessInBackground();
        }
    }
    // } catch (Exception e) {
    //     // whatever.
    // }
    return NOERROR;
}

Boolean CActivityManagerService::CanGcNowLocked()
{
    Boolean processingBroadcasts = FALSE;
    for (Int32 i = 0; i < mBroadcastQueues->GetLength(); ++i) {
        AutoPtr<BroadcastQueue> q = (*mBroadcastQueues)[i];
        if (q->mParallelBroadcasts.IsEmpty() == FALSE || q->mOrderedBroadcasts.IsEmpty() == FALSE) {
            processingBroadcasts = TRUE;
        }
    }
    return !processingBroadcasts
        && (IsSleeping() || mStackSupervisor->AllResumedActivitiesIdle());
}

ECode CActivityManagerService::PerformAppGcsLocked()
{
    if (mProcessesToGc.IsEmpty()) {
        return NOERROR;
    }
    if (CanGcNowLocked()) {
        while (mProcessesToGc.IsEmpty() == FALSE) {
            List< AutoPtr<ProcessRecord> >::Iterator it = mProcessesToGc.Begin();
            AutoPtr<ProcessRecord> proc = *it;
            mProcessesToGc.Erase(it);
            if (proc->mCurRawAdj > ProcessList::PERCEPTIBLE_APP_ADJ || proc->mReportLowMemory) {
                if ((proc->mLastRequestedGc + GC_MIN_INTERVAL)
                        <= SystemClock::GetUptimeMillis()) {
                    // To avoid spamming the system, we will GC processes one
                    // at a time, waiting a few seconds between each.
                    PerformAppGcLocked(proc);
                    ScheduleAppGcsLocked();
                    return NOERROR;
                }
                else {
                    // It hasn't been long enough since we last GCed this
                    // process...  put it in the list to wait for its time.
                    AddProcessToGcListLocked(proc);
                    break;
                }
           }
        }

        ScheduleAppGcsLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::PerformAppGcsIfAppropriateLocked()
{
    if (CanGcNowLocked()) {
        PerformAppGcsLocked();
        return NOERROR;
    }
    // Still not idle, wait some more.
    ScheduleAppGcsLocked();
    return NOERROR;
}

ECode CActivityManagerService::ScheduleAppGcsLocked()
{
    mHandler->RemoveMessages(GC_BACKGROUND_PROCESSES_MSG);

    if (!mProcessesToGc.IsEmpty()) {
        // Schedule a GC for the time to the next process.
        List< AutoPtr<ProcessRecord> >::Iterator it = mProcessesToGc.Begin();
        AutoPtr<ProcessRecord> proc = *it;

        Int64 when = proc->mLastRequestedGc + GC_MIN_INTERVAL;
        Int64 now = SystemClock::GetUptimeMillis();
        if (when < (now+GC_TIMEOUT)) {
            when = now + GC_TIMEOUT;
        }

        Boolean result;
        mHandler->SendEmptyMessageAtTime(GC_BACKGROUND_PROCESSES_MSG, when, &result);
    }
    return NOERROR;
}

/**
 * Add a process to the array of processes waiting to be GCed.  Keeps the
 * list in sorted order by the last GC time.  The process can't already be
 * on the list.
 */
ECode CActivityManagerService::AddProcessToGcListLocked(
    /* [in] */ ProcessRecord* proc)
{
    Boolean added = FALSE;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mProcessesToGc.RBegin();
    for (; rit != mProcessesToGc.REnd(); ++rit) {
        if ((*rit)->mLastRequestedGc < proc->mLastRequestedGc) {
            added = TRUE;
            mProcessesToGc.Insert(rit.GetBase(), proc);
            break;
        }
    }
    if (!added) {
        mProcessesToGc.PushFront(proc);
    }
    return NOERROR;
}

/**
 * Set up to ask a process to GC itself.  This will either do it
 * immediately, or put it on the list of processes to gc the next
 * time things are idle.
 */
ECode CActivityManagerService::ScheduleAppGcLocked(
    /* [in] */ ProcessRecord* app)
{
    Int64 now = SystemClock::GetUptimeMillis();
    if ((app->mLastRequestedGc + GC_MIN_INTERVAL) > now) {
        return NOERROR;
    }

    AutoPtr<ProcessRecord> aa = app;
    if (Find(mProcessesToGc.Begin(), mProcessesToGc.End(), aa)
            == mProcessesToGc.End()) {
        AddProcessToGcListLocked(app);
        ScheduleAppGcsLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::CheckExcessivePowerUsageLocked(
    /* [in] */ Boolean doKills)
{
    UpdateCpuStatsNow();

    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();

    Boolean doWakeKills = doKills;
    Boolean doCpuKills = doKills;
    if (mLastPowerCheckRealtime == 0) {
        doWakeKills = FALSE;
    }
    if (mLastPowerCheckUptime == 0) {
        doCpuKills = FALSE;
    }

    Boolean isScreenOn;
    stats->IsScreenOn(&isScreenOn);
    if (isScreenOn) {
        doWakeKills = FALSE;
    }
    const Int64 curRealtime = SystemClock::GetElapsedRealtime();
    const Int64 realtimeSince = curRealtime - mLastPowerCheckRealtime;
    const Int64 curUptime = SystemClock::GetUptimeMillis();
    const Int64 uptimeSince = curUptime - mLastPowerCheckUptime;
    mLastPowerCheckRealtime = curRealtime;
    mLastPowerCheckUptime = curUptime;
    if (realtimeSince < WAKE_LOCK_MIN_CHECK_DURATION) {
        doWakeKills = FALSE;
    }
    if (uptimeSince < CPU_MIN_CHECK_DURATION) {
        doCpuKills = FALSE;
    }
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> app = *rit;
        if (app->mSetProcState >= IActivityManager::PROCESS_STATE_HOME) {
            Int64 wtime;
            {
                AutoLock lock(stats);
                Int32 uid;
                app->mInfo->GetUid(&uid);
                stats->GetProcessWakeTime(uid, app->mPid, curRealtime, &wtime);
            }
            Int64 wtimeUsed = wtime - app->mLastWakeTime;
            Int64 cputimeUsed = app->mCurCpuTime - app->mLastCpuTime;
            if (DEBUG_POWER) {
                StringBuilder sb(128);
                sb.Append("Wake for ");
                app->ToShortString(sb);
                sb.Append(": over ");
                TimeUtils::FormatDuration(realtimeSince, sb);
                sb.Append(" used ");
                TimeUtils::FormatDuration(wtimeUsed, sb);
                sb.Append(" (");
                sb.Append((wtimeUsed*100)/realtimeSince);
                sb.Append("%)");
                Slogger::I(TAG, sb.ToString());
                // sb.SetLength(0);
                sb.Append("CPU for ");
                app->ToShortString(sb);
                sb.Append(": over ");
                TimeUtils::FormatDuration(uptimeSince, sb);
                sb.Append(" used ");
                TimeUtils::FormatDuration(cputimeUsed, sb);
                sb.Append(" (");
                sb.Append((cputimeUsed*100)/uptimeSince);
                sb.Append("%)");
                Slogger::I(TAG, sb.ToString());
            }
            // If a process has held a wake lock for more
            // than 50% of the time during this period,
            // that sounds bad.  Kill!
            if (doWakeKills && realtimeSince > 0
                    && ((wtimeUsed*100)/realtimeSince) >= 50) {
                {
                    AutoLock lock(stats);
                    Int32 uid;
                    app->mInfo->GetUid(&uid);
                    stats->ReportExcessiveWakeLocked(uid, app->mProcessName,
                            realtimeSince, wtimeUsed);
                }
                StringBuilder sb("excessive wake held ");
                sb += wtimeUsed;
                sb += " during ";
                sb += realtimeSince;
                app->Kill(sb.ToString(), TRUE);
                app->mBaseProcessTracker->ReportExcessiveWake(app->mPkgList);
            }
            else if (doCpuKills && uptimeSince > 0
                    && ((cputimeUsed*100)/uptimeSince) >= 50) {
                {
                    AutoLock lock(stats);
                    Int32 uid;
                    app->mInfo->GetUid(&uid);
                    stats->ReportExcessiveCpuLocked(uid, app->mProcessName,
                            uptimeSince, cputimeUsed);
                }
                StringBuilder sb("excessive cpu ");
                sb += cputimeUsed;
                sb += " during ";
                sb += uptimeSince;
                app->Kill(sb.ToString(), TRUE);
                app->mBaseProcessTracker->ReportExcessiveCpu(app->mPkgList);
            }
            else {
                app->mLastWakeTime = wtime;
                app->mLastCpuTime = app->mCurCpuTime;
            }
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::ApplyOomAdjLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ ProcessRecord* TOP_APP,
    /* [in] */ Boolean doingAll,
    /* [in] */ Int64 now)
{
    Boolean success = TRUE;

    if (app->mCurRawAdj != app->mSetRawAdj) {
        app->mSetRawAdj = app->mCurRawAdj;
    }

    Int32 changes = 0;
    Int32 uid;
    app->mInfo->GetUid(&uid);
    if (app->mCurAdj != app->mSetAdj) {
        ProcessList::SetOomAdj(app->mPid, uid, app->mCurAdj);
        if (DEBUG_SWITCH || DEBUG_OOM_ADJ) {
            Slogger::V(TAG, "Set %d %s adj %d: %s",
                    app->mPid, app->mProcessName.string(),
                    app->mCurAdj, app->mAdjType.string());
        }
        app->mSetAdj = app->mCurAdj;
    }

    if (app->mSetSchedGroup != app->mCurSchedGroup) {
        app->mSetSchedGroup = app->mCurSchedGroup;
        if (DEBUG_SWITCH || DEBUG_OOM_ADJ) {
            Slogger::V(TAG, "Setting process group of %s to %d",
                    app->mProcessName.string(), app->mCurSchedGroup);
        }
        if (!app->mWaitingToKill.IsNull() &&
                app->mSetSchedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE) {
            app->Kill(app->mWaitingToKill, TRUE);
            success = FALSE;
        }
        else {
            if (TRUE) {
                const Int64 oldId = Binder::ClearCallingIdentity();
                if (FAILED(Process::SetProcessGroup(app->mPid, app->mCurSchedGroup))) {
                    Slogger::W(TAG, "Failed setting process group of %d to %d",
                            app->mPid, app->mCurSchedGroup);
                }
                Binder::RestoreCallingIdentity(oldId);
            }
            else {
                if (app->mThread != NULL) {
//                try {
                    app->mThread->SetSchedulingGroup(app->mCurSchedGroup);
//                } catch (RemoteException e) {
//                }
                }
            }
             Process::SetSwappiness(app->mPid,
                app->mCurSchedGroup <= IProcess::THREAD_GROUP_BG_NONINTERACTIVE);
        }
    }

    if (app->mRepForegroundActivities != app->mForegroundActivities) {
        app->mRepForegroundActivities = app->mForegroundActivities;
        changes |= ProcessChangeItem::CHANGE_ACTIVITIES;
    }
    if (app->mRepProcState != app->mCurProcState) {
        app->mRepProcState = app->mCurProcState;
        changes |= ProcessChangeItem::CHANGE_PROCESS_STATE;
        if (app->mThread != NULL) {
            if (FALSE) {
                //RuntimeException h = new RuntimeException("here");
                Slogger::I(TAG, "Sending new process state %d to %s",
                    app->mRepProcState, TO_CSTR(app));
            }
            app->mThread->SetProcessState(app->mRepProcState);
        }
    }
    if (app->mSetProcState < 0 || ProcessList::ProcStatesDifferForMem(app->mCurProcState,
            app->mSetProcState)) {
        app->mLastStateTime = now;
        app->mNextPssTime = ProcessList::ComputeNextPssTime(app->mCurProcState, TRUE,
                IsSleeping(), now);
        if (DEBUG_PSS) Slogger::D(TAG, "Process state change from %s to %s next pss in %lld: %s",
                ProcessList::MakeProcStateString(app->mSetProcState).string(),
                ProcessList::MakeProcStateString(app->mCurProcState).string(),
                (app->mNextPssTime-now), TO_CSTR(app));
    }
    else {
        if (now > app->mNextPssTime || (now > (app->mLastPssTime + ProcessList::PSS_MAX_INTERVAL)
                && now > (app->mLastStateTime+ProcessList::PSS_MIN_TIME_FROM_STATE_CHANGE))) {
            RequestPssLocked(app, app->mSetProcState);
            app->mNextPssTime = ProcessList::ComputeNextPssTime(app->mCurProcState, FALSE,
                    IsSleeping(), now);
        }
        else if (FALSE && DEBUG_PSS) {
            Slogger::D(TAG, "Not requesting PSS of %s: next=%lld", TO_CSTR(app), (app->mNextPssTime-now));
        }
    }
    if (app->mSetProcState != app->mCurProcState) {
        if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                "Proc state change of %s to %d", app->mProcessName.string(),
                app->mCurProcState);
        Boolean setImportant = app->mSetProcState < IActivityManager::PROCESS_STATE_SERVICE;
        Boolean curImportant = app->mCurProcState < IActivityManager::PROCESS_STATE_SERVICE;
        if (setImportant && !curImportant) {
            // This app is no longer something we consider important enough to allow to
            // use arbitrary amounts of battery power.  Note
            // its current wake lock time to later know to kill it if
            // it is not behaving well.
            AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
            {    AutoLock syncLock(stats);
                stats->GetProcessWakeTime(uid,
                    app->mPid, SystemClock::GetElapsedRealtime(), &app->mLastWakeTime);
            }

            app->mLastCpuTime = app->mCurCpuTime;

        }
        app->mSetProcState = app->mCurProcState;
        if (app->mSetProcState >= IActivityManager::PROCESS_STATE_HOME) {
            app->mNotCachedSinceIdle = FALSE;
        }
        if (!doingAll) {
            SetProcessTrackerStateLocked(app, mProcessStats->GetMemFactorLocked(), now);
        }
        else {
            app->mProcStateChanged = TRUE;
        }
    }

    if (changes != 0) {
        if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Changes in %s: %d", TO_CSTR(app), changes);
        AutoPtr<ProcessChangeItem> item;
        List<AutoPtr<ProcessChangeItem> >::ReverseIterator rit = mPendingProcessChanges.RBegin();
        while (rit != mPendingProcessChanges.REnd()) {
            item = *rit;
            if (item->mPid == app->mPid) {
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Re-using existing item: %s", TO_CSTR(item));
                break;
            }
            ++rit;
        }
        if (rit == mPendingProcessChanges.REnd()) {
            // No existing item in pending changes; need a new one.
            Int32 NA = mAvailProcessChanges.GetSize();
            if (NA > 0) {
                item = mAvailProcessChanges.GetBack();
                mAvailProcessChanges.PopBack();
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Retreiving available item: %s", TO_CSTR(item));
            }
            else {
                item = new ProcessChangeItem();
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Allocating new item: %s", TO_CSTR(item));
            }
            item->mChanges = 0;
            item->mPid = app->mPid;
            item->mUid = uid;
            if (mPendingProcessChanges.GetSize() == 0) {
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG,
                        "*** Enqueueing dispatch processes changed!");
                Boolean res;
                mHandler->SendEmptyMessage(DISPATCH_PROCESSES_CHANGED, &res);
            }
            mPendingProcessChanges.PushBack(item);
        }
        item->mChanges |= changes;
        item->mProcessState = app->mRepProcState;
        item->mForegroundActivities = app->mRepForegroundActivities;
        if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG,
            "Item %p %s: changes=%d, procState=%d foreground=%d type=%s source=%s target=%s",
            item.Get(), app->ToShortString().string(), item->mChanges, item->mProcessState,
            item->mForegroundActivities, app->mAdjType.string(),
            TO_CSTR(app->mAdjSource), TO_CSTR(app->mAdjTarget));
    }

    return success;
}

ECode CActivityManagerService::SetProcessTrackerStateLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    if (proc->mThread != NULL) {
        if (proc->mBaseProcessTracker != NULL) {
            proc->mBaseProcessTracker->SetState(proc->mRepProcState, memFactor, now, proc->mPkgList);
        }
        if (proc->mRepProcState >= 0) {
            Int32 uid;
            proc->mInfo->GetUid(&uid);
            mBatteryStatsService->NoteProcessState(proc->mProcessName, uid,
                    proc->mRepProcState);
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::UpdateOomAdjLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 cachedAdj,
    /* [in] */ ProcessRecord* TOP_APP,
    /* [in] */ Boolean doingAll,
    /* [in] */ Int64 now)
{
    if (app->mThread == NULL) {
        return FALSE;
    }

    ComputeOomAdjLocked(app, cachedAdj, TOP_APP, doingAll, now);

    return ApplyOomAdjLocked(app, TOP_APP, doingAll, now);
}

ECode CActivityManagerService::UpdateProcessForegroundLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ Boolean isForeground,
    /* [in] */ Boolean oomAdj)
{
    if (isForeground != proc->mForegroundServices) {
        proc->mForegroundServices = isForeground;
        String packageName;
        IPackageItemInfo::Probe(proc->mInfo)->GetPackageName(&packageName);
        Int32 uid;
        proc->mInfo->GetUid(&uid);
        AutoPtr<List<AutoPtr<ProcessRecord> > > curProcs = mForegroundPackages->Get(packageName, uid);
        if (isForeground) {
            if (curProcs == NULL) {
                curProcs = new List<AutoPtr<ProcessRecord> >();
                mForegroundPackages->Put(packageName, uid, curProcs);
            }
            if (Find(curProcs->Begin(), curProcs->End(), AutoPtr<ProcessRecord>(proc))
                == curProcs->End()) {
                curProcs->PushBack(proc);
                mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_FOREGROUND_START,
                        packageName, uid);
            }
        }
        else {
            if (curProcs != NULL) {
                List<AutoPtr<ProcessRecord> >::Iterator find = Find(
                    curProcs->Begin(), curProcs->End(), AutoPtr<ProcessRecord>(proc));
                if (find != curProcs->End()) {
                    curProcs->Erase(find);
                    mBatteryStatsService->NoteEvent(
                        IBatteryStatsHistoryItem::EVENT_FOREGROUND_FINISH,
                            packageName, uid);
                    if (curProcs->GetSize() <= 0) {
                        mForegroundPackages->Remove(packageName, uid);
                    }
                }
            }
        }
        if (oomAdj) {
            UpdateOomAdjLocked();
        }
    }
    return NOERROR;
}

AutoPtr<ActivityRecord> CActivityManagerService::ResumedAppLocked()
{
    AutoPtr<ActivityRecord> act = mStackSupervisor->ResumedAppLocked();
    String pkg;
    Int32 uid;
    if (act != NULL) {
        pkg = act->mPackageName;
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(act->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        appInfo->GetUid(&uid);
    }
    else {
        pkg = NULL;
        uid = -1;
    }
    // Has the UID or resumed package name changed?
    if (uid != mCurResumedUid || (pkg != mCurResumedPackage
            && (pkg == NULL || !pkg.Equals(mCurResumedPackage)))) {
        if (mCurResumedPackage != NULL) {
            mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_TOP_FINISH,
                    mCurResumedPackage, mCurResumedUid);
        }
        mCurResumedPackage = pkg;
        mCurResumedUid = uid;
        if (mCurResumedPackage != NULL) {
            mBatteryStatsService->NoteEvent(IBatteryStatsHistoryItem::EVENT_TOP_START,
                    mCurResumedPackage, mCurResumedUid);
        }
    }
    return act;
}

Boolean CActivityManagerService::UpdateOomAdjLocked(
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<ActivityRecord> TOP_ACT = ResumedAppLocked();
    AutoPtr<ProcessRecord> TOP_APP = TOP_ACT != NULL ? TOP_ACT->mApp : NULL;
    Boolean wasCached = app->mCached;

    mAdjSeq++;

    // This is the desired cached adjusment we want to tell it to use.
    // If our app is currently cached, we know it, and that is it.  Otherwise,
    // we don't know it yet, and it needs to now be cached we will then
    // need to do a complete oom adj.
    Int32 cachedAdj = app->mCurRawAdj >= ProcessList::CACHED_APP_MIN_ADJ
            ? app->mCurRawAdj : ProcessList::UNKNOWN_ADJ;
    Boolean success = UpdateOomAdjLocked(app, cachedAdj, TOP_APP, FALSE,
            SystemClock::GetUptimeMillis());
    if (wasCached != app->mCached || app->mCurRawAdj == ProcessList::UNKNOWN_ADJ) {
        // Changed to/from cached state, so apps after it in the LRU
        // list may also be changed.
        UpdateOomAdjLocked();
    }
    return success;
}

ECode CActivityManagerService::UpdateOomAdjLocked()
{
    AutoPtr<ActivityRecord> TOP_ACT = ResumedAppLocked();
    AutoPtr<ProcessRecord> TOP_APP = TOP_ACT != NULL ? TOP_ACT->mApp : NULL;
    Int64 now = SystemClock::GetUptimeMillis();
    Int64 oldTime = now - ProcessList::MAX_EMPTY_TIME;
    Int32 N = mLruProcesses.GetSize();

//    if (FALSE) {
//        RuntimeException e = new RuntimeException();
//        e.fillInStackTrace();
//        Slogger::I(TAG, "updateOomAdj: top=" + TOP_ACT, e);
//    }

    mAdjSeq++;
    mNewNumServiceProcs = 0;
    mNewNumAServiceProcs = 0;

    Int32 emptyProcessLimit;
    Int32 cachedProcessLimit;
    if (mProcessLimit <= 0) {
        emptyProcessLimit = cachedProcessLimit = 0;
    }
    else if (mProcessLimit == 1) {
        emptyProcessLimit = 1;
        cachedProcessLimit = 0;
    }
    else {
        emptyProcessLimit = ProcessList::ComputeEmptyProcessLimit(mProcessLimit);
        cachedProcessLimit = mProcessLimit - emptyProcessLimit;
    }
    // Let's determine how many processes we have running vs.
    // how many slots we have for background processes; we may want
    // to put multiple processes in a slot of there are enough of
    // them.

    Int32 numSlots = (ProcessList::CACHED_APP_MAX_ADJ
                - ProcessList::CACHED_APP_MIN_ADJ + 1) / 2;
    Int32 numEmptyProcs = N - mNumNonCachedProcs - mNumCachedHiddenProcs;
    if (numEmptyProcs > cachedProcessLimit) {
        // If there are more empty processes than our limit on cached
        // processes, then use the cached process limit for the factor.
        // This ensures that the really old empty processes get pushed
        // down to the bottom, so if we are running low on memory we will
        // have a better chance at keeping around more cached processes
        // instead of a gazillion empty processes.
        numEmptyProcs = cachedProcessLimit;
    }

    Int32 emptyFactor = numEmptyProcs/numSlots;
    if (emptyFactor < 1) emptyFactor = 1;
    Int32 cachedFactor = (mNumCachedHiddenProcs > 0 ? mNumCachedHiddenProcs : 1)/numSlots;
    if (cachedFactor < 1) cachedFactor = 1;
    Int32 stepCached = 0;
    Int32 stepEmpty = 0;
    Int32 numCached = 0;
    Int32 numEmpty = 0;
    Int32 numTrimming = 0;

    mNumNonCachedProcs = 0;
    mNumCachedHiddenProcs = 0;

    // First update the OOM adjustment for each of the
    // application processes based on their current state.
    Int32 curCachedAdj = ProcessList::CACHED_APP_MIN_ADJ;
    Int32 nextCachedAdj = curCachedAdj + 1;
    Int32 curEmptyAdj = ProcessList::CACHED_APP_MIN_ADJ;
    Int32 nextEmptyAdj = curEmptyAdj + 2;

    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (Int32 i = 0; rit != mLruProcesses.REnd(); ++rit, i++) {
        AutoPtr<ProcessRecord> app = *rit;
        if (!app->mKilledByAm && app->mThread != NULL) {
            app->mProcStateChanged = FALSE;
            ComputeOomAdjLocked(app, ProcessList::UNKNOWN_ADJ, TOP_APP, TRUE, now);

            // If we haven't yet assigned the final cached adj
            // to the process, do that now.
            if (app->mCurAdj >= ProcessList::UNKNOWN_ADJ) {
                switch (app->mCurProcState) {
                    case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY:
                    case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY_CLIENT:
                        // This process is a cached process holding activities...
                        // assign it the next cached value for that type, and then
                        // step that cached level.
                        app->mCurRawAdj = curCachedAdj;
                        app->mCurAdj = app->ModifyRawOomAdj(curCachedAdj);
                        if (DEBUG_LRU && FALSE) Slogger::D(TAG, "Assigning activity LRU #%d"
                            " adj: %d (curCachedAdj=%d)", i, app->mCurAdj, curCachedAdj);
                        if (curCachedAdj != nextCachedAdj) {
                            stepCached++;
                            if (stepCached >= cachedFactor) {
                                stepCached = 0;
                                curCachedAdj = nextCachedAdj;
                                nextCachedAdj += 2;
                                if (nextCachedAdj > ProcessList::CACHED_APP_MAX_ADJ) {
                                    nextCachedAdj = ProcessList::CACHED_APP_MAX_ADJ;
                                }
                            }
                        }
                        break;
                    default:
                        // For everything else, assign next empty cached process
                        // level and bump that up.  Note that this means that
                        // long-running services that have dropped down to the
                        // cached level will be treated as empty (since their process
                        // state is still as a service), which is what we want.
                        app->mCurRawAdj = curEmptyAdj;
                        app->mCurAdj = app->ModifyRawOomAdj(curEmptyAdj);
                        if (DEBUG_LRU && FALSE) Slogger::D(TAG, "Assigning empty LRU #%d"
                            " adj: %d (curEmptyAdj=%d)", i, app->mCurAdj, curEmptyAdj);
                        if (curEmptyAdj != nextEmptyAdj) {
                            stepEmpty++;
                            if (stepEmpty >= emptyFactor) {
                                stepEmpty = 0;
                                curEmptyAdj = nextEmptyAdj;
                                nextEmptyAdj += 2;
                                if (nextEmptyAdj > ProcessList::CACHED_APP_MAX_ADJ) {
                                    nextEmptyAdj = ProcessList::CACHED_APP_MAX_ADJ;
                                }
                            }
                        }
                        break;
                }
            }

            ApplyOomAdjLocked(app, TOP_APP, TRUE, now);

            // Count the number of process types.
            switch (app->mCurProcState) {
                case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY:
                case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY_CLIENT:
                    mNumCachedHiddenProcs++;
                    numCached++;
                    if (numCached > cachedProcessLimit) {
                        StringBuilder sb("cached #");
                        sb += numCached;
                        app->Kill(sb.ToString(), TRUE);
                    }
                    break;
                case IActivityManager::PROCESS_STATE_CACHED_EMPTY:
                    if (numEmpty > ProcessList::TRIM_EMPTY_APPS
                            && app->mLastActivityTime < oldTime) {
                        StringBuilder sb("empty for");
                        sb += ((oldTime + ProcessList::MAX_EMPTY_TIME - app->mLastActivityTime) / 1000);
                        sb += "s";
                        app->Kill(sb.ToString(), TRUE);
                    }
                    else {
                        numEmpty++;
                        if (numEmpty > emptyProcessLimit) {
                            StringBuilder sb("empty #");
                            sb += numEmpty;
                            app->Kill(sb.ToString(), TRUE);
                        }
                    }
                    break;
                default:
                    mNumNonCachedProcs++;
                    break;
            }

            if (app->mIsolated && app->mServices.GetSize() <= 0) {
                // If this is an isolated process, and there are no
                // services running in it, then the process is no longer
                // needed.  We agressively kill these because we can by
                // definition not re-use the same process again, and it is
                // good to avoid having whatever code was running in them
                // left sitting around after no longer needed.
                app->Kill(String("isolated not needed"), TRUE);
            }

            if (app->mCurProcState >= IActivityManager::PROCESS_STATE_HOME
                    && !app->mKilledByAm) {
                numTrimming++;
            }
        }
    }

    mNumServiceProcs = mNewNumServiceProcs;

    // Now determine the memory trimming level of background processes.
    // Unfortunately we need to start at the back of the list to do this
    // properly.  We only do this if the number of background apps we
    // are managing to keep around is less than half the maximum we desire;
    // if we are keeping a good number around, we'll let them use whatever
    // memory they want.
    Int32 numCachedAndEmpty = numCached + numEmpty;
    Int32 memFactor;
    if (numCached <= ProcessList::TRIM_CACHED_APPS
            && numEmpty <= ProcessList::TRIM_EMPTY_APPS) {
        if (numCachedAndEmpty <= ProcessList::TRIM_CRITICAL_THRESHOLD) {
            memFactor = IProcessStats::ADJ_MEM_FACTOR_CRITICAL;
        }
        else if (numCachedAndEmpty <= ProcessList::TRIM_LOW_THRESHOLD) {
            memFactor = IProcessStats::ADJ_MEM_FACTOR_LOW;
        }
        else {
            memFactor = IProcessStats::ADJ_MEM_FACTOR_MODERATE;
        }
    }
    else {
        memFactor = IProcessStats::ADJ_MEM_FACTOR_NORMAL;
    }
    // We always allow the memory level to go up (better).  We only allow it to go
    // down if we are in a state where that is allowed, *and* the total number of processes
    // has gone down since last time.
    if (DEBUG_OOM_ADJ) Slogger::D(TAG, "oom: memFactor=%d last=%d allowLow=%d numProcs=%d last=%d",
        memFactor, mLastMemoryLevel, mAllowLowerMemLevel, mLruProcesses.GetSize(), mLastNumProcesses);
    if (memFactor > mLastMemoryLevel) {
        if (!mAllowLowerMemLevel || (Int32)mLruProcesses.GetSize() >= mLastNumProcesses) {
            memFactor = mLastMemoryLevel;
            if (DEBUG_OOM_ADJ) Slogger::D(TAG, "Keeping last mem factor!");
        }
    }
    mLastMemoryLevel = memFactor;
    mLastNumProcesses = mLruProcesses.GetSize();
    Boolean allChanged = mProcessStats->SetMemFactorLocked(memFactor, !IsSleeping(), now);
    Int32 trackerMemFactor = mProcessStats->GetMemFactorLocked();
    if (memFactor != IProcessStats::ADJ_MEM_FACTOR_NORMAL) {
        if (mLowRamStartTime == 0) {
            mLowRamStartTime = now;
        }
        Int32 step = 0;
        Int32 fgTrimLevel;
        switch (memFactor) {
            case IProcessStats::ADJ_MEM_FACTOR_CRITICAL:
                fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL;
                break;
            case IProcessStats::ADJ_MEM_FACTOR_LOW:
                fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_LOW;
                break;
            default:
                fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_MODERATE;
                break;
        }
        Int32 factor = numTrimming/3;
        Int32 minFactor = 2;
        if (mHomeProcess != NULL) minFactor++;
        if (mPreviousProcess != NULL) minFactor++;
        if (factor < minFactor) factor = minFactor;
        Int32 curLevel = IComponentCallbacks2::TRIM_MEMORY_COMPLETE;

        rit = mLruProcesses.RBegin();
        for (; rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if (allChanged || app->mProcStateChanged) {
                SetProcessTrackerStateLocked(app, trackerMemFactor, now);
                app->mProcStateChanged = FALSE;
            }
            if (app->mCurProcState >= IActivityManager::PROCESS_STATE_HOME
                    && !app->mKilledByAm) {
                if (app->mTrimMemoryLevel < curLevel && app->mThread != NULL) {
                    if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                        "Trimming memory of %s to %d",
                        app->mProcessName.string(), curLevel);
                    app->mThread->ScheduleTrimMemory(curLevel);
                    if (FALSE) {
                        // For now we won't do this; our memory trimming seems
                        // to be good enough at this point that destroying
                        // activities causes more harm than good.
                        if (curLevel >= IComponentCallbacks2::TRIM_MEMORY_COMPLETE
                                && app != mHomeProcess && app != mPreviousProcess) {
                            // Need to do this on its own message because the stack may not
                            // be in a consistent state at this point.
                            // For these apps we will also finish their activities
                            // to help them free memory.
                            mStackSupervisor->ScheduleDestroyAllActivities(app, String("trim"));
                        }
                    }
                }
                app->mTrimMemoryLevel = curLevel;
                step++;
                if (step >= factor) {
                    step = 0;
                    switch (curLevel) {
                        case IComponentCallbacks2::TRIM_MEMORY_COMPLETE:
                            curLevel = IComponentCallbacks2::TRIM_MEMORY_MODERATE;
                            break;
                        case IComponentCallbacks2::TRIM_MEMORY_MODERATE:
                            curLevel = IComponentCallbacks2::TRIM_MEMORY_BACKGROUND;
                            break;
                    }
                }
            }
            else if (app->mCurProcState == IActivityManager::PROCESS_STATE_HEAVY_WEIGHT) {
                if (app->mTrimMemoryLevel < IComponentCallbacks2::TRIM_MEMORY_BACKGROUND
                        && app->mThread != NULL) {
                    //try {
                    if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                        "Trimming memory of heavy-weight %s to %d", app->mProcessName.string(),
                        IComponentCallbacks2::TRIM_MEMORY_BACKGROUND);
                        app->mThread->ScheduleTrimMemory(
                                IComponentCallbacks2::TRIM_MEMORY_BACKGROUND);
                    //} catch (RemoteException e) {
                    //}
                }
                app->mTrimMemoryLevel = IComponentCallbacks2::TRIM_MEMORY_BACKGROUND;
            }
            else {
                if ((app->mCurProcState >= IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND
                    || app->mSystemNoUi) && app->mPendingUiClean) {
                    // If this application is now in the background and it
                    // had done UI, then give it the special trim level to
                    // have it free UI resources.
                    Int32 level = IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN;
                    if (app->mTrimMemoryLevel < level && app->mThread != NULL) {
                        //try {
                            if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                                "Trimming memory of bg-ui %s to %d",
                                app->mProcessName.string(), level);
                            app->mThread->ScheduleTrimMemory(level);
                        //} catch (RemoteException e) {
                        //}
                    }
                    app->mPendingUiClean = FALSE;
                }
                if (app->mTrimMemoryLevel < fgTrimLevel && app->mThread != NULL) {
                    //try {
                        if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                            "Trimming memory of fg %s to %d",
                            app->mProcessName.string(), fgTrimLevel);
                        app->mThread->ScheduleTrimMemory(fgTrimLevel);
                    //} catch (RemoteException e) {
                    //}
                }
                app->mTrimMemoryLevel = fgTrimLevel;
            }
        }
    }
    else {
        if (mLowRamStartTime != 0) {
            mLowRamTimeSinceLastIdle += now - mLowRamStartTime;
            mLowRamStartTime = 0;
        }
        rit = mLruProcesses.RBegin();
        for (; rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if (allChanged || app->mProcStateChanged) {
                SetProcessTrackerStateLocked(app, trackerMemFactor, now);
                app->mProcStateChanged = FALSE;
            }
            if ((app->mCurProcState >= IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND
                    || app->mSystemNoUi) && app->mPendingUiClean) {
                if (app->mTrimMemoryLevel < IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN
                        && app->mThread != NULL) {
                    //try {
                    if (DEBUG_SWITCH || DEBUG_OOM_ADJ) Slogger::V(TAG,
                        "Trimming memory of ui hidden %s to %d", app->mProcessName.string(),
                        IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN);
                        app->mThread->ScheduleTrimMemory(
                                IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN);
                    //} catch (RemoteException e) {
                    //}
                }
                app->mPendingUiClean = FALSE;
            }
            app->mTrimMemoryLevel = 0;
        }
    }

    if (mAlwaysFinishActivities) {
        // Need to do this on its own message because the stack may not
        // be in a consistent state at this point.
        mStackSupervisor->ScheduleDestroyAllActivities(NULL, String("always-finish"));
    }

    if (allChanged) {
        RequestPssAllProcsLocked(now, FALSE, mProcessStats->IsMemFactorLowered());
    }

    if (mProcessStats->ShouldWriteNowLocked(now)) {
        AutoPtr<Runnable> runnable = new WriteStateRunnable(this);
        Boolean res;
        mHandler->Post(runnable, &res);
    }

    if (DEBUG_OOM_ADJ) {
        if (FALSE) {
            // RuntimeException here = new RuntimeException("here");
            // here.fillInStackTrace();
            // Slog.d(TAG, "Did OOM ADJ in " + (SystemClock.uptimeMillis()-now) + "ms", here);
        }
        else {
            Slogger::D(TAG, "Did OOM ADJ in %lldms", (SystemClock::GetUptimeMillis()-now));
        }
    }

    return NOERROR;
}

ECode CActivityManagerService::TrimApplications()
{
    {
        AutoLock lock(this);
        Int32 i = 0;

        // First remove any unused application processes whose package
        // has been removed.
        mRemovedProcesses->GetSize(&i);
        for (i = i - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mRemovedProcesses->Get(i, (IInterface**)&obj);
            ProcessRecord* app = (ProcessRecord*)IProcessRecord::Probe(obj);
            if (app->mActivities.IsEmpty()
                && app->mCurReceiver == NULL && app->mServices.IsEmpty()) {
                // Slogger::I(
                //     TAG, "Exiting empty application process "
                //     + app.processName + " ("
                //     + (app.thread != NULL ? app.thread.asBinder() : NULL)
                //     + ")\n");
                if (app->mPid > 0 && app->mPid != MY_PID) {
                    app->Kill(String("empty"), FALSE);
                }
                else {
                    // try
                    // {
                    app->mThread->ScheduleExit();
                    // }
                    // catch (Exception e)
                    // {
                    //     // Ignore exceptions.
                    // }
                }
                CleanUpApplicationRecordLocked(app, FALSE, TRUE, -1);
                mRemovedProcesses->Remove(i);

                if (app->mPersistent) {
                    AddAppLocked(app->mInfo, FALSE, String(NULL) /* ABI override */);
                }
            }
        }

        // Now update the oom adj for all processes.
        UpdateOomAdjLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::SignalPersistentProcesses(
    /* [in] */ Int32 sig)
{
    if (sig != IProcess::SIGNAL_USR1) {
        // throw new SecurityException("Only SIGNAL_USR1 is allowed");
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    if (CheckCallingPermission(Manifest::permission::SIGNAL_PERSISTENT_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Manifest::permission::SIGNAL_PERSISTENT_PROCESSES);
        return E_SECURITY_EXCEPTION;
    }

    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL && r->mPersistent) {
            Process::SendSignal(r->mPid, sig);
        }
    }

    return NOERROR;
}

ECode CActivityManagerService::StopProfilerLocked(
    /* [in] */ ProcessRecord* proc,
    /* [in] */ Int32 profileType)
{
    if (proc == NULL || proc == mProfileProc) {
        proc = mProfileProc;
        profileType = mProfileType;
        ClearProfilerLocked();
    }
    if (proc == NULL) {
        return NOERROR;
    }
    //try {
    proc->mThread->ProfilerControl(FALSE, NULL, profileType);
    //} catch (RemoteException e) {
    //    throw new IllegalStateException("Process disappeared");
    //}
    return NOERROR;
}

ECode CActivityManagerService::ClearProfilerLocked()
{
    if (mProfileFd != NULL) {
        //try {
        mProfileFd->Close();
        //} catch (IOException e) {
        //}
    }
    mProfileApp = NULL;
    mProfileProc = NULL;
    mProfileFile = NULL;
    mProfileType = 0;
    mAutoStopProfiler = FALSE;
    mSamplingInterval = 0;
    return NOERROR;
}

ECode CActivityManagerService::ProfileControl(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean start,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ Int32 profileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    {
        AutoLock lock(this);
        // note: hijacking SET_ACTIVITY_WATCHER, but should be changed to
        // its own permission.
        if (CheckCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER)
                != IPackageManager::PERMISSION_GRANTED) {
            // throw new SecurityException("Requires permission "
            //         + Manifest::permission::SET_ACTIVITY_WATCHER);
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<IParcelFileDescriptor> fd;
        if (start && (profilerInfo == NULL || (profilerInfo->GetProfileFd((IParcelFileDescriptor**)&fd),
            fd == NULL))) {
            Slogger::E(TAG, "null profile info or fd");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<ProcessRecord> proc;
        if (process != NULL) {
            proc = FindProcessLocked(process, userId, String("profileControl"));
        }

        if (start && (proc == NULL || proc->mThread == NULL)) {
            // throw new IllegalArgumentException("Unknown process: " + process);
            if (fd != NULL) {
                fd->Close();
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (start) {
            StopProfilerLocked(NULL, 0);
            SetProfileApp(proc->mInfo, proc->mProcessName, profilerInfo);
            mProfileProc = proc;
            mProfileType = profileType;
            // try {
            AutoPtr<IParcelFileDescriptor> pfd;
            if (SUCCEEDED(fd->Dup((IParcelFileDescriptor**)&pfd))) {
                fd = NULL;
                fd = pfd;
            }
            else {
                fd = NULL;
            }
            profilerInfo->SetProfileFd(fd);
            proc->mThread->ProfilerControl(start, profilerInfo, profileType);
            mProfileFd = NULL;
        }
        else {
            StopProfilerLocked(proc, profileType);
            if (fd != NULL) {
                // try {
                fd->Close();
                // } catch (IOException e) {
                // }
            }
        }

        *result = TRUE;
        if (fd != NULL) {
            fd->Close();
        }
        return NOERROR;
    }
    // } catch (RemoteException e) {
    //     throw new IllegalStateException("Process disappeared");
    // } finally {
    // if (fd != NULL) {
        // try {
        // fd->Close();
        // } catch (IOException e) {
        // }
    // }
    // }
}

AutoPtr<ProcessRecord> CActivityManagerService::FindProcessLocked(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callName)
{
    HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, TRUE, ALLOW_FULL_ONLY, callName, String(NULL), &userId);
    AutoPtr<ProcessRecord> proc;
    // try {
    Int32 pid = StringUtils::ParseInt32(process);
    {
        AutoLock lock(mPidsSelfLockedLock);
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = mPidsSelfLocked.Find(pid);
        if (it != mPidsSelfLocked.End()) proc = it->mSecond;
    }
    // } catch (NumberFormatException e) {
    // }

    if (proc == NULL) {
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > all;
        all = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator it;
        it = all->Find(process);
        AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > procs;
        if (it != all->End()) procs = it->mSecond;
        if (procs != NULL && !procs->IsEmpty()) {
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it = procs->Begin();
            proc = it->mSecond;
            if (userId != IUserHandle::USER_ALL && proc->mUserId != userId) {
                for (++it; it != procs->End(); ++it) {
                    AutoPtr<ProcessRecord> thisProc = it->mSecond;
                    if (thisProc->mUserId == userId) {
                        proc = thisProc;
                        break;
                    }
                }
            }
        }
    }

    return proc;
}

ECode CActivityManagerService::DumpHeap(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean managed,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    // try {
        {
            AutoLock lock(this);
            // note: hijacking SET_ACTIVITY_WATCHER, but should be changed to
            // its own permission (same as profileControl).
            if (CheckCallingPermission(Manifest::permission::SET_ACTIVITY_WATCHER)
                    != IPackageManager::PERMISSION_GRANTED) {
                // throw new SecurityException("Requires permission "
                //         + Manifest::permission::SET_ACTIVITY_WATCHER);
                if (fd != NULL) {
                    fd->Close();
                }
                return E_SECURITY_EXCEPTION;
            }

            if (fd == NULL) {
                // throw new IllegalArgumentException("null fd");
                if (fd != NULL) {
                    fd->Close();
                }
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<ProcessRecord> proc = FindProcessLocked(process, userId, String("dumpHeap"));
            if (proc == NULL || proc->mThread == NULL) {
                // throw new IllegalArgumentException("Unknown process: " + process);
                if (fd != NULL) {
                    fd->Close();
                }
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            String value;
            sysProp->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
            Boolean isDebuggable = value.Equals("1");
            if (!isDebuggable) {
                Int32 flags;
                proc->mInfo->GetFlags(&flags);
                if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) == 0) {
                    // throw new SecurityException("Process not debuggable: " + proc);
                    if (fd != NULL) {
                        fd->Close();
                    }
                    return E_SECURITY_EXCEPTION;
                }
            }

            proc->mThread->DumpHeap(managed, path, fd);
            fd = NULL;
            return TRUE;
        }
    // } catch (RemoteException e) {
    //     throw new IllegalStateException("Process disappeared");
    // } finally {
    // if (fd != NULL) {
        // try {
        // fd->Close();
        // } catch (IOException e) {
        // }
    // }
    // }
    return NOERROR;
}

ECode CActivityManagerService::Monitor()
{
    AutoLock lock(this);
    return NOERROR;
}

ECode CActivityManagerService::OnCoreSettingsChange(
    /* [in] */ IBundle* settings)
{
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> processRecord = *rit;
        // try {
        if (processRecord->mThread != NULL) {
            processRecord->mThread->SetCoreSettings(settings);
        }
        // } catch (RemoteException re) {
        //     /* ignore */
        // }
    }
    return NOERROR;
}

// Multi-user methods

/**
 * Start user, if its not already running, but don't bring it to foreground.
 */
ECode CActivityManagerService::StartUserInBackground(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    return StartUser(userId, /* foreground */ FALSE, result);
}

/**
 * Start user, if its not already running, and bring it to foreground.
 */
ECode CActivityManagerService::StartUserInForeground(
    /* [in] */ Int32 userId,
    /* [in] */ IDialog* dlg,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(StartUser(userId, /* foreground */ TRUE, result));
    IDialogInterface::Probe(dlg)->Dismiss();
    return NOERROR;
}

/**
 * Refreshes the list of users related to the current user when either a
 * user switch happens or when a new related user is started in the
 * background.
 */
ECode CActivityManagerService::UpdateCurrentProfileIdsLocked()
{
    AutoPtr<IList> profiles;
    GetUserManagerLocked()->GetProfiles(
        mCurrentUserId, FALSE /* enabledOnly */, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    AutoPtr<ArrayOf<Int32> > currentProfileIds = ArrayOf<Int32>::Alloc(size); // profiles will not be null
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        profiles->Get(i, (IInterface**)&item);
        Int32 id;
        IUserInfo::Probe(item)->GetId(&id);
        (*currentProfileIds)[i] = id;
    }
    mCurrentProfileIds = currentProfileIds;

    {    AutoLock syncLock(mUserProfileGroupIdsSelfLockedLock);
        mUserProfileGroupIdsSelfLocked.Clear();
        AutoPtr<IList> users;
        GetUserManagerLocked()->GetUsers(FALSE, (IList**)&users);
        users->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            users->Get(i, (IInterface**)&item);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(item);
            Int32 id, profileGroupId;
            user->GetId(&id);
            user->GetProfileGroupId(&profileGroupId);
            if (profileGroupId != IUserInfo::NO_PROFILE_GROUP_ID) {
                mUserProfileGroupIdsSelfLocked[id] = profileGroupId;
            }
        }
    }
    return NOERROR;
}

AutoPtr<HashSet<Int32> > CActivityManagerService::GetProfileIdsLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<HashSet<Int32> > userIds = new HashSet<Int32>();
    AutoPtr<IList> profiles;
    GetUserManagerLocked()->GetProfiles(
        userId, FALSE /* enabledOnly */, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        profiles->Get(i, (IInterface**)&item);
        Int32 id;
        IUserInfo::Probe(item)->GetId(&id);
        userIds->Insert(id);
    }
    return userIds;
}

ECode CActivityManagerService::SwitchUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceShellRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES, userId));
    String userName;
    {    AutoLock syncLock(this);
        AutoPtr<IUserInfo> userInfo;
        GetUserManagerLocked()->GetUserInfo(userId, (IUserInfo**)&userInfo);
        if (userInfo == NULL) {
            Slogger::W(TAG, "No user info for user #%d", userId);
            return NOERROR;
        }
        Boolean res;
        userInfo->IsManagedProfile(&res);
        if (res) {
            Slogger::W(TAG, "Cannot switch to User #%d: not a full user", userId);
            return NOERROR;
        }
        IPackageItemInfo::Probe(userInfo)->GetName(&userName);
        mTargetUserId = userId;
    }
    mHandler->RemoveMessages(START_USER_SWITCH_MSG);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(START_USER_SWITCH_MSG, userId, 0,
        CoreUtils::Convert(userName), (IMessage**)&msg);
    Boolean res;
    mHandler->SendMessage(msg, &res);
    *result = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::ShowUserSwitchDialog(
    /* [in] */ Int32 userId,
    /* [in] */ const String& userName)
{
    // The dialog will show and then initiate the user switch by calling startUserInForeground
    AutoPtr<IDialog> d = new UserSwitchingDialog(this, mContext, userId, userName,
            TRUE /* above system */);
    return d->Show();
}

ECode CActivityManagerService::StartUser(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean foreground,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> StartUser: %d", userId);
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: switchUser() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString().string());
        return E_SECURITY_EXCEPTION;
    }

    if (DEBUG_MU) Slogger::I(TAG_MU, "starting userid:%d fore:%d", userId, foreground);

    const Int64 ident = Binder::ClearCallingIdentity();
    // try {
        {
            AutoLock lock(this);
            const Int32 oldUserId = mCurrentUserId;
            if (oldUserId == userId) {
                *result = TRUE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }

            mStackSupervisor->SetLockTaskModeLocked(NULL, FALSE);

            AutoPtr<IUserInfo> userInfo;
            GetUserManagerLocked()->GetUserInfo(userId, (IUserInfo**)&userInfo);
            if (userInfo == NULL) {
                Slogger::W(TAG, "No user info for user #%d", userId);
                *result = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }

            Boolean res;
            if (foreground && (userInfo->IsManagedProfile(&res), res)) {
                Slogger::W(TAG, "Cannot switch to User #%d: not a full user", userId);
                *result = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }

            if (foreground) {
                mWindowManager->StartFreezingScreen(R::anim::screen_user_exit,
                        R::anim::screen_user_enter);
            }

            Boolean needStart = FALSE;

            // If the user we are switching to is not currently started, then
            // we need to start it now.
            if (mStartedUsers[userId] == NULL) {
                AutoPtr<IUserHandle> userHandle;
                CUserHandle::New(userId, (IUserHandle**)&userHandle);
                mStartedUsers[userId] = new UserStartedState(userHandle, FALSE);
                UpdateStartedUserArrayLocked();
                needStart = TRUE;
            }

            mUserLru.Remove(userId);
            mUserLru.PushBack(userId);

            if (foreground) {
                mCurrentUserId = userId;
                mTargetUserId = IUserHandle::USER_NULL; // reset, mCurrentUserId has caught up
                UpdateCurrentProfileIdsLocked();
                mWindowManager->SetCurrentUser(userId, mCurrentProfileIds);

                // Once the internal notion of the active user has switched, we lock the device
                // with the option to show the user switcher on the keyguard.
                mWindowManager->LockNow(NULL);
            }
            else {
                UpdateCurrentProfileIdsLocked();
                mWindowManager->SetCurrentProfileIds(mCurrentProfileIds);
                mUserLru.Remove(mCurrentUserId);
                mUserLru.PushBack(mCurrentUserId);
            }

            AutoPtr<UserStartedState> uss = mStartedUsers[userId];

            // Make sure user is in the started state.  If it is currently
            // stopping, we need to knock that off.
            if (uss->mState == UserStartedState::STATE_STOPPING) {
                // If we are stopping, we haven't sent ACTION_SHUTDOWN,
                // so we can just fairly silently bring the user back from
                // the almost-dead.
                uss->mState = UserStartedState::STATE_RUNNING;
                UpdateStartedUserArrayLocked();
                needStart = TRUE;
            }
            else if (uss->mState == UserStartedState::STATE_SHUTDOWN) {
                // This means ACTION_SHUTDOWN has been sent, so we will
                // need to treat this as a new boot of the user.
                uss->mState = UserStartedState::STATE_BOOTING;
                UpdateStartedUserArrayLocked();
                needStart = TRUE;
            }

            if (uss->mState == UserStartedState::STATE_BOOTING) {
                // Booting up a new user, need to tell system services about it.
                // Note that this is on the same handler as scheduling of broadcasts,
                // which is important because it needs to go first.
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(SYSTEM_USER_START_MSG, userId, 0, (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessage(msg, &result);
            }

            if (foreground) {
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(SYSTEM_USER_CURRENT_MSG, userId, oldUserId, (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessage(msg, &result);
                mHandler->RemoveMessages(REPORT_USER_SWITCH_MSG);
                mHandler->RemoveMessages(USER_SWITCH_TIMEOUT_MSG);

                msg = NULL;
                mHandler->ObtainMessage(REPORT_USER_SWITCH_MSG, oldUserId, userId,
                    uss->Probe(EIID_IInterface), (IMessage**)&msg);
                mHandler->SendMessage(msg, &result);

                msg = NULL;
                mHandler->ObtainMessage(USER_SWITCH_TIMEOUT_MSG, oldUserId, userId,
                    uss->Probe(EIID_IInterface), (IMessage**)&msg);
                mHandler->SendMessageDelayed(msg, USER_SWITCH_TIMEOUT, &result);
            }

            String nullStr;
            if (needStart) {
                // Send USER_STARTED broadcast
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_USER_STARTED, (IIntent**)&intent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                        | IIntent::FLAG_RECEIVER_FOREGROUND);
                intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                Int32 bval;
                BroadcastIntentLocked(NULL, nullStr, intent,
                    nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
                    FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &bval);
            }

            Int32 userInfoFlags;
            userInfo->GetFlags(&userInfoFlags);
            if ((userInfoFlags&IUserInfo::FLAG_INITIALIZED) == 0) {
                if (userId != IUserHandle::USER_OWNER) {
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_USER_INITIALIZE, (IIntent**)&intent);
                    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                    AutoPtr<IIntentReceiver> receiver;
                    CActivityManagerSwitchUserReceiver::New(this, uss.Get(), foreground,
                        oldUserId,  userId, (IIntentReceiver**)&receiver);
                    Int32 bval;
                    BroadcastIntentLocked(NULL, nullStr, intent, nullStr,
                        receiver, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
                        TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID,
                        userId, &bval);
                    uss->mInitializing = TRUE;
                }
                else {
                    Int32 userInfoId;
                    userInfo->GetId(&userInfoId);
                    GetUserManagerLocked()->MakeInitialized(userInfoId);
                }
            }

            if (foreground) {
                if (!uss->mInitializing) {
                    MoveUserToForeground(uss, oldUserId, userId);
                }
            }
            else {
                mStackSupervisor->StartBackgroundUserLocked(userId, uss);
            }

            if (needStart) {
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_USER_STARTING, (IIntent**)&intent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
                intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                AutoPtr<IIntentReceiver> receiver;
                CActivityManagerNeedStartReceiver::New((IIntentReceiver**)&receiver);
                Int32 bval;
                BroadcastIntentLocked(NULL, nullStr, intent,
                    nullStr, receiver, 0, nullStr, NULL,
                    Manifest::permission::INTERACT_ACROSS_USERS, IAppOpsManager::OP_NONE,
                    TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &bval);
            }
        }
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    *result = TRUE;
    return NOERROR;
}

ECode CActivityManagerService::SendUserSwitchBroadcastsLocked(
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    Int64 ident = Binder::ClearCallingIdentity();
    String nullStr;
    // try {
    AutoPtr<IIntent> intent;
    if (oldUserId >= 0) {
        // Send USER_BACKGROUND broadcast to all profiles of the outgoing user
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(oldUserId, FALSE, (IList**)&profiles);
        Int32 count;
        profiles->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> item;
            profiles->Get(i, (IInterface**)&item);
            Int32 profileUserId;
            IUserInfo::Probe(item)->GetId(&profileUserId);
            intent = NULL;
            CIntent::New(IIntent::ACTION_USER_BACKGROUND, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutExtra(IIntent::EXTRA_USER_HANDLE, profileUserId);
            Int32 result;
            BroadcastIntentLocked(NULL, nullStr, intent,
                nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
                FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, profileUserId, &result);
        }
    }
    if (newUserId >= 0) {
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(newUserId, FALSE, (IList**)&profiles);
        Int32 count;
        profiles->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> item;
            profiles->Get(i, (IInterface**)&item);
            Int32 profileUserId;
            IUserInfo::Probe(item)->GetId(&profileUserId);
            intent = NULL;
            CIntent::New(IIntent::ACTION_USER_FOREGROUND, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutExtra(IIntent::EXTRA_USER_HANDLE, profileUserId);
            Int32 result;
            BroadcastIntentLocked(NULL, nullStr, intent,
                nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
                FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, profileUserId, &result);
        }
        intent = NULL;
        CIntent::New(IIntent::ACTION_USER_SWITCHED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, newUserId);

        Int32 result;
        BroadcastIntentLocked(NULL, nullStr, intent,
                nullStr, NULL, 0, nullStr, NULL,
                Manifest::permission::MANAGE_USERS, IAppOpsManager::OP_NONE,
                FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID,
                IUserHandle::USER_ALL, &result);
    }
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::DispatchUserSwitch(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    Int32 N;
    mUserSwitchObservers->BeginBroadcast(&N);
    if (N > 0) {
        AutoPtr<IIRemoteCallback> callback;
        CActivityManagerDispatchUserSwitchCallback::New(
            this, N, uss, oldUserId, newUserId, (IIRemoteCallback**)&callback);

        {
            AutoLock lock(this);
            uss->mSwitching = TRUE;
            mCurUserSwitchCallback = callback;
        }
        for (Int32 i = 0; i < N; i++) {
            // try {
            AutoPtr<IInterface> item;
            mUserSwitchObservers->GetBroadcastItem(i, (IInterface**)&item);
            AutoPtr<IIUserSwitchObserver> observer = IIUserSwitchObserver::Probe(item);
            observer->OnUserSwitching(newUserId, callback);
            // } catch (RemoteException e) {
            // }
        }
    }
    else {
        AutoLock lock(this);
        SendContinueUserSwitchLocked(uss, oldUserId, newUserId);
    }
    mUserSwitchObservers->FinishBroadcast();
    return NOERROR;
}

ECode CActivityManagerService::TimeoutUserSwitch(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    AutoLock lock(this);
    Slogger::W(TAG, "User switch timeout: from %d to %d", oldUserId, newUserId);
    return SendContinueUserSwitchLocked(uss, oldUserId, newUserId);
}

ECode CActivityManagerService::SendContinueUserSwitchLocked(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    mCurUserSwitchCallback = NULL;

    mHandler->RemoveMessages(USER_SWITCH_TIMEOUT_MSG);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(CONTINUE_USER_SWITCH_MSG, oldUserId, newUserId,
        uss ? uss->Probe(EIID_IInterface) : NULL, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CActivityManagerService::OnUserInitialized(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Boolean foreground,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    Logger::I(TAG, " >> OnUserInitialized: oldUserId:%d, newUserId:%d", oldUserId, newUserId);
    {
        AutoLock syncLock(this);
        if (foreground) {
            MoveUserToForeground(uss, oldUserId, newUserId);
        }
    }

    return CompleteSwitchAndInitalize(uss, newUserId, TRUE, FALSE);
}

ECode CActivityManagerService::MoveUserToForeground(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    Boolean homeInFront = mStackSupervisor->SwitchUserLocked(newUserId, uss);
    if (homeInFront) {
        StartHomeActivityLocked(newUserId);
    }
    else {
        mStackSupervisor->ResumeTopActivitiesLocked();
    }
    // EventLogTags.writeAmSwitchUser(newUserId);
    GetUserManagerLocked()->UserForeground(newUserId);
    SendUserSwitchBroadcastsLocked(oldUserId, newUserId);
    return NOERROR;
}

ECode CActivityManagerService::ContinueUserSwitch(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 oldUserId,
    /* [in] */ Int32 newUserId)
{
    return CompleteSwitchAndInitalize(uss, newUserId, FALSE, TRUE);
}

ECode CActivityManagerService::CompleteSwitchAndInitalize(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 newUserId,
    /* [in] */ Boolean clearInitializing,
    /* [in] */ Boolean clearSwitching)
{
    Boolean unfrozen = FALSE;

    {
        AutoLock lock(this);
        if (clearInitializing) {
            uss->mInitializing = FALSE;
            Int32 ussId;
            uss->mHandle->GetIdentifier(&ussId);
            GetUserManagerLocked()->MakeInitialized(ussId);
        }
        if (clearSwitching) {
            uss->mSwitching = FALSE;
        }
        if (!uss->mSwitching && !uss->mInitializing) {
            mWindowManager->StopFreezingScreen();
            unfrozen = TRUE;
        }
    }

    if (unfrozen) {
        Int32 N;
        mUserSwitchObservers->BeginBroadcast(&N);
        for (Int32 i=0; i<N; i++) {
            // try {
            AutoPtr<IInterface> item;
            mUserSwitchObservers->GetBroadcastItem(i, (IInterface**)&item);
            AutoPtr<IIUserSwitchObserver> observer = IIUserSwitchObserver::Probe(item);
            observer->OnUserSwitchComplete(newUserId);
            // } catch (RemoteException e) {
            // }
        }
        mUserSwitchObservers->FinishBroadcast();
    }
    return NOERROR;
}

ECode CActivityManagerService::ScheduleStartProfilesLocked()
{
    Boolean res;
    mHandler->HasMessages(START_PROFILES_MSG, &res);
    if (!res) {
        mHandler->SendEmptyMessageDelayed(START_PROFILES_MSG,
            IDateUtils::SECOND_IN_MILLIS, &res);
    }
    return NOERROR;
}

ECode CActivityManagerService::StartProfilesLocked()
{
    if (DEBUG_MU) Slogger::I(TAG_MU, "startProfilesLocked");
    AutoPtr<IList> profiles;
    GetUserManagerLocked()->GetProfiles(
        mCurrentUserId, FALSE /* enabledOnly */, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    AutoPtr<IList> toStart;
    CArrayList::New(size, (IList**)&toStart);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        profiles->Get(i, (IInterface**)&item);
        AutoPtr<IUserInfo> user = IUserInfo::Probe(item);
        Int32 flags, id;
        user->GetFlags(&flags);
        user->GetId(&id);
        if ((flags & IUserInfo::FLAG_INITIALIZED) == IUserInfo::FLAG_INITIALIZED
                && id != mCurrentUserId) {
            toStart->Add(user);
        }
    }
    Int32 n;
    toStart->GetSize(&n);
    Int32 i = 0;
    Boolean res;
    for (; i < n && i < (MAX_RUNNING_USERS - 1); ++i) {
        AutoPtr<IInterface> item;
        toStart->Get(i, (IInterface**)&item);
        Int32 id;
        IUserInfo::Probe(item)->GetId(&id);
        FAIL_RETURN(StartUserInBackground(id, &res));
    }
    if (i < n) {
        Slogger::W(TAG_MU, "More profiles than MAX_RUNNING_USERS");
    }
    return NOERROR;
}

ECode CActivityManagerService::FinishUserBoot(
    /* [in] */ UserStartedState* uss)
{
    VALIDATE_NOT_NULL(uss);

    AutoLock lock(this);
    Int32 id;
    uss->mHandle->GetIdentifier(&id);
    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(id);
    AutoPtr<UserStartedState> state;
    if (it != mStartedUsers.End())
        state = it->mSecond;
    if (uss->mState == UserStartedState::STATE_BOOTING
            && state.Get() == uss) {
        uss->mState = UserStartedState::STATE_RUNNING;
        Int32 userId;
        uss->mHandle->GetIdentifier(&userId);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_BOOT_COMPLETED, NULL, (IIntent**)&intent);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
        intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
        Int32 result;
        String nullStr;
        BroadcastIntentLocked(NULL, nullStr, intent,
            nullStr, NULL, 0, nullStr, NULL,
            Manifest::permission::RECEIVE_BOOT_COMPLETED, IAppOpsManager::OP_BOOT_COMPLETED,
            TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &result);
    }
    return NOERROR;
}

ECode CActivityManagerService::FinishUserSwitch(
    /* [in] */ UserStartedState* uss)
{
    AutoLock lock(this);

    FinishUserBoot(uss);

    StartProfilesLocked();

    Int32 num = mUserLru.GetSize();
    List<Int32>::Iterator lit = mUserLru.Begin();
    while (num > MAX_RUNNING_USERS && lit != mUserLru.End()) { // i < mUserLru.GetSize()) {
        Int32 oldUserId = *lit;
        HashMap<Int32, AutoPtr<UserStartedState> >::Iterator hit = mStartedUsers.Find(oldUserId);
        AutoPtr<UserStartedState> oldUss;
        if (hit != mStartedUsers.End()) oldUss = hit->mSecond;
        if (oldUss == NULL) {
            // Shouldn't happen, but be sane if it does.
            lit = mUserLru.Erase(lit);
            num--;
            continue;
        }
        if (oldUss->mState == UserStartedState::STATE_STOPPING
                || oldUss->mState == UserStartedState::STATE_SHUTDOWN) {
            // This user is already stopping, doesn't count.
            num--;
            ++lit;
            continue;
        }
        if (oldUserId == IUserHandle::USER_OWNER || oldUserId == mCurrentUserId) {
            // Owner and current can't be stopped, but count as running.
            ++lit;
            continue;
        }
        // This is a user to be stopped.
        StopUserLocked(oldUserId, NULL);
        num--;
        ++lit;
    }
    return NOERROR;
}

ECode CActivityManagerService::StopUser(
    /* [in] */ Int32 userId,
    /* [in] */ IStopUserCallback* callback,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: switchUser() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }
    if (userId <= 0) {
        //throw new IllegalArgumentException("Can't stop primary user " + userId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(EnforceShellRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES, userId));

    AutoLock lock(this);
    *result = StopUserLocked(userId, callback);
    return NOERROR;
}

Int32 CActivityManagerService::StopUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ IStopUserCallback* callback)
{
    if (DEBUG_MU) Slogger::I(TAG_MU, "stopUserLocked userId=%d", userId);
    if (mCurrentUserId == userId && mTargetUserId == IUserHandle::USER_NULL) {
        return IActivityManager::USER_OP_IS_CURRENT;
    }

    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(userId);
    AutoPtr<UserStartedState> uss;
    if (it != mStartedUsers.End()) uss = it->mSecond;
    if (uss == NULL) {
        // User is not started, nothing to do...  but we do need to
        // callback if requested.
        if (callback != NULL) {
            AutoPtr<IRunnable> runnable = new StopUserLockedRunnable(userId, callback);
            Boolean result;
            mHandler->Post(runnable, &result);
        }
        return IActivityManager::USER_OP_SUCCESS;
    }

    if (callback != NULL) {
        uss->mStopCallbacks.PushBack(callback);
    }

    if (uss->mState != UserStartedState::STATE_STOPPING
            && uss->mState != UserStartedState::STATE_SHUTDOWN) {
        uss->mState = UserStartedState::STATE_STOPPING;
        UpdateStartedUserArrayLocked();

        Int64 ident = Binder::ClearCallingIdentity();
        // try {
            // We are going to broadcast ACTION_USER_STOPPING and then
            // once that is done send a final ACTION_SHUTDOWN and then
            // stop the user.
            AutoPtr<IIntent> stoppingIntent;
            CIntent::New(IIntent::ACTION_USER_STOPPING, (IIntent**)&stoppingIntent);
            stoppingIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
            stoppingIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
            stoppingIntent->PutBooleanExtra(IIntent::EXTRA_SHUTDOWN_USERSPACE_ONLY, TRUE);
            AutoPtr<IIntent> shutdownIntent;
            CIntent::New(IIntent::ACTION_SHUTDOWN, (IIntent**)&shutdownIntent);

            // This is the result receiver for the final shutdown broadcast.
            AutoPtr<IIntentReceiver> shutdownReceiver;
            CActivityManagerShutdownReceiver::New(this, uss.Get(), (IIntentReceiver**)&shutdownReceiver);

            // This is the result receiver for the initial stopping broadcast.
            AutoPtr<IIntentReceiver> stoppingReceiver;
            CActivityManagerStoppingReceiver::New(
                this, uss.Get(), shutdownIntent, shutdownReceiver, userId, (IIntentReceiver**)&stoppingReceiver);
            // Kick things off.
            Int32 result;
            String nullStr;
            BroadcastIntentLocked(NULL, nullStr, stoppingIntent,
                nullStr, stoppingReceiver, 0, nullStr, NULL,
                Manifest::permission::INTERACT_ACROSS_USERS, IAppOpsManager::OP_NONE,
                TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }

    return IActivityManager::USER_OP_SUCCESS;
}

ECode CActivityManagerService::FinishUserStop(
    /* [in] */ UserStartedState* uss)
{
    Int32 userId;
    uss->mHandle->GetIdentifier(&userId);
    Boolean stopped;
    List<AutoPtr<IStopUserCallback> > callbacks;
    {
        AutoLock lock(this);
        callbacks.Insert(callbacks.Begin(), uss->mStopCallbacks.Begin(), uss->mStopCallbacks.End());
        HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(userId);
        AutoPtr<UserStartedState> state;
        if (it != mStartedUsers.End()) state = it->mSecond;
        if (state.Get() != uss) {
            stopped = FALSE;
        }
        else if (uss->mState != UserStartedState::STATE_SHUTDOWN) {
            stopped = FALSE;
        }
        else {
            stopped = TRUE;
            // User can no longer run.
            mStartedUsers.Erase(it);
            mUserLru.Remove(userId);
            UpdateStartedUserArrayLocked();

            // Clean up all state and processes associated with the user.
            // Kill all the processes for the user.
            ForceStopUserLocked(userId, String("finish user"));
        }

        // Explicitly remove the old information in mRecentTasks.
        RemoveRecentTasksForUserLocked(userId);
    }

    List<AutoPtr<IStopUserCallback> >::Iterator it;
    for (it = callbacks.Begin(); it != callbacks.End(); ++it) {
        //try {
        if (stopped) (*it)->UserStopped(userId);
        else (*it)->UserStopAborted(userId);
        //} catch (RemoteException e) {
        //}
    }

    if (stopped) {
        mSystemServiceManager->CleanupUser(userId);
        {    AutoLock syncLock(this);
            mStackSupervisor->RemoveUserLocked(userId);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetCurrentUser(
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    if ((CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS)
            != IPackageManager::PERMISSION_GRANTED) && (
            CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL)
            != IPackageManager::PERMISSION_GRANTED)) {
        Slogger::W(TAG, "Permission Denial: getCurrentUser() from pid=%d, uid=%d requires Manifest::permission::INTERACT_ACROSS_USERS",
            Binder::GetCallingPid(), Binder::GetCallingUid());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(this);
        Int32 userId = mTargetUserId != IUserHandle::USER_NULL ? mTargetUserId : mCurrentUserId;
        return GetUserManagerLocked()->GetUserInfo(userId, userInfo);
    }

    return NOERROR;
}

Int32 CActivityManagerService::GetCurrentUserIdLocked()
{
    return mTargetUserId != IUserHandle::USER_NULL ? mTargetUserId : mCurrentUserId;
}

ECode CActivityManagerService::IsUserRunning(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean orStopped,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: isUserRunning() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::INTERACT_ACROSS_USERS";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    *result = IsUserRunningLocked(userId, orStopped);
    return NOERROR;
}

Boolean CActivityManagerService::IsUserRunningLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean orStopped)
{
    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(userId);
    AutoPtr<UserStartedState> state;
    if (it != mStartedUsers.End()) state = it->mSecond;
    if (state == NULL) {
        return FALSE;
    }
    if (orStopped) {
       return TRUE;
    }
    return state->mState != UserStartedState::STATE_STOPPING
            && state->mState != UserStartedState::STATE_SHUTDOWN;
}

ECode CActivityManagerService::GetRunningUserIds(
    /* [out, callee] */ ArrayOf<Int32>** userIds)
{
    VALIDATE_NOT_NULL(userIds);
    *userIds = NULL;

   if (CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: isUserRunning() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::INTERACT_ACROSS_USERS";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    *userIds = mStartedUserArray;
    REFCOUNT_ADD(*userIds);
    return NOERROR;
}

ECode CActivityManagerService::UpdateStartedUserArrayLocked()
{
    Int32 num = 0;

    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it;
    for (it = mStartedUsers.Begin(); it != mStartedUsers.End(); ++it) {
        AutoPtr<UserStartedState> uss = it->mSecond;
        // This list does not include stopping users.
        if (uss->mState != UserStartedState::STATE_STOPPING
                && uss->mState != UserStartedState::STATE_SHUTDOWN) {
            num++;
        }
    }
    mStartedUserArray = ArrayOf<Int32>::Alloc(num);;
    num = 0;
    for (it = mStartedUsers.Begin(); it != mStartedUsers.End(); ++it) {
        AutoPtr<UserStartedState> uss = it->mSecond;
        if (uss->mState != UserStartedState::STATE_STOPPING
                && uss->mState != UserStartedState::STATE_SHUTDOWN) {
            (*mStartedUserArray)[num] = it->mFirst;
            num++;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::RegisterUserSwitchObserver(
    /* [in] */ IIUserSwitchObserver* observer)
{
   if (CheckCallingPermission(Manifest::permission::INTERACT_ACROSS_USERS_FULL)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: registerUserSwitchObserver() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    Boolean result;
    return mUserSwitchObservers->Register(observer, &result);
}

ECode CActivityManagerService::UnregisterUserSwitchObserver(
    /* [in] */ IIUserSwitchObserver* observer)
{
    Boolean result;
    return mUserSwitchObservers->Unregister(observer, &result);
}

Boolean CActivityManagerService::UserExists(
    /* [in] */ Int32 userId)
{
    if (userId == 0) {
        return TRUE;
    }
    AutoPtr<CUserManagerService> ums = GetUserManagerLocked();

    if (ums != NULL) {
       AutoPtr<IUserInfo> userInfo;
       ums->GetUserInfo(userId, (IUserInfo**)&userInfo);
       if(userInfo != NULL) return TRUE;
    }
    return FALSE;
}

AutoPtr< ArrayOf<Int32> > CActivityManagerService::GetUsersLocked()
{
    AutoPtr<CUserManagerService> ums = GetUserManagerLocked();

    if (ums != NULL) {
        return ums->GetUserIds();
    }
    AutoPtr< ArrayOf<Int32> > users = ArrayOf<Int32>::Alloc(1);
    users->Set(0, 0);
    return users;
}

AutoPtr<CUserManagerService> CActivityManagerService::GetUserManagerLocked()
{
   if (mUserManager == NULL) {
        AutoPtr<IInterface> iface = ServiceManager::GetService(IContext::USER_SERVICE);
        mUserManager = (CUserManagerService*)IIUserManager::Probe(iface);
   }
   return mUserManager;
}

Int32 CActivityManagerService::ApplyUserId(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId)
{
    return UserHandle::GetUid(userId, uid);
}

AutoPtr<IApplicationInfo> CActivityManagerService::GetAppInfoForUser(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Int32 userId)
{
    if (info == NULL) return NULL;
    AutoPtr<IApplicationInfo> newInfo;
    CApplicationInfo::New(info, (IApplicationInfo**)&newInfo);
    Int32 uid;
    info->GetUid(&uid);
    newInfo->SetUid(ApplyUserId(uid, userId));
    String infoPkgName;
    IPackageItemInfo::Probe(info)->GetPackageName(&infoPkgName);
    StringBuilder sb(USER_DATA_DIR);
    sb += userId;
    sb += "/";
    sb += infoPkgName;
    newInfo->SetDataDir(sb.ToString());
    return newInfo;
}

AutoPtr<IActivityInfo> CActivityManagerService::GetActivityInfoForUser(
    /* [in] */ IActivityInfo* aInfo,
    /* [in] */ Int32 userId)
{
    if (aInfo == NULL) {
        return aInfo;
    }
    else {
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);
        if (userId < 1 && uid < IUserHandle::PER_USER_RANGE) {
            return aInfo;
        }
    }

    AutoPtr<IActivityInfo> info;
    CActivityInfo::New(aInfo, (IActivityInfo**)&info);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IApplicationInfo> userAppInfo = GetAppInfoForUser(appInfo, userId);
    IComponentInfo::Probe(info)->SetApplicationInfo(userAppInfo);
    return info;
}

void CActivityManagerService::HandleShowErrorMsg(
    /* [in] */ StringObjectHashMap* data)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    Settings::Secure::GetInt32(resolver, ISettingsSecure::ANR_SHOW_BACKGROUND, 0, &value);
    Boolean showBackground = value != 0;
    {
        AutoLock lock(this);
        StringObjectHashMap::Iterator it = data->Find(String("app"));
        AutoPtr<ProcessRecord> proc;
        if (it != data->End()) {
            proc = (ProcessRecord*)IProcessRecord::Probe(it->mSecond);
        }

        it = data->Find(String("result"));
        AutoPtr<AppErrorResult> res;
        if (it != data->End()) {
            res = (AppErrorResult*)IObject::Probe(it->mSecond);
        }

        if (proc != NULL && proc->mCrashDialog != NULL) {
            Slogger::E(TAG, "App already has crash dialog: %p", proc.Get());
            if (res != NULL) {
                res->SetResult(0);
            }
            return;
        }
        assert(proc != NULL);
        Boolean isBackground = (UserHandle::GetAppId(proc->mUid)
                >= IProcess::FIRST_APPLICATION_UID && proc->mPid != MY_PID);
        for (Int32 i = 0; i < mCurrentProfileIds->GetLength(); i++) {
            isBackground &= (proc->mUserId != (*mCurrentProfileIds)[i]);
        }
        if (isBackground && !showBackground) {
            Slogger::W(TAG, "Skipping crash dialog of %s: background", TO_CSTR(proc));
            if (res != NULL) {
                res->SetResult(0);
            }
            return;
        }
        if (mShowDialogs && !mSleeping && !mShuttingDown) {
            AutoPtr<AppErrorDialog> errorDialog = new AppErrorDialog();
            errorDialog->constructor(GetUiContext(), this, res, proc);
            AutoPtr<IDialog> d = IDialog::Probe(errorDialog);
            d->Show();
            proc->mCrashDialog = d;
        }
        else {
            // The device is asleep, so just pretend that the user
            // saw a crash dialog and hit "force quit".
            if (res != NULL) {
                res->SetResult(0);
            }
        }
    }

    EnsureBootCompleted();
}

void CActivityManagerService::HandleShowNotRespondingMsg(
    /* [in] */ StringObjectHashMap* data,
    /* [in] */ Int32 arg1)
{
    {
        AutoLock lock(this);
        AutoPtr<ProcessRecord> proc;
        StringObjectHashMap::Iterator it = data->Find(String("app"));
        if (it != data->End()) {
            proc = (ProcessRecord*)IProcessRecord::Probe(it->mSecond);
        }

        if (proc != NULL && proc->mAnrDialog != NULL) {
            Slogger::E(TAG, "App already has anr dialog: %p", proc.Get());
            return;
        }

        AutoPtr<IIntent> intent;
        CIntent::New(String("android.intent.action.ANR"), (IIntent**)&intent);
        if (!mProcessesReady) {
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                | IIntent::FLAG_RECEIVER_FOREGROUND);
        }
        String nullStr;
        Int32 result;
        BroadcastIntentLocked(NULL, nullStr, intent,
            nullStr, NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE,
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, 0, &result);

        if (mShowDialogs) {
            AutoPtr<ActivityRecord> record;
            it = data->Find(String("activity"));
            if (it != data->End()) {
                record = (ActivityRecord*)IActivityRecord::Probe(it->mSecond);
            }
            AutoPtr<AppNotRespondingDialog> appDialog = new AppNotRespondingDialog();
            appDialog->constructor(this, GetUiContext(), proc, record, arg1 != 0);
            AutoPtr<IDialog> d = IDialog::Probe(appDialog);
            d->Show();
            proc->mAnrDialog = d;
        }
        else {
            // Just kill the app if there is no dialog to be shown.
            KillAppAtUsersRequest(proc, NULL);
        }
    }

    EnsureBootCompleted();
}

void CActivityManagerService::HandleShowStrictModeViolationMsg(
    /* [in] */ StringObjectHashMap* data)
{
    {
        AutoLock lock(this);
        StringObjectHashMap::Iterator it = data->Find(String("app"));
        if (it == data->End()) {
            Slogger::E(TAG, "App not found when showing strict mode dialog.");
            // break;
            return;
        }

        AutoPtr<ProcessRecord> proc = (ProcessRecord*)IProcessRecord::Probe(it->mSecond);
        assert(proc != NULL);
        if (proc->mCrashDialog != NULL) {
            Slogger::E(TAG, "App already has strict mode dialog: %p", proc.Get());
            return;
        }

        it = data->Find(String("result"));
        AutoPtr<AppErrorResult> res;
        if (it != data->End()) {
            res = (AppErrorResult*)IObject::Probe(it->mSecond);
        }

        if (mShowDialogs && !mSleeping && !mShuttingDown) {
            AutoPtr<StrictModeViolationDialog> dialog = new StrictModeViolationDialog();
            dialog->constructor(GetUiContext(), this, res, proc);
            AutoPtr<IDialog> d = IDialog::Probe(dialog);
            d->Show();
            proc->mCrashDialog = d;
        }
        else {
            // The device is asleep, so just pretend that the user
            // saw a crash dialog and hit "force quit".
            res->SetResult(0);
        }
    }
    EnsureBootCompleted();
}

void CActivityManagerService::HandleShowFactoryErrorMsg(
    /* [in] */ ICharSequence* msg)
{
    AutoPtr<FactoryErrorDialog> dialog = new FactoryErrorDialog();
    dialog->constructor(GetUiContext(), msg);
    AutoPtr<IDialog> d = IDialog::Probe(dialog);
    d->Show();
    EnsureBootCompleted();
}

void CActivityManagerService::HandleUpdateConfigurationMsg(
    /* [in] */ IConfiguration* config)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean hasPut;
    Settings::System::PutConfiguration(resolver, config, &hasPut);
}

void CActivityManagerService::HandleGCBackgroundProcessesMsg()
{
    AutoLock lock(this);
    PerformAppGcsIfAppropriateLocked();
}

void CActivityManagerService::HandleWaitForDebuggerMsg(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 arg1)
{
    AutoLock lock(this);
    if (arg1 != 0) {
        if (!app->mWaitedForDebugger) {
            AutoPtr<AppWaitingForDebuggerDialog> appDialog = new AppWaitingForDebuggerDialog();
            appDialog->constructor(this, GetUiContext(), app);
            AutoPtr<IDialog> d = IDialog::Probe(appDialog);
            app->mWaitDialog = d;
            app->mWaitedForDebugger = TRUE;
            d->Show();
        }
    }
    else {
        if (app->mWaitDialog != NULL) {
            IDialogInterface::Probe(app->mWaitDialog)->Dismiss();
            app->mWaitDialog = NULL;
        }
    }
}

void CActivityManagerService::HandleServiceTimeoutMsg(
    /* [in] */ ProcessRecord* processRecord)
{
    assert(processRecord != NULL);
    if (mDidDexOpt) {
        mDidDexOpt = FALSE;

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(SERVICE_TIMEOUT_MSG,
            processRecord->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessageDelayed(msg, ActiveServices::SERVICE_TIMEOUT, &result);
    }
    else {
        mServices->ServiceTimeout(processRecord);
    }
}

void CActivityManagerService::HandleUpdateTimeZone()
{
    AutoLock lock(this);
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            // try {
            r->mThread->UpdateTimeZone();
            // } catch (RemoteException ex) {
            //     Slogger::W(TAG, "Failed to update time zone for: " + r.info.processName);
            // }
        }
    }
}

void CActivityManagerService::HandleClearDnsCache()
{
    AutoLock lock(this);
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            // try {
            r->mThread->ClearDnsCache();
            // } catch (RemoteException ex) {
            //     Slogger::W(TAG, "Failed to clear dns cache for: " + r.info.processName);
            // }
        }
    }
}

void CActivityManagerService::HandleUpdateHttpProxy(
    /* [in] */ IProxyInfo* proxy)
{
    String host("");
    String port("");
    String exclList("");
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> pacFileUrl;
    uriHelper->GetEMPTY((IUri**)&pacFileUrl);
    if (proxy != NULL) {
        proxy->GetHost(&host);
        Int32 intPort;
        proxy->GetPort(&intPort);
        port = StringUtils::ToString(intPort);
        proxy->GetExclusionListAsString(&exclList);
        pacFileUrl = NULL;
        proxy->GetPacFileUrl((IUri**)&pacFileUrl);
    }

    AutoLock lock(this);
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            if (FAILED(r->mThread->SetHttpProxy(host, port, exclList, pacFileUrl))) {
                String processName;
                r->mInfo->GetProcessName(&processName);
                Slogger::W(TAG, "Failed to update http proxy for: %s", processName.string());
            }
        }
    }
}

void CActivityManagerService::HandleShowUidErrorMsg()
{
    String title("System UIDs Inconsistent");
    String text("UIDs on the system are inconsistent, you need to wipe your data partition or your device will be unstable.");
    Logger::E(TAG,  "%s: %s", title.string(), text.string());
    if (mShowDialogs) {
        // XXX This is a temporary dialog, no need to localize.
        AutoPtr<BaseErrorDialog> dialog = new BaseErrorDialog(GetUiContext());
        AutoPtr<IWindow> dWindow;
        dialog->GetWindow((IWindow**)&dWindow);
        dWindow->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
        dialog->SetCancelable(FALSE);
        AutoPtr<ICharSequence> cs;
        CString::New(title, (ICharSequence**)&cs);
        dialog->SetTitle(cs);
        cs = NULL;
        CString::New(text, (ICharSequence**)&cs);
        dialog->SetMessage(cs);
        AutoPtr<IDialogInterfaceOnClickListener> listener = new ErrorMsgButtonOnClickListener(this);
        cs = NULL;
        CString::New(String("I'm Feeling Lucky"), (ICharSequence**)&cs);
        dialog->SetButton(IDialogInterface::BUTTON_POSITIVE, cs, listener);
        mUidAlert = IAlertDialog::Probe(dialog);
        dialog->Show();
    }
}

void CActivityManagerService::HandleIMFeelingLuckyMsg()
{
    if (mUidAlert != NULL) {
        IDialogInterface::Probe(mUidAlert)->Dismiss();
        mUidAlert = NULL;
    }
}

void CActivityManagerService::HandleProcStartTimeoutMsg(
    /* [in] */ ProcessRecord* pr)
{
    if (mDidDexOpt) {
        mDidDexOpt = FALSE;

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(PROC_START_TIMEOUT_MSG,
            pr->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessageDelayed(msg, PROC_START_TIMEOUT, &result);
    }

    {
        AutoLock lock(this);
        ProcessStartTimedOutLocked(pr);
    }
}

void CActivityManagerService::HandleDoPendingActivityLaunchesMsg()
{
    AutoLock lock(this);
    mStackSupervisor->DoPendingActivityLaunchesLocked(TRUE);
}

void CActivityManagerService::HandleKillApplicationMsg(
    /* [in] */ Int32 appid,
    /* [in] */ Boolean restart,
    /* [in] */ IBundle* bundle)
{
    AutoLock lock(this);
    String pkg, reason;
    bundle->GetString(String("pkg"), &pkg);
    bundle->GetString(String("reason"), &reason);
    ForceStopPackageLocked(pkg, appid, restart, FALSE, TRUE, FALSE,
        FALSE, IUserHandle::USER_ALL, reason);
}

void CActivityManagerService::HandlePostHeavyNotificationMsg(
    /* [in] */ ActivityRecord* ar)
{
    AutoPtr<ActivityRecord> root = ar;
    AutoPtr<INotificationManagerHelper> nmHelper;
    CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&nmHelper);
    AutoPtr<IINotificationManager> inm;
    nmHelper->GetService((IINotificationManager**)&inm);
    if (inm == NULL) {
        return;
    }

    AutoPtr<ProcessRecord> process = root->mApp;
    if (process == NULL) {
        return;
    }

    String pInfoPkgName;
    IPackageItemInfo::Probe(process->mInfo)->GetPackageName(&pInfoPkgName);
    AutoPtr<IContext> context;
    if (FAILED(mContext->CreatePackageContext(pInfoPkgName, 0, (IContext**)&context))) {
        Slogger::W(TAG, "Unable to create context for heavy notification");
        return;
    }
    AutoPtr<IApplicationInfo> appInfo;
    context->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(appInfo)->LoadLabel(pkgManager, (ICharSequence**)&label);
    AutoPtr<ArrayOf<IInterface*> > labelArray = ArrayOf<IInterface*>::Alloc(1);
    labelArray->Set(0, label);
    String text;
    mContext->GetString(R::string::heavy_weight_notification, labelArray, &text);
    AutoPtr<ICharSequence> textCs;
    CString::New(text, (ICharSequence**)&textCs);
    AutoPtr<INotification> notification;
    CNotification::New((INotification**)&notification);
    notification->SetIcon(R::drawable::stat_sys_adb); //context.getApplicationInfo().icon;
    notification->SetWhen(0);
    notification->SetFlags(INotification::FLAG_ONGOING_EVENT);
    notification->SetTickerText(textCs);
    notification->SetDefaults(0); // please be quiet
    notification->SetSound(NULL);
    notification->SetVibrate(NULL);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 color;
    resources->GetColor(R::color::system_notification_accent_color, &color);
    notification->SetColor(color);
    AutoPtr<ICharSequence> ctext;
    mContext->GetText(R::string::heavy_weight_notification_detail, (ICharSequence**)&ctext);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IUserHandle> uHandle;
    CUserHandle::New(root->mUserId, (IUserHandle**)&uHandle);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivityAsUser(mContext, 0, root->mIntent,
        IPendingIntent::FLAG_CANCEL_CURRENT, NULL,
        uHandle, (IPendingIntent**)&pendingIntent);

    notification->SetLatestEventInfo(GetUiContext(), textCs, ctext, pendingIntent);

    AutoPtr<ArrayOf<Int32> > outId = ArrayOf<Int32>::Alloc(1);
    AutoPtr<ArrayOf<Int32> > outIdTmp;
    ECode ec = inm->EnqueueNotificationWithTag(String("android"), String("android"), String(NULL),
        R::string::heavy_weight_notification,
        notification, outId, root->mUserId, (ArrayOf<Int32>**)&outIdTmp);
    outId = outIdTmp;
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error showing notification for heavy-weight app");
    }
}

void CActivityManagerService::HandleCancelHeavyNotificationMsg(
    /* [in] */ Int32 id)
{
    AutoPtr<INotificationManagerHelper> nmHelper;
    CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&nmHelper);
    AutoPtr<IINotificationManager> inm;
    nmHelper->GetService((IINotificationManager**)&inm);
    if (inm == NULL) {
        return;
    }
    // try {
    inm->CancelNotificationWithTag(String("android"), String(NULL),
                R::string::heavy_weight_notification,  id);
    // } catch (RuntimeException e) {
    //     Slogger::W(ActivityManagerService.TAG,
    //             "Error canceling notification for service", e);
    // } catch (RemoteException e) {
    // }
}

void CActivityManagerService::HandleCheckExcessiveWakeLocksMsg()
{
    AutoLock lock(this);
    CheckExcessivePowerUsageLocked(TRUE);

    mHandler->RemoveMessages(CHECK_EXCESSIVE_WAKE_LOCKS_MSG);
    Boolean result;
    mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);
}

void CActivityManagerService::HandleShowCompatModeDialogMsg(
    /* [in] */ ActivityRecord* obj)
{
    AutoPtr<ActivityRecord> ar = obj;

    AutoLock lock(this);

    if (mCompatModeDialog != NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(ar->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String appInfoPkgName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&appInfoPkgName);
        String dialogPkgName;
        IPackageItemInfo::Probe(mCompatModeDialog->mAppInfo)->GetPackageName(&dialogPkgName);
        if (dialogPkgName.Equals(appInfoPkgName)) {
            return;
        }
        IDialogInterface::Probe(mCompatModeDialog)->Dismiss();
        mCompatModeDialog = NULL;
    }

    if (ar != NULL && FALSE) {
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(ar->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        if (mCompatModePackages->GetPackageAskCompatModeLocked(
                ar->mPackageName)) {
            Int32 mode = mCompatModePackages->ComputeCompatModeLocked(appInfo);
            if (mode == IActivityManager::COMPAT_MODE_DISABLED
                    || mode == IActivityManager::COMPAT_MODE_ENABLED) {
                mCompatModeDialog = new CompatModeDialog(
                        this, mContext, appInfo);
                mCompatModeDialog->Show();
            }
        }
    }
}

void CActivityManagerService::HandleReportMemUsageMsg(
    /* [in] */ IList* memInfos)
{
    AutoPtr<ReportMemUsageThread> thread = new ReportMemUsageThread(memInfos, this);
    thread->Start();
}

void CActivityManagerService::HandleCollectPssBgMsg()
{
    Int64 start = SystemClock::GetUptimeMillis();
    AutoPtr<IMemInfoReader> memInfo;
    {
        AutoLock syncLock(this);
        if (mFullPssPending) {
            mFullPssPending = FALSE;
            CMemInfoReader::New((IMemInfoReader**)&memInfo);
        }
    }
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    if (memInfo != NULL) {
        UpdateCpuStatsNow();
        Int64 nativeTotalPss = 0;
        {
            AutoLock syncLock(mProcessCpuTracker);
            Int32 N;
            mProcessCpuTracker->CountStats(&N);
            for (Int32 j = 0; j < N; j++) {
                AutoPtr<IProcessCpuTrackerStats> st;
                mProcessCpuTracker->GetStats(j, (IProcessCpuTrackerStats**)&st);
                Int64 vsize;
                st->GetVsize(&vsize);
                Int32 uid;
                if (vsize <= 0 || (st->GetUid(&uid), uid >= IProcess::FIRST_APPLICATION_UID)) {
                    // This is definitely an application process; skip it.
                    continue;
                }
                Int32 pid;
                st->GetPid(&pid);
                {
                    AutoLock lock(mPidsSelfLockedLock);
                    if (mPidsSelfLocked.Find(pid) != mPidsSelfLocked.End()) {
                        // This is one of our own processes; skip it.
                        continue;
                    }
                }
                Int64 pss = 0;
                dbg->GetPss(pid, NULL, &pss);
                nativeTotalPss += pss;
            }
        }
        memInfo->ReadMemInfo();
        {
            AutoLock syncLock(this);
            if (DEBUG_PSS) Slogger::D(TAG, "Collected native and kernel memory in %lldms",
                SystemClock::GetUptimeMillis() - start);
            Int64 cachedSizeKb, freeSizeKb, zramTotalSizeKb, buffersSizeKb, shmemSizeKb, slabSizeKb;
            memInfo->GetCachedSizeKb(&cachedSizeKb);
            memInfo->GetFreeSizeKb(&freeSizeKb);
            memInfo->GetZramTotalSizeKb(&zramTotalSizeKb);
            memInfo->GetBuffersSizeKb(&buffersSizeKb);
            memInfo->GetShmemSizeKb(&shmemSizeKb);
            memInfo->GetSlabSizeKb(&slabSizeKb);
            mProcessStats->AddSysMemUsageLocked(cachedSizeKb, freeSizeKb, zramTotalSizeKb,
                buffersSizeKb + shmemSizeKb + slabSizeKb, nativeTotalPss);
        }
    }

    Int32 i = 0, num = 0;
    AutoPtr<ArrayOf<Int64> > tmp;
    List<AutoPtr<ProcessRecord> >::Iterator it = mPendingPssProcesses.Begin();
    do {
        AutoPtr<ProcessRecord> proc;
        Int32 procState = 0;
        Int32 pid = 0;
        {
            AutoLock syncLock(this);
            if (it == mPendingPssProcesses.End()) {
                if (DEBUG_PSS) Slogger::D(TAG, "Collected PSS of %d of %d processes in %lldms",
                        num, i, SystemClock::GetUptimeMillis()-start);
                mPendingPssProcesses.Clear();
                return;
            }
            proc = *it;
            procState = proc->mPssProcState;
            if (proc->mThread != NULL && procState == proc->mSetProcState) {
                pid = proc->mPid;
            }
            else {
                proc = NULL;
                pid = 0;
            }
            i++;
            ++it;
        }
        if (proc != NULL) {
            Int64 pss = 0;
            tmp = NULL;
            dbg->GetPss(pid, (ArrayOf<Int64>**)&tmp, &pss);
            {
                AutoLock syncLock(this);
                if (proc->mThread != NULL && proc->mSetProcState == procState
                    && proc->mPid == pid) {
                    num++;
                    proc->mLastPssTime = SystemClock::GetUptimeMillis();
                    proc->mBaseProcessTracker->AddPss(pss, (*tmp)[0], TRUE, proc->mPkgList);
                    if (DEBUG_PSS) Slogger::D(TAG, "PSS of %s: %lld lastPss=%lld state=%s",
                        proc->ToShortString().string(), pss, proc->mLastPss,
                        ProcessList::MakeProcStateString(procState).string());
                    if (proc->mInitialIdlePss == 0) {
                        proc->mInitialIdlePss = pss;
                    }
                    proc->mLastPss = pss;
                    if (procState >= IActivityManager::PROCESS_STATE_HOME) {
                        proc->mLastCachedPss = pss;
                    }
                }
            }
        }
    } while (TRUE);
}

ECode CActivityManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CActivityManagerService";
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
