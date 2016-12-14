
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/os/CBatterySipper.h"
#include "elastos/droid/internal/os/CBatteryStatsHelper.h"
#include "elastos/droid/internal/os/CPowerProfile.h"
#include "elastos/droid/internal/os/CBatteryStatsImpl.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CMemoryFile.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IBatteryStatsTimer;
using Elastos::Droid::Os::IBatteryStatsUidProc;
using Elastos::Droid::Os::IBatteryStatsUidSensor;
using Elastos::Droid::Os::IBatteryStatsUidWakelock;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::CMemoryFile;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::IDouble;
using Elastos::IO::CFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

//==============================================================================
// CBatteryStatsHelper::BatterySipperComparator
//==============================================================================

CAR_INTERFACE_IMPL(CBatteryStatsHelper::BatterySipperComparator, Object, IComparator)

ECode CBatteryStatsHelper::BatterySipperComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    AutoPtr<CBatterySipper> lhs = (CBatterySipper*)IBatterySipper::Probe(_lhs);
    AutoPtr<CBatterySipper> rhs = (CBatterySipper*)IBatterySipper::Probe(_rhs);
    if (lhs->mMobilemspp < rhs->mMobilemspp) {
        *result = 1;
    }
    else if (lhs->mMobilemspp > rhs->mMobilemspp) {
        *result = -1;
    }
    return NOERROR;
}


//==============================================================================
// CBatteryStatsHelper
//==============================================================================

const Boolean CBatteryStatsHelper::DEBUG;
const String CBatteryStatsHelper::TAG("CBatteryStatsHelper");
AutoPtr<IBatteryStats> CBatteryStatsHelper::sStatsXfer;
AutoPtr<IIntent> CBatteryStatsHelper::sBatteryBroadcastXfer;

static AutoPtr<IArrayMap> InitFileXfer()
{
    AutoPtr<IArrayMap> arrayMap;
    CArrayMap::New((IArrayMap**)&arrayMap);
    return arrayMap;
}
AutoPtr<IArrayMap> CBatteryStatsHelper::sFileXfer = InitFileXfer();

CAR_INTERFACE_IMPL(CBatteryStatsHelper, Object, IBatteryStatsHelper)

CAR_OBJECT_IMPL(CBatteryStatsHelper)

CBatteryStatsHelper::CBatteryStatsHelper()
    : mRawRealtime(0)
    , mRawUptime(0)
    , mBatteryRealtime(0)
    , mBatteryUptime(0)
    , mTypeBatteryRealtime(0)
    , mTypeBatteryUptime(0)
    , mBatteryTimeRemaining(0)
    , mChargeTimeRemaining(0)
    , mCollectBatteryBroadcast(FALSE)
    , mWifiOnly(FALSE)
    , mStatsType(IBatteryStats::STATS_SINCE_CHARGED)
    , mStatsPeriod(0)
    , mMaxPower(1)
    , mMaxRealPower(1)
    , mComputedPower(0)
    , mTotalPower(0)
    , mWifiPower(0)
    , mBluetoothPower(0)
    , mMinDrainedPower(0)
    , mMaxDrainedPower(0)
    , mAppMobileActive(0)
    , mAppWifiRunning(0)
{}

ECode CBatteryStatsHelper::constructor(
    /* [in] */ IContext* context)
{
    CArrayList::New((IList**)&mUsageList);
    CArrayList::New((IList**)&mWifiSippers);
    CArrayList::New((IList**)&mBluetoothSippers);
    CSparseArray::New((ISparseArray**)&mUserSippers);
    CSparseArray::New((ISparseArray**)&mUserPower);
    CArrayList::New((IList**)&mMobilemsppList);

    return constructor(context, TRUE);
}

ECode CBatteryStatsHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean collectBatteryBroadcast)
{
    CArrayList::New((IList**)&mUsageList);
    CArrayList::New((IList**)&mWifiSippers);
    CArrayList::New((IList**)&mBluetoothSippers);
    CSparseArray::New((ISparseArray**)&mUserSippers);
    CSparseArray::New((ISparseArray**)&mUserPower);
    CArrayList::New((IList**)&mMobilemsppList);

    mContext = context;
    mCollectBatteryBroadcast = collectBatteryBroadcast;
    mWifiOnly = CheckWifiOnly(context);
    return NOERROR;
}

ECode CBatteryStatsHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean collectBatteryBroadcast,
    /* [in] */ Boolean wifiOnly)
{
    CArrayList::New((IList**)&mUsageList);
    CArrayList::New((IList**)&mWifiSippers);
    CArrayList::New((IList**)&mBluetoothSippers);
    CSparseArray::New((ISparseArray**)&mUserSippers);
    CSparseArray::New((ISparseArray**)&mUserPower);
    CArrayList::New((IList**)&mMobilemsppList);

    mContext = context;
    mCollectBatteryBroadcast = collectBatteryBroadcast;
    mWifiOnly = wifiOnly;
    return NOERROR;
}

Boolean CBatteryStatsHelper::CheckWifiOnly(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(service);
    Boolean res;
    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &res);
    return !res;
}

ECode CBatteryStatsHelper::StoreStatsHistoryInFile(
    /* [in] */ const String& fname)
{
    AutoLock lock(sFileXfer);
    AutoPtr<IFile> path = MakeFilePath(mContext, fname);
    AutoPtr<IBatteryStats> stats;
    GetStats((IBatteryStats**)&stats);
    IMap::Probe(sFileXfer)->Put(path, stats);
    AutoPtr<IFileOutputStream> fout;
    // try {
    CFileOutputStream::New(path, (IFileOutputStream**)&fout);
    AutoPtr<IParcel> hist;
    CParcel::New((IParcel**)&hist);// CParcel.obtain();
    stats->WriteToParcelWithoutUids(hist);
    AutoPtr<ArrayOf<Byte> > histData;
    hist->Marshall((ArrayOf<Byte>**)&histData);
    IOutputStream::Probe(fout)->Write(histData);
    // } catch (IOException e) {
    //     Log.w(TAG, "Unable to write history to file", e);
    // } finally {
    //     if (fout != null) {
    //         try {
    //             fout.close();
    //         } catch (IOException e) {
    //         }
    //     }
    // }
    if (fout != NULL) {
        ICloseable::Probe(fout)->Close();
    }
    return NOERROR;
}

AutoPtr<IBatteryStats> CBatteryStatsHelper::StatsFromFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname)
{
    {    AutoLock syncLock(sFileXfer);
        AutoPtr<IFile> path = MakeFilePath(context, fname);
        AutoPtr<IInterface> value;
        IMap::Probe(sFileXfer)->Get(path, (IInterface**)&value);
        AutoPtr<IBatteryStats> stats = IBatteryStats::Probe(value);
        if (stats != NULL) {
            return stats;
        }
        AutoPtr<IFileInputStream> fin;
        // try {
        CFileInputStream::New(path, (IFileInputStream**)&fin);
        AutoPtr<ArrayOf<Byte> > data = ReadFully(fin);
        AutoPtr<IParcel> parcel;
        CParcel::New((IParcel**)&parcel);// Parcel.obtain();
        parcel->Unmarshall(data, 0, data->GetLength());
        parcel->SetDataPosition(0);
        CBatteryStatsImpl::New((IBatteryStats**)&stats);
        IParcelable::Probe(stats)->ReadFromParcel(parcel);
        if (fin != NULL) {
            ICloseable::Probe(fin)->Close();
        }
        return stats;
        // } catch (IOException e) {
        //     Log.w(TAG, "Unable to read history to file", e);
        // } finally {
        //     if (fin != null) {
        //         try {
        //             fin.close();
        //         } catch (IOException e) {
        //         }
        //     }
        // }
    }
    AutoPtr<IInterface> service = ServiceManager::GetService(IBatteryStats::SERVICE_NAME);
    return IBatteryStats::Probe(GetStats(IIBatteryStats::Probe(service)));
}

void CBatteryStatsHelper::DropFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname)
{
    AutoPtr<IFile> file = MakeFilePath(context, fname);
    file->Delete();
}

AutoPtr<IFile> CBatteryStatsHelper::MakeFilePath(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname)
{
    AutoPtr<IFile> dir;
    context->GetFilesDir((IFile**)&dir);
    AutoPtr<IFile> file;
    CFile::New(dir, fname, (IFile**)&file);
    return file;
}

ECode CBatteryStatsHelper::ClearStats()
{
    mStats = NULL;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetStats(
    /* [out] */ IBatteryStats** stats)
{
    VALIDATE_NOT_NULL(stats);

    if (mStats == NULL) {
        Load();
    }
    *stats = mStats;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode CBatteryStatsHelper::GetBatteryBroadcast(
    /* [out] */ IIntent** batteryBroadcast)
{
    VALIDATE_NOT_NULL(batteryBroadcast)
    if (mBatteryBroadcast == NULL && mCollectBatteryBroadcast) {
        Load();
    }
    *batteryBroadcast = mBatteryBroadcast;
    REFCOUNT_ADD(*batteryBroadcast)
    return NOERROR;
}

ECode CBatteryStatsHelper::GetPowerProfile(
    /* [out] */ IPowerProfile** powerProfile)
{
    VALIDATE_NOT_NULL(powerProfile)
    *powerProfile = mPowerProfile;
    REFCOUNT_ADD(*powerProfile)
    return NOERROR;
}

ECode CBatteryStatsHelper::Create(
    /* [in] */ IBatteryStats* stats)
{
    CPowerProfile::New(mContext, (IPowerProfile**)&mPowerProfile);
    mStats = stats;
    return NOERROR;
}

ECode CBatteryStatsHelper::Create(
    /* [in] */ IBundle* icicle)
{
    if (icicle != NULL) {
        mStats = sStatsXfer;
        mBatteryBroadcast = sBatteryBroadcastXfer;
    }
    AutoPtr<IInterface> service = ServiceManager::GetService(IBatteryStats::SERVICE_NAME);
    mBatteryInfo = IIBatteryStats::Probe(service);
    CPowerProfile::New(mContext, (IPowerProfile**)&mPowerProfile);
    return NOERROR;
}

ECode CBatteryStatsHelper::StoreState()
{
    sStatsXfer = mStats;
    sBatteryBroadcastXfer = mBatteryBroadcast;
    return NOERROR;
}

String CBatteryStatsHelper::MakemAh(
    /* [in] */ Double power)
{
    String str;
    if (power < .00001)
        str.AppendFormat("%.8f", power);
    else if (power < .0001)
        str.AppendFormat("%.7f", power);
    else if (power < .001)
        str.AppendFormat("%.6f", power);
    else if (power < .01)
        str.AppendFormat("%.5f", power);
    else if (power < .1)
        str.AppendFormat("%.4f", power);
    else if (power < 1)
        str.AppendFormat("%.3f", power);
    else if (power < 10)
        str.AppendFormat("%.2f", power);
    else if (power < 100)
        str.AppendFormat("%.1f", power);
    else
        str.AppendFormat("%.0f", power);

    return str;
}

ECode CBatteryStatsHelper::RefreshStats(
    /* [in] */ Int32 statsType,
    /* [in] */ Int32 asUser)
{
    AutoPtr<ISparseArray> users;
    CSparseArray::New(1, (ISparseArray**)&users);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(asUser, (IUserHandle**)&user);
    users->Put(asUser, user);
    return RefreshStats(statsType, users);
}

ECode CBatteryStatsHelper::RefreshStats(
    /* [in] */ Int32 statsType,
    /* [in] */ IList* asUsers)
{
    Int32 n;
    asUsers->GetSize(&n);
    AutoPtr<ISparseArray> users;
    CSparseArray::New(n, (ISparseArray**)&users);
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> item;
        asUsers->Get(i, (IInterface**)&item);
        AutoPtr<IUserHandle> userHandle = IUserHandle::Probe(item);
        Int32 identifier;
        userHandle->GetIdentifier(&identifier);
        users->Put(identifier, item);
    }
    return RefreshStats(statsType, users);
}

ECode CBatteryStatsHelper::RefreshStats(
    /* [in] */ Int32 statsType,
    /* [in] */ ISparseArray* asUsers)
{
    return RefreshStats(statsType, asUsers, SystemClock::GetElapsedRealtime() * 1000,
            SystemClock::GetUptimeMillis() * 1000);
}

ECode CBatteryStatsHelper::RefreshStats(
    /* [in] */ Int32 statsType,
    /* [in] */ ISparseArray* asUsers,
    /* [in] */ Int64 rawRealtimeUs,
    /* [in] */ Int64 rawUptimeUs)
{
    // Initialize mStats if necessary.
    AutoPtr<IBatteryStats> stats;
    GetStats((IBatteryStats**)&stats);

    mMaxPower = 0;
    mMaxRealPower = 0;
    mComputedPower = 0;
    mTotalPower = 0;
    mWifiPower = 0;
    mBluetoothPower = 0;
    mAppMobileActive = 0;
    mAppWifiRunning = 0;

    mUsageList->Clear();
    mWifiSippers->Clear();
    mBluetoothSippers->Clear();
    mUserSippers->Clear();
    mUserPower->Clear();
    mMobilemsppList->Clear();

    if (mStats == NULL) {
        return NOERROR;
    }

    mStatsType = statsType;
    mRawUptime = rawUptimeUs;
    mRawRealtime = rawRealtimeUs;
    mStats->GetBatteryUptime(rawUptimeUs, &mBatteryUptime);
    mStats->GetBatteryRealtime(rawRealtimeUs, &mBatteryRealtime);
    mStats->ComputeBatteryUptime(rawUptimeUs, mStatsType, &mTypeBatteryUptime);
    mStats->ComputeBatteryRealtime(rawRealtimeUs, mStatsType, &mTypeBatteryRealtime);
    mStats->ComputeBatteryTimeRemaining(rawRealtimeUs, &mBatteryTimeRemaining);
    mStats->ComputeChargeTimeRemaining(rawRealtimeUs, &mChargeTimeRemaining);

    if (DEBUG) {
        Logger::D(TAG, "Raw time: realtime=%lld uptime=%lld",
            (rawRealtimeUs/1000), (rawUptimeUs/1000));
        Logger::D(TAG, "Battery time: realtime=%lld uptime=%lld",
            (mBatteryRealtime/1000), (mBatteryUptime/1000));
        Logger::D(TAG, "Battery type time: realtime=%lld uptime=%lld",
            (mTypeBatteryRealtime/1000), (mTypeBatteryUptime/1000));
    }
    Int32 low, high;
    mStats->GetLowDischargeAmountSinceCharge(&low);
    mStats->GetHighDischargeAmountSinceCharge(&high);
    Double capacity;
    mPowerProfile->GetBatteryCapacity(&capacity);
    mMinDrainedPower = (low * capacity) / 100;
    mMaxDrainedPower = (high * capacity) / 100;

    ProcessAppUsage(asUsers);

    // Before aggregating apps in to users, collect all apps to sort by their ms per packet.
    Int32 size = 0;
    mUsageList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mUsageList->Get(i, (IInterface**)&item);
        IBatterySipper* bs = IBatterySipper::Probe(item);
        bs->ComputeMobilemspp();
        Double mobilemspp;
        bs->GetMobilemspp(&mobilemspp);
        if (mobilemspp != 0) {
            mMobilemsppList->Add(bs);
        }
    }

    mUserSippers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mUserSippers->ValueAt(i, (IInterface**)&item);
        IList* user = IList::Probe(item);
        Int32 count;
        user->GetSize(&count);
        for (Int32 j = 0; j < count; j++) {
            AutoPtr<IInterface> obj;
            user->Get(j, (IInterface**)&obj);
            IBatterySipper* bs = IBatterySipper::Probe(obj);

            bs->ComputeMobilemspp();
            Double mobilemspp;
            bs->GetMobilemspp(&mobilemspp);
            if (mobilemspp != 0) {
                mMobilemsppList->Add(bs);
            }
        }
    }

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    AutoPtr<IComparator> comparator = new BatterySipperComparator();
    collections->Sort(mMobilemsppList, comparator);

    ProcessMiscUsage();

    if (DEBUG) {
        Logger::D(TAG, "Accuracy: total computed=%s, min discharge=%s, max discharge=%s",
            MakemAh(mComputedPower).string(), MakemAh(mMinDrainedPower).string(),
            MakemAh(mMaxDrainedPower).string());
    }
    mTotalPower = mComputedPower;
    mStats->GetLowDischargeAmountSinceCharge(&low);
    if (low > 1) {
        if (mMinDrainedPower > mComputedPower) {
            Double amount = mMinDrainedPower - mComputedPower;
            mTotalPower = mMinDrainedPower;
            AddEntryNoTotal(BatterySipperDrainType_UNACCOUNTED, 0, amount);
        }
        else if (mMaxDrainedPower < mComputedPower) {
            Double amount = mComputedPower - mMaxDrainedPower;
            AddEntryNoTotal(BatterySipperDrainType_OVERCOUNTED, 0, amount);
        }
    }

    return collections->Sort(mUsageList);
}

void CBatteryStatsHelper::ProcessAppUsage(
    /* [in] */ ISparseArray* asUsers)
{
    AutoPtr<IInterface> userAll;
    asUsers->Get(IUserHandle::USER_ALL, (IInterface**)&userAll);
    Boolean forAllUsers = (userAll != NULL);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&service);
    AutoPtr<ISensorManager> sensorManager = ISensorManager::Probe(service);
    Int32 which = mStatsType;
    Int32 speedSteps;
    mPowerProfile->GetNumSpeedSteps(&speedSteps);
    AutoPtr<ArrayOf<Double> > powerCpuNormal = ArrayOf<Double>::Alloc(speedSteps);
    AutoPtr<ArrayOf<Int64> > cpuSpeedStepTimes = ArrayOf<Int64>::Alloc(speedSteps);
    for (Int32 p = 0; p < speedSteps; p++) {
        mPowerProfile->GetAveragePower(IPowerProfile::POWER_CPU_ACTIVE, p, &(*powerCpuNormal)[p]);
    }
    Double mobilePowerPerPacket = GetMobilePowerPerPacket();
    Double mobilePowerPerMs = GetMobilePowerPerMs();
    Double wifiPowerPerPacket = GetWifiPowerPerPacket();
    Int64 appWakelockTimeUs = 0;
    AutoPtr<CBatterySipper> osApp;
    mStatsPeriod = mTypeBatteryRealtime;
    AutoPtr<ISparseArray> uidStats;
    mStats->GetUidStats((ISparseArray**)&uidStats);
    Int32 NU;
    uidStats->GetSize(&NU);
    for (Int32 iu = 0; iu < NU; iu++) {
        AutoPtr<IInterface> item;
        uidStats->ValueAt(iu, (IInterface**)&item);
        AutoPtr<IBatteryStatsUid> u = IBatteryStatsUid::Probe(item);
        Double p; // in mAs
        Double power = 0; // in mAs
        Double highestDrain = 0;
        String packageWithHighestDrain;
        AutoPtr<IMap> processStats;
        u->GetProcessStats((IMap**)&processStats);
        Int64 cpuTime = 0;
        Int64 cpuFgTime = 0;
        Int64 wakelockTime = 0;
        Int64 gpsTime = 0;
        Int32 size;
        processStats->GetSize(&size);
        if (size > 0) {
            // Process CPU time
            AutoPtr<ISet> entrySet;
            processStats->GetEntrySet((ISet**)&entrySet);
            AutoPtr<IIterator> it;
            entrySet->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* entry = IMapEntry::Probe(obj);
                AutoPtr<IInterface> key, value;
                entry->GetKey((IInterface**)&key);
                entry->GetValue((IInterface**)&value);
                String strKey;
                IObject::Probe(key)->ToString(&strKey);

                AutoPtr<IBatteryStatsUidProc> ps = IBatteryStatsUidProc::Probe(value);
                Int64 userTime;
                ps->GetUserTime(which, &userTime);
                Int64 systemTime;
                ps->GetSystemTime(which, &systemTime);
                Int64 foregroundTime;
                ps->GetForegroundTime(which, &foregroundTime);
                cpuFgTime += foregroundTime * 10; // convert to millis
                Int64 tmpCpuTime = (userTime + systemTime) * 10; // convert to millis
                Int32 totalTimeAtSpeeds = 0;
                // Get the total first
                for (Int32 step = 0; step < speedSteps; step++) {
                    ps->GetTimeAtCpuSpeedStep(step, which, &(*cpuSpeedStepTimes)[step]);
                    totalTimeAtSpeeds += (*cpuSpeedStepTimes)[step];
                }
                if (totalTimeAtSpeeds == 0)
                    totalTimeAtSpeeds = 1;
                // Then compute the ratio of time spent at each speed
                Double processPower = 0;
                for (Int32 step = 0; step < speedSteps; step++) {
                    Double ratio = (Double) (*cpuSpeedStepTimes)[step] / totalTimeAtSpeeds;
                    if (DEBUG && ratio != 0) {
                        Int32 uid;
                        u->GetUid(&uid);
                        Logger::D(TAG, "UID %d: CPU step #%d ratio=%s power=%s", uid, step, MakemAh(ratio).string(),
                            MakemAh(ratio*tmpCpuTime*(*powerCpuNormal)[step] / (60*60*1000)).string());
                    }
                    processPower += ratio * tmpCpuTime * (*powerCpuNormal)[step];
                }
                cpuTime += tmpCpuTime;
                if (DEBUG && processPower != 0) {
                    Logger::D(TAG, "process %s, cpu power=%s", strKey.string(),
                        MakemAh(processPower / (60*60*1000)).string());
                }
                power += processPower;
                if (packageWithHighestDrain.IsNull() || packageWithHighestDrain.StartWith("*")) {
                    highestDrain = processPower;
                    packageWithHighestDrain = strKey;
                }
                else if (highestDrain < processPower && !strKey.StartWith("*")) {
                    highestDrain = processPower;
                    packageWithHighestDrain = strKey;
                }
            }
        }
        if (cpuFgTime > cpuTime) {
            if (DEBUG && cpuFgTime > cpuTime + 10000) {
                Logger::D(TAG, "WARNING! Cputime is more than 10 seconds behind Foreground time");
            }
            cpuTime = cpuFgTime; // Statistics may not have been gathered yet.
        }
        power /= (60*60*1000);

        // Process wake lock usage
        AutoPtr<IMap> wakelockStats;
        u->GetWakelockStats((IMap**)&wakelockStats);
        AutoPtr<ISet> entrySet;
        wakelockStats->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IMapEntry* entry = IMapEntry::Probe(obj);
            AutoPtr<IInterface> value;
            entry->GetValue((IInterface**)&value);

            AutoPtr<IBatteryStatsUidWakelock> wakelock = IBatteryStatsUidWakelock::Probe(value);
            // Only care about partial wake locks since full wake locks
            // are canceled when the user turns the screen off.
            AutoPtr<IBatteryStatsTimer> timer;
            wakelock->GetWakeTime(IBatteryStats::WAKE_TYPE_PARTIAL, (IBatteryStatsTimer**)&timer);
            if (timer != NULL) {
                Int64 time;
                timer->GetTotalTimeLocked(mRawRealtime, which, &time);
                wakelockTime += time;
            }
        }
        appWakelockTimeUs += wakelockTime;
        wakelockTime /= 1000; // convert to millis

        // Add cost of holding a wake lock
        Double averagePower;
        mPowerProfile->GetAveragePower(IPowerProfile::POWER_CPU_AWAKE, &averagePower);
        p = (wakelockTime * averagePower) / (60*60*1000);
        if (DEBUG && p != 0) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: wake %lld power=%s", uid, wakelockTime, MakemAh(p).string());
        }
        power += p;

        // Add cost of mobile traffic
        Int64 mobileRx;
        u->GetNetworkActivityPackets(IBatteryStats::NETWORK_MOBILE_RX_DATA, mStatsType, &mobileRx);
        Int64 mobileTx;
        u->GetNetworkActivityPackets(IBatteryStats::NETWORK_MOBILE_TX_DATA, mStatsType, &mobileTx);
        Int64 mobileRxB;
        u->GetNetworkActivityBytes(IBatteryStats::NETWORK_MOBILE_RX_DATA, mStatsType, &mobileRxB);
        Int64 mobileTxB;
        u->GetNetworkActivityBytes(IBatteryStats::NETWORK_MOBILE_TX_DATA, mStatsType, &mobileTxB);
        Int64 mobileActive;
        u->GetMobileRadioActiveTime(mStatsType, &mobileActive);
        if (mobileActive > 0) {
            // We are tracking when the radio is up, so can use the active time to
            // determine power use.
            mAppMobileActive += mobileActive;
            p = (mobilePowerPerMs * mobileActive) / 1000;
        }
        else {
            // We are not tracking when the radio is up, so must approximate power use
            // based on the number of packets.
            p = (mobileRx + mobileTx) * mobilePowerPerPacket;
        }
        if (DEBUG && p != 0) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: mobile packets %lld active time %lld power=%s",
                uid, (mobileRx+mobileTx), mobileActive, MakemAh(p).string());
        }
        power += p;

        // Add cost of wifi traffic
        Int64 wifiRx;
        u->GetNetworkActivityPackets(IBatteryStats::NETWORK_WIFI_RX_DATA, mStatsType, &wifiRx);
        Int64 wifiTx;
        u->GetNetworkActivityPackets(IBatteryStats::NETWORK_WIFI_TX_DATA, mStatsType, &wifiTx);
        Int64 wifiRxB;
        u->GetNetworkActivityBytes(IBatteryStats::NETWORK_WIFI_RX_DATA, mStatsType, &wifiRxB);
        Int64 wifiTxB;
        u->GetNetworkActivityBytes(IBatteryStats::NETWORK_WIFI_TX_DATA, mStatsType, &wifiTxB);
        p = (wifiRx + wifiTx) * wifiPowerPerPacket;
        if (DEBUG && p != 0) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: wifi packets %lld power=%s",
                    uid, (mobileRx+mobileTx), MakemAh(p).string());
        }
        power += p;

        // Add cost of keeping WIFI running.
        Int64 wifiRunningTime;
        u->GetWifiRunningTime(mRawRealtime, which, &wifiRunningTime);
        Int64 wifiRunningTimeMs = wifiRunningTime / 1000;
        mAppWifiRunning += wifiRunningTimeMs;
        mPowerProfile->GetAveragePower(IPowerProfile::POWER_WIFI_ON, &averagePower);
        p = (wifiRunningTimeMs * averagePower) / (60*60*1000);
        if (DEBUG && p != 0) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: wifi running %lld power=%s",
                    uid, wifiRunningTimeMs, MakemAh(p).string());
        }
        power += p;

        // Add cost of WIFI scans
        Int64 wifiScanTime;
        u->GetWifiScanTime(mRawRealtime, which, &wifiScanTime);
        Int64 wifiScanTimeMs = wifiScanTime / 1000;
        mPowerProfile->GetAveragePower(IPowerProfile::POWER_WIFI_SCAN, &averagePower);
        p = (wifiScanTimeMs * averagePower) / (60*60*1000);
        if (DEBUG) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: wifi scan %lld power=%s", uid, wifiScanTimeMs, MakemAh(p).string());
        }
        power += p;
        for (Int32 bin = 0; bin < IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS; bin++) {
            Int64 batchScanTime;
            u->GetWifiBatchedScanTime(bin, mRawRealtime, which, &batchScanTime);
            Int64 batchScanTimeMs = batchScanTime / 1000;
            mPowerProfile->GetAveragePower(IPowerProfile::POWER_WIFI_BATCHED_SCAN, bin, &averagePower);
            p = (batchScanTimeMs * averagePower) / (60*60*1000);
            if (DEBUG && p != 0) {
                Int32 uid;
                u->GetUid(&uid);
                Logger::D(TAG, "UID %d: wifi batched scan # %d time=%lld power=%s", uid, bin, batchScanTimeMs, MakemAh(p).string());
            }
            power += p;
        }

        // Process Sensor usage
        AutoPtr<ISparseArray> sensorStats;
        u->GetSensorStats((ISparseArray**)&sensorStats);
        Int32 NSE;
        sensorStats->GetSize(&NSE);
        for (Int32 ise = 0; ise < NSE; ise++) {
            AutoPtr<IInterface> item;
            sensorStats->ValueAt(ise, (IInterface**)&item);
            AutoPtr<IBatteryStatsUidSensor> sensor = IBatteryStatsUidSensor::Probe(item);
            Int32 sensorHandle;
            sensorStats->KeyAt(ise, &sensorHandle);
            AutoPtr<IBatteryStatsTimer> timer;
            sensor->GetSensorTime((IBatteryStatsTimer**)&timer);
            Int64 time;
            timer->GetTotalTimeLocked(mRawRealtime, which, &time);
            Int64 sensorTime = time / 1000;
            Double multiplier = 0;
            switch (sensorHandle) {
                case IBatteryStatsUidSensor::GPS:
                    mPowerProfile->GetAveragePower(IPowerProfile::POWER_GPS_ON, &multiplier);
                    gpsTime = sensorTime;
                    break;
                default:
                    AutoPtr<ArrayOf<ISensor*> > sensorList;
                    sensorManager->GetSensorList(ISensor::TYPE_ALL, (ArrayOf<ISensor*>**)&sensorList);
                    for (Int32 i = 0; i < sensorList->GetLength(); i++) {
                        AutoPtr<ISensor> s = (*sensorList)[i];
                        Int32 handle;
                        s->GetHandle(&handle);
                        if (handle == sensorHandle) {
                            Float power;
                            s->GetPower(&power);
                            multiplier = power;
                            break;
                        }
                    }
            }
            p = (multiplier * sensorTime) / (60*60*1000);
            if (DEBUG && p != 0) {
                Int32 uid;
                u->GetUid(&uid);
                Logger::D(TAG, "UID %d: sensor #%d time=%lld power=%s",
                    uid, sensorHandle, sensorTime, MakemAh(p).string());
            }
            power += p;
        }

        if (DEBUG && power != 0) {
            Int32 uid;
            u->GetUid(&uid);
            Logger::D(TAG, "UID %d: total power=%s", uid, MakemAh(power).string());
        }

        // Add the app to the list if it is consuming power
        Int32 uid;
        u->GetUid(&uid);
        Int32 userId = UserHandle::GetUserId(uid);
        if (power != 0 || uid == 0) {
            AutoPtr<ArrayOf<Double> > array = ArrayOf<Double>::Alloc(1);
            (*array)[0] = power;
            AutoPtr<IBatterySipper> app;
            CBatterySipper::New(BatterySipperDrainType_APP, u, array, (IBatterySipper**)&app);
            app->SetCpuTime(cpuTime);
            app->SetGpsTime(gpsTime);
            app->SetWifiRunningTime(wifiRunningTimeMs);
            app->SetCpuFgTime(cpuFgTime);
            app->SetWakeLockTime(wakelockTime);
            app->SetMobileRxPackets(mobileRx);
            app->SetMobileTxPackets(mobileTx);
            app->SetMobileActive(mobileActive / 1000);
            Int32 data;
            u->GetMobileRadioActiveCount(mStatsType, &data);
            app->SetMobileActiveCount(data);
            app->SetWifiRxPackets(wifiRx);
            app->SetWifiTxPackets(wifiTx);
            app->SetMobileRxBytes(mobileRxB);
            app->SetMobileTxBytes(mobileTxB);
            app->SetWifiRxBytes(wifiRxB);
            app->SetWifiTxBytes(wifiTxB);
            app->SetPackageWithHighestDrain(packageWithHighestDrain);

            AutoPtr<IInterface> user;
            if (uid == IProcess::WIFI_UID) {
                mWifiSippers->Add(app);
                mWifiPower += power;
            }
            else if (uid == IProcess::BLUETOOTH_UID) {
                mBluetoothSippers->Add(app);;
                mBluetoothPower += power;
            }
            else if (!forAllUsers && (asUsers->Get(userId, (IInterface**)&user), user) == NULL
                    && UserHandle::GetAppId(uid) >= IProcess::FIRST_APPLICATION_UID) {
                AutoPtr<IInterface> obj;
                mUserSippers->Get(userId, (IInterface**)&obj);
                AutoPtr<IList> list = IList::Probe(obj); // List<BatterySipper>

                if (list == NULL) {
                    CArrayList::New((IList**)&list);
                    mUserSippers->Put(userId, list);
                }
                list->Add(app);
                if (power != 0) {
                    AutoPtr<IInterface> userPowerObj;
                    mUserPower->Get(userId, (IInterface**)&userPowerObj);
                    IDouble* userPower = IDouble::Probe(userPowerObj);
                    if (userPower == NULL) {
                        userPower = CoreUtils::Convert(power);
                    }
                    else {
                        Double data;
                        userPower->GetValue(&data);
                        userPower = NULL;
                        userPower = CoreUtils::Convert(data + power);
                    }
                    mUserPower->Put(userId, userPower);
                }
            }
            else {
                mUsageList->Add(app);
                if (power > mMaxPower) mMaxPower = power;
                if (power > mMaxRealPower) mMaxRealPower = power;
                mComputedPower += power;
            }
            if (uid == 0) {
                osApp = (CBatterySipper*)app.Get();
            }
        }
    }

    // The device has probably been awake for longer than the screen on
    // time and application wake lock time would account for.  Assign
    // this remainder to the OS, if possible.
    if (osApp != NULL) {
        Int64 wakeTimeMillis = mBatteryUptime / 1000;
        Int64 screenOnTime;
        mStats->GetScreenOnTime(mRawRealtime, which, &screenOnTime);
        wakeTimeMillis -= (appWakelockTimeUs / 1000) + (screenOnTime / 1000);
        if (wakeTimeMillis > 0) {
            Double averagePower;
            mPowerProfile->GetAveragePower(IPowerProfile::POWER_CPU_AWAKE, &averagePower);
            Double power = (wakeTimeMillis * averagePower) /  (60*60*1000);
            if (DEBUG) {
                Logger::D(TAG, "OS wakeLockTime %lld power %s", wakeTimeMillis, MakemAh(power).string());
            }
            osApp->mWakeLockTime += wakeTimeMillis;
            osApp->mValue += power;
            (*osApp->mValues)[0] += power;
            if (osApp->mValue > mMaxPower) mMaxPower = osApp->mValue;
            if (osApp->mValue > mMaxRealPower) mMaxRealPower = osApp->mValue;
            mComputedPower += power;
        }
    }
}

void CBatteryStatsHelper::AddPhoneUsage()
{
    Int64 phoneOnTime;
    mStats->GetPhoneOnTime(mRawRealtime, mStatsType, &phoneOnTime);
    Int64 phoneOnTimeMs = phoneOnTime / 1000;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_RADIO_ACTIVE, &averagePower);
    Double phoneOnPower = averagePower * phoneOnTimeMs / (60*60*1000);
    if (phoneOnPower != 0) {
        AutoPtr<IBatterySipper> bs = AddEntry(BatterySipperDrainType_PHONE, phoneOnTimeMs, phoneOnPower);
    }
}

void CBatteryStatsHelper::AddScreenUsage()
{
    Double power = 0;
    Int64 screenOnTime;
    mStats->GetScreenOnTime(mRawRealtime, mStatsType, &screenOnTime);
    Int64 screenOnTimeMs = screenOnTime / 1000;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_SCREEN_ON, &averagePower);
    power += screenOnTimeMs * averagePower;
    Double screenFullPower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_SCREEN_FULL, &screenFullPower);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        Double screenBinPower = screenFullPower * (i + 0.5f)
                / IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS;
        Int64 brightnessTime;
        mStats->GetScreenBrightnessTime(i, mRawRealtime, mStatsType, &brightnessTime);
        brightnessTime = brightnessTime / 1000;
        Double p = screenBinPower*brightnessTime;
        if (DEBUG && p != 0) {
            Logger::D(TAG, "Screen bin #%d: time=%lld power=%s",
                i, brightnessTime, MakemAh(p / (60 * 60 * 1000)).string());
        }
        power += p;
    }
    power /= (60*60*1000); // To hours
    if (power != 0) {
        AddEntry(BatterySipperDrainType_SCREEN, screenOnTimeMs, power);
    }
}

void CBatteryStatsHelper::AddRadioUsage()
{
    Double power = 0;
    Int32 BINS = ISignalStrength::NUM_SIGNAL_STRENGTH_BINS;
    Int64 signalTimeMs = 0;
    Int64 noCoverageTimeMs = 0;
    for (Int32 i = 0; i < BINS; i++) {
        Int64 strengthTime;
        mStats->GetPhoneSignalStrengthTime(i, mRawRealtime, mStatsType, &strengthTime);
        Int64 strengthTimeMs = strengthTime / 1000;
        Double averagePower;
        mPowerProfile->GetAveragePower(IPowerProfile::POWER_RADIO_ON, i, &averagePower);
        Double p = (strengthTimeMs * averagePower) / (60*60*1000);
        if (DEBUG && p != 0) {
            Logger::D(TAG, "Cell strength #%d: time=%lld power=%s",
                i, strengthTimeMs, MakemAh(p).string());
        }
        power += p;
        signalTimeMs += strengthTimeMs;
        if (i == 0) {
            noCoverageTimeMs = strengthTimeMs;
        }
    }
    Int64 scanningTime;
    mStats->GetPhoneSignalScanningTime(mRawRealtime, mStatsType, &scanningTime);
    Int64 scanningTimeMs = scanningTime / 1000;
    Double averagePower;
     mPowerProfile->GetAveragePower(IPowerProfile::POWER_RADIO_SCANNING, &averagePower);
    Double p = (scanningTimeMs * averagePower) / (60*60*1000);
    if (DEBUG && p != 0) {
        Logger::D(TAG, "Cell radio scanning: time=%lld power=%s", scanningTimeMs, MakemAh(p).string());
    }
    power += p;
    Int64 radioActiveTimeUs;
    mStats->GetMobileRadioActiveTime(mRawRealtime, mStatsType, &radioActiveTimeUs);
    Int64 remainingActiveTime = (radioActiveTimeUs - mAppMobileActive) / 1000;
    if (remainingActiveTime > 0) {
        power += GetMobilePowerPerMs() * remainingActiveTime;
    }
    if (power != 0) {
        AutoPtr<IBatterySipper> bs = AddEntry(BatterySipperDrainType_CELL, signalTimeMs, power);
        if (signalTimeMs != 0) {
            bs->SetNoCoveragePercent(noCoverageTimeMs * 100.0 / signalTimeMs);
        }
        bs->SetMobileActive(remainingActiveTime);
        Int32 data;
        mStats->GetMobileRadioActiveUnknownCount(mStatsType, &data);
        bs->SetMobileActiveCount(data);
    }
}

void CBatteryStatsHelper::AggregateSippers(
    /* [in] */ IBatterySipper* _bs,
    /* [in] */ IList* from,
    /* [in] */ const String& tag)
{
    CBatterySipper* bs = (CBatterySipper*)_bs;
    Int32 size;
    from->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        from->Get(i, (IInterface**)&obj);
        CBatterySipper* wbs = (CBatterySipper*)IBatterySipper::Probe(obj);

        if (DEBUG) {
            Logger::D(TAG, "%s adding sipper %s: cpu=%lld", tag.string(), TO_CSTR(wbs), wbs->mCpuTime);
        }
        bs->mCpuTime += wbs->mCpuTime;
        bs->mGpsTime += wbs->mGpsTime;
        bs->mWifiRunningTime += wbs->mWifiRunningTime;
        bs->mCpuFgTime += wbs->mCpuFgTime;
        bs->mWakeLockTime += wbs->mWakeLockTime;
        bs->mMobileRxPackets += wbs->mMobileRxPackets;
        bs->mMobileTxPackets += wbs->mMobileTxPackets;
        bs->mMobileActive += wbs->mMobileActive;
        bs->mMobileActiveCount += wbs->mMobileActiveCount;
        bs->mWifiRxPackets += wbs->mWifiRxPackets;
        bs->mWifiTxPackets += wbs->mWifiTxPackets;
        bs->mMobileRxBytes += wbs->mMobileRxBytes;
        bs->mMobileTxBytes += wbs->mMobileTxBytes;
        bs->mWifiRxBytes += wbs->mWifiRxBytes;
        bs->mWifiTxBytes += wbs->mWifiTxBytes;
    }
    bs->ComputeMobilemspp();
}

void CBatteryStatsHelper::AddWiFiUsage()
{
    Int64 onTime;
    mStats->GetWifiOnTime(mRawRealtime, mStatsType, &onTime);
    Int64 onTimeMs = onTime / 1000;
    Int64 runningTime;
    mStats->GetGlobalWifiRunningTime(mRawRealtime, mStatsType, &runningTime);
    Int64 runningTimeMs = runningTime / 1000;
    if (DEBUG) Logger::D(TAG, "WIFI runningTime=%lld app runningTime=%lld", runningTimeMs, mAppWifiRunning);
    runningTimeMs -= mAppWifiRunning;
    if (runningTimeMs < 0) runningTimeMs = 0;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_WIFI_ON, &averagePower);
    Double wifiPower = (onTimeMs * 0 /* TODO */* averagePower + runningTimeMs * averagePower) / (60*60*1000);
    if (DEBUG && wifiPower != 0) {
        Logger::D(TAG, "Wifi: time=%lld power=%s", runningTimeMs, MakemAh(wifiPower).string());
    }
    if ((wifiPower+mWifiPower) != 0) {
        AutoPtr<IBatterySipper> bs = AddEntry(BatterySipperDrainType_WIFI, runningTimeMs, wifiPower + mWifiPower);
        AggregateSippers(bs, mWifiSippers, String("WIFI"));
    }
}

void CBatteryStatsHelper::AddIdleUsage()
{
    Int64 onTime;
    mStats->GetScreenOnTime(mRawRealtime, mStatsType, &onTime);
    Int64 idleTimeMs = (mTypeBatteryRealtime - onTime) / 1000;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_CPU_IDLE, &averagePower);
    Double idlePower = (idleTimeMs * averagePower) / (60*60*1000);
    if (DEBUG && idlePower != 0) {
        Logger::D(TAG, "Idle: time=%lld power=%s", idleTimeMs, MakemAh(idlePower).string());
    }
    if (idlePower != 0) {
        AddEntry(BatterySipperDrainType_IDLE, idleTimeMs, idlePower);
    }
}

void CBatteryStatsHelper::AddBluetoothUsage()
{
    Int64 onTime;
    mStats->GetBluetoothOnTime(mRawRealtime, mStatsType, &onTime);
    Int64 btOnTimeMs = onTime / 1000;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_BLUETOOTH_ON, &averagePower);
    Double btPower = btOnTimeMs * averagePower / (60*60*1000);
    if (DEBUG && btPower != 0) {
        Logger::D(TAG, "Bluetooth: time=%lld power=%s", btOnTimeMs, MakemAh(btPower).string());
    }
    Int32 btPingCount;
    mStats->GetBluetoothPingCount(&btPingCount);
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_BLUETOOTH_AT_CMD, &averagePower);
    Double pingPower = (btPingCount * averagePower) / (60*60*1000);
    if (DEBUG && pingPower != 0) {
        Logger::D(TAG, "Bluetooth ping: count=%d power=", btPingCount, MakemAh(pingPower).string());
    }
    btPower += pingPower;
    if ((btPower+mBluetoothPower) != 0) {
        AutoPtr<IBatterySipper> bs = AddEntry(BatterySipperDrainType_BLUETOOTH, btOnTimeMs,
                btPower + mBluetoothPower);
        AggregateSippers(bs, mBluetoothSippers, String("Bluetooth"));
    }
}

void CBatteryStatsHelper::AddFlashlightUsage()
{
    Int64 flashlightOnTime;
    mStats->GetFlashlightOnTime(mRawRealtime, mStatsType, &flashlightOnTime);
    Int64 flashlightOnTimeMs = flashlightOnTime / 1000;
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_FLASHLIGHT, &averagePower);
    Double flashlightPower = flashlightOnTimeMs * averagePower / (60*60*1000);
    if (flashlightPower != 0) {
        AddEntry(BatterySipperDrainType_FLASHLIGHT, flashlightOnTimeMs, flashlightPower);
    }
}

void CBatteryStatsHelper::AddUserUsage()
{
    Int32 size;
    mUserSippers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        Int32 userId;
        mUserSippers->KeyAt(i, &userId);

        AutoPtr<IInterface> item;
        mUserSippers->ValueAt(i, (IInterface**)&item);
        IList* sippers = IList::Probe(item);

        AutoPtr<IInterface> userPowerObj;
        mUserPower->Get(userId, (IInterface**)&userPowerObj);
        IDouble* userPower = IDouble::Probe(userPowerObj);

        Double power = 0.0;
        if (userPower != NULL) {
            userPower->GetValue(&power);
        }

        AutoPtr<IBatterySipper> bs = AddEntry(BatterySipperDrainType_USER, 0, power);
        bs->SetUserId(userId);
        AggregateSippers(bs, sippers, String("User"));
    }
}

Double CBatteryStatsHelper::GetMobilePowerPerPacket()
{
    Int64 MOBILE_BPS = 200000; // TODO: Extract average bit rates from system
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_RADIO_ACTIVE, &averagePower);
    Double MOBILE_POWER = averagePower / 3600;

    Int64 mobileRx;
    mStats->GetNetworkActivityPackets(IBatteryStats::NETWORK_MOBILE_RX_DATA, mStatsType, &mobileRx);
    Int64 mobileTx;
    mStats->GetNetworkActivityPackets(IBatteryStats::NETWORK_MOBILE_TX_DATA, mStatsType, &mobileTx);
    Int64 mobileData = mobileRx + mobileTx;

    Int64 radioDataUptime;
    mStats->GetMobileRadioActiveTime(mRawRealtime, mStatsType, &radioDataUptime);
    Int64 radioDataUptimeMs = radioDataUptime / 1000;
    Double mobilePps = (mobileData != 0 && radioDataUptimeMs != 0)
            ? (mobileData / (Double)radioDataUptimeMs)
            : (((Double)MOBILE_BPS) / 8 / 2048);

    return (MOBILE_POWER / mobilePps) / (60*60);
}

Double CBatteryStatsHelper::GetMobilePowerPerMs()
{
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_RADIO_ACTIVE, &averagePower);
    return averagePower / (60*60*1000);
}

Double CBatteryStatsHelper::GetWifiPowerPerPacket()
{
    Double averagePower;
    mPowerProfile->GetAveragePower(IPowerProfile::POWER_WIFI_ACTIVE, &averagePower);
    Int64 WIFI_BPS = 1000000; // TODO: Extract average bit rates from system
    Double WIFI_POWER = averagePower / 3600;
    return (WIFI_POWER / (((Double)WIFI_BPS) / 8 / 2048)) / (60*60);
}

void CBatteryStatsHelper::ProcessMiscUsage()
{
    AddUserUsage();
    AddPhoneUsage();
    AddScreenUsage();
    AddFlashlightUsage();
    AddWiFiUsage();
    AddBluetoothUsage();
    AddIdleUsage(); // Not including cellular idle power
    // Don't compute radio usage if it's a wifi-only device
    if (!mWifiOnly) {
        AddRadioUsage();
    }
}

AutoPtr<IBatterySipper> CBatteryStatsHelper::AddEntry(
    /* [in] */ BatterySipperDrainType drainType,
    /* [in] */ Int64 time,
    /* [in] */ Double power)
{
    mComputedPower += power;
    if (power > mMaxRealPower) mMaxRealPower = power;
    return AddEntryNoTotal(drainType, time, power);
}

AutoPtr<IBatterySipper> CBatteryStatsHelper::AddEntryNoTotal(
    /* [in] */ BatterySipperDrainType drainType,
    /* [in] */ Int64 time,
    /* [in] */ Double power)
{
    if (power > mMaxPower) mMaxPower = power;
    AutoPtr<ArrayOf<Double> > array = ArrayOf<Double>::Alloc(1);
    (*array)[0] = power;
    AutoPtr<IBatterySipper> bs;
    CBatterySipper::New(drainType, NULL, array, (IBatterySipper**)&bs);
    bs->SetUsageTime(time);
    mUsageList->Add(bs);
    return bs;
}

ECode CBatteryStatsHelper::GetUsageList(
    /* [out] */ IList** usageList)
{
    VALIDATE_NOT_NULL(usageList)
    *usageList = mUsageList;
    REFCOUNT_ADD(*usageList)
    return NOERROR;
}

ECode CBatteryStatsHelper::GetMobilemsppList(
    /* [out] */ IList** mobilemsppList)
{
    VALIDATE_NOT_NULL(mobilemsppList)
    *mobilemsppList = mMobilemsppList;
    REFCOUNT_ADD(*mobilemsppList)
    return NOERROR;
}

ECode CBatteryStatsHelper::GetStatsPeriod(
    /* [out] */ Int64* statsPeriod)
{
    VALIDATE_NOT_NULL(statsPeriod)
    *statsPeriod = mStatsPeriod;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetStatsType(
    /* [out] */ Int32* statsType)
{
    VALIDATE_NOT_NULL(statsType)
    *statsType = mStatsType;
    return NOERROR;
};

ECode CBatteryStatsHelper::GetMaxPower(
    /* [out] */ Double* maxPower)
{
    VALIDATE_NOT_NULL(maxPower)
    *maxPower = mMaxPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetMaxRealPower(
    /* [out] */ Double* maxRealPower)
{
    VALIDATE_NOT_NULL(maxRealPower)
    *maxRealPower = mMaxRealPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetTotalPower(
    /* [out] */ Double* totalPower)
{
    VALIDATE_NOT_NULL(totalPower)
    *totalPower = mTotalPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetComputedPower(
    /* [out] */ Double* computedPower)
{
    VALIDATE_NOT_NULL(computedPower)
    *computedPower = mComputedPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetMinDrainedPower(
    /* [out] */ Double* minDrainedPower)
{
    VALIDATE_NOT_NULL(minDrainedPower)
    *minDrainedPower = mMinDrainedPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetMaxDrainedPower(
    /* [out] */ Double* maxDrainedPower)
{
    VALIDATE_NOT_NULL(maxDrainedPower)
    *maxDrainedPower = mMaxDrainedPower;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetBatteryTimeRemaining(
    /* [out] */ Int64* batteryTimeRemaining)
{
    VALIDATE_NOT_NULL(batteryTimeRemaining)
    *batteryTimeRemaining = mBatteryTimeRemaining;
    return NOERROR;
}

ECode CBatteryStatsHelper::GetChargeTimeRemaining(
    /* [out] */ Int64* chargeTimeRemaining)
{
    VALIDATE_NOT_NULL(chargeTimeRemaining)
    *chargeTimeRemaining = mChargeTimeRemaining;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CBatteryStatsHelper::ReadFully(
    /* [in] */ IFileInputStream* stream)
{
    Int32 avail;
    IInputStream::Probe(stream)->Available(&avail);
    return ReadFully(stream, avail);
}

AutoPtr<ArrayOf<Byte> > CBatteryStatsHelper::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [in] */ Int32 avail)
{
    Int32 pos = 0;
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(avail);
    while (TRUE) {
        Int32 amt;
        IInputStream::Probe(stream)->Read(data, pos, data->GetLength()-pos, &amt);
        //Logger::I("foo", "Read " + amt + " bytes at " + pos
        //        + " of avail " + data.length);
        if (amt <= 0) {
            //Logger::I("foo", "**** FINISHED READING: pos=" + pos
            //        + " len=" + data.length);
            return data;
        }
        pos += amt;
        IInputStream::Probe(stream)->Available(&avail);
        if (avail > data->GetLength()-pos) {
            AutoPtr<ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(pos+avail);
            newData->Copy(data, pos);
            data = newData;
        }
    }
}

void CBatteryStatsHelper::Load()
{
    if (mBatteryInfo == NULL) {
        return;
    }
    mStats = IBatteryStats::Probe(GetStats(mBatteryInfo));
    if (mCollectBatteryBroadcast) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&filter);
        mBatteryBroadcast = NULL;
        mContext->RegisterReceiver(NULL, filter, (IIntent**)&mBatteryBroadcast);
    }
}

AutoPtr<IBatteryStatsImpl> CBatteryStatsHelper::GetStats(
    /* [in] */ IIBatteryStats* service)
{
    // try {
    AutoPtr<IParcelFileDescriptor> pfd;
    ECode ec = service->GetStatisticsStream((IParcelFileDescriptor**)&pfd);
    if (FAILED(ec)) goto fail;
    if (pfd != NULL) {
        AutoPtr<IFileInputStream> fis;
        CParcelFileDescriptorAutoCloseInputStream::New(
                pfd, (IFileInputStream**)&fis);
        // try {
        AutoPtr<IFileDescriptor> fileDesc;
        pfd->GetFileDescriptor((IFileDescriptor**)&fileDesc);
        Int32 size;
        CMemoryFile::GetSize(fileDesc, &size);
        AutoPtr<ArrayOf<Byte> > data = ReadFully(fis, size);
        AutoPtr<IParcel> parcel; // CParcel.obtain();
        CParcel::New((IParcel**)&parcel);
        parcel->Unmarshall(data, 0, data->GetLength());
        parcel->SetDataPosition(0);
        AutoPtr<CBatteryStatsImpl> stats;
        CBatteryStatsImpl::NewByFriend((CBatteryStatsImpl**)&stats);
        stats->ReadFromParcel(parcel);
        stats->DistributeWorkLocked(IBatteryStats::STATS_SINCE_CHARGED);
        return IBatteryStatsImpl::Probe(stats);
        // } catch (IOException e) {
        //     Log.w(TAG, "Unable to read statistics stream", e);
        // }
    }
    // } catch (RemoteException e) {
    //     Log.w(TAG, "RemoteException:", e);
    // }
fail:
    if (FAILED(ec)) {
        Logger::W(TAG, "RemoteException: 0x%08x", ec);
    }
    AutoPtr<IBatteryStatsImpl> stats;
    CBatteryStatsImpl::New((IBatteryStatsImpl**)&stats);
    return stats;
}

} // Os
} // Internal
} // Droid
} // Elastos
