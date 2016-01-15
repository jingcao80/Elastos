
#include "CBatteryService.h"
#include "am/CBatteryStatsService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/ioctl.h>
#include <hardware/power.h>
#include <hardware_legacy/power.h>
#include <cutils/android_reboot.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::R;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Server::Am::CBatteryStatsService;

namespace Elastos {
namespace Droid {
namespace Server {

//=====================================================================
// CBatteryService::ShutdownIfNoPowerLockedRunnable
//=====================================================================
ECode CBatteryService::ShutdownIfNoPowerLockedRunnable::Run()
{
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("sys.battery_zero"), String("1"));
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_REQUEST_SHUTDOWN, (IIntent**)&intent);
        intent->PutBooleanExtra(IIntent::EXTRA_KEY_CONFIRM, FALSE);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> CURRENT;
        helper->GetCURRENT((IUserHandle**)&CURRENT);
        mHost->mContext->StartActivityAsUser(intent, CURRENT);
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService::ShutdownIfInBootFastModeRunnable
//=====================================================================
ECode CBatteryService::ShutdownIfInBootFastModeRunnable::Run()
{
    Slogger::D(TAG, "shutdownIfInBootFastModeLocked now shutdown!");
    mHost->NativeShutdown(); //acquire a wakelock named battery
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("sys.battery_zero"), String("1"));
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_REQUEST_SHUTDOWN, (IIntent**)&intent);
        intent->PutBooleanExtra(IIntent::EXTRA_KEY_CONFIRM, FALSE);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> CURRENT;
        helper->GetCURRENT((IUserHandle**)&CURRENT);
        mHost->mContext->StartActivityAsUser(intent, CURRENT);
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService::ShutdownIfOverTempRunnable
//=====================================================================
ECode CBatteryService::ShutdownIfOverTempRunnable::Run()
{
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("sys.temperature_high"), String("1"));
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_REQUEST_SHUTDOWN, (IIntent**)&intent);
        intent->PutBooleanExtra(IIntent::EXTRA_KEY_CONFIRM, FALSE);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> CURRENT;
        helper->GetCURRENT((IUserHandle**)&CURRENT);
        mHost->mContext->StartActivityAsUser(intent, CURRENT);
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService::ActionPowerConnectedRunnable
//=====================================================================
ECode CBatteryService::ActionPowerConnectedRunnable::Run()
{
    AutoPtr<IIntent> statusIntent;
    CIntent::New(IIntent::ACTION_POWER_CONNECTED, (IIntent**)&statusIntent);
    statusIntent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mHost->mContext->SendBroadcastAsUser(statusIntent, ALL);
    return NOERROR;
}


//=====================================================================
// CBatteryService::ActionPowerDisconnectedRunnable
//=====================================================================
ECode CBatteryService::ActionPowerDisconnectedRunnable::Run()
{
    AutoPtr<IIntent> statusIntent;
    CIntent::New(IIntent::ACTION_POWER_DISCONNECTED, (IIntent**)&statusIntent);
    statusIntent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mHost->mContext->SendBroadcastAsUser(statusIntent, ALL);
    return NOERROR;
}


//=====================================================================
// CBatteryService::ActionBatteryLowRunnable
//=====================================================================
ECode CBatteryService::ActionBatteryLowRunnable::Run()
{
    AutoPtr<IIntent> statusIntent;
    CIntent::New(IIntent::ACTION_BATTERY_LOW, (IIntent**)&statusIntent);
    statusIntent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mHost->mContext->SendBroadcastAsUser(statusIntent, ALL);
    return NOERROR;
}


//=====================================================================
// CBatteryService::ActionBatteryOkayRunnable
//=====================================================================
ECode CBatteryService::ActionBatteryOkayRunnable::Run()
{
    AutoPtr<IIntent> statusIntent;
    CIntent::New(IIntent::ACTION_BATTERY_OKAY, (IIntent**)&statusIntent);
    statusIntent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mHost->mContext->SendBroadcastAsUser(statusIntent, ALL);
    return NOERROR;
}


//=====================================================================
// CBatteryService::SendIntentRunnable
//=====================================================================
ECode CBatteryService::SendIntentRunnable::Run()
{
    ActivityManagerNative::BroadcastStickyIntent(mIntent, String(NULL), IUserHandle::USER_ALL);
    return NOERROR;
}


//=====================================================================
// CBatteryService::PowerSupplyObserver
//=====================================================================
void CBatteryService::PowerSupplyObserver::OnUEvent(
    /* [in] */ UEventObserver::UEvent* event)
{
    AutoLock Lock(mHost->mLock);
    mHost->UpdateLocked();
}


//=====================================================================
// CBatteryService::InvalidChargerObserver
//=====================================================================
void CBatteryService::InvalidChargerObserver::OnUEvent(
    /* [in] */ UEventObserver::UEvent* event)
{
    String result = event->Get(String("SWITCH_STATE"));
    Int32 invalidCharger = CString("1").Equals(result) ? 1 : 0;
    AutoLock Lock(mHost->mLock);
    if (mHost->mInvalidCharger != invalidCharger) {
        mHost->mInvalidCharger = invalidCharger;
        mHost->UpdateLocked();
    }
}


//=====================================================================
// CBatteryService::Led
//=====================================================================
CBatteryService::Led::Led(
    /* [in] */ IContext* context,
    /* [in] */ LightsService* lights,
    /* [in] */ CBatteryService* host)
    : mHost(host)
{
    mBatteryLight = lights->GetLight(LightsService::_LIGHT_ID_BATTERY);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_notificationsBatteryLowARGB, &mBatteryLowARGB);
    res->GetInteger(R::integer::config_notificationsBatteryMediumARGB, &mBatteryMediumARGB);
    res->GetInteger(R::integer::config_notificationsBatteryFullARGB, &mBatteryFullARGB);
    res->GetInteger(R::integer::config_notificationsBatteryLedOn, &mBatteryLedOn);
    res->GetInteger(R::integer::config_notificationsBatteryLedOff, &mBatteryLedOff);
}

ECode CBatteryService::Led::UpdateLightsLocked()
{
    Int32 level = mHost->mBatteryLevel;
    Int32 status = mHost->mBatteryStatus;
    if (level < mHost->mLowBatteryWarningLevel) {
        if (status == IBatteryManager::BATTERY_STATUS_CHARGING) {
            // Solid red when battery is charging
            mBatteryLight->SetColor(mBatteryLowARGB);
        }
        else {
            // Flash red when battery is low and not charging
            mBatteryLight->SetFlashing(mBatteryLowARGB, LightsService::_LIGHT_FLASH_TIMED,
                    mBatteryLedOn, mBatteryLedOff);
        }
    }
    else if (status == IBatteryManager::BATTERY_STATUS_CHARGING
            || status == IBatteryManager::BATTERY_STATUS_FULL) {
        if (status == IBatteryManager::BATTERY_STATUS_FULL || level >= 90) {
            // Solid green when full or charging and nearly full
            mBatteryLight->SetColor(mBatteryFullARGB);
        }
        else {
            // Solid orange when charging and halfway full
            mBatteryLight->SetColor(mBatteryMediumARGB);
        }
    }
    else {
        // No lights if not charging and not low
        mBatteryLight->TurnOff();
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService::
//=====================================================================
const String CBatteryService::TAG("CBatteryService");
const Boolean CBatteryService::DEBUG = FALSE;
const Int32 CBatteryService::BATTERY_SCALE;
const Int32 CBatteryService::DUMP_MAX_LENGTH;
const Int32 CBatteryService::BATTERY_PLUGGED_NONE;
const Int32 CBatteryService::BOOT_FAST_REAL_SHUT_DOWN_LEVEL;

static AutoPtr< ArrayOf<String> > InitDUMPSYS_ARGS()
{
    AutoPtr< ArrayOf<String> > marray = ArrayOf<String>::Alloc(2);
    marray->Set(0, String("--checkin"));
    marray->Set(1, String("-u"));
    return marray;
}
const AutoPtr< ArrayOf<String> > CBatteryService::DUMPSYS_ARGS = InitDUMPSYS_ARGS();
const String CBatteryService::BATTERY_STATS_SERVICE_NAME("batteryinfo");
const String CBatteryService::DUMPSYS_DATA_PATH("/data/system/");

CBatteryService::CBatteryService()
    : mCriticalBatteryLevel(0)
    , mAcOnline(FALSE)
    , mUsbOnline(FALSE)
    , mWirelessOnline(FALSE)
    , mBatteryStatus(0)
    , mBatteryHealth(0)
    , mBatteryPresent(FALSE)
    , mBatteryLevel(0)
    , mBatteryVoltage(0)
    , mBatteryTemperature(0)
    , mBatteryLevelCritical(FALSE)
    , mLastBatteryStatus(0)
    , mLastBatteryHealth(0)
    , mLastBatteryPresent(FALSE)
    , mLastBatteryLevel(0)
    , mLastBatteryVoltage(0)
    , mLastBatteryTemperature(0)
    , mLastBatteryLevelCritical(FALSE)
    , mInvalidCharger(0)
    , mLastInvalidCharger(0)
    , mLowBatteryWarningLevel(0)
    , mLowBatteryCloseWarningLevel(0)
    , mShutdownBatteryTemperature(0)
    , mPlugType(0)
    , mLastPlugType(-1)
    , mDischargeStartTime(0)
    , mDischargeStartLevel(0)
    , mUpdatesStopped(FALSE)
    , mSentLowBatteryBroadcast(FALSE)
{
    mPowerSupplyObserver = new PowerSupplyObserver(this);
    mInvalidChargerObserver = new InvalidChargerObserver(this);
}

#define POWER_SUPPLY_PATH "/sys/class/power_supply"

struct BatteryManagerConstants {
    Int32 statusUnknown;
    Int32 statusCharging;
    Int32 statusDischarging;
    Int32 statusNotCharging;
    Int32 statusFull;
    Int32 healthUnknown;
    Int32 healthGood;
    Int32 healthOverheat;
    Int32 healthDead;
    Int32 healthOverVoltage;
    Int32 healthUnspecifiedFailure;
    Int32 healthCold;
};
static BatteryManagerConstants gConstants;
static Boolean gConstantsInitialized = FALSE;

struct PowerSupplyPaths {
    char* acOnlinePath;
    char* usbOnlinePath;
    char* wirelessOnlinePath;
    char* batteryStatusPath;
    char* batteryHealthPath;
    char* batteryPresentPath;
    char* batteryCapacityPath;
    char* batteryVoltagePath;
    char* batteryTemperaturePath;
    char* batteryTechnologyPath;
};
static PowerSupplyPaths gPaths;

static Int32 gVoltageDivisor = 1;

static Int32 ReadFromFile(const char* path, char* buf, size_t size);

static Boolean InitgPaths()
{
    char    path[PATH_MAX];
    struct dirent* entry;

    DIR* dir = opendir(POWER_SUPPLY_PATH);
    if (dir == NULL) {
        Slogger::E("CBatteryService", "Could not open %s\n", POWER_SUPPLY_PATH);
    }
    else {
        while ((entry = readdir(dir))) {
            const char* name = entry->d_name;

            // ignore "." and ".."
            if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
                continue;
            }

            char buf[20];
            // Look for "type" file in each subdirectory
            snprintf(path, sizeof(path), "%s/%s/type", POWER_SUPPLY_PATH, name);
            int length = ReadFromFile(path, buf, sizeof(buf));
            if (length > 0) {
                if (buf[length - 1] == '\n')
                    buf[length - 1] = 0;

                if (strcmp(buf, "Mains") == 0) {
                    snprintf(path, sizeof(path), "%s/%s/online", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.acOnlinePath = strdup(path);
                }
                else if (strcmp(buf, "USB") == 0) {
                    snprintf(path, sizeof(path), "%s/%s/online", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.usbOnlinePath = strdup(path);
                }
                else if (strcmp(buf, "Wireless") == 0) {
                    snprintf(path, sizeof(path), "%s/%s/online", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.wirelessOnlinePath = strdup(path);
                }
                else if (strcmp(buf, "Battery") == 0) {
                    snprintf(path, sizeof(path), "%s/%s/status", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.batteryStatusPath = strdup(path);
                    snprintf(path, sizeof(path), "%s/%s/health", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.batteryHealthPath = strdup(path);
                    snprintf(path, sizeof(path), "%s/%s/present", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.batteryPresentPath = strdup(path);
                    snprintf(path, sizeof(path), "%s/%s/capacity", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.batteryCapacityPath = strdup(path);

                    snprintf(path, sizeof(path), "%s/%s/voltage_now", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0) {
                        gPaths.batteryVoltagePath = strdup(path);
                        // voltage_now is in microvolts, not millivolts
                        gVoltageDivisor = 1000;
                    } else {
                        snprintf(path, sizeof(path), "%s/%s/batt_vol", POWER_SUPPLY_PATH, name);
                        if (access(path, R_OK) == 0)
                            gPaths.batteryVoltagePath = strdup(path);
                    }

                    snprintf(path, sizeof(path), "%s/%s/temp", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0) {
                        gPaths.batteryTemperaturePath = strdup(path);
                    } else {
                        snprintf(path, sizeof(path), "%s/%s/batt_temp", POWER_SUPPLY_PATH, name);
                        if (access(path, R_OK) == 0)
                            gPaths.batteryTemperaturePath = strdup(path);
                    }

                    snprintf(path, sizeof(path), "%s/%s/technology", POWER_SUPPLY_PATH, name);
                    if (access(path, R_OK) == 0)
                        gPaths.batteryTechnologyPath = strdup(path);
                }
            }
        }
        closedir(dir);
    }

    if (!gPaths.acOnlinePath)
        Slogger::E("CBatteryService", "acOnlinePath not found");
    if (!gPaths.usbOnlinePath)
        Slogger::E("CBatteryService", "usbOnlinePath not found");
    if (!gPaths.wirelessOnlinePath)
        Slogger::E("CBatteryService", "wirelessOnlinePath not found");
    if (!gPaths.batteryStatusPath)
        Slogger::E("CBatteryService", "batteryStatusPath not found");
    if (!gPaths.batteryHealthPath)
        Slogger::E("CBatteryService", "batteryHealthPath not found");
    if (!gPaths.batteryPresentPath)
        Slogger::E("CBatteryService", "batteryPresentPath not found");
    if (!gPaths.batteryCapacityPath)
        Slogger::E("CBatteryService", "batteryCapacityPath not found");
    if (!gPaths.batteryVoltagePath)
        Slogger::E("CBatteryService", "batteryVoltagePath not found");
    if (!gPaths.batteryTemperaturePath)
        Slogger::E("CBatteryService", "batteryTemperaturePath not found");
    if (!gPaths.batteryTechnologyPath)
        Slogger::E("CBatteryService", "batteryTechnologyPath not found");
    return TRUE;
}

static Boolean gPathsInitialized = InitgPaths();

static void InitgConstants()
{
    if (gConstantsInitialized) return;

    gConstants.statusUnknown = IBatteryManager::BATTERY_STATUS_UNKNOWN;
    gConstants.statusCharging = IBatteryManager::BATTERY_STATUS_CHARGING;
    gConstants.statusDischarging = IBatteryManager::BATTERY_STATUS_DISCHARGING;
    gConstants.statusNotCharging = IBatteryManager::BATTERY_STATUS_NOT_CHARGING;
    gConstants.statusFull = IBatteryManager::BATTERY_STATUS_FULL;
    gConstants.healthUnknown = IBatteryManager::BATTERY_HEALTH_UNKNOWN;
    gConstants.healthGood = IBatteryManager::BATTERY_HEALTH_GOOD;
    gConstants.healthOverheat = IBatteryManager::BATTERY_HEALTH_OVERHEAT;
    gConstants.healthDead = IBatteryManager::BATTERY_HEALTH_DEAD;
    gConstants.healthOverVoltage = IBatteryManager::BATTERY_HEALTH_OVER_VOLTAGE;
    gConstants.healthUnspecifiedFailure = IBatteryManager::BATTERY_HEALTH_UNSPECIFIED_FAILURE;
    gConstants.healthCold = IBatteryManager::BATTERY_HEALTH_COLD;
    gConstantsInitialized = TRUE;
}

static Int32 GetBatteryStatus(const char* status)
{
    InitgConstants();
    switch (status[0]) {
        case 'C': return gConstants.statusCharging;         // Charging
        case 'D': return gConstants.statusDischarging;      // Discharging
        case 'F': return gConstants.statusFull;             // Full
        case 'N': return gConstants.statusNotCharging;      // Not charging
        case 'U': return gConstants.statusUnknown;          // Unknown

        default: {
            Slogger::W("CBatteryService", "Unknown battery status '%s'", status);
            return gConstants.statusUnknown;
        }
    }
}

static Int32 GetBatteryHealth(const char* status)
{
    InitgConstants();
    switch (status[0]) {
        case 'C': return gConstants.healthCold;         // Cold
        case 'D': return gConstants.healthDead;         // Dead
        case 'G': return gConstants.healthGood;         // Good
        case 'O': {
            if (strcmp(status, "Overheat") == 0) {
                return gConstants.healthOverheat;
            }
            else if (strcmp(status, "Over voltage") == 0) {
                return gConstants.healthOverVoltage;
            }
            Slogger::W("CBatteryService", "Unknown battery health[1] '%s'", status);
            return gConstants.healthUnknown;
        }

        case 'U': {
            if (strcmp(status, "Unspecified failure") == 0) {
                return gConstants.healthUnspecifiedFailure;
            }
            else if (strcmp(status, "Unknown") == 0) {
                return gConstants.healthUnknown;
            }
            // fall through
        }

        default: {
            Slogger::W("CBatteryService", "Unknown battery health[2] '%s'", status);
            return gConstants.healthUnknown;
        }
    }
}

static Int32 ReadFromFile(const char* path, char* buf, size_t size)
{
    if (!path)
        return -1;
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        Slogger::E("CBatteryService", "Could not open '%s'", path);
        return -1;
    }

    ssize_t count = read(fd, buf, size);
    if (count > 0) {
        while (count > 0 && buf[count-1] == '\n')
            count--;
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    }

    close(fd);
    return count;
}

static void SetBooleanField(const char* path, Boolean* field)
{
    const int SIZE = 16;
    char buf[SIZE];

    Boolean value = FALSE;
    if (ReadFromFile(path, buf, SIZE) > 0) {
        if (buf[0] != '0') {
            value = TRUE;
        }
    }
    *field = value;
}

static void SetInt32Field(const char* path, Int32* field)
{
    const int SIZE = 128;
    char buf[SIZE];

    Int32 value = 0;
    if (ReadFromFile(path, buf, SIZE) > 0) {
        value = atoi(buf);
    }
    *field = value;
}

static void SetVoltageField(const char* path, Int32* field)
{
    const int SIZE = 128;
    char buf[SIZE];

    Int32 value = 0;
    if (ReadFromFile(path, buf, SIZE) > 0) {
        value = atoi(buf);
        value /= gVoltageDivisor;
    }
    *field = value;
}

void CBatteryService::NativeUpdate()
{
    SetBooleanField(gPaths.acOnlinePath, &mAcOnline);
    SetBooleanField(gPaths.usbOnlinePath, &mUsbOnline);
    SetBooleanField(gPaths.wirelessOnlinePath, &mWirelessOnline);
    SetBooleanField(gPaths.batteryPresentPath, &mBatteryPresent);

    SetInt32Field(gPaths.batteryCapacityPath, &mBatteryLevel);
    SetVoltageField(gPaths.batteryVoltagePath, &mBatteryVoltage);
    SetInt32Field(gPaths.batteryTemperaturePath, &mBatteryTemperature);

    const int SIZE = 128;
    char buf[SIZE];

    if (ReadFromFile(gPaths.batteryStatusPath, buf, SIZE) > 0) {
        mBatteryStatus = GetBatteryStatus(buf);
    }
    else {
        InitgConstants();
        mBatteryStatus = gConstants.statusUnknown;
    }

    if (ReadFromFile(gPaths.batteryHealthPath, buf, SIZE) > 0) {
        mBatteryHealth = GetBatteryHealth(buf);
    }

    if (ReadFromFile(gPaths.batteryTechnologyPath, buf, SIZE) > 0) {
        mBatteryTechnology = buf;
    }
}

void CBatteryService::NativeShutdown()
{
    Slogger::D("CBatteryService", "android_server_BatteryService_shutDownNotFromPMS");
    acquire_wake_lock(PARTIAL_WAKE_LOCK, "battery");
    //android_reboot(ANDROID_RB_POWEROFF, 0, 0);
}

ECode CBatteryService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Handle32 lights)
{
    mContext = context;
    CHandler::New(TRUE/*async*/, (IHandler**)&mHandler);
    mLed = new Led(context, (LightsService*)lights, this);
    mBatteryStats = CBatteryStatsService::GetService();

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_criticalBatteryWarningLevel, &mCriticalBatteryLevel);
    res->GetInteger(R::integer::config_lowBatteryWarningLevel, &mLowBatteryWarningLevel);
    res->GetInteger(R::integer::config_lowBatteryCloseWarningLevel, &mLowBatteryCloseWarningLevel);
    res->GetInteger(R::integer::config_shutdownBatteryTemperature, &mShutdownBatteryTemperature);

    mPowerSupplyObserver->StartObserving(String("SUBSYSTEM=power_supply"));

    // watch for invalid charger messages if the invalid_charger switch exists
    AutoPtr<IFile> f;
    CFile::New(String("/sys/devices/virtual/switch/invalid_charger/state"),(IFile**)&f);
    Boolean isExists;
    f->Exists(&isExists);
    if (isExists) {
        mInvalidChargerObserver->StartObserving(
            String("DEVPATH=/devices/virtual/switch/invalid_charger"));
    }

    // set initial status
    AutoLock lock(mLock);
    UpdateLocked();

    return NOERROR;
}

void CBatteryService::SystemReady()
{
    // check our power situation now that it is safe to display the shutdown dialog.
    AutoLock lock(mLock);
    ShutdownIfNoPowerLocked();
    ShutdownIfOverTempLocked();
}

Boolean CBatteryService::IsPowered(
    /* [in] */ Int32 plugTypeSet)
{
    AutoLock lock(mLock);
    return IsPoweredLocked(plugTypeSet);
}

Boolean CBatteryService::IsPoweredLocked(
    /* [in] */ Int32 plugTypeSet)
{
    // assume we are powered if battery state is unknown so
    // the "stay on while plugged in" option will work.
    if (mBatteryStatus == IBatteryManager::BATTERY_STATUS_UNKNOWN) {
        return TRUE;
    }
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_AC) != 0 && mAcOnline) {
        return TRUE;
    }
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_USB) != 0 && mUsbOnline) {
        return TRUE;
    }
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_WIRELESS) != 0 && mWirelessOnline) {
        return TRUE;
    }
    return FALSE;
}

Int32 CBatteryService::GetPlugType()
{
    AutoLock lock(mLock);
    return mPlugType;
}

Int32 CBatteryService::GetBatteryLevel()
{
    AutoLock lock(mLock);
    return mBatteryLevel;
}

Boolean CBatteryService::IsBatteryLow()
{
    AutoLock lock(mLock);
    return (mBatteryPresent && mBatteryLevel <= mLowBatteryWarningLevel);
}

void CBatteryService::ShutdownIfNoPowerLocked()
{
    // shut down gracefully if our battery is critically low and we are not powered.
    // wait until the system has booted before attempting to display the shutdown dialog.
    Boolean isPoweredLocked = IsPoweredLocked(IBatteryManager::BATTERY_PLUGGED_ANY);
    if (mBatteryLevel == 0 && !isPoweredLocked) {
        AutoPtr<ShutdownIfNoPowerLockedRunnable> run = new ShutdownIfNoPowerLockedRunnable(this);
        Boolean b;
        mHandler->Post(run, &b);
    }
}

void CBatteryService::ShutdownIfInBootFastModeLocked()
{
    // shut down gracefully if our battery is boot fast mode and critically low and we are not powered.
    // wait until the system has booted before attempting to display the shutdown dialog.
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
        AutoPtr<IPowerManager> power = IPowerManager::Probe(service);
        Boolean bootstats;
        power->IsBootFastStatus(&bootstats);
        if (mBatteryLevel < BOOT_FAST_REAL_SHUT_DOWN_LEVEL
                && !IsPoweredLocked(IBatteryManager::BATTERY_PLUGGED_ANY) && bootstats) {
            AutoPtr<ShutdownIfInBootFastModeRunnable> run = new ShutdownIfInBootFastModeRunnable(this);
            Boolean b;
            mHandler->Post(run, &b);
        }
    }
}

void CBatteryService::ShutdownIfOverTempLocked()
{
    // shut down gracefully if temperature is too high (> 68.0C by default)
    // wait until the system has booted before attempting to display the
    // shutdown dialog.
    if (mBatteryTemperature > mShutdownBatteryTemperature) {
        AutoPtr<ShutdownIfOverTempRunnable> run = new ShutdownIfOverTempRunnable(this);
        Boolean b;
        mHandler->Post(run, &b);
    }
}

void CBatteryService::UpdateLocked()
{
    if (!mUpdatesStopped) {
        // Update the values of mAcOnline, et. all.
        NativeUpdate();

        // Process the new values.
        ProcessValuesLocked();
    }
}

void CBatteryService::ProcessValuesLocked()
{
    Boolean logOutlier = FALSE;
    Int64 dischargeDuration = 0;

    mBatteryLevelCritical = (mBatteryLevel <= mCriticalBatteryLevel);
    if (mAcOnline) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_AC;
    }
    else if (mUsbOnline) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_USB;
    }
    else if (mWirelessOnline) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_WIRELESS;
    }
    else {
        mPlugType = BATTERY_PLUGGED_NONE;
    }

    if (DEBUG) {
        StringBuilder sb("Processing new values: ");
        sb += "mAcOnline=";
        sb += mAcOnline;
        sb += ", mUsbOnline=";
        sb += mUsbOnline;
        sb += ", mWirelessOnline=";
        sb += mWirelessOnline;
        sb += ", mBatteryStatus=";
        sb += mBatteryStatus;
        sb += ", mBatteryHealth=";
        sb += mBatteryHealth;
        sb += ", mBatteryPresent=";
        sb += mBatteryPresent;
        sb += ", mBatteryLevel=";
        sb += mBatteryLevel;
        sb += ", mBatteryTechnology=";
        sb += mBatteryTechnology;
        sb += ", mBatteryVoltage=";
        sb += mBatteryVoltage;
        sb += ", mBatteryTemperature=";
        sb += mBatteryTemperature;
        sb += ", mBatteryLevelCritical=";
        sb += mBatteryLevelCritical;
        sb += ", mPlugType=";
        sb += mPlugType;
        Slogger::D(TAG, sb.ToString());
    }

    // TODO:
    // Let the battery stats keep track of the current level.
    // try {
    mBatteryStats->SetBatteryState(mBatteryStatus, mBatteryHealth,
            mPlugType, mBatteryLevel, mBatteryTemperature,
            mBatteryVoltage);
    // } catch (RemoteException e) {
    //     Should never happen.
    // }

    ShutdownIfNoPowerLocked();
    ShutdownIfInBootFastModeLocked();
    ShutdownIfOverTempLocked();

    if (mBatteryStatus != mLastBatteryStatus ||
            mBatteryHealth != mLastBatteryHealth ||
            mBatteryPresent != mLastBatteryPresent ||
            mBatteryLevel != mLastBatteryLevel ||
            mPlugType != mLastPlugType ||
            mBatteryVoltage != mLastBatteryVoltage ||
            mBatteryTemperature != mLastBatteryTemperature ||
            mInvalidCharger != mLastInvalidCharger) {

        if (mPlugType != mLastPlugType) {
            if (mLastPlugType == BATTERY_PLUGGED_NONE) {
                // discharging -> charging

                // There's no value in this data unless we've discharged at least once and the
                // battery level has changed; so don't log until it does.
                if (mDischargeStartTime != 0 && mDischargeStartLevel != mBatteryLevel) {
                    dischargeDuration = SystemClock::GetElapsedRealtime() - mDischargeStartTime;
                    logOutlier = TRUE;
                    //EventLog::WriteEvent(EventLogTags::BATTERY_DISCHARGE, dischargeDuration,
                    //        mDischargeStartLevel, mBatteryLevel);
                    // make sure we see a discharge event before logging again
                    mDischargeStartTime = 0;
                }
            }
            else if (mPlugType == BATTERY_PLUGGED_NONE) {
                // charging -> discharging or we just powered up
                mDischargeStartTime = SystemClock::GetElapsedRealtime();
                mDischargeStartLevel = mBatteryLevel;
            }
        }
        if (mBatteryStatus != mLastBatteryStatus ||
            mBatteryHealth != mLastBatteryHealth ||
            mBatteryPresent != mLastBatteryPresent ||
            mPlugType != mLastPlugType) {
            //EventLog::WriteEvent(EventLogTags::BATTERY_STATUS,
            //    mBatteryStatus, mBatteryHealth, mBatteryPresent ? 1 : 0,
            //    mPlugType, mBatteryTechnology);
        }
        if (mBatteryLevel != mLastBatteryLevel ||
            mBatteryVoltage != mLastBatteryVoltage ||
            mBatteryTemperature != mLastBatteryTemperature) {
            //EventLog::WriteEvent(EventLogTags::BATTERY_LEVEL,
            //        mBatteryLevel, mBatteryVoltage, mBatteryTemperature);
        }
        if (mBatteryLevelCritical && !mLastBatteryLevelCritical &&
                mPlugType == BATTERY_PLUGGED_NONE) {
            // We want to make sure we log discharge cycle outliers
            // if the battery is about to die.
            dischargeDuration = SystemClock::GetElapsedRealtime() - mDischargeStartTime;
            logOutlier = TRUE;
        }

        const Boolean plugged = mPlugType != BATTERY_PLUGGED_NONE;
        const Boolean oldPlugged = mLastPlugType != BATTERY_PLUGGED_NONE;

        /* The ACTION_BATTERY_LOW broadcast is sent in these situations:
         * - is just un-plugged (previously was plugged) and battery level is
         *   less than or equal to WARNING, or
         * - is not plugged and battery level falls to WARNING boundary
         *   (becomes <= mLowBatteryWarningLevel).
         */
        const Boolean sendBatteryLow = !plugged
                && mBatteryStatus != IBatteryManager::BATTERY_STATUS_UNKNOWN
                && mBatteryLevel <= mLowBatteryWarningLevel
                && (oldPlugged || mLastBatteryLevel > mLowBatteryWarningLevel);

        SendIntentLocked();

        // Separate broadcast is sent for power connected / not connected
        // since the standard intent will not wake any applications and some
        // applications may want to have smart behavior based on this.
        Boolean b;
        if (mPlugType != 0 && mLastPlugType == 0) {
            AutoPtr<ActionPowerConnectedRunnable> run = new ActionPowerConnectedRunnable(this);
            mHandler->Post(run, &b);
        }
        else if (mPlugType == 0 && mLastPlugType != 0) {
            AutoPtr<ActionPowerDisconnectedRunnable> run = new ActionPowerDisconnectedRunnable(this);
            mHandler->Post(run, &b);
        }

        if (sendBatteryLow) {
            AutoPtr<ActionBatteryLowRunnable> run = new ActionBatteryLowRunnable(this);
            mHandler->Post(run, &b);
        }
        else if (mSentLowBatteryBroadcast && mLastBatteryLevel >= mLowBatteryCloseWarningLevel) {
            mSentLowBatteryBroadcast = FALSE;
            AutoPtr<ActionBatteryOkayRunnable> run = new ActionBatteryOkayRunnable(this);
            mHandler->Post(run, &b);
        }

        // Update the battery LED
        mLed->UpdateLightsLocked();

        // This needs to be done after sendIntent() so that we get the lastest battery stats.
        if (logOutlier && dischargeDuration != 0) {
            LogOutlierLocked(dischargeDuration);
        }

        mLastBatteryStatus = mBatteryStatus;
        mLastBatteryHealth = mBatteryHealth;
        mLastBatteryPresent = mBatteryPresent;
        mLastBatteryLevel = mBatteryLevel;
        mLastPlugType = mPlugType;
        mLastBatteryVoltage = mBatteryVoltage;
        mLastBatteryTemperature = mBatteryTemperature;
        mLastBatteryLevelCritical = mBatteryLevelCritical;
        mLastInvalidCharger = mInvalidCharger;
    }
}

void CBatteryService::SendIntentLocked()
{
    //  Pack up the values and broadcast them to everyone
    const AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_BATTERY_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);

    Int32 icon = GetIconLocked(mBatteryLevel);

    intent->PutExtra(IBatteryManager::EXTRA_STATUS, mBatteryStatus);
    intent->PutExtra(IBatteryManager::EXTRA_HEALTH, mBatteryHealth);
    intent->PutBooleanExtra(IBatteryManager::EXTRA_PRESENT, mBatteryPresent);
    intent->PutExtra(IBatteryManager::EXTRA_LEVEL, mBatteryLevel);
    intent->PutExtra(IBatteryManager::EXTRA_SCALE, BATTERY_SCALE);
    intent->PutExtra(IBatteryManager::EXTRA_ICON_SMALL, icon);
    intent->PutExtra(IBatteryManager::EXTRA_PLUGGED, mPlugType);
    intent->PutExtra(IBatteryManager::EXTRA_VOLTAGE, mBatteryVoltage);
    intent->PutExtra(IBatteryManager::EXTRA_TEMPERATURE, mBatteryTemperature);
    intent->PutExtra(IBatteryManager::EXTRA_TECHNOLOGY, mBatteryTechnology);
    intent->PutExtra(IBatteryManager::EXTRA_INVALID_CHARGER, mInvalidCharger);

    if (DEBUG) {
        StringBuilder sb("Sending ACTION_BATTERY_CHANGED.  level:");
        sb += mBatteryLevel;
        sb += ", scale:";
        sb += BATTERY_SCALE;
        sb += ", status:";
        sb += mBatteryStatus;
        sb += ", health:";
        sb += mBatteryHealth;
        sb += ", present:";
        sb += mBatteryPresent;
        sb += ", voltage: ";
        sb += mBatteryVoltage;
        sb += ", temperature: ";
        sb += mBatteryTemperature;
        sb += "technology: ";
        sb += mBatteryTechnology;
        sb += ", AC powered:";
        sb += mAcOnline;
        sb += ", USB powered:";
        sb += mUsbOnline;
        sb += ", Wireless powered:";
        sb += mWirelessOnline;
        sb += ", icon";
        sb += icon;
        sb += ", invalid charger:";
        sb += mInvalidCharger;
        Slogger::D(TAG, sb.ToString());
    }

    AutoPtr<SendIntentRunnable> run = new SendIntentRunnable(this, intent);
    Boolean b;
    mHandler->Post(run, &b);
}

void CBatteryService::LogBatteryStatsLocked()
{
    AutoPtr<IInterface> service = ServiceManager::GetService(BATTERY_STATS_SERVICE_NAME);
    AutoPtr<IBinder> batteryInfoService = IBinder::Probe(service);
    if (batteryInfoService == NULL) return;

    AutoPtr<IDropBoxManager> db;
    mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&db);
    Boolean isTagEnabled;
    ;
    if (db == NULL
            || (db->IsTagEnabled(String("BATTERY_DISCHARGE_INFO"), &isTagEnabled), !isTagEnabled)) {
        return;
    }

    AutoPtr<IFile> dumpFile;
    AutoPtr<IFileOutputStream> dumpStream;
    // try {
    // dump the service to a file
    StringBuilder sb(DUMPSYS_DATA_PATH);
    sb += BATTERY_STATS_SERVICE_NAME;
    sb += ".dump";
    CFile::New(sb.ToString(), (IFile**)&dumpFile);
    CFileOutputStream::New(dumpFile, (IFileOutputStream**)&dumpStream);
    AutoPtr<IFileDescriptor> fd;
    dumpStream->GetFD((IFileDescriptor**)&fd);
    // batteryInfoService->Dump(fd, DUMPSYS_ARGS);
    Boolean b;
    FileUtils::Sync(dumpStream, &b);

    // add dump file to drop box
    db->AddFile(String("BATTERY_DISCHARGE_INFO"), dumpFile, IDropBoxManager::IS_TEXT);
    // } catch (RemoteException e) {
    //    Slog.e(TAG, "failed to dump battery service", e);
    // } catch (IOException e) {
    //    Slog.e(TAG, "failed to write dumpsys file", e);
    // } finally {
    // make sure we clean up
    if (dumpStream != NULL) {
        //try {
        dumpStream->Close();
        //} catch (IOException e) {
        //    Slog.e(TAG, "failed to close dumpsys output stream");
        //}
    }
    Boolean isDelete;
    if (dumpFile != NULL && (dumpFile->Delete(&isDelete), !isDelete)) {
        StringBuilder sb("failed to delete temporary dumpsys file: ");
        String path;
        dumpFile->GetAbsolutePath(&path);
        sb += path;
        Slogger::E(TAG, sb.ToString());
    }
    //}
}

void CBatteryService::LogOutlierLocked(
    /* [in] */ Int64 duration)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String dischargeThresholdString;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetString(cr,
            ISettingsGlobal::BATTERY_DISCHARGE_THRESHOLD, &dischargeThresholdString);
    String durationThresholdString;
    settingsGlobal->GetString(cr,
            ISettingsGlobal::BATTERY_DISCHARGE_DURATION_THRESHOLD, &durationThresholdString);

    if (!dischargeThresholdString.IsNull() && !durationThresholdString.IsNull()) {
        //try {
        Int64 durationThreshold;
        Int32 dischargeThreshold;
        ECode ec = StringUtils::ParseInt64(durationThresholdString, &durationThreshold);
        if (FAILED(ec)) goto EXCEPTION;
        ec = StringUtils::ParseInt32(dischargeThresholdString, &dischargeThreshold);
        if (FAILED(ec)) goto EXCEPTION;
        if (duration <= durationThreshold &&
                mDischargeStartLevel - mBatteryLevel >= dischargeThreshold) {
            // If the discharge cycle is bad enough we want to know about it.
            LogBatteryStatsLocked();
        }
        if (DEBUG) {
            StringBuilder sb("duration threshold: ");
            sb += durationThreshold;
            sb += " discharge threshold: ";
            sb += dischargeThreshold;
            Slogger::V(TAG, sb.ToString());
        }
        if (DEBUG) {
            StringBuilder sb("duration: ");
            sb += duration;
            sb += " discharge: ";
            sb += (mDischargeStartLevel - mBatteryLevel);
            Slogger::V(TAG, sb.ToString());
        }
        //} catch (NumberFormatException e) {
        //    Slog.e(TAG, "Invalid DischargeThresholds GService string: " +
        //            durationThresholdString + " or " + dischargeThresholdString);
        //    return;
        //}
    }

EXCEPTION:
    Slogger::E(TAG, "Invalid DischargeThresholds GService string: %s or %s",
            durationThresholdString.string(), dischargeThresholdString.string());
    return;
}

Int32 CBatteryService::GetIconLocked(
    /* [in] */ Int32 level)
{
    if (mBatteryStatus == IBatteryManager::BATTERY_STATUS_CHARGING) {
        return R::drawable::stat_sys_battery_charge;
    }
    else if (mBatteryStatus == IBatteryManager::BATTERY_STATUS_DISCHARGING) {
        return R::drawable::stat_sys_battery;
    }
    else if (mBatteryStatus == IBatteryManager::BATTERY_STATUS_NOT_CHARGING
            || mBatteryStatus == IBatteryManager::BATTERY_STATUS_FULL) {
        if (IsPoweredLocked(IBatteryManager::BATTERY_PLUGGED_ANY)
                && mBatteryLevel >= 100) {
            return R::drawable::stat_sys_battery_charge;
        }
        else {
            return R::drawable::stat_sys_battery;
        }
    }
    else {
        return R::drawable::stat_sys_battery_unknown;
    }
}

ECode CBatteryService::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
