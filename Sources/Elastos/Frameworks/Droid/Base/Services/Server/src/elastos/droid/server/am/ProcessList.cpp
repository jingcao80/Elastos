
#include "elastos/droid/server/am/ProcessList.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Internal::Utility::CMemInfoReader;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 ProcessList::MIN_CRASH_INTERVAL;
const Int32 ProcessList::UNKNOWN_ADJ;
const Int32 ProcessList::CACHED_APP_MAX_ADJ;
const Int32 ProcessList::CACHED_APP_MIN_ADJ;
const Int32 ProcessList::SERVICE_B_ADJ;
const Int32 ProcessList::PREVIOUS_APP_ADJ;
const Int32 ProcessList::HOME_APP_ADJ;
const Int32 ProcessList::SERVICE_ADJ;
const Int32 ProcessList::HEAVY_WEIGHT_APP_ADJ;
const Int32 ProcessList::BACKUP_APP_ADJ;
const Int32 ProcessList::PERCEPTIBLE_APP_ADJ;
const Int32 ProcessList::VISIBLE_APP_ADJ;
const Int32 ProcessList::FOREGROUND_APP_ADJ;
const Int32 ProcessList::PERSISTENT_SERVICE_ADJ;
const Int32 ProcessList::PERSISTENT_PROC_ADJ;
const Int32 ProcessList::SYSTEM_ADJ;
const Int32 ProcessList::NATIVE_ADJ;
const Int32 ProcessList::MEMORY_PAGE_SIZE;
const Int32 ProcessList::MIN_CACHED_APPS;

static Int32 InitMAX_CACHED_APPS()
{
    Int32 value;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->GetInt32(String("ro.sys.fw.bg_apps_limit"), 32, &value);
    return value;
}
const Int32 ProcessList::MAX_CACHED_APPS = InitMAX_CACHED_APPS();
const Int64 ProcessList::MAX_EMPTY_TIME;
const Int32 ProcessList::MAX_EMPTY_APPS = ProcessList::ComputeEmptyProcessLimit(
    ProcessList::MAX_CACHED_APPS);
const Int32 ProcessList::TRIM_EMPTY_APPS = MAX_EMPTY_APPS / 2;
const Int32 ProcessList::TRIM_CACHED_APPS = (MAX_CACHED_APPS - MAX_EMPTY_APPS) / 3;
const Int32 ProcessList::TRIM_CRITICAL_THRESHOLD;
const Int32 ProcessList::TRIM_LOW_THRESHOLD;
const Byte ProcessList::LMK_TARGET;
const Byte ProcessList::LMK_PROCPRIO;
const Byte ProcessList::LMK_PROCREMOVE;
const Int32 ProcessList::PSS_MIN_TIME_FROM_STATE_CHANGE;
const Int32 ProcessList::PSS_MAX_INTERVAL;
const Int32 ProcessList::PSS_ALL_INTERVAL;
const Int32 ProcessList::PROC_MEM_PERSISTENT;
const Int32 ProcessList::PROC_MEM_TOP;
const Int32 ProcessList::PROC_MEM_IMPORTANT;
const Int32 ProcessList::PROC_MEM_SERVICE;
const Int32 ProcessList::PROC_MEM_CACHED;
const Int32 ProcessList::PSS_SHORT_INTERVAL;
const Int32 ProcessList::PSS_FIRST_TOP_INTERVAL;
const Int32 ProcessList::PSS_FIRST_BACKGROUND_INTERVAL;
const Int32 ProcessList::PSS_FIRST_CACHED_INTERVAL;
const Int32 ProcessList::PSS_SAME_IMPORTANT_INTERVAL;
const Int32 ProcessList::PSS_SAME_SERVICE_INTERVAL;
const Int32 ProcessList::PSS_SAME_CACHED_INTERVAL;

const Int32 ProcessList::sProcStateToProcMem[14] = {
    PROC_MEM_PERSISTENT,            // ActivityManager.PROCESS_STATE_PERSISTENT
    PROC_MEM_PERSISTENT,            // ActivityManager.PROCESS_STATE_PERSISTENT_UI
    PROC_MEM_TOP,                   // ActivityManager.PROCESS_STATE_TOP
    PROC_MEM_IMPORTANT,             // ActivityManager.PROCESS_STATE_IMPORTANT_FOREGROUND
    PROC_MEM_IMPORTANT,             // ActivityManager.PROCESS_STATE_IMPORTANT_BACKGROUND
    PROC_MEM_IMPORTANT,             // ActivityManager.PROCESS_STATE_BACKUP
    PROC_MEM_IMPORTANT,             // ActivityManager.PROCESS_STATE_HEAVY_WEIGHT
    PROC_MEM_SERVICE,               // ActivityManager.PROCESS_STATE_SERVICE
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_RECEIVER
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_HOME
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_LAST_ACTIVITY
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY_CLIENT
    PROC_MEM_CACHED,                // ActivityManager.PROCESS_STATE_CACHED_EMPTY
};

const Int64 ProcessList::sFirstAwakePssTimes[14] = {
    PSS_SHORT_INTERVAL,             // ActivityManager.PROCESS_STATE_PERSISTENT
    PSS_SHORT_INTERVAL,             // ActivityManager.PROCESS_STATE_PERSISTENT_UI
    PSS_FIRST_TOP_INTERVAL,         // ActivityManager.PROCESS_STATE_TOP
    PSS_FIRST_BACKGROUND_INTERVAL,  // ActivityManager.PROCESS_STATE_IMPORTANT_FOREGROUND
    PSS_FIRST_BACKGROUND_INTERVAL,  // ActivityManager.PROCESS_STATE_IMPORTANT_BACKGROUND
    PSS_FIRST_BACKGROUND_INTERVAL,  // ActivityManager.PROCESS_STATE_BACKUP
    PSS_FIRST_BACKGROUND_INTERVAL,  // ActivityManager.PROCESS_STATE_HEAVY_WEIGHT
    PSS_FIRST_BACKGROUND_INTERVAL,  // ActivityManager.PROCESS_STATE_SERVICE
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_RECEIVER
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_HOME
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_LAST_ACTIVITY
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY_CLIENT
    PSS_FIRST_CACHED_INTERVAL,      // ActivityManager.PROCESS_STATE_CACHED_EMPTY
};

const Int64 ProcessList::sSameAwakePssTimes[14] = {
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_PERSISTENT
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_PERSISTENT_UI
    PSS_SHORT_INTERVAL,             // ActivityManager.PROCESS_STATE_TOP
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_IMPORTANT_FOREGROUND
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_IMPORTANT_BACKGROUND
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_BACKUP
    PSS_SAME_IMPORTANT_INTERVAL,    // ActivityManager.PROCESS_STATE_HEAVY_WEIGHT
    PSS_SAME_SERVICE_INTERVAL,      // ActivityManager.PROCESS_STATE_SERVICE
    PSS_SAME_SERVICE_INTERVAL,      // ActivityManager.PROCESS_STATE_RECEIVER
    PSS_SAME_CACHED_INTERVAL,       // ActivityManager.PROCESS_STATE_HOME
    PSS_SAME_CACHED_INTERVAL,       // ActivityManager.PROCESS_STATE_LAST_ACTIVITY
    PSS_SAME_CACHED_INTERVAL,       // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY
    PSS_SAME_CACHED_INTERVAL,       // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY_CLIENT
    PSS_SAME_CACHED_INTERVAL,       // ActivityManager.PROCESS_STATE_CACHED_EMPTY
};

AutoPtr<ILocalSocket> ProcessList::sLmkdSocket;
AutoPtr<IOutputStream> ProcessList::sLmkdOutputStream;

ProcessList::ProcessList()
    : mCachedRestoreLevel(0)
    , mHaveDisplaySize(FALSE)
{
    Int32 oomAdj[] = {
        FOREGROUND_APP_ADJ, VISIBLE_APP_ADJ, PERCEPTIBLE_APP_ADJ,
        BACKUP_APP_ADJ, CACHED_APP_MIN_ADJ, CACHED_APP_MAX_ADJ
    };
    Int32 size = ArraySize(oomAdj);
    mOomAdj = ArrayOf<Int32>::Alloc(oomAdj, size);

    Int32 oomMinFreeLow[] = {
        12288, 18432, 24576,
        36864, 43008, 49152
    };
    size = ArraySize(oomMinFreeLow);
    mOomMinFreeLow = ArrayOf<Int32>::Alloc(oomMinFreeLow, size);

    Int32 oomMinFreeHigh[] =  {
        73728, 92160, 110592,
        129024, 147456, 184320
    };
    size = ArraySize(oomMinFreeHigh);
    mOomMinFreeHigh = ArrayOf<Int32>::Alloc(oomMinFreeHigh, size);

    mOomMinFree = ArrayOf<Int32>::Alloc(mOomAdj->GetLength());

    Int32 oomMinFreeLowRam[] =  {
        12288, 20478, 32766,
        40962, 49152, 57342
    };
    size = ArraySize(oomMinFreeLowRam);
    mOomMinFreeLowRam = ArrayOf<Int32>::Alloc(oomMinFreeLowRam, size);

    AutoPtr<IMemInfoReader> minfo;
    CMemInfoReader::New((IMemInfoReader**)&minfo);
    minfo->ReadMemInfo();
    minfo->GetTotalSize(&mTotalMemMb);
    mTotalMemMb /= (1024 * 1024);
    UpdateOomLevels(0, 0, FALSE);
}

ECode ProcessList::ApplyDisplaySize(
    /* [in] */ CWindowManagerService* wm)
{
    if (!mHaveDisplaySize) {
        AutoPtr<IPoint> p, outP;
        CPoint::New((IPoint**)&p);
        wm->GetBaseDisplaySize(IDisplay::DEFAULT_DISPLAY, p, (IPoint**)&outP);
        Int32 x, y;
        p->GetX(&x);
        p->GetY(&y);
        if (x != 0 && y != 0) {
            UpdateOomLevels(x, y, TRUE);
            mHaveDisplaySize = TRUE;
        }
    }
    return NOERROR;
}

ECode ProcessList::UpdateOomLevels(
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 displayHeight,
    /* [in] */ Boolean write)
{
    // Scale buckets from avail memory: at 300MB we use the lowest values to
    // 700MB or more for the top values.
    Float scaleMem = ((Float)(mTotalMemMb - 300)) / (700 - 300);

    // Scale buckets from screen size.
    Int32 minSize = 480 * 800;  //  384000
    Int32 maxSize = 1280 * 800; // 1024000  230400 870400  .264
    Float scaleDisp = ((Float)(displayWidth * displayHeight) - minSize) / (maxSize - minSize);
    if (FALSE) {
        Slogger::I("XXXXXX", "scaleMem=%f", scaleMem);
        Slogger::I("XXXXXX", "scaleDisp=%f dw=%d dh=%d", scaleDisp, displayWidth, displayHeight);
    }

    Float scale = scaleMem > scaleDisp ? scaleMem : scaleDisp;
    if (scale < 0)
        scale = 0;
    else if (scale > 1)
        scale = 1;
    AutoPtr<IResourcesHelper> rHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rHelper);
    AutoPtr<IResources> res;
    rHelper->GetSystem((IResources**)&res);
    Int32 minfree_adj;
    res->GetInteger(R::integer::config_lowMemoryKillerMinFreeKbytesAdjust, &minfree_adj);
    Int32 minfree_abs;
    res->GetInteger(R::integer::config_lowMemoryKillerMinFreeKbytesAbsolute, &minfree_abs);
    if (FALSE) {
        Slogger::I("XXXXXX", "minfree_adj=%d minfree_abs=%d", minfree_adj, minfree_abs);
    }

    if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0) {
        // Increase the high min-free levels for cached processes for 64-bit
        (*mOomMinFreeHigh)[4] = 225000;
        (*mOomMinFreeHigh)[5] = 325000;
    }

    for (Int32 i = 0; i < mOomAdj->GetLength(); i++) {
        AutoPtr<IActivityManagerHelper> amHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
        Boolean isLowRamDeviceStatic;
        amHelper->IsLowRamDeviceStatic(&isLowRamDeviceStatic);
        if (isLowRamDeviceStatic) {
            // Overwrite calculated LMK parameters with the low-tier tested/validated values
            (*mOomMinFree)[i] = (*mOomMinFreeLowRam)[i];
        }
        else {
            Int32 low = (*mOomMinFreeLow)[i];
            Int32 high = (*mOomMinFreeHigh)[i];
            (*mOomMinFree)[i] = (Int32)(low + ((high-low)*scale));
        }
    }

    if (minfree_abs >= 0) {
        for (Int32 i = 0; i < mOomAdj->GetLength(); i++) {
            (*mOomMinFree)[i] = (Int32)((Float)minfree_abs * (*mOomMinFree)[i]
                    / (*mOomMinFree)[mOomAdj->GetLength() - 1]);
        }
    }

    if (minfree_adj != 0) {
        for (Int32 i = 0; i  <mOomAdj->GetLength(); i++) {
            (*mOomMinFree)[i] += (Int32)((Float)minfree_adj * (*mOomMinFree)[i]
                    / (*mOomMinFree)[mOomAdj->GetLength() - 1]);
            if ((*mOomMinFree)[i] < 0) {
                (*mOomMinFree)[i] = 0;
            }
        }
    }

    // The maximum size we will restore a process from cached to background, when under
    // memory duress, is 1/3 the size we have reserved for kernel caches and other overhead
    // before killing background processes.
    mCachedRestoreLevel = (GetMemLevel(CACHED_APP_MAX_ADJ) / 1024) / 3;

    // Ask the kernel to try to keep enough memory free to allocate 3 full
    // screen 32bpp buffers without entering direct reclaim.
    Int32 reserve = displayWidth * displayHeight * 4 * 3 / 1024;
    Int32 reserve_adj;
    res->GetInteger(R::integer::config_extraFreeKbytesAdjust, &reserve_adj);
    Int32 reserve_abs;
    res->GetInteger(R::integer::config_extraFreeKbytesAbsolute, &reserve_abs);

    if (reserve_abs >= 0) {
        reserve = reserve_abs;
    }

    if (reserve_adj != 0) {
        reserve += reserve_adj;
        if (reserve < 0) {
            reserve = 0;
        }
    }

    if (write) {
        AutoPtr<IByteBufferHelper> bbHelper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
        AutoPtr<IByteBuffer> buf;
        bbHelper->Allocate(4 * (2*mOomAdj->GetLength() + 1), (IByteBuffer**)&buf);
        assert(buf != NULL);
        buf->PutInt32(LMK_TARGET);
        for (Int32 i = 0; i < mOomAdj->GetLength(); i++) {
            buf->PutInt32(((*mOomMinFree)[i] * 1024) / MEMORY_PAGE_SIZE);
            buf->PutInt32((*mOomAdj)[i]);
        }

        WriteLmkd(buf);
        AutoPtr<ISystemProperties> sysProps;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
        sysProps->Set(String("sys.sysctl.extra_free_kbytes"), StringUtils::ToString(reserve));
    }
    // GB: 2048,3072,4096,6144,7168,8192
    // HC: 8192,10240,12288,14336,16384,20480
    return NOERROR;
}

Int32 ProcessList::ComputeEmptyProcessLimit(
    /* [in] */ Int32 totalProcessLimit)
{
    return totalProcessLimit / 2;
}

String ProcessList::BuildOomTag(
    /* [in] */ const String& prefix,
    /* [in] */ const String& space,
    /* [in] */ Int32 val,
    /* [in] */ Int32 base)
{
    if (val == base) {
        if (space == NULL)
            return prefix;
        return prefix + "  ";
    }
    StringBuilder sb(prefix);
    sb += "+";
    sb += val - base;
    return sb.ToString();
}

String ProcessList::MakeOomAdjString(
    /* [in] */ Int32 setAdj)
{
    if (setAdj >= CACHED_APP_MIN_ADJ) {
        return BuildOomTag(String("cch"), String("  "), setAdj, CACHED_APP_MIN_ADJ);
    }
    else if (setAdj >= SERVICE_B_ADJ) {
        return BuildOomTag(String("svcb "), String(NULL), setAdj, SERVICE_B_ADJ);
    }
    else if (setAdj >= PREVIOUS_APP_ADJ) {
        return BuildOomTag(String("prev "), String(NULL), setAdj, PREVIOUS_APP_ADJ);
    }
    else if (setAdj >= HOME_APP_ADJ) {
        return BuildOomTag(String("home "), String(NULL), setAdj, HOME_APP_ADJ);
    }
    else if (setAdj >= SERVICE_ADJ) {
        return BuildOomTag(String("svc  "), String(NULL), setAdj, SERVICE_ADJ);
    }
    else if (setAdj >= HEAVY_WEIGHT_APP_ADJ) {
        return BuildOomTag(String("hvy  "), String(NULL), setAdj, HEAVY_WEIGHT_APP_ADJ);
    }
    else if (setAdj >= BACKUP_APP_ADJ) {
        return BuildOomTag(String("bkup "), String(NULL), setAdj, BACKUP_APP_ADJ);
    }
    else if (setAdj >= PERCEPTIBLE_APP_ADJ) {
        return BuildOomTag(String("prcp "), String(NULL), setAdj, PERCEPTIBLE_APP_ADJ);
    }
    else if (setAdj >= VISIBLE_APP_ADJ) {
        return BuildOomTag(String("vis  "), String(NULL), setAdj, VISIBLE_APP_ADJ);
    }
    else if (setAdj >= FOREGROUND_APP_ADJ) {
        return BuildOomTag(String("fore "), String(NULL), setAdj, FOREGROUND_APP_ADJ);
    }
    else if (setAdj >= PERSISTENT_SERVICE_ADJ) {
        return BuildOomTag(String("psvc "), String(NULL), setAdj, PERSISTENT_SERVICE_ADJ);
    }
    else if (setAdj >= PERSISTENT_PROC_ADJ) {
        return BuildOomTag(String("pers "), String(NULL), setAdj, PERSISTENT_PROC_ADJ);
    }
    else if (setAdj >= SYSTEM_ADJ) {
        return BuildOomTag(String("sys  "), String(NULL), setAdj, SYSTEM_ADJ);
    }
    else if (setAdj >= NATIVE_ADJ) {
        return BuildOomTag(String("ntv  "), String(NULL), setAdj, NATIVE_ADJ);
    }
    else {
        return StringUtils::ToString(setAdj);
    }
}

String ProcessList::MakeProcStateString(
    /* [in] */ Int32 curProcState)
{
    String procState;
    switch (curProcState) {
        case -1:
            procState = "N ";
            break;
        case IActivityManager::PROCESS_STATE_PERSISTENT:
            procState = "P ";
            break;
        case IActivityManager::PROCESS_STATE_PERSISTENT_UI:
            procState = "PU";
            break;
        case IActivityManager::PROCESS_STATE_TOP:
            procState = "T ";
            break;
        case IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND:
            procState = "IF";
            break;
        case IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND:
            procState = "IB";
            break;
        case IActivityManager::PROCESS_STATE_BACKUP:
            procState = "BU";
            break;
        case IActivityManager::PROCESS_STATE_HEAVY_WEIGHT:
            procState = "HW";
            break;
        case IActivityManager::PROCESS_STATE_SERVICE:
            procState = "S ";
            break;
        case IActivityManager::PROCESS_STATE_RECEIVER:
            procState = "R ";
            break;
        case IActivityManager::PROCESS_STATE_HOME:
            procState = "HO";
            break;
        case IActivityManager::PROCESS_STATE_LAST_ACTIVITY:
            procState = "LA";
            break;
        case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY:
            procState = "CA";
            break;
        case IActivityManager::PROCESS_STATE_CACHED_ACTIVITY_CLIENT:
            procState = "Ca";
            break;
        case IActivityManager::PROCESS_STATE_CACHED_EMPTY:
            procState = "CE";
            break;
        default:
            procState = "??";
            break;
    }
    return procState;
}

void ProcessList::AppendRamKb(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Int64 ramKb)
{
    for (Int32 j = 0, fact = 10; j < 6; j++, fact *= 10) {
        if (ramKb < fact) {
            sb->AppendChar(' ');
        }
    }
    sb->Append(ramKb);
}

Boolean ProcessList::ProcStatesDifferForMem(
    /* [in] */ Int32 procState1,
    /* [in] */ Int32 procState2)
{
    return sProcStateToProcMem[procState1] != sProcStateToProcMem[procState2];
}

Int64 ProcessList::ComputeNextPssTime(
    /* [in] */ Int32 procState,
    /* [in] */ Boolean first,
    /* [in] */ Boolean sleeping,
    /* [in] */ Int64 now)
{
    // Is a bug in android?
    const Int64* table = sleeping
        ? (first ? sFirstAwakePssTimes : sSameAwakePssTimes)
        : (first ? sFirstAwakePssTimes : sSameAwakePssTimes);

    // handle an invalid state scenario
    if ((procState < 0) || (procState >= 14/*table.length*/)) {
        Slogger::W(CActivityManagerService::TAG,
                "Invalid Process State within computeNextPssTime");
        return now + PSS_MIN_TIME_FROM_STATE_CHANGE;
    }

    return now + table[procState];
}

Int64 ProcessList::GetMemLevel(
    /* [in] */ Int32 adjustment)
{
    for (Int32 i = 0; i< mOomAdj->GetLength(); i++) {
        if (adjustment <= (*mOomAdj)[i]) {
            return (*mOomMinFree)[i] * 1024;
        }
    }
    return (*mOomMinFree)[mOomAdj->GetLength()-1] * 1024;
}

Int64 ProcessList::GetCachedRestoreThresholdKb()
{
    return mCachedRestoreLevel;
}

void ProcessList::SetOomAdj(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 amt)
{
    if (amt == UNKNOWN_ADJ)
        return;

    Int64 start = SystemClock::GetElapsedRealtime();
    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    AutoPtr<IByteBuffer> buf;
    bbHelper->Allocate(4 * 4, (IByteBuffer**)&buf);
    buf->PutInt32(LMK_PROCPRIO);
    buf->PutInt32(pid);
    buf->PutInt32(uid);
    buf->PutInt32(amt);
    WriteLmkd(buf);
    Int64 now = SystemClock::GetElapsedRealtime();
    if ((now - start) > 250) {
        Slogger::W("ActivityManager", "SLOW OOM ADJ: %lldms for pid %d = %d",
            (now-start), pid, amt);
    }
}

void ProcessList::Remove(
    /* [in] */ Int32 pid)
{
    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    AutoPtr<IByteBuffer> buf;
    bbHelper->Allocate(4 * 2, (IByteBuffer**)&buf);
    buf->PutInt32(LMK_PROCREMOVE);
    buf->PutInt32(pid);
    WriteLmkd(buf);
}

Boolean ProcessList::OpenLmkdSocket()
{
    sLmkdSocket = NULL;
    CLocalSocket::New(ILocalSocket::SOCKET_SEQPACKET, (ILocalSocket**)&sLmkdSocket);
    AutoPtr<ILocalSocketAddress> lsAddr;
    CLocalSocketAddress::New(String("lmkd"), LocalSocketAddressNamespace_RESERVED,
        (ILocalSocketAddress**)&lsAddr);
    if (FAILED(sLmkdSocket->Connect(lsAddr))) {
        Slogger::W(CActivityManagerService::TAG,
               "lowmemorykiller daemon socket open failed");
        sLmkdSocket = NULL;
        return FALSE;
    }
    sLmkdOutputStream = NULL;
    sLmkdSocket->GetOutputStream((IOutputStream**)&sLmkdOutputStream);

    return TRUE;
}

void ProcessList::WriteLmkd(
    /* [in] */ IByteBuffer* buf)
{
    for (Int32 i = 0; i < 3; i++) {
        if (sLmkdSocket == NULL) {
            if (OpenLmkdSocket() == FALSE) {
                Thread::Sleep(1000);
                continue;
            }
        }
        AutoPtr<ArrayOf<Byte> > array;
        buf->GetArray((ArrayOf<Byte>**)&array);
        Int32 position;
        IBuffer::Probe(buf)->GetPosition(&position);
        if (FAILED(sLmkdOutputStream->Write(array, 0, position))) {
            Slogger::W(CActivityManagerService::TAG,
                   "Error writing to lowmemorykiller socket");
            ICloseable::Probe(sLmkdSocket)->Close();
            sLmkdSocket = NULL;

        }
        return;
    }
}

String ProcessList::ToString()
{
    return String("ProcessList");
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
