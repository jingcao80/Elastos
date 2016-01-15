
#include "elastos/droid/server/am/CActivityManagerService.h"
#if UN_FINISHED
namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {
const String CActivityManagerService::TAG("CActivityManagerService");

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#else

#include <elastos/utility/etl/Algorithm.h>
#include <unistd.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/Algorithm.h>
#include <Elastos.CoreLibrary.h>
#include <cutils/properties.h>
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SELinux.h"
#include "elastos/droid/os/Looper.h"
//#include "utils/EventLogTags.h"
//#include "utils/AutoStringArray.h"
// #include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/Context.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/view/LayoutInflater.h"
//#include "elastos/droid/view/WindowManagerPolicy.h"
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
#include "elastos/droid/server/am/PendingThumbnailsRecord.h"
#include "elastos/droid/server/am/LaunchWarningWindow.h"
#include "elastos/droid/server/am/ProcessList.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ProviderMap.h"
#include "elastos/droid/server/am/ReceiverList.h"
#include "elastos/droid/server/am/UserStartedState.h"
#include "elastos/droid/server/am/CUsageStatsService.h"
#include "elastos/droid/server/am/CShutdownReceiver.h"
#include "elastos/droid/server/am/CStoppingReceiver.h"
#include "elastos/droid/server/am/TaskRecord.h"
#include "elastos/droid/server/am/ActivityState.h"
//#include "elastos/droid/server/am/SystemServer.h"
//#include "elastos/droid/server/am/CServiceRestarter.h"
//#include "wm/CWindowManagerService.h"
#include "elastos/droid/server/am/AppWaitingForDebuggerDialog.h"
#include "elastos/droid/app/NotificationManager.h"
#include "elastos/droid/net/Uri.h"
#include "util/TimeUtils.h"
#include "AttributeCache.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <dlfcn.h>
#include "CSystemServer.h"

using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IPrintWriter;
using Elastos::IO::CPrintWriter;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;
using Elastos::Core::CObjectContainer;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CObjectStringMap;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger64;
using Elastos::Droid::System::IZygote;
using Elastos::Droid::System::CZygote;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::EIID_IIntentFilter;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IPermissionController;
using Elastos::Droid::App::CPermissionController;
using Elastos::Droid::App::CContextImpl;
using Elastos::Droid::App::IContextImpl;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerWaitResult;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::CActivityManagerMemoryInfo;
using Elastos::Droid::App::Backup::IIBackupManager;
// using Elastos::Droid::Net::IProxy;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Server::AttributeCache;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::Content::EIID_IIIntentSender;

using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebug;
using Libcore::IO::IOsConstants;
using Libcore::IO::COsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;

#define UNUSED(x) (void)x

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const char* ToChars(
    /* [in] */ IObject* obj)
{
    if (obj) {
        String str;
        obj->ToString(&str);
        return str.string();
    }
    return "NULL";
}

const char* ToChars(
    /* [in] */ IInterface* obj)
{
    if (IObject::Probe(obj)) {
        String str;
        IObject::Probe(obj)->ToString(&str);
        return str.string();
    }
    return "NULL";
}

static void DumpMemery(const char* filename)
{
    AutoPtr<IOsConstants> osConstans;
    COsConstants::AcquireSingleton((IOsConstants**)&osConstans);
    Int32 m1, m2, m3;
    osConstans->GetOsConstant(String("O_RDWR"), &m1);
    osConstans->GetOsConstant(String("O_CREAT"), &m2);
    osConstans->GetOsConstant(String("O_TRUNC"), &m3);

    AutoPtr<IFile> file;
    CFile::New(String("/data/debug"), (IFile**)&file);
    Boolean res;
    file->Mkdirs(&res);
    AutoPtr<IFileDescriptor> ifd;
    CFileDescriptor::New((IFileDescriptor**)&ifd);
    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    String path("/data/debug/");
    path.AppendFormat("%s.txt", filename);
    Int32 fd;
    ECode ec = ioBridge->Open(path, m1 | m2 | m3, &fd);
    if (FAILED(ec)) {
        ALOGE("Please run 'chmod 777 /data/debug' in adb shell.");
        return;
    }

    ifd->SetDescriptor(fd);

    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    dbg->DumpHeap(ifd);

    ALOGD(" >> dump memery to %s", path.string());
}

// static ECode GetInt32Prop(
//    /* [in] */ const String& name,
//    /* [in] */ Boolean allowZero,
//    /* [out] */ Int32* value)
// {
//     AutoPtr<ISystemProperties> sysProp;
//     CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
//     String str;
//     sysProp->Get(name, &str);
//    if (str.IsNull()) {
// //        throw new IllegalArgumentException("Property not defined: " + name);
//        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//    }
//    Int32 val = StringUtils::ParseInt32(str);
//    if (val == 0 && !allowZero) {
// //        throw new IllegalArgumentException("Property must not be zero: " + name);
//        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//    }
//    *value = val;
//    return NOERROR;
// }

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
        return (IInterface*)this;
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
    if (object == (IInterface*)this) {
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
        ProcessList::SYSTEM_ADJ, ProcessList::PERSISTENT_PROC_ADJ,
        ProcessList::FOREGROUND_APP_ADJ, ProcessList::VISIBLE_APP_ADJ,
        ProcessList::PERCEPTIBLE_APP_ADJ, ProcessList::HEAVY_WEIGHT_APP_ADJ,
        ProcessList::BACKUP_APP_ADJ, ProcessList::SERVICE_ADJ,
        ProcessList::HOME_APP_ADJ, ProcessList::PREVIOUS_APP_ADJ,
        ProcessList::SERVICE_B_ADJ, ProcessList::HIDDEN_APP_MAX_ADJ
    };

    Int32 size = ArraySize(DUMP_MEM_OOM_ADJ);
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(size);
    array->Copy(DUMP_MEM_OOM_ADJ, size);
    return array;
}

static AutoPtr< ArrayOf<String> > InitDumpMemOomLabel()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    (*array)[0] = String("System");
    (*array)[1] = String("Persistent");
    (*array)[2] = String("Foreground");
    (*array)[3] = String("Visible");
    (*array)[4] = String("Perceptible");
    (*array)[5] = String("Heavy Weight");
    (*array)[6] = String("Backup");
    (*array)[7] = String("A Services");
    (*array)[8] = String("Home");
    (*array)[9] = String("Previous");
    (*array)[10] = String("B Services");
    (*array)[11] = String("Background");
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

const String CALLED_PRE_BOOTS_FILENAME("called_pre_boots.dat");
const String TAG("ActivityManager");
const String TAG_MU("ActivityManagerServiceMU");
const Boolean CActivityManagerService::DEBUG = FALSE;
const Boolean CActivityManagerService::localLOGV = DEBUG;
const Boolean CActivityManagerService::DEBUG_BACKUP = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_BROADCAST = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_BROADCAST_LIGHT = DEBUG_BROADCAST || FALSE;
const Boolean CActivityManagerService::DEBUG_BACKGROUND_BROADCAST = DEBUG_BROADCAST || FALSE;
const Boolean CActivityManagerService::DEBUG_CLEANUP = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_CONFIGURATION = localLOGV || FALSE;
const Boolean CActivityManagerService::DEBUG_FOCUS = FALSE;
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
const Boolean CActivityManagerService::DEBUG_URI_PERMISSION = localLOGV || FALSE;
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

const Int32 CActivityManagerService::FIRST_ACTIVITY_STACK_MSG = 100;
const Int32 CActivityManagerService::FIRST_BROADCAST_QUEUE_MSG = 200;
const Int32 CActivityManagerService::FIRST_COMPAT_MODE_MSG = 300;
const Int32 CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG = 100;

const Int32 CActivityManagerService::LAST_DONE_VERSION = 10000;

const AutoPtr< ArrayOf<Int64> > CActivityManagerService::DUMP_MEM_BUCKETS = InitDumpMemBuckets();
const AutoPtr< ArrayOf<Int32> > CActivityManagerService::DUMP_MEM_OOM_ADJ = InitDumpMemOomAdj();
const AutoPtr< ArrayOf<String> > CActivityManagerService::DUMP_MEM_OOM_LABEL = InitDumpMemOomLabel();

const String CActivityManagerService::USER_DATA_DIR = String("/data/user/");
const String CActivityManagerService::SYSTEM_DEBUGGABLE = String("ro.debuggable");

const Int32 CActivityManagerService::MAX_DUP_SUPPRESSED_STACKS = 5000;

AutoPtr<IContext> CActivityManagerService::sSystemContext;
AutoPtr<CActivityManagerService> CActivityManagerService::mSelf;
AutoPtr<IActivityThread> CActivityManagerService::mSystemThread;

pthread_key_t CActivityManagerService::sCallerIdentity = InitSCallerIdentity();

const Int32 CActivityManagerService::COLLECT_PSS_BG_MSG = 1;

//==============================================================================
// CActivityManagerService::PendingActivityLaunch
//==============================================================================
CActivityManagerService::PendingActivityLaunch::PendingActivityLaunch()
    : mStartFlags(0)
{}

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

CAR_INTERFACE_IMPL(CActivityManagerService::ForegroundToken, IProxyDeathRecipient)

//==============================================================================
// GrantUri
//==============================================================================
GrantUri::GrantUri(
    /* [in] */ Int32 sourceUserId,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean prefix)
    : mSourceUserId(sourceUserId)
    , mUri(uri)
    , mPrefix(prefix)
{
}

ECode GrantUri::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = ToString().GetHashCode();
    return NOERROR;
}

ECode GrantUri::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Equals((GrantUri*)IObject::Probe(other));
    return NOERROR;
}

Boolean GrantUri::Equals(
    /* [in] */ GrantUri* other)
{
    if (other != NULL) {
        Boolean result;
        IObject::Probe(mUri)->Equals(other->mUri, &result);
        return result && (mSourceUserId == other->mSourceUserId)
                && mPrefix == other->mPrefix;
    }
    return FALSE;
}

ECode GrantUri::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    *info = ToString();
    return NOERROR;
}

String GrantUri::ToString()
{
    StringBuilder result;
    result += mSourceUserId;
    result += " @ ";
    result += mUri;
    if (mPrefix)
        result += " [mPrefix]";
    return result.ToString();
}

String GrantUri::ToSafeString()
{
    StringBuilder result;
    result += mSourceUserId;
    result += " @ ";
    String str;
    mUri->ToSafeString(&str);
    result += str;
    if (mPrefix)
        result += " [mPrefix]";
    return result.ToString();
}

AutoPtr<GrantUri> GrantUri::Resolve(
    /* [in] */ Int32 defaultSourceUserHandle,
    /* [in] */ IUri* uri)
{
    AutoPtr<IContentProviderHelper> cpHelper;
    CContentProviderHelper::AcquireSingleton((IContentProviderHelper**)&cpHelper);
    Int32 userId;
    cpHelper->GetUserIdFromUri(uri, defaultSourceUserHandle, &userId);
    AutoPtr<IUri> retUri;
    cpHelper->GetUriWithoutUserId(uri, (IUri**)&retUri);
    return new GrantUri(userId, retUri, FALSE);
}

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
            Int64 nextWriteDelay = (mHost->mLastWriteTime + BATTERY_STATS_TIME) - now;
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
// CActivityManagerService::ItemMatcher
//==============================================================================

CActivityManagerService::ItemMatcher::ItemMatcher()
    : mAll(TRUE)
{}

CActivityManagerService::ItemMatcher::~ItemMatcher()
{
    mComponents.Clear();
    mStrings.Clear();
    mObjects.Clear();
}

void CActivityManagerService::ItemMatcher::Build(
   /* [in] */ const String& name)
{
    AutoPtr<IComponentNameHelper> componentNameHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&componentNameHelper);
    AutoPtr<IComponentName> componentName;
    componentNameHelper->UnflattenFromString(name, (IComponentName**)&componentName);
    if (componentName != NULL) {
        mComponents.PushBack(componentName);
        mAll = FALSE;
    }
    else {
        Int32 objectId = 0;
        // Not a '/' separated full component name; maybe an object ID?
        //try {
        ECode ec = StringUtils::ParseInt32(name, 16, &objectId);
        if (SUCCEEDED(ec)) {
            mObjects.PushBack(objectId);
            mAll = FALSE;
        }
        else {
        //} catch (RuntimeException e) {
            // Not an integer; just do string match.
            mStrings.PushBack(name);
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
        if (CString("--").Equals(name)) {
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

    Int32 size = mComponents.GetSize();
    for (Int32 i = 0; i < size; i++) {
        Boolean equals;
        if (mComponents[i]->Equals(comp, &equals), equals) {
            return TRUE;
        }
    }

    // for (Int32 i=0; i<mObjects.GetSize(); i++) {
    //     assert(0);
    //     // if (System.identityHashCode(object) == mObjects[i]) {
    //     //     return TRUE;
    //     // }
    // }

    String flat;
    comp->FlattenToString(&flat);
    size = mStrings.GetSize();
    for (Int32 i = 0; i < size; i++) {
        if (flat.Contains(mStrings[i])) {
            return TRUE;
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
    /* [in] */ Int32 id)
    : mLabel(label)
    , mShortLabel(shortLabel)
    , mPss(pss)
    , mId(id)
{}

CActivityManagerService::MemItem::~MemItem()
{
    mSubitems = NULL;
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
    // IBinder target = filter.receiverList.receiver.asBinder();
    // for (Int32 i=dest.size()-1; i>=0; i--) {
    //     if (dest.get(i).receiverList.receiver.asBinder() == target) {
    //         return FALSE;
    //     }
    // }
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
    return ArrayOf<BroadcastFilter*>::Alloc(size);
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

    AutoLock lock(mOwner->mLock);
    return mOwner->AppDiedLocked(mApp, mPid, mAppThread);
}

CAR_INTERFACE_IMPL(CActivityManagerService::AppDeathRecipient, IProxyDeathRecipient)


ECode CActivityManagerService::SwitchUserIntentReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return mHost->UserInitialized(mUss, mUserId);
}

CAR_INTERFACE_IMPL(CActivityManagerService::SwitchUserIntentReceiver, IIntentReceiver);

CAR_INTERFACE_IMPL(CActivityManagerService::NeedStartIntentReceiver, IIntentReceiver);

//==============================================================================
// CActivityManagerService::ReportMemUsageThread
//==============================================================================

ECode CActivityManagerService::ReportMemUsageThread::ProcessMemInfoComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs)
{
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
    UpdateCpuStatsNow();
    AutoPtr<IDebug> debug;
    CDebug::AcquireSingleton((IDebug**)&debug);
    {
        AutoLock lock(mHost->mProcessCpuTracker);
        Int32 N;
        mProcessCpuTracker->CountStats(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IProcessCpuTrackerStats> st;
            mProcessCpuTracker->GetStats(i, (IProcessCpuTrackerStats**)&st);
            Int32 vsize;
            st->GetVsize(&vsize);
            if (vsize > 0) {
                Int32 pid;
                st->GetPid(&pid);
                Int64 pss;
                debug->GetPss(pid, NULL, &pss);
                if (pss > 0) {
                    if (infoMap.Find(pid) == infoMap.End()) {
                        String name;
                        st->GetName(&name);
                        AutoPtr<ProcessMemInfo> mi = new ProcessMemInfo(name, pid,
                            ProcessList::NATIVE_ADJ, -1, String("native"), String(NULL));
                        mi->mPss = pss;
                        memInfos->PushBack((IProcessMemInfo*)mi.Get());
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
    assert(0);
    // debug->GetMemInfo(infos);
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
    dropBuilder.Append(stack);
    dropBuilder.AppendChar('\n');
    dropBuilder.AppendChar('\n');
    dropBuilder.Append(logBuilder);
    dropBuilder.AppendChar('\n');
    /*
    dropBuilder.Append(oomString);
    dropBuilder.AppendChar('\n');
    */
    AutoPtr<IWriter> catSw;
    CStringWriter::New((IWriter**)&catSw);
    synchronized (mHost) {
        AutoPtr<IPrintWriter> catPw;
        CFastPrintWriter::New(catSw, FALSE, 256, (IPrintWriter**)&catPw);
        AutoPtr<ArrayOf<String> > emptyArgs = ArrayOf<String>::Alloc(0);
        catPw.Println();
        DumpProcessesLocked(NULL, catPw, emptyArgs, 0, FALSE, NULL);
        catPw.Println();
        mServices->DumpServicesLocked(NULL, catPw, emptyArgs, 0,
                FALSE, FALSE, NULL);
        catPw.Println();
        DumpActivitiesLocked(NULL, catPw, emptyArgs, 0, FALSE, FALSE, NULL);
        IFlushable::Probe(catPw)->Flush();
    }
    String str;
    IObject::Probe(catSw)->ToString(&str);
    dropBuilder.Append(str);
    AddErrorToDropBox(String("lowmem"), NULL, String("system_server"), NULL,
        NULL, tag.ToString(), dropBuilder.ToString(), NULL, NULL);
    //Slogger::I(TAG, "Sent to dropbox:");
    //Slogger::I(TAG, dropBuilder.toString());
    synchronized (mHost) {
        Int64 now = SystemClock::GetUptimeMillis();
        if (mLastMemUsageReportTime < now) {
            mLastMemUsageReportTime = now;
        }
    }
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
    AutoLock lock(mHost->mLock);
    mDialog->Dismiss();
    mHost->mLaunchWarningShown = FALSE;
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::ShowLaunchWarningLockedRunnable
//==============================================================================

ECode CActivityManagerService::ShowLaunchWarningLockedRunnable::Run()
{
    AutoLock lock(mHost->mLock);
    AutoPtr<LaunchWarningWindow> lwindow = new LaunchWarningWindow(mHost->mContext, mCur, mNext);
    AutoPtr<IDialog> d = IDialog::Probe(lwindow);
    d->Show();
    AutoPtr<IRunnable> runnable = new ShowRunnable(mHost, d);
    Boolean result;
    mHost->mHandler->PostDelayed(runnable, 4000, &result);
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::FinishBootingBroadcastReceiver
//==============================================================================

ECode CActivityManagerService::FinishBootingBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<ArrayOf<String> > pkgs;
    intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgs);
    if (pkgs != NULL) {
        for (Int32 i = 0; i < pkgs->GetLength(); ++i) {
            String pkg = (*pkgs)[i];
            {
                AutoLock lock(mHost->mLock);
                if (mHost->ForceStopPackageLocked(pkg, -1, FALSE, FALSE, FALSE, FALSE, 0)) {
                    SetResultCode(IActivity::RESULT_OK);
                    return NOERROR;
                }
            }
        }
    }
    return NOERROR;
}

//==============================================================================
// CActivityManagerService::SetProcessForegroundToken
//==============================================================================

ECode CActivityManagerService::SetProcessForegroundToken::ProxyDied()
{
    return mHost->ForegroundTokenDied(this);
}

// //==============================================================================
// // CActivityManagerService::FinisherReceiver
// //==============================================================================

// ECode CActivityManagerService::FinisherReceiver::PerformReceive(
//    /* [in] */ IIntent* intent,
//    /* [in] */ Int32 resultCode,
//    /* [in] */ const String& data,
//    /* [in] */ IBundle* extras,
//    /* [in] */ Boolean ordered,
//    /* [in] */ Boolean sticky,
//    /* [in] */ Int32 sendingUser)
// {
//     // The raw IIntentReceiver interface is called
//     // with the AM lock held, so redispatch to
//     // execute our code without the lock.
//     AutoPtr<IRunnable> runnable = new PerformReceiveRunnable(mHost, mReceivers, mCallback);
//     Boolean result;
//     mHost->mHandler->Post(runnable, &result);
//     return NOERROR;
// }

// CAR_INTERFACE_IMPL(CActivityManagerService::FinisherReceiver, IIntentReceiver)

// //==============================================================================
// // CActivityManagerService::PerformReceiveRunnable
// //==============================================================================

// ECode CActivityManagerService::PerformReceiveRunnable::Run()
// {
//     {
//         AutoLock lock(mHost->mLock);
//         mHost->mDidUpdate = TRUE;
//     }
//     mHost->WriteLastDonePreBootReceivers(mReceivers);
//     AutoPtr<ICharSequence> text;
//     mHost->mContext->GetText(
//             R::string::android_upgrading_complete, (ICharSequence**)&text);
//     mHost->ShowBootMessage(text, FALSE);
//     mHost->SystemReady(mCallback);
//     return NOERROR;
// }

// CAR_INTERFACE_IMPL(CActivityManagerService::PerformReceiveRunnable, IRunnable)

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

CAR_INTERFACE_IMPL(CActivityManagerService::ErrorMsgButtonOnClickListener, IDialogInterfaceOnClickListener)

// //==============================================================================
// // CActivityManagerService::SystemBroadcastReceiver
// //==============================================================================

// ECode CActivityManagerService::SystemBroadcastReceiver::PerformReceive(
//     /* [in] */ IIntent* intent,
//     /* [in] */ Int32 resultCode,
//     /* [in] */ const String& data,
//     /* [in] */ IBundle* extras,
//     /* [in] */ Boolean ordered,
//     /* [in] */ Boolean sticky,
//     /* [in] */ Int32 sendingUser)
// {
//     return NOERROR;
// }

// CAR_INTERFACE_IMPL(CActivityManagerService::SystemBroadcastReceiver, IIntentReceiver)

//==============================================================================
// CActivityManagerService::DropBoxTagThread
//==============================================================================

CActivityManagerService::DropBoxTagThread::DropBoxTagThread(
    /* [in] */ CActivityManagerService* host,
    /* [in] */ StringBuilder* sb,
    /* [in] */ Object* lock,
    /* [in] */ IDropBoxManager* dbox,
    /* [in] */ const String& tag)
    : mHost(host)
    , mSb(sb)
    , mSbLock(lock)
    , mDbox(dbox)
    , mTag(tag)
{
    Thread::constructor(String("Error dump: ") + tag);
}

ECode CActivityManagerService::DropBoxTagThread::Run()
{
    String report;
    {
        if (mSbLock != NULL) {
            mSbLock->Lock();
        }
        report = mSb->ToString();
        mSb->Delete(0, mSb->GetLength());
        mSb->TrimToSize();
        if (mSbLock != NULL) {
            mSbLock->Unlock();
        }
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
        AutoLock lock(mHost->mStrictModeBufferLock);
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
        FileUtils::ReadTextFile(mLogFile, 128 * 1024, String("\n\n[[TRUNCATED]]"), &str);
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
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 lines;
    global->GetInt32(resolver, setting, 0, &lines);
    if (lines > 0) {
        mSb->AppendCStr("\n");

        // Merge several logcat streams, and take the last N lines
        AutoPtr<IInputStreamReader> input;
        Slogger::E(TAG, "// TODO: WorkerThread::Run() %s", mSb->ToString().string());
    //TODO:
    //     try {
    //         java.lang.Process logcat = new ProcessBuilder("/system/bin/logcat",
    //                 "-v", "time", "-b", "events", "-b", "system", "-b", "main",
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

CAR_INTERFACE_IMPL(CActivityManagerService::StopUserLockedRunnable, IRunnable);

//==============================================================================
// CActivityManagerService::DispatchUserSwitchCallback
//==============================================================================

CActivityManagerService::DispatchUserSwitchCallback::SendResult(
    /* [in] */ IBundle* data)
{
    AutoLock lock(mHost->mLock);
    if (mHost->mCurUserSwitchCallback.Get() == this) {
        mCount++;
        if (mCount == N) {
            mHost->SendContinueUserSwitchLocked(mUss, mOldUserId, mNewUserId);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CActivityManagerService::DispatchUserSwitchCallback, IRemoteCallback)


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
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj)
            mHost->HandleWaitForDebuggerMsg(pr, arg1);
        }
        break;
        case CActivityManagerService::SERVICE_TIMEOUT_MSG: {
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj)
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
            AutoPtr<ProcessRecord> pr = (ProcessRecord*)IProcessRecord::Probe(obj)
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
                    Slogger::D(TAG, "Applying new update lock state '%d' for %s", nextState, ToChars(r));
                }
                if (nextState) {
                    mHost->mUpdateLock->Acquire();
                }
                else {
                    mHost->mUpdateLock->Release();
                }
            }
        }
        break;
        case PERSIST_URI_GRANTS_MSG: {
            mHost->WriteGrantedUriPermissions();
        }
        break;
        case REQUEST_ALL_PSS_MSG: {
            mHost->RequestPssAllProcsLocked(SystemClock::GetUptimeMillis(), TRUE, FALSE);
        }
        break;
        case START_PROFILES_MSG: {
            synchronized (mHost) {
                mHost->StartProfilesLocked();
            }
        }
        break;
        case UPDATE_TIME: {
            synchronized (mHost) {
                List< AutoPtr<ProcessRecord> >::Iterator it = mHost->mLruProcesses.Begin();
                for (; it != mHost->mLruProcesses.End(); ++it) {
                    AutoPtr<ProcessRecord> r = *it;
                    if (r->mThread != NULL) {
                        if (FAILED(r->mThread->UpdateTimePrefs(arg1 == 0 ? FALSE : TRUE))) {
                            String processName;
                            r->mInfo->GetProcessName(&processName)
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
            synchronized (mHost) {
                AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(IBinder::Probe(obj));
                if (r != NULL && r->mApp != NULL && r->mApp->mThread != NULL) {
                    r->mApp->mThread->ScheduleEnterAnimationComplete(r->mAppToken);
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
            AutoPtr<IInterface> service = ServiceManager::GetService("mount");
            AutoPtr<IIMountService> mountService = IIMountService::Probe(service);
            String tag;
            l->ToLanguageTag(&tag);
            Logger::D(TAG, "Storing locale %s for decryption UI", tag.string());
            if (FAILED(mountService->SetField(IStorageManager::SYSTEM_LOCALE_KEY, tag))) {
                Logger::E(TAG, "Error storing locale for decryption UI");
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
    synchronized (mHost) {
        List<AutoPtr<TaskRecord> >::ReverseIterator rit = mHost->mRecentTasks.RBegin();
        while (rit != mHost->mRecentTasks.REnd()) {
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
    synchronized (mHost) {
        List<AutoPtr<TaskRecord> >::Iterator it = mHost->mRecentTasks.Begin();
        for (it != mHost->mRecentTasks.End(); ++it) {
            AutoPtr<TaskRecord> tr = *it;
            if (tr->mUserId != eventUserId)
                continue;
            AutoPtr<Pair<AutoPtr<IIntent>, Int32> > pair =
                new Pair<AutoPtr<IIntent>, Int32>(tr->mIntent, tr->mTaskId)
            recentTaskIntents->Add(pair);
        }
    }
    // Check the recent tasks and filter out all tasks with components that no longer exist.
    List<AutoPtr<Pair<AutoPtr<IIntent>, Int32> > >::rIt = recentTaskIntents.Begin();
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
    synchronized (mHost) {
        List<Int32>::ReverseIterator rit = tasksToRemove.RBegin();
        for (; rit != tasksToRemove.REnd(); ++rit) {
            // Remove the task but don't kill the process (since other components in that
            // package may still be running and in the background)
            RemoveTaskByIdLocked(*rit, 0);
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
            synchronized (mHost) {
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
CActivityManagerService::Lifecycle::Lifecycle(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CActivityManagerService::NewByFriend(context, (CActivityManagerService**)&mService);
}

// @Override
ECode CActivityManagerService::Lifecycle::OnStart()
{
    mService->Start();
    return NOERROR;
}

AutoPtr<CActivityManagerService> CActivityManagerService::Lifecycle::GetService()
{
    return mService;
}

//==============================================================================
// CActivityManagerService
//==============================================================================

CActivityManagerService::CActivityManagerService()
    : mNextIsolatedProcessUid(0)
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
    , mCurTask(1)
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
    , mLastAddedTaskUid(0)
    , mLruProcessActivityStart(0)
    , mLruProcessServiceStart(0)
    , mLastFullPssTime(SystemClock::GetUptimeMillis())
    , mFullPssPending(FALSE)
    , mSleeping(FALSE)
    , mRunningVoice(FALSE)
    , mWentToSleep(FALSE)
    , mLockScreenShown(FALSE)
    , mCurResumedUid(-1)
{
    CAtomicInteger64::New(0, (IAtomicInteger64**)&mLastCpuTime);
    CAtomicBoolean::New(TRUE, (IAtomicBoolean**)&mProcessCpuMutexFree);
    mProcessList = new ProcessList();

    mStrictModeBuffer = new StringBuilder();
    mStringBuilder = new StringBuilder(256);
    mProcessNames = new ProcessMap< AutoPtr<ProcessRecord> >();
    mProcessCrashTimes = new ProcessMap<Int64>();
    mBadProcesses = new ProcessMap<AutoPtr<BadProcessInfo> >();
    mForegroundPackages = new ProcessMap<AutoPtr<List<ProcessRecord> > >();

    mStartedUserArray = ArrayOf<Int32>::Alloc(1);
    (*mStartedUserArray)[0] = 0;

    CRemoteCallbackList::New((IRemoteCallbackList**)&mUserSwitchObservers);

    CRemoteCallbackList::New((IRemoteCallbackList**)&mProcessObservers);

    CProcessCpuTracker::New(MONITOR_THREAD_CPU_USAGE, (IProcessCpuTracker**)&mProcessCpuTracker)

    mActiveProcessChanges = ArrayOf<ProcessChangeItem*>::Alloc(5);

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->GetInt32(String("ro.opengles.version"), IConfigurationInfo::GL_ES_VERSION_UNDEFINED, &GL_ES_VERSION);

    CConfiguration::New((IConfiguration**)&mConfiguration);

    mProcDeaths = ArrayOf<Int32>::Alloc(20);

    mCurrentUserArray = ArrayOf<Int32>::Alloc(1);
    (*mCurrentUserArray)[0] = 0;
    mBroadcastQueues = ArrayOf<BroadcastQueue*>::Alloc(2);

    mReceiverResolver = new ReceiverResolver();

    CUpdateLock::New(String("immersive"), (IUpdateLock**)&mUpdateLock);

    mCurrentProfileIds = ArrayOf<Int32>::Alloc(1);
    (*mCurrentProfileIds)[0] = IUserHandle::USER_OWNER;

    AutoPtr<IBackgroundThreadHelper> btHelper;
    CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&btHelper);
    AutoPtr<IHandler> handler;
    btHelper->GetHandler((IHandler**)&handler);
    looper = NULL;
    handler->GetLooper((ILooper**)&looper);
    mBgHandler = new BgHandler(looper, this);
    mPackageMonitor = new MyPackageMonitor(this);
}

CActivityManagerService::~CActivityManagerService()
{
    mPendingActivityLaunches.Clear();
    mRecentTasks.Clear();
    mIsolatedProcesses.Clear();
    mPidsSelfLocked.Clear();
    mForegroundProcesses.Clear();
    mProcessesOnHold.Clear();
    mStartingProcesses.Clear();
    mPersistentStartingProcesses.Clear();
    mRemovedProcesses.Clear();
    mLruProcesses.Clear();
    mProcessesToGc.Clear();
    mStartedUsers.Clear();
    mUserLru.Clear();
    //mPendingResultRecords.Clear();
    mIntentSenderRecords.Clear();
    mRegisteredReceivers.Clear();

    mStickyBroadcasts.Clear();

    mPendingThumbnails.Clear();
    mCancelledThumbnails.Clear();
    mLaunchingProviders.Clear();
    mAppBindArgs.Clear();
    mPendingProcessChanges.Clear();
    mAvailProcessChanges.Clear();
    mRequestPssList.Clear();
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
    Boolean mked;
    systemDir->Mkdirs(&mked);

    CBatteryStatsService::NewByFriend(systemDir, mHandler,
            (CBatteryStatsService**)&mBatteryStatsService);
    mBatteryStatsService->GetActiveStatistics()->ReadLocked();
    mBatteryStatsService->GetActiveStatistics()->WriteAsyncLocked();
    mOnBattery = DEBUG_POWER ? TRUE
        : mBatteryStatsService->GetActiveStatistics()->GetIsOnBattery();
    mBatteryStatsService->GetActiveStatistics()->SetCallback(this);

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
    mIntentFirewall = new IntentFirewall(ifInterface, mHandler);
    mStackSupervisor = new ActivityStackSupervisor(this);
    mTaskPersister = new TaskPersister(systemDir, mStackSupervisor);

    mProcessCpuThread = new ProcessCpuThread(this);

    mLockToAppRequest = new LockToAppRequestDialog(mContext, this);

    Watchdog::GetInstance()->AddMonitor(this);
    Watchdog::GetInstance()->AddThread(mHandler);

    return NOERROR;
}

AutoPtr<BroadcastQueue> CActivityManagerService::BroadcastQueueForIntent(
    /* [in] */ IIntent* intent)
{
    ELA_ASSERT_WITH_BLOCK(intent != NULL) {
        printf("CActivityManagerService::BroadcastQueueForIntent:"
            " IIntent parameter is NULL.");
    }

    Int32 flags = 0;
    intent->GetFlags(&flags);
    Boolean isFg = (flags & IIntent::FLAG_RECEIVER_FOREGROUND) != 0;
    if (DEBUG_BACKGROUND_BROADCAST) {
        Slogger::I(TAG, "Broadcast intent %p on %s queue.",
                intent, (isFg ? "foreground" : "background"));
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

ECode CActivityManagerService::GetSystemContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);

    if (sSystemContext == NULL) {
        FAIL_RETURN(CContextImpl::New((IContextImpl**)&sSystemContext));
    }
    *ctx = sSystemContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CActivityManagerService::SetSystemProcess()
{
//    try {
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm)
    sm->AddService(IContext::ACTIVITY_SERVICE, (IIActivityManager*)this, TRUE);
    sm->AddService(IProcessStats::SERVICE_NAME, (IIProcessStats*)mProcessStats.Get());
    // sm->AddService("meminfo", new MemBinder(this));
    // sm->AddService("gfxinfo", new GraphicsBinder(this));
    // sm->AddService("dbinfo", new DbBinder(this));
    // if (MONITOR_CPU_USAGE) {
    //     sm->AddService("cpuinfo", new CpuBinder(this));
    // }

    AutoPtr<IPermissionController> permissionController;
    CPermissionController::New((IIActivityManager*)m, (IPermissionController**)&permissionController);
    sm->AddService(String("permission"), permissionController.Get());

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> info;
    pm->GetApplicationInfo(String("android"), STOCK_PM_FLAGS, (IApplicationInfo**)&info);
    mSystemThread->InstallSystemApplicationInfo(info, NULL);

    {
        AutoLock lock(this);

        String processName;
        info->GetProcessName(&processName);
        AutoPtr<ProcessRecord> app = NewProcessRecordLocked(
                info, processName, FALSE, 0);
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

void CActivityManagerService::StartObservingNativeCrashes()
{
    AutoPtr<NativeCrashListener> ncl = new NativeCrashListener(this);
    ncl->Start();
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
//             pw.println("Permission Denial: can't dump meminfo from from pid="
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
//             pw.println("Permission Denial: can't dump gfxinfo from from pid="
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
//             pw.println("Permission Denial: can't dump dbinfo from from pid="
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
//             pw.println("Permission Denial: can't dump cpuinfo from from pid="
//                     + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                     + " without permission " + android.Manifest.permission.DUMP);
//             return;
//         }

//         synchronized (mActivityManagerService.mProcessCpuTracker) {
//             pw.print(mActivityManagerService.mProcessCpuTracker.printCurrentLoad());
//             pw.print(mActivityManagerService.mProcessCpuTracker.printCurrentState(
//                     SystemClock.uptimeMillis()));
//         }
//     }
// }

void CActivityManagerService::SetSystemServiceManager(
    /* [in] */ SystemServiceManager* mgr)
{
    mSystemServiceManager = mgr;
}

void CActivityManagerService::Start()
{
    Process::RemoveAllProcessGroups();
    mProcessCpuThread->Start();

    mBatteryStatsService->Publish(mContext);
    mAppOpsService->Publish(mContext);
    Slogger::D("AppOps", "AppOpsService published");
    AutoPtr<LocalService> localService = new LocalService(this);
    LocalServices::AddService(String("ActivityManagerInternal"), localService);
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
        if (sysProp->Get(String("events.cpu"), &value), value.Equals("true")) {
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
    synchronized (bstats) {
        {
            AutoLock lock(mPidsSelfLockedLock);
            if (haveNewCpuStats) {
                if (mOnBattery) {
                    Int32 perc = bstats->StartAddingCpuLocked();
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
                        Int32 otherUTime = (utime * perc) / 100;
                        st->GetRelStime(&stime);
                        Int32 otherSTime = (stime * perc) / 100;
                        totalUTime += otherUTime;
                        totalSTime += otherSTime;
                        if (pr != NULL) {
                            AutoPtr<IBatteryStatsUidProc> ps = pr->mCurProcBatteryStats;
                            Boolean isActive;
                            if (ps == NULL || !(ps->IsActive(&isActive), isActive)) {
                                Int32 uid;
                                pr->mInfo->GetUid(&uid);
                                ps = NULL;
                                bstats->GetProcessStatsLocked(uid, pr->mProcessName,
                                    (IBatteryStatsUidProc**)&ps);
                                pr->mCurProcBatteryStats = ps;
                            }
                            ps->AddCpuTimeLocked(utime-otherUTime, stime-otherSTime);
                            ps->AddSpeedStepTimes(cpuSpeedTimes);
                            pr->mCurCpuTime += (utime + stime) * 10;
                        }
                        else {
                            AutoPtr<IBatteryStatsUidProc> ps;
                            st->GetBatteryStats((IBatteryStatsUidProc**)&ps);
                            Boolean isActive;
                            if (ps == NULL || !(ps->IsActive(&isActive), isActive)) {
                                Int32 uid, mapUid;
                                st->GetUid(&uid);
                                bstats->MapUid(uid, &mapUid);
                                String name;
                                st->GetName(&name);
                                ps = NULL;
                                bstats->GetProcessStatsLocked(mapUid, name,
                                    (IBatteryStatsUidProc**)&ps);
                                st->SetBatteryStats(ps);
                            }
                            ps->AddCpuTimeLocked(utime-otherUTime, stime-otherSTime);
                            ps->AddSpeedStepTimes(cpuSpeedTimes);
                        }
                    }
                    bstats->FinishAddingCpuLocked(perc, totalUTime,
                            totalSTime, cpuSpeedTimes);
                }
            }
        }

        if (mLastWriteTime < (now - BATTERY_STATS_TIME)) {
            mLastWriteTime = now;
            AutoPtr<IBatteryStatsImpl> bsImpl = mBatteryStatsService->GetActiveStatistics();
            bsImpl->WriteAsyncLocked();
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

AutoPtr<IHashMap> CActivityManagerService::GetCommonServicesLocked()
{
    if (mAppBindArgs == NULL) {
        CHashMap::New((IHashMap**)&mAppBindArgs);

        // Setup the application init args
        AutoPtr<IInterface> keyPm = CoreUtils::Convert(String("package"));
        AutoPtr<IInterface> keyWm = CoreUtils::Convert(String("window"));
        AutoPtr<IInterface> keySm = CoreUtils::Convert(IContext::ALARM_SERVICE);
        AutoPtr<IServiceManager> sm;
        CServiceManager::AcquireSingleton((IServiceManager**)&sm);
        AutoPtr<IInterface> packageManager;
        sm->GetService(keyPm, (IInterface**)&packageManager);
        AutoPtr<IInterface> windowManager;
        sm->GetService(keyWm, (IInterface**)&windowManager);
        AutoPtr<IInterface> serviceManager;
        sm->GetService(keySm, (IInterface**)&serviceManager);
        mAppBindArgs->Put(keyPm, packageManager);
        mAppBindArgs->Put(keyWm, windowManager);
        mAppBindArgs->Put(keySm, serviceManager);
    }

    return mAppBindArgs;
}

ECode CActivityManagerService::SetFocusedActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    if (mFocusedActivity.Get() != r) {
        if (DEBUG_FOCUS) Slogger::D(TAG, "setFocusedActivityLocked: r=%s", ToChars(r));
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
    synchronized (this) {
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
    /* [in] */ IBinder token)
{
    if (DEBUG_VISBILITY) Slogger::D(TAG, "notifyActivityDrawn: token=%s", ToChars(token));
    synchronized (this) {
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
    msg->Obtain((IMessage**)&msg);
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
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.End(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }
    if (lrui < 0) {
        Slogger::W/*Wtf*/(TAG, "Adding dependent process %s not on LRU list: %s %s from %s",
            ToChars(app), what.string(), ToChars(obj), ToChars(srcApp));
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
        lrui, index, ToChars(app));
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
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.End(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }
    if (lrui >= 0) {
        if (!app->mKilled) {
            Slogger::W/*WtfStack*/(TAG, "Removing process that hasn't been killed: %s", ToChars(app));
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
    if (!activityChange && hasActivity) {
        // The process has activities, so we are only allowing activity-based adjustments
        // to move it.  It should be kept in the front of the list with other
        // processes that have activities, and we don't want those to change their
        // order except due to activity operations.
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
            if (DEBUG_LRU) Slogger::D(TAG, "Not moving, already top activity: %s", ToChars(app));
            return;
        }
    }
    else {
        if (mLruProcessServiceStart > 0
            && mLruProcesses[mLruProcessServiceStart-1].Get() == app) {
            if (DEBUG_LRU) Slogger::D(TAG, "Not moving, already top other: %s", ToChars(app));
            return;
        }
    }

    Int32 lrui = -1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.End(); ++rit, --i) {
        if (rit->Get() == app) {
            lrui = i;
            break;
        }
    }

    if (app->mPersistent && lrui >= 0) {
        // We don't care about the position of persistent processes, as long as
        // they are in the list.
        if (DEBUG_LRU) Slogger::D(TAG, "Not moving, persistent: %s", ToChars(app));
        return;
    }

    /* In progress: compute new position first, so we can avoid doing work
       if the process is not actually going to move.  Not yet working.
    Int32 addIndex;
    Int32 nextIndex;
    Boolean inActivity = false, inService = false;
    if (hasActivity) {
        // Process has activities, put it at the very tipsy-top.
        addIndex = mLruProcesses.GetSize();
        nextIndex = mLruProcessServiceStart;
        inActivity = true;
    }
    else if (hasService) {
        // Process has services, put it at the top of the service list.
        addIndex = mLruProcessActivityStart;
        nextIndex = mLruProcessServiceStart;
        inActivity = true;
        inService = true;
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
            + mLruProcessActivityStart + "): %s", ToChars(app));
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
            if (DEBUG_LRU) Slogger::D(TAG, "Adding to second-top of LRU activity list: %s", ToChars(app));
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
                            uid, i, ToChars(subProc), ToChars(nextSubProc));
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
            if (DEBUG_LRU) Slogger::D(TAG, "Adding to top of LRU activity list: %s", ToChars(app));
            mLruProcesses.PushBack(app);
        }
        nextIndex = mLruProcessServiceStart;
    }
    else if (hasService) {
        // Process has services, put it at the top of the service list.
        if (DEBUG_LRU) Slogger::D(TAG, "Adding to top of LRU service list: %s", ToChars(app));
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
            for (Int32 i = mLruProcesses.GetSize() - 1; rit != mLruProcesses.End(); ++rit, --i) {
                if (rit->Get() == client) {
                    clientIndex = i;
                    break;
                }
            }
            if (DEBUG_LRU && clientIndex < 0) Slogger::D(TAG, "Unknown client %s when updating %s",
                    ToChars(client), ToChars(app));
            if (clientIndex <= lrui) {
                // Don't allow the client index restriction to push it down farther in the
                // list than it already is.
                clientIndex = lrui;
            }
            if (clientIndex >= 0 && index > clientIndex) {
                index = clientIndex;
            }
        }
        if (DEBUG_LRU) Slogger::D(TAG, "Adding at %d of LRU list: %s", index, ToChars(app));
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
                    String("service connection"), cr, app);
        }
    }
    // for (Int32 j=app->mConProviders.size()-1; j>=0; j--) {
    List<AutoPtr<CContentProviderConnection> >::Iterator cpcIt;
    for (; cpcIt != app->mConProviders.End(); ++cpcIt) {
        AutoPtr<ContentProviderRecord> cpr = (*cpcIt)->mProvider;
        if (cpr->mProc != NULL && cpr->mProc->mLruSeq != mLruSeq && !cpr->mProc->mPersistent) {
            nextIndex = UpdateLruProcessInternalLocked(cpr->mProc, now, nextIndex,
                    String("provider reference"), cpr, app);
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
            AutoPtr<IUserHandleHelper> uhHelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
            Boolean isSameUser;
            uhHelper->IsSameUser(it2->mFirst, uid, &isSameUser);
            if (isSameUser)
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
        && mLastMemoryLevel > ProcessStats.ADJ_MEM_FACTOR_NORMAL
        && proc->mSetProcState >= IActivityManager::PROCESS_STATE_CACHED_EMPTY) {
        if (DEBUG_PSS) Slogger::D(TAG, "May not keep %s: pss=%lld", ToChars(proc), proc->mLastCachedPss);
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
        pm->PerformDexOptIfNeeded(packageName, NULL/* instruction set */, &result);
        if (result) {
            mDidDexOpt = TRUE;
        }
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

Boolean CActivityManagerService::IsNextTransitionForward()
{
    Int32 transit;
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
    synchronized(this) {
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
        info->SetPackageName(String("android"));
        AutoPtr<ProcessRecord> proc = StartProcessLocked(processName, info /* info */,
                FALSE /* knownToBeDead */, 0 /* intentFlags */, String("")  /* hostingType */,
                NULL /* hostingName */, TRUE /* allowWhileBooting */, TRUE /* isolated */,
                uid, TRUE /* keepIfLarge */, abiOverride, entryPoint, entryPointArgs,
                crashHandler);
        return proc != NULL ? proc->mPid : 0;
    }
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
        String appDes = app->ToString();
        Slogger::V(TAG, "startProcess: name=%s app=%s knownToBeDead=%d thread=%s pid=%d",
            processName.string(), ToChars(app), knownToBeDead,
            (app != NULL ? ToChars(app->mThread) : "NULL"), (app != NULL ? app->mPid : -1));
    }

    if (app != NULL && app->mPid > 0) {
        if (!knownToBeDead || app->mThread == NULL) {
            // We already have the app running, or are waiting for it to
            // come up (we have a pid but not yet its thread), so keep it.
            if (DEBUG_PROCESSES) {
                String appDes = app->ToString();
                Slogger::V(TAG, "App already running: %s", appDes.string());
            }
            // If this is a new package in the process, add the package to the list
            String packageName;
            info->GetPackageName(&packageName);
            Int32 versionCode;
            info->GetVersionCode(&versionCode);
            app->AddPackage(packageName, versionCode, mProcessStats);
            CheckTime(startTime, String("startProcess: done, added package to proc"));
            return app;
        }
        // An application record is attached to a previous process,
        // clean it up now.
        if (DEBUG_PROCESSES || DEBUG_CLEANUP)
            Slogger::V(TAG, "App died: %s", ToChars(app));
        CheckTime(startTime, String("startProcess: bad proc running, killing"));
        Int32 uid;
        app->mInfo->GetUid(&uid);
        Process::KillProcessGroup(uid, app->mPid);
        HandleAppDiedLocked(app, TRUE, TRUE);
        CheckTime(startTime, String("startProcess: done killing old proc"));
    }

    String hostingNameStr;
    if (hostingName != NULL) {
        hostingName->FlattenToShortString(&hostingNameStr);
    }

    String infoProcName;
    info->GetProcessName(&infoProcName);
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
//                EventLog.writeEvent(EventLogTags.AM_PROC_GOOD, UserHandleGetUserId(info.uid), info.uid,
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
        app = NewProcessRecordLocked( info, processName, isolated, isolatedUid);
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
        info->GetPackageName(&infoPackageName);
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
        if (Find(mProcessesOnHold.Begin(), mProcessesOnHold.End(), app) == mProcessesOnHold.End()) {
            mProcessesOnHold.PushBack(app);
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
    return StartProcessLocked(app, hostingType, hostingNameStr, String(NULL) /* abiOverride */,
            String(NULL) /* entryPoint */, NULL /* entryPointArgs */);
}

ECode CActivityManagerService::StartProcessLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& hostingType,
    /* [in] */ const String& hostingNameStr,
    /* [in] */ const String& abiOverride,
    /* [in] */ const String& entryPoint,
    /* [in] */ ArrayOf<String>* entryPointArgs)
{
    Int64 startTime = SystemClock::GetElapsedRealtime();
    if (app->mPid > 0 && app->mPid != MY_PID) {
        CheckTime(startTime, String("startProcess: removing from pids map"));
        {
            AutoLock lock(mPidsSelfLockedLock);
            mPidsSelfLocked.Erase(app->mPid);
            mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG,
                app ? app->Probe(EIID_IInterface) : NULL);
        }
        CheckTime(startTime, String("startProcess: done removing from pids map"));
        app->SetPid(0);
    }

    AutoPtr<ProcessRecord> temp = app;
    if (DEBUG_PROCESSES &&
            Find(mProcessesOnHold.Begin(), mProcessesOnHold.End(), temp) != mProcessesOnHold.End()) {
        String appDes = app->ToString();
        Slogger::V(TAG, "startProcessLocked removing on hold: ", appDes.string());
    }
    mProcessesOnHold.Remove(app);

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
            app->mInfo->GetPackageName(&packageName);
            pm->GetPackageGids(packageName, (ArrayOf<Int32>**)&permGids);

            AutoPtr<IEnvironment> env;
            CEnvironment::AcquireSingleton((IEnvironment**)&env);
            Boolean isEmulated;
            if (env->IsExternalStorageEmulated(&isEmulated), isEmulated) {
                CheckTime(startTime, String("startProcess: checking external storage perm"));
                Int32 res;
                pm->CheckPermission(
                        Elastos::Droid::Manifest::permission::ACCESS_ALL_EXTERNAL_STORAGE,
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
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        Int32 appGid, userGid;
        uhHelper->GetSharedAppGid(UserHandleGetAppId(uid), &appGid);
        uhHelper->getUserGid(UserHandleGetUserId(uid), &userGid);
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

    String requiredAbi;
    if (abiOverride != NULL)
        requiredAbi = abiOverride;
    else
        app->mInfo->GetPrimaryCpuAbi(&requiredAbi);

    if (requiredAbi == NULL) {
        requiredAbi = (*Build::SUPPORTED_ABIS)[0];
    }

    String instructionSet;
    // if (app.info.primaryCpuAbi != NULL) {
    //     instructionSet = VMRuntime.getInstructionSet(app.info.primaryCpuAbi);
    // }

    // Start the process.  It will either succeed and return a result containing
    // the PID of the new process, or else throw a RuntimeException.
    Boolean isActivityProcess = (entryPoint == NULL);
    if (entryPoint == NULL)
        entryPoint = "android.app.ActivityThread";
    CheckTime(startTime, String("startProcess: asking zygote to start proc"));

    String sourcePath, dataDir;
    app->mInfo->GetSourceDir(&sourcePath);
    app->mInfo->GetDataDir(&dataDir);
    Int32 targetSdkVersion;
    app->mInfo->GetTargetSdkVersion(&targetSdkVersion);
    String seinfo;
    app->mInfo->GetSeinfo(&seinfo);
    AutoPtr<IProcessStartResult> startResult;
    Process::Start(sourcePath.EndWith(".apk") ? entryPoint : String("CActivityThreadHelper"),
        app->mProcessName, uid, uid, gids, debugFlags, mountExternal,
        targetSdkVersion, seinfo, requiredAbi, instructionSet, dataDir,
        entryPointArgs, (IProcessStartResult**)&startResult);
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
//    EventLog.writeEvent(EventLogTags.AM_PROC_START, UserHandleGetUserId(uid), startResult.pid, uid,
//            app.processName, hostingType,
//            hostingNameStr != NULL ? hostingNameStr : "");

//    if (app.persistent) {
//        Watchdog.getInstance().processStarted(app.processName, startResult.pid);
//    }

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
    buf->Append(String(" for "));
    buf->Append(hostingType);
    if (!hostingNameStr.IsNull()) {
        buf->Append(String(" "));
        buf->Append(hostingNameStr);
    }
    buf->Append(String(": pid="));
    buf->Append(pid);
    buf->Append(String(" uid="));
    buf->Append(uid);
    buf->Append(String(" gids={"));
    if (gids != NULL) {
        for (Int32 gi = 0; gi < gids->GetLength(); gi++) {
            if (gi != 0) buf->Append(String(", "));
            buf->Append((*gids)[gi]);
        }
    }
    buf->Append(String("}"));
    if (requiredAbi != NULL) {
        buf->Append(" abi=");
        buf->Append(requiredAbi);
    }
    Slogger::I(TAG, buf->ToString());
    app->SetPid(pid);
    Boolean usingWrapper;
    startResult->GetUsingWrapper(&usingWrapper);
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
            msg->SetObj(app->Probe(EIID_IInterface));
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
        Slogger::D(TAG, "updateUsageStats: comp=%s res=%d", ToChars(component), resumed);
    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    if (resumed) {
        if (mUsageStatsService != NULL) {
            mUsageStatsService->ReportEvent(component->mRealActivity, component->mUserId,
                    IUsageEvents::MOVE_TO_FOREGROUND);
        }
        synchronized (stats) {
            stats->NoteActivityResumedLocked(component->mApp->mUid);
        }
    }
    else {
        if (mUsageStatsService != NULL) {
            mUsageStatsService->ReportEvent(component->mRealActivity, component->mUserId,
                    IUsageEvents::MOVE_TO_BACKGROUND);
        }
        synchronized (stats) {
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
        uriHelper->Parse(mTopData, &uri);
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
    if (mFactoryTest == FactoryTest::FACTORY_TEST_LOW_LEVEL
            && mTopAction.IsNull()) {
        // We are running in factory test mode, but unable to find
        // the factory test app, so just sit around displaying the
        // error message and don't try to start anything.
        return FALSE;
    }

    AutoPtr<IIntent> intent = GetHomeIntent();
    AutoPtr<IActivityInfo> aInfo = ResolveActivityInfo(intent, STOCK_PM_FLAGS, userId);
    if (aInfo != NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);

        String cname, name, pname;
        appInfo->GetPackageName(&cname);
        aInfo->GetName(&name);
        aInfo->GetProcessName(&pname);
        AutoPtr<IComponentName> component;
        CComponentName::New(cname, name, (IComponentName**)&component);
        intent->SetComponent((IComponentName*)component);

        // Don't do this if the home app is currently being
        // instrumented.
        AutoPtr<IActivityInfo> newActivityInfo;
        CActivityInfo::New(aInfo, (IActivityInfo**)&newActivityInfo);
        AutoPtr<IApplicationInfo> newAppInfo = GetAppInfoForUser(appInfo, userId);
        newActivityInfo->SetApplicationInfo(newAppInfo);
        Int32 uid;
        newAppInfo->GetUid(&uid);
        newActivityInfo->GetProcessName(&pname);
        AutoPtr<ProcessRecord> app = GetProcessRecordLocked(pname, uid, TRUE);
        if (app == NULL || app->mInstrumentationClass == NULL) {
            Int32 flags, status;
            String nullStr;
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
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Int32 value;
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);
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
            aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                ri = rInfo;
                break;
            }
        }

        if (ri != NULL) {
            AutoPtr<IActivityInfo> aInfo;
            ri->GetActivityInfo((IActivityInfo**)&aInfo);
            AutoPtr<IBundle> metaData;
            aInfo->GetMetaData((IBundle**)&metaData);
            String vers;
            if (metaData != NULL) {
                metaData->GetString(IIntent::METADATA_SETUP_VERSION, &vers);
            }else {
                vers = String(NULL);
            }
            AutoPtr<IApplicationInfo> appInfo;
            aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            AutoPtr<IBundle> appMetaData;
            appInfo->GetMetaData((IBundle**)&appMetaData);
            if (vers.IsNull() && appMetaData != NULL) {
                appMetaData->GetString(
                       IIntent::METADATA_SETUP_VERSION, &vers);
            }
            AutoPtr<ISettingsSecure> settingsSecure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
            String lastVers;
            settingsSecure->GetString(
                    resolver, ISettingsSecure::LAST_SETUP_SHOWN, &lastVers);
            if (!vers.IsNull() && !vers.Equals(lastVers)) {
                intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                String pkgName, name;
                aInfo->GetPackageName(&pkgName);
                aInfo->GetName(&name);
                AutoPtr<IComponentName> component;
                CComponentName::New(pkgName, name, (IComponentName**)&component);
                intent->SetComponent(component);
                Int32 status;
                mStackSupervisor->StartActivityLocked(NULL, intent, String(NULL), aInfo,
                   NULL, NULL, NULL, String(NULL), 0, 0, 0, String(NULL), 0, 0, 0, NULL,
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
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Boolean isIsolated;
    uhHelper->IsIsolated(Binder::GetCallingUid(), &isIsolated);
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
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_SCREEN_COMPATIBILITY,
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
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_SCREEN_COMPATIBILITY,
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
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_SCREEN_COMPATIBILITY,
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
        AutoPtr<IProcessObserver> observer = IProcessObserver::Probe(bitem);
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
        AutoPtr<IProcessObserver> observer = IProcessObserver::Probe(item);
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
    /* [in] */ const String& profilerInfo,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    return StartActivityAsUser(caller, callingPackage, intent, resolvedType, resultTo,
        resultWho, requestCode, startFlags, profilerInfo, options, UserHandleGetCallingUserId(), result);
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
    /* [in] */ const String& profilerInfo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
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
    /* [in] */ const String& profilerInfo,
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
    Int32 targetUid;
    String targetPackage;
    synchronized (this) {
        if (resultTo == NULL) {
            Slogger::E(TAG, "Must be called from an activity");
            return E_SECURITY_EXCEPTION;
        }
        sourceRecord = mStackSupervisor->IsInAnyStackLocked(resultTo);
        if (sourceRecord == NULL) {
            Slogger::E(TAG, "Called with bad activity token: %s", ToChars(resultTo));
            return E_SECURITY_EXCEPTION;
        }
        String packageName;
        sourceRecord->mInfo->GetPackageName(&packageName);
        if (!packageName.Equals("android")) {
            Slogger::E(TAG,
                    "Must be called from an activity that is declared in the android package");
            return E_SECURITY_EXCEPTION;
        }
        if (sourceRecord->mApp == NULL) {
            Slogger::E(TAG, "Called without a process attached to activity");
            return E_SECURITY_EXCEPTION;
        }
        if (UserHandleGetAppId(sourceRecord->mApp->mUid) != IProcess::SYSTEM_UID) {
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
        userId = UserHandleGetUserId(sourceRecord->mApp->mUid);
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
    /* [in] */ const String& profilerInfo,
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
            stack->mResumedActivity->mInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
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
        msg += Binder::GetCallingUid()
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
        ALLOW_FULL_ONLY, String("startVoiceActivity"), String(NULL), userId, &userId));
    // TODO: Switch to user app stacks here.
    return mStackSupervisor->StartActivityMayWait(NULL, callingUid, callingPackage, intent,
        resolvedType, session, interactor, NULL, String(NULL), 0, startFlags, profilerInfo, NULL,
        NULL, options, userId, NULL, NULL);
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
            UserHandleGetCallingUserId(), (IList**)&resolves);

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
            aInfo->GetPackageName(&cName);
            aInfo->GetName(&name);
            r->mInfo->GetName(&rName);
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
        aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String appCName, aName;
        appInfo->GetPackageName(&appCName);
        aInfo->GetName(&aName);
        AutoPtr<IComponentName> component;
        CComponentName::New(appCName, aName,(IComponentName**)&component);
        newIntent->SetComponent(component);

        Int32 flags;
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
    Int32 callingUid;
    String callingPackage;
    AutoPtr<IIntent> intent;
    Int32 userId;
    synchronized (this) {
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
    return StartActivityInPackage(callingUid, callingPackage, intent, NULL, NULL, NULL, 0, 0,
            options, userId, NULL, task, result);
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
    /* [in] */ IActivityContainer* container,
    /* [in] */ TaskRecord* inTask
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
    return mStackSupervisor->StartActivities(NULL, uid, callerPackage, intents, resolvedTypes,
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

    List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks.RBegin();
    while (rit != mRecentTasks.REnd()) {
        AutoPtr<TaskRecord> tr = *rit;
        if (tr->mUserId == userId) {
            if(DEBUG_TASKS) Slogger::I(TAG, "remove RecentTask %s when finishing user %d",
                ToChars(tr), userId);
            List<AutoPtr<TaskRecord> >::ReverseIterator tmpIt(rit);
            mRecentTasks.Erase(--tmpIt.GetBase());
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
    List<AutoPtr<TaskRecord> >::Iterator startIt = mRecentTasks.Begin();
    for (Int32 i = 0; i < start, i++)
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
    List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks.RBegin();
    while (rit != mRecentTasks.REnd()) {
        AutoPtr<TaskRecord> task = *rit;
        if (task->mAffiliatedTaskId == affiliateId) {
            List<AutoPtr<TaskRecord> >::ReverseIterator tmpIt(rit);
            mRecentTasks.Erase(--tmpIt.GetBase());
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
        Slogger::W(TAG, "Link error 1 first->next=%s", ToChars(first->mNextAffiliate));
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
                ToChars(next), ToChars(next->mPrevAffiliate), ToChars(prev));
            next->SetPrevAffiliate(prev);
            mTaskPersister->Wakeup(next, FALSE);
        }
        if (prev->mNextAffiliate != next) {
            Slogger::W(TAG, "Link error 3 prev=%s next=%s setting next=",
                ToChars(prev), ToChars(prev->mPrevAffiliate), ToChars(next));
            prev->SetNextAffiliate(next);
            mTaskPersister->Wakeup(prev, FALSE);
        }
        prev->mInRecents = TRUE;
    }
    // The last one is the beginning of the list and has no prev.
    AutoPtr<TaskRecord> last = mTmpRecents.GetBack();
    if (last.mPrevAffiliate != NULL) {
        Slogger::W(TAG, "Link error 4 last.prev=%s", ToChars(last->mPrevAffiliate));
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

    Int32 N = mRecentTasks.GetSize();

    AutoPtr<ArrayOf<Int32> > users = userId == IUserHandle::USER_ALL
            ? GetUsersLocked() : ArrayOf<Int32>::Alloc(1);
    if (userId != IUserHandle::USER_ALL)
        (*users)[0] = userId;

    for (Int32 ui = 0; ui < users->GetLength(); ui++) {
        Int32 user = (*users)[i];
        List<AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
        while (it != mRecentTasks.End()) {
            AutoPtr<TaskRecord> task = *it;
            if (task->mUserId != user) {
                // Only look at tasks for the user ID of interest.
                ++it;
                continue;
            }
            if (task->mAutoRemoveRecents && task->GetTopActivity() == NULL) {
                // This situation is broken, and we should just get rid of it now.
                it = mRecentTasks.Erase(it);
                task->RemovedFromRecents(mTaskPersister);
                Slogger::W(TAG, "Removing auto-remove without activity: %s", ToChars(task));
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
                        it = mRecentTasks.Erase(it);
                        task->RemovedFromRecents(mTaskPersister);
                        Slogger::W(TAG, "Removing no longer valid recent: %s", ToChars(task));
                        continue;
                    }
                    else {
                        // Otherwise just not available for now.
                        if (task->mIsAvailable) {
                            if (DEBUG_RECENTS) Slogger::D(TAG, "Making recent unavailable: %s",
                                ToChars(task));
                        }
                        task->mIsAvailable = FALSE;
                    }
                }
                else {
                    Boolean enabled;
                    ai->GetEnabled(&enabled);
                    AutoPtr<IApplicationInfo> appInfo;
                    ai->GetApplicationInfo((IApplicationInfo**)&appInfo);
                    Boolean aEnabled;
                    appInfo->GetEnabled(&aEnabled);
                    Int32 flags;
                    appInfo->GetFlags(&flags);
                    if (!enabled || !aEnabled || (flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
                        if (task->mIsAvailable) {
                            if (DEBUG_RECENTS)
                                Slogger::D(TAG, "Making recent unavailable: %s (enabled=%d/%d flags=0x%08x)",
                                    ToChars(task), enabled, aEnabled, flags);
                        }
                        task->mIsAvailable = FALSE;
                    }
                    else {
                        if (!task->mIsAvailable) {
                            if (DEBUG_RECENTS) Slogger::D(TAG, "Making recent available: %s",
                                    ToChars(task));
                        }
                        task->mIsAvailable = TRUE;
                    }
                }
            }
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
    Int32 N = mRecentTasks.GetSize();
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
    List<AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
    for (Int32 i = 0; i < endIndex; i++)
        ++it;
    AutoPtr<TaskRecord> prev = top;
    while (endIndex < N) {
        AutoPtr<TaskRecord> cur = *it;
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: looking at next chain @%d %s",
                endIndex, ToChars(cur));
        if (cur == top) {
            // Verify start of the chain.
            if (cur->mNextAffiliate != NULL || cur->mNextAffiliateTaskId != -1) {
                Slogger::Wtf(TAG, "Bad chain @%d: first task has next affiliate: %s",
                    endIndex, ToChars(prev));
                sane = FALSE;
                break;
            }
        }
        else {
            // Verify middle of the chain's next points back to the one before.
            if (cur->mNextAffiliate != prev
                    || cur->mNextAffiliateTaskId != prev->taskId) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d middle task %s @%d has bad next"
                    " affiliate %s id %d, expected %s", endIndex, ToChars(cur), endIndex,
                    ToChars(cur->mNextAffiliate), cur->mNextAffiliateTaskId, ToChars(prev));
                sane = FALSE;
                break;
            }
        }
        if (cur->mPrevAffiliateTaskId == -1) {
            // Chain ends here.
            if (cur->mPrevAffiliate != NULL) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d: last task %s has previous affiliate %s",
                        endIndex, ToChars(cur), ToChars(cur->mPrevAffiliate));
                sane = FALSE;
            }
            if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: end of chain @%d", endIndex);
            break;
        }
        else {
            // Verify middle of the chain's prev points to a valid item.
            if (cur->mPrevAffiliate == NULL) {
                Slogger::W/*Wtf*/(TAG, "Bad chain @%d: task %s has previous affiliate NULL but should be id %d"
                    endIndex, ToChars(cur), cur->mPrevAffiliateTaskId);
                sane = FALSE;
                break;
            }
        }
        if (cur->mAffiliatedTaskId != task->mAffiliatedTaskId) {
            Slogger::W/*Wtf*/(TAG, "Bad chain @%d: task %s has affiliated id %d but should be %d",
                    endIndex, ToChars(cur), cur->mAffiliatedTaskId, task->mAffiliatedTaskId);
            sane = FALSE;
            break;
        }
        prev = cur;
        endIndex++;
        ++it;
        if (endIndex >= N) {
            Slogger::W/*Wtf*/(TAG, "Bad chain ran off index %d: last task %s",
                endIndex, ToChars(prev));
            sane = FALSE;
            break;
        }
    }
    if (sane) {
        if (endIndex < taskIndex) {
            Slogger::W/*Wtf*/(TAG, "Bad chain @%d: did not extend to task %s @%d",
                endIndex, ToChars(task), taskIndex);
            sane = FALSE;
        }
    }
    if (sane) {
        // All looks good, we can just move all of the affiliated tasks
        // to the top.
        List<AutoPtr<TaskRecord> >::Iterator topIt = mRecentTasks.Begin();
        List<AutoPtr<TaskRecord> >::Iterator endIt = mRecentTasks.Begin();
        for (Int32 i = 0; i < topIndex; i++)
            ++topIt;
        for (Int32 i = 0; i <= endIndex; i++)
            ++endIt;
        mRecentTasks.Splice(mRecentTasks.Begin(), mRecentTasks, topIt, endIt)
        // for (Int32 i = topIndex; i <= endIndex; i++) {
        //     if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: moving affiliated " + task
        //             + " from " + i + " to " + (i-topIndex));
        //     TaskRecord cur = mRecentTasks.remove(i);
        //     mRecentTasks.add(i-topIndex, cur);
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
    Boolean isAffiliated = task->mAffiliatedTaskId != task->mTaskId
            || task->mNextAffiliateTaskId != -1 || task->mPrevAffiliateTaskId != -1;

    Int32 N = mRecentTasks.GetSize();
    // Quick case: check if the top-most recent task is the same.
    if (!isAffiliated && N > 0 && mRecentTasks.Begin()->Get() == task) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: already at top: %s", ToChars(task));
        return NOERROR;
    }
    // Another quick case: check if this is part of a set of affiliated
    // tasks that are at the top.
    if (isAffiliated && N > 0 && task->mInRecents
        && task->mAffiliatedTaskId == (*mRecentTasks.Begin())->mAffiliatedTaskId) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: affiliated %s at top when adding %s",
            ToChars(*mRecentTasks.Begin()), ToChars(task));
        return;
    }
    // Another quick case: never add voice sessions.
    if (task->mVoiceSession != NULL) {
        if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: not adding voice interaction %s", ToChars(task));
        return NOERROR;
    }

    Boolean needAffiliationFix = FALSE;

    // Slightly less quick case: the task is already in recents, so all we need
    // to do is move it.
    if (task->mInRecents) {
        Int32 taskIndex = -1;
        List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
        for (Int32 i = 0; it != mRecentTasks.End(); ++it, i++) {
            if (it->Get() == task) {
                taskIndex = i;
                break;
            }
        }
        if (taskIndex >= 0) {
            if (!isAffiliated) {
                // Simple case: this is not an affiliated task, so we just move it to the front.
                mRecentTasks.Erase(it);
                mRecentTasks.Insert(mRecentTasks.Begin(), task);
                NotifyTaskPersisterLocked(task, FALSE);
                if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: moving to top %s from %d",
                    ToChars(task), taskIndex);
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
            Slogger::W/*Wtf*/(TAG, "Task with inRecent not in recents: %s", ToChars(task));
            needAffiliationFix = TRUE;
        }
    }

    if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: trimming tasks for %s", ToChars(task));
    TrimRecentsForTask(task, TRUE);

    N = mRecentTasks.GetSize();
    AutoPtr<IActivityManagerHelper> amhelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amhelper);
    Int32 maxRecentTasks;
    amhelper->GetMaxRecentTasksStatic(&maxRecentTasks);
    while (N >= maxRecentTasks) {
        AutoPtr<TaskRecord> tr = mRecentTasks.GetBack();
        mRecentTasks->PopBack();
        tr->RemovedFromRecents(mTaskPersister);
        N--;
    }
    task->mInRecents = TRUE;
    if (!isAffiliated || needAffiliationFix) {
        // If this is a simple non-affiliated task, or we had some failure trying to
        // handle it as part of an affilated task, then just place it at the top.
        mRecentTasks.Insert(mRecentTasks.Begin(), task);
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
            List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
            for (Int32 i = 0; it != mRecentTasks.End(); ++it, i++) {
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
                    taskIndex, ToChars(task));
                mRecentTasks.Insert(insertIt, task);

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
                        ToChars(other));
                needAffiliationFix = TRUE;
            }
        }
        else {
            if (DEBUG_RECENTS) Slogger::D(TAG,
                    "addRecent: adding affiliated task without next/prev:%s", ToChars(task));
            needAffiliationFix = TRUE;
        }
    }
    if (DEBUG_RECENTS) Slogger::D(TAG, "addRecent: adding %s", ToChars(task));

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
    List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
    while (it != mRecentTasks.End()) {
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
        it = mRecentTasks.Erase(it);
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
    /* [in] */ IBinder token)
{
    synchronized (this) {
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
                    Watchdog->GetInstance()->SetActivityController(NULL);
                }

                if (!resumeOK) {
                    return NOERROR;
                }
            }
        }

        const Int64 origId = Binder::ClearCallingIdentity();
        ECode ec;
        if (finishTask && r == rootR) {
            // If requested, remove the task that is associated to this activity only if it
            // was the root activity in the task.  The result code and data is ignored because
            // we don't support returning them across task boundaries.
            ec = RemoveTaskByIdLocked(tr->mTaskId, 0, result);
        }
        else {
            ec = tr->mStack->RequestFinishActivityLocked(token, resultCode,
                    resultData, String("app-request"), TRUE, result);
        }
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }
}

ECode CActivityManagerService::FinishHeavyWeightApp()
{
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: finishHeavyWeightApp() from pid=%d, uid=%d"
            " requires Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES",
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: crashApplication() from pid=%d, uid=%d requires"
            " Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES",
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
                if (p->mPkgList.Find(packageName) != p->mPkgList.End()) {
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
    ECode ec;
    // Do not allow task to finish in Lock Task mode.
    if (r->mTask == mStackSupervisor->mLockTaskModeTask) {
        if (rootR == r) {
            ec = mStackSupervisor->ShowLockTaskToast();
            goto _EXIT_;
        }
    }

    if (r != NULL) {
        ec = r->mTask->mStack->FinishActivityAffinityLocked(r, finished);
    }
_EXIT_:
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::FinishVoiceTask(
    /* [in] */ IIVoiceInteractionSession* session)
{
    synchronized(this) {
        const Int64 origId = Binder::ClearCallingIdentity();
        ECode ec = mStackSupervisor->FinishVoiceTask(session);
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }

}

ECode CActivityManagerService::ReleaseActivityInstance(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(this) {
        const Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<ActivityRecord> r = ActivityRecord::IsInStackLocked(token);
        ECode ec = NOERROR;
        if (r->mTask == NULL || r->mTask->mStack == NULL) {
            *result = FALSE;
        }
        else
            ec = r->mTask->mStack->SafelyDestroyActivityLocked(
                r, String("app-req"), result);
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }
}

ECode CActivityManagerService::ReleaseSomeActivities(
    /* [in] */ IApplicationThread* appInt)
{
    synchronized(this) {
        const Int64 origId = Binder::ClearCallingIdentity();
        AutoPtr<ProcessRecord> app = GetRecordForAppLocked(appInt);
        ECode ec = mStackSupervisor->ReleaseSomeActivitiesLocked(app, String("low-mem"));
        Binder::RestoreCallingIdentity(origId);
        return ec;
    }
}

ECode CActivityManagerService::WillActivityBeVisible(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);

    AutoLock lock(this);

    AutoPtr<ActivityStack> stack = ActivityRecord::GetStackLocked(token);
    if (stack != NULL) {
        return stack->WillActivityBeVisibleLocked(token, visible);
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
            if (rec == dyingProc || rec->mThread == null) {
                continue;
            }
            if (doReport) {
                AutoPtr<IProcessMemInfo> menInfo = new ProcessMemInfo(rec->mProcessName,
                    rec->mPid, rec->mSetAdj, rec->mSetProcState, rec->mAdjType, rec->MakeAdjReason());
                memInfos->Add(menInfo);
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
    synchronized (mPidsSelfLockedLock) {
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator find = mPidsSelfLocked.Find(pid);
        AutoPtr<ProcessRecord> curProc = find != mPidsSelfLocked.End() ? find->mSecond : NULL;
        if (curProc != app) {
            Slogger::W(TAG, "Spurious death for %s, curProc for %d: %s",
                ToChars(app), pid, ToChars(curProc));
            return NOERROR;
        }
    }

    Int32 uid;
    app->mInfo->GetUid(&uid);
    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    synchronized (stats) {
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
                ToChars(app), pid, ToChars(thread));
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
            , ToChars(thread));
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
        Process::GetPidsForCommands(nativeProcs, (ArrayOf<Int32>**)&pids);
        if (pids != NULL) {
            AutoPtr<IDebug> debug;
            CDebug::AcquireSingleton((IDebug**)&debug);
            for (Int32 i = 0; i < pids->GetLength(); ++i) {
                debug->DumpNativeBacktraceToFile(pid, tracesPath);
            }
        }
    }

    // Lastly, measure CPU usage.
    if (processCpuTracker != NULL) {
        processCpuTracker->Init();
        processCpuTracker->Update();
        // try {
            synchronized(processCpuTracker) {
                IObject::Probe(processCpuTracker)->Wait(500); // measure over 1/2 second.
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
            AutoPtr<IProcessStatsStats> stats;
            processCpuTracker->GetWorkingStats(i, (IProcessStatsStats**)&stats);
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
    Handle32 oldPolicy, tmpPolicy;
    helper->AllowThreadDiskReads(&oldPolicy);
    helper->AllowThreadDiskWrites(&tmpPolicy);
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
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
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
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(tracesFile, (IFileOutputStream**)&fos);
    String str = sb.ToString();
    ArrayOf<Byte> bytes((Byte*)str.string(), str.GetByteLength());
    fos->WriteBytes(bytes);
    if (app == NULL) {
        String str("\n*** No application process!");
        ArrayOf<Byte> bytes((Byte*)str.string(), str.GetByteLength());
        fos->WriteBytes(bytes);
    }
    fos->Close();
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
            Slogger::I(TAG, "During shutdown skipping ANR: %s %s", ToChars(app), annotation.string());
            return NOERROR;
        }
        else if (app->mNotResponding) {
            Slogger::I(TAG, "Skipping duplicate ANR: %s %s", ToChars(app), annotation.string());
            return NOERROR;
        }
        else if (app->mCrashing) {
            Slogger::I(TAG, "Crashing app skipping ANR: %s %s", ToChars(app), annotation.string());
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
    info.Append("PID: ").;
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
        NATIVE_STACKS_OF_INTEREST);

    String cpuInfo;
    if (MONITOR_CPU_USAGE) {
        UpdateCpuStatsNow();
        synchronized (mProcessCpuTracker) {
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
                if (res < 0 && app.pid != MY_PID) {
                    app->Kill(String("anr"), TRUE);
                }
                else {
                    synchronized (this) {
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
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 value;
    secure->GetInt32(resolver,
           ISettingsSecure::ANR_SHOW_BACKGROUND, 0, &value);
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

        // Bring up the infamous App Not Responding dialog
        AutoPtr<StringObjectHashMap> data = new StringObjectHashMap();
        (*data)[String("app")] = app;
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
    /* [in] */ IPackageDataObserver* observer,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

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
               Elastos::Droid::Manifest::permission::CLEAR_APP_USER_DATA,
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
        List<AutoPtr<TaskRecord> >::ReverseIterator rit = mRecentTasks.RBegin();
        while (rit != mRecentTasks.REnd()) {
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

    synchronized(this) {
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::KILL_BACKGROUND_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED &&
            CheckCallingPermission(Elastos::Droid::Manifest::permission::RESTART_PACKAGES)
                != IPackageManager::PERMISSION_GRANTED) {
        Slogger::W(TAG, "Permission Denial: killBackgroundProcesses() from pid=%d, uid=%d requires Elastos::Droid::Manifest::permission::KILL_BACKGROUND_PROCESSES",
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
        appId = UserHandleGetAppId(uid);
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::KILL_BACKGROUND_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: killAllBackgroundProcesses() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::KILL_BACKGROUND_PROCESSES";
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: forceStopPackage() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::FORCE_STOP_PACKAGES";
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
    synchronized (this) {
        Int32 callingPid = Binder::GetCallingPid();
        if (callingPid == Process::MyPid()) {
            //  Yeah, um, no.
            Slogger::W(TAG, "Can't addPackageDependency on system process");
            return NOERROR;
        }
        AutoPtr<ProcessRecord> proc;
        synchronized (mPidsSelfLockedLock) {
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
            proc->mPkgDeps.Insert(packageName);
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
    Int32 callerUid = Binder::GetCallingUid();
    // Only the system server can kill an application
    if (callerUid == IProcess::SYSTEM_UID) {
        // Post an aysnc message to kill the application
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(KILL_APPLICATION_MSG, appid, 0, (IMessage**)&msg);
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        bundle-<PutString(String("pkg"), pkg);
        bundle-<PutString(String("reason"), reason);
        msg->SetObj(bundle);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
    else {
        Slogger::E(TAG, "%d cannot kill pkg: %s", callerUid, pkg.string());
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
        NULL, 0, nullStr, NULL, nullStr, IAppOpsManager::OP_NONE FALSE, FALSE, -1,
        IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
    return NOERROR;
}

ECode CActivityManagerService::GetProcessMemoryInfo(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out, callee] */ ArrayOf<IDebugMemoryInfo*>** retInfos)
{
    VALIDATE_NOT_NULL(retInfos);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessMemoryInfo")));
    Int32 length = pids->GetLength();
    AutoPtr<ArrayOf<IDebugMemoryInfo*> > infos = ArrayOf<IDebugMemoryInfo*>::Alloc(length);
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    for (Int32 i = length - 1; i >= 0; i--) {
        AutoPtr<ProcessRecord> proc;
        Int32 oomAdj;
        synchronized (this) {
            synchronized (mPidsSelfLockedLock) {
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
        dbg->GetMemoryInfo((*pids)[i], info);
        if (proc != NULL) {
            synchronized (this) {
                if (proc->mThread != NULL && proc->mSetAdj == oomAdj) {
                    // Record this for posterity if the process has been stable.
                    Int64 pss, uss;
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

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessPss")));
    Int32 length = pids->GetLength();
    AutoPtr<ArrayOf<Int64> > pss = ArrayOf<Int64>::Alloc(length);
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    for (Int32 i = length - 1; i >= 0; i--) {
        AutoPtr<ProcessRecord> proc;
        Int32 oomAdj;
        synchronized (this) {
            synchronized (mPidsSelfLockedLock) {
                HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it =
                    mPidsSelfLocked.Find((*pids)[i]);
                if (it != mPidsSelfLocked.End()) {
                    proc = it->mSecond;
                }
                oomAdj = proc != NULL ? proc->mSetAdj : 0;
            }
        }
        AutoPtr<ArrayOf<Int64> > tmpUss = ArrayOf<Int64>::Alloc(1);
        dbg->GetPss((*pids)[i], tmpUss, &(*pss)[i]);
        if (proc != NULL) {
            synchronized (this) {
                if (proc->mThread != NULL && proc->mSetAdj == oomAdj) {
                    // Record this for posterity if the process has been stable.
                    proc->mBaseProcessTracker->AddPss((*pss)[i], (tmpUss)[0], FALSE, proc->mPkgList);
                }
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

    Int32 callerUid = Binder::GetCallingUid();
    // Only the system server can kill an application
    if (callerUid == IProcess::SYSTEM_UID) {
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
        // throw new SecurityException(callerUid + " cannot kill app process: " +
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
    ForceStopPackageLocked(packageName, UserHandleGetAppId(uid), FALSE,
            FALSE, TRUE, FALSE, FALSE, UserHandleGetUserId(uid), reason);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_PACKAGE_RESTARTED,
            uri, (IIntent**)&intent);
    if (!mProcessesReady) {
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                | IIntent::FLAG_RECEIVER_FOREGROUND);
    }
    intent->PutExtra(IIntent::EXTRA_UID, uid);
    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, UserHandleGetUserId(uid));
    Int32 result;
    return BroadcastIntentLocked(NULL, String(NULL), intent,
        String(NULL), NULL, 0, String(NULL), NULL, String(NULL), IAppOpsManager::OP_NONE,
        FALSE, FALSE,
        MY_PID, IProcess::SYSTEM_UID, UserHandleGetUserId(uid), &result);
}

ECode CActivityManagerService::ForceStopUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ const String& reason)
{
    ForceStopPackageLocked(String(NULL), -1, FALSE, FALSE, TRUE, FALSE, FALSE, userId, reason);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_USER_STOPPED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
    Int32 result;
    BroadcastIntentLocked(NULL, String(NULL), intent,
        String(NULL), NULL, 0, String(NULL), NULL, String(NULL), IAppOpsManager::OP_NONE,
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
                if (appId >= 0 && UserHandleGetAppId(app->mUid) != appId) {
                    continue;
                }
            // Package has been specified, we want to hit all processes
            // that match it.  We need to qualify this by the processes
            // that are running under the specified app and user ID.
            }
            else {
                Boolean isDep = app->mPkgDeps != NULL
                    && app->mPkgDeps.Find(packageName) != app->mPkgDeps.End();
                if (!isDep && UserHandleGetAppId(app->mUid) != appId) {
                    continue;
                }
                if (userId != IUserHandle::USER_ALL && app->mUserId != userId) {
                    continue;
                }

                if (app->mPkgList.Find(packageName) == app->mPkgList.End() && !isDep) {
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
    Int32 i, N;
    UNUSED(i);
    UNUSED(N);

    if (userId == IUserHandle::USER_ALL && name.IsNull()) {
        Slogger::W(TAG, "Can't force stop all processes of all users, that is insane!");
    }

    if (appId < 0 && !name.IsNull()) {
        Int32 uid;
        AppGlobals::GetPackageManager()->GetPackageUid(name, 0, &uid);
        appId = UserHandleGetAppId(uid);
    }

    if (doit) {
        if (!name.IsNull()) {
            Slogger::I(TAG, "Force stopping %s appId=%d user=%d: %s",
                    name.string(), appId, userId, reason.string());
        }
        else {
            Slogger::I(TAG, "Force stopping user %d: %s", userId, reason.string());
        }
        HashMap<String, AutoPtr<HashMap<Int32, Millisecond64> > >::Iterator it = mProcessCrashTimes->GetMap()->Begin();
        for (; it != mProcessCrashTimes->GetMap()->End();) {
            AutoPtr<HashMap<Int32, Millisecond64> > ba = it->mSecond;
            HashMap<Int32, Millisecond64>::Iterator it2 = ba->Begin();
            for (; it2 != ba->End();) {
                Boolean remove = FALSE;
                Int32 entUid = it2->mFirst;
                if (!name.IsNull()) {
                    if (userId == IUserHandle::USER_ALL) {
                        if (UserHandleGetAppId(entUid) == appId) {
                            remove = TRUE;
                        }
                    }
                    else {
                        AutoPtr<IUserHandleHelper> uhHelper;
                        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
                        Int32 uid;
                        uhHelper->GetUid(userId, appId, &uid);
                        if (entUid == uid) {
                            remove = TRUE;
                        }
                    }
                }
                else if (UserHandleGetUserId(entUid) == userId) {
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

    Boolean bval;
    if (mServices->ForceStopLocked(name, userId, evenPersistent, doit, &bval), bval) {
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
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

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
                    Int32 uappId;
                    helper->GetAppId(pir->mUid, &uappId);
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
            AttributeCache* ac = AttributeCache::GetInstance();
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
        app->Kill(reason, TRUE);
        HandleAppDiedLocked(app, TRUE, allowRestart);
        RemoveLruProcessLocked(app);

        if (app->mPersistent && !app->mIsolated) {
            if (!callerWillRestart) {
                AddAppLocked(app->mInfo, FALSE, NULL /* ABI override */);
            }
            else {
                needRestart = TRUE;
            }
        }
    }
    else {
        mRemovedProcesses.PushBack(app);
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
        if (mBackupTarget != NULL && mBackupTarget->mApp->mPid == pid) {
            Slogger::W(TAG, "Unattached app died before backup, skipping");
            // try {
            AutoPtr<IInterface> backupService = ServiceManager::GetService(IContext::BACKUP_SERVICE);
            AutoPtr<IIBackupManager> bm = IIBackupManager::Probe(backupService);
            String pkgName;
            app->mInfo->GetPackageName(&pkgName);
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
        String appApDes;
        IBinder::Probe(thread)->ToString(&appApDes);
        Slogger::W(TAG, "No pending application record for pid %d (IApplicationThread %s); dropping process",
                pid, appApDes.string());
        //EventLog.writeEvent(EventLogTags.AM_DROP_PROCESS, pid);
        if (pid > 0 && pid != MY_PID) {
            Process::KillProcessQuiet(pid);
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
        String appDes = app->ToString();
        Slogger::V(TAG, "Binding process pid %d to record %s", pid, appDes.string());
    }

    String processName = app->mProcessName;

    AutoPtr<AppDeathRecipient> adr = new AppDeathRecipient(app, pid, thread, this);
    AutoPtr< ::IProxy > proxy = (::IProxy*)thread->Probe(::EIID_IProxy);
    assert(proxy != NULL);
    if (FAILED(proxy->LinkToDeath(adr.Get(), 0))) {
        app->ResetPackageList();
        StartProcessLocked(app, String("link fail"), processName);
        return FALSE;
    }
    app->mDeathRecipient = adr;
//    EventLog.writeEvent(EventLogTags.AM_PROC_BOUND, app.userId, app.pid, app.processName);

    app->mThread = thread;
    app->mCurAdj = app->mSetAdj = -100;
    app->mCurSchedGroup = IProcess::THREAD_GROUP_DEFAULT;
    app->mSetSchedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
    app->mForcingToForeground = NULL;
    app->mForegroundServices = FALSE;
    app->mHasShownUi = FALSE;
    app->mDebugging = FALSE;

    mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, app->Probe(EIID_IInterface));

    AutoPtr<IObjectContainer> providers;
    Boolean normalMode = mProcessesReady || IsAllowedWhileBooting(app->mInfo);
    if (normalMode) {
        GenerateApplicationProvidersLocked(app, (IObjectContainer**)&providers);
    }

    if (!normalMode) {
        String appDes = app->ToString();
        Slogger::I(TAG, "Launching preboot mode app: %s", appDes.string());
    }

    if (localLOGV) {
        String appDes = app->ToString();
        String appApDes;
        IBinder::Probe(thread)->ToString(&appApDes);
        Slogger::V(TAG, "New app %s record %s, thread=%s pid=%d", processName.string(), appDes.string(), appApDes.string(), pid);
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
    Boolean profileAutoStop = FALSE;
    if (!mProfileApp.IsNull() && mProfileApp.Equals(processName)) {
        mProfileProc = app;
        profileFile = mProfileFile;
        profileFd = mProfileFd;
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
        app->mInstrumentationInfo->GetPackageName(&insPkgName);
        EnsurePackageDexOpt(insPkgName);
    }
    else {
        app->mInfo->GetPackageName(&infoPkgName);
        EnsurePackageDexOpt(infoPkgName);
    }
    if (app->mInstrumentationClass != NULL) {
        String classPkgName;
        app->mInstrumentationClass->GetPackageName(&classPkgName);
        EnsurePackageDexOpt(classPkgName);
    }
    if (DEBUG_CONFIGURATION) {
        String confDes;
        mConfiguration->ToString(&confDes);
        Slogger::V(TAG, "Binding proc %s with config %s", processName.string(), confDes.string());
    }
    AutoPtr<IApplicationInfo> appInfo = app->mInstrumentationInfo != NULL
            ? app->mInstrumentationInfo : app->mInfo;
    app->mCompat = CompatibilityInfoForPackageLocked(appInfo);
   if (profileFd != NULL) {
        AutoPtr<IFileDescriptor> fd;
        profileFd->GetFileDescriptor((IFileDescriptor**)&fd);
        AutoPtr<IParcelFileDescriptorHelper> helper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
        AutoPtr<IParcelFileDescriptor> pfd;
        helper->Dup(fd, (IParcelFileDescriptor**)&pfd);
        profileFd = NULL;
        profileFd = pfd;
   }
    AutoPtr<IHashMap> services = GetCommonServicesLocked();
    AutoPtr<IConfiguration> configuration;
    CConfiguration::New(mConfiguration, (IConfiguration**)&configuration);
    AutoPtr<IBundle> coreSettings = mCoreSettingsObserver->GetCoreSettingsLocked();
    thread->BindApplication(processName, appInfo, providers,
            app->mInstrumentationClass, profileFile, profileFd, profileAutoStop,
            app->mInstrumentationArguments, app->mInstrumentationWatcher, testMode,
            enableOpenGlTrace, isRestrictedBackupMode || !normalMode, app->mPersistent,
            configuration, app->mCompat, services, coreSettings);
    UpdateLruProcessLocked(app, FALSE);
    app->mLastRequestedGc = app->mLastLowMemory = SystemClock::GetUptimeMillis();
//    } catch (Exception e) {
//        // todo: Yikes!  What should we do?  For now we will try to
//        // start another process, but that could easily get us in
//        // an infinite loop of restarting processes...
//        Slogger::W(TAG, "Exception thrown during bind!", e);
//
//        app.resetPackageList();
//        app.unlinkDeathRecipient();
//        startProcessLocked(app, "bind fail", processName);
//        return FALSE;
//    }
//
    // Remove this record from the list of starting applications.
    mPersistentStartingProcesses.Remove(app);
    if (DEBUG_PROCESSES && (Find(mProcessesOnHold.Begin(),
            mProcessesOnHold.End(), app) != mProcessesOnHold.End())) {
        String appDes = app->ToString();
        Slogger::V(TAG, "Attach application locked removing on hold: %s", appDes.string());
    }
    mProcessesOnHold.Remove(app);

    Boolean badApp = FALSE;
    Boolean didSomething = FALSE;

    // See if the top visible activity is waiting to run in this process...
    AutoPtr<ActivityRecord> hr = mMainStack->GetTopRunningActivityLocked(NULL);
    if (hr != NULL && normalMode) {
        AutoPtr<IApplicationInfo> hrAppInfo;
        hr->mInfo->GetApplicationInfo((IApplicationInfo**)&hrAppInfo);
        Int32 hrUid;
        hrAppInfo->GetUid(&hrUid);
        if (hr->mApp == NULL && app->mUid == hrUid && !processName.Compare(hr->mProcessName)) {
//            try {
            Boolean succeeded;
            if (mHeadless) {
                Slogger::E(TAG, "Starting activities not supported on headless device: %p", hr.Get());
            }
            else if (SUCCEEDED(mMainStack->RealStartActivityLocked(hr, app, TRUE, TRUE, &succeeded))
                    && succeeded) {
                didSomething = TRUE;
            }
//            } catch (Exception e) {
//                Slogger::W(TAG, "Exception in new application when starting activity "
//                      + hr.intent.getComponent().flattenToShortString(), e);
//                badApp = TRUE;
//            }
        }
        else {
            mStackSupervisor->EnsureActivitiesVisibleLocked(hr, NULL, processName, 0);
        }
    }

    // Find any services that should be running in this process...
    if (!badApp) {
//        try {
        Boolean result = FALSE;
        ECode ec = mServices->AttachApplicationLocked(app, processName, &result);
        didSomething |= result;
        if (FAILED(ec)) {
            badApp = TRUE;
        }
//        } catch (Exception e) {
//            badApp = TRUE;
//        }
    }

    // Check if a next-broadcast receiver is in this process...
    if (!badApp && IsPendingBroadcastProcessLocked(pid)) {
        // try {
        didSomething = SendPendingBroadcastsLocked(app);
        //} catch (Exception e) {
            // If the app died trying to launch the receiver we declare it 'bad'
            //TODO
            //badApp = TRUE;
        //}
    }
    // Check whether the next backup agent is in this process...
    Int32 backupUid;
    if (!badApp && mBackupTarget != NULL && (mBackupTarget->mAppInfo->GetUid(&backupUid), backupUid == app->mUid)) {
        if (DEBUG_BACKUP) Slogger::V(TAG, "New app is backup target, launching agent for %p", app.Get());
        String pkgName;
        mBackupTarget->mAppInfo->GetPackageName(&pkgName);
        EnsurePackageDexOpt(pkgName);
        //try {
        thread->ScheduleCreateBackupAgent(mBackupTarget->mAppInfo,
        CompatibilityInfoForPackageLocked(mBackupTarget->mAppInfo),
        mBackupTarget->mBackupMode);
        // } catch (Exception e) {
        //     Slogger::W(TAG, "Exception scheduling backup agent creation: ");
        //     e.printStackTrace();
        // }
    }

    if (badApp) {
        // todo: Also need to kill application to deal with all
        // kinds of exceptions.
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
    AutoPtr<ActivityRecord> r = mMainStack->ActivityIdleInternal(token, FALSE, config);
    if (stopProfiling) {
         AutoLock lock(this);
         if (mProfileProc.Get() == r->mApp) {
             if (mProfileFd != NULL) {
//                 try {
                     mProfileFd->Close();
//                 } catch (IOException e) {
//                 }
                 ClearProfilerLocked();
             }
         }
     }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
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

ECode CActivityManagerService::DismissKeyguardOnNextActivity()
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("dismissKeyguardOnNextActivity")));
    const Int64 token = Binder::ClearCallingIdentity();
//     try {
        {
            AutoLock lock(this);
            if (mLockScreenShown) {
                mLockScreenShown = FALSE;
                ComeOutOfSleepIfNeededLocked();
            }
            mMainStack->DismissKeyguardOnNextActivityLocked();
        }
//     } finally {
        Binder::RestoreCallingIdentity(token);
//     }
    return NOERROR;
}

ECode CActivityManagerService::FinishBooting()
{
    AutoPtr<IIntentFilter> pkgFilter;
    CIntentFilter::New((IIntentFilter**)&pkgFilter);
    pkgFilter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART);
    pkgFilter->AddDataScheme(String("package"));
    AutoPtr<IBroadcastReceiver> receiver = new FinishBootingBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(receiver, pkgFilter, (IIntent**)&intent);

    {
        AutoLock lock(this);
        String nullStr;

        // Ensure that any processes we had put on hold are now started
        // up.
        if (!mProcessesOnHold.IsEmpty()) {
            List< AutoPtr<ProcessRecord> > procs = mProcessesOnHold;
            List< AutoPtr<ProcessRecord> >::Iterator it;
            for (it = procs.Begin(); it != procs.End(); ++it) {
                if (DEBUG_PROCESSES) Slogger::V(TAG, "Starting process on hold: %p", (*it).Get());
                StartProcessLocked(*it, String("onHold"), nullStr);
            }
        }

        if (mFactoryTest != CSystemServer::FACTORY_TEST_LOW_LEVEL) {
            // Start looking for apps that are abusing wake locks.
            Boolean result;
            mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);

            // Tell anyone interested that we are done booting!
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            sysProp->Set(String("sys.boot_completed"), String("1"));
            sysProp->Set(String("dev.bootcomplete"), String("1"));
            HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it;

            Int32 userId, ival;
            for (it = mStartedUsers.Begin(); it != mStartedUsers.End(); ++it) {
                AutoPtr<UserStartedState> uss = it->mSecond;
                if (uss->mState == UserStartedState::STATE_BOOTING) {
                    uss->mState = UserStartedState::STATE_RUNNING;
                    userId = it->mFirst;
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_BOOT_COMPLETED, NULL, (IIntent**)&intent);
                    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                    BroadcastIntentLocked(NULL, nullStr, intent,
                        nullStr, NULL, 0, nullStr, NULL,
                        Elastos::Droid::Manifest::permission::RECEIVE_BOOT_COMPLETED,
                        FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &ival);
                }
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::SendBootFastComplete()
{
    Slogger::D(TAG,"sendBootFastComplete");
    FAIL_RETURN(EnforceNotIsolatedCaller(String("sendBootFastComplete")));
    const Int64 token = Binder::ClearCallingIdentity();
//     try {
    {
        AutoLock lock(this);
        String nullStr;

        // Ensure that any processes we had put on hold are now started
        // up.
        if (!mProcessesOnHold.IsEmpty()) {
            List< AutoPtr<ProcessRecord> > procs = mProcessesOnHold;
            List< AutoPtr<ProcessRecord> >::Iterator it;
            for (it = procs.Begin(); it != procs.End(); ++it) {
                if (DEBUG_PROCESSES) Slogger::V(TAG, "Starting process on hold: %p", (*it).Get());
                StartProcessLocked(*it, String("on-hold"), nullStr);
            }
        }

        if (mFactoryTest != CSystemServer::FACTORY_TEST_LOW_LEVEL) {
            Slogger::D(TAG,"not mFactoryTest");
            // Start looking for apps that are abusing wake locks.
            Boolean result;
            mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);

            // Tell anyone interested that we are done booting!
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            sysProp->Set(String("sys.boot_completed"), String("1"));
            sysProp->Set(String("dev.bootcomplete"), String("1"));
            HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it;
            Int32 i = 0;
            for (it = mStartedUsers.Begin(); it != mStartedUsers.End(); ++it, ++i) {
                Slogger::D(TAG,"sendBootFastComplete start user = %d", i);
                AutoPtr<UserStartedState> uss = it->mSecond;
                //if (uss.mState == UserStartedState.STATE_BOOTING) {
                    uss->mState = UserStartedState::STATE_RUNNING;
                    const Int32 userId = it->mFirst;
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_BOOT_COMPLETED, NULL, (IIntent**)&intent);
                    FAIL_RETURN(intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId));
                    Int32 result;
                    FAIL_RETURN(BroadcastIntentLocked(NULL, nullStr, intent,
                           nullStr, NULL, 0, nullStr, NULL,
                           Elastos::Droid::Manifest::permission::RECEIVE_BOOT_COMPLETED,
                           FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &result));
                    Slogger::D(TAG,"real send boot complete");
                //}
            }
        }
    }
//     } finally {
    Binder::RestoreCallingIdentity(token);
//     }
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
    ECode ec = mMainStack->ActivityResumed(token);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::ActivityPaused(
    /* [in] */ IBinder* token)
{
    const Int64 origId = Binder::ClearCallingIdentity();
    ECode ec = mMainStack->ActivityPaused(token, FALSE);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::ActivityStopped(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* icicle,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ ICharSequence* description)
{
    VALIDATE_NOT_NULL(token);
    VALIDATE_NOT_NULL(icicle);
    if (localLOGV) {
        String tokenDes;
        token->ToString(&tokenDes);
        Slogger::V(TAG, "Activity stopped: token=%s", tokenDes.string());
    }
    // Refuse possible leaked file descriptors
    Boolean hasFD = FALSE;
    if (icicle != NULL && (icicle->HasFileDescriptors(&hasFD), hasFD)) {
        //throw new IllegalArgumentException("File descriptors passed in Bundle");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ActivityRecord> r;

    const Int64 origId = Binder::ClearCallingIdentity();

    {
        AutoLock lock(this);
        r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            r->mStack->ActivityStoppedLocked(r, icicle, thumbnail, description);
        }
    }

    if (r != NULL) {
        SendPendingThumbnail(r, NULL, NULL, NULL, FALSE);
    }

    TrimApplications();

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ActivityDestroyed(
    /* [in] */ IBinder* token)
{
    VALIDATE_NOT_NULL(token);
    if (DEBUG_SWITCH) {
        String tokenDes;
        token->ToString(&tokenDes);
        Slogger::V(TAG, "ACTIVITY DESTROYED: %s", tokenDes.string());
    }
    return mMainStack->ActivityDestroyed(token);
}

ECode CActivityManagerService::GetCallingPackage(
    /* [in] */ IBinder* token,
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package);
    *package = NULL;

    AutoLock lock(this);
    AutoPtr<ActivityRecord> r = GetCallingRecordLocked(token);
    if (r != NULL && r->mApp != NULL && r->mInfo != NULL) {
        return r->mInfo->GetPackageName(package);
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

    Boolean bValue;
    // Refuse possible leaked file descriptors
    if (intents != NULL) {
        Int32 length = intents->GetLength();
        if (length < 1) {
            // throw new IllegalArgumentException("Intents array length must be >= 1");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IIntent> intent = (*intents)[i];
            if (intent != NULL) {
                if (intent->HasFileDescriptors(&bValue), bValue) {
                    // throw new IllegalArgumentException("File descriptors passed in Intent");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                Int32 flags;
                intent->GetFlags(&flags);
                if (type == IActivityManager::INTENT_SENDER_BROADCAST &&
                    (flags & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) != 0) {
                    // throw new IllegalArgumentException(
                    //     "Can't use FLAG_RECEIVER_BOOT_UPGRADE here");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                AutoPtr<IIntent> tmp;
                CIntent::New(intent, (IIntent**)&tmp);
                intents->Set(i, tmp);
            }
        }

        if (resolvedTypes != NULL && resolvedTypes->GetLength() != length) {
            // throw new IllegalArgumentException(
            //     "Intent array length does not match resolvedTypes length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (options != NULL) {
        if (options->HasFileDescriptors(&bValue), bValue) {
            // throw new IllegalArgumentException("File descriptors passed in options");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    {
        AutoLock lock(this);
        Int32 callingUid = Binder::GetCallingUid();
        Int32 origUserId = userId;
        Int32 result;
        userId = HandleIncomingUser(Binder::GetCallingPid(), callingUid, userId,
                type == IActivityManager::INTENT_SENDER_BROADCAST, FALSE,
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
            pkgMgr->GetPackageUid(packageName, UserHandleGetUserId(callingUid), &uid);
            AutoPtr<IUserHandleHelper> uhHelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
            Boolean isSameApp;
            uhHelper->IsSameApp(callingUid, uid, &isSameApp);
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
                // throw new SecurityException(msg);
                return E_SECURITY_EXCEPTION;
            }
        }

        return GetIntentSenderLocked(type, packageName, callingUid, userId,
                token, resultWho, requestCode, intents, resolvedTypes, flags, options, intentSender);

        // } catch (RemoteException e) {
        //     throw new SecurityException(e);
        // }
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
    CPendingIntentRecord::NewByFriend((Handle32)this, (Handle32)key.Get(), callingUid, (CPendingIntentRecord**)&rec);
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)rec->Probe(EIID_IWeakReferenceSource);
    AutoPtr<IWeakReference> wr;
    wrs->GetWeakReference((IWeakReference**)&wr);
    mIntentSenderRecords[key] = wr;
    if (type == IActivityManager::INTENT_SENDER_ACTIVITY_RESULT) {
        if (activity->mPendingResults == NULL) {
            activity->mPendingResults = new HashSet< AutoPtr<IWeakReference> >(9);
        }
        activity->mPendingResults->Insert(wr);
    }
    *sender = rec;
    REFCOUNT_ADD(*sender);
    return NOERROR;
}

ECode CActivityManagerService::CancelIntentSender(
    /* [in] */ IIIntentSender* sender)
{
    if (sender == NULL || sender->Probe(EIID_PendingIntentRecord) == NULL) {
        return NOERROR;
    }

    {
        AutoLock lock(this);
        CPendingIntentRecord* rec = (CPendingIntentRecord*)sender;
        // try {
        AutoPtr<IIPackageManager> pm =  AppGlobals::GetPackageManager();
        Int32 uid;
        pm->GetPackageUid(rec->mKey->mPackageName, UserHandleGetCallingUserId(), &uid);
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        Boolean isSameApp;
        uhHelper->IsSameApp(uid, Binder::GetCallingUid(), &isSameApp);
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

    if (pendingResult == NULL || pendingResult->Probe(EIID_PendingIntentRecord) == NULL) {
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

    if (sender == NULL || sender->Probe(EIID_PendingIntentRecord) == NULL) {
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

    if (pendingResult == NULL || pendingResult->Probe(EIID_PendingIntentRecord) == NULL) {
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
        intent->GetPackage(&pkgName);
        AutoPtr<IComponentName> compName;
        intent->GetComponent((IComponentName**)&compName);
        if (!pkgName.IsNull() && compName != NULL) {
            return NOERROR;
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

    if (pendingResult == NULL || pendingResult->Probe(EIID_PendingIntentRecord) == NULL) {
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

    if (sender == NULL || sender->Probe(EIID_PendingIntentRecord) == NULL) {
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

ECode CActivityManagerService::SetProcessLimit(
    /* [in] */ Int32 max)
{
    ECode ec = EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_PROCESS_LIMIT,
            String("setProcessLimit()"));
    {
        AutoLock lock(this);
        mProcessLimit = max < 0 ? ProcessList::MAX_HIDDEN_APPS : max;
        mProcessLimitOverride = max;
    }
    TrimApplications();
    return ec;
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
        pr->mForegroundServices = FALSE;
    }
    UpdateOomAdjLocked();
    return NOERROR;
}

ECode CActivityManagerService::SetProcessForeground(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 pid,
    /* [in] */ Boolean isForeground)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_PROCESS_LIMIT,
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
                AutoPtr<IProxy> proxy = (IProxy*)oldToken->mToken->Probe(EIID_IProxy);
                Boolean result;
                proxy->UnlinkToDeath(oldToken, 0, &result);
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
                AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
                proxy->LinkToDeath(newToken, 0);
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

//// =========================================================
//// PERMISSIONS
//// =========================================================

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
        StringBuilder sb("  adjusting {pid,uid} to {");
        sb += tlsIdentity->mPid;
        sb += ",";
        sb += tlsIdentity->mUid;
        sb += "}";
        Slogger::D(TAG, sb.ToString().string());

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
        *result = CheckComponentPermission(permission,
                pid, UserHandleGetAppId(uid), -1, TRUE);
    }

    return NOERROR;
}

Int32 CActivityManagerService::CheckCallingPermission(
    /* [in] */ const String& permission)
{
    Int32 result;
    CheckPermission(permission,
            Binder::GetCallingPid(),
            UserHandleGetAppId(Binder::GetCallingUid()),
            &result);
    return result;
}

ECode CActivityManagerService::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& func)
{
    if (CheckCallingPermission(permission)
            == IPackageManager::PERMISSION_GRANTED) {
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
    /* [in] */ IUri* uri,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags)
{
    if (DEBUG_URI_PERMISSION) {
        Slogger::V(TAG, "checkHoldingPermissionsLocked: uri=%p uid=%d", uri, uid);
    }
    AutoPtr<IApplicationInfo> piAppInfo;
    pi->GetApplicationInfo((IApplicationInfo**)&piAppInfo);
    Int32 piUid;
    piAppInfo->GetUid(&piUid);
    Boolean piExported;
    pi->GetExported(&piExported);
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
    if (!readMet && !piReadPermission.IsNull() && perm == IPackageManager::PERMISSION_GRANTED) {
        readMet = TRUE;
    }
    String piWritePermission;
    pi->GetWritePermission(&piWritePermission);
    Int32 perm2;
    ASSERT_SUCCEEDED(pm->CheckUidPermission(piWritePermission, uid, &perm2));
    if (!writeMet && !piWritePermission.IsNull() && perm2 == IPackageManager::PERMISSION_GRANTED) {
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
        uri->GetPath(&path);
        Int32 i = pps->GetLength();
        while (i > 0 && (!readMet || !writeMet)) {
            i--;
            AutoPtr<IPathPermission> pp = (*pps)[i];
            Boolean isMatch;
            pp->Match(path, &isMatch);
            if (isMatch) {
                if (!readMet) {
                    String pprperm;
                    pp->GetReadPermission(&pprperm);
                    Int32 perm;
                    ASSERT_SUCCEEDED(pm->CheckUidPermission(pprperm, uid, &perm));

                    if (DEBUG_URI_PERMISSION) {
                        String pathLog;
                        pp->GetPath(&pathLog);
                        Slogger::V(TAG, "Checking read perm for %s for %s: match=%d check=%d", pprperm.string(), pathLog.string(),
                                isMatch, perm);
                    }
                    if (!pprperm.IsNull()) {
                        if (perm == IPackageManager::PERMISSION_GRANTED) {
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
                    pp->Match(path, &isMatch);
                    if (DEBUG_URI_PERMISSION) {
                        String pathLog;
                        pp->GetPath(&pathLog);
                        Slogger::V(TAG, "Checking write perm %s for %s: match=%d check=%d",
                            ppwperm.string(), pathLog.string(), isMatch, perm);
                    }
                    if (!ppwperm.IsNull()) {
                        if (perm == IPackageManager::PERMISSION_GRANTED) {
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

Boolean CActivityManagerService::CheckUriPermissionLocked(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags)
{
    // Root gets to do everything.
    if (uid == 0) {
        return TRUE;
    }

    UriPermissionIterator it = mGrantedUriPermissions.Find(uid);
    if (it == mGrantedUriPermissions.End()) return FALSE;

    AutoPtr<PermissionMap> perms = it->mSecond;
    AutoPtr<UriPermission> perm = (*perms)[uri];
    if (perm == NULL) return FALSE;
    return (modeFlags & perm->mModeFlags) == modeFlags;
}

ECode CActivityManagerService::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permission)
{
    VALIDATE_NOT_NULL(permission);

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
       *permission = CheckUriPermissionLocked(uri, uid, modeFlags)
              ? IPackageManager::PERMISSION_GRANTED
              : IPackageManager::PERMISSION_DENIED;
        return NOERROR;
   }
}

ECode CActivityManagerService::CheckGrantUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 lastTargetUid,
    /* [out] */ Int32* permission)
{
    VALIDATE_NOT_NULL(permission);

    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    if (modeFlags == 0) {
        *permission = -1;
        return NOERROR;
    }

    if (!targetPkg.IsNull()) {
        if (DEBUG_URI_PERMISSION) {
            Slogger::V(TAG, "Checking grant %s permission to %p", targetPkg.string(), uri);
        }
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    // If this is not a content: uri, we can't do anything with it.
    String scheme;
    uri->GetScheme(&scheme);
    if (!scheme.Equals(IContentResolver::SCHEME_CONTENT)) {
        if (DEBUG_URI_PERMISSION) {
           Slogger::V(TAG, "Can't grant URI permission for non-content URI: %p", uri);
        }
        *permission = -1;
        return NOERROR;
    }

    String name;
    uri->GetAuthority(&name);
    AutoPtr<IProviderInfo> pi;
    AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByName(name,
               UserHandleGetUserId(callingUid));
    if (cpr != NULL) {
        pi = cpr->mInfo;
    }
    else {
        FAIL_RETURN(pm->ResolveContentProvider(name,
                IPackageManager::GET_URI_PERMISSION_PATTERNS, UserHandleGetUserId(callingUid),
                (IProviderInfo**)&pi));
    }
    if (pi == NULL) {
        String uriString;
        uri->ToSafeString(&uriString);
        Slogger::W(TAG, "No content provider found for permission check: %s", uriString.string());
        *permission = -1;
        return NOERROR;
    }

    Int32 targetUid = lastTargetUid;
    if (targetUid < 0 && !targetPkg.IsNull()) {
        if (SUCCEEDED(pm->GetPackageUid(targetPkg, UserHandleGetUserId(callingUid), &targetUid))) {
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
        if (CheckHoldingPermissionsLocked(pm, pi, uri, targetUid, modeFlags)) {
            // No need to grant the target this permission.
            if (DEBUG_URI_PERMISSION) {
                Slogger::V(TAG, "Target %s already has full permission to %p", targetPkg.string(), uri);
            }
            *permission = -1;
            return NOERROR;
        }
    }
    else {
        // First...  there is no target package, so can anyone access it?
        Boolean allowed;
        pi->GetExported(&allowed);
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

    // Second...  is the provider allowing granting of URI permissions?
    Boolean grant;
    pi->GetGrantUriPermissions(&grant);
    if (!grant) {
        String piCName, piName;
        pi->GetPackageName(&piCName);
        pi->GetName(&piName);
        Slogger::E(TAG, "Provider %s/ does not allow granting of Uri permissions (uri %p)",
                piCName.string(), piName.string(), uri);
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<ArrayOf<IPatternMatcher*> > patterns;
    pi->GetUriPermissionPatterns((ArrayOf<IPatternMatcher*>**)&patterns);
    if (patterns != NULL) {
        const Int32 N = patterns->GetLength();
        Boolean allowed = FALSE;
        for (Int32 i=0; i<N; i++) {
            String uriPath;
            uri->GetPath(&uriPath);
            Boolean matched;
            (*patterns)[i]->Match(uriPath, &matched);
            if ((*patterns)[i] != NULL && matched) {
                allowed = TRUE;
                break;
            }
        }
        if (!allowed) {
            String piCName, piName;
            pi->GetPackageName(&piCName);
            pi->GetName(&piName);
            Slogger::E(TAG, "Provider %s/%s does not allow granting of permission to path of Uri %p", piCName.string(),
                    piName.string(), uri);
            return E_SECURITY_EXCEPTION;
        }
    }

    // Third...  does the caller itself have permission to access
    // this uri?
    if (callingUid != Process::MyUid()) {
        if (!CheckHoldingPermissionsLocked(pm, pi, uri, callingUid, modeFlags)) {
            if (!CheckUriPermissionLocked(uri, callingUid, modeFlags)) {
               Slogger::E(TAG, "Uid %d does not have permission to uri %p", callingUid,
                        uri);
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
    /* [out] */ Int32* granted)
{
    VALIDATE_NOT_NULL(granted);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("checkGrantUriPermission")));
    {
        AutoLock lock(this);
        return CheckGrantUriPermissionLocked(callingUid, targetPkg, uri, modeFlags, -1, granted);
    }
}

ECode CActivityManagerService::GrantUriPermissionUncheckedLocked(
    /* [in] */ Int32 targetUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ UriPermissionOwner* owner)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
           | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    if (modeFlags == 0) {
        return NOERROR;
    }

    // So here we are: the caller has the assumed permission
    // to the uri, and the target doesn't.  Let's now give this to
    // the target.

    if (DEBUG_URI_PERMISSION) {
        Slogger::V(TAG, "Granting %s/%d permission to %p", targetPkg.string(),
                targetUid, uri);
    }

    AutoPtr<PermissionMap> targetUris;
    UriPermissionIterator it = mGrantedUriPermissions.Find(targetUid);
    if (it == mGrantedUriPermissions.End()) {
        targetUris = new PermissionMap(5);
        mGrantedUriPermissions[targetUid] = targetUris;
    }
    else {
        targetUris = it->mSecond;
    }

    AutoPtr<UriPermission> perm = (*targetUris)[uri];
    if (perm == NULL) {
        perm = new UriPermission(targetUid, uri);
        (*targetUris)[uri] = perm;
    }

    perm->mModeFlags |= modeFlags;
    if (owner == NULL) {
        perm->mGlobalModeFlags |= modeFlags;
    }
    else {
        if ((modeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
            perm->mReadOwners.Insert(owner);
            owner->AddReadPermission(perm);
        }
        if ((modeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
            perm->mWriteOwners.Insert(owner);
            owner->AddWritePermission(perm);
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GrantUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ UriPermissionOwner* owner)
{
    if (targetPkg.IsNull()) {
       // throw new NullPointerException("targetPkg");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 targetUid;
    CheckGrantUriPermissionLocked(callingUid, targetPkg, uri, modeFlags, -1, &targetUid);
    if (targetUid < 0) {
        return NOERROR;
    }

    GrantUriPermissionUncheckedLocked(targetUid, targetPkg, uri, modeFlags, owner);
    return NOERROR;
}

AutoPtr<NeededUriGrants>
CActivityManagerService::CheckGrantUriPermissionFromIntentLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 mode,
    /* [in] */ NeededUriGrants* needed)
{
    if (DEBUG_URI_PERMISSION) {
        StringBuilder sb("Checking URI perm to data");
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
        sb += StringUtils::Int32ToHexString(flags);
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
    if (data != NULL) {
        Int32 target;
        CheckGrantUriPermissionLocked(callingUid, targetPkg, data,
                mode, needed != NULL ? needed->mTargetUid : -1, &target);
        if (target > 0) {
            if (needed == NULL) {
                needed = new NeededUriGrants(targetPkg, target, mode);
            }
            needed->PushBack(data);
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
                Int32 target = -1;
                CheckGrantUriPermissionLocked(callingUid, targetPkg, uri,
                        mode, needed != NULL ? needed->mTargetUid : -1, &target);
                if (target > 0) {
                    if (needed == NULL) {
                        needed = new NeededUriGrants(targetPkg, target, mode);
                    }
                    needed->PushBack(uri);
                }
            }
            else {
                AutoPtr<IIntent> clipIntent;
                clipItem->GetIntent((IIntent**)&clipIntent);
                if (clipIntent != NULL) {
                    AutoPtr<NeededUriGrants> newNeeded = CheckGrantUriPermissionFromIntentLocked(
                            callingUid, targetPkg, clipIntent, mode, needed);
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
    /* [in] */ UriPermissionOwner* owner)
{
    Int32 flags;
    AutoPtr<NeededUriGrants> needed = CheckGrantUriPermissionFromIntentLocked(callingUid, targetPkg,
            intent, intent != NULL ? (intent->GetFlags(&flags), flags) : 0, NULL);
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
    /* [in] */ Int32 modeFlags)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("grantUriPermission")));
    AutoLock lock(this);

    AutoPtr<ProcessRecord> r = GetRecordForAppLocked(caller);
    if (r == NULL) {
        // throw new SecurityException("Unable to find app for caller "
        // + caller
        // + " when granting permission to uri " + uri);
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

    GrantUriPermissionLocked(r->mUid, targetPkg, uri, modeFlags, NULL);
    return NOERROR;
}

ECode CActivityManagerService::RemoveUriPermissionIfNeededLocked(
    /* [in] */ UriPermission* perm)
{
    if ((perm->mModeFlags & (IIntent::FLAG_GRANT_READ_URI_PERMISSION
          | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION)) == 0) {
        UriPermissionIterator it = mGrantedUriPermissions.Find(perm->mUid);
        if (it != mGrantedUriPermissions.End()) {
            if (DEBUG_URI_PERMISSION) {
                Slogger::V(TAG, "Removing %d permission to %p", perm->mUid, perm->mUri.Get());
            }
            it->mSecond->Erase(perm->mUri);
            if (it->mSecond->Begin() == it->mSecond->End()) {
                mGrantedUriPermissions.Erase(perm->mUid);
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::RevokeUriPermissionLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
           | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    if (modeFlags == 0) {
       return NOERROR;
    }

    if (DEBUG_URI_PERMISSION) {
        String str;
        uri->ToString(&str);
        Slogger::V(TAG, "Revoking all granted permissions to %s", str.string());
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    String authority;
    uri->GetAuthority(&authority);
    AutoPtr<IProviderInfo> pi;
    Int32 userId = UserHandleGetUserId(callingUid);
    AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByName(authority, userId);
    if (cpr != NULL) {
        pi = cpr->mInfo;
    }
    else {
        // try {
        pm->ResolveContentProvider(authority,
            IPackageManager::GET_URI_PERMISSION_PATTERNS, userId, (IProviderInfo**)&pi);
        // }
        // catch (RemoteException ex) {
        // }
    }
    if (pi == NULL) {
        String uriString;
        uri->ToSafeString(&uriString);
        Slogger::W(TAG, "No content provider found for permission revoke: %s", uriString.string());
        return NOERROR;
    }

    // Does the caller have this permission on the URI?
    if (!CheckHoldingPermissionsLocked(pm, pi, uri, callingUid, modeFlags)) {
        // Right now, if you are not the original owner of the permission,
        // you are not allowed to revoke it.
        //if (!checkUriPermissionLocked(uri, callingUid, modeFlags)) {
        // throw new SecurityException("Uid " + callingUid
        //         + " does not have permission to uri " + uri);
        return E_SECURITY_EXCEPTION;
        //}
    }

    // Go through all of the permissions and remove any that match.
    AutoPtr<ArrayOf<String> > SEGMENTS;
    uri->GetPathSegments((ArrayOf<String>**)&SEGMENTS);
    if (SEGMENTS != NULL) {
        Int32 NS = SEGMENTS->GetLength();
        Int32 N = mGrantedUriPermissions.GetSize();
        UriPermissionIterator it;
        for (it = mGrantedUriPermissions.Begin(); it != mGrantedUriPermissions.End(); ++it) {
           AutoPtr<PermissionMap> perms = it->mSecond;
           typename PermissionMap::Iterator pit = perms->Begin();
        toploop:
            while (pit != perms->End()) {
                AutoPtr<UriPermission> perm = pit->mSecond;
                AutoPtr<IUri> targetUri = perm->mUri;
                String targetAuthority;
                targetUri->GetAuthority(&targetAuthority);
                if (authority != targetAuthority) {
                    ++pit;
                    continue;
                }
                AutoPtr<ArrayOf<String> > targetSegments;
                targetUri->GetPathSegments((ArrayOf<String>**)&targetSegments);
                if (targetSegments == NULL) {
                    ++pit;
                    continue;
                }
                if (targetSegments->GetLength() < NS) {
                    ++pit;
                    continue;
                }
                for (Int32 j=0; j < NS; j++) {
                    if (SEGMENTS[j] != targetSegments[j]) {
                        ++pit;
                        goto toploop;
                    }
                }
                if (DEBUG_URI_PERMISSION) {
                   Slogger::V(TAG, "Revoking %d permission to %p", perm->mUid, perm->mUri.Get());
                }
                perm->ClearModes(modeFlags);
                if (perm->mModeFlags == 0) {
                    perms->Erase(pit++);
                }
            }
            if (perms->Begin() == perms->End()) {
                UriPermissionIterator itbakup = it;
                mGrantedUriPermissions.Erase(itbakup);
                N--;
            }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::RevokeUriPermission(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IUri* uri,
    /* [in]*/ Int32 modeFlags)
{
    FAIL_RETURN(EnforceNotIsolatedCaller(String("revokeUriPermission")));
    AutoLock lock(this);

    AutoPtr<ProcessRecord> r = GetRecordForAppLocked(caller);
    if (r == NULL) {
        // throw new SecurityException("Unable to find app for caller "
        //        + caller
        //        + " when revoking permission to uri " + uri);
        return E_SECURITY_EXCEPTION;
    }
    if (uri == NULL) {
        Slogger::W(TAG, "revokeUriPermission: null uri");
        return E_INVALID_ARGUMENT;;
    }

    modeFlags &= (IIntent::FLAG_GRANT_READ_URI_PERMISSION
          | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    if (modeFlags == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    String authority;
    uri->GetAuthority(&authority);
    AutoPtr<ContentProviderRecord> cpr = mProviderMap->GetProviderByName(authority, r->mUserId);
    AutoPtr<IProviderInfo> pi;
    if (cpr != NULL) {
        pi = cpr->mInfo;
    }
    else {
        // try {
        pm->ResolveContentProvider(authority,
                IPackageManager::GET_URI_PERMISSION_PATTERNS, r->mUserId, (IProviderInfo**)&pi);
        // }
        // catch (RemoteException ex) {
        // }
    }
    if (pi == NULL) {
        String uriString;
        uri->ToSafeString(&uriString);
        Slogger::W(TAG, "No content provider found for permission revoke: %s", uriString.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    RevokeUriPermissionLocked(r->mUid, uri, modeFlags);

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
    AutoPtr<UriPermissionOwner> owner = new UriPermissionOwner(this, (Handle32)&name);
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
    /* [in] */ Int32 modeFlags)
{
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

    GrantUriPermissionLocked(fromUid, targetPkg, uri, modeFlags, owner);
    return NOERROR;
}

ECode CActivityManagerService::RevokeUriPermissionFromOwner(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode)
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
        owner->RemoveUriPermissionLocked(uri, mode);
    }
    return NOERROR;
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
    const Int64 hiddenAppMem = mProcessList->GetMemLevel(ProcessList::HIDDEN_APP_MIN_ADJ);
    (*outInfo)->SetAvailMem(Process::GetFreeMemory());
    (*outInfo)->SetTotalMem(Process::GetTotalMemory());
    (*outInfo)->SetThreshold(homeAppMem);
    Int64 aMem;
    (*outInfo)->GetAvailMem(&aMem);
    (*outInfo)->SetLowMemory(aMem < (homeAppMem + ((hiddenAppMem-homeAppMem)/2)));
    (*outInfo)->SetHiddenAppThreshold(hiddenAppMem);
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

ECode CActivityManagerService::GetTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [in] */ IThumbnailReceiver* receiver,
    /* [out] */ IObjectContainer** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = NULL;

    AutoPtr<IObjectContainer> listContainer;
    CParcelableObjectContainer::New((IObjectContainer**)&listContainer);

    AutoPtr<PendingThumbnailsRecord> pending;
    AutoPtr<IApplicationThread> topThumbnail;
    AutoPtr<ActivityRecord> topRecord;

   {
        AutoLock lock(this);
        if (localLOGV) Slogger::V(
           TAG, "getTasks: max=%d, flags=%d, receiver=%p", maxNum, flags, receiver);

        if (CheckCallingPermission(Elastos::Droid::Manifest::permission::GET_TASKS)
                != IPackageManager::PERMISSION_GRANTED) {
            if (receiver != NULL) {
                // If the caller wants to wait for pending thumbnails,
                // it ain't gonna get them.
                // try {
                receiver->Finished();
                // } catch (RemoteException ex) {
                // }
            }
            Slogger::W(TAG, "Permission Denial: getTasks() from pid=%d, uid=%d requires Elastos::Droid::Manifest::permission::GET_TASKS",
                    Binder::GetCallingPid(), Binder::GetCallingUid());
            // throw new SecurityException(msg);
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<ActivityRecord> next;
        List< AutoPtr<ActivityRecord> >::ReverseIterator rit = mMainStack->mHistory.RBegin();
        if (rit != mMainStack->mHistory.REnd()) {
            next = *rit;
        }
        AutoPtr<ActivityRecord> top;
        AutoPtr<TaskRecord> curTask;
        Int32 numActivities = 0;
        Int32 numRunning = 0;
        while (rit != mMainStack->mHistory.REnd() && maxNum > 0) {
            AutoPtr<ActivityRecord> r = next;
            ++rit;
            if (rit != mMainStack->mHistory.REnd()) {
                next = *rit;
            }
            else {
                next = NULL;
            }

            // Initialize state for next task if needed.
            if (top == NULL ||
                    (top->mState == ActivityState_INITIALIZING
                        && top->mTask == r->mTask)) {
                top = r;
                curTask = r->mTask;
                numActivities = numRunning = 0;
            }

            // Add 'r' into the current task.
            numActivities++;
            if (r->mApp != NULL && r->mApp->mThread != NULL) {
                numRunning++;
            }

            AutoPtr<IComponentName> component;
            r->mIntent->GetComponent((IComponentName**)&component);
            if (localLOGV) {
                String s;
                component->FlattenToShortString(&s);
                Slogger::V(
                TAG, "%s: task=%p", s.string(), r->mTask.Get());
            }

            // If the next one is a different task, generate a new
            // TaskInfo entry for what we have.
            if (next == NULL || next->mTask != curTask) {
                AutoPtr<IActivityManagerRunningTaskInfo> ci;
                CActivityManagerRunningTaskInfo::New((IActivityManagerRunningTaskInfo**)&ci);
                ci->SetId(curTask->mTaskId);
                ci->SetBaseActivity(component);
                AutoPtr<IComponentName> topCom;
                top->mIntent->GetComponent((IComponentName**)&topCom);
                ci->SetTopActivity(topCom);
                if (top->mThumbHolder != NULL) {
                    ci->SetDescription(top->mThumbHolder->mLastDescription);
                }
                ci->SetNumActivities(numActivities);
                ci->SetNumRunning(numRunning);
                //System.out.println(
                //    "#" + maxNum + ": " + " descr=" + ci.description);
                AutoPtr<IBitmap> thumbnail;
                ci->GetThumbnail((IBitmap**)&thumbnail);
                if (thumbnail == NULL && receiver != NULL) {
                    if (localLOGV) {
                        if (top->mApp != NULL) {
                            Slogger::V(
                            TAG, "State=%pIdle=%d app=%p thr=%p", top->mState, top->mIdle,
                            top->mApp.Get(), top->mApp->mThread.Get());
                        }
                        else {
                            Slogger::V(
                            TAG, "State=%pIdle=%d app=%p thr=NULL", top->mState, top->mIdle,
                            top->mApp.Get());
                        }
                    }
                    if (top->mState == ActivityState_RESUMED
                            || top->mState == ActivityState_PAUSING) {
                        if (top->mIdle && top->mApp != NULL
                            && top->mApp->mThread != NULL) {
                            topRecord = top;
                            topThumbnail = top->mApp->mThread;
                        }
                        else {
                            top->mThumbnailNeeded = TRUE;
                        }
                   }
                   if (pending == NULL) {
                        pending = new PendingThumbnailsRecord(receiver);
                   }
                   pending->mPendingRecords.Insert(top);
                }
                listContainer->Add(ci);
                maxNum--;
                top = NULL;
            }
        }

        if (pending != NULL) {
            mPendingThumbnails.PushBack(pending);
        }
    }

    if (localLOGV) Slogger::V(TAG, "We have pending thumbnails: %p", pending.Get());

    if (topThumbnail != NULL) {
        if (localLOGV) Slogger::V(TAG, "Requesting top thumbnail");
        // try {
        ECode ec = topThumbnail->RequestThumbnail(IBinder::Probe(topRecord->mAppToken));
        if (ec != NOERROR) {
        // } catch (Exception e) {
            Slogger::W(TAG, "Exception thrown when requesting thumbnail%08X", ec);
            SendPendingThumbnail(NULL, IBinder::Probe(topRecord->mAppToken), NULL, NULL, TRUE);
        }
    }

    if (pending == NULL && receiver != NULL) {
        // In this case all thumbnails were available and the client
        // is being asked to be told when the remaining ones come in...
        // which is unusually, since the top-most currently running
        // activity should never have a canned thumbnail!  Oh well.
        // try {
        receiver->Finished();
        // } catch (RemoteException ex) {
        // }
    }

    *tasks = listContainer;
    REFCOUNT_ADD(*tasks);
    return NOERROR;
}

ECode CActivityManagerService::GetRecentTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IObjectContainer** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = NULL;

    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, TRUE, String("GetRecentTasks"), String(NULL), &userId));

    AutoLock lock(this);
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::GET_TASKS,
            String("GetRecentTasks()")));
    Boolean detailed = CheckCallingPermission(
            Elastos::Droid::Manifest::permission::GET_DETAILED_TASKS)
            == IPackageManager::PERMISSION_GRANTED;

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    CParcelableObjectContainer::New(tasks);
    List< AutoPtr<TaskRecord> >::Iterator it;
    for (it = mRecentTasks.Begin(); it != mRecentTasks.End() && maxNum > 0; ++it) {
        AutoPtr<TaskRecord> tr = *it;
        // Only add calling user's recent tasks
        if (tr->mUserId != userId) continue;
        // Return the entry if desired by the caller.  We always return
        // the first entry, because callers always expect this to be the
        // foreground app.  We may filter others if the caller has
        // not supplied RECENT_WITH_EXCLUDED and there is some reason
        // we should exclude the entry.

        Int32 flg;
        if (it == mRecentTasks.Begin()
                || ((flags & IActivityManager::RECENT_WITH_EXCLUDED) != 0)
                || (tr->mIntent == NULL)
                || ((tr->mIntent->GetFlags(&flg), flg & IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS) == 0)) {
            AutoPtr<IActivityManagerRecentTaskInfo> rti;
            CActivityManagerRecentTaskInfo::New((IActivityManagerRecentTaskInfo**)&rti);
            rti->SetId(tr->mNumActivities > 0 ? tr->mTaskId : -1);
            rti->SetPersistentId(tr->mTaskId);
            AutoPtr<IIntent> tmpIntent;
            CIntent::New(tr->mIntent != NULL ? tr->mIntent : tr->mAffinityIntent, (IIntent**)&tmpIntent);
            rti->SetBaseIntent(tmpIntent);
            if (!detailed) {
                AutoPtr<IIntent> baseIntent;
                rti->GetBaseIntent((IIntent**)&baseIntent);
                baseIntent->ReplaceExtras(NULL);
            }
            rti->SetOrigActivity(tr->mOrigActivity);
            rti->SetDescription(tr->mLastDescription);

            if ((flags & IActivityManager::RECENT_IGNORE_UNAVAILABLE) != 0) {
                // Check whether this activity is currently available.
                // try {
                AutoPtr<IComponentName> name;
                rti->GetOrigActivity((IComponentName**)&name);
                AutoPtr<IIntent> baseIntent;
                rti->GetBaseIntent((IIntent**)&baseIntent);
                if (name != NULL) {
                    AutoPtr<IActivityInfo> ai;
                    pm->GetActivityInfo(name, 0, userId, (IActivityInfo**)&ai);
                    if (ai == NULL) {
                        continue;
                    }
                }
                else if (baseIntent != NULL) {
                    AutoPtr<IObjectContainer> container;
                    pm->QueryIntentActivities(baseIntent, String(NULL), 0, userId, (IObjectContainer**)&container);
                    if (container == NULL) {
                        continue;
                    }
                }
                // } catch (RemoteException e) {
                //     // Will never happen.
                // }
            }

            (*tasks)->Add(rti);
            maxNum--;
        }
    }
    return NOERROR;
}

AutoPtr<TaskRecord> CActivityManagerService::TaskForIdLocked(
    /* [in] */ Int32 id)
{
    List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
    for(; it != mRecentTasks.End(); it++) {
        if((*it)->mTaskId == id)
            return *it;
    }
    return NULL;
 }

ECode CActivityManagerService::GetTaskThumbnails(
    /* [in] */ Int32 id,
    /* [out] */ IActivityManagerTaskThumbnails** thumbnails)
{
    VALIDATE_NOT_NULL(thumbnails);
    *thumbnails = NULL;

    {
        AutoLock lock(this);
        FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::READ_FRAME_BUFFER,
                String("getTaskThumbnails()")));
        AutoPtr<TaskRecord> tr = TaskForIdLocked(id);
        if (tr != NULL) {
            AutoPtr<IActivityManagerTaskThumbnails> temp = mMainStack->GetTaskThumbnailsLocked(tr);
            *thumbnails = temp;
            REFCOUNT_ADD(*thumbnails);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::GetTaskTopThumbnail(
    /* [in] */ Int32 id,
    /* [out] */ IBitmap** task)
 {
    SLOGGERD(TAG, "CActivityManagerService::GetTaskTopThumbnail");
    AutoLock lock(this);
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::READ_FRAME_BUFFER,
            String("GetTaskTopThumbnail()")));
    AutoPtr<TaskRecord> tr = TaskForIdLocked(id);
    if (tr != NULL) {
        SLOGGERD(TAG, "CActivityManagerService::GetTaskTopThumbnail 20");
        AutoPtr<IBitmap> bitmap = mMainStack->GetTaskTopThumbnailLocked(tr);
        *task = bitmap;
        REFCOUNT_ADD(*task);
        return NOERROR;
    }
    *task = NULL;
    return NOERROR;
}

ECode CActivityManagerService::RemoveSubTask(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 subTaskIndex,
    /* [out] */ Boolean* removed)
{
    VALIDATE_NOT_NULL(removed);

    AutoLock lock(this);
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::REMOVE_TASKS,
            String("removeSubTask()")));
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    *removed = mMainStack->RemoveTaskActivitiesLocked(taskId, subTaskIndex,
                TRUE) != NULL;
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::CleanUpRemovedTaskLocked(
    /* [in] */ TaskRecord* tr,
    /* [in] */ Int32 flags)
{
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
                HashSet<String>::Iterator pit = proc->mPkgList.Find(pkg);
                if (pit == proc->mPkgList.End()) {
                    continue;
                }
                procs.PushBack(proc);
            }
        }

        // Kill the running processes.
        List<AutoPtr<ProcessRecord> >::Iterator procsIt;
        for (procsIt = procs.Begin(); procsIt != procs.End(); ++procsIt) {
            AutoPtr<ProcessRecord> pr = *procsIt;
            if (pr->mSetSchedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE) {
                Slogger::I(TAG, "Killing %s: remove task", pr->ToShortString().string());
//                 EventLog.writeEvent(EventLogTags.AM_KILL, pr.userId, pr.pid,
//                         pr.processName, pr.setAdj, "remove task");
                pr->mKilledBackground = TRUE;
                Process::KillProcessQuiet(pr->mPid);
            }
            else {
                pr->mWaitingToKill = String("remove task");
            }
        }
    }
    return NOERROR;
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
        FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::REMOVE_TASKS,
                String("removeTask()")));
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<ActivityRecord> r = mMainStack->RemoveTaskActivitiesLocked(taskId, -1, FALSE);
        if (r != NULL) {
            mRecentTasks.Remove(r->mTask);
            CleanUpRemovedTaskLocked(r->mTask, flags);
            *removed = TRUE;
            goto _EXIT_;
        }
        else {
            AutoPtr<TaskRecord> tr;
            List< AutoPtr<TaskRecord> >::Iterator it = mRecentTasks.Begin();
            while (it != mRecentTasks.End()) {
                AutoPtr<TaskRecord> t = *it;
                if (t->mTaskId == taskId) {
                    tr = t;
                    break;
                }
                ++it;
            }

            if (tr != NULL) {
                if (tr->mNumActivities <= 0) {
                    // Caller is just removing a recent task that is
                    // not actively running.  That is easy!
                    mRecentTasks.Erase(it);
                    CleanUpRemovedTaskLocked(tr, flags);
                    *removed = TRUE;
                    goto _EXIT_;
                }
                else {
                    Slogger::W(TAG, "removeTask: task %d does not have activities to remove,  but numActivities=%d: %s",
                        taskId, tr->mNumActivities, tr->ToString().string());
                }
            }
        }

_EXIT_:
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }

    return NOERROR;
}

Int32 CActivityManagerService::FindAffinityTaskTopLocked(
    /* [in] */ Int32 startIndex,
    /* [in] */ const String& affinity)
{
    Int32 j;
    List< AutoPtr<ActivityRecord> >::Iterator it;
    for (j = 0, it = mMainStack->mHistory.Begin(); j <= startIndex; ++j) { ++it; }

    AutoPtr<TaskRecord> startTask = (*it)->mTask;
    AutoPtr<TaskRecord> jt = startTask;

    // First look backwards
    List< AutoPtr<ActivityRecord> >::ReverseIterator rit(it);
    for (j = startIndex - 1; rit != mMainStack->mHistory.REnd(); ++rit, j--) {
        AutoPtr<ActivityRecord> r = *rit;
        if (r->mTask != jt) {
            jt = r->mTask;
            if (affinity.Equals(jt->mAffinity)) {
                return j;
            }
        }
    }

    // Now look forwards
    jt = startTask;
    for (j = startIndex + 1, ++it; it != mMainStack->mHistory.End(); ++it, j++) {
        AutoPtr<ActivityRecord> r = *it;
        if (r->mTask != jt) {
            if (affinity.Equals(jt->mAffinity)) {
                return j;
            }
            jt = r->mTask;
        }
    }

    // Might it be at the top?
    if (affinity.Equals((*mMainStack->mHistory.RBegin())->mTask->mAffinity)) {
        return mMainStack->mHistory.GetSize() - 1;
    }

    return -1;
}

/**
 * TODO: Add mController hook
 */
ECode CActivityManagerService::MoveTaskToFront(
    /* [in] */ Int32 task,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::REORDER_TASKS,
            String("moveTaskToFront()")));

    AutoLock lock(this);

    AutoPtr<IActivityOptionsHelper> aoHelper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
    if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
           Binder::GetCallingUid(), String("Task to front"))) {
        aoHelper->Abort(options);
        return NOERROR;
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<TaskRecord> tr = TaskForIdLocked(task);
    if (tr != NULL) {
        if ((flags & IActivityManager::MOVE_TASK_NO_USER_ACTION) == 0) {
            mMainStack->mUserLeaving = TRUE;
        }
        if ((flags & IActivityManager::MOVE_TASK_WITH_HOME) != 0) {
            // Caller wants the home activity moved with it.  To accomplish this,
            // we'll just move the home task to the top first.
            mMainStack->MoveHomeToFrontLocked();
        }
        mMainStack->MoveTaskToFrontLocked(tr, NULL, options);
        Binder::RestoreCallingIdentity(origId);
        return NOERROR;
    }

    List< AutoPtr<ActivityRecord> >::ReverseIterator rit;
    for (rit = mMainStack->mHistory.RBegin(); rit != mMainStack->mHistory.REnd(); ++rit) {
        AutoPtr<ActivityRecord> hr = *rit;
        if (hr->mTask->mTaskId == task) {
            if ((flags & IActivityManager::MOVE_TASK_NO_USER_ACTION) == 0) {
                mMainStack->mUserLeaving = TRUE;
            }
            if ((flags & IActivityManager::MOVE_TASK_WITH_HOME) != 0) {
                // Caller wants the home activity moved with it.  To accomplish this,
                // we'll just move the home task to the top first.
                mMainStack->MoveHomeToFrontLocked();
            }
            mMainStack->MoveTaskToFrontLocked(hr->mTask, NULL, options);
            Binder::RestoreCallingIdentity(origId);
            return NOERROR;
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(origId);
    // }
    aoHelper->Abort(options);
    return NOERROR;
}

ECode CActivityManagerService::MoveTaskToBack(
    /* [in] */ Int32 task)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::REORDER_TASKS,
           String("moveTaskToBack()")));

    AutoLock lock(this);
    if (mMainStack->mResumedActivity != NULL
           && mMainStack->mResumedActivity->mTask->mTaskId == task) {
       if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
               Binder::GetCallingUid(), String("Task to back"))) {
           return NOERROR;
       }
    }
    const Int64 origId = Binder::ClearCallingIdentity();
    mMainStack->MoveTaskToBackLocked(task, NULL);
    Binder::RestoreCallingIdentity(origId);
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
        Int32 taskId = GetTaskForActivityLocked(token, !nonRoot);
        if (taskId >= 0) {
            *completed = mMainStack->MoveTaskToBackLocked(taskId, NULL);
        }
        Binder::RestoreCallingIdentity(origId);
    }

    return NOERROR;
}

ECode CActivityManagerService::MoveTaskBackwards(
    /* [in] */ Int32 task)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::REORDER_TASKS,
            String("moveTaskBackwards()")));

    AutoLock lock(this);
    if (!CheckAppSwitchAllowedLocked(Binder::GetCallingPid(),
            Binder::GetCallingUid(), String("Task backwards"))) {
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

ECode CActivityManagerService::GetTaskForActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyRoot,
    /* [out] */ Int32* taskId)
{
    VALIDATE_NOT_NULL(taskId);

    AutoLock lock(this);
    *taskId = GetTaskForActivityLocked(token, onlyRoot);
    return NOERROR;
}

Int32 CActivityManagerService::GetTaskForActivityLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyRoot)
{
    List<AutoPtr<ActivityRecord> >::Iterator it;
    AutoPtr<TaskRecord> lastTask;
    for (it = mMainStack->mHistory.Begin(); it != mMainStack->mHistory.End(); ++it) {
        AutoPtr<ActivityRecord> r = *it;
        if (IBinder::Probe(r->mAppToken) == token) {
            if (!onlyRoot || lastTask != r->mTask) {
                return r->mTask->mTaskId;
            }
            return -1;
        }
        lastTask = r->mTask;
    }

    return -1;
}

// =========================================================
// THUMBNAILS
// =========================================================

ECode CActivityManagerService::ReportThumbnail(
    /* [in] */ IBinder* token,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ ICharSequence* description)
{
    //System.out.println("Report thumbnail for " + token + ": " + thumbnail);
    const Int64 origId = Binder::ClearCallingIdentity();
    ECode ec = SendPendingThumbnail(NULL, token, thumbnail, description, TRUE);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::SendPendingThumbnail(
    /* [in] */ ActivityRecord* r,
    /* [in] */ IBinder* token,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ ICharSequence* description,
    /* [in] */ Boolean always)
{
    AutoPtr<TaskRecord> task;
    List< AutoPtr<PendingThumbnailsRecord> > receivers;

    {
        AutoLock lock(this);

        if (r == NULL) {
            r = ActivityRecord::IsInStackLocked(token);
            if (r == NULL) {
                return NOERROR;
            }
        }
        if (thumbnail == NULL && r->mThumbHolder != NULL) {
            thumbnail = r->mThumbHolder->mLastThumbnail;
            description = r->mThumbHolder->mLastDescription;
        }
        if (thumbnail == NULL && !always) {
            // If there is no thumbnail, and this entry is not actually
            // going away, then abort for now and pick up the next
            // thumbnail we get.
            return NOERROR;
        }
        task = r->mTask;

        AutoPtr<ActivityRecord> ar = r;
        List<AutoPtr<PendingThumbnailsRecord> >::Iterator it = mPendingThumbnails.Begin();
        for (; it != mPendingThumbnails.End();) {
            AutoPtr<PendingThumbnailsRecord> pr = *it;
            if (pr->mPendingRecords.Erase(ar)) {
                receivers.PushBack(pr);
                if (pr->mPendingRecords.IsEmpty()) {
                    pr->mFinished = TRUE;
                    it = mPendingThumbnails.Erase(it);
                    continue;
                }
            }

            ++it;
        }
    }

    if (receivers.IsEmpty() == FALSE) {
        List<AutoPtr<PendingThumbnailsRecord> >::Iterator it = receivers.Begin();
        AutoPtr<PendingThumbnailsRecord> pr;
        for (; it != receivers.End(); ++it) {
            pr = *it;
            if (FAILED(pr->mReceiver->NewThumbnail(
                    task != NULL ? task->mTaskId : -1, thumbnail, description))) {
                goto SendPendingThumbnailException;
            }
            if (pr->mFinished) {
                if (FAILED(pr->mReceiver->Finished()))
                    goto SendPendingThumbnailException;
            }
            continue;
SendPendingThumbnailException:
            Slogger::E(TAG, "Exception thrown when sending thumbnail");
        }
    }
    return NOERROR;
}

// =========================================================
// CONTENT PROVIDERS
// =========================================================

ECode CActivityManagerService::GenerateApplicationProvidersLocked(
    /* [in] */ ProcessRecord* app,
    /* [out] */ IObjectContainer** providers)
{
    VALIDATE_NOT_NULL(providers);
    *providers = NULL;

    AutoPtr<IIPackageManager> pkgMgr = AppGlobals::GetPackageManager();
    ECode ec = pkgMgr->QueryContentProviders(
            app->mProcessName, app->mUid,
            STOCK_PM_FLAGS | IPackageManager::GET_URI_PERMISSION_PATTERNS,
            providers);
    if (FAILED(ec)) {
        return ec;
    }
    if (DEBUG_MU)
        Slogger::V(TAG_MU, "generateApplicationProvidersLocked, app.info.uid = %d", app->mUid);
    Int32 userId = app->mUserId;
    if (*providers != NULL) {
        String proName, cpiPkgName, cpiName, cpiCName;
        Boolean singleton;
        Int32 cpiflags;
        Boolean hasNext = FALSE;
        AutoPtr<IObjectEnumerator> enumerator;
        (*providers)->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IProviderInfo> cpi;
            enumerator->Current((IInterface**)&cpi);
            AutoPtr<IApplicationInfo> appInfo;
            cpi->GetProcessName(&proName);
            cpi->GetApplicationInfo((IApplicationInfo**)&appInfo);
            cpi->GetPackageName(&cpiPkgName);
            cpi->GetName(&cpiName);
            cpi->GetFlags(&cpiflags);

            singleton = IsSingleton(proName, appInfo, cpiName, cpiflags);
            if (singleton && UserHandleGetUserId(app->mUid) != 0) {
                // This is a singleton provider, but a user besides the
                // default user is asking to initialize a process it runs
                // in...  well, no, it doesn't actually run in this process,
                // it runs in the process of the default user.  Get rid of it.
                (*providers)->Remove((IInterface*)cpi.Get());
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

            appInfo->GetPackageName(&cpiCName);
            app->AddPackage(cpiCName);
            EnsurePackageDexOpt(cpiCName);
        }
    }

    return NOERROR;
}

/**
 * Check if {@link ProcessRecord} has a possible chance at accessing the
 * given {@link ProviderInfo}. Final permission checking is always done
 * in {@link ContentProvider}.
 */
String CActivityManagerService::CheckContentProviderPermissionLocked(
    /* [in] */ IProviderInfo* cpi,
    /* [in] */ ProcessRecord* r)
{
    String nullStr;
    if (cpi == NULL) {
        return nullStr;
    }

    Int32 callingPid, callingUid;
    if (r != NULL) {
        callingPid = r->mPid;
        callingUid = r->mUid;
    }
    else {
        callingPid = Binder::GetCallingPid();
        callingUid = Binder::GetCallingUid();
    }

    Boolean cpiExported;
    String cpiReadPerm, cpiWritePerm;
    cpi->GetReadPermission(&cpiReadPerm);
    cpi->GetWritePermission(&cpiWritePerm);
    cpi->GetExported(&cpiExported);
    AutoPtr<IApplicationInfo> cpiAppInfo;
    cpi->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
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
            if (CheckComponentPermission(readPerm, callingPid, callingUid,
                    cpiUid, cpiExported)
                    == IPackageManager::PERMISSION_GRANTED) {
                return nullStr;
            }

            String writePerm;
            pp->GetWritePermission(&writePerm);
            if (CheckComponentPermission(writePerm, callingPid, callingUid,
                    cpiUid, cpiExported)
                    == IPackageManager::PERMISSION_GRANTED) {
                return nullStr;
            }
        }
    }

    UriPermissionIterator::Iterator it = mGrantedUriPermissions.Find(callingUid);
    if (it != mGrantedUriPermissions.End()) {
        AutoPtr<PermissionMap> perms = it->mSecond;
        if (perms != NULL) {
            String cpiAuth, auth;
            cpi->GetAuthority(&cpiAuth);

            typename PermissionMap::Iterator pit = perms->Begin();
            for (; pit != perms->End(); ++pit) {
                pit->mFirst->GetAuthority(&auth);
                if (auth.Equals(cpiAuth)) {
                    return nullStr;
                }
            }
        }
    }

    String cpiName;
    cpi->GetName(&cpiName);
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
                    cpr->mInfo->GetProcessName(&pName);
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
        CContentProviderConnection::New((Handle32)cpr, (Handle32)r, (IBinder**)&conn);
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
            cpr->mInfo->GetProcessName(&pName);
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

        // First check if this content provider has been published...
        cpr = mProviderMap->GetProviderByName(name, userId);
        Boolean providerRunning = cpr != NULL;
        if (providerRunning) {
            cpi = cpr->mInfo;
            String msg = CheckContentProviderPermissionLocked(cpi, r);
            if (!msg.IsNull()) {
                Slogger::E(TAG, msg.string());
                return E_SECURITY_EXCEPTION;
            }

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

            // In this case the provider instance already exists, so we can
            // return it right away.
            conn = IncProviderCountLocked(r, cpr, token, stable);
            if (conn != NULL && (conn->mStableCount+conn->mUnstableCount) == 1) {
                if (cpr->mProc != NULL && r->mSetAdj <= ProcessList::PERCEPTIBLE_APP_ADJ) {
                    // If this is a perceptible app accessing the provider,
                    // make sure to count it as being accessed and thus
                    // back up on the LRU list.  This is good because
                    // content providers are often expensive to start.
                    UpdateLruProcessLocked(cpr->mProc, FALSE);
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
                 Boolean success = UpdateOomAdjLocked(cpr->mProc);
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
                     Slogger::I(TAG,
                             "Existing provider %s is crashing; detaching %s",
                             cnflattenString.string(), r->ToString().string());
                     Boolean lastRef = DecProviderCountLocked(conn, cpr, token, stable);
                     AutoPtr<ProcessRecord> tempProc = cpr->mProc;
                     AppDiedLocked(tempProc, cpr->mProc->mPid, cpr->mProc->mThread);
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
            ECode ec = AppGlobals::GetPackageManager()->ResolveContentProvider(name,
                    STOCK_PM_FLAGS | IPackageManager::GET_URI_PERMISSION_PATTERNS, userId,
                    (IProviderInfo**)&cpi);
            if (FAILED(ec) || cpi == NULL) {
                *providerHolder = NULL;
                return ec;
            }

            String cpiProcName, cpiName, cpiPkgName, cpiAppInfoPkgName;
            cpi->GetProcessName(&cpiProcName);
            cpi->GetPackageName(&cpiPkgName);
            cpi->GetName(&cpiName);
            Int32 flags;
            cpi->GetFlags(&flags);
            AutoPtr<IApplicationInfo> cpiAppInfo;
            cpi->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
            cpiAppInfo->GetPackageName(&cpiAppInfoPkgName);

            singleton = IsSingleton(cpiProcName, cpiAppInfo, cpiName, flags);
            if (singleton) {
                userId = 0;
            }
            cpi->SetApplicationInfo(GetAppInfoForUser(cpiAppInfo, userId));

            String msg = CheckContentProviderPermissionLocked(cpi, r);
            if (!msg.IsNull()) {
               Slogger::E(TAG, msg.string());
               return E_SECURITY_EXCEPTION;
            }

            if (!mProcessesReady && !mDidUpdate && !mWaitingUpdate
                    && !cpiProcName.Equals("system")) {
                // If this content provider does not run in the system
                // process, and the system is not yet ready to run other
                // processes, then fail fast instead of hanging.
                Slogger::E(TAG, "Attempt to launch content provider before system ready");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // Make sure that the user who owns this provider is started.  If not,
            // we don't want to allow it to run.
            if (mStartedUsers[userId] == NULL) {
                Int32 uid;
                cpiAppInfo->GetUid(&uid);
                Slogger::W(TAG, "Unable to launch app %s/%d for provider %s:user %d is stopped.",
                        cpiAppInfoPkgName.string(), uid, name.string(), userId);
                *providerHolder = NULL;
                return NOERROR;
            }

            AutoPtr<IComponentName> comp;
            CComponentName::New(cpiPkgName, cpiName, (IComponentName**)&comp);
            cpr = mProviderMap->GetProviderByClass(comp, userId);
            Boolean firstClass = cpr == NULL;
            if (firstClass) {
                AutoPtr<IApplicationInfo> ai;
                ec = AppGlobals::GetPackageManager()->GetApplicationInfo(
                        cpiAppInfoPkgName, STOCK_PM_FLAGS, userId, (IApplicationInfo**)&ai);
                if (FAILED(ec) || (ai == NULL)) {
                    Slogger::W(TAG, "No package info for content provider %s",
                            cpiName.string());
                    *providerHolder = NULL;
                    return ec;
                }
                ai = GetAppInfoForUser(ai, userId);
                cpr = new ContentProviderRecord(this, cpi, ai, comp, singleton);
            }

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
                cpr->mAppInfo->GetName(&cprName);
                Slogger::W(TAG, "LAUNCHING REMOTE PROVIDER (myuid %d pruid %d): %s",
                        r->mUid, cprUid, cprName.string());
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

                AppGlobals::GetPackageManager()->SetPackageStoppedState(
                        cpiAppInfoPkgName, FALSE, userId);

                AutoPtr<IComponentName> component;
                CComponentName::New(cpiAppInfoPkgName, cpiName, (IComponentName**)&component);
                AutoPtr<ProcessRecord> proc = StartProcessLocked(cpiProcName,
                        cpr->mAppInfo, FALSE, 0, String("content provider"), component, FALSE, FALSE);
                if (proc == NULL) {
                    Int32 cpiUid;
                    cpiAppInfo->GetUid(&cpiUid);
                    Slogger::W(TAG, "Unable to launch app %s/%d for provider %s: process is bad",
                            cpiAppInfoPkgName.string(), cpiUid, name.string());
                    *providerHolder = NULL;
                    Binder::RestoreCallingIdentity(origId);
                    return NOERROR;
                }

                cpr->mLaunchingApp = proc;
                mLaunchingProviders.PushBack(cpr);
                Binder::RestoreCallingIdentity(origId);
            }
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
    }
    // Wait for the provider to be published...
    {
        AutoPtr<IThread> thread;
        Thread::Attach((IThread**)&thread);
        cpr->Lock();

        while (cpr->mProvider == NULL) {
            if (cpr->mLaunchingApp == NULL) {
                cpr->Unlock();

                AutoPtr<IApplicationInfo> cpiAppInfo;
                cpi->GetApplicationInfo((IApplicationInfo**)&cpiAppInfo);
                String pkgName;
                cpiAppInfo->GetPackageName(&pkgName);
                Int32 uid;
                cpiAppInfo->GetUid(&uid);
                Slogger::W(TAG, "Unable to launch app %s/%d for provider %s: launching app became null",
                        pkgName.string(), uid, name.string());
    //            EventLog.writeEvent(EventLogTags.AM_PROVIDER_LOST_PROCESS,
    //                    UserHandleGetUserId(cpi.applicationInfo.uid),
    //                    cpi.applicationInfo.packageName,
    //                    cpi.applicationInfo.uid, name);
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

    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
           FALSE, TRUE, String("getContentProvider"), String(NULL), &userId));

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

    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::ACCESS_CONTENT_PROVIDERS_EXTERNALLY,
            String("Do not have permission in call getContentProviderExternal()")));
    FAIL_RETURN(HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(), userId,
            FALSE, TRUE, String("getContentProvider"), String(NULL), &userId));
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
 * @param cpr
 */
ECode CActivityManagerService::RemoveContentProvider(
    /* [in] */ IBinder* connection,
    /* [in] */ Boolean stable)
{
    VALIDATE_NOT_NULL(connection);
    FAIL_RETURN(EnforceNotIsolatedCaller(String("removeContentProvider")));
    {
        AutoLock lock(this);
        AutoPtr<CContentProviderConnection> conn = (CContentProviderConnection*)connection;
        if (conn == NULL) {
            String conDes;
            connection->ToString(&conDes);
            Slogger::W(TAG, "removeContentProvider: %s not a CContentProviderConnection",
                    conDes.string());
            //throw new IllegalArgumentException(msg);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (DecProviderCountLocked(conn, NULL, NULL, stable)) {
            UpdateOomAdjLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::RemoveContentProviderExternal(
    /* [in] */ const String& name,
    /* [in] */ IBinder* token)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::ACCESS_CONTENT_PROVIDERS_EXTERNALLY,
           String("Do not have permission in call removeContentProviderExternal()")));
    return RemoveContentProviderExternalUnchecked(name, token, UserHandleGetCallingUserId());
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
    cpr->mInfo->GetPackageName(&cprPkgName);
    cpr->mInfo->GetName(&cprName);
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
    /* [in] */ IObjectContainer* providers)
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

    Boolean hasNext = FALSE;
    AutoPtr<IObjectEnumerator> enumerator;
    providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    while(enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IContentProviderHolder> src;
        enumerator->Current((IInterface**)&src);

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
        srcProviderInfo->GetName(&srcName);
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
            dst->mInfo->GetPackageName(&dstPkgName);
            dst->mInfo->GetName(&dstName);
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
    AppDiedLocked(proc, proc->mPid, proc->mThread);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CActivityManagerService::InstallSystemProviders()
{
    AutoPtr<IObjectContainer> providers;
    {
        AutoLock(this);
        AutoPtr<ProcessRecord> app = mProcessNames->Get(String("system"), IProcess::SYSTEM_UID);
        GenerateApplicationProvidersLocked(app, (IObjectContainer**)&providers);
        if (providers != NULL) {
            AutoPtr<IObjectEnumerator> enumerator;
            providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext = FALSE;
            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<IInterface> item;
                enumerator->Current((IInterface**)&item);
                AutoPtr<IProviderInfo> pi = IProviderInfo::Probe(item);
                AutoPtr<IApplicationInfo> appInfo;
                pi->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 flags;
                appInfo->GetFlags(&flags);
                String name;
                pi->GetName(&name);
                if ((flags&IApplicationInfo::FLAG_SYSTEM) == 0) {
                    Slogger::W(TAG, "Not installing system proc provider %s: not system .apk", name.string());
                    providers->Remove(item);
                }
            }
        }
    }

    if (providers != NULL) {
        Int32 count;
        providers->GetObjectCount(&count);
        AutoPtr<ArrayOf<IProviderInfo*> > providersArray = ArrayOf<IProviderInfo*>::Alloc(count);

        Int32 i = 0;
        AutoPtr<IObjectEnumerator> enumerator;
        providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> item;
            enumerator->Current((IInterface**)&item);
            AutoPtr<IProviderInfo> pi = IProviderInfo::Probe(item);
            providersArray->Set(i, pi);
            ++i;
        }
        mSystemThread->InstallSystemProviders(providersArray);
    }

    mCoreSettingsObserver = new CoreSettingsObserver(mSelf);

    mUsageStatsService->MonitorPackages();
    return NOERROR;
}

/**
 * Allows app to retrieve the MIME type of a URI without having permission
 * to access its content provider.
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
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(type);

    String nullStr;
    String funcName("getProviderMimeType");
    FAIL_RETURN(EnforceNotIsolatedCaller(funcName));
    HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, FALSE, TRUE, funcName, nullStr, &userId);
    String name;
    uri->GetAuthority(&name);
    const Int64 ident = Binder::ClearCallingIdentity();
    AutoPtr<IContentProviderHolder> holder;

    if (SUCCEEDED(GetContentProviderExternalUnchecked(name, NULL, userId,
            (IContentProviderHolder**)&holder))) {
        if (holder != NULL) {
            AutoPtr<IIContentProvider> provider;
            holder->GetContentProvider((IIContentProvider**)&provider);
            provider->GetType(uri, type);
            RemoveContentProviderExternalUnchecked(name, NULL, userId);
            Binder::RestoreCallingIdentity(ident);
            return NOERROR;
        }
    }
    else {
        String uriDes;
        uri->ToString(&uriDes);
        Slogger::W(TAG, "Content provider dead retrieving %s", uriDes.string());
    }

    if (holder != NULL) {
        RemoveContentProviderExternalUnchecked(name, NULL, userId);
    }

    *type = NULL;
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

// =========================================================
// GLOBAL MANAGEMENT
// =========================================================

AutoPtr<ProcessRecord> CActivityManagerService::NewProcessRecordLocked(
    /* [in] */ IApplicationThread* thread,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ const String& customProcess,
    /* [in] */ Boolean isolated)
{
    String proc;
    if (!customProcess.IsNull()) {
        proc = customProcess;
    }
    else {
        info->GetProcessName(&proc);
    }

    AutoPtr<ProcessRecord> result;

    AutoPtr<BatteryStatsImpl::Uid::Proc> ps;
    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    Int32 uid;
    info->GetUid(&uid);
    if (isolated) {
        Int32 userId = UserHandleGetUserId(uid);
        Int32 stepsLeft = IProcess::LAST_ISOLATED_UID - IProcess::FIRST_ISOLATED_UID + 1;
        uid = 0;

        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        while (TRUE) {
            if (mNextIsolatedProcessUid < IProcess::FIRST_ISOLATED_UID
                    || mNextIsolatedProcessUid > IProcess::LAST_ISOLATED_UID) {
                mNextIsolatedProcessUid = IProcess::FIRST_ISOLATED_UID;
            }
            AutoPtr<IUserHandleHelper> uhHelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
            uhHelper->GetUid(userId, mNextIsolatedProcessUid, &uid);
            mNextIsolatedProcessUid++;

            it = mIsolatedProcesses.Find(uid);
            if (it == mIsolatedProcesses.End()) {
                // No process for this uid, use it.
                break;
            }
            stepsLeft--;
            if (stepsLeft <= 0) {
                return result;
            }
        }
    }
    synchronized (stats) {
        Int32 _uid;
        info->GetUid(&_uid);
        ps = stats->GetProcessStatsLocked(_uid, proc);
    }

    result = new ProcessRecord(ps, thread, info, proc, uid);
    return result;
}

AutoPtr<ProcessRecord> CActivityManagerService::AddAppLocked(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Boolean isolated)
{
    String pname, pkgName;
    info->GetProcessName(&pname);
    info->GetPackageName(&pkgName);
    Int32 uid;
    info->GetUid(&uid);
    AutoPtr<ProcessRecord> app;
    if (!isolated) {
        app = GetProcessRecordLocked(pname, uid);
    }
    else {
        app = NULL;
    }

    if (app == NULL) {
        String nullStr;
        app = NewProcessRecordLocked(NULL, info, nullStr, isolated);
        mProcessNames->Put(pname, app->mUid, app);
        if (isolated) {
            mIsolatedProcesses[app->mUid] = app;
        }
        UpdateLruProcessLocked(app, TRUE);
    }
    // This package really, really can not be stopped.
//    try {
        ECode ec = AppGlobals::GetPackageManager()->SetPackageStoppedState(
                pkgName, FALSE, UserHandleGetUserId(app->mUid));
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
        app->mMaxAdj = ProcessList::PERSISTENT_PROC_ADJ;
    }

    if (app->mThread == NULL &&
        (Find(mPersistentStartingProcesses.Begin(),
                mPersistentStartingProcesses.End(), app) == mPersistentStartingProcesses.End())) {
        mPersistentStartingProcesses.PushBack(app);
        StartProcessLocked(app, String("added application"), app->mProcessName);
    }

    return app;
}

ECode CActivityManagerService::UnhandledBack()
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::FORCE_BACK,
           String("unhandledBack()")));

    AutoLock lock(this);
    Int32 count = mMainStack->mHistory.GetSize();
    if (DEBUG_SWITCH) Slogger::D(
            TAG, "Performing unhandledBack(): stack size = %d", count);
    if (count > 1) {
        const Int64 origId = Binder::ClearCallingIdentity();
        mMainStack->FinishActivityLocked(*mMainStack->mHistory.RBegin(),
                count - 1, IActivity::RESULT_CANCELED, NULL, String("unhandled-back"), TRUE);
        Binder::RestoreCallingIdentity(origId);
    }
    return NOERROR;
}

ECode CActivityManagerService::OpenContentUri(
    /* [in] */ IUri* uri,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("openContentUri")));
    const Int32 userId = UserHandleGetCallingUserId();
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
        contentProvider->OpenFile(uri, String("r"), (IParcelFileDescriptor**)&pfd);
//        } catch (FileNotFoundException e) {
//            // do nothing; pfd will be returned null
//        } finally {
        // Ensure that whatever happens, we clean up the identity state
        pthread_setspecific(sCallerIdentity, NULL);
        identity->Release();
//        }
//
        // We've got the fd now, so we're done with the provider.
        RemoveContentProviderExternalUnchecked(name, NULL, userId);
    }
    else {
        Slogger::D(TAG, "Failed to get provider for authority '%s'", name.string());
    }
    *result = pfd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//     // Actually is sleeping or shutting down or whatever else in the future
//     // is an inactive state.
Boolean CActivityManagerService::IsSleeping()
{
    return mSleeping || mShuttingDown;
}

ECode CActivityManagerService::GoingToSleep()
{
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::DEVICE_POWER)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::DEVICE_POWER);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    mWentToSleep = TRUE;
    UpdateEventDispatchingLocked();

    if (!mSleeping) {
        mSleeping = TRUE;
        mMainStack->StopIfSleepingLocked();

        // Initialize the wake times of all processes.
        CheckExcessivePowerUsageLocked(FALSE);

        mHandler->RemoveMessages(CHECK_EXCESSIVE_WAKE_LOCKS_MSG);
        Boolean result;
        mHandler->SendEmptyMessageDelayed(CHECK_EXCESSIVE_WAKE_LOCKS_MSG, POWER_CHECK_DELAY, &result);
    }
    return NOERROR;
}

ECode CActivityManagerService::Shutdown(
    /* [in]*/ Int32 timeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::SHUTDOWN)
          != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::SHUTDOWN);
        return E_SECURITY_EXCEPTION;
    }

    Boolean timedout = FALSE;

    {
        AutoLock lock(this);
        mShuttingDown = TRUE;
        UpdateEventDispatchingLocked();

        if (mMainStack->mResumedActivity != NULL) {
            mMainStack->StopIfSleepingLocked();
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            const Int64 endTime = now + timeout;
            while (mMainStack->mResumedActivity != NULL
                    || mMainStack->mPausingActivity != NULL) {
                system->GetCurrentTimeMillis(&now);
                Int64 delay = endTime - now;
                if (delay <= 0) {
                    Slogger::W(TAG, "Activity manager shutdown timed out");
                    timedout = TRUE;
                    break;
                }
                // try {
                mLock.Wait();
                // } catch (InterruptedException e) {
                // }
            }
        }
    }

    mUsageStatsService->Shutdown();
    mBatteryStatsService->Shutdown();

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

    AutoPtr<ActivityRecord> r = NULL;

    const Int64 origId = Binder::ClearCallingIdentity();

    {
        AutoLock lock(this);
        r = ActivityRecord::IsInStackLocked(token);
        if (r != NULL) {
            mMainStack->ActivitySleptLocked(r);
        }
    }

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode CActivityManagerService::ComeOutOfSleepIfNeededLocked()
{
    if (!mWentToSleep && !mLockScreenShown) {
        if (mSleeping) {
            mSleeping = FALSE;
            mMainStack->AwakeFromSleepingLocked();
            mStackSupervisor->ResumeTopActivitiesLocked();
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::WakingUp()
{
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::DEVICE_POWER) != IPackageManager::PERMISSION_GRANTED) {
        //throw new SecurityException("Requires permission " + Elastos::Droid::Manifest::permission::DEVICE_POWER);
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(this);
        mWentToSleep = FALSE;
        UpdateEventDispatchingLocked();
        ComeOutOfSleepIfNeededLocked();
    }
    return NOERROR;
}

ECode CActivityManagerService::UpdateEventDispatchingLocked()
{
    mWindowManager->SetEventDispatching(mBooted && !mWentToSleep && !mShuttingDown);
    return NOERROR;
}

ECode CActivityManagerService::SetLockScreenShown(
    /* [in] */ Boolean shown)
{
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::DEVICE_POWER)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::DEVICE_POWER);
        return E_SECURITY_EXCEPTION;
    }

    AutoLock lock(this);
    mLockScreenShown = shown;
    ComeOutOfSleepIfNeededLocked();
    return NOERROR;
}

ECode CActivityManagerService::StopAppSwitches()
{
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::STOP_APP_SWITCHES)
           != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::STOP_APP_SWITCHES);
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::STOP_APP_SWITCHES)
          != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //        + Elastos::Droid::Manifest::permission::STOP_APP_SWITCHES);
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
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& name)
{
    if (mAppSwitchesAllowedTime < SystemClock::GetUptimeMillis()) {
        return TRUE;
    }

    Int32 perm = CheckComponentPermission(
            Elastos::Droid::Manifest::permission::STOP_APP_SWITCHES, callingPid, callingUid, -1, TRUE);
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }

    Slogger::W(TAG, "%s request from %d stopped", name.string(), callingUid);
    return FALSE;
}

ECode CActivityManagerService::SetDebugApp(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean waitForDebugger,
    /* [in] */ Boolean persistent)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_DEBUG_APP,
           String("setDebugApp()")));

    // Note that this is not really thread safe if there are multiple
    // callers into it at the same time, but that's not a situation we
    // care about.
    if (persistent) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        Boolean result;
        settingsGlobal->PutString(
            resolver, ISettingsGlobal::DEBUG_APP,
            packageName, &result);
        settingsGlobal->PutInt32(
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
        const Int64 origId = Binder::ClearCallingIdentity();
        ForceStopPackageLocked(packageName, -1, FALSE, FALSE, TRUE, TRUE,
                IUserHandle::USER_ALL);
        Binder::RestoreCallingIdentity(origId);
    }
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
    Boolean isDebuggable = CString("1").Equals(value);
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
    /* [in] */ const String& profileFile,
    /* [in] */ IParcelFileDescriptor* profileFd,
    /* [in] */ Boolean autoStopProfiler)
{
    AutoLock lock(this);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
    Boolean isDebuggable = CString("1").Equals(value);
    if (!isDebuggable) {
        Int32 flags;
        app->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) == 0) {
            // throw new SecurityException("Process not debuggable: " + app.packageName);
            return E_SECURITY_EXCEPTION;
        }
    }
    mProfileApp = processName;
    mProfileFile = profileFile;
    if (mProfileFd != NULL) {
        // try {
        mProfileFd->Close();
        // } catch (IOException e) {
        // }
        mProfileFd = NULL;
    }
    mProfileFd = profileFd;
    mProfileType = 0;
    mAutoStopProfiler = autoStopProfiler;
    return NOERROR;
}

ECode CActivityManagerService::SetAlwaysFinish(
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_ALWAYS_FINISH,
        String("setAlwaysFinish()")));

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    settingsGlobal->PutInt32(
           resolver,
           ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES, enabled ? 1 : 0, &result);

    AutoLock lock(this);
    mAlwaysFinishActivities = enabled;
    return NOERROR;
}

ECode CActivityManagerService::SetActivityController(
    /* [in] */ IActivityController* controller)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER,
            String("SetActivityController()")));

    AutoLock lock(this);
    mController = controller;
    return NOERROR;
}

ECode CActivityManagerService::IsUserAMonkey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // For now the fact that there is a controller implies
    // we have a monkey.
    AutoLock lock(this);
    *result = mController != NULL;
    return NOERROR;
}

ECode CActivityManagerService::RequestBugReport()
{
    // No permission check because this can't do anything harmful --
    // it will just eventually cause the user to be presented with
    // a UI to select where the bug report goes.
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Set(String("ctl.start"), String("bugreport"));
    return NOERROR;
}

ECode CActivityManagerService::InputDispatchingTimedOut(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean aboveSystem,
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::FILTER_EVENTS)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::FILTER_EVENTS);
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<ProcessRecord> proc;

    // TODO: Unify this code with ActivityRecord.keyDispatchingTimedOut().
    {
        AutoLock lock(this);
        {
            AutoLock lock(mPidsSelfLockedLock);
            proc = mPidsSelfLocked[pid];
        }
        if (proc != NULL) {
            if (proc->mDebugging) {
                *timeout = -1;
                return NOERROR;
            }

            if (mDidDexOpt) {
                // Give more time since we were dexopting.
                mDidDexOpt = FALSE;
                *timeout = -1;
                return NOERROR;
            }

            if (proc->mInstrumentationClass != NULL) {
                AutoPtr<IBundle> info;
                CBundle::New((IBundle**)&info);
                info->PutString(String("shortMsg"), String("keyDispatchingTimedOut"));
                info->PutString(String("longMsg"), String("Timed out while dispatching key event"));
                FinishInstrumentationLocked(proc, IActivity::RESULT_CANCELED, info);
                proc = NULL;
            }
        }
    }

    if (proc != NULL) {
        AppNotResponding(proc, NULL, NULL, aboveSystem, String("keyDispatchingTimedOut"));
        if (proc->mInstrumentationClass != NULL || proc->mUsingWrapper) {
            *timeout = INSTRUMENTATION_KEY_DISPATCHING_TIMEOUT;
            return NOERROR;
        }
    }

    *timeout = KEY_DISPATCHING_TIMEOUT;
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

ECode CActivityManagerService::RegisterProcessObserver(
    /* [in] */ IProcessObserver* observer)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER,
            String("registerProcessObserver()")));
    AutoLock lock(this);
    Boolean registed;
    mProcessObservers->Register(observer, &registed);
    return NOERROR;
}

ECode CActivityManagerService::UnregisterProcessObserver(
    /* [in] */ IProcessObserver* observer)
{
    AutoLock lock(this);
    Boolean unregisted;
    mProcessObservers->Unregister(observer, &unregisted);
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
    AutoPtr<ActivityRecord> r = mMainStack->GetTopRunningActivityLocked(NULL);
    *result = (r != NULL) ? r->mImmersive : FALSE;
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
    lp->SetWidth(IWindowManagerLayoutParams::WRAP_CONTENT);
    lp->SetHeight(IWindowManagerLayoutParams::WRAP_CONTENT);
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
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
    wm->AddView(v, lp);
    return NOERROR;
}

ECode CActivityManagerService::NoteWakeupAlarm(
    /* [in] */ IIIntentSender* sender)
{
    if (sender == NULL || sender->Probe(EIID_PendingIntentRecord) == NULL) {
        return NOERROR;
    }

    AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
    synchronized (stats) {
        if (mBatteryStatsService->IsOnBattery()) {
            mBatteryStatsService->EnforceCallingPermission();
            AutoPtr<CPendingIntentRecord> rec = (CPendingIntentRecord*)sender;
            Int32 MY_UID = Binder::GetCallingUid();
            Int32 uid = rec->mUid == MY_UID ? IProcess::SYSTEM_UID : rec->mUid;
            AutoPtr<BatteryStatsImpl::Uid::Pkg> pkg = stats->GetPackageStatsLocked(uid, rec->mKey->mPackageName);
            pkg->IncWakeupsLocked();
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

        // If the worst oom_adj is somewhere in the hidden proc LRU range,
        // then constrain it so we will kill all hidden procs.
        if (worstType < ProcessList::HIDDEN_APP_MAX_ADJ
                && worstType > ProcessList::HIDDEN_APP_MIN_ADJ) {
            worstType = ProcessList::HIDDEN_APP_MIN_ADJ;
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
           if (adj >= worstType && !proc->mKilledBackground) {
               Slogger::W(TAG, "Killing %p (adj %d): %s", proc.Get(), adj, reason.string());
//                EventLog.writeEvent(EventLogTags.AM_KILL, proc.userId, proc.pid,
//                        proc.processName, adj, reason);
                killed = TRUE;
                proc->mKilledBackground = TRUE;
                Process::KillProcessQuiet((*pids)[i]);
            }
        }
    }
    *result = killed;
    return NOERROR;
}

ECode CActivityManagerService::KillProcessesBelowForeground(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* killed)
{
    VALIDATE_NOT_NULL(killed);

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        // throw new SecurityException("killProcessesBelowForeground() only available to system");
        return E_SECURITY_EXCEPTION;
    }

    return KillProcessesBelowAdj(ProcessList::FOREGROUND_APP_ADJ, reason);
}

Boolean CActivityManagerService::KillProcessesBelowAdj(
    /* [in] */ Int32 belowAdj,
    /* [in] */ const String& reason)
{
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        // throw new SecurityException("killProcessesBelowAdj() only available to system");
        return E_SECURITY_EXCEPTION;
    }

    Boolean killed = FALSE;
    {
        AutoLock lock(mPidsSelfLockedLock);
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
            const Int32 pid = it->mFirst;
            AutoPtr<ProcessRecord> proc = it->mSecond;
            if (proc == NULL) continue;

            const Int32 adj = proc->mSetAdj;
            if (adj > belowAdj && !proc->mKilledBackground) {
                Slogger::W(TAG, "Killing %p (adj %d): %s", proc.Get(), adj, reason.string());
//                 EventLog.writeEvent(EventLogTags.AM_KILL, proc.userId,
//                         proc.pid, proc.processName, adj, reason);
                killed = TRUE;
                proc->mKilledBackground = TRUE;
                Process::KillProcessQuiet(pid);
            }
        }
    }
    return killed;
}

ECode CActivityManagerService::StartRunning(
    /* [in] */ const String& pkg,
    /* [in] */ const String& cls,
    /* [in] */ const String& action,
    /* [in] */ const String& data)
{
   {
        AutoLock lock(this);

        if (mStartRunning) {
            return NOERROR;
        }
        mStartRunning = TRUE;
        if (!pkg.IsNull() && !cls.IsNull()) {
            mTopComponent = NULL;
            CComponentName::New(pkg, cls, (IComponentName**)&mTopComponent);
        }
        mTopAction = !action.IsNull() ? action : IIntent::ACTION_MAIN;
        mTopData = data;
        if (!mSystemReady) {
            return NOERROR;
        }
    }

    return SystemReady(NULL);
}

ECode CActivityManagerService::RetrieveSettings()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    String debugApp;
    global->GetString(
        resolver, ISettingsGlobal::DEBUG_APP, &debugApp);
    Int32 value;
    global->GetInt32(
        resolver, ISettingsGlobal::WAIT_FOR_DEBUGGER, 0, &value);
    Boolean waitForDebugger = value != 0;
    global->GetInt32(
            resolver, ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES, 0, &value);
    Boolean alwaysFinishActivities = value != 0;

    AutoPtr<IConfiguration> configuration;
    CConfiguration::New((IConfiguration**)&configuration);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    settingsSystem->GetConfiguration(resolver, configuration);

    {
        AutoLock lock(this);
        mDebugApp = mOrigDebugApp = debugApp;
        mWaitForDebugger = mOrigWaitForDebugger = waitForDebugger;
        mAlwaysFinishActivities = alwaysFinishActivities;
        // This happens before any activities are started, so we can
        // change mConfiguration in-place.
        UpdateConfigurationLocked(configuration, NULL, FALSE, TRUE);
        if (DEBUG_CONFIGURATION) Slogger::V(TAG, "Initial config: %p", mConfiguration.Get());
    }
    return NOERROR;
}

Boolean CActivityManagerService::TestIsSystemReady()
{
    // no need to synchronize(this) just to read & return the value
    return mSystemReady;
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
    CFile::New(systemDir, String("called_pre_boots.dat"), (IFile**)&fname);
    return fname;
}

AutoPtr<List< AutoPtr<IComponentName> > >
CActivityManagerService::ReadLastDonePreBootReceivers()
{
    AutoPtr<List< AutoPtr<IComponentName> > > lastDoneReceivers = new List< AutoPtr<IComponentName> >();
    AutoPtr<IFile> file = GetCalledPreBootReceiversFile();
    AutoPtr<IFileInputStream> fis;
    // try {
    CFileInputStream::New(file, (IFileInputStream**)&fis);
    AutoPtr<IBufferedInputStream> bufferStream;
    CBufferedInputStream::New(fis, 2048, (IBufferedInputStream**)&bufferStream);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(bufferStream, (IDataInputStream**)&dis);
    Int32 fvers;
    IDataInput::Probe(dis)->ReadInt32(&fvers);
    if (fvers == LAST_DONE_VERSION) {
        String vers;
        IDataInput::Probe(dis)->ReadUTF(&vers);
        String codename;
        IDataInput::Probe(dis)->ReadUTF(&codename);
        String build;
        IDataInput::Probe(dis)->ReadUTF(&build);
        if (Build::VERSION::RELEASE.Equals(vers)
                    && Build::VERSION::CODENAME.Equals(codename)
                    && Build::VERSION::INCREMENTAL.Equals(build)) {
                Int32 num;
                IDataInput::Probe(dis)->ReadInt32(&num);
                while (num > 0) {
                    num--;
                    String pkg;
                    IDataInput::Probe(dis)->ReadUTF(&pkg);
                    String cls;
                    IDataInput::Probe(dis)->ReadUTF(&cls);
                    AutoPtr<IComponentName> component;
                    CComponentName::New(pkg, cls, (IComponentName**)&component);
                    lastDoneReceivers->PushBack(component);
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
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IDataOutputStream> dos;
    // try {
    Slogger::I(TAG, "Writing new set of last done pre-boot receivers...");
    CFileOutputStream::New(file, (IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> buffer;
    CBufferedOutputStream::New(fos, 2048, (IBufferedOutputStream**)&buffer);
    CDataOutputStream::New(buffer, (IDataOutputStream**)&dos);
    IDataOutput::Probe(dos)->WriteInt32(LAST_DONE_VERSION);
    IDataOutput::Probe(dos)->WriteUTF(Build::VERSION::RELEASE);
    IDataOutput::Probe(dos)->WriteUTF(Build::VERSION::CODENAME);
    IDataOutput::Probe(dos)->WriteUTF(Build::VERSION::INCREMENTAL);
    IDataOutput::Probe(dos)->WriteInt32(list->GetSize());
    List< AutoPtr<IComponentName> >::ConstIterator it;
    for (it = list->Begin(); it != list->End(); ++it) {
        String pkgName, className;
        (*it)->GetPackageName(&pkgName);
        (*it)->GetClassName(&className);
        IDataOutput::Probe(dos)->WriteUTF(pkgName);
        IDataOutput::Probe(dos)->WriteUTF(className);
    }
    // } catch (IOException e) {
    //     Slogger::W(TAG, "Failure writing last done pre-boot receivers", e);
    //     file.delete();
    // } finally {
    Boolean isSync;
    FileUtils::Sync(fos, &isSync);
    if (dos != NULL) {
        // try {
        FAIL_RETURN(dos->Close());
        // } catch (IOException e) {
        //     // TODO Auto-generated catch block
        //     e.printStackTrace();
        // }
    }
    // }
    return NOERROR;
}

ECode CActivityManagerService::SystemReady(
    /* [in] */ IRunnable* goingCallback)
{
    {
        AutoLock lock(this);

        if (mSystemReady) {
            if (goingCallback != NULL) goingCallback->Run();
            return NOERROR;
        }

        // Check to see if there are any update receivers to run.
        if (!mDidUpdate) {
            if (mWaitingUpdate) {
                return NOERROR;
            }
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_PRE_BOOT_COMPLETED, (IIntent**)&intent);
            AutoPtr<IObjectContainer> ris;
            // try {
            AppGlobals::GetPackageManager()->QueryIntentReceivers(
                    intent, String(NULL), 0, 0, (IObjectContainer**)&ris);
            // } catch (RemoteException e) {
            // }
            if (ris != NULL) {
                AutoPtr<IObjectEnumerator> enumerator;
                ris->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                List< AutoPtr<IResolveInfo> > risList;
                Boolean hasNext = FALSE;
                while(enumerator->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> object;
                    enumerator->Current((IInterface**)&object);
                    AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(object);
                    risList.PushBack(ri);
                }
                List< AutoPtr<IResolveInfo> >::ReverseIterator rit = risList.RBegin();
                while (rit != risList.REnd()) {
                    AutoPtr<IResolveInfo> ri = *rit;
                    AutoPtr<IActivityInfo> riActivityInfo;
                    ri->GetActivityInfo((IActivityInfo**)&riActivityInfo);
                    AutoPtr<IApplicationInfo> riApplicationInfo;
                    riActivityInfo->GetApplicationInfo((IApplicationInfo**)&riApplicationInfo);
                    Int32 riFlags;
                    riApplicationInfo->GetFlags(&riFlags);
                    if ((riFlags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                        rit = List< AutoPtr<IResolveInfo> >::ReverseIterator(risList.Erase(--rit.GetBase()));
                        ris->Remove(ri);
                    }
                    else ++rit;
                }
                intent->AddFlags(IIntent::FLAG_RECEIVER_BOOT_UPGRADE);

                AutoPtr<List< AutoPtr<IComponentName> > > lastDoneReceivers = ReadLastDonePreBootReceivers();
                AutoPtr<List< AutoPtr<IComponentName> > > doneReceivers = new List< AutoPtr<IComponentName> >();
                List< AutoPtr<IResolveInfo> >::Iterator it = risList.Begin();
                while (it != risList.End()) {
                    AutoPtr<IResolveInfo> ri = *it;
                    AutoPtr<IActivityInfo> ai;
                    ri->GetActivityInfo((IActivityInfo**)&ai);
                    String aiPkgName, aiName;
                    ai->GetPackageName(&aiPkgName);
                    ai->GetName(&aiName);
                    AutoPtr<IComponentName> comp;
                    CComponentName::New(aiPkgName, aiName, (IComponentName**)&comp);
                    if (Find(lastDoneReceivers->Begin(), lastDoneReceivers->End(), comp) != lastDoneReceivers->End()) {
                        it = risList.Erase(it);
                        ris->Remove(ri);
                    }
                    else ++it;
                }

                AutoPtr<ArrayOf<Int32> > users = GetUsersLocked();
                Int32 i = 0;
                for (it = risList.Begin(); it != risList.End(); ++it, ++i) {
                    AutoPtr<IActivityInfo> ai;
                    (*it)->GetActivityInfo((IActivityInfo**)&ai);
                    String aiPkgName, aiName;
                    AutoPtr<IComponentName> comp;
                    CComponentName::New(aiPkgName, aiName, (IComponentName**)&comp);
                    doneReceivers->PushBack(comp);
                    intent->SetComponent(comp);
                    for (Int32 j = 0; j < users->GetLength(); j++) {
                        AutoPtr<IIntentReceiver> finisher;
                        if (i == risList.GetSize() - 1 && j == users->GetLength() - 1) {
                            // TODO
                            // finisher = new FinisherReceiver(this, doneReceivers, goingCallback);
                        }
                        AutoPtr<IComponentName> component;
                        intent->GetComponent((IComponentName**)&component);
                        Slogger::I(TAG, "Sending system update to %p for user %d", component.Get(), (*users)[j]);
                        Int32 result;
                        BroadcastIntentLocked(NULL, String(NULL), intent, String(NULL), finisher,
                                0, String(NULL), NULL, String(NULL), TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID,
                                (*users)[j], &result);
                        if (finisher != NULL) {
                            mWaitingUpdate = TRUE;
                        }
                    }
                }
            }
            if (mWaitingUpdate) {
                return NOERROR;
            }
            mDidUpdate = TRUE;
        }

        mSystemReady = TRUE;
        if (!mStartRunning) {
            return NOERROR;
        }
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

    {
        AutoLock lock(this);
        // Make sure we have no pre-ready processes sitting around.

        if (mFactoryTest == CSystemServer::FACTORY_TEST_LOW_LEVEL) {
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
                ai->GetApplicationInfo((IApplicationInfo**)&app);
                Int32 appFlags;
                app->GetFlags(&appFlags);
                if ((appFlags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                    mTopAction = IIntent::ACTION_FACTORY_TEST;
                    mTopData = NULL;
                    String appPkgName, aiName;
                    app->GetPackageName(&appPkgName);
                    ai->GetName(&aiName);
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

    if (goingCallback != NULL) goingCallback->Run();

    {
        AutoLock lock(this);
        PFL_EX("TODO: systemui is persistent, maxAdj = -12");
        if (mFactoryTest != CSystemServer::FACTORY_TEST_LOW_LEVEL) {
            // try {
            AutoPtr<IObjectContainer> apps;
            AppGlobals::GetPackageManager()->GetPersistentApplications(STOCK_PM_FLAGS, (IObjectContainer**)&apps);
            if (apps != NULL) {
                 AutoPtr<IObjectEnumerator> enumerator;
                 apps->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                 Boolean hasNext = FALSE;
                 while (enumerator->MoveNext(&hasNext), hasNext) {
                     AutoPtr<IInterface> obj;
                     enumerator->Current((IInterface**)&obj);
                     AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
                     String infoPkgName;
                     if (info != NULL) {
                         info->GetPackageName(&infoPkgName);
                         if (!infoPkgName.Equals("android")) AddAppLocked(info, FALSE);
                     }
                 }
            }
            // } catch (RemoteException ex) {
            //     // pm is in same process, this will never happen.
            // }
        }

        // Start up initial activity.
        mBooting = TRUE;

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
            String(NULL), NULL, 0, String(NULL), NULL, String(NULL),
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, mCurrentUserId, &result);

        AutoPtr<IIntent> newIntent;
        CIntent::New(IIntent::ACTION_USER_STARTING, (IIntent**)&newIntent);
        newIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        newIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, mCurrentUserId);
        //TODO
        AutoPtr<IIntentReceiver> receiver;
        // AutoPtr<IIntentReceiver> receiver = new SystemBroadcastReceiver();
        BroadcastIntentLocked(NULL, String(NULL), newIntent,
                String(NULL), receiver, 0, String(NULL), NULL,
                Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS,
                TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
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
    return HandleAppCrashLocked(app);
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
// *                      ActivityManager.AppErrorStateInfo
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
        HandleAppCrashLocked(app);
        Slogger::I(CActivityManagerService::TAG, "Killing %p: user's request", app);
//            EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
//                    app.processName, app.setAdj, "user's request after error");
        Process::KillProcessQuiet(app->mPid);
    }
    return NOERROR;
}

Boolean CActivityManagerService::HandleAppCrashLocked(
    /* [in] */ ProcessRecord* app)
{
    if (mHeadless) {
        Logger::E(TAG, "handleAppCrashLocked: %s", app->mProcessName.string());
        return FALSE;
    }
    Int64 now = SystemClock::GetUptimeMillis();

    Int64 crashTime;
    String processName;
    app->mInfo->GetProcessName(&processName);
    if (!app->mIsolated) {
        crashTime = mProcessCrashTimes->Get(processName, app->mUid);
    }
    else {
        crashTime = -2;
    }
    if (crashTime != -2 && now < crashTime + ProcessList::MIN_CRASH_INTERVAL) {
        // This process loses!
        Slogger::W(TAG, "Process %s has crashed too many times: killing!", processName.string());
    //     EventLog.writeEvent(EventLogTags.AM_PROCESS_CRASHED_TOO_MUCH,
    //             app.userId, app.info.processName, app.uid);
        Int32 i = mMainStack->mHistory.GetSize() - 1;
        List< AutoPtr<ActivityRecord> >::ReverseIterator rit;
        for (rit = mMainStack->mHistory.RBegin(); rit != mMainStack->mHistory.REnd(); ++rit, --i) {
            AutoPtr<ActivityRecord> r = *rit;
            if (r->mApp.Get() == app) {
                AutoPtr<IComponentName> component;
                r->mIntent->GetComponent((IComponentName**)&component);
                String str;
                component->FlattenToShortString(&str);
                Slogger::W(TAG, "  Force finishing activity %s", str.string());
                r->mStack->FinishActivityLocked(r, i, IActivity::RESULT_CANCELED,
                        NULL, String("crashed"), FALSE);
            }
        }
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
                mBadProcesses->Put(processName, app->mUid, now);
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
        AutoPtr<ActivityRecord> r = mMainStack->GetTopRunningActivityLocked(NULL);
        if (r != NULL && r->mApp.Get() == app) {
            // If the top running activity is from this crashing
            // process, then terminate it to avoid getting in a loop.
            AutoPtr<IComponentName> rComponent;
            r->mIntent->GetComponent((IComponentName**)&rComponent);
            String str;
            rComponent->FlattenToShortString(&str);
            Slogger::W(TAG, "  Force finishing activity %s", str.string());
            Int32 index = mMainStack->GetIndexOfActivityLocked(r);
            r->mStack->FinishActivityLocked(r, index,
                    IActivity::RESULT_CANCELED, NULL, String("crashed"), FALSE);
            // Also terminate any activities below it that aren't yet
            // stopped, to avoid a situation where one will get
            // re-start our crashing activity once it gets resumed again.
            index--;
            if (index >= 0) {
                r = mMainStack->mHistory[index];
                if (r->mState == ActivityState_RESUMED
                        || r->mState == ActivityState_PAUSING
                        || r->mState == ActivityState_PAUSED) {
                    if (!r->mIsHomeActivity || mHomeProcess != r->mApp) {
                        AutoPtr<IComponentName> component;
                        r->mIntent->GetComponent((IComponentName**)&component);
                        String str;
                        component->FlattenToShortString(&str);
                        Slogger::W(TAG, "  Force finishing activity %s", str.string());
                        r->mStack->FinishActivityLocked(r, index,
                                IActivity::RESULT_CANCELED, NULL, String("crashed"), FALSE);
                    }
                }
            }
        }
    }

    // Bump up the crash count of any services currently running in the proc.
    if (app->mServices.Begin() != app->mServices.End()) {
        // Any services running in the application need to be placed
        // back in the pending list.
        HashSet< AutoPtr<CServiceRecord> >::Iterator it;
        for (it = app->mServices.Begin(); it != app->mServices.End(); ++it) {
            AutoPtr<CServiceRecord> sr = *it;
            sr->mCrashCount++;
        }
    }

    // If the crashing process is what we consider to be the "home process" and it has been
    // replaced by a third-party app, clear the package preferred activities from packages
    // with a home activity running in the process to prevent a repeatedly crashing app
    // from blocking the user to manually clear the list.
    Int32 homeFlags;
    if (app == mHomeProcess && mHomeProcess->mActivities.Begin() != mHomeProcess->mActivities.End()
                && ((mHomeProcess->mInfo->GetFlags(&homeFlags), homeFlags & IApplicationInfo::FLAG_SYSTEM) == 0)) {
        List< AutoPtr<ActivityRecord> >::Iterator it;
        for (it = mHomeProcess->mActivities.Begin(); it != mHomeProcess->mActivities.End(); ++it) {
            AutoPtr<ActivityRecord> r = *it;
            if (r->mIsHomeActivity) {
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

    return TRUE;
}

ECode CActivityManagerService::StartAppProblemLocked(
    /* [in] */ ProcessRecord* app)
{
    if (app->mUserId == mCurrentUserId) {
        AutoPtr<IApplicationErrorReportHelper> helper;
        CApplicationErrorReportHelper::AcquireSingleton((IApplicationErrorReportHelper**)&helper);
        String pkgName;
        app->mInfo->GetPackageName(&pkgName);
        Int32 Flags;
        app->mInfo->GetFlags(&Flags);
        app->mErrorReportReceiver = NULL;
        helper->GetErrorReportReceiver(
                mContext, pkgName, Flags, (IComponentName**)&app->mErrorReportReceiver);
    }
    else {
        // If this app is not running under the current user, then we
        // can't give it a report button because that would require
        // launching the report UI under a different user.
        app->mErrorReportReceiver = NULL;
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

//     EventLog.writeEvent(EventLogTags.AM_CRASH, Binder::GetCallingPid(),
//             UserHandleGetUserId(Binder::GetCallingUid()), processName,
//             r == null ? -1 : r.info.flags,
//             crashInfo.exceptionClassName,
//             crashInfo.exceptionMessage,
//             crashInfo.throwFileName,
//             crashInfo.throwLineNumber);

    AddErrorToDropBox(String("crash"), r, processName, NULL, NULL, String(NULL), String(NULL), NULL, crashInfo);

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
       Int32 stackFingerprint;
       info->HashCode(&stackFingerprint);
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

    Boolean bufferWasEmpty;
    Boolean needsFlush;
    AutoPtr<StringBuilder> sb;
    if (isSystemApp) sb = mStrictModeBuffer;
    else sb = new StringBuilder(1024);
    {
        if (isSystemApp) mStrictModeBufferLock.Lock();

        bufferWasEmpty = sb->GetLength() == 0;
        AppendDropBoxProcessHeaders(process, processName, *sb);
        sb->AppendCStr("Build: ");
        sb->Append(Build::FINGERPRINT);
        sb->AppendCStr("\n");
        sb->AppendCStr("System-App: ");
        sb->AppendBoolean(isSystemApp);
        sb->AppendCStr("\n");
        sb->AppendCStr("Uptime-Millis: ");

        Int64 violationUptimeMillis;
        info->GetViolationUptimeMillis(&violationUptimeMillis);
        sb->AppendInt64(violationUptimeMillis);
        sb->AppendCStr("\n");
        Int32 violationNumThisLoop;
        info->GetViolationNumThisLoop(&violationNumThisLoop);
        if (violationNumThisLoop != 0) {
            sb->Append(String("Loop-Violation-Number: "));
            sb->Append(violationNumThisLoop);
            sb->AppendCStr("\n");
        }
        Int32 numAnimationsRunning;
        info->GetNumAnimationsRunning(&numAnimationsRunning);
        if (numAnimationsRunning != 0) {
            sb->Append(String("Animations-Running: "));
            sb->Append(numAnimationsRunning);
            sb->AppendCStr("\n");
        }
        String broadcastIntentAction;
        info->GetBroadcastIntentAction(&broadcastIntentAction);
        if (!broadcastIntentAction.IsNull()) {
            sb->Append(String("Broadcast-Intent-Action: "));
            sb->Append(broadcastIntentAction);
            sb->AppendCStr("\n");
        }
        Int32 durationMillis;
        info->GetDurationMillis(&durationMillis);
        if (durationMillis != -1) {
            sb->Append(String("Duration-Millis: "));
            sb->Append(durationMillis);
            sb->AppendCStr("\n");
        }
        Int64 numInstances;
        info->GetNumInstances(&numInstances);
        if (numInstances != -1) {
            sb->Append(String("Instance-Count: "));
            sb->AppendInt64(numInstances);
            sb->AppendCStr("\n");
        }
        AutoPtr<ArrayOf<String> > tags;
        info->GetTags((ArrayOf<String>**)&tags);
        if (tags != NULL) {
            for(Int32 i = 0; i < tags->GetLength(); i++)
            {
                sb->Append(String("Span-Tag: "));
                sb->Append((*tags)[i]);
                sb->AppendCStr("\n");
            }
        }
        sb->AppendCStr("\n");
        AutoPtr<IApplicationErrorReportCrashInfo> crashInfo;
        info->GetCrashInfo((IApplicationErrorReportCrashInfo**)&crashInfo);
        String trace;
        if (crashInfo != NULL && (crashInfo->GetStackTrace(&trace), !trace.IsNull())) {
            sb->Append(trace);
        }
        sb->AppendCStr("\n");

        // Only buffer up to ~64k.  Various logging bits truncate
        // things at 128k.
        needsFlush = (sb->GetLength() > 64 * 1024);

        if (isSystemApp) mStrictModeBufferLock.Unlock();
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
                this, sb, isSystemApp ? &mStrictModeBufferLock : NULL, dbox, dropboxTag);
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
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ProcessRecord> r = FindAppProcess(app, String("WTF"));
    const String processName = app == NULL ? String("system_server")
            : (r == NULL ? String("unknown") : r->mProcessName);

//     EventLog.writeEvent(EventLogTags.AM_WTF,
//             UserHandleGetUserId(Binder::GetCallingUid()), Binder::GetCallingPid(),
//             processName,
//             r == null ? -1 : r.info.flags,
//             tag, crashInfo.exceptionMessage);

    AddErrorToDropBox(String("wtf"), r, processName, NULL, NULL, tag, String(NULL), NULL, crashInfo);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    global->GetInt32(resolver,
            ISettingsGlobal::WTF_IS_FATAL, 0, &value);
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
        HashSet<String>::Iterator it;
        for (it = process->mPkgList.Begin(); it != process->mPkgList.End(); ++it) {
            String pkg = *it;
            sb.Append("Package: ");
            sb.Append(pkg);
            // try {
            AutoPtr<IPackageInfo> pi;
            FAIL_RETURN(pm->GetPackageInfo(pkg, 0, UserHandleGetCallingUserId(), (IPackageInfo**)&pi));
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
        sb->AppendCStr("Activity: ");
        sb->Append(activity->mShortComponentName);
        sb->AppendCStr("\n");
    }
    if (parent != NULL && parent->mApp != NULL && parent->mApp->mPid != process->mPid) {
        sb->AppendCStr("Parent-Process: ");
        sb->Append(parent->mApp->mProcessName);
        sb->AppendCStr("\n");
    }
    if (parent != NULL && parent != activity) {
        sb->AppendCStr("Parent-Activity: ");
        sb->Append(parent->mShortComponentName);
        sb->AppendCStr("\n");
    }
    if (subject != NULL) {
        sb->AppendCStr("Subject: ");
        sb->Append(subject);
        sb->AppendCStr("\n");
    }
    sb->AppendCStr("Build: ");
    sb->Append(Build::FINGERPRINT);
    sb->AppendCStr("\n");
//TODO:
//    if (Debug.isDebuggerConnected()) {
//        sb.Append("Debugger: Connected\n");
//    }
    sb->AppendCStr("\n");

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
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
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
            // try {
            String name = r != NULL ? r->mProcessName : String(NULL);
            Int32 pid = r != NULL ? r->mPid : Binder::GetCallingPid();
            Boolean crashed;
            if (FAILED(mController->AppCrashed(name, pid,
                    shortMsg, longMsg, timeMillis, stackTrace, &crashed))) {
                mController = NULL;
            }
            if (!crashed) {
                Slogger::W(TAG, "Force-killing crashed app %s at watcher's request", name.string());
                Process::KillProcess(pid);
                return NOERROR;
            }
            // } catch (RemoteException e) {
            //     mController = null;
            // }
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
    /* [in] */ Millisecond64 timeMillis,
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
    /* [in] */ Millisecond64 timeMillis,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    if (r->mErrorReportReceiver == NULL) {
        return NULL;
    }

    if (!r->mCrashing && !r->mNotResponding) {
        return NULL;
    }

    AutoPtr<IApplicationErrorReport> report;
    CApplicationErrorReport::New((IApplicationErrorReport**)&report);
    String rPkgName;
    r->mInfo->GetPackageName(&rPkgName);
    report->SetPackageName(rPkgName);
    String rErrorPkgName;
    r->mErrorReportReceiver->GetPackageName(&rErrorPkgName);
    report->SetInstallerPackageName(rErrorPkgName);
    report->SetProcessName(r->mProcessName);
    report->SetTime(timeMillis);
    Int32 rFlags;
    r->mInfo->GetFlags(&rFlags);
    report->SetSystemApp((rFlags & IApplicationInfo::FLAG_SYSTEM) != 0);

    if (r->mCrashing) {
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
    /* [out] */ IObjectContainer** procs)
{
    VALIDATE_NOT_NULL(procs);
    *procs = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getProcessesInErrorState")));
    // assume our apps are happy - lazy create the list
    List<AutoPtr<IActivityManagerProcessErrorStateInfo> > errList;

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 permission;
    helper->CheckUidPermission(
           Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
           Binder::GetCallingUid(), &permission);
    const Boolean allUsers = permission == IPackageManager::PERMISSION_GRANTED;
    Int32 userId = UserHandleGetUserId(Binder::GetCallingUid());

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
                    errList.PushBack(report);
                }
                else {
                    Slogger::W(TAG, "Missing app error report, app = %s crashing = %d notResponding = %d", app->mProcessName.string(),
                            app->mCrashing, app->mNotResponding);
                }
            }
        }
    }

    AutoPtr<IObjectContainer> listContainer;
    if (errList.IsEmpty() == FALSE) {
        CParcelableObjectContainer::New((IObjectContainer**)&listContainer);
        List<AutoPtr<IActivityManagerProcessErrorStateInfo> >::Iterator it;
        for (it != errList.Begin(); it != errList.End(); ++it) {
            listContainer->Add(*it);
        }
    }
    *procs = listContainer;
    REFCOUNT_ADD(*procs);
    return NOERROR;
}

Int32 CActivityManagerService::OomAdjToImportance(
    /* [in] */ Int32 adj,
    /* [in] */ IActivityManagerRunningAppProcessInfo* currApp)
{
    if (adj >= ProcessList::HIDDEN_APP_MIN_ADJ) {
        if (currApp != NULL) {
            currApp->SetLru(adj - ProcessList::HIDDEN_APP_MIN_ADJ + 1);
        }

        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
    }
    else if (adj >= ProcessList::SERVICE_B_ADJ) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE;
    }
    else if (adj >= ProcessList::HOME_APP_ADJ) {
        if (currApp != NULL) {
            currApp->SetLru(0);
        }

        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
    }
    else if (adj >= ProcessList::SERVICE_ADJ) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE;
    }
    else if (adj >= ProcessList::HEAVY_WEIGHT_APP_ADJ) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_CANT_SAVE_STATE;
    }
    else if (adj >= ProcessList::PERCEPTIBLE_APP_ADJ) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_PERCEPTIBLE;
    }
    else if (adj >= ProcessList::VISIBLE_APP_ADJ) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_VISIBLE;
    }
    else {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND;
    }
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

    if (app->mHasActivities) {
        outInfo->GetFlags(&flag);
        outInfo->SetFlags(flag | IActivityManagerRunningAppProcessInfo::FLAG_HAS_ACTIVITIES);
    }

    outInfo->SetLastTrimLevel(app->mTrimMemoryLevel);
    Int32 adj = app->mCurAdj;

    outInfo->SetImportance(OomAdjToImportance(adj, outInfo));
    outInfo->SetImportanceReasonCode(app->mAdjTypeCode);

    return NOERROR;
}

ECode CActivityManagerService::GetRunningAppProcesses(
    /* [out] */ IObjectContainer** appProcs)
{
    VALIDATE_NOT_NULL(appProcs);

    *appProcs = NULL;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("getRunningAppProcesses")));

    // Lazy instantiation of lists
    AutoPtr<IObjectContainer> runList;

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 permission;
    FAIL_RETURN(helper->CheckUidPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
            Binder::GetCallingUid(), &permission));
    Boolean allUsers = permission == IPackageManager::PERMISSION_GRANTED;
    Int32 userId = UserHandleGetUserId(Binder::GetCallingUid());
    {
        AutoLock lock(this);
        // Iterate across all processes
        List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
        for (rit = mLruProcesses.RBegin(); rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if (!allUsers && app->mUserId != userId) {
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
                        currApp->SetImportanceReasonImportance(OomAdjToImportance(
                                    app->mAdjSourceOom, NULL));
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
                if (runList == NULL) {
                     FAIL_RETURN(CParcelableObjectContainer::New((IObjectContainer**)&runList));
                }
                runList->Add(currApp);
            }
        }
    }
    *appProcs = runList;
    REFCOUNT_ADD(*appProcs);
    return NOERROR;
}

ECode CActivityManagerService::GetRunningExternalApplications(
    /* [out] */ IObjectContainer** apps)
{
    VALIDATE_NOT_NULL(apps);
    *apps = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getRunningExternalApplications")));
    AutoPtr<IObjectContainer> runningApps;
    GetRunningAppProcesses((IObjectContainer**)&runningApps);
    AutoPtr<IObjectContainer> retList;
    CParcelableObjectContainer::New((IObjectContainer**)&retList);
    Int32 appsCount;
    if (runningApps != NULL && (runningApps->GetObjectCount(&appsCount), appsCount > 0)) {
        HashSet<String> extList;
        AutoPtr<IObjectEnumerator> appsEnumerator;
        runningApps->GetObjectEnumerator((IObjectEnumerator**)&appsEnumerator);
        Boolean hasNext = FALSE;
        while (appsEnumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            appsEnumerator->Current((IInterface**)&obj);
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
            pm->GetApplicationInfo(pkg, 0, UserHandleGetCallingUserId(), (IApplicationInfo**)&info);
            Int32 infoFlags;
            info->GetFlags(&infoFlags);
            if ((infoFlags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
                retList->Add(info);
            }
           // } catch (RemoteException e) {
           // }
        }
    }
    *apps = retList;
    REFCOUNT_ADD(*apps);
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
            if (it != mPidsSelfLocked.End()) proc = it->mSecond;
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::DUMP)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("Permission Denial: can't dump ActivityManager from from pid=");
        sb += Binder::GetCallingPid();
        sb += ", uid=";
        sb += Binder::GetCallingUid();
        sb += " without permission ";
        sb += "Elastos::Droid::Manifest::permission::DUMP";
        pw->PrintStringln(sb.ToString());
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
        if (CString("-a").Equals(opt)) {
            dumpAll = TRUE;
        }
        else if (CString("-c").Equals(opt)) {
            dumpClient = TRUE;
        }
        else if (CString("-h").Equals(opt)) {
            pw->PrintStringln(String("Activity manager dump options:"));
            pw->PrintStringln(String("  [-a] [-c] [-h] [cmd] ..."));
            pw->PrintStringln(String("  cmd may be one of:"));
            pw->PrintStringln(String("    a[ctivities]: activity stack state"));
            pw->PrintStringln(String("    b[roadcasts] [PACKAGE_NAME] [history [-s]]: broadcast state"));
            pw->PrintStringln(String("    i[ntents] [PACKAGE_NAME]: pending intent state"));
            pw->PrintStringln(String("    p[rocesses] [PACKAGE_NAME]: process state"));
            pw->PrintStringln(String("    o[om]: out of memory management"));
            pw->PrintStringln(String("    prov[iders] [COMP_SPEC ...]: content provider state"));
            pw->PrintStringln(String("    provider [COMP_SPEC]: provider client-side state"));
            pw->PrintStringln(String("    s[ervices] [COMP_SPEC ...]: service state"));
            pw->PrintStringln(String("    service [COMP_SPEC]: service client-side state"));
            pw->PrintStringln(String("    package [PACKAGE_NAME]: all state related to given package"));
            pw->PrintStringln(String("    all: dump all activities"));
            pw->PrintStringln(String("    top: dump the top activity"));
            pw->PrintStringln(String("  cmd may also be a COMP_SPEC to dump activities."));
            pw->PrintStringln(String("  COMP_SPEC may be a component name (com.foo/.myApp),"));
            pw->PrintStringln(String("    a partial substring in a component name, a"));
            pw->PrintStringln(String("    hex object identifier."));
            pw->PrintStringln(String("  -a: include all available server state."));
            pw->PrintStringln(String("  -c: include client state."));
            return NOERROR;
        }
        else {
            pw->PrintStringln(String("Unknown argument: ") + opt + "; use -h for help");
        }
    }

    Int64 origId = Binder::ClearCallingIdentity();
    Boolean more = FALSE;
    // Is the caller requesting to dump a particular piece of data?
    if (opti < args->GetLength()) {
        String cmd = (*args)[opti];
        opti++;
        if (CString("activities").Equals(cmd) || CString("a").Equals(cmd)) {
            AutoLock lock(this);
            DumpActivitiesLocked(fd, pw, args, opti, TRUE, dumpClient, String(NULL));
        }
        else if (CString("broadcasts").Equals(cmd) || CString("b").Equals(cmd)) {
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
        else if (CString("intents").Equals(cmd) || CString("i").Equals(cmd)) {
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
        else if (CString("processes").Equals(cmd) || CString("p").Equals(cmd)) {
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
        else if (CString("oom").Equals(cmd) || CString("o").Equals(cmd)) {
            {
                AutoLock lock(this);
                DumpOomLocked(fd, pw, args, opti, TRUE);
            }
        }
        else if (CString("provider").Equals(cmd)) {
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
                pw->PrintStringln(String("No providers match: ") + name);
                pw->PrintStringln(String("Use -h for help."));
            }
        }
        else if (CString("providers").Equals(cmd) || CString("prov").Equals(cmd)) {
            AutoLock lock(this);
            DumpProvidersLocked(fd, pw, args, opti, TRUE, String(NULL));
        }
        else if (CString("service").Equals(cmd)) {
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
                pw->PrintStringln(String("No services match: ") + name);
                pw->PrintStringln(String("Use -h for help."));
            }
        }
        else if (CString("package").Equals(cmd)) {
            AutoPtr<ArrayOf<String> > newArgs;
            if (opti >= args->GetLength()) {
                pw->PrintStringln(String("package: no package name specified"));
                pw->PrintStringln(String("Use -h for help."));
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
        else if (CString("services").Equals(cmd) || CString("s").Equals(cmd)) {
            AutoLock lock(this);
            mServices->DumpServicesLocked(fd, pw, args, opti, TRUE, dumpClient, String(NULL));
        }
        else {
            // Dumping a single activity?
            if (!DumpActivity(fd, pw, cmd, args, opti, dumpAll)) {
                pw->PrintStringln(String("Bad activity command, or no activities match: ") + cmd);
                pw->PrintStringln(String("Use -h for help."));
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
        Boolean needSep;
        needSep = DumpPendingIntentsLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        if (needSep) {
            pw->PrintStringln(String(" "));
        }
        if (dumpAll) {
            pw->PrintStringln(String("-------------------------------------------------------------------------------"));
        }
        needSep = DumpBroadcastsLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        if (needSep) {
            pw->PrintStringln(String(" "));
        }
        if (dumpAll) {
            pw->PrintStringln(String("-------------------------------------------------------------------------------"));
        }
        needSep = DumpProvidersLocked(fd, pw, args, opti, dumpAll, dumpPackage);
        if (needSep) {
            pw->PrintStringln(String(" "));
        }
        if (dumpAll) {
            pw->PrintStringln(String("-------------------------------------------------------------------------------"));
        }
        needSep = mServices->DumpServicesLocked(fd, pw, args, opti, dumpAll, dumpClient, dumpPackage);
        if (needSep) {
            pw->PrintStringln(String(" "));
        }
        if (dumpAll) {
            pw->PrintStringln(String("-------------------------------------------------------------------------------"));
        }
        needSep = DumpActivitiesLocked(fd, pw, args, opti, dumpAll, dumpClient, dumpPackage);
        if (needSep) {
            pw->PrintStringln(String(" "));
        }
        if (dumpAll) {
            pw->PrintStringln(String("-------------------------------------------------------------------------------"));
        }
        DumpProcessesLocked(fd, pw, args, opti, dumpAll, dumpPackage);
    }
    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

Boolean CActivityManagerService::DumpActivitiesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean dumpClient,
    /* [in] */ const String& dumpPackage)
{
    pw->PrintStringln(String("ACTIVITY MANAGER ACTIVITIES (dumpsys activity activities)"));
    pw->PrintStringln(String("  Main stack:"));
    DumpHistoryList(fd, pw, &mMainStack->mHistory, String("  "), String("Hist"), TRUE, !dumpAll, dumpClient,
            dumpPackage);
    pw->PrintStringln(String(" "));
    pw->PrintStringln(String("  Running activities (most recent first):"));
    DumpHistoryList(fd, pw, &mMainStack->mLRUActivities, String("  "), String("Run"), FALSE, !dumpAll, FALSE,
            dumpPackage);
    if (mMainStack->mWaitingVisibleActivities.Begin() != mMainStack->mWaitingVisibleActivities.End()) {
        pw->PrintStringln(String(" "));
        pw->PrintStringln(String("  Activities waiting for another to become visible:"));
        DumpHistoryList(fd, pw, &mMainStack->mWaitingVisibleActivities, String("  "), String("Wait"), FALSE,
                !dumpAll, FALSE, dumpPackage);
    }
    if (mMainStack->mStoppingActivities.Begin() != mMainStack->mStoppingActivities.End()) {
        pw->PrintStringln(String(" "));
        pw->PrintStringln(String("  Activities waiting to stop:"));
        DumpHistoryList(fd, pw, &mMainStack->mStoppingActivities, String("  "), String("Stop"), FALSE,
                !dumpAll, FALSE, dumpPackage);
    }
    if (mMainStack->mGoingToSleepActivities.Begin() != mMainStack->mGoingToSleepActivities.End()) {
        pw->PrintStringln(String(" "));
        pw->PrintStringln(String("  Activities waiting to sleep:"));
        DumpHistoryList(fd, pw, &mMainStack->mGoingToSleepActivities, String("  "), String("Sleep"), FALSE,
                !dumpAll, FALSE, dumpPackage);
    }
    if (mMainStack->mFinishingActivities.Begin() != mMainStack->mFinishingActivities.End()) {
        pw->PrintStringln(String(" "));
        pw->PrintStringln(String("  Activities waiting to finish:"));
        DumpHistoryList(fd, pw, &mMainStack->mFinishingActivities, String("  "), String("Fin"), FALSE,
                !dumpAll, FALSE, dumpPackage);
    }

    pw->PrintStringln(String(" "));
    if (mMainStack->mPausingActivity != NULL) {
        String pauseString;
        mMainStack->mPausingActivity->ToString(&pauseString);
        pw->PrintStringln(String("  mPausingActivity: ") + pauseString);
    }
    String resumeString;
    mMainStack->mResumedActivity->ToString(&resumeString);
    pw->PrintStringln(String("  mResumedActivity: ") + resumeString);
    String focusedString;
    mFocusedActivity->ToString(&focusedString);
    pw->PrintStringln(String("  mFocusedActivity: ") + focusedString);
    if (dumpAll) {
        String lastString;
        mMainStack->mLastPausedActivity->ToString(&lastString);
        pw->PrintStringln(String("  mLastPausedActivity: ") + lastString);
        StringBuilder sb("  mSleepTimeout: ");
        sb += mMainStack->mSleepTimeout;
        pw->PrintStringln(sb.ToString());
        StringBuilder sb1("  mDismissKeyguardOnNextActivity: ");
        sb1 += mMainStack->mDismissKeyguardOnNextActivity;
        pw->PrintStringln(sb1.ToString());
    }

    if (mRecentTasks.Begin() != mRecentTasks.End()) {
        pw->Println();
        pw->PrintStringln(String("  Recent tasks:"));

        List< AutoPtr<TaskRecord> >::Iterator it;
        Int32 i = 0;
        for (it = mRecentTasks.Begin(); it != mRecentTasks.End(); ++it, ++i) {
            AutoPtr<TaskRecord> tr = *it;
            if (dumpPackage != NULL) {
                String aStr;
                if (tr->mRealActivity == NULL ||
                        (tr->mRealActivity->ToString(&aStr), !dumpPackage.Equals(aStr))) {
                    continue;
                }
            }
            pw->PrintString(String("  * Recent #"));
            pw->PrintInt32(i);
            pw->PrintString(String(": "));
        //TODO:
        //     pw->PrintObjectln(tr);
            if (dumpAll) {
                // TODO:
                // tr->Dump(pw, String("    "));
            }
        }
    }

    if (dumpAll) {
        pw->PrintStringln(String(" "));
        StringBuilder sb("  mCurTask: ");
        sb += mCurTask;
        pw->PrintStringln(sb.ToString());
    }

    return TRUE;
}

Boolean CActivityManagerService::DumpProcessesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Int32 numPers = 0;

    pw->PrintStringln(String("ACTIVITY MANAGER RUNNING PROCESSES (dumpsys activity processes)"));

    if (dumpAll) {
        AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > > > map;
        map = mProcessNames->GetMap();
        HashMap<String, AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > >::Iterator mapIt;
        for (mapIt = map->Begin(); mapIt != map->End(); ++mapIt) {
            AutoPtr<HashMap<Int32, AutoPtr<ProcessRecord> > > procs = mapIt->mSecond;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator procsIt;
            for (procsIt = procs->Begin(); procsIt != procs->End(); ++procsIt) {
                AutoPtr<ProcessRecord> r = procsIt->mSecond;
                String rPkgName;
                r->mInfo->GetPackageName(&rPkgName);
                if (!dumpPackage.IsNull() && !dumpPackage.Equals(rPkgName)) {
                    continue;
                }
                if (!needSep) {
                    pw->PrintStringln(String("  All known processes:"));
                    needSep = TRUE;
                }
                pw->PrintString(r->mPersistent ? String("  *PERS*") : String("  *APP*"));
                    pw->PrintString(String(" UID "));
                    pw->PrintInt32(procsIt->mFirst);
                    pw->PrintString(String(" "));
                    pw->PrintObjectln((IInterface*)r);
                //TODO:
                // r->Dump(pw, String("    "));
                if (r->mPersistent) {
                    numPers++;
                }
            }
        }
    }

    if (mIsolatedProcesses.Begin() != mIsolatedProcesses.End()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Isolated process list (sorted by uid):"));
        HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
        Int32 i = 0;
        for (it = mIsolatedProcesses.Begin(); it != mIsolatedProcesses.End(); ++it, ++i) {
            AutoPtr<ProcessRecord> r = it->mSecond;
            String rPkgName;
            r->mInfo->GetPackageName(&rPkgName);
            if (!dumpPackage.IsNull() && !dumpPackage.Equals(rPkgName)) {
                continue;
            }
            String s;
            s.AppendFormat("%sIsolated #%2d: %s", i, r->ToString().string());
            pw->PrintStringln(s);
        }
    }

    if (mLruProcesses.Begin() != mLruProcesses.End()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Process LRU list (sorted by oom_adj):"));
        DumpProcessOomList(pw, this, &mLruProcesses, String("    "),
                String("Proc"), String("PERS"), FALSE, dumpPackage);
        needSep = TRUE;
    }

    if (dumpAll) {
        {
            AutoLock lock(mPidsSelfLockedLock);
            Boolean printed = FALSE;
            HashMap<Int32, AutoPtr<ProcessRecord> >::Iterator it;
            for (it = mPidsSelfLocked.Begin(); it != mPidsSelfLocked.End(); ++it) {
                AutoPtr<ProcessRecord> r = it->mSecond;
                String rPkgName;
                r->mInfo->GetPackageName(&rPkgName);
                if (!dumpPackage.IsNull() && !dumpPackage.Equals(rPkgName)) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->PrintStringln(String(" "));
                    needSep = TRUE;
                    pw->PrintStringln(String("  PID mappings:"));
                    printed = TRUE;
                }
                pw->PrintString(String("    PID #"));
                pw->PrintInt32(it->mFirst);
                pw->PrintString(String(": "));
                pw->PrintObjectln((IInterface*)r);
            }
        }
    }

    if (mForegroundProcesses.Begin() != mForegroundProcesses.End()) {
        {
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
                String rPkgName;
                if (!dumpPackage.IsNull() && (r == NULL
                        || (r->mInfo->GetPackageName(&rPkgName), !dumpPackage.Equals(rPkgName)))) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->PrintStringln(String(" "));
                    needSep = TRUE;
                    pw->PrintStringln(String("  Foreground Processes:"));
                    printed = TRUE;
                }
                pw->PrintString(String("    PID #"));
                pw->PrintInt32(it->mFirst);
                pw->PrintString(String(": "));
                pw->PrintObjectln((IInterface*)it->mSecond);
            }
        }
    }

    if (mPersistentStartingProcesses.Begin() != mPersistentStartingProcesses.End()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Persisent processes that are starting:"));
        DumpProcessList(pw, this, &mPersistentStartingProcesses, String("    "),
                String("Starting Norm"), String("Restarting PERS"), dumpPackage);
    }

    if (mRemovedProcesses.Begin() != mRemovedProcesses.End()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Processes that are being removed:"));
        DumpProcessList(pw, this, &mRemovedProcesses, String("    "),
                String("Removed Norm"), String("Removed PERS"), dumpPackage);
    }

    if (!mProcessesOnHold.IsEmpty()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Processes that are on old until the system is ready:"));
        DumpProcessList(pw, this, &mProcessesOnHold, String("    "),
                String("OnHold Norm"), String("OnHold PERS"), dumpPackage);
    }

    needSep = DumpProcessesToGc(fd, pw, args, opti, needSep, dumpAll, dumpPackage);

    AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, Int64> > > > tMap = mProcessCrashTimes->GetMap();
    if (tMap->Begin() != tMap->End()) {
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
                String rPkgName;
                if (!dumpPackage.IsNull() && (r == NULL
                        || (r->mInfo->GetPackageName(&rPkgName), !dumpPackage.Equals(rPkgName)))) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->PrintStringln(String(" "));
                    needSep = TRUE;
                    pw->PrintStringln(String("  Time since processes crashed:"));
                    printed = TRUE;
                }
                pw->PrintString(String("    Process "));
                pw->PrintString(pname);
                pw->PrintString(String(" uid "));
                pw->PrintInt32(puid);
                pw->PrintString(String(": last crashed "));
                TimeUtils::FormatDuration(now - uidsIt->mSecond, pw);
                pw->PrintStringln(String(" ago"));
            }
        }
    }

    AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, Int64> > > > badMap = mBadProcesses->GetMap();
    if (badMap->Begin() != badMap->End()) {
        Boolean printed = FALSE;
        HashMap<String, AutoPtr<HashMap<Int32, Int64> > >::Iterator badMapIt;
        for (badMapIt = badMap->Begin(); badMapIt != badMap->End(); ++badMapIt) {
            String pname = badMapIt->mFirst;
            AutoPtr<HashMap<Int32, Int64> > uids = badMapIt->mSecond;
            HashMap<Int32, Int64>::Iterator uidsIt;
            for (uidsIt = uids->Begin(); uidsIt != uids->End(); ++uidsIt) {
                Int32 puid = uidsIt->mFirst;
                AutoPtr<ProcessRecord> r = mProcessNames->Get(pname, puid);
                String rPkgName;
                if (!dumpPackage.IsNull() && (r == NULL
                        || (r->mInfo->GetPackageName(&rPkgName), !dumpPackage.Equals(rPkgName)))) {
                    continue;
                }
                if (!printed) {
                    if (needSep) pw->PrintStringln(String(" "));
                    needSep = TRUE;
                    pw->PrintStringln(String("  Bad processes:"));
                }
                pw->PrintString(String("    Bad process "));
                pw->PrintString(pname);
                pw->PrintString(String(" uid "));
                pw->PrintInt32(puid);
                pw->PrintString(String(": crashed at time "));
                pw->PrintInt64ln(uidsIt->mSecond);
            }
        }
    }

    pw->Println();
    pw->PrintStringln(String("  mStartedUsers:"));
    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator uIt;
    for (uIt = mStartedUsers.Begin(); uIt != mStartedUsers.End(); ++uIt) {
        AutoPtr<UserStartedState> uss = uIt->mSecond;
        pw->PrintString(String("    User #"));
        Int32 id;
        uss->mHandle->GetIdentifier(&id);
        pw->PrintInt32(id);
        pw->PrintString(String(": "));
        uss->Dump(String(""), pw);
    }
    pw->PrintString(String("  mStartedUserArray: ["));
    for (Int32 i = 0; i < mStartedUserArray->GetLength(); i++) {
        pw->PrintString(String(", "));
        pw->PrintInt32((*mStartedUserArray)[i]);
    }
    pw->PrintStringln(String("]"));
    pw->PrintString(String("  mUserLru: ["));
    List<Int32>::Iterator userIt;
    for (userIt = mUserLru.Begin(); userIt != mUserLru.End(); ++userIt) {
        pw->PrintString(String(", "));
        pw->Print(*userIt);
    }
    pw->PrintStringln(String("]"));
    if (dumpAll) {
        pw->PrintString(String("  mStartedUserArray: "));
        StringBuilder sb;
        for (Int32 i = 0; i < mStartedUserArray->GetLength(); ++i) {
            sb += (*mStartedUserArray)[i];
        }
        pw->PrintStringln(sb.ToString());
    }

    pw->PrintStringln(String("  mHomeProcess: ") + (mHomeProcess != NULL ? mHomeProcess->ToString() : "NULL"));
    pw->PrintStringln(String("  mPreviousProcess: ") + (mPreviousProcess != NULL ? mPreviousProcess->ToString() : "NULL"));
    if (dumpAll) {
        StringBuilder sb(128);
        sb.Append("  mPreviousProcessVisibleTime: ");
        TimeUtils::FormatDuration(mPreviousProcessVisibleTime, sb);
        pw->PrintStringln(sb.ToString());
    }
    if (mHeavyWeightProcess != NULL) {
        pw->PrintStringln(String("  mHeavyWeightProcess: ") + mHeavyWeightProcess->ToString());
    }
    String configString;
    mConfiguration->ToString(&configString);
    pw->PrintStringln(String("  mConfiguration: ") + configString);
    if (dumpAll) {
        StringBuilder sb("  mConfigWillChange: ");
        sb += mMainStack->mConfigWillChange;
        pw->PrintStringln(sb.ToString());
        HashMap<String, Int32>& packages = mCompatModePackages->GetPackages();
        if (packages.Begin() != packages.End()) {
            Boolean printed = FALSE;
            HashMap<String, Int32>::Iterator it;
            for (it = packages.Begin(); it != packages.End(); ++it) {
                String pkg = it->mFirst;
                Int32 mode = it->mSecond;
                if (!dumpPackage.IsNull() && !dumpPackage.Equals(pkg)) {
                    continue;
                }
                if (!printed) {
                    pw->PrintStringln(String("  mScreenCompatPackages:"));
                    printed = TRUE;
                }
                pw->PrintString(String("    "));
                pw->PrintString(pkg);
                pw->PrintString(String(": "));
                pw->PrintInt32(mode);
                pw->Println();
            }
        }
    }
    if (mSleeping || mWentToSleep || mLockScreenShown) {
        StringBuilder sb("  mSleeping=");
        sb += mSleeping;
        sb += " mWentToSleep=";
        sb += mWentToSleep;
        sb += " mLockScreenShown ";
        sb += mLockScreenShown;
        pw->PrintStringln(sb.ToString());
    }
    if (mShuttingDown) {
        StringBuilder sb("  mShuttingDown=");
        sb += mShuttingDown;
        pw->PrintStringln(sb.ToString());
    }
    if (!mDebugApp.IsNull() || !mOrigDebugApp.IsNull() || mDebugTransient
            || mOrigWaitForDebugger) {
        StringBuilder sb("  mDebugApp=");
        sb += mDebugApp + "/orig=" + mOrigDebugApp + " mDebugTransient=";
        sb += mDebugTransient;
        sb += " mOrigWaitForDebugger=";
        sb += mOrigWaitForDebugger;
        pw->PrintStringln(sb.ToString());
    }
    if (!mOpenGlTraceApp.IsNull()) {
        pw->PrintStringln(String("  mOpenGlTraceApp=") + mOpenGlTraceApp);
    }
    if (!mProfileApp.IsNull() || mProfileProc != NULL || !mProfileFile.IsNull()
            || mProfileFd != NULL) {
        pw->PrintStringln(String("  mProfileApp=") + mProfileApp + " mProfileProc=" + mProfileProc->ToString());
        StringBuilder sb("  mProfileFile=");
        sb += mProfileFile + " mProfileFd=";
        sb += mProfileFd;
        pw->PrintStringln(sb.ToString());
        StringBuilder sb1("  mProfileType=");
        sb1 += mProfileType;
        sb1 += " mAutoStopProfiler=";
        sb1 += mAutoStopProfiler;
        pw->PrintStringln(sb1.ToString());
    }
    if (mAlwaysFinishActivities || mController != NULL) {
        StringBuilder sb("  mAlwaysFinishActivities=");
        sb += mAlwaysFinishActivities;
        sb += " mController=";
        sb += mController;
        pw->PrintStringln(sb.ToString());
    }
    if (dumpAll) {
        StringBuilder sb("  Total persistent processes: ");
        sb += numPers;
        pw->PrintStringln(sb.ToString());
        StringBuilder sb1("  mStartRunning=");
        sb1 += mStartRunning;
        sb1 += " mProcessesReady=";
        sb1 += mProcessesReady;
        sb1 += " mSystemReady=";
        sb1 += mSystemReady;
        pw->PrintStringln(sb1.ToString());
        StringBuilder sb2("  mBooting=");
        sb2 += mBooting;
        sb2 += " mBooted=";
        sb2 += mBooted;
        sb2 += " mFactoryTest=";
        sb2 += mFactoryTest;
        pw->PrintStringln(sb2.ToString());
        pw->PrintString(String("  mLastPowerCheckRealtime="));
        TimeUtils::FormatDuration(mLastPowerCheckRealtime, pw);
        pw->PrintStringln(String(""));
        pw->PrintString(String("  mLastPowerCheckUptime="));
        TimeUtils::FormatDuration(mLastPowerCheckUptime, pw);
        pw->PrintStringln(String(""));
        StringBuilder sb3("  mGoingToSleep=");
        sb3 += mMainStack->mGoingToSleep;
        pw->PrintStringln(sb3.ToString());
        StringBuilder sb4("  mLaunchingActivity=");
        sb4 += mMainStack->mLaunchingActivity;
        pw->PrintStringln(sb4.ToString());
        StringBuilder sb5("  mAdjSeq=");
        sb5 += mAdjSeq;
        sb5 += " mLruSeq=";
        sb5 += mLruSeq;
        pw->PrintStringln(sb5.ToString());
        StringBuilder sb6("  mNumNonHiddenProcs=");
        sb6 += mNumNonHiddenProcs;
        sb6 += " mNumHiddenProcs=";
        sb6 += mNumHiddenProcs;
        sb6 += " mNumServiceProcs=";
        sb6 += mNumServiceProcs;
        sb6 += " mNewNumServiceProcs=";
        sb6 += mNewNumServiceProcs;
        pw->PrintStringln(sb6.ToString());
    }

    return TRUE;
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
    if (mProcessesToGc.Begin() != mProcessesToGc.End()) {
        Boolean printed = FALSE;
        Int64 now = SystemClock::GetUptimeMillis();
        List< AutoPtr<ProcessRecord> >::Iterator it;
        for (it = mProcessesToGc.Begin(); it != mProcessesToGc.End(); ++it) {
            AutoPtr<ProcessRecord> proc = *it;
            String procPkgName;
            proc->mInfo->GetPackageName(&procPkgName);
            if (!dumpPackage.IsNull() && !dumpPackage.Equals(procPkgName)) {
                continue;
            }
            if (!printed) {
                if (needSep) pw->PrintStringln(String(" "));
                needSep = TRUE;
                pw->PrintStringln(String("  Processes that are waiting to GC:"));
                printed = TRUE;
            }
            pw->PrintString(String("    Process "));
            pw->PrintObjectln((IInterface*)proc.Get());
            pw->PrintString(String("      lowMem="));
            pw->PrintBoolean(proc->mReportLowMemory);
            pw->PrintString(String(", last gced="));
            pw->PrintInt64(now - proc->mLastRequestedGc);
            pw->PrintString(String(" ms ago, last lowMem="));
            pw->PrintInt64(now - proc->mLastLowMemory);
            pw->PrintStringln(String(" ms ago"));

        }
    }
    return needSep;
}

Boolean CActivityManagerService::DumpOomLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    Boolean needSep = FALSE;

    if (mLruProcesses.Begin() != mLruProcesses.End()) {
        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  OOM levels:"));
        pw->PrintString(String("    SYSTEM_ADJ: "));
        pw->PrintInt32ln(ProcessList::SYSTEM_ADJ);
        pw->PrintString(String("    PERSISTENT_PROC_ADJ: "));
        pw->PrintInt32ln(ProcessList::PERSISTENT_PROC_ADJ);
        pw->PrintString(String("    FOREGROUND_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::FOREGROUND_APP_ADJ);
        pw->PrintString(String("    VISIBLE_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::VISIBLE_APP_ADJ);
        pw->PrintString(String("    PERCEPTIBLE_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::PERCEPTIBLE_APP_ADJ);
        pw->PrintString(String("    HEAVY_WEIGHT_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::HEAVY_WEIGHT_APP_ADJ);
        pw->PrintString(String("    BACKUP_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::BACKUP_APP_ADJ);
        pw->PrintString(String("    SERVICE_ADJ: "));
        pw->PrintInt32ln(ProcessList::SERVICE_ADJ);
        pw->PrintString(String("    HOME_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::HOME_APP_ADJ);
        pw->PrintString(String("    PREVIOUS_APP_ADJ: "));
        pw->PrintInt32ln(ProcessList::PREVIOUS_APP_ADJ);
        pw->PrintString(String("    SERVICE_B_ADJ: "));
        pw->PrintInt32ln(ProcessList::SERVICE_B_ADJ);
        pw->PrintString(String("    HIDDEN_APP_MIN_ADJ: "));
        pw->PrintInt32ln(ProcessList::HIDDEN_APP_MIN_ADJ);
        pw->PrintString(String("    HIDDEN_APP_MAX_ADJ: "));
        pw->PrintInt32ln(ProcessList::HIDDEN_APP_MAX_ADJ);

        if (needSep) pw->PrintStringln(String(" "));
        needSep = TRUE;
        pw->PrintStringln(String("  Process OOM control:"));
        DumpProcessOomList(pw, this, &mLruProcesses, String("    "),
                String("Proc"), String("PERS"), TRUE, String(NULL));
        needSep = TRUE;
    }

    needSep = DumpProcessesToGc(fd, pw, args, opti, needSep, dumpAll, String(NULL));

    pw->Println();
    pw->PrintStringln(String("  mHomeProcess: ") + mHomeProcess->ToString());
    pw->PrintStringln(String("  mPreviousProcess: ") + mPreviousProcess->ToString());
    if (mHeavyWeightProcess != NULL) {
        pw->PrintStringln(String("  mHeavyWeightProcess: ") + mHeavyWeightProcess->ToString());
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
    return FALSE;
//     return mProviderMap.dumpProvider(fd, pw, name, args, opti, dumpAll);
}

Boolean CActivityManagerService::DumpActivity(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    List<AutoPtr<ActivityRecord> > activities;

    if (CString("all").Equals(name)) {
        AutoLock lock(this);
        List< AutoPtr<ActivityRecord> >::Iterator it;
        for (it = mMainStack->mHistory.Begin(); it != mMainStack->mHistory.End(); ++it) {
            AutoPtr<ActivityRecord> r1 = *it;
            activities.PushBack(r1);
        }
    }
    else if (CString("top").Equals(name)) {
        AutoLock lock(this);
        if (mMainStack->mHistory.Begin() != mMainStack->mHistory.End()) {
            List< AutoPtr<ActivityRecord> >::ReverseIterator rit = mMainStack->mHistory.RBegin();
            activities.PushBack(*rit);
        }
    }
    else {
        AutoPtr<ItemMatcher> matcher = new ItemMatcher();
        matcher->Build(name);

        {
            AutoLock lock(this);
            List< AutoPtr<ActivityRecord> >::Iterator it;
            for (it = mMainStack->mHistory.Begin(); it != mMainStack->mHistory.End(); ++it) {
                AutoPtr<ActivityRecord> r1 = *it;
                AutoPtr<IComponentName> cname;
                r1->mIntent->GetComponent((IComponentName**)&cname);
                if (matcher->Match(r1, cname)) {
                    activities.PushBack(r1);
                }
            }
        }
    }

    if (activities.Begin() == activities.End()) {
        return FALSE;
    }

    Int32 argsLength = args->GetLength();
    AutoPtr<ArrayOf<String> > newArgs = ArrayOf<String>::Alloc(argsLength - opti);
    if (argsLength > 2) newArgs->Copy(args, opti, argsLength- opti);

    AutoPtr<TaskRecord> lastTask;
    Boolean needSep = FALSE;
    List<AutoPtr<ActivityRecord> >::ReverseIterator rit;
    for (rit = activities.RBegin(); rit != activities.REnd(); ++rit) {
        AutoPtr<ActivityRecord> r = *rit;
        if (needSep) {
            pw->Println();
        }
        needSep = TRUE;
        {
            AutoLock lock(this);
            if (lastTask != r->mTask) {
                lastTask = r->mTask;
                pw->PrintString(String("TASK "));
                pw->PrintString(lastTask->mAffinity);
                pw->PrintString(String(" id="));
                pw->PrintInt32ln(lastTask->mTaskId);
                if (dumpAll) {
                    lastTask->Dump(pw, String("  "));
                }
            }
        }
        DumpActivity(String("  "), fd, pw, *rit, newArgs, dumpAll);
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
        pw->PrintString(prefix);
        pw->PrintString(String("ACTIVITY "));
        pw->PrintString(r->mShortComponentName);
        pw->PrintString(String(" "));
        pw->PrintInt32((Int32)r);
        pw->PrintString(String(" pid="));
        if (r->mApp != NULL) pw->PrintInt32ln(r->mApp->mPid);
        else pw->PrintStringln(String("(not running)"));
        if (dumpAll) {
            r->Dump(pw, innerPrefix);
        }
    }
    if (r->mApp != NULL && r->mApp->mThread != NULL) {
        // flush anything that is already in the PrintWriter since the thread is going
        // to write to the file descriptor directly
        IFlushable::Probe(pw)->Flush();
        // try {
            AutoPtr<TransferPipe> tp = new TransferPipe();
            // try {
            AutoPtr<IFileDescriptor> fileDes;
            tp->GetWriteFd()->GetFileDescriptor((IFileDescriptor**)&fileDes);
            FAIL_RETURN(r->mApp->mThread->DumpActivity(fileDes, IBinder::Probe(r->mAppToken), innerPrefix, args));
            tp->Go(fd);
            // } finally {
            tp->Kill();
            // }
        // } catch (IOException e) {
        //     pw.println(innerPrefix + "Failure while dumping the activity: " + e);
        // } catch (RemoteException e) {
        //     pw.println(innerPrefix + "Got a RemoteException while dumping the activity");
        // }
    }
    return NOERROR;
}

Boolean CActivityManagerService::DumpBroadcastsLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;
    Boolean onlyHistory = FALSE;
    String newDumpPackage = dumpPackage;

    if (newDumpPackage.Equals("history")) {
        if (opti < args->GetLength() && (*args)[opti].Equals("-s")) {
            dumpAll = FALSE;
        }
        onlyHistory = TRUE;
        newDumpPackage = NULL;
    }

    pw->PrintStringln(String("ACTIVITY MANAGER BROADCAST STATE (dumpsys activity broadcasts)"));
    if (!onlyHistory && dumpAll) {
        if (mRegisteredReceivers.Begin() != mRegisteredReceivers.End()) {
            Boolean printed = FALSE;
            HashMap<AutoPtr<IIntentReceiver>, AutoPtr<ReceiverList> >::Iterator it;
            for (it = mRegisteredReceivers.Begin(); it != mRegisteredReceivers.End(); ++it) {
                AutoPtr<ReceiverList> r = it->mSecond;
                String rPkgName;
                r->mApp->mInfo->GetPackageName(&rPkgName);
                if (newDumpPackage != NULL && (r->mApp == NULL ||
                        !newDumpPackage.Equals(rPkgName))) {
                    continue;
                }
                if (!printed) {
                    pw->PrintStringln(String("  Registered Receivers:"));
                    needSep = TRUE;
                    printed = TRUE;
                }
                pw->PrintString(String("  * "));
                pw->PrintObjectln((IInterface*)r);
                // TODO:
                // r->Dump(pw, "    ");
            }
        }

//         if (mReceiverResolver.dump(pw, needSep ?
//                 "\n  Receiver Resolver Table:" : "  Receiver Resolver Table:",
//                 "    ", newDumpPackage, FALSE)) {
//             needSep = TRUE;
//         }
    }

    for (Int32 i = 0; i < mBroadcastQueues->GetLength(); ++i) {
        AutoPtr<BroadcastQueue> q = (*mBroadcastQueues)[i];
        needSep = q->DumpLocked(fd, pw, args, opti, dumpAll, newDumpPackage, needSep);
    }

    needSep = TRUE;

    if (!onlyHistory && !mStickyBroadcasts.IsEmpty() && newDumpPackage == NULL) {
        StickyBroadcastIterator it;
        for (it = mStickyBroadcasts.Begin(); it != mStickyBroadcasts.End(); ++it) {
            if (needSep) {
                pw->Println();
            }
            needSep = TRUE;
            pw->PrintString(String("  Sticky broadcasts for user "));
            pw->PrintInt32(it->mFirst);
            pw->PrintStringln(String(":"));
            AutoPtr<StringBuilder> sb = new StringBuilder(128);
            AutoPtr<StringIntentMap> ent = it->mSecond;
            typename StringIntentMap::Iterator entIt;
            for (entIt = ent->Begin(); entIt != ent->End(); ++entIt) {
                pw->PrintString(String("  * Sticky action "));
                pw->PrintString(entIt->mFirst);
                if (dumpAll) {
                    pw->PrintStringln(String(":"));
                    AutoPtr<StringIntentList> intents = entIt->mSecond;
                    List< AutoPtr<IIntent> >::Iterator intentsIt;
                    for (intentsIt = intents->Begin(); intentsIt != intents->End(); ++intentsIt) {
//                         sb.setLength(0);
                        sb->AppendCStr("    Intent: ");
                        String str;
                        (*intentsIt)->ToShortString((IStringBuilder*)sb->Probe(EIID_IStringBuilder), FALSE, TRUE, FALSE, FALSE);
                        pw->PrintStringln(sb->ToString());
                        AutoPtr<IBundle> bundle;
                        (*intentsIt)->GetExtras((IBundle**)&bundle);
                        if (bundle != NULL) {
                            pw->PrintString(String("      "));
                            String bStr;
                            bundle->ToString(&bStr);
                            pw->PrintStringln(bStr);
                        }
                    }
                }
                else {
                    pw->PrintStringln(String(""));
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
            pw->PrintStringln(sb.ToString());
        }
        pw->PrintStringln(String("  mHandler:"));
//         mHandler.dump(new PrintWriterPrinter(pw), "    ");
        needSep = TRUE;
    }

    return needSep;
}

Boolean CActivityManagerService::DumpProvidersLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = TRUE;

    AutoPtr<ItemMatcher> matcher = new ItemMatcher();
    matcher->Build(args, opti);

    pw->PrintStringln(String("ACTIVITY MANAGER CONTENT PROVIDERS (dumpsys activity providers)"));

//     mProviderMap.dumpProvidersLocked(pw, dumpAll);

    if (mLaunchingProviders.Begin() != mLaunchingProviders.End()) {
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
                if (needSep) pw->PrintStringln(String(" "));
                needSep = TRUE;
                pw->PrintStringln(String("  Launching content providers:"));
                printed = TRUE;
            }
            pw->PrintString(String("  Launching #"));
            pw->PrintInt32(i);
            pw->PrintString(String(": "));
            pw->PrintObjectln((IInterface*)r);
        }
    }

    if (!mGrantedUriPermissions.IsEmpty()) {
        if (needSep) pw->Println();
        needSep = TRUE;
        pw->PrintStringln(String("Granted Uri Permissions:"));
        UriPermissionIterator it = mGrantedUriPermissions.Begin();
        typename PermissionMap::Iterator pIt;
        for (; it != mGrantedUriPermissions.End(); ++it) {
            Int32 uid = it->mFirst;
            AutoPtr<PermissionMap> perms = it->mSecond;
            pw->PrintString(String("  * UID "));
            pw->PrintInt32(uid);
            pw->PrintStringln(String(" holds:"));
            for (pIt = perms->Begin(); pIt != perms->End(); ++pIt) {
                AutoPtr<UriPermission> perm = pIt->mSecond;
                pw->PrintString(String("    "));
                // TODO:
                // pw->Println(perm);
                if (dumpAll) {
                    perm->Dump(pw, String("      "));
                }
            }
        }
        needSep = TRUE;
    }

    return needSep;
}

Boolean CActivityManagerService::DumpPendingIntentsLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;

    if (mIntentSenderRecords.Begin() != mIntentSenderRecords.End()) {
        Boolean printed = FALSE;
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
            if (!printed) {
                pw->PrintStringln(String("ACTIVITY MANAGER PENDING INTENTS (dumpsys activity intents)"));
                printed = TRUE;
            }
            needSep = TRUE;
            if (rec != NULL) {
                pw->PrintString(String("  * "));
                pw->PrintObjectln((IInterface*)rec->Probe(EIID_IInterface));
                if (dumpAll) {
                // TODO:
                //     rec.dump(pw, "    ");
                }
            }
            else {
                pw->PrintString(String("  * "));
            // TODO:
            //     pw.println(ref);
            }
        }
    }

    return needSep;
}

ECode CActivityManagerService::DumpHistoryList(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ List< AutoPtr<ActivityRecord> >* list,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ Boolean complete,
    /* [in] */ Boolean brief,
    /* [in] */ Boolean client,
    /* [in] */ const String& dumpPackage)
{
    AutoPtr<TaskRecord> lastTask;
    Boolean needNL = FALSE;
    String innerPrefix = prefix + "      ";
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(0);
    List< AutoPtr<ActivityRecord> >::ReverseIterator rit;
    Int32 i = 0;
    for (rit = list->RBegin(); rit != list->REnd(); ++rit, ++i) {
        AutoPtr<ActivityRecord> r = *rit;
        if (!dumpPackage.IsNull() && !dumpPackage.Equals(r->mPackageName)) {
            continue;
        }
        const Boolean full = !brief && (complete || !r->IsInHistory());
        if (needNL) {
            pw->PrintStringln(String(" "));
            needNL = FALSE;
        }
        if (lastTask != r->mTask) {
            lastTask = r->mTask;
            pw->PrintString(prefix);
            pw->PrintString(full ? String("* ") : String("  "));
            //TODO:
            // pw->Println(lastTask);
            if (full) {
                lastTask->Dump(pw, prefix + "  ");
            }
            else if (complete) {
                // Complete + brief == give a summary.  Isn't that obvious?!?
                if (lastTask->mIntent != NULL) {
                    pw->PrintString(prefix);
                    pw->PrintString(String("  "));
                    String inStr;
                    lastTask->mIntent->ToInsecureStringWithClip(&inStr);
                    pw->PrintStringln(inStr);
                }
            }
        }
        pw->PrintString(prefix);
        pw->PrintString(full ? String("  * ") : String("    "));
        pw->PrintString(label);
        pw->PrintString(String(" #"));
        pw->PrintInt32(i);
        pw->PrintString(String(": "));
        pw->PrintObjectln(r);
        if (full) {
            r->Dump(pw, innerPrefix);
        }
        else if (complete) {
            // Complete + brief == give a summary.  Isn't that obvious?!?
            pw->PrintString(innerPrefix);
            String inStr;
            r->mIntent->ToInsecureStringWithClip(&inStr);
            pw->PrintStringln(inStr);
            if (r->mApp != NULL) {
                pw->PrintString(innerPrefix);
                pw->PrintObjectln((IInterface*)r->mApp);
            }
        }
        if (client && r->mApp != NULL && r->mApp->mThread != NULL) {
            // flush anything that is already in the PrintWriter since the thread is going
            // to write to the file descriptor directly
            IFlushable::Probe(pw)->Flush();
            // try {
            AutoPtr<TransferPipe> tp = new TransferPipe();
            // try {
            AutoPtr<IFileDescriptor> des;
            tp->GetWriteFd()->GetFileDescriptor((IFileDescriptor**)&des);
            r->mApp->mThread->DumpActivity(des, IBinder::Probe(r->mAppToken), innerPrefix, args);
            // Short timeout, since blocking here can
            // deadlock with the application.
                    tp->Go(fd, 2000);
                // } finally {
                    tp->Kill();
                // }
            // } catch (IOException e) {
            //     pw.println(innerPrefix + "Failure while dumping the activity: " + e);
            // } catch (RemoteException e) {
            //     pw.println(innerPrefix + "Got a RemoteException while dumping the activity");
            // }
            needNL = TRUE;
        }
    }
    return NOERROR;
}

String CActivityManagerService::BuildOomTag(
    /* [in] */ const String& prefix,
    /* [in] */ const String& space,
    /* [in] */ Int32 val,
    /* [in] */ Int32 base)
{
    if (val == base) {
        if (space == NULL) return prefix;
        return prefix + "  ";
    }
    return prefix + "+" + StringUtils::Int32ToString(val-base);
}

Int32 CActivityManagerService::DumpProcessList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ CActivityManagerService* service,
    /* [in] */ List< AutoPtr<ProcessRecord> >* list,
    /* [in] */ const String& prefix,
    /* [in] */ const String& normalLabel,
    /* [in] */ const String& persistentLabel,
    /* [in] */ const String& dumpPackage)
{
    Int32 numPers = 0;
    Int32 i = list->GetSize()-1;
    List< AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = list->RBegin(); rit != list->REnd(); ++rit, --i) {
        AutoPtr<ProcessRecord> r = *rit;
        String rPkgName;
        r->mInfo->GetPackageName(&rPkgName);
        if (dumpPackage != NULL && !dumpPackage.Equals(rPkgName)) {
            continue;
        }
        String s;
        s.AppendFormat("%s%s #%2d: %s",
                prefix.string(), (r->mPersistent ? persistentLabel.string() : normalLabel.string()),
                i, r->ToString().string());
        pw->PrintStringln(s);
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
    return FALSE;
}

AutoPtr<List<AutoPtr<ProcessRecord> > > CActivityManagerService::CollectProcesses(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 start,
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
                if (proc->mPid == pid) {
                    procs->PushBack(proc);
                }
                else if (proc->mProcessName.Equals((*args)[start])) {
                    procs->PushBack(proc);
                }
            }
            if (procs->Begin() == procs->End()) {
                pw->PrintStringln(String("No process found for: ") + (*args)[start]);
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
    AutoPtr< List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, 0, args);
    if (procs == NULL) {
        return NOERROR;
    }

    Int64 uptime = SystemClock::GetUptimeMillis();
    Int64 realtime = SystemClock::GetElapsedRealtime();
    pw->PrintStringln(String("Applications Graphics Acceleration Info:"));
    StringBuilder sb("Uptime: ");
    sb += uptime;
    sb += " Realtime: ";
    sb += realtime;
    pw->PrintStringln(sb.ToString());

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            StringBuilder sb("\n** Graphics info for pid ");
            sb += r->mPid;
            sb += " [";
            sb += r->mProcessName;
            sb += "] **";
            pw->PrintStringln(sb.ToString());
            IFlushable::Probe(pw)->Flush();
            // try {
            AutoPtr<TransferPipe> tp = new TransferPipe();
                // try {
                AutoPtr<IParcelFileDescriptor> writeFd = tp->GetWriteFd();
                AutoPtr<IFileDescriptor> fileDes;
                writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
                r->mThread->DumpGfxInfo(fileDes, args);
                tp->Go(fd);
                // } finally {
                tp->Kill();
                // }
            // } catch (IOException e) {
            //     pw.println("Failure while dumping the app: " + r);
            //     pw.flush();
            // } catch (RemoteException e) {
            //     pw.println("Got a RemoteException while dumping the app " + r);
            //     pw.flush();
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
    AutoPtr<List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, 0, args);
    if (procs == NULL) {
        return NOERROR;
    }

    pw->PrintStringln(String("Applications Database Info:"));

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            StringBuilder sb("\n** Database info for pid ");
            sb += r->mPid;
            sb += " [";
            sb += r->mProcessName;
            sb += "] **";
            pw->PrintStringln(sb.ToString());
            IFlushable::Probe(pw)->Flush();
            // try {
            AutoPtr<TransferPipe> tp = new TransferPipe();
                // try {
                AutoPtr<IParcelFileDescriptor> writeFd = tp->GetWriteFd();
                AutoPtr<IFileDescriptor> fileDes;
                writeFd->GetFileDescriptor((IFileDescriptor**)&fileDes);
                r->mThread->DumpDbInfo(fileDes, args);
                tp->Go(fd);
                // } finally {
                tp->Kill();
                // }
            // } catch (IOException e) {
            //     pw.println("Failure while dumping the app: " + r);
            //     pw.flush();
            // } catch (RemoteException e) {
            //     pw.println("Got a RemoteException while dumping the app " + r);
            //     pw.flush();
            // }
        }
    }
    return NOERROR;
}

ECode CActivityManagerService::DumpMemItems(
   /* [in] */ IPrintWriter* pw,
   /* [in] */ const String& prefix,
   /* [in] */ List< AutoPtr<MemItem> >* items,
   /* [in] */ Boolean sort)
{
    List< AutoPtr<MemItem> >::Iterator it;
    AutoPtr<MemItem> item;
    if (sort) {
        Int32 size = items->GetSize();
        if (size > 1) {
            Boolean finished = TRUE;
            List< AutoPtr<MemItem> >::Iterator nextIt;
            AutoPtr<MemItem> nextItem;
            for (Int32 i = 0; i < size; ++i) {
                finished = TRUE;

                it = items->Begin();
                nextIt = items->Begin();
                ++nextIt;
                for (Int32 j = 0; j < size - 1; ++j, ++it, ++nextIt) {
                    item = *it;
                    nextItem = *nextIt;
                    if (item->mPss > nextItem->mPss) {
                        finished = FALSE;
                        *it = nextItem;
                        *nextIt = item;
                    }
                }

                if (finished) {
                    break;
                }
            }
        }
    }

    for (it = items->Begin(); it != items->End(); ++it) {
        item = *it;
        pw->PrintString(prefix);
        pw->PrintInt32(item->mPss);
        pw->PrintString(String(" kB: "));
        pw->PrintStringln(item->mLabel);
        if (item->mSubitems != NULL) {
            DumpMemItems(pw, prefix + "           ", item->mSubitems, TRUE);
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
    if (start >= 0) start++;
    else start = 0;
    Int32 end = label.GetLength();
    for (Int32 i=0; i<DUMP_MEM_BUCKETS->GetLength(); i++) {
        if ((*DUMP_MEM_BUCKETS)[i] >= memKB) {
            Int64 bucket = (*DUMP_MEM_BUCKETS)[i]/1024;
            out.AppendInt64(bucket);
            out.AppendCStr(stackLike ? "MB." : "MB ");
            AutoPtr<ICharSequence> cs;
            CString::New(label, (ICharSequence**)&cs);
            out.AppendCharSequence(cs, start, end);
            return NOERROR;
        }
    }
    out.AppendInt64(memKB/1024);
    out.AppendCStr(stackLike ? "MB." : "MB ");
    AutoPtr<ICharSequence> cs;
    CString::New(label, (ICharSequence**)&cs);
    out.AppendCharSequence(cs, start, end);
    return NOERROR;
}

ECode CActivityManagerService::DumpApplicationMemoryUsage(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean brief,
    /* [in] */ IPrintWriter* categoryPw,
    /* [in] */ StringBuilder& outTag,
    /* [in] */ StringBuilder& outStack)
{
    if (args == NULL) {
        return NOERROR;
    }

    Slogger::D(TAG, "======================DumpApplicationMemoryUsage====================");
    Boolean dumpAll = FALSE;
    Boolean oomOnly = FALSE;

    Int32 opti = 0;
    String opt;
    while (opti < args->GetLength()) {
        opt = (*args)[opti];
        if (opt.IsNullOrEmpty() || opt.GetChar(0) != '-') {
            break;
        }
        opti++;
        if (opt.Equals("-a")) {
            dumpAll = TRUE;
        }
        else if (opt.Equals("--oom")) {
            oomOnly = TRUE;
        }
        else if (opt.Equals("-h")) {
            pw->PrintStringln(String("meminfo dump options: [-a] [--oom] [process]"));
            pw->PrintStringln(String("  -a: include all available information for each process."));
            pw->PrintStringln(String("  --oom: only show processes organized by oom adj."));
            pw->PrintStringln(String("If [process] is specified it can be the name or "));
            pw->PrintStringln(String("pid of a specific process to dump."));
            return NOERROR;
        }
        else {
            pw->PrintStringln(String("Unknown argument: ") + opt + "; use -h for help");
        }
    }

    AutoPtr<List<AutoPtr<ProcessRecord> > > procs = CollectProcesses(pw, opti, args);
    if (procs == NULL) {
        return NOERROR;
    }

    const Boolean isCheckinRequest = ScanArgs(args, String("--checkin"));
    Int64 uptime = SystemClock::GetUptimeMillis();
    Int64 realtime = SystemClock::GetElapsedRealtime();

    if (procs->GetSize() == 1 || isCheckinRequest) {
        dumpAll = TRUE;
    }

    if (isCheckinRequest) {
        // short checkin version
        StringBuilder sb("");
        sb += uptime;
        sb += ",";
        sb += realtime;
        pw->PrintStringln(sb.ToString());
        IFlushable::Probe(pw)->Flush();
    }
    else {
        pw->PrintStringln(String("Applications Memory Usage (kB):"));
        StringBuilder sb("Uptime: ");
        sb += uptime;
        sb += " Realtime: ";
        sb += realtime;
        pw->PrintStringln(sb.ToString());
    }

    AutoPtr<ArrayOf<String> > innerArgs = ArrayOf<String>::Alloc(args->GetLength()-opti);
    innerArgs->Copy(args, opti, args->GetLength()-opti);

    typedef List<AutoPtr<MemItem> > MemItemList;
    AutoPtr<MemItemList> procMems = new MemItemList();
    Int64 nativePss=0, dalvikPss=0, otherPss=0;
    AutoPtr<ArrayOf<Int64> > miscPss = ArrayOf<Int64>::Alloc(IDebugMemoryInfo::NUM_OTHER_STATS);

    AutoPtr<ArrayOf<Int64> > oomPss = ArrayOf<Int64>::Alloc(DUMP_MEM_OOM_LABEL->GetLength());
    AutoPtr<ArrayOf<MemItemList* > > oomProcs = ArrayOf<MemItemList* >::Alloc(DUMP_MEM_OOM_LABEL->GetLength());

    Int64 totalPss = 0;
    Int32 mem = 0;
    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);

    List<AutoPtr<ProcessRecord> >::ReverseIterator rit;
    for (rit = procs->RBegin(); rit != procs->REnd(); ++rit) {
        AutoPtr<ProcessRecord> r = *rit;
        if (r->mThread != NULL) {
            if (!isCheckinRequest && dumpAll) {
                StringBuilder sb("\n** MEMINFO in pid ");
                sb += r->mPid;
                sb += " [";
                sb += r->mProcessName;
                sb += "] **";
                pw->PrintStringln(sb.ToString());
                IFlushable::Probe(pw)->Flush();
            }
            AutoPtr<IDebugMemoryInfo> mi;
            if (dumpAll) {
                // try {
                    r->mThread->DumpMemInfo(fd, isCheckinRequest, dumpAll, (ArrayOf<String>*)innerArgs, (IDebugMemoryInfo**)&mi);
                // } catch (RemoteException e) {
                //     if (!isCheckinRequest) {
                //         pw.println("Got RemoteException!");
                //         pw.flush();
                //     }
                // }
            }
            else {
                CDebugMemoryInfo::New((IDebugMemoryInfo**)&mi);
                dbg->GetMemoryInfo(r->mPid, (IDebugMemoryInfo*)mi);
            }

            if (!isCheckinRequest && mi != NULL) {
                Int64 myTotalPss;
                Int64 pss;
                mi->GetTotalPss(&pss);
                myTotalPss = pss;
                totalPss += myTotalPss;
                StringBuilder sb(r->mProcessName);
                sb += " (pid ";
                sb += r->mPid;
                sb += ")";
                AutoPtr<MemItem> pssItem = new MemItem(sb.ToString(),
                    r->mProcessName, myTotalPss, 0);
                procMems->PushBack(pssItem);

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

                for (Int32 oomIndex = 0; oomIndex < oomPss->GetLength(); oomIndex++) {
                    if (r->mSetAdj <= (*DUMP_MEM_OOM_ADJ)[oomIndex]
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

    if (!isCheckinRequest && procs->GetSize() > 1) {
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
                label = (*DUMP_MEM_OOM_LABEL)[j];
                AutoPtr<MemItem> item = new MemItem(label, label, (*oomPss)[j], (*DUMP_MEM_OOM_ADJ)[j]);
                item->mSubitems = (*oomProcs)[j];
                oomMems->PushBack(item);
            }
        }

        if (outTag.GetLength() != 0 || outStack.GetLength() != 0) {
            if (outTag.GetLength() != 0) {
                AppendMemBucket(outTag, totalPss, String("total"), FALSE);
            }
            if (outStack.GetLength() != 0) {
                AppendMemBucket(outStack, totalPss, String("total"), TRUE);
            }
            Boolean firstLine = TRUE;
            List<AutoPtr<MemItem> >::Iterator it = oomMems->Begin();
            for (; it != oomMems->End(); ++it) {
                AutoPtr<MemItem> miCat = *it;
                if (miCat->mSubitems == NULL || miCat->mSubitems->IsEmpty()) {
                    continue;
                }
                if (miCat->mId < ProcessList::SERVICE_ADJ
                        || miCat->mId == ProcessList::HOME_APP_ADJ
                        || miCat->mId == ProcessList::PREVIOUS_APP_ADJ) {
                    if (outTag.GetLength() != 0 && miCat->mId <= ProcessList::FOREGROUND_APP_ADJ) {
                        outTag.Append(String(" / "));
                    }
                    if (outStack.GetLength() != 0) {
                        if (miCat->mId >= ProcessList::FOREGROUND_APP_ADJ) {
                            if (firstLine) {
                                outStack.Append(String(":"));
                                firstLine = FALSE;
                            }
                            outStack.Append(String("\n\t at "));
                        }
                        else {
                            outStack.Append(String("$"));
                        }
                    }

                    List<AutoPtr<MemItem> >::Iterator subIt = miCat->mSubitems->Begin();
                    for (Int32 j = 0; subIt != miCat->mSubitems->End(); ++subIt, ++j) {
                        AutoPtr<MemItem> mi = *subIt;;
                        if (j > 0) {
                            if (outTag.GetLength() != 0) {
                                outTag.Append(String(" "));
                            }
                            if (outStack.GetLength() != 0) {
                                outStack.Append(String("$"));
                            }
                        }
                        if (outTag.GetLength() != 0 && miCat->mId <= ProcessList::FOREGROUND_APP_ADJ) {
                            AppendMemBucket(outTag, mi->mPss, mi->mShortLabel, FALSE);
                        }
                        if (outStack.GetLength() != 0) {
                            AppendMemBucket(outStack, mi->mPss, mi->mShortLabel, TRUE);
                        }
                    }
                    if (outStack.GetLength() != 0 && miCat->mId >= ProcessList::FOREGROUND_APP_ADJ) {
                        outStack.Append(String("("));
                        for (Int32 k = 0; k < DUMP_MEM_OOM_ADJ->GetLength(); k++) {
                            if ((*DUMP_MEM_OOM_ADJ)[k] == miCat->mId) {
                                outStack.Append((*DUMP_MEM_OOM_LABEL)[k]);
                                outStack.Append(String(":"));
                                outStack.Append((*DUMP_MEM_OOM_ADJ)[k]);
                            }
                        }
                        outStack.Append(String(")"));
                    }
                }
            }
        }

        if (!brief && !oomOnly) {
            pw->Println();
            pw->PrintStringln(String("Total PSS by process:"));
            DumpMemItems(pw, String("  "), procMems, TRUE);
            pw->Println();
        }
        pw->PrintStringln(String("Total PSS by OOM adjustment:"));
        DumpMemItems(pw, String("  "), oomMems, FALSE);
        if (!oomOnly) {
            AutoPtr<IPrintWriter> out = categoryPw != NULL ? categoryPw : pw;
            out->Println();
            out->PrintStringln(String("Total PSS by category:"));
            DumpMemItems(out, String("  "), catMems, TRUE);
        }
        pw->Println();
        pw->PrintString(String("Total PSS: "));
        pw->PrintInt64(totalPss);
        pw->PrintStringln(String(" kB"));
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
        pw->PrintString(String("      KSM: "));
        pw->PrintInt64(sharing);
        pw->PrintString(String(" kB saved from shared "));
        pw->PrintInt64(shared);
        pw->PrintStringln(String(" kB"));
        pw->PrintString(String("           "));
        pw->PrintInt64(unshared);
        pw->PrintString(String(" kB unshared; "));
        pw->PrintInt64(voltile);
        pw->PrintStringln(String(" kB volatile"));
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
            //    synchronized(cpr) {
            AutoPtr<IThread> thread;
            Thread::Attach((IThread**)&thread);
            cpr->Lock();
            cpr->mLaunchingApp = NULL;
            cpr->NotifyAll();
            cpr->Unlock();
        }

        mProviderMap->RemoveProviderByClass(cpr->mName, UserHandleGetUserId(cpr->mUid));

        String cprAuth;
        cpr->mInfo->GetAuthority(&cprAuth);
        AutoPtr<ArrayOf<String> > names;
        StringUtils::Split(cprAuth, String(";"), (ArrayOf<String>**)&names);
        for (Int32 j = 0; j < names->GetLength(); j++) {
            mProviderMap->RemoveProviderByName((*names)[j], UserHandleGetUserId(cpr->mUid));
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
                    && capp->mPid != 0
                    && capp->mPid != MY_PID) {
                String cprName;
                cpr->mInfo->GetName(&cprName);
                Slogger::I(TAG, "Kill %s (pid %d): provider %s in dying process %s", capp->mProcessName.string(),
                       + "" + capp->mPid, cprName.string(), (proc != NULL ? proc->mProcessName.string() : String("??").string()));
//                EventLog.writeEvent(EventLogTags.AM_KILL, capp.userId, capp.pid,
//                        capp.processName, capp.setAdj, "dying provider "
//                                + cpr.name.toShortString());
                Process::KillProcess(capp->mPid);
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

ECode CActivityManagerService::CleanUpApplicationRecordLocked(
    /* [in] */ ProcessRecord* aApp,
    /* [in] */ Boolean restarting,
    /* [in] */ Boolean allowRestart,
    /* [in] */ Int32 index)
{
    VALIDATE_NOT_NULL(aApp);
    AutoPtr<ProcessRecord> app = aApp;

    if (index >= 0) {
        mLruProcesses.Remove(index);
    }

    mProcessesToGc.Remove(app);

    // Dismiss any open dialogs.
    if (app->mCrashDialog != NULL) {
        app->mCrashDialog->Dismiss();
        app->mCrashDialog = NULL;
    }
    if (app->mAnrDialog != NULL) {
        app->mAnrDialog->Dismiss();
        app->mAnrDialog = NULL;
    }
    if (app->mWaitDialog != NULL) {
        app->mWaitDialog->Dismiss();
        app->mWaitDialog = NULL;
    }

    app->mCrashing = FALSE;
    app->mNotResponding = FALSE;

    app->ResetPackageList();
    app->UnlinkDeathRecipient();
    app->mThread = NULL;
    app->mForcingToForeground = NULL;
    app->mForegroundServices = FALSE;
    app->mForegroundActivities = FALSE;
    app->mHasShownUi = FALSE;
    app->mHasAboveClient = FALSE;

    mServices->KillServicesLocked(app, allowRestart);

    Boolean restart = FALSE;

    // Remove published content providers.
    if (!app->mPubProviders.IsEmpty()) {
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
    }

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
                //synchronized(cpr)
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
    if (!app->mReceivers.IsEmpty()) {
        HashSet< AutoPtr<ReceiverList> >::Iterator it = app->mReceivers.Begin();
        for (; it != app->mReceivers.End(); ++it) {
            RemoveReceiverLocked(*it);
        }
        app->mReceivers.Clear();
    }

    // If the app is undergoing backup, tell the backup manager about it
    if (mBackupTarget != NULL && app->mPid == mBackupTarget->mApp->mPid) {
        if (DEBUG_BACKUP || DEBUG_CLEANUP) {
            String aiDes;
            mBackupTarget->mAppInfo->ToString(&aiDes);
            Slogger::D(TAG, "App %s died during backup", aiDes.string());
        }
        // try {
        AutoPtr<IServiceManager> sm;
        ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
        assert(sm != NULL);
        AutoPtr<IInterface> service;
        sm->GetService(IContext::BACKUP_SERVICE,
                (IInterface**)&service);
        AutoPtr<IIBackupManager> bm = IIBackupManager::Probe(service);
        String appCName;
        app->mInfo->GetPackageName(&appCName);
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
        return NOERROR;
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
        if (Find(mPersistentStartingProcesses.Begin(), mPersistentStartingProcesses.End(), app)
                == mPersistentStartingProcesses.End()) {
            mPersistentStartingProcesses.PushBack(app);
            restart = TRUE;
        }
    }
    if ((DEBUG_PROCESSES || DEBUG_CLEANUP)
            && (Find(mProcessesOnHold.Begin(), mProcessesOnHold.End(), app) != mProcessesOnHold.End())) {
        String appDes = app->ToString();
        Slogger::V(TAG, "Clean-up removing on hold: %s", appDes.string());
    }
    mProcessesOnHold.Remove(app);

    if (app == mHomeProcess) {
        mHomeProcess = NULL;
    }
    if (app == mPreviousProcess) {
        mPreviousProcess = NULL;
    }

    if (restart && !app->mIsolated) {
        // We have components that still need to be running in the
        // process, so re-launch it.
        mProcessNames->Put(app->mProcessName, app->mUid, app);
        StartProcessLocked(app, String("restart"), app->mProcessName);
    }
    else if (app->mPid > 0 && app->mPid != MY_PID) {
        // Goodbye!
        {
            AutoLock lock(mPidsSelfLockedLock);

            mPidsSelfLocked.Erase(app->mPid);
            mHandler->RemoveMessages(PROC_START_TIMEOUT_MSG, app->Probe(EIID_IInterface));
        }
        app->SetPid(0);
    }
    return NOERROR;
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
    /* [out] */ IObjectContainer** services)
{
    VALIDATE_NOT_NULL(services);
    *services = NULL;

    FAIL_RETURN(EnforceNotIsolatedCaller(String("getServices")));
    AutoLock lock(this);
    FAIL_RETURN(CParcelableObjectContainer::New(services));
    AutoPtr<List<AutoPtr<IActivityManagerRunningServiceInfo> > > infoList = mServices->GetRunningServiceInfoLocked(maxNum, flags);
    if (infoList) {
        List<AutoPtr<IActivityManagerRunningServiceInfo> >::Iterator it = infoList->Begin();
        for (; it != infoList->End(); ++it) {
            IInterface* obj = (IInterface*)((*it).Get());
            (*services)->Add(obj);
        }

        infoList = NULL;
    }
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
    CheckValidCaller(callingUid, userId);
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

    CheckValidCaller(Binder::GetCallingUid(), userId);

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
    VALIDATE_NOT_NULL(result);
    *result = IUserHandle::USER_NULL;
    Int32 callingUserId = UserHandleGetUserId(callingUid);
    if (callingUserId != userId) {
        if (callingUid != 0 && callingUid != IProcess::SYSTEM_UID) {
            String INTERACT_ACROSS_USERS = String("android.permission.INTERACT_ACROSS_USERS");//TODO Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS;
            String INTERACT_ACROSS_USERS_FULL = String("android.permission.INTERACT_ACROSS_USERS_FULL");//TODO Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL;
            if ((requireFull || CheckComponentPermission(INTERACT_ACROSS_USERS,
                    callingPid, callingUid, -1, TRUE) != IPackageManager::PERMISSION_GRANTED)
                && CheckComponentPermission(INTERACT_ACROSS_USERS_FULL,
                    callingPid, callingUid, -1, TRUE) != IPackageManager::PERMISSION_GRANTED)
            {
                if (userId == IUserHandle::USER_CURRENT_OR_SELF) {
                    // In this case, they would like to just execute as their
                    // owner user instead of failing.
                    userId = callingUserId;
                }
                else {
                    StringBuilder builder(128);
                    builder += ("Permission Denial: ");
                    builder += (name);
                    if (callerPackage.IsNull()) {
                        builder += (" from ");
                        builder += (callerPackage);
                    }
                    builder += (" asks to run as user ");
                    builder += (userId);
                    builder += (" but is calling from user ");
                    builder += (UserHandleGetUserId(callingUid));
                    builder += ("; this requires ");
                    builder += (INTERACT_ACROSS_USERS_FULL);
                    if (!requireFull) {
                        builder += (" or ");
                        builder += (INTERACT_ACROSS_USERS);
                    }
                    String msg = builder.ToString();
                    Slogger::W(TAG, msg.string());
                    return E_SECURITY_EXCEPTION;
                }
            }
        }
        if (userId == IUserHandle::USER_CURRENT || userId == IUserHandle::USER_CURRENT_OR_SELF) {
            // Note that we may be accessing this outside of a lock...
            // shouldn't be a big deal, if this is being called outside
            // of a locked context there is intrinsically a race with
            // the value the caller will receive and someone else changing it.
            userId = mCurrentUserId;
        }
        if (!allowAll && userId < 0) {
//            throw new IllegalArgumentException(
//                 "Call does not support special user #" + userId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    *result = userId;
    return NOERROR;
}

Boolean CActivityManagerService::IsSingleton(
    /* [in] */ const String& componentProcessName,
    /* [in] */ IApplicationInfo* aInfo,
    /* [in] */ const String& className,
    /* [in] */ Int32 flags)
{
    if (aInfo == NULL) {
        return FALSE;

    }
    Boolean result = FALSE;
    Int32 aUid, infoFlags;
    aInfo->GetUid(&aUid);
    aInfo->GetFlags(&infoFlags);
    String pkgName;
    aInfo->GetPackageName(&pkgName);

    if (UserHandleGetAppId(aUid) >= IProcess::FIRST_APPLICATION_UID) {
        if ((flags&IServiceInfo::FLAG_SINGLE_USER) != 0) {
            AutoPtr<IActivityManagerHelper> helper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
            Int32 permission;
            helper->CheckUidPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS, aUid, &permission);
            if (permission != IPackageManager::PERMISSION_GRANTED) {
                AutoPtr<IComponentName> comp;
                CComponentName::New(pkgName, className, (IComponentName**)&comp);
                String compString;
                comp->FlattenToShortString(&compString);
                StringBuilder msg("Permission Denial: Component ");
                msg += compString;
                msg += " requests FLAG_SINGLE_USER, but app does not hold ";
                msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS";
                Slogger::W(TAG, msg.ToString().string());
                // throw new SecurityException(msg);
                return FALSE;
            }
            result = TRUE;
        }
    }
    else if (componentProcessName == pkgName) {
        result = (infoFlags & IApplicationInfo::FLAG_PERSISTENT) != 0;
    }
    else if (componentProcessName.Equals("system")) {
        result = TRUE;
    }
    if (DEBUG_MU) {
        String infoDes;
        aInfo->ToString(&infoDes);
        Slogger::V(TAG, "isSingleton(%s, %s, %s, 0x%s) = %d",
                componentProcessName.string(), infoDes.string(), className.string(),
                StringUtils::Int32ToHexString(flags).string(), result);
    }
    return result;
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
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::BACKUP, String("bindBackupAgent")));

    {
        AutoLock lock(this);
        // !!! TODO: currently no check here that we're already bound
        AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> ss;
        AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
        synchronized (stats) {
            Int32 uid;
            String pkgName,name;
            app->GetUid(&uid);
            app->GetPackageName(&pkgName);
            app->GetName(&name);
            ss = stats->GetServiceStatsLocked(uid, pkgName, name);
        }

        // Backup agent is now in use, its package can't be stopped.
        // try {
            AppGlobals::GetPackageManager()->SetPackageStoppedState(
                pkgName, FALSE, UserHandleGetUserId(uid));
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
                FALSE, 0, String("backup"), hostingName, FALSE, FALSE);
        if (proc == NULL) {
            Slogger::E(TAG, "Unable to start backup agent process %p", r.Get());
            return FALSE;
        }

        r->mApp = proc;
        mBackupTarget = r;
        app->GetPackageName(&mBackupAppName);

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
    if (DEBUG_BACKUP) Slogger::V(TAG, "clearPendingBackup");
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::BACKUP, String("clearPendingBackup")));

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
    if (DEBUG_BACKUP) Slogger::V(TAG, "backupAgentCreated: %s = %p", agentPackageName.string(), agent);

    {
        AutoLock lock(this);
        if (!agentPackageName.Equals(mBackupAppName)) {
            Slogger::E(TAG, "Backup agent created for %s but not requested!", agentPackageName.string());
            return NOERROR;
        }
    }

    Int64 oldIdent = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IServiceManager> sm;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    AutoPtr<IInterface> service;
    sm->GetService(IContext::BACKUP_SERVICE, (IInterface**)&service);
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
        appInfo->GetPackageName(&appInfoPkgName);
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
    VALIDATE_NOT_NULL(filter);
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;

    String nullStr;
    FAIL_RETURN(EnforceNotIsolatedCaller(String("registerReceiver")));
    Int32 callingUid;
    Int32 callingPid;

    String callerPackage(aCallerPackage.string());
    AutoLock lock(this);

    AutoPtr<ProcessRecord> callerApp;
    if (caller != NULL) {
        callerApp = GetRecordForAppLocked(caller);
        if (callerApp == NULL) {
            // throw new SecurityException(
            //         "Unable to find app for caller " + caller
            //         + " (pid=" + Binder::GetCallingPid()
            //         + ") when registering receiver " + receiver);
            Slogger::E(TAG, "Unable to find app for caller %p (pid=%d) when registering receiver %p"
                    , caller, Binder::GetCallingPid(), receiver);
            return E_SECURITY_EXCEPTION;
        }

        Int32 uid;
        callerApp->mInfo->GetUid(&uid);
        if (uid != IProcess::SYSTEM_UID
                && callerApp->mPkgList.Find(callerPackage) == callerApp->mPkgList.End()) {
            // throw new SecurityException("Given caller package " + callerPackage
            //         + " is not running in process " + callerApp);
            Slogger::E(TAG, "Given caller package %s is not running in process %p", callerPackage.string(), callerApp.Get());
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
            TRUE, TRUE, String("registerReceiver"), callerPackage, &userId));

    AutoPtr<List<AutoPtr<IIntent> > > allSticky;

    // Look for any matching sticky broadcasts...
    Int32 count ;
    filter->CountActions(&count);
    if (count > 0){
        String action;
        for(Int32 i = 0; i < count; i++){
            filter->GetAction(i, &action);
            allSticky = GetStickiesLocked(action, filter, allSticky,
                    IUserHandle::USER_ALL);
            allSticky = GetStickiesLocked(action, filter, allSticky,
                    UserHandleGetUserId(callingUid));
        }
    }
    else {
        allSticky = GetStickiesLocked(String(NULL), filter, allSticky,
                IUserHandle::USER_ALL);
        allSticky = GetStickiesLocked(String(NULL), filter, allSticky,
                UserHandleGetUserId(callingUid));
    }

    // The first sticky in the list is returned directly back to
    // the client.
    AutoPtr<IIntent> sticky = allSticky != NULL ? (*allSticky)[0] : NULL;

    if (DEBUG_BROADCAST) Slogger::V(TAG, "Register receiver %p: %p", receiver, sticky.Get());

    if (receiver == NULL) {
        *intent = sticky;
        REFCOUNT_ADD(*intent);
        return NOERROR;
    }

    AutoPtr<ReceiverList> rl;
    HashMap<AutoPtr<IIntentReceiver>, AutoPtr<ReceiverList> >::Iterator rrIt = mRegisteredReceivers.Find(receiver);
    if (rrIt != mRegisteredReceivers.End()) {
        rl = rrIt->mSecond;
    }
    if (rl == NULL) {
        rl = new ReceiverList(this, callerApp, callingPid, callingUid,
                userId, receiver);
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
        mRegisteredReceivers[receiver] = rl;
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

    AutoPtr<BroadcastFilter> bf = new BroadcastFilter(filter, rl, callerPackage,
            permission, callingUid, userId);
    rl->PushBack(bf);
   // if (!bf->DebugCheck()) {
   //     Slogger::W(TAG, "==> For Dynamic broadast");
   // }

    mReceiverResolver->AddFilter(bf);
    // Enqueue broadcasts for all existing stickies that match
    // this filter.
    if (allSticky != NULL) {
        AutoPtr<IInterface> bfObj = bf->Probe(EIID_IInterface);
        AutoPtr<IList> receivers;
        CArrayList::New((IList**)&receivers);
        receivers->PushBack(bfObj);

        List<AutoPtr<IIntent> >::Iterator it = allSticky->Begin();
        for (; it != allSticky->End(); ++it) {
            AutoPtr<IIntent> intent = *it;
            AutoPtr<BroadcastQueue> queue = BroadcastQueueForIntent(intent);
            AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, NULL,
                   String(NULL), -1, -1, nullStr, receivers, NULL, 0,
                   String(NULL), NULL, FALSE, TRUE, TRUE, -1);
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
        HashMap<AutoPtr<IIntentReceiver>, AutoPtr<ReceiverList> >::Iterator ite =
                mRegisteredReceivers.Find(receiver);
        if (ite != mRegisteredReceivers.End()){
            rl = ite->mSecond;
        }

        if (rl != NULL){
            if (rl->mCurBroadcast != NULL){
                AutoPtr<BroadcastRecord> r = rl->mCurBroadcast;
                Boolean doNext = FinishReceiverLocked(IBinder::Probe(receiver), r->mResultCode, r->mResultData,
                        r->mResultExtras, r->mResultAbort, TRUE);
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
   mRegisteredReceivers.Erase(rl->mReceiver);
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
    /* [in] */ ArrayOf<Int32>* users)
{
    AutoPtr<IObjectContainer> receivers;
    // try {
        HashSet<AutoPtr<IComponentName> > singleUserReceivers;
        Boolean scannedFirstReceivers = FALSE;
        Boolean hasNext = FALSE;
        Int32 flags;
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        for (Int32 i = 0; i < users->GetLength(); i++) {
            Int32 user = (*users)[i];
            AutoPtr<IObjectContainer> newReceivers;
            pm->QueryIntentReceivers(intent, resolvedType, STOCK_PM_FLAGS, user, (IObjectContainer**)&newReceivers);
            if (user != 0 && newReceivers != NULL) {
                // If this is not the primary user, we need to check for
                // any receivers that should be filtered out.
                // List<AutoPtr<IResolveInfo> >::Iterator it = newReceivers->Begin();
                // while(it != newReceivers->End())
                AutoPtr<IObjectEnumerator> enumerator;
                newReceivers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                while (enumerator->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IResolveInfo> ri;
                    enumerator->Current((IInterface**)&ri);

                    AutoPtr<IActivityInfo> ai;
                    ri->GetActivityInfo((IActivityInfo**)&ai);
                    ai->GetFlags(&flags);
                    if ((flags & IActivityInfo::FLAG_PRIMARY_USER_ONLY) != 0) {
                        enumerator->MoveNext(&hasNext);
                        newReceivers->Remove((IInterface*)ri);
                        continue;
                    }
                }
            }

            if (newReceivers != NULL) {
                Int32 len;
                newReceivers->GetObjectCount(&len);
                if (len == 0) {
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

                    // List<AutoPtr<IResolveInfo> >::Iterator it = receivers->Begin();
                    // while(it != receivers->End())
                    AutoPtr<IObjectEnumerator> enumerator;
                    receivers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                    Boolean hasNext = FALSE;
                    Int32 flags;
                    String pkgName, name;
                    while (enumerator->MoveNext(&hasNext), hasNext) {
                        AutoPtr<IResolveInfo> ri;
                        enumerator->Current((IInterface**)&ri);
                        AutoPtr<IActivityInfo> ai;
                        ri->GetActivityInfo((IActivityInfo**)&ai);
                        ai->GetFlags(&flags);
                        if ((flags & IActivityInfo::FLAG_SINGLE_USER) != 0) {
                            AutoPtr<IComponentName> cn;
                            ai->GetPackageName(&pkgName);
                            ai->GetName(&name);
                            CComponentName::New(pkgName, name, (IComponentName**)&cn);
                            singleUserReceivers.Insert(cn);
                        }
                    }
                }

                // Add the new results to the existing results, tracking
                // and de-dupping single user receivers.
                // List<AutoPtr<IResolveInfo> >::Iterator it = newReceivers->Begin();
                // while(it != newReceivers->End())
                AutoPtr<IObjectEnumerator> enumerator;
                receivers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                Boolean hasNext = FALSE;
                Int32 flags;
                String pkgName, name;
                while (enumerator->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IResolveInfo> ri;
                    enumerator->Current((IInterface**)&ri);
                    AutoPtr<IActivityInfo> ai;
                    ri->GetActivityInfo((IActivityInfo**)&ai);
                    ai->GetFlags(&flags);
                    if ((flags & IActivityInfo::FLAG_SINGLE_USER) != 0) {
                        AutoPtr<IComponentName> cn;
                        ai->GetPackageName(&pkgName);
                        ai->GetName(&name);
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
    if (receivers != NULL) {
        AutoPtr<IList> list;
        CArrayList::New((IList**)&list);
        AutoPtr<IObjectEnumerator> enumerator;
        receivers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> ri;
            enumerator->Current((IInterface**)&ri);
            list->PushBack(ri);
        }
        return list;
    }
    else {
        return NULL;
    }
}

ECode CActivityManagerService::BroadcastIntentLocked(
    /* [in] */ ProcessRecord* callerApp,
    /* [in] */ const String& callerPackage,
    /* [in] */ IIntent* _intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* resultTo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* map,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    AutoPtr<IIntent> intent;
    CIntent::New(_intent, (IIntent **)&intent);

    // By default broadcasts do not go to stopped apps.
    intent->AddFlags(IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES);

    if (DEBUG_BROADCAST_LIGHT) {
        String intDes;
        intent->ToString(&intDes);
        Slogger::D(TAG, "Send %s ordered=%d, userId=%d, %s",
            (sticky ? "broadcast sticky: " : "broadcast: "), ordered, userId, intDes.string());
    }
    if ((resultTo != NULL) && !ordered) {
        String str;
        intent->GetAction(&str);
        Slogger::W(TAG, "Broadcast %s not ordered but result callback requested!", str.string());
    }

   FAIL_RETURN(HandleIncomingUser(callingPid, callingUid, userId,
           TRUE, FALSE, String("broadcast"), callerPackage, &userId));

    // Make sure that the user who is receiving this broadcast is started.
    // If not, we will just skip it.
    if (userId != IUserHandle::USER_ALL) {
        HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(userId);
        if(it == mStartedUsers.End()) {
            Int32 flags;
            intent->GetFlags(&flags);
            if (callingUid != IProcess::SYSTEM_UID || (flags
                    & IIntent::FLAG_RECEIVER_BOOT_UPGRADE) == 0) {
                // Slogger::W(TAG, StringBuilder("Skipping broadcast of ") + intent
                // + ": user " + userId + " is stopped");
                *result = IActivityManager::BROADCAST_SUCCESS;
                return NOERROR;
            }
        }
    }

    /*
    * Prevent non-system code (defined here to be non-persistent
    * processes) from sending protected broadcasts.
    */
    Int32 callingAppId = UserHandleGetAppId(callingUid);
    if (callingAppId == IProcess::SYSTEM_UID || callingAppId == IProcess::PHONE_UID
            || callingAppId == IProcess::SHELL_UID || callingAppId == IProcess::BLUETOOTH_UID ||
            callingUid == 0) {
        // Always okay.
    }
    else if (callerApp == NULL || !callerApp->mPersistent) {
        //try {
        String action;
        intent->GetAction(&action);
        Boolean b;
        AppGlobals::GetPackageManager()->IsProtectedBroadcast(action, &b);
        if (b) {
            Slogger::W(TAG, "Permission Denial: not allowed to send broadcast %s from pid=%d, uid=%d"
                    , action.string(), callingPid, callingUid);
            //throw new SecurityException(msg);
            return E_SECURITY_EXCEPTION;
        }
        //} catch (RemoteException e) {
        //    Slogger::W(TAG, "Remote exception", e);
        //    return ActivityManager.BROADCAST_SUCCESS;
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
            || uidRemoved) {
        if (CheckComponentPermission(
                    Elastos::Droid::Manifest::permission::BROADCAST_PACKAGE_REMOVED,
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
                   AutoLock lock(bs);
                   bs->RemoveUidStatsLocked(uid);
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
                            ForceStopPackageLocked((*list)[i], -1, FALSE, TRUE, TRUE, FALSE, userId);
                        }
                        SendPackageBroadcastLocked(
                                IApplicationThread::EXTERNAL_STORAGE_UNAVAILABLE, list, userId);
                    }
                }
                else {
                    AutoPtr<IUri> data;
                    intent->GetData((IUri**)&data);
                    String ssp;
                    if (data != NULL && (data->GetSchemeSpecificPart(&ssp), !ssp.IsNull())) {
                        Boolean dontKillApp = FALSE;
                        intent->GetBooleanExtra(IIntent::EXTRA_DONT_KILL_APP, FALSE, &dontKillApp);
                        if (!dontKillApp) {
                            Int32 uid = -1;
                            intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &uid);
                            ForceStopPackageLocked(ssp, uid, FALSE, TRUE, TRUE, FALSE, userId);
                        }
                        if (action.Equals(IIntent::ACTION_PACKAGE_REMOVED)) {
                            AutoPtr< ArrayOf<String> > sa = ArrayOf<String>::Alloc(1);
                            (*sa)[0] = ssp;
                            SendPackageBroadcastLocked(IApplicationThread::PACKAGE_REMOVED,
                                                       sa, userId);
                        }
                    }
                }
            }
        }
        else {
            String action;
            intent->GetAction(&action);
            Slogger::W(TAG, "Permission Denial: %s broadcast from %s (pid=%d, uid=%d) requires Elastos::Droid::Manifest::permission::BROADCAST_PACKAGE_REMOVED"
                    , action.string(), callerPackage.string(), callingPid, callingUid);
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

    if (action.Equals(IIntent::ACTION_CLEAR_DNS_CACHE)) {
        mHandler->SendEmptyMessage(CLEAR_DNS_CACHE_MSG, &bval);
    }

    if (action.Equals(Elastos::Droid::Net::IProxy::PROXY_CHANGE_ACTION)) {
        AutoPtr<IProxyInfo> proxy;
        intent->GetParcelableExtra(String("proxy"),(IParcelable**)&proxy);

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(UPDATE_HTTP_PROXY_MSG, proxy->Probe(EIID_IInterface), (IMessage**)&msg);
        mHandler->SendMessage(msg, &bval);
    }

    // Add to the sticky list if requested.
    if (sticky) {
        Int32 permission;
        CheckPermission(Elastos::Droid::Manifest::permission::BROADCAST_STICKY,
                        callingPid, callingUid, &permission);
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            Slogger::W(TAG, "Permission Denial: broadcastIntent() requesting a sticky broadcast from pid=%d, uid=%d requires Elastos::Droid::Manifest::permission::BROADCAST_STICKY"
                    , callingPid, callingUid);
            return E_SECURITY_EXCEPTION;
        }
        if (!requiredPermission.IsNull()) {
            Slogger::W(TAG, "Can't broadcast sticky intent %p and enforce permission %s"
                    , intent.Get(), requiredPermission.string());
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
                            // throw new IllegalArgumentException(
                            //         "Sticky broadcast " + intent + " for user "
                            //         + userId + " conflicts with existing global broadcast");
                            Slogger::E(TAG, "Sticky broadcast %p for user %d conflicts with existing global broadcast"
                                    , intent.Get(), userId);
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
        receivers = CollectReceiverComponents(intent, resolvedType, users);
    }

    AutoPtr<IComponentName> name;
    intent->GetComponent((IComponentName**)&name);
    if (name == NULL) {
        registeredReceivers = mReceiverResolver->QueryIntent(
                intent, resolvedType, FALSE, userId);
    }

    Boolean replacePending =
        (flags & IIntent::FLAG_RECEIVER_REPLACE_PENDING) !=  0;

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
            rlist->PushBack((*it)->Probe(EIID_IInterface));
        }
        // If we are not serializing this broadcast, then send the
        // registered receivers separately so they don't wait for the
        // components to be launched.
        AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, callerApp,
                callerPackage, callingPid, callingUid, requiredPermission,
                rlist, resultTo, resultCode, resultData, map,
                ordered, sticky, FALSE, userId);
        if (DEBUG_BROADCAST) Slogger::V(TAG, "Enqueueing parallel broadcast %p", r.Get());
        Boolean replaced = replacePending && queue->ReplaceParallelBroadcastLocked(r);
        if (!replaced) {
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
            intent->GetStringArrayExtra(String(IIntent::EXTRA_CHANGED_PACKAGE_LIST),
                                        (ArrayOf<String> **)&skipPackages);
        }
        if (skipPackages != NULL && (skipPackages->GetLength() > 0)) {
            for (Int32 i = 0; i < skipPackages->GetLength(); ++i) {
                String skipPackage = (*skipPackages)[i];
                if (!skipPackage.IsNull()) {
                    List< AutoPtr<IInterface> >::Iterator iter = receivers->Begin();
                    while(iter != receivers->End()) {
                        AutoPtr<IResolveInfo> curt = IResolveInfo::Probe(*iter);
                        AutoPtr<IActivityInfo> aInfo;
                        curt->GetActivityInfo((IActivityInfo **)&aInfo);
                        String cname;
                        aInfo->GetPackageName(&cname);
                        if (cname.Equals(skipPackage)) {
                            iter = receivers->Erase(iter);
                            continue;
                        }
                        ++iter;
                    }
                }
            }
        }

        AutoPtr<IResolveInfo> curt;
        AutoPtr<BroadcastFilter> curr;
        List< AutoPtr<IInterface> >::Iterator iter = receivers->Begin();
        while (iter != receivers->End() && registeredReceivers != NULL && ir != registeredReceivers->End()) {
            if (curt == NULL) {
                curt = IResolveInfo::Probe(*iter);
            }
            if (curr == NULL) {
                curr = *ir;
            }
            Int32 curtPriority, currPriority;
            curt->GetPriority(&curtPriority);
            curr->GetPriority(&currPriority);
            if (currPriority >= curtPriority) {
                // Insert this broadcast record into the final list.
                receivers->Insert(iter, curr->Probe(EIID_IInterface));
                ++ir;
                curr = NULL;
            }
            else {
                // Skip to the next ResolveInfo in the final list.
                ++iter;
                curt = NULL;
            }
        }
    }

    if (registeredReceivers) {
        while(ir != registeredReceivers->End()) {
            if (receivers == NULL) {
                CArrayList::New((IList**)&receivers);
            }
            receivers->PushBack((IInterface*)*ir);
            ir++;
        }
    }

    if ((receivers != NULL && !receivers->IsEmpty()) || resultTo != NULL)  {
        AutoPtr<BroadcastQueue> queue = BroadcastQueueForIntent(intent);
        AutoPtr<BroadcastRecord> r = new BroadcastRecord(queue, intent, callerApp,
                callerPackage, callingPid, callingUid, requiredPermission,
                receivers, resultTo, resultCode, resultData, map, ordered,
                sticky, FALSE, userId);
        if (DEBUG_BROADCAST) {
            Slogger::V(TAG, "Enqueueing ordered broadcast %p: prev had %d", r.Get(), queue->mOrderedBroadcasts.GetSize());
        }
        if (DEBUG_BROADCAST) {
            Int32 seq = -1;
            r->mIntent->GetInt32Extra(String("seq"), -1, &seq);
            String ia;
            r->mIntent->GetAction(&ia);
            Slogger::I(TAG, "Enqueueing broadcast %s seq=%d", action.string(), seq);
        }
        Boolean replaced = replacePending && queue->ReplaceOrderedBroadcastLocked(r);
        if (!replaced) {
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
    /* [in] */ Boolean serialized,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
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
        callerApp->mInfo->GetPackageName(&callerPackage);
    }
    ECode ec = BroadcastIntentLocked(callerApp,
            callerPackage, newIntent, resolvedType, resultTo,
            resultCode, resultData, map, requiredPermission, serialized,
            sticky, callingPid, callingUid, userId, result);
    Binder::RestoreCallingIdentity(origId);
    return ec;
}

ECode CActivityManagerService::BroadcastIntentInPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ IIntent* _intent,
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
    AutoLock lock(this);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(VerifyBroadcastLocked(_intent, (IIntent**)&intent));

    Int64 origId = Binder::ClearCallingIdentity();
    BroadcastIntentLocked(NULL, packageName, intent, resolvedType,
            resultTo, resultCode, resultData, map, requiredPermission,
            serialized, sticky, -1, uid, userId, status);
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
            Binder::GetCallingUid(), _userId, TRUE, FALSE,
            String("removeStickyBroadcast"), String(NULL), &userId);

    {
        AutoLock lock(this);
        if (CheckCallingPermission(Elastos::Droid::Manifest::permission::BROADCAST_STICKY)
                != IPackageManager::PERMISSION_GRANTED) {
            // String msg = "Permission Denial: unbroadcastIntent() from pid="
            //         + Binder::GetCallingPid()
            //         + ", uid=" + Binder::GetCallingUid()
            //         + " requires " + Elastos::Droid::Manifest::permission::BROADCAST_STICKY;
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
    /* [in] */ Boolean resultAbort,
    /* [in] */ Boolean _explicit)
{
    AutoPtr<BroadcastRecord> r = BroadcastRecordForReceiverLocked(receiver);
    if (r == NULL) {
        Slogger::W(TAG, "finishReceiver called but not found on queue");
        return FALSE;
    }

    return r->mQueue->FinishReceiverLocked(r, resultCode, resultData, resultExtras, resultAbort,
            _explicit);
}

ECode CActivityManagerService::FinishReceiver(
    /* [in] */ IBinder* who,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean resultAbort)
{
    if (DEBUG_BROADCAST) {
        String whoDes;
        who->ToString(&whoDes);
        Slogger::V(TAG, "Finish receiver: %s", whoDes.string());
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
        r = BroadcastRecordForReceiverLocked(who);
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
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    FAIL_RETURN(EnforceNotIsolatedCaller(String("startInstrumentation")));
    HandleIncomingUser(Binder::GetCallingPid(), Binder::GetCallingUid(),
            userId, FALSE, TRUE, String("startInstrumentation"), String(NULL), &userId);
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
        ii->GetPackageName(&iiPkgName);
        Int32 match;
        pkgManager->CheckSignatures(
                tPackage, iiPkgName, &match);
        if (match < 0 && match != IPackageManager::SIGNATURE_FIRST_NOT_SIGNED) {
            StringBuilder msg("Permission Denial: starting instrumentation ");
            msg += className;
            msg += " from pid=";
            msg += Binder::GetCallingPid();
            msg += ", uid=";
            msg += Binder::GetCallingPid();
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
        ForceStopPackageLocked(tPackage, -1, TRUE, FALSE, TRUE, TRUE, userId);
        AutoPtr<ProcessRecord> app = AddAppLocked(ai, FALSE);
        app->mInstrumentationClass = className;
        app->mInstrumentationInfo = ai;
        app->mInstrumentationProfileFile = profileFile;
        app->mInstrumentationArguments = arguments;
        app->mInstrumentationWatcher = watcher;
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
    app->mInstrumentationWatcher = NULL;
    app->mInstrumentationClass = NULL;
    app->mInstrumentationInfo = NULL;
    app->mInstrumentationProfileFile = NULL;
    app->mInstrumentationArguments = NULL;

    String pName;
    app->mInfo->GetProcessName(&pName);
    ForceStopPackageLocked(pName, -1, FALSE, FALSE, TRUE, TRUE, app->mUserId);
    return NOERROR;
}

ECode CActivityManagerService::FinishInstrumentation(
    /* [in] */ IApplicationThread* target,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    Int32 userId = UserHandleGetCallingUserId();
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

ECode CActivityManagerService::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config);
    AutoLock lock(this);
    return CConfiguration::New(mConfiguration, config);
}

ECode CActivityManagerService::UpdatePersistentConfiguration(
    /* [in] */ IConfiguration* values)
{
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::CHANGE_CONFIGURATION,
            String("updateConfiguration()")));
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::WRITE_SETTINGS,
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
    FAIL_RETURN(EnforceCallingPermission(Elastos::Droid::Manifest::permission::CHANGE_CONFIGURATION,
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
            AutoPtr<ISettingsSystem> settingsSystem;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
            settingsSystem->ClearConfiguration(values);
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
    // do nothing if we are headless
    if (mHeadless) return TRUE;

    Int32 changes = 0;
    Boolean kept = TRUE;

    if (values != NULL) {
        AutoPtr<IConfiguration> newConfig;
        CConfiguration::New(mConfiguration, (IConfiguration**)&newConfig);
        newConfig->UpdateFrom(values, &changes);
        if (changes != 0) {
            if (DEBUG_SWITCH || DEBUG_CONFIGURATION) {
                String confDes;
                values->ToString(&confDes);
                Slogger::I(TAG, "Updating configuration to: %s", confDes.string());
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

            mConfigurationSeq++;
            if (mConfigurationSeq <= 0) {
                mConfigurationSeq = 1;
            }
            newConfig->SetSeq(mConfigurationSeq);
            mConfiguration = newConfig;
            String confDes;
            newConfig->ToString(&confDes);
            Slogger::I(TAG, "Config changed: %s", confDes.string());

            AutoPtr<IConfiguration> configCopy;
            CConfiguration::New(mConfiguration, (IConfiguration**)&configCopy);

            // TODO: If our config changes, should we auto dismiss any currently
            // showing dialogs?
            mShowDialogs = ShouldShowDialogs(newConfig);

            AttributeCache* ac = AttributeCache::GetInstance();
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

            AutoPtr<ISettingsSystem> ss;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
            Boolean hasChanges;
            ss->HasInterestingConfigurationChanges(changes, &hasChanges);
            if (persistent && hasChanges) {
                AutoPtr<IConfiguration> config;
                CConfiguration::New(configCopy, (IConfiguration**)&config);

                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(UPDATE_CONFIGURATION_MSG,
                    config->Probe(EIID_IInterface), (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessage(msg, &result);
            }

            List<AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
            for (; rit != mLruProcesses.REnd(); ++rit) {
                AutoPtr<ProcessRecord> app = *rit;
                if (app->mThread != NULL) {
                    if (DEBUG_CONFIGURATION) {
                        String confDes;
                        mConfiguration->ToString(&confDes);
                        Slogger::V(TAG, "Sending to proc %s new config %s",
                                app->mProcessName.string(), confDes.string());
                    }
                    app->mThread->ScheduleConfigurationChanged(configCopy);
                }
            }
            AutoPtr<IIntent> intent;
            CIntent::New(String(IIntent::ACTION_CONFIGURATION_CHANGED), (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                    | IIntent::FLAG_RECEIVER_REPLACE_PENDING
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
            Int32 result;
            String nullStr;
            BroadcastIntentLocked(NULL, nullStr, intent, nullStr, NULL, 0,
                    nullStr, NULL, nullStr, FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
            if ((changes & IActivityInfo::CONFIG_LOCALE) != 0) {
                AutoPtr<IIntent> newIntent;
                CIntent::New(String(IIntent::ACTION_LOCALE_CHANGED), (IIntent**)&newIntent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                BroadcastIntentLocked(NULL, nullStr,
                        newIntent, nullStr, NULL, 0, nullStr, NULL,
                        nullStr, FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
            }
        }
    }

    if (changes != 0 && starting == NULL) {
        // If the configuration changed, and the caller is not already
        // in the process of starting an activity, then find the top
        // activity to check if its configuration needs to change.
        starting = mMainStack->GetTopRunningActivityLocked(NULL);
    }

    if (starting != NULL) {
        kept = mMainStack->EnsureActivityConfigurationLocked(starting, changes);
        // And we need to make sure at this point that all other activities
        // are made visible with the correct configuration.
        mStackSupervisor->EnsureActivitiesVisibleLocked(starting, changes);
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
    }
    return NOERROR;
}

ECode CActivityManagerService::TargetTaskAffinityMatchesActivity(
    /* [in] */ IBinder* token,
    /* [in] */ const String& destAffinity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(token);
    *result = srec != NULL && srec->mTask->mAffinity != NULL &&
            srec->mTask->mAffinity.Equals(destAffinity);
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

    AutoPtr<IComponentName> dest;
    destIntent->GetComponent((IComponentName**)&dest);

    {
        AutoLock lock(this);
        AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(token);
        if (srec == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        List<AutoPtr<ActivityRecord> > history = srec->mStack->mHistory;
        Int32 start = -1;
        List<AutoPtr<ActivityRecord> >::Iterator it;
        Int32 i;
        for (it = history.Begin(), i = 0; it != history.End(); ++it, ++i) {
            if (srec == *it) {
                start = i;
                break;
            }
        }
        if (start < 0) {
            // Current activity is not in history stack; do nothing.
            *result = FALSE;
            return NOERROR;
        }
        Int32 finishTo = start - 1;
        AutoPtr<ActivityRecord> parent;
        Boolean foundParentInTask = FALSE;
        if (dest != NULL) {
            AutoPtr<TaskRecord> tr = srec->mTask;
            for (Int32 i = start - 1; i >= 0; i--) {
                AutoPtr<ActivityRecord> r = history[i];
                String rPkgName, destPkgName;
                r->mInfo->GetPackageName(&rPkgName);
                dest->GetPackageName(&destPkgName);
                String rName, destClassName;
                r->mInfo->GetName(&rName);
                dest->GetClassName(&destClassName);
                if (tr != r->mTask) {
                    // Couldn't find parent in the same task; stop at the one above this.
                    // (Root of current task; in-app "home" behavior)
                    // Always at least finish the current activity.
                    finishTo = Elastos::Core::Math::Min(start - 1, i + 1);
                    parent = history[finishTo];
                    break;
                }
                else if (rPkgName.Equals(destPkgName) &&
                        rName.Equals(destClassName)) {
                    finishTo = i;
                    parent = r;
                    foundParentInTask = TRUE;
                    break;
                }
            }
        }

        if (mController != NULL) {
            AutoPtr<ActivityRecord> next = mMainStack->GetTopRunningActivityLocked(token, 0);
            if (next != NULL) {
                // ask watcher if this is allowed
                Boolean resumeOK = TRUE;
                // try {
                FAIL_RETURN(mController->ActivityResuming(next->mPackageName, &resumeOK));
                // } catch (RemoteException e) {
                //     mController = null;
                // }

                if (!resumeOK) {
                    *result = FALSE;
                    return NOERROR;
                }
            }
        }
        const Int64 origId = Binder::ClearCallingIdentity();
        for (Int32 i = start; i > finishTo; i--) {
            AutoPtr<ActivityRecord> r = history[i];
            mMainStack->RequestFinishActivityLocked(IBinder::Probe(r->mAppToken), resultCode, resultData,
                    String("navigate-up"), TRUE);
            // Only return the supplied result for the first activity finished
            resultCode = IActivity::RESULT_CANCELED;
            resultData = NULL;
        }

        if (parent != NULL && foundParentInTask) {
            Int32 parentLaunchMode;
            parent->mInfo->GetLaunchMode(&parentLaunchMode);
            Int32 destIntentFlags;
            destIntent->GetFlags(&destIntentFlags);
            if (parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_INSTANCE ||
                    parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_TASK ||
                    parentLaunchMode == IActivityInfo::LAUNCH_SINGLE_TOP ||
                    (destIntentFlags & IIntent::FLAG_ACTIVITY_CLEAR_TOP) != 0) {
                AutoPtr<IApplicationInfo> appInfo;
                srec->mInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);
                parent->DeliverNewIntentLocked(uid, destIntent);
            }
            else {
                // try {
                AutoPtr<IComponentName> comp;
                destIntent->GetComponent((IComponentName**)&comp);
                AutoPtr<IActivityInfo> aInfo;
                AppGlobals::GetPackageManager()->GetActivityInfo(
                            comp, 0, srec->mUserId, (IActivityInfo**)&aInfo);
                Int32 res;
                mMainStack->StartActivityLocked(srec->mApp->mThread, destIntent,
                            String(NULL), aInfo, IBinder::Probe(parent->mAppToken), String(NULL),
                            0, -1, parent->mLaunchedFromUid, 0, NULL, TRUE, NULL, &res);
                    foundParentInTask = res == IActivityManager::START_SUCCESS;
                // } catch (RemoteException e) {
                //     foundParentInTask = FALSE;
                // }
                mMainStack->RequestFinishActivityLocked(IBinder::Probe(parent->mAppToken), resultCode,
                        resultData, String("navigate-up"), TRUE);
            }
        }
        Binder::RestoreCallingIdentity(origId);
        *result = foundParentInTask;
        return NOERROR;
    }
}

ECode CActivityManagerService::GetLaunchedFromUid(
    /* [in] */ IBinder* activityToken,
    /* [out] */ Int32* result)
{
    AutoPtr<ActivityRecord> srec = ActivityRecord::ForToken(activityToken);
    if (srec == NULL) {
        *result = -1;
    }
    else{
        *result = srec->mLaunchedFromUid;
    }
    return NOERROR;
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
    /* [in] */ Int32 hiddenAdj,
    /* [in] */ Int32 clientHiddenAdj,
    /* [in] */ Int32 emptyAdj,
    /* [in] */ ProcessRecord* TOP_APP,
    /* [in] */ Boolean recursed,
    /* [in] */ Boolean doingAll)
{
// Slogger::D(TAG, "================== CActivityManagerService::ComputeOomAdjLocked[app: %p] ================", app);
    assert(app != NULL && "CActivityManagerService::ComputeOomAdjLocked: app can not be NULL.");

    if (mAdjSeq == app->mAdjSeq) {
        // This adjustment has already been computed.  If we are calling
        // from the top, we may have already computed our adjustment with
        // an earlier hidden adjustment that isn't really for us... if
        // so, use the new hidden adjustment.
        if (!recursed && app->mHidden) {
            if (app->mHasActivities) {
                app->mCurAdj = app->mCurRawAdj = app->mNonStoppingAdj = hiddenAdj;
            }
            else if (app->mHasClientActivities) {
                app->mCurAdj = app->mCurRawAdj = app->mNonStoppingAdj = clientHiddenAdj;
            }
            else {
                app->mCurAdj = app->mCurRawAdj = app->mNonStoppingAdj = emptyAdj;
            }
        }
        return app->mCurRawAdj;
    }

    if (app->mThread == NULL) {
        app->mAdjSeq = mAdjSeq;
        app->mCurSchedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mCurAdj = app->mCurRawAdj = ProcessList::HIDDEN_APP_MAX_ADJ;
        return app->mCurAdj;
    }

    app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo::REASON_UNKNOWN;
    app->mAdjSource = NULL;
    app->mAdjTarget = NULL;
    app->mEmpty = FALSE;
    app->mHidden = FALSE;
    app->mHasClientActivities = FALSE;

    Int32 activitiesSize = app->mActivities.GetSize();

    if (app->mMaxAdj <= ProcessList::FOREGROUND_APP_ADJ) {
        // The max adjustment doesn't allow this app to be anything
        // below foreground, so it is not worth doing work for it.
        app->mAdjType = String("fixed");
        app->mAdjSeq = mAdjSeq;
        app->mCurRawAdj = app->mNonStoppingAdj = app->mMaxAdj;
        app->mHasActivities = FALSE;
        app->mForegroundActivities = FALSE;
        app->mKeeping = TRUE;
        app->mCurSchedGroup = IProcess::THREAD_GROUP_DEFAULT;

        // System process can do UI, and when they do we want to have
        // them trim their memory after the user leaves the UI.  To
        // facilitate this, here we need to determine whether or not it
        // is currently showing UI.
        app->mSystemNoUi = TRUE;
        if (app == TOP_APP) {
            app->mSystemNoUi = FALSE;
            app->mHasActivities = TRUE;
        }
        else if (activitiesSize > 0) {
            List<AutoPtr<ActivityRecord> >::Iterator it = app->mActivities.Begin();
            for (; it != app->mActivities.End(); ++it) {
                AutoPtr<ActivityRecord> r = *it;
                if (r->mVisible) {
                    app->mSystemNoUi = FALSE;
                }
                if (r->mApp.Get() == app) {
                    app->mHasActivities = TRUE;
                }
            }
        }
        app->mCurAdj = app->mMaxAdj;
        return app->mCurAdj;
    }

    app->mKeeping = FALSE;
    app->mSystemNoUi = FALSE;
    app->mHasActivities = FALSE;

    // Determine the importance of the process, starting with most
    // important to least, and assign an appropriate OOM adjustment.
    Int32 adj;
    Int32 schedGroup;
    Boolean foregroundActivities = FALSE;
    Boolean interesting = FALSE;
    AutoPtr<BroadcastQueue> queue;

    if (app == TOP_APP) {
        // The last app on the list is the foreground app.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mAdjType = String("top-activity");
        foregroundActivities = TRUE;
        interesting = TRUE;
        app->mHasActivities = TRUE;
    }
    else if (app->mInstrumentationClass != NULL) {
        // Don't want to kill running instrumentation.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mAdjType = String("instrumentation");
        interesting = TRUE;
    }
    else if ((queue = IsReceivingBroadcast(app)) != NULL) {
        // An app that is currently receiving a broadcast also
        // counts as being in the foreground for OOM killer purposes.
        // It's placed in a sched group based on the nature of the
        // broadcast as reflected by which queue it's active in.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = (queue == mFgBroadcastQueue)
                ? IProcess::THREAD_GROUP_DEFAULT : IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mAdjType = String("broadcast");
    }
    else if (app->mExecutingServices.IsEmpty() == FALSE) {
        // An app that is currently executing a service callback also
        // counts as being in the foreground.
        adj = ProcessList::FOREGROUND_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        app->mAdjType = String("exec-service");
    }
    else {
        // Assume process is hidden (has activities); we will correct
        // later if this is not the case.
        adj = hiddenAdj;
        schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mHidden = TRUE;
        app->mAdjType = String("bg-act");
    }

    Boolean hasStoppingActivities = FALSE;

    // Examine all activities if not already foreground.
    if (!foregroundActivities && activitiesSize > 0) {
        List<AutoPtr<ActivityRecord> >::Iterator it = app->mActivities.Begin();
        for (; it != app->mActivities.End(); ++it) {
            AutoPtr<ActivityRecord> r = *it;
            if (r->mVisible) {
                // App has a visible activity; only upgrade adjustment.
                if (adj > ProcessList::VISIBLE_APP_ADJ) {
                    adj = ProcessList::VISIBLE_APP_ADJ;
                    app->mAdjType = String("visible");
                }
                schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                app->mHidden = FALSE;
                app->mHasActivities = TRUE;
                foregroundActivities = TRUE;
                break;
            }
            else if (r->mState == ActivityState_PAUSING
                    || r->mState == ActivityState_PAUSED) {
                if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                    adj = ProcessList::PERCEPTIBLE_APP_ADJ;
                    app->mAdjType = String("pausing");
                }
                app->mHidden = FALSE;
                foregroundActivities = TRUE;
            }
            else if (r->mState == ActivityState_STOPPING) {
                // We will apply the actual adjustment later, because
                // we want to allow this process to immediately go through
                // any memory trimming that is in effect.
                app->mHidden = FALSE;
                foregroundActivities = TRUE;
                hasStoppingActivities = TRUE;
            }
            if (r->mApp.Get() == app) {
                app->mHasActivities = TRUE;
            }
        }
    }

    if (adj == hiddenAdj && !app->mHasActivities) {
        if (app->mHasClientActivities) {
            adj = clientHiddenAdj;
            app->mAdjType = String("bg-client-act");
        }
        else {
            // Whoops, this process is completely empty as far as we know
            // at this point.
            adj = emptyAdj;
            app->mEmpty = TRUE;
            app->mAdjType = String("bg-empty");
        }
    }

    if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
        if (app->mForegroundServices) {
            // The user is aware of this app, so make it visible.
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
            app->mHidden = FALSE;
            app->mAdjType = String("fg-service");
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
        else if (app->mForcingToForeground != NULL) {
            // The user is aware of this app, so make it visible.
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
            app->mHidden = FALSE;
            app->mAdjType = String("force-fg");
            app->mAdjSource = app->mForcingToForeground;
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
    }

    if (app->mForegroundServices) {
        interesting = TRUE;
    }

    if (adj > ProcessList::HEAVY_WEIGHT_APP_ADJ && app == mHeavyWeightProcess) {
        // We don't want to kill the current heavy-weight process.
        adj = ProcessList::HEAVY_WEIGHT_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mHidden = FALSE;
        app->mAdjType = String("heavy");
    }

    if (adj > ProcessList::HOME_APP_ADJ && app == mHomeProcess) {
        // This process is hosting what we currently consider to be the
        // home app, so we don't want to let it go into the background.
        adj = ProcessList::HOME_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mHidden = FALSE;
        app->mAdjType = String("home");
    }

    if (adj > ProcessList::PREVIOUS_APP_ADJ && app == mPreviousProcess
            && app->mActivities.IsEmpty() == FALSE) {
        // This was the previous process that showed UI to the user.
        // We want to try to keep it around more aggressively, to give
        // a good experience around switching between two apps.
        adj = ProcessList::PREVIOUS_APP_ADJ;
        schedGroup = IProcess::THREAD_GROUP_BG_NONINTERACTIVE;
        app->mHidden = FALSE;
        app->mAdjType = String("previous");
    }

    if (FALSE) Slogger::I(TAG, "OOM %s: initial adj=%d reason=%s",
            app->ToString().string(), adj, app->mAdjType.string());

    // By default, we use the computed adjustment.  It may be changed if
    // there are applications dependent on our services or providers, but
    // this gives us a baseline and makes sure we don't get into an
    // infinite recursion.
    app->mAdjSeq = mAdjSeq;
    app->mCurRawAdj = app->mNonStoppingAdj = adj;

    if (mBackupTarget != NULL && app == mBackupTarget->mApp) {
        // If possible we want to avoid killing apps while they're being backed up
        if (adj > ProcessList::BACKUP_APP_ADJ) {
            if (DEBUG_BACKUP) {
                String appDes = app->ToString();
                Slogger::V(TAG, "oom BACKUP_APP_ADJ for %s", appDes.string());
            }
            adj = ProcessList::BACKUP_APP_ADJ;
            app->mAdjType = String("backup");
            app->mHidden = FALSE;
        }
    }

    if (app->mServices.IsEmpty() == FALSE && (adj > ProcessList::FOREGROUND_APP_ADJ
            || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE)) {
        Millisecond64 now = SystemClock::GetUptimeMillis();
        // This process is more important if the top activity is
        // bound to the service.
        HashSet< AutoPtr<CServiceRecord> >::Iterator it = app->mServices.Begin();
        for (; it != app->mServices.End() && adj > ProcessList::FOREGROUND_APP_ADJ; ++it) {
            AutoPtr<CServiceRecord> s = *it;
            if (s->mStartRequested) {
                if (app->mHasShownUi && app != mHomeProcess) {
                    // If this process has shown some UI, let it immediately
                    // go to the LRU list because it may be pretty heavy with
                    // UI stuff.  We'll tag it with a label just to help
                    // debug and understand what is going on.
                    if (adj > ProcessList::SERVICE_ADJ) {
                        app->mAdjType = String("started-bg-ui-services");
                    }
                }
                else {
                    if (now < (s->mLastActivity + ActiveServices::MAX_SERVICE_INACTIVITY)) {
                        // This service has seen some activity within
                        // recent memory, so we will keep its process ahead
                        // of the background processes.
                        if (adj > ProcessList::SERVICE_ADJ) {
                            adj = ProcessList::SERVICE_ADJ;
                            app->mAdjType = String("started-services");
                            app->mHidden = FALSE;
                        }
                    }
                    // If we have let the service slide into the background
                    // state, still have some text describing what it is doing
                    // even though the service no longer has an impact.
                    if (adj > ProcessList::SERVICE_ADJ) {
                        app->mAdjType = String("started-bg-services");
                    }
                    // Don't kill this process because it is doing work; it
                    // has said it is doing work.
                    app->mKeeping = TRUE;
                }
            }

            if (s->mConnections.IsEmpty() == FALSE && (adj > ProcessList::FOREGROUND_APP_ADJ
                    || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE)) {
                CServiceRecord::ConnectionIterator::Iterator kt = s->mConnections.Begin();
                for (; kt != s->mConnections.End() && adj > ProcessList::FOREGROUND_APP_ADJ; ++kt) {
                    AutoPtr<List< AutoPtr<ConnectionRecord> > > clist = kt->mSecond;
                    List< AutoPtr<ConnectionRecord> >::Iterator lt = clist->Begin();
                    for (; lt != clist->End() && adj > ProcessList::FOREGROUND_APP_ADJ; ++lt) {
                        // XXX should compute this based on the max of
                        // all connected clients.
                        AutoPtr<ConnectionRecord> cr = *lt;
                        if (cr->mBinding->mClient == app) {
                            // Binding to ourself is not interesting.
                            continue;
                        }
                        if ((cr->mFlags & IContext::BIND_WAIVE_PRIORITY) == 0) {
                            AutoPtr<ProcessRecord> client = cr->mBinding->mClient;
                            Int32 clientAdj = adj;
                            Int32 myHiddenAdj = hiddenAdj;
                            if (myHiddenAdj > client->mHiddenAdj) {
                                if (client->mHiddenAdj >= ProcessList::VISIBLE_APP_ADJ) {
                                    myHiddenAdj = client->mHiddenAdj;
                                }
                                else {
                                    myHiddenAdj = ProcessList::VISIBLE_APP_ADJ;
                                }
                            }

                            Int32 myClientHiddenAdj = clientHiddenAdj;
                            if (myClientHiddenAdj > client->mClientHiddenAdj) {
                                if (client->mClientHiddenAdj >= ProcessList::VISIBLE_APP_ADJ) {
                                    myClientHiddenAdj = client->mClientHiddenAdj;
                                }
                                else {
                                    myClientHiddenAdj = ProcessList::VISIBLE_APP_ADJ;
                                }
                            }
                            Int32 myEmptyAdj = emptyAdj;
                            if (myEmptyAdj > client->mEmptyAdj) {
                                if (client->mEmptyAdj >= ProcessList::VISIBLE_APP_ADJ) {
                                    myEmptyAdj = client->mEmptyAdj;
                                }
                                else {
                                    myEmptyAdj = ProcessList::VISIBLE_APP_ADJ;
                                }
                            }

                            clientAdj = ComputeOomAdjLocked(client, myHiddenAdj,
                                    myClientHiddenAdj, myEmptyAdj, TOP_APP, TRUE, doingAll);
                            String adjType;

                            if ((cr->mFlags&IContext::BIND_ALLOW_OOM_MANAGEMENT) != 0) {
                                // Not doing bind OOM management, so treat
                                // this guy more like a started service.
                                if (app->mHasShownUi && app != mHomeProcess) {
                                    // If this process has shown some UI, let it immediately
                                    // go to the LRU list because it may be pretty heavy with
                                    // UI stuff.  We'll tag it with a label just to help
                                    // debug and understand what is going on.
                                    if (adj > clientAdj) {
                                        adjType = String("bound-bg-ui-services");
                                    }
                                    app->mHidden = FALSE;
                                    clientAdj = adj;
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
                                            adjType = String("bound-bg-services");
                                        }
                                        clientAdj = adj;
                                    }
                                }
                            }
                            else if ((cr->mFlags&IContext::BIND_AUTO_CREATE) != 0) {
                                if ((cr->mFlags&IContext::BIND_NOT_VISIBLE) == 0) {
                                    // If this connection is keeping the service
                                    // created, then we want to try to better follow
                                    // its memory management semantics for activities.
                                    // That is, if it is sitting in the background
                                    // LRU list as a hidden process (with activities),
                                    // we don't want the service it is connected to
                                    // to go into the empty LRU and quickly get killed,
                                    // because I'll we'll do is just end up restarting
                                    // the service.
                                    app->mHasClientActivities |= client->mHasActivities;
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
                                    adjType = String("bound-bg-ui-services");
                                }
                                else {
                                    if ((cr->mFlags&(IContext::BIND_ABOVE_CLIENT
                                            | IContext::BIND_IMPORTANT)) != 0) {
                                        adj = clientAdj;
                                    }
                                    else if ((cr->mFlags&IContext::BIND_NOT_VISIBLE) != 0
                                            && clientAdj < ProcessList::PERCEPTIBLE_APP_ADJ
                                            && adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                                        adj = ProcessList::PERCEPTIBLE_APP_ADJ;
                                    }
                                    else if (clientAdj > ProcessList::VISIBLE_APP_ADJ) {
                                        adj = clientAdj;
                                    }
                                    else {
                                        app->mPendingUiClean = TRUE;
                                        if (adj > ProcessList::VISIBLE_APP_ADJ) {
                                            adj = ProcessList::VISIBLE_APP_ADJ;
                                        }
                                    }
                                    if (!client->mHidden) {
                                        app->mHidden = FALSE;
                                    }
                                    if (client->mKeeping) {
                                        app->mKeeping = TRUE;
                                    }
                                    adjType = String("service");
                                }
                            }

                            if (!adjType.IsNull()) {
                                app->mAdjType = adjType;
                                app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo::REASON_SERVICE_IN_USE;
                                app->mAdjSource = (IInterface*)cr->mBinding->mClient;
                                app->mAdjSourceOom = clientAdj;
                                app->mAdjTarget = (IObject*)s->mName->Probe(EIID_IObject);
                            }
                            if ((cr->mFlags & IContext::BIND_NOT_FOREGROUND) == 0) {
                                if (client->mCurSchedGroup == IProcess::THREAD_GROUP_DEFAULT) {
                                    schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                                }
                            }
                        }

                        AutoPtr<ActivityRecord> a = cr->mActivity;
                        if ((cr->mFlags & IContext::BIND_ADJUST_WITH_ACTIVITY) != 0) {
                            if (a != NULL && adj > ProcessList::FOREGROUND_APP_ADJ &&
                                    (a->mVisible || a->mState == ActivityState_RESUMED
                                     || a->mState == ActivityState_PAUSING)) {
                                adj = ProcessList::FOREGROUND_APP_ADJ;
                                if ((cr->mFlags&IContext::BIND_NOT_FOREGROUND) == 0) {
                                    schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                                }
                                app->mHidden = FALSE;
                                app->mAdjType = String("service");
                                app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo::REASON_SERVICE_IN_USE;
                                app->mAdjSource = (IInterface*)a;
                                app->mAdjSourceOom = adj;
                                app->mAdjTarget = (IObject*)s->mName->Probe(EIID_IObject);
                            }
                        }
                    }
                }
            }
        }

        // Finally, if this process has active services running in it, we
        // would like to avoid killing it unless it would prevent the current
        // application from running.  By default we put the process in
        // with the rest of the background processes; as we scan through
        // its services we may bump it up from there.
        if (adj > hiddenAdj) {
            adj = hiddenAdj;
            app->mHidden = FALSE;
            app->mAdjType = String("bg-services");
        }
    }

    if (app->mPubProviders.IsEmpty() == FALSE && (adj > ProcessList::FOREGROUND_APP_ADJ
            || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE)) {

        HashMap<String, AutoPtr<ContentProviderRecord> >::Iterator jt = app->mPubProviders.Begin();
        for (; jt != app->mPubProviders.End() && (adj > ProcessList::FOREGROUND_APP_ADJ
                || schedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE); ++jt) {
            AutoPtr<ContentProviderRecord> cpr = jt->mSecond;
            if (cpr->mConnections.IsEmpty() == FALSE) {
                List< AutoPtr<CContentProviderConnection> >::Iterator kt = cpr->mConnections.Begin();
                for (; kt != cpr->mConnections.End() && adj > ProcessList::FOREGROUND_APP_ADJ; ++kt) {
                    AutoPtr<CContentProviderConnection> conn = *kt;
                    AutoPtr<ProcessRecord> client = conn->mClient;
                    if (client.Get() == app) {
                        // Being our own client is not interesting.
                        continue;
                    }
                    Int32 myHiddenAdj = hiddenAdj;
                    if (myHiddenAdj > client->mHiddenAdj) {
                        if (client->mHiddenAdj > ProcessList::FOREGROUND_APP_ADJ) {
                            myHiddenAdj = client->mHiddenAdj;
                        }
                        else {
                            myHiddenAdj = ProcessList::FOREGROUND_APP_ADJ;
                        }
                    }
                    Int32 myClientHiddenAdj = clientHiddenAdj;
                    if (myClientHiddenAdj > client->mClientHiddenAdj) {
                        if (client->mClientHiddenAdj >= ProcessList::FOREGROUND_APP_ADJ) {
                            myClientHiddenAdj = client->mClientHiddenAdj;
                        }
                        else {
                            myClientHiddenAdj = ProcessList::FOREGROUND_APP_ADJ;
                        }
                    }
                    Int32 myEmptyAdj = emptyAdj;
                    if (myEmptyAdj > client->mEmptyAdj) {
                        if (client->mEmptyAdj > ProcessList::FOREGROUND_APP_ADJ) {
                            myEmptyAdj = client->mEmptyAdj;
                        }
                        else {
                            myEmptyAdj = ProcessList::FOREGROUND_APP_ADJ;
                        }
                    }
                    Int32 clientAdj = ComputeOomAdjLocked(client, myHiddenAdj,
                            myClientHiddenAdj, myEmptyAdj, TOP_APP, TRUE, doingAll);
                    if (adj > clientAdj) {
                        if (app->mHasShownUi && app != mHomeProcess
                                && clientAdj > ProcessList::PERCEPTIBLE_APP_ADJ) {
                            app->mAdjType = String("bg-ui-provider");
                        }
                        else {
                            adj = clientAdj > ProcessList::FOREGROUND_APP_ADJ
                                    ? clientAdj : ProcessList::FOREGROUND_APP_ADJ;
                            app->mAdjType = String("provider");
                        }
                        if (!client->mHiddenAdj) {
                            app->mHidden = FALSE;
                        }
                        if (client->mKeeping) {
                            app->mKeeping = TRUE;
                        }
                        app->mAdjType = String("provider");
                        app->mAdjTypeCode = IActivityManagerRunningAppProcessInfo::REASON_PROVIDER_IN_USE;
                        app->mAdjSource = (IInterface*)client;
                        app->mAdjSourceOom = clientAdj;
                        app->mAdjTarget = (IObject*)cpr->mName->Probe(EIID_IObject);
                    }
                    if (client->mCurSchedGroup == IProcess::THREAD_GROUP_DEFAULT) {
                        schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                    }
                }
            }
            // If the provider has external (non-framework) process
            // dependencies, ensure that its adjustment is at least
            // FOREGROUND_APP_ADJ.
            if (cpr->HasExternalProcessHandles()) {
                if (adj > ProcessList::FOREGROUND_APP_ADJ) {
                    adj = ProcessList::FOREGROUND_APP_ADJ;
                    schedGroup = IProcess::THREAD_GROUP_DEFAULT;
                    app->mHidden = FALSE;
                    app->mKeeping = TRUE;
                    app->mAdjType = String("provider");
                    app->mAdjTarget = (IObject*)cpr->mName->Probe(EIID_IObject);
                }
            }
        }
    }

    if (adj == ProcessList::SERVICE_ADJ) {
        if (doingAll) {
            app->mServiceb = mNewNumServiceProcs > (mNumServiceProcs/3);
            mNewNumServiceProcs++;
        }
        if (app->mServiceb) {
            adj = ProcessList::SERVICE_B_ADJ;
        }
    }
    else {
        app->mServiceb = FALSE;
    }

    app->mNonStoppingAdj = adj;

    if (adj < ProcessList::HIDDEN_APP_MIN_ADJ) {
        app->mKeeping = TRUE;
    }

    if (hasStoppingActivities) {
        // Only upgrade adjustment.
        if (adj > ProcessList::PERCEPTIBLE_APP_ADJ) {
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
            app->mAdjType = String("stopping");
        }
    }

    app->mCurRawAdj = adj;

    //Slogger::I(TAG, "OOM ADJ " + app + ": pid=" + app.pid +
    //      " adj=" + adj + " curAdj=" + app.curAdj + " maxAdj=" + app.maxAdj);
    if (adj > app->mMaxAdj) {
        adj = app->mMaxAdj;
        if (app->mMaxAdj <= ProcessList::PERCEPTIBLE_APP_ADJ) {
            schedGroup = IProcess::THREAD_GROUP_DEFAULT;
        }
    }
    if (adj < ProcessList::HIDDEN_APP_MIN_ADJ) {
        app->mKeeping = TRUE;
    }

    if (app->mHasAboveClient) {
        // If this process has bound to any services with BIND_ABOVE_CLIENT,
        // then we need to drop its adjustment to be lower than the service's
        // in order to honor the request.  We want to drop it by one adjustment
        // level...  but there is special meaning applied to various levels so
        // we will skip some of them.
        if (adj < ProcessList::FOREGROUND_APP_ADJ) {
            // System process will not get dropped, ever
        }
        else if (adj < ProcessList::VISIBLE_APP_ADJ) {
            adj = ProcessList::VISIBLE_APP_ADJ;
        }
        else if (adj < ProcessList::PERCEPTIBLE_APP_ADJ) {
            adj = ProcessList::PERCEPTIBLE_APP_ADJ;
        }
        else if (adj < ProcessList::HIDDEN_APP_MIN_ADJ) {
            adj = ProcessList::HIDDEN_APP_MIN_ADJ;
        }
        else if (adj < ProcessList::HIDDEN_APP_MAX_ADJ) {
            adj++;
        }
    }

    Int32 importance = app->mMemImportance;
    if (importance == 0 || adj != app->mCurAdj || schedGroup != app->mCurSchedGroup) {
        app->mCurAdj = adj;
        app->mCurSchedGroup = schedGroup;
        if (!interesting) {
            // For this reporting, if there is not something explicitly
            // interesting in this process then we will push it to the
            // background importance.
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
        }
        else if (adj >= ProcessList::HIDDEN_APP_MIN_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
        }
        else if (adj >= ProcessList::SERVICE_B_ADJ) {
            importance =  IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE;
        }
        else if (adj >= ProcessList::HOME_APP_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
        }
        else if (adj >= ProcessList::SERVICE_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE;
        }
        else if (adj >= ProcessList::HEAVY_WEIGHT_APP_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_CANT_SAVE_STATE;
        }
        else if (adj >= ProcessList::PERCEPTIBLE_APP_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_PERCEPTIBLE;
        }
        else if (adj >= ProcessList::VISIBLE_APP_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_VISIBLE;
        }
        else if (adj >= ProcessList::FOREGROUND_APP_ADJ) {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND;
        }
        else {
            importance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_PERSISTENT;
        }
    }

    Int32 changes = importance != app->mMemImportance ? ProcessChangeItem::CHANGE_PROCESS_STATE : 0;
    if (foregroundActivities != app->mForegroundActivities) {
        changes |= ProcessChangeItem::CHANGE_ACTIVITIES;
    }
    if (changes != 0) {
        if (DEBUG_PROCESS_OBSERVERS) {
            Slogger::I(TAG, "Changes in %s : %d", app->ToString().string(), changes);
        }
        app->mMemImportance = importance;
        app->mForegroundActivities = foregroundActivities;
        List< AutoPtr<ProcessChangeItem> >::Iterator it = mPendingProcessChanges.Begin();
        AutoPtr<ProcessChangeItem> item;
        for (; it != mPendingProcessChanges.End();) {
            item = *it;
            if (item->mPid == app->mPid) {
                if (DEBUG_PROCESS_OBSERVERS) {
                    Slogger::I(TAG, "Re-using existing item: "/* + item*/);
                }
                break;
            }
            else {
                ++it;
            }
        }

        if (it == mPendingProcessChanges.End()) {
            // No existing item in pending changes; need a new one.
            Int32 NA = mAvailProcessChanges.GetSize();
            if (NA > 0) {
                item = mAvailProcessChanges.GetBack();
                mAvailProcessChanges.PopBack();
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Retreiving available item: "/* + item*/);
            }
            else {
                item = new ProcessChangeItem();
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Allocating new item: "/* + item*/);
            }
            item->mChanges = 0;
            item->mPid = app->mPid;
            app->mInfo->GetUid(&(item->mUid));
            if (mPendingProcessChanges.IsEmpty()) {
                if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "*** Enqueueing dispatch processes changed!");
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(DISPATCH_PROCESSES_CHANGED, (IMessage**)&msg);
                msg->SendToTarget();
            }
            mPendingProcessChanges.PushBack(item);
        }
        item->mChanges |= changes;
        item->mProcessState = importance;
        item->mForegroundActivities = foregroundActivities;
        // if (DEBUG_PROCESS_OBSERVERS) Slogger::I(TAG, "Item "
        //         + StringUtils::Int32ToHexString(System::IdentityHashCode(item))
        //         + " " + app->ToString() + ": changes=" + item->mChanges
        //         + " importance=" + item->mProcessState
        //         + " foreground=" + item->mForegroundActivities
        //         + " type=" + app->mAdjType + " source=" + app->mAdjSource
        //         + " target=" + app->mAdjTarget);
    }

    return app->mCurRawAdj;
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
        if (q->mParallelBroadcasts.Begin() != q->mParallelBroadcasts.End() ||
                q->mOrderedBroadcasts.Begin() != q->mOrderedBroadcasts.End()) {
            processingBroadcasts = TRUE;
        }
    }
    return !processingBroadcasts
            && (mSleeping || (mMainStack->mResumedActivity != NULL &&
                    mMainStack->mResumedActivity->mIdle));
}

ECode CActivityManagerService::PerformAppGcsLocked()
{
    if (mProcessesToGc.Begin() == mProcessesToGc.End()) {
        return NOERROR;
    }
    if (CanGcNowLocked()) {
        while (mProcessesToGc.Begin() != mProcessesToGc.End()) {
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
    Millisecond64 now = SystemClock::GetUptimeMillis();
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

    if (stats->IsScreenOn()) {
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
        if (!app->mKeeping) {
            Int64 wtime;
            synchronized (stats) {
                Int32 uid;
                app->mInfo->GetUid(&uid);
                wtime = stats->GetProcessWakeTime(uid,
                        app->mPid, curRealtime);
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
                synchronized (stats) {
                    Int32 uid;
                    app->mInfo->GetUid(&uid);
                    stats->ReportExcessiveWakeLocked(uid, app->mProcessName,
                            realtimeSince, wtimeUsed);
                }
                Slogger::W(TAG, "Excessive wake lock in %s (pid %d): held %d during %d", app->mProcessName.string(),
                        app->mPid, wtimeUsed, realtimeSince);
    //             EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
    //                     app.processName, app.setAdj, "excessive wake lock");
                Process::KillProcessQuiet(app->mPid);
            }
            else if (doCpuKills && uptimeSince > 0
                    && ((cputimeUsed*100)/uptimeSince) >= 50) {
                    synchronized (stats) {
                        Int32 uid;
                        app->mInfo->GetUid(&uid);
                        stats->ReportExcessiveCpuLocked(uid, app->mProcessName,
                                uptimeSince, cputimeUsed);
                    }
                Slogger::W(TAG, "Excessive CPU in %s (pid %d): used %d during %d", app->mProcessName.string(),
                        app->mPid, cputimeUsed, uptimeSince);
    //             EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
    //                     app.processName, app.setAdj, "excessive cpu");
                Process::KillProcessQuiet(app->mPid);
            }
            else {
                app->mLastWakeTime = wtime;
                app->mLastCpuTime = app->mCurCpuTime;
            }
        }
    }
    return NOERROR;
}

Boolean CActivityManagerService::UpdateOomAdjLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 hiddenAdj,
    /* [in] */ Int32 clientHiddenAdj,
    /* [in] */ Int32 emptyAdj,
    /* [in] */ ProcessRecord* TOP_APP,
    /* [in] */ Boolean doingAll)
{
    app->mHiddenAdj = hiddenAdj;
    app->mClientHiddenAdj = clientHiddenAdj;
    app->mEmptyAdj = emptyAdj;

    if (app->mThread == NULL) {
        return FALSE;
    }

    Boolean wasKeeping = app->mKeeping;
    Boolean success = TRUE;
    ComputeOomAdjLocked(app, hiddenAdj, clientHiddenAdj, emptyAdj, TOP_APP, FALSE, doingAll);

    if (app->mCurRawAdj != app->mSetRawAdj) {
        if (wasKeeping && !app->mKeeping) {
            // This app is no longer something we want to keep.  Note
            // its current wake lock time to later know to kill it if
            // it is not behaving well.
            AutoPtr<IBatteryStatsImpl> stats = mBatteryStatsService->GetActiveStatistics();
            synchronized (stats) {
                Int32 uid;
                app->mInfo->GetUid(&uid);
                app->mLastWakeTime = stats->GetProcessWakeTime(
                    uid, app->mPid, SystemClock::GetElapsedRealtime());
            }
            app->mLastCpuTime = app->mCurCpuTime;
        }

        app->mSetRawAdj = app->mCurRawAdj;
    }
    if (app->mCurAdj != app->mSetAdj) {
        if (Process::SetOomAdj(app->mPid, app->mCurAdj)) {
            if (DEBUG_SWITCH || DEBUG_OOM_ADJ) {
                Slogger::V(TAG, "Set %d %s adj %d: %s",
                        app->mPid, app->mProcessName.string(),
                        app->mCurAdj, app->mAdjType.string());
            }
            app->mSetAdj = app->mCurAdj;
        }
        else {
            success = FALSE;
            Slogger::W(TAG, "Failed setting oom adj of %s to %d",
                    app->ToString().string(), app->mCurAdj);
        }
    }

    if (app->mSetSchedGroup != app->mCurSchedGroup) {
        app->mSetSchedGroup = app->mCurSchedGroup;
        if (DEBUG_SWITCH || DEBUG_OOM_ADJ) {
            Slogger::V(TAG, "Setting process group of %s to %d",
                    app->mProcessName.string(), app->mCurSchedGroup);
        }
        if (!app->mWaitingToKill.IsNull() &&
                app->mSetSchedGroup == IProcess::THREAD_GROUP_BG_NONINTERACTIVE) {
            Slogger::I(TAG, "Killing %s: %s",
                    app->ToShortString().string(), app->mWaitingToKill.string());
            //EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
            //        app.processName, app.setAdj, app.waitingToKill);
            app->mKilledBackground = TRUE;
            Process::KillProcessQuiet(app->mPid);
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
        }
    }

    return success;
}

AutoPtr<ActivityRecord> CActivityManagerService::ResumedAppLocked()
{
    AutoPtr<ActivityRecord> resumedActivity = mMainStack->mResumedActivity;
    if (resumedActivity == NULL || resumedActivity->mApp == NULL) {
        resumedActivity = mMainStack->mPausingActivity;
        if (resumedActivity == NULL || resumedActivity->mApp == NULL) {
            resumedActivity = mMainStack->GetTopRunningActivityLocked(NULL);
        }
    }
    return resumedActivity;
}

Boolean CActivityManagerService::UpdateOomAdjLocked(
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<ActivityRecord> TOP_ACT = ResumedAppLocked();
    AutoPtr<ProcessRecord> TOP_APP = TOP_ACT != NULL ? TOP_ACT->mApp : NULL;
    Int32 curAdj = app->mCurAdj;
    UNUSED(curAdj);
    Boolean wasHidden = app->mCurAdj >= ProcessList::HIDDEN_APP_MIN_ADJ
            && app->mCurAdj <= ProcessList::HIDDEN_APP_MAX_ADJ;

    mAdjSeq++;

    Boolean success = UpdateOomAdjLocked(app, app->mHiddenAdj, app->mClientHiddenAdj,
            app->mEmptyAdj, TOP_APP, FALSE);
    Boolean nowHidden = app->mCurAdj >= ProcessList::HIDDEN_APP_MIN_ADJ
            && app->mCurAdj <= ProcessList::HIDDEN_APP_MAX_ADJ;
    if (nowHidden != wasHidden) {
        // Changed to/from hidden state, so apps after it in the LRU
        // list may also be changed.
        UpdateOomAdjLocked();
    }
    return success;
}

ECode CActivityManagerService::UpdateOomAdjLocked()
{
    AutoPtr<ActivityRecord> TOP_ACT = ResumedAppLocked();
    AutoPtr<ProcessRecord> TOP_APP = TOP_ACT != NULL ? TOP_ACT->mApp : NULL;
    Int64 oldTime = SystemClock::GetUptimeMillis() - ProcessList::MAX_EMPTY_TIME;
//    if (FALSE) {
//        RuntimeException e = new RuntimeException();
//        e.fillInStackTrace();
//        Slogger::I(TAG, "updateOomAdj: top=" + TOP_ACT, e);
//    }

    mAdjSeq++;
    mNewNumServiceProcs = 0;

    Int32 emptyProcessLimit;
    Int32 hiddenProcessLimit;
    if (mProcessLimit <= 0) {
        emptyProcessLimit = hiddenProcessLimit = 0;
    }
    else if (mProcessLimit == 1) {
        emptyProcessLimit = 1;
        hiddenProcessLimit = 0;
    }
    else {
        emptyProcessLimit = (mProcessLimit*2)/3;
        hiddenProcessLimit = mProcessLimit - emptyProcessLimit;
    }
    // Let's determine how many processes we have running vs.
    // how many slots we have for background processes; we may want
    // to put multiple processes in a slot of there are enough of
    // them.

    Int32 numSlots = (ProcessList::HIDDEN_APP_MAX_ADJ
                - ProcessList::HIDDEN_APP_MIN_ADJ + 1) / 2;
    Int32 numEmptyProcs = mLruProcesses.GetSize() - mNumNonHiddenProcs - mNumHiddenProcs;
    if (numEmptyProcs > hiddenProcessLimit) {
        // If there are more empty processes than our limit on hidden
        // processes, then use the hidden process limit for the factor.
        // This ensures that the really old empty processes get pushed
        // down to the bottom, so if we are running low on memory we will
        // have a better chance at keeping around more hidden processes
        // instead of a gazillion empty processes.
        numEmptyProcs = hiddenProcessLimit;
    }

    Int32 emptyFactor = numEmptyProcs/numSlots;
    if (emptyFactor < 1) emptyFactor = 1;
    Int32 hiddenFactor = (mNumHiddenProcs > 0 ? mNumHiddenProcs : 1)/numSlots;
    if (hiddenFactor < 1) hiddenFactor = 1;
    Int32 stepHidden = 0;
    Int32 stepEmpty = 0;
    Int32 numHidden = 0;
    Int32 numEmpty = 0;
    Int32 numTrimming = 0;

    mNumNonHiddenProcs = 0;
    mNumHiddenProcs = 0;

    // First update the OOM adjustment for each of the
    // application processes based on their current state.
    Int32 i = mLruProcesses.GetSize();
    UNUSED(i);
    Int32 curHiddenAdj = ProcessList::HIDDEN_APP_MIN_ADJ;
    Int32 nextHiddenAdj = curHiddenAdj + 1;
    Int32 curEmptyAdj = ProcessList::HIDDEN_APP_MIN_ADJ;
    Int32 nextEmptyAdj = curEmptyAdj + 2;
    Int32 curClientHiddenAdj = curEmptyAdj;

    List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mLruProcesses.RBegin();
    for (; rit != mLruProcesses.REnd(); ++rit) {
        AutoPtr<ProcessRecord> app = *rit;
        // Slogger::I(TAG, "OOM app = %s, cur hidden = %d\n", app->ToString().string(), curHiddenAdj);
        //printf("OOM app = %s, cur hidden = %d\n", app->ToString().string(), curHiddenAdj);
        UpdateOomAdjLocked(app, curHiddenAdj, curClientHiddenAdj, curEmptyAdj, TOP_APP, TRUE);
        //printf("app->mCurRawAdj = %d, app->mAdjType = %s\n", app->mCurRawAdj, app->mAdjType.string());
        if (!app->mKilledBackground) {
            if (app->mCurRawAdj == curHiddenAdj && app->mHasActivities) {
                // This process was assigned as a hidden process...  step the
                // hidden level.
                mNumHiddenProcs++;
                if (curHiddenAdj != nextHiddenAdj) {
                    stepHidden++;
                    if (stepHidden >= hiddenFactor) {
                        stepHidden = 0;
                        curHiddenAdj = nextHiddenAdj;
                        nextHiddenAdj += 2;
                        if (nextHiddenAdj > ProcessList::HIDDEN_APP_MAX_ADJ) {
                            nextHiddenAdj = ProcessList::HIDDEN_APP_MAX_ADJ;
                        }
                        if (curClientHiddenAdj <= curHiddenAdj) {
                            curClientHiddenAdj = curHiddenAdj + 1;
                            if (curClientHiddenAdj > ProcessList::HIDDEN_APP_MAX_ADJ) {
                                curClientHiddenAdj = ProcessList::HIDDEN_APP_MAX_ADJ;
                            }
                        }
                    }
                }
                numHidden++;
                if (numHidden > hiddenProcessLimit) {
                    Slogger::I(TAG, "No longer want %s (pid %d): hidden #%d",
                            app->mProcessName.string(), app->mPid, numHidden);
                    //EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
                    //        app.processName, app.setAdj, "too many background");
                    app->mKilledBackground = TRUE;
                    Process::KillProcessQuiet(app->mPid);
                }
            }
            else if (app->mCurRawAdj == curHiddenAdj && app->mHasClientActivities) {
                // This process has a client that has activities.  We will have
                // given it the current hidden adj; here we will just leave it
                // without stepping the hidden adj.
                curClientHiddenAdj++;
                if (curClientHiddenAdj > ProcessList::HIDDEN_APP_MAX_ADJ) {
                    curClientHiddenAdj = ProcessList::HIDDEN_APP_MAX_ADJ;
                }
            }
            else {
                if (app->mCurRawAdj == curEmptyAdj || app->mCurRawAdj == curHiddenAdj) {
                    // This process was assigned as an empty process...  step the
                    // empty level.
                    if (curEmptyAdj != nextEmptyAdj) {
                        stepEmpty++;
                        if (stepEmpty >= emptyFactor) {
                            stepEmpty = 0;
                            curEmptyAdj = nextEmptyAdj;
                            nextEmptyAdj += 2;
                            if (nextEmptyAdj > ProcessList::HIDDEN_APP_MAX_ADJ) {
                                nextEmptyAdj = ProcessList::HIDDEN_APP_MAX_ADJ;
                            }
                        }
                    }
                }
                else if (app->mCurRawAdj < ProcessList::HIDDEN_APP_MIN_ADJ) {
                    mNumNonHiddenProcs++;
                }
                if (app->mCurAdj >= ProcessList::HIDDEN_APP_MIN_ADJ
                        && !app->mHasClientActivities) {
                    if (numEmpty > ProcessList::TRIM_EMPTY_APPS
                            && app->mLastActivityTime < oldTime) {
                        Slogger::I(TAG, "No longer want %s (pid %d): empty for %d s.",
                                app->mProcessName.string(), app->mPid,
                                ((oldTime + ProcessList::MAX_EMPTY_TIME - app->mLastActivityTime)/ 1000));
                        //EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
                        //        app.processName, app.setAdj, "old background process");
                        app->mKilledBackground = TRUE;
                        Process::KillProcessQuiet(app->mPid);
                    }
                    else {
                        numEmpty++;
                        if (numEmpty > emptyProcessLimit) {
                            Slogger::I(TAG, "No longer want %s (pid %d):empty #%d",
                                    app->mProcessName.string(), app->mPid, numEmpty);
                            //EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
                            //        app.processName, app.setAdj, "too many background");
                            app->mKilledBackground = TRUE;
                            Process::KillProcessQuiet(app->mPid);
                        }
                    }
                }
            }
            if (app->mIsolated && app->mServices.IsEmpty()) {
                // If this is an isolated process, and there are no
                // services running in it, then the process is no longer
                // needed.  We agressively kill these because we can by
                // definition not re-use the same process again, and it is
                // good to avoid having whatever code was running in them
                // left sitting around after no longer needed.
                Slogger::I(TAG, "Isolated process %s (pid %d) no longer needed.",
                        app->mProcessName.string(), app->mPid);
                //EventLog.writeEvent(EventLogTags.AM_KILL, app.userId, app.pid,
                //        app.processName, app.setAdj, "isolated not needed");
                app->mKilledBackground = TRUE;
                Process::KillProcessQuiet(app->mPid);
            }
            if (app->mNonStoppingAdj >= ProcessList::HOME_APP_ADJ
                    && app->mNonStoppingAdj != ProcessList::SERVICE_B_ADJ
                    && !app->mKilledBackground) {
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
    if (numHidden <= ProcessList::TRIM_HIDDEN_APPS
            && numEmpty <= ProcessList::TRIM_EMPTY_APPS)
    {
        Int32 numHiddenAndEmpty = numHidden + numEmpty;
        Int32 factor = numTrimming/3;
        Int32 minFactor = 2;
        if (mHomeProcess != NULL) minFactor++;
        if (mPreviousProcess != NULL) minFactor++;
        if (factor < minFactor) factor = minFactor;
        Int32 step = 0;
        Int32 fgTrimLevel;
        if (numHiddenAndEmpty <= ProcessList::TRIM_CRITICAL_THRESHOLD) {
            fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL;
        }
        else if (numHiddenAndEmpty <= ProcessList::TRIM_LOW_THRESHOLD) {
            fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_LOW;
        }
        else {
            fgTrimLevel = IComponentCallbacks2::TRIM_MEMORY_RUNNING_MODERATE;
        }
        Int32 curLevel = IComponentCallbacks2::TRIM_MEMORY_COMPLETE;

        rit = mLruProcesses.RBegin();
        for (; rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if (app->mNonStoppingAdj >= ProcessList::HOME_APP_ADJ
                    && app->mNonStoppingAdj != ProcessList::SERVICE_B_ADJ
                    && !app->mKilledBackground) {
                if (app->mTrimMemoryLevel < curLevel && app->mThread != NULL) {
                    //try {
                        app->mThread->ScheduleTrimMemory(curLevel);
                    //} catch (RemoteException e) {
                    //}
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
                            mMainStack->ScheduleDestroyActivities(app, FALSE, String("trim"));
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
            else if (app->mNonStoppingAdj == ProcessList::HEAVY_WEIGHT_APP_ADJ) {
                if (app->mTrimMemoryLevel < IComponentCallbacks2::TRIM_MEMORY_BACKGROUND
                        && app->mThread != NULL) {
                    //try {
                        app->mThread->ScheduleTrimMemory(
                                IComponentCallbacks2::TRIM_MEMORY_BACKGROUND);
                    //} catch (RemoteException e) {
                    //}
                }
                app->mTrimMemoryLevel = IComponentCallbacks2::TRIM_MEMORY_BACKGROUND;
            }
            else {
                if ((app->mNonStoppingAdj > ProcessList::VISIBLE_APP_ADJ || app->mSystemNoUi)
                        && app->mPendingUiClean) {
                    // If this application is now in the background and it
                    // had done UI, then give it the special trim level to
                    // have it free UI resources.
                    Int32 level = IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN;
                    if (app->mTrimMemoryLevel < level && app->mThread != NULL) {
                        //try {
                            app->mThread->ScheduleTrimMemory(level);
                        //} catch (RemoteException e) {
                        //}
                    }
                    app->mPendingUiClean = FALSE;
                }
                if (app->mTrimMemoryLevel < fgTrimLevel && app->mThread != NULL) {
                    //try {
                        app->mThread->ScheduleTrimMemory(fgTrimLevel);
                    //} catch (RemoteException e) {
                    //}
                }
                app->mTrimMemoryLevel = fgTrimLevel;
            }
        }
    }
    else
    {
        rit = mLruProcesses.RBegin();
        for (; rit != mLruProcesses.REnd(); ++rit) {
            AutoPtr<ProcessRecord> app = *rit;
            if ((app->mNonStoppingAdj > ProcessList::VISIBLE_APP_ADJ || app->mSystemNoUi)
                    && app->mPendingUiClean) {
                if (app->mTrimMemoryLevel < IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN
                        && app->mThread != NULL) {
                    //try {
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
        mMainStack->ScheduleDestroyActivities(NULL, FALSE, String("always-finish"));
    }
    return NOERROR;
}

ECode CActivityManagerService::TrimApplications()
{
    {
        AutoLock lock(this);

        // First remove any unused application processes whose package
        // has been removed.
        // for (i = mRemovedProcesses.GetSize() - 1; i >= 0; i--)
        List< AutoPtr<ProcessRecord> >::ReverseIterator rit = mRemovedProcesses.RBegin();
        while(rit != mRemovedProcesses.REnd()) {
            AutoPtr<ProcessRecord> app = *rit;
            if (app->mActivities.IsEmpty()
                && app->mCurReceiver == NULL && app->mServices.IsEmpty()) {
                // Slogger::I(
                //     TAG, "Exiting empty application process "
                //     + app.processName + " ("
                //     + (app.thread != NULL ? app.thread.asBinder() : NULL)
                //     + ")\n");
                if (app->mPid > 0 && app->mPid != MY_PID) {
                    // EventLog.writeEvent(EventLogTags.AM_KILL, app->mUserId, app->mPid,
                    // app->mProcessName, app->mSetAdj, "empty");
                    Process::KillProcessQuiet(app->mPid);
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

                rit = List< AutoPtr<ProcessRecord> >::ReverseIterator(mRemovedProcesses.Erase(--(rit.GetBase())));

                if (app->mPersistent) {
                    if (app->mPersistent) {
                        AddAppLocked(app->mInfo, FALSE);
                    }
                }
                continue;
            }
            ++rit;
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
    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::SIGNAL_PERSISTENT_PROCESSES)
            != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires permission "
        //         + Elastos::Droid::Manifest::permission::SIGNAL_PERSISTENT_PROCESSES);
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
    /* [in] */ const String& path,
    /* [in] */ Int32 profileType)
{
    String newPath = path;
    if (proc == NULL || proc == mProfileProc) {
        proc = mProfileProc;
        newPath = mProfileFile;
        profileType = mProfileType;
        ClearProfilerLocked();
    }
    if (proc == NULL) {
        return NOERROR;
    }
    //try {
    proc->mThread->ProfilerControl(FALSE, newPath, NULL, profileType);
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
    return NOERROR;
}

ECode CActivityManagerService::ProfileControl(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean start,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 profileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    {
        AutoLock lock(this);
        // note: hijacking SET_ACTIVITY_WATCHER, but should be changed to
        // its own permission.
        if (CheckCallingPermission(Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER)
                != IPackageManager::PERMISSION_GRANTED) {
            // throw new SecurityException("Requires permission "
            //         + Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER);
            if (fd != NULL) {
                fd->Close();
            }
            return E_SECURITY_EXCEPTION;
        }

        if (start && fd == NULL) {
            // throw new IllegalArgumentException("null fd");
            if (fd != NULL) {
                fd->Close();
            }
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
            StopProfilerLocked(NULL, String(NULL), 0);
            SetProfileApp(proc->mInfo, proc->mProcessName, path, fd, FALSE);
            mProfileProc = proc;
            mProfileType = profileType;
            // try {
            AutoPtr<IFileDescriptor> descriptor;
            fd->GetFileDescriptor((IFileDescriptor**)&descriptor);
            AutoPtr<IParcelFileDescriptorHelper> helper;
            CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
            AutoPtr<IParcelFileDescriptor> pfd;
            if (SUCCEEDED(helper->Dup(descriptor, (IParcelFileDescriptor**)&pfd))) {
                fd = NULL;
                fd = pfd;
            }
            else {
                fd = NULL;
            }
            // } catch (IOException e) {
            //     fd = null;
            // }
            proc->mThread->ProfilerControl(start, path, fd, profileType);
            fd = NULL;
            mProfileFd = NULL;
        }
        else {
            StopProfilerLocked(proc, path, profileType);
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
            userId, TRUE, TRUE, callName, String(NULL), &userId);
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
            if (CheckCallingPermission(Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER)
                    != IPackageManager::PERMISSION_GRANTED) {
                // throw new SecurityException("Requires permission "
                //         + Elastos::Droid::Manifest::permission::SET_ACTIVITY_WATCHER);
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
            Boolean isDebuggable = CString("1").Equals(value);
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

ECode CActivityManagerService::SwitchUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: switchUser() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString().string());
        return E_SECURITY_EXCEPTION;
    }

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

            AutoPtr<IUserInfo> userInfo;
            GetUserManagerLocked()->GetUserInfo(userId, (IUserInfo**)&userInfo);
            if (userInfo == NULL) {
                Slogger::W(TAG, "No user info for user #%d", userId);
                *result = FALSE;
                Binder::RestoreCallingIdentity(ident);
                return NOERROR;
            }

            mWindowManager->StartFreezingScreen(R::anim::screen_user_exit,
                    R::anim::screen_user_enter);

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

            mCurrentUserId = userId;
            mCurrentUserArray = ArrayOf<Int32>::Alloc(1);
            mCurrentUserArray->Set(0, userId);
            mUserLru.Remove(userId);
            mUserLru.PushBack(userId);

            mWindowManager->SetCurrentUser(userId);

            // Once the internal notion of the active user has switched, we lock the device
            // with the option to show the user switcher on the keyguard.
            mWindowManager->LockNow(NULL);

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

            mHandler->RemoveMessages(REPORT_USER_SWITCH_MSG);
            mHandler->RemoveMessages(USER_SWITCH_TIMEOUT_MSG);

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(REPORT_USER_SWITCH_MSG, oldUserId, userId,
                uss->Probe(EIID_IInterface), (IMessage**)&msg);
            Boolean result;
            mHandler->SendMessage(msg, &result);

            AutoPtr<IMessage> msg2;
            mHandler->ObtainMessage(USER_SWITCH_TIMEOUT_MSG, oldUserId, userId,
                uss->Probe(EIID_IInterface), (IMessage**)&msg2);
            mHandler->SendMessageDelayed(msg2, USER_SWITCH_TIMEOUT, &result);

            if (needStart) {
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_USER_STARTED, (IIntent**)&intent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                        | IIntent::FLAG_RECEIVER_FOREGROUND);
                intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                Int32 bValue;
                BroadcastIntentLocked(NULL, String(NULL), intent,
                    String(NULL), NULL, 0, String(NULL), NULL, String(NULL),
                    FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &bValue);
            }

            Int32 userInfoFlags;
            userInfo->GetFlags(&userInfoFlags);
            if ((userInfoFlags&IUserInfo::FLAG_INITIALIZED) == 0) {
                if (userId != 0) {
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_USER_INITIALIZE, (IIntent**)&intent);
                    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                    AutoPtr<IIntentReceiver> receiver = new SwitchUserIntentReceiver(this, uss, userId);
                    Int32 bValue;
                    BroadcastIntentLocked(NULL, String(NULL), intent, String(NULL),
                            receiver, 0, String(NULL), NULL, String(NULL), TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID,
                            userId, &bValue);
                    uss->mInitializing = TRUE;
                }
                else {
                    Int32 userInfoId;
                    userInfo->GetId(&userInfoId);
                    GetUserManagerLocked()->MakeInitialized(userInfoId);
                }
            }

            Boolean haveActivities = mMainStack->SwitchUserLocked(userId, uss);
            if (!haveActivities) {
                StartHomeActivityLocked(userId);
            }
    //         EventLogTags.writeAmSwitchUser(userId);
            GetUserManagerLocked()->UserForeground(userId);
            SendUserSwitchBroadcastsLocked(oldUserId, userId);
            if (needStart) {
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_USER_STARTING, (IIntent**)&intent);
                intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
                intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
                AutoPtr<IIntentReceiver> receiver = new NeedStartIntentReceiver();
                Int32 bValue;
                BroadcastIntentLocked(NULL, String(NULL), intent,
                        String(NULL), receiver, 0, String(NULL), NULL,
                        Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS,
                        TRUE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &bValue);
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
        CIntent::New(IIntent::ACTION_USER_BACKGROUND, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                | IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, oldUserId);
        Int32 result;
        BroadcastIntentLocked(NULL, nullStr, intent,
            nullStr, NULL, 0, nullStr, NULL, nullStr,
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, oldUserId, &result);
    }
    if (newUserId >= 0) {
        intent = NULL;
        CIntent::New(IIntent::ACTION_USER_FOREGROUND, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
                | IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, newUserId);
        Int32 result;
        BroadcastIntentLocked(NULL, nullStr, intent,
            nullStr, NULL, 0, nullStr, NULL, nullStr,
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, newUserId, &result);
        intent = NULL;
        CIntent::New(IIntent::ACTION_USER_SWITCHED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, newUserId);

        BroadcastIntentLocked(NULL, nullStr, intent,
                nullStr, NULL, 0, nullStr, NULL,
                Elastos::Droid::Manifest::permission::MANAGE_USERS,
                FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, IUserHandle::USER_ALL, &result);
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
        AutoPtr<IRemoteCallback> callback = new DispatchUserSwitchCallback(this, N, uss, oldUserId, newUserId);

        {
            AutoLock lock(this);
            uss->mSwitching = TRUE;
            mCurUserSwitchCallback = callback;
        }
        for (Int32 i = 0; i < N; i++) {
            // try {
            AutoPtr<IInterface> item;
            mUserSwitchObservers->GetBroadcastItem(i, (IInterface**)&item);
            AutoPtr<IUserSwitchObserver> observer = IUserSwitchObserver::Probe(item);
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

ECode CActivityManagerService::UserInitialized(
    /* [in] */ UserStartedState* uss,
    /* [in] */ Int32 newUserId)
{
    return CompleteSwitchAndInitalize(uss, newUserId, TRUE, FALSE);
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
            AutoPtr<IUserSwitchObserver> observer = IUserSwitchObserver::Probe(item);
            observer->OnUserSwitchComplete(newUserId);
            // } catch (RemoteException e) {
            // }
        }
        mUserSwitchObservers->FinishBroadcast();
    }
    return NOERROR;
}

ECode CActivityManagerService::FinishUserSwitch(
    /* [in] */ UserStartedState* uss)
{
    VALIDATE_NOT_NULL(uss);

    AutoLock lock(this);
    Int32 id;
    uss->mHandle->GetIdentifier(&id);
    HashMap<Int32, AutoPtr<UserStartedState> >::Iterator it = mStartedUsers.Find(id);
    AutoPtr<UserStartedState> state;
    if (it != mStartedUsers.End()) state = it->mSecond;
    if (uss->mState == UserStartedState::STATE_BOOTING
            && state.Get() == uss) {
        uss->mState = UserStartedState::STATE_RUNNING;
        Int32 userId;
        uss->mHandle->GetIdentifier(&userId);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_BOOT_COMPLETED, NULL, (IIntent**)&intent);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
        Int32 result;
        String nullStr;
        BroadcastIntentLocked(NULL, nullStr, intent,
            nullStr, NULL, 0, nullStr, NULL,
            Elastos::Droid::Manifest::permission::RECEIVE_BOOT_COMPLETED,
            FALSE, FALSE, MY_PID, IProcess::SYSTEM_UID, userId, &result);
    }
    Int32 num = mUserLru.GetSize();
    Int32 i = 0;
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

    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: switchUser() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }
    if (userId <= 0) {
        //throw new IllegalArgumentException("Can't stop primary user " + userId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(this);
    *result = StopUserLocked(userId, callback);
    return NOERROR;
}

Int32 CActivityManagerService::StopUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ IStopUserCallback* callback)
{
    if (mCurrentUserId == userId) {
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
            AutoPtr<IIntent> shutdownIntent;
            CIntent::New(IIntent::ACTION_SHUTDOWN, (IIntent**)&shutdownIntent);

            // This is the result receiver for the final shutdown broadcast.
            AutoPtr<IIntentReceiver> shutdownReceiver;
            CShutdownReceiver::New((Handle32)this, (Handle32)uss.Get(), (IIntentReceiver**)&shutdownReceiver);

            // This is the result receiver for the initial stopping broadcast.
            AutoPtr<IIntentReceiver> stoppingReceiver;
            CStoppingReceiver::New((Handle32)this, (Handle32)uss.Get(), shutdownIntent,
                    shutdownReceiver, userId, (IIntentReceiver**)&stoppingReceiver);
            // Kick things off.
            Int32 result;
            String nullStr;
            BroadcastIntentLocked(NULL, nullStr, stoppingIntent,
                nullStr, stoppingReceiver, 0, nullStr, NULL,
                Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS,
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
            ForceStopUserLocked(userId);
            AutoPtr<AttributeCache> ac = AttributeCache::GetInstance();
            if (ac != NULL) {
                ac->RemoveUser(userId);
            }
        }
    }

    List<AutoPtr<IStopUserCallback> >::Iterator it;
    for (it = callbacks.Begin(); it != callbacks.End(); ++it) {
        //try {
        if (stopped) (*it)->UserStopped(userId);
        else (*it)->UserStopAborted(userId);
        //} catch (RemoteException e) {
        //}
    }
    return NOERROR;
}

ECode CActivityManagerService::GetCurrentUser(
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    if ((CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS)
            != IPackageManager::PERMISSION_GRANTED) && (
            CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)
            != IPackageManager::PERMISSION_GRANTED)) {
        Slogger::W(TAG, "Permission Denial: getCurrentUser() from pid=%d, uid=%d requires Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS", Binder::GetCallingPid, Binder::GetCallingUid);
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(this);
        return GetUserManagerLocked()->GetUserInfo(mCurrentUserId, userInfo);
    }

    return NOERROR;
}

Int32 CActivityManagerService::GetCurrentUserIdLocked()
{
    return mCurrentUserId;
}

ECode CActivityManagerService::IsUserRunning(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean orStopped,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS)
            != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: isUserRunning() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS";
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

   if (CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: isUserRunning() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS";
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
    /* [in] */ IUserSwitchObserver* observer)
{
   if (CheckCallingPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)
           != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder msg("Permission Denial: registerUserSwitchObserver() from pid=");
        msg += Binder::GetCallingPid();
        msg += ", uid=";
        msg += Binder::GetCallingUid();
        msg += " requires ";
        msg += "Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL";
        Slogger::W(TAG, msg.ToString());
        //throw new SecurityException(msg);
        return E_SECURITY_EXCEPTION;
    }

    Boolean result;
    return mUserSwitchObservers->Register(observer, &result);
}

ECode CActivityManagerService::UnregisterUserSwitchObserver(
    /* [in] */ IUserSwitchObserver* observer)
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
       AutoPtr<IBinder> b;
       AutoPtr<IInterface> iface;
       AutoPtr<IServiceManager> csm;
       ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&csm));
       ASSERT_SUCCEEDED(csm->GetService(IContext::USER_SERVICE, (IInterface**)&iface));
       mUserManager = (CUserManagerService*)IIUserManager::Probe(iface);
   }
   return mUserManager;
}

ECode CActivityManagerService::CheckValidCaller(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId)
{
    Int32 id = UserHandleGetUserId(uid);
    if (id == userId || uid == IProcess::SYSTEM_UID || uid == 0) return NOERROR;

    // throw new SecurityException("Caller uid=" + uid
    //         + " is not privileged to communicate with user=" + userId);
    return E_SECURITY_EXCEPTION;
}

Int32 CActivityManagerService::ApplyUserId(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId)
{
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Int32 id;
    uhHelper->GetUid(userId, uid, &id);
    return id;
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
    info->GetPackageName(&infoPkgName);
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
        aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);
        if (userId < 1 && uid < IUserHandle::PER_USER_RANGE) {
            return aInfo;
        }
    }

    AutoPtr<IActivityInfo> info;
    CActivityInfo::New(aInfo, (IActivityInfo**)&info);
    AutoPtr<IApplicationInfo> appInfo;
    info->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IApplicationInfo> userAppInfo = GetAppInfoForUser(appInfo, userId);
    info->SetApplicationInfo(userAppInfo);
    return info;
}

void CActivityManagerService::HandleShowErrorMsg(
    /* [in] */ StringObjectHashMap* data)
{
    AutoPtr<ISettingsSecure> secure;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 value;
    secure->GetInt32(resolver, ISettingsSecure::ANR_SHOW_BACKGROUND, 0, &value);
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
        Boolean isBackground = (UserHandleGetAppId(proc->mUid)
                >= IProcess::FIRST_APPLICATION_UID && proc->mPid != MY_PID);
        for (Int32 i = 0; i < mCurrentProfileIds->GetLength(); i++) {
            isBackground &= (proc->mUserId != (*mCurrentProfileIds)[i]);
        }
        if (isBackground && !showBackground) {
            Slogger::W(TAG, "Skipping crash dialog of %s: background", ToChars(proc));
            if (res != NULL) {
                res->SetResult(0);
            }
            return;
        }
        if (mShowDialogs && !mSleeping && !mShuttingDown) {
            AutoPtr<AppErrorDialog> errorDialog = new AppErrorDialog(mContext,
                    this, res, proc);
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
            AutoPtr<AppNotRespondingDialog> appDialog = new AppNotRespondingDialog(this,
                mContext, proc, record,
                arg1 != 0);
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
            AutoPtr<StrictModeViolationDialog> dialog = new StrictModeViolationDialog(mContext,
                this, res, proc);
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
    AutoPtr<FactoryErrorDialog> dialog = new FactoryErrorDialog(mContext, msg);
    AutoPtr<IDialog> d = IDialog::Probe(dialog);
    d->Show();
    EnsureBootCompleted();
}

void CActivityManagerService::HandleUpdateConfigurationMsg(
    /* [in] */ IConfiguration* config)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Boolean hasPut;
    ss->PutConfiguration(resolver, config, &hasPut);
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
            AutoPtr<AppWaitingForDebuggerDialog> appDialog = new AppWaitingForDebuggerDialog(this, mContext, app);
            AutoPtr<IDialog> d = IDialog::Probe(appDialog);
            app->mWaitDialog = d;
            app->mWaitedForDebugger = TRUE;
            d->Show();
        }
    }
    else {
        if (app->mWaitDialog != NULL) {
            app->mWaitDialog->Dismiss();
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
        AutoPtr<BaseErrorDialog> dialog = new BaseErrorDialog(mContext);
        AutoPtr<IAlertDialog> d = (IAlertDialog*)dialog->Probe(EIID_IAlertDialog);
        AutoPtr<IWindow> dWindow;
        d->GetWindow((IWindow**)&dWindow);
        dWindow->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
        d->SetCancelable(FALSE);
        AutoPtr<ICharSequence> cs;
        CString::New(title, (ICharSequence**)&cs);
        d->SetTitle(cs);
        cs = NULL;
        CString::New(text, (ICharSequence**)&cs);
        d->SetMessage(cs);
        AutoPtr<IDialogInterfaceOnClickListener> listener = new ErrorMsgButtonOnClickListener(this);
        cs = NULL;
        CString::New(String("I'm Feeling Lucky"), (ICharSequence**)&cs);
        d->SetButton(IDialogInterface::BUTTON_POSITIVE, cs, listener);
        mUidAlert = d;
        d->Show();
    }
}

void CActivityManagerService::HandleIMFeelingLuckyMsg()
{
    if (mUidAlert != NULL) {
        mUidAlert->Dismiss();
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
    AutoPtr<IINotificationManager> inm = NotificationManager::GetService();
    if (inm == NULL) {
        return;
    }

    AutoPtr<ProcessRecord> process = root->mApp;
    if (process == NULL) {
        return;
    }

    String pInfoPkgName;
    process->mInfo->GetPackageName(&pInfoPkgName);
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

    notification->SetLatestEventInfo(context, textCs, ctext, pendingIntent);

    AutoPtr<ArrayOf<Int32> > outId;
    if (FAILED(inm->EnqueueNotificationWithTag(String("android"), String("android") String(NULL),
        R::string::heavy_weight_notification,
        notification, (ArrayOf<Int32>**)&outId, root->mUserId))) {
        Slogger::W(TAG, "Error showing notification for heavy-weight app");
    }
}

void CActivityManagerService::HandleCancelHeavyNotificationMsg(
    /* [in] */ Int32 id)
{
    AutoPtr<IINotificationManager> inm = NotificationManager::GetService();
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
        ar->mInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String appInfoPkgName;
        appInfo->GetPackageName(&appInfoPkgName);
        String dialogPkgName;
        mCompatModeDialog->mAppInfo->GetPackageName(&dialogPkgName);
        if (dialogPkgName.Equals(appInfoPkgName)) {
            return;
        }
        mCompatModeDialog->Dismiss();
        mCompatModeDialog = NULL;
    }

    if (ar != NULL && FALSE) {
        AutoPtr<IApplicationInfo> appInfo;
        ar->mInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
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
    synchronized (this) {
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
        synchronized (mProcessCpuTracker) {
            Int32 N;
            mProcessCpuTracker->CountStats(&N);
            for (Int32 j = 0; j < N; j++) {
                AutoPtr<IProcessCpuTrackerStats> st;
                mProcessCpuTracker->GetStats(j, (IProcessCpuTrackerStats**)&st);
                Int32 vsize;
                st->GetVsize(&vsize);
                Int32 uid;
                if (vsize <= 0 || (st->GetUid(&uid), uid >= Process.FIRST_APPLICATION_UID)) {
                    // This is definitely an application process; skip it.
                    continue;
                }
                synchronized (mPidsSelfLockedLock) {
                    Int32 pid;
                    st->GetPid(&pid);
                    if (mPidsSelfLocked.Find(pid) != mPidsSelfLocked.End()) {
                        // This is one of our own processes; skip it.
                        continue;
                    }
                }
                Int64 pss;
                dbg->GetPss(pid, NULL, &pss);
                nativeTotalPss += pss;
            }
        }
        memInfo->ReadMemInfo();
        synchronized (this) {
            if (DEBUG_PSS) Slogger::D(TAG, "Collected native and kernel memory in %lldms",
                SystemClock::GetUptimeMillis() - start);
            Int64 cachedSizeKb, freeSizeKb, zramTotalSizeKb, buffersSizeKb, shmemSizeKb, slabSizeKb;
            memInfo->GetCachedSizeKb(&cachedSizeKb);
            memInfo->GetFreeSizeKb(&freeSizeKb);
            memInfo->GetZramTotalSizeKb(&zramTotalSizeKb);
            memInfo->GetBuffersSizeKb(&buffersSizeKb);
            memInfo->GetShmemSizeKb(&shmemSizeKb);
            memInfo->GetSlabSizeKb(&slabSizeKb);
            mProcessCpuTracker->AddSysMemUsageLocked(cachedSizeKb, freeSizeKb, zramTotalSizeKb,
                buffersSizeKb + shmemSizeKb + slabSizeKb, nativeTotalPss);
        }
    }

    Int32 i = 0, num = 0;
    AutoPtr<ArrayOf<Int64> > tmp = ArrayOf<Int64>::Alloc(1);
    List<AutoPtr<ProcessRecord> >::Iterator it = mPendingPssProcesses.Begin();
    do {
        AutoPtr<ProcessRecord> proc;
        Int32 procState;
        Int32 pid;
        synchronized (this) {
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
            Int64 pss;
            dbg->GetPss(pid, tmp, &pss);
            synchronized (this) {
                if (proc->mThread != NULL && proc->mSetProcState == procState
                    && proc->mPid == pid) {
                    num++;
                    proc->mLastPssTime = SystemClock::GetUptimeMillis();
                    proc->mBaseProcessTracker->AddPss(pss, (*tmp)[0], TRUE, proc->mPkgList);
                    if (DEBUG_PSS) Slogger::D(TAG, "PSS of %s: %lld lastPss=%lld state=%d",
                        proc->ToShortString().string(), pss, proc->mLastPss,
                        ProcessList::MakeProcStateString(procState));
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

Int32 CActivityManagerService::UserHandleGetUserId(
    /* [in] */ Int32 uid)
{
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Int32 userId;
    uhHelper->GetUserId(uid, &userId);
    return userId;
}

Int32 CActivityManagerService::UserHandleGetAppId(
    /* [in] */ Int32 uid)
{
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Int32 appId;
    uhHelper->GetAppId(uid, &appId);
    return appId;
}

Int32 CActivityManagerService::UserHandleGetCallingUserId()
{
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Int32 userId;
    uhHelper->GetCallingUserId(&userId);
    return userId;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif
