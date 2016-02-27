
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManagerAppTask.h"
#include "elastos/droid/app/CActivityManagerTaskDescription.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/utility/CSize.h"
#include "elastos/droid/internal/utility/CMemInfoReader.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Internal::Utility::CMemInfoReader;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace App {

String CActivityManager::TAG("CActivityManager");
Boolean CActivityManager::localLOGV = FALSE;
Int32 CActivityManager::gMaxRecentTasks = -1;

CAR_INTERFACE_IMPL(CActivityManager, Object, IActivityManager)

CAR_OBJECT_IMPL(CActivityManager)

CActivityManager::CActivityManager()
{
}

/*package*/
ECode CActivityManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    mHandler = handler;
    return NOERROR;
}

ECode CActivityManager::GetFrontActivityScreenCompatMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = 0;

    ECode ec = NOERROR;
//     try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am) {
        ec = am->GetFrontActivityScreenCompatMode(mode);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // System dead, we will be dead too soon!
            *mode = 0;
            ec = NOERROR;
        }
    }
//     } catch (RemoteException e) {
//         // System dead, we will be dead too soon!
//         return 0;
//     }

    return ec;
}

/** @hide */
ECode CActivityManager::SetFrontActivityScreenCompatMode(
    /* [in] */ Int32 mode)
{
    // try {
    return ActivityManagerNative::GetDefault()->SetFrontActivityScreenCompatMode(mode);
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    // }
}

/** @hide */
ECode CActivityManager::GetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->GetPackageScreenCompatMode(packageName, mode);
    if (FAILED(ec)) {
        *mode = 0;
    }
    return ec;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return 0;
    // }
}

/** @hide */
ECode CActivityManager::SetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    // try {
    return ActivityManagerNative::GetDefault()->SetPackageScreenCompatMode(packageName, mode);
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    // }
}

/** @hide */
ECode CActivityManager::GetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* ask)
{
    // try {
    VALIDATE_NOT_NULL(ask)
    ECode ec = ActivityManagerNative::GetDefault()->GetPackageAskScreenCompat(packageName, ask);
    if (FAILED(ec)) {
        *ask = FALSE;
    }
    return ec;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return false;
    // }
}

/** @hide */
ECode CActivityManager::SetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean ask)
{
    // try {
    return ActivityManagerNative::GetDefault()->SetPackageAskScreenCompat(packageName, ask);
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    // }
}

ECode CActivityManager::GetMemoryClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls)
    *cls = StaticGetMemoryClass();
    return E_NOT_IMPLEMENTED;
}

Int32 CActivityManager::StaticGetMemoryClass()
{
    // Really brain dead right now -- just take this from the configured
    // vm heap size, and assume it is in megabytes and thus ends with "m".
    String vmHeapSize;
    SystemProperties::Get(String("dalvik.vm.heapgrowthlimit"), String(""), &vmHeapSize);
    if (!vmHeapSize.IsNullOrEmpty()) {
        return StringUtils::ParseInt32(vmHeapSize.Substring(0, vmHeapSize.GetLength()-1));
    }
    return StaticGetLargeMemoryClass();
}

ECode CActivityManager::GetLargeMemoryClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls)
    *cls = StaticGetLargeMemoryClass();
    return E_NOT_IMPLEMENTED;
}

Int32 CActivityManager::StaticGetLargeMemoryClass()
{
    // Really brain dead right now -- just take this from the configured
    // vm heap size, and assume it is in megabytes and thus ends with "m".
    String vmHeapSize;
    SystemProperties::Get(String("dalvik.vm.heapsize"), String("16m"), &vmHeapSize);
    return StringUtils::ParseInt32(vmHeapSize.Substring(0, vmHeapSize.GetLength() - 1));
}

ECode CActivityManager::IsLowRamDevice(
    /* [out] */ Boolean* isLow)
{
    VALIDATE_NOT_NULL(isLow)
    *isLow = IsLowRamDeviceStatic();
    return NOERROR;
}

Boolean CActivityManager::IsLowRamDeviceStatic()
{
    String value;
    SystemProperties::Get(String("ro.config.low_ram"), String("false"), &value);
    return value.Equals("true");
}

ECode CActivityManager::GetRecentTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [out] */ IList** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = NULL;

    // try {
    AutoPtr<IList> temp;
    FAIL_RETURN(ActivityManagerNative::GetDefault()->GetRecentTasks(
            maxNum, flags, UserHandle::GetMyUserId(), (IList**)&temp));
    *tasks = temp;
    REFCOUNT_ADD(*tasks);
    return NOERROR;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return NULL;
    // }
}

ECode CActivityManager::GetRecentTasksForUser(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = NULL;

    // try {
    AutoPtr<IList> temp;
    FAIL_RETURN(ActivityManagerNative::GetDefault()->GetRecentTasks(
        maxNum, flags, userId, (IList**)&temp));
    *tasks = temp;
    REFCOUNT_ADD(*tasks);
    return NOERROR;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return NULL;
    // }
}

ECode CActivityManager::GetAppTasks(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IArrayList> tasks; //ArrayList<AppTask>
    CArrayList::New((IArrayList**)&tasks);

    AutoPtr<IList> appTasks;//List<IAppTask> appTasks;
    // try {
    String pkgName;
    mContext->GetPackageName(&pkgName);
    ECode ec = ActivityManagerNative::GetDefault()->GetAppTasks(pkgName, (IList**)&appTasks);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // System dead, we will be dead too soon!
        return NOERROR;
    }
    // } catch (RemoteException e) {
        // System dead, we will be dead too soon!
        // return NULL;
    // }
    Int32 numAppTasks;
    appTasks->GetSize(&numAppTasks);
    for (Int32 i = 0; i < numAppTasks; i++) {
        AutoPtr<IInterface> obj;
        appTasks->Get(i, (IInterface**)&obj);
        IIAppTask* at = IIAppTask::Probe(obj);
        AutoPtr<IActivityManagerAppTask> ama;
        CActivityManagerAppTask::New(at, (IActivityManagerAppTask**)&ama);
        tasks->Add(ama);
    }
    *result = IList::Probe(tasks);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CActivityManager::GetAppTaskThumbnailSize(
    /* [out] */ ISize** result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    synchronized(this) {
        FAIL_RETURN(EnsureAppTaskThumbnailSizeLocked())
        Int32 x, y;
        mAppTaskThumbnailSize->GetX(&x);
        mAppTaskThumbnailSize->GetY(&y);
        AutoPtr<ISize> size;
        CSize::New(x, y, (ISize**)&size);
        *result = size;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CActivityManager::EnsureAppTaskThumbnailSizeLocked()
{
    ECode ec = NOERROR;
    if (mAppTaskThumbnailSize == NULL) {
        // try {
        ec = ActivityManagerNative::GetDefault()->GetAppTaskThumbnailSize((IPoint**)&mAppTaskThumbnailSize);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ec = E_ILLEGAL_STATE_EXCEPTION;
        }
        // } catch (RemoteException e) {
        //     throw new IllegalStateException("System dead?", e);
        // }
    }
    return ec;
}

ECode CActivityManager::AddAppTask(
    /* [in] */ IActivity* activity,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityManagerTaskDescription* description,
    /* [in] */ IBitmap* thumbnail,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    AutoPtr<IPoint> size;
    synchronized(this) {
        FAIL_RETURN(EnsureAppTaskThumbnailSizeLocked())
        size = mAppTaskThumbnailSize;
    }

    Int32 tw, th, sx, sy;
    thumbnail->GetWidth(&tw);
    thumbnail->GetHeight(&th);
    size->GetX(&sx);
    size->GetY(&sy);
    if (tw != sx || th != sy) {
        BitmapConfig bc;
        thumbnail->GetConfig(&bc);

        AutoPtr<IBitmap> bm;
        CBitmap::CreateBitmap(sx, sy, bc, (IBitmap**)&bm);

        // Use ScaleType.CENTER_CROP, except we leave the top edge at the top.
        Float scale;
        Float dx = 0, UNUSED(dy) = 0;
        if (tw * sx > sy * th) {
            scale = (Float) sx / (Float) th;
            dx = (sy - tw * scale) * 0.5f;
        }
        else {
            scale = (Float) sy / (Float) tw;
            dy = (sx - th * scale) * 0.5f;
        }

        AutoPtr<IMatrix> matrix;
        CMatrix::New((IMatrix**)&matrix);
        matrix->SetScale(scale, scale);
        Boolean bval;
        matrix->PostTranslate((Int32) (dx + 0.5f), 0, &bval);

        AutoPtr<ICanvas> canvas;
        CCanvas::New(bm, (ICanvas**)&canvas);
        canvas->DrawBitmap(thumbnail, matrix, NULL);
        canvas->SetBitmap(NULL);

        thumbnail = bm;
    }
    if (description == NULL) {
        CActivityManagerTaskDescription::New((IActivityManagerTaskDescription**)&description);
    }

    AutoPtr<IBinder> token;
    activity->GetActivityToken((IBinder**)&token);
    Int32 result;
    ECode ec = ActivityManagerNative::GetDefault()->AddAppTask(
        token, intent, description, thumbnail, &result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "E_REMOTE_EXCEPTION: System dead?");
        ec = E_ILLEGAL_STATE_EXCEPTION;
    }

    return ec;
}

ECode CActivityManager::GetRunningTasks(
    /* [in] */ Int32 maxNum,
    /* [out] */ IList** tasks)
{
    VALIDATE_NOT_NULL(tasks)
    *tasks = NULL;
    // try {
        return ActivityManagerNative::GetDefault()->GetTasks(maxNum, 0, tasks);
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return NULL;
    // }
}

ECode CActivityManager::RemoveTask(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* removed)
{
    VALIDATE_NOT_NULL(removed)
    // try {
    return ActivityManagerNative::GetDefault()->RemoveTask(taskId, flags, removed);
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return false;
    // }
}

ECode CActivityManager::GetTaskThumbnail(
    /* [in] */ Int32 id,
    /* [out] */ IActivityManagerTaskThumbnail** taskThumbnail)
{
    // try {
    VALIDATE_NOT_NULL(taskThumbnail)
    ECode ec = ActivityManagerNative::GetDefault()->GetTaskThumbnail(id, taskThumbnail);
    if (FAILED(ec)) {
        *taskThumbnail = NULL;
    }
    return ec;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return NULL;
    // }
}

/** @hide */
ECode CActivityManager::IsInHomeStack(
    /* [in] */ Int32 taskId,
    /* [out] */ Boolean* isin)
{
    VALIDATE_NOT_NULL(isin);
    *isin = FALSE;
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->IsInHomeStack(taskId, isin);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return false;
    // }
}

ECode CActivityManager::MoveTaskToFront(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags)
{
    return MoveTaskToFront(taskId, flags, NULL);
}

ECode CActivityManager::MoveTaskToFront(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    // try {
    ActivityManagerNative::GetDefault()->MoveTaskToFront(taskId, flags, options);
    return NOERROR;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    // }
}

ECode CActivityManager::GetRunningServices(
    /* [in] */ Int32 maxNum,
    /* [out] */ IList** runningServices)
{
    VALIDATE_NOT_NULL(runningServices);

    ECode ec = ActivityManagerNative::GetDefault()->GetServices(maxNum, 0, runningServices);
    if (FAILED(ec)) {
        *runningServices = NULL;
    }
    return NOERROR;
}

ECode CActivityManager::GetRunningServiceControlPanel(
    /* [in] */ IComponentName* service,
    /* [out] */ IPendingIntent** pendingIntent)
{
    // try {
    VALIDATE_NOT_NULL(pendingIntent)
    ECode ec = ActivityManagerNative::GetDefault()->GetRunningServiceControlPanel(service, pendingIntent);
    if (FAILED(ec)) {
        *pendingIntent = NULL;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     // System dead, we will be dead too soon!
    //     return NULL;
    // }
}

ECode CActivityManager::GetMemoryInfo(
    /* [out] */ IActivityManagerMemoryInfo** outInfo)
{
    VALIDATE_NOT_NULL(outInfo);

//     try {
    ECode ec = ActivityManagerNative::GetDefault()->GetMemoryInfo(outInfo);
    if (FAILED(ec)) {
        *outInfo = NULL;
    }
    return NOERROR;
//     } catch (RemoteException e) {
//     }
}

ECode CActivityManager::ClearApplicationUserData(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer,
    /* [out] */ Boolean* result)
{
    // try {
    VALIDATE_NOT_NULL(result)
    ECode ec = ActivityManagerNative::GetDefault()->ClearApplicationUserData(packageName,
            observer, UserHandle::GetMyUserId(), result);
    if (FAILED(ec)) {
        *result = FALSE;
    }
    return ec;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CActivityManager::ClearApplicationUserData(
    /* [out] */ Boolean* result )
{
    String pkgname;
    mContext->GetPackageName(&pkgname);
    return ClearApplicationUserData(pkgname, NULL, result);
}

ECode CActivityManager::GetProcessesInErrorState(
    /* [out] */ IList** records)
{
    // try {
    VALIDATE_NOT_NULL(records)
    ECode ec = ActivityManagerNative::GetDefault()->GetProcessesInErrorState(records);
    if (FAILED(ec)) {
        *records = NULL;
    }
    return ec;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CActivityManager::GetRunningExternalApplications(
    /* [out] */ IList** records)
{
    // try {
    VALIDATE_NOT_NULL(records)
    ECode ec = ActivityManagerNative::GetDefault()->GetRunningExternalApplications(records);
    if (FAILED(ec)) {
        *records = NULL;
    }
    return ec;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CActivityManager::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config);
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->GetConfiguration(config);
    if (FAILED(ec)) *config = NULL;
    return ec;
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CActivityManager::GetRunningAppProcesses(
    /* [out] */ IList** records)
{
    // try {
    VALIDATE_NOT_NULL(records)
    ECode ec = ActivityManagerNative::GetDefault()->GetRunningAppProcesses(records);
    if (FAILED(ec)) {
        *records = NULL;
    }
    return ec;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CActivityManager::GetProcessMemoryInfo(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out] */ ArrayOf<IDebugMemoryInfo*>** memoryInfos)
{
    // try {
    VALIDATE_NOT_NULL(memoryInfos)
    ECode ec = ActivityManagerNative::GetDefault()->GetProcessMemoryInfo(pids, memoryInfos);
    if (FAILED(ec)) {
        *memoryInfos = NULL;
    }
    return ec;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CActivityManager::RestartPackage(
    /* [in] */ const String& packageName)
{
    return KillBackgroundProcesses(packageName);
}

ECode CActivityManager::KillBackgroundProcesses(
    /* [in] */ const String& packageName)
{
    // try {
    return ActivityManagerNative::GetDefault()->KillBackgroundProcesses(packageName, UserHandle::GetMyUserId());
    // } catch (RemoteException e) {
    // }
}

ECode CActivityManager::ForceStopPackage(
    /* [in] */ const String& packageName)
{
    return ForceStopPackageAsUser(packageName, UserHandle::GetMyUserId());
}

ECode CActivityManager::ForceStopPackageAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    // try {
    return ActivityManagerNative::GetDefault()->ForceStopPackage(packageName, userId);
    // } catch (RemoteException e) {
    // }
}

ECode CActivityManager::GetDeviceConfigurationInfo(
    /* [out] */ IConfigurationInfo** configurationInfo)
{
    // try {
    VALIDATE_NOT_NULL(configurationInfo)
    return ActivityManagerNative::GetDefault()->GetDeviceConfigurationInfo(configurationInfo);
    // } catch (RemoteException e) {
    // }
    // return NULL;
}

ECode CActivityManager::GetLauncherLargeIconDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    Int32 den = 0;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensityDpi(&den);
    AutoPtr<IConfiguration> conf;
    Int32 sw;
    res->GetConfiguration((IConfiguration**)&conf);
    conf->GetSmallestScreenWidthDp(&sw);

    if (sw < 600) {
        // Smaller than approx 7" tablets, use the regular icon size.
        *density = den;
        return NOERROR;
    }

    switch (den) {
        case IDisplayMetrics::DENSITY_LOW:
            *density = IDisplayMetrics::DENSITY_MEDIUM;
        case IDisplayMetrics::DENSITY_MEDIUM:
            *density = IDisplayMetrics::DENSITY_HIGH;
        case IDisplayMetrics::DENSITY_TV:
            *density = IDisplayMetrics::DENSITY_XHIGH;
        case IDisplayMetrics::DENSITY_HIGH:
            *density = IDisplayMetrics::DENSITY_XHIGH;
        case IDisplayMetrics::DENSITY_XHIGH:
            *density = IDisplayMetrics::DENSITY_XXHIGH;
        case IDisplayMetrics::DENSITY_XXHIGH:
            *density = IDisplayMetrics::DENSITY_XHIGH * 2;
        default:
            // The density is some abnormal value.  Return some other
            // abnormal value that is a reasonable scaling of it.
            *density = (Int32)((den * 1.5) + 0.5);
    }
    return NOERROR;
}

ECode CActivityManager::GetLauncherLargeIconSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = GetLauncherLargeIconSizeInner(mContext);
    return NOERROR;
}

Int32 CActivityManager::GetLauncherLargeIconSizeInner(
        /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 pSize;
    res->GetDimensionPixelSize(R::dimen::app_icon_size, &pSize);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 sw;
    config->GetSmallestScreenWidthDp(&sw);

    if (sw < 600) {
        // Smaller than approx 7" tablets, use the regular icon size.
        return pSize;
    }

    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 density;
    dm->GetDensityDpi(&density);

    switch (density) {
        case IDisplayMetrics::DENSITY_LOW:
            return (pSize * IDisplayMetrics::DENSITY_MEDIUM) / IDisplayMetrics::DENSITY_LOW;
        case IDisplayMetrics::DENSITY_MEDIUM:
            return (pSize * IDisplayMetrics::DENSITY_HIGH) / IDisplayMetrics::DENSITY_MEDIUM;
        case IDisplayMetrics::DENSITY_TV:
            return (pSize * IDisplayMetrics::DENSITY_XHIGH) / IDisplayMetrics::DENSITY_HIGH;
        case IDisplayMetrics::DENSITY_HIGH:
            return (pSize * IDisplayMetrics::DENSITY_XHIGH) / IDisplayMetrics::DENSITY_HIGH;
        case IDisplayMetrics::DENSITY_XHIGH:
            return (pSize * IDisplayMetrics::DENSITY_XXHIGH) / IDisplayMetrics::DENSITY_XHIGH;
        case IDisplayMetrics::DENSITY_XXHIGH:
            return (pSize * IDisplayMetrics::DENSITY_XHIGH*2) / IDisplayMetrics::DENSITY_XXHIGH;
        default:
            // The density is some abnormal value.  Return some other
            // abnormal value that is a reasonable scaling of it.
            return (Int32)((pSize*1.5f) + .5f);
    }
    return 0;
}

/**
 * Returns the launch count of each installed package.
 *
 * @hide
 */
/*public Map<String, Integer> getAllPackageLaunchCounts() {
    try {
        IUsageStats usageStatsService = IUsageStats.Stub.asInterface(
                ServiceManager.getService("usagestats"));
        if (usageStatsService == NULL) {
            return new HashMap<String, Integer>();
        }

        UsageStats.PackageStats[] allPkgUsageStats = usageStatsService.getAllPkgUsageStats(
                ActivityThread.currentPackageName());
        if (allPkgUsageStats == NULL) {
            return new HashMap<String, Integer>();
        }

        Map<String, Integer> launchCounts = new HashMap<String, Integer>();
        for (UsageStats.PackageStats pkgUsageStats : allPkgUsageStats) {
            launchCounts.put(pkgUsageStats.getPackageName(), pkgUsageStats.getLaunchCount());
        }

        return launchCounts;
    } catch (RemoteException e) {
        Log.w(TAG, "Could not query launch counts", e);
        return new HashMap<String, Integer>();
    }
}*/

ECode CActivityManager::SwitchUser(
    /* [in] */ Int32 userid,
    /* [out] */ Boolean* switched)
{
    VALIDATE_NOT_NULL(switched);
    *switched = FALSE;
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am) {
        am->SwitchUser(userid, switched);
    }

    return NOERROR;
}

ECode CActivityManager::IsUserRunning(
    /* [in] */ Int32 userid,
    /* [out] */ Boolean* isRunning)
{
    VALIDATE_NOT_NULL(isRunning);
    *isRunning = FALSE;
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am) {
        am->IsUserRunning(userid, FALSE, isRunning);
    }

    return NOERROR;
}

ECode CActivityManager::StartLockTaskMode(
    /* [in] */ Int32 taskId)
{
    // try {
    return ActivityManagerNative::GetDefault()->StartLockTaskMode(taskId);
    // } catch (RemoteException e) {
    // }
}

ECode CActivityManager::StopLockTaskMode()
{
    // try {
    return ActivityManagerNative::GetDefault()->StopLockTaskMode();
    // } catch (RemoteException e) {
    // }
}

ECode CActivityManager::IsInLockTaskMode(
    /* [out] */ Boolean* mode)
{
    VALIDATE_NOT_NULL(mode)
    // try {
    return ActivityManagerNative::GetDefault()->IsInLockTaskMode(mode);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CActivityManager::UpdateConfiguration(
    /* [in] */ IConfiguration* values)
{
    // try {
    return ActivityManagerNative::GetDefault()->UpdateConfiguration(values);
    // } catch (RemoteException e) {
    // }
}

Int32 CActivityManager::GetMemoryClass()
{
    // Really brain dead right now -- just take this from the configured
    // vm heap size, and assume it is in megabytes and thus ends with "m".
    String vmHeapSize;
    SystemProperties::Get(String("dalvik.vm.heapgrowthlimit"), String(""), &vmHeapSize);
    if (!vmHeapSize.IsNullOrEmpty()) {
        return StringUtils::ParseInt32(vmHeapSize.Substring(0, vmHeapSize.GetLength() - 1));
    }
    return StaticGetLargeMemoryClass();
}

Int32 CActivityManager::GetLargeMemoryClass()
{
    // Really brain dead right now -- just take this from the configured
    // vm heap size, and assume it is in megabytes and thus ends with "m".
    String vmHeapSize;
    SystemProperties::Get(String("dalvik.vm.heapsize"), String("16m"), &vmHeapSize);
    return StringUtils::ParseInt32(vmHeapSize.Substring(0, vmHeapSize.GetLength()-1));
}

Boolean CActivityManager::IsHighEndGfx()
{
    AutoPtr<IResources> resources = CResources::GetSystem();
    Boolean bval;
    resources->GetBoolean(R::bool_::config_avoidGfxAccel, &bval);
    return !IsLowRamDeviceStatic() && bval;
}

Boolean CActivityManager::IsForcedHighEndGfx()
{
    Boolean value;
    SystemProperties::GetBoolean(String("persist.sys.force_highendgfx"), FALSE, &value);
    return value;
}

Int32 CActivityManager::GetMaxRecentTasksStatic()
{
    if (gMaxRecentTasks < 0) {
        return gMaxRecentTasks = IsLowRamDeviceStatic() ? 50 : 100;
    }
    return gMaxRecentTasks;
}

Int32 CActivityManager::GetDefaultAppRecentsLimitStatic()
{
    return GetMaxRecentTasksStatic() / 6;
}

Int32 CActivityManager::GetMaxAppRecentsLimitStatic()
{
    return GetMaxRecentTasksStatic() / 2;
}

Boolean CActivityManager::IsLargeRAM()
{
    AutoPtr<IMemInfoReader> reader;
    CMemInfoReader::New((IMemInfoReader**)&reader);
    reader->ReadMemInfo();
    Int64 size;
    reader->GetTotalSize(&size);
    if (size >= (640*1024*1024)) {
        // Currently 640MB RAM available to the kernel is the point at
        // which we have plenty of RAM to spare.
        return TRUE;
    }
    return FALSE;
}

ECode CActivityManager::GetMyMemoryState(
    /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo)
{
    return ActivityManagerNative::GetDefault()->GetMyMemoryState(outInfo);
}

Boolean CActivityManager::IsUserAMonkey()
{
    // try {
    Boolean result;
    if (FAILED(ActivityManagerNative::GetDefault()->IsUserAMonkey(&result))) {
        return FALSE;
    }
    return result;
    // } catch (RemoteException e) {
    // }
    // return false;
}

Boolean CActivityManager::IsRunningInTestHarness()
{
    Boolean result;
    SystemProperties::GetBoolean(String("ro.test_harness"), FALSE, &result);
    return result;
}

Int32 CActivityManager::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported)
{
    // Root, system server get to do everything.
    if (uid == 0 || uid == IProcess::SYSTEM_UID) {
        return IPackageManager::PERMISSION_GRANTED;
    }
    // Isolated processes don't get any permissions.
    if (UserHandle::IsIsolated(uid)) {
        return IPackageManager::PERMISSION_DENIED;
    }
    // If there is a uid that owns whatever is being accessed, it has
    // blanket access to it regardless of the permissions it requires.
    if (owningUid >= 0 && UserHandle::IsSameApp(uid, owningUid)) {
        return IPackageManager::PERMISSION_GRANTED;
    }
    // If the target is not exported, then nobody else can get to it.
    if (!exported) {
        return IPackageManager::PERMISSION_DENIED;
    }
    if (permission.IsNull()) {
        return IPackageManager::PERMISSION_GRANTED;
    }

    Int32 result;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    if (FAILED(pm->CheckUidPermission(permission, uid, &result))) {
        // Should never happen, but if it does... deny!
//        Slogger::E(TAG, "PackageManager is dead?!?");
        return IPackageManager::PERMISSION_DENIED;
    }

    return result;
}

Int32 CActivityManager::CheckUidPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 uid)
{
    Int32 result;
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    if (FAILED(pm->CheckUidPermission(permission, uid, &result))) {
        return IPackageManager::PERMISSION_DENIED;
    }

    return result;
}

ECode CActivityManager::HandleIncomingUser(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean allowAll,
    /* [in] */ Boolean requireFull,
    /* [in] */ const String& name,
    /* [in] */ const String& callerPackage,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 uid = 0;
    helper->GetUserId(callingUid, &uid);
    if (uid == userId) {
        *value = userId;
        return NOERROR;
    }

    //     try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am) {
        ECode ec = am->HandleIncomingUser(
            callingPid, callingUid, userId, allowAll, requireFull, name, callerPackage, value);
        if (FAILED(ec)) {
            *value = 0;
            return E_SECURITY_EXCEPTION;
        }
    }
    return NOERROR;
}

Int32 CActivityManager::GetCurrentUser()
{
    Int32 uid = 0;
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am) {
        AutoPtr<IUserInfo> ui;
        am->GetCurrentUser((IUserInfo**)&ui);
        if (ui != NULL) {
            ui->GetId(&uid);
        }
    }

    return uid;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
