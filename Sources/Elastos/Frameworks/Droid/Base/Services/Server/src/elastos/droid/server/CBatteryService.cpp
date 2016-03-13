
#include "elastos/droid/server/CBatteryService.h"
#include "elastos/droid/server/CBatteryBinderService.h"
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/server/lights/LightsManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Os::EIID_IIBatteryPropertiesListener;
using Elastos::Droid::Os::EIID_IBatteryManagerInternal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Server::Am::CBatteryStatsService;
using Elastos::Droid::Server::Lights::LightsManager;
using Elastos::Droid::Server::Lights::EIID_ILightsManager;

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {


//=====================================================================
// CBatteryService::BootPhaseContentObserver
//=====================================================================
CBatteryService::BootPhaseContentObserver::BootPhaseContentObserver(
    /* [in] */ CBatteryService* host)
    : mHost(host)
{
}

// @Override
ECode CBatteryService::BootPhaseContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    synchronized(mHost->mLock) {
        mHost->UpdateBatteryWarningLevelLocked();
    }
    return NOERROR;
}

//=====================================================================
// CBatteryService::ShutdownIfNoPowerLockedRunnable
//=====================================================================
ECode CBatteryService::ShutdownIfNoPowerLockedRunnable::Run()
{
    if (ActivityManagerNative::IsSystemReady()) {
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
// CBatteryService::InvalidChargerObserver
//=====================================================================
ECode CBatteryService::InvalidChargerObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    String result;
    event->Get(String("SWITCH_STATE"), &result);
    Int32 invalidCharger = result.Equals("1") ? 1 : 0;
    AutoLock Lock(mHost->mLock);
    if (mHost->mInvalidCharger != invalidCharger) {
        mHost->mInvalidCharger = invalidCharger;
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService::Led
//=====================================================================
CBatteryService::Led::Led(
    /* [in] */ IContext* context,
    /* [in] */ ILightsManager* lights,
    /* [in] */ CBatteryService* host)
    : mHost(host)
{
    LightsManager* lm = (LightsManager*)lights;
    mBatteryLight = lm->GetLight(LightsManager::LIGHT_ID_BATTERY);

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
    Int32 level, status;
    mHost->mBatteryProps->GetBatteryLevel(&level);
    mHost->mBatteryProps->GetBatteryStatus(&status);
    if (level < mHost->mLowBatteryWarningLevel) {
        if (status == IBatteryManager::BATTERY_STATUS_CHARGING) {
            // Solid red when battery is charging
            mBatteryLight->SetColor(mBatteryLowARGB);
        }
        else {
            // Flash red when battery is low and not charging
            mBatteryLight->SetFlashing(mBatteryLowARGB, Light::LIGHT_FLASH_TIMED,
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
// CBatteryService::BatteryListener
//=====================================================================

CAR_INTERFACE_IMPL_2(CBatteryService::BatteryListener, Object, IIBatteryPropertiesListener, IBinder)

CBatteryService::BatteryListener::BatteryListener()
{}

ECode CBatteryService::BatteryListener::constructor(
    /* [in] */ ISystemService* batteryService)
{
    mHost = (CBatteryService*)batteryService;
    return NOERROR;
}

// //@Override
ECode CBatteryService::BatteryListener::BatteryPropertiesChanged(
    /* [in] */ IBatteryProperties* props)
{
    Int64 identity = Binder::ClearCallingIdentity();
    mHost->Update(props);
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CBatteryService::BatteryListener::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//=====================================================================
// CBatteryService::BinderService
//=====================================================================
CBatteryService::BinderService::BinderService()
{
}

ECode CBatteryService::BinderService::constructor(
    /* [in] */ ISystemService* batteryService)
{
    mHost = (CBatteryService*)batteryService;
    return NOERROR;
}

// //@Override
ECode CBatteryService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 permission;
    mHost->mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &permission);
    if (permission != IPackageManager::PERMISSION_GRANTED) {
        pw->Print(String("Permission Denial: can't dump Battery service from from pid="));
        pw->Print(Binder::GetCallingPid());
        pw->Print(String(", uid="));
        pw->Println(Binder::GetCallingUid());
        return NOERROR;
    }

    mHost->DumpInternal(pw, args);
    return NOERROR;
}

ECode CBatteryService::BinderService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//=====================================================================
// CBatteryService::LocalService
//=====================================================================

CAR_INTERFACE_IMPL(CBatteryService::LocalService, Object, IBatteryManagerInternal)

CBatteryService::LocalService::LocalService(
    /* [in] */ CBatteryService* host)
{
    mHost = host;
}

//@Override
ECode CBatteryService::LocalService::IsPowered(
    /* [in] */ Int32 plugTypeSet,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mLock) {
        *result = mHost->IsPoweredLocked(plugTypeSet);
    }
    return NOERROR;
}

//@Override
ECode CBatteryService::LocalService::GetPlugType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mLock) {
        *result = mHost->mPlugType;
    }
    return NOERROR;
}

//@Override
ECode CBatteryService::LocalService::GetBatteryLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mLock) {
        mHost->mBatteryProps->GetBatteryLevel(result);
    }
    return NOERROR;
}

//@Override
ECode CBatteryService::LocalService::GetBatteryLevelLow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mLock) {
        *result = mHost->mBatteryLevelLow;
    }
    return NOERROR;
}

//@Override
ECode CBatteryService::LocalService::GetInvalidCharger(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mLock) {
        *result = mHost->mInvalidCharger;
    }
    return NOERROR;
}


//=====================================================================
// CBatteryService
//=====================================================================
const String CBatteryService::TAG("CBatteryService");
const Boolean CBatteryService::DEBUG = FALSE;
const Int32 CBatteryService::BATTERY_SCALE;
const Int32 CBatteryService::DUMP_MAX_LENGTH;
const Int32 CBatteryService::BATTERY_PLUGGED_NONE;

static AutoPtr< ArrayOf<String> > InitDUMPSYS_ARGS()
{
    AutoPtr< ArrayOf<String> > marray = ArrayOf<String>::Alloc(2);
    marray->Set(0, String("--checkin"));
    marray->Set(1, String("--unplugged"));
    return marray;
}
const AutoPtr< ArrayOf<String> > CBatteryService::DUMPSYS_ARGS = InitDUMPSYS_ARGS();
const String CBatteryService::DUMPSYS_DATA_PATH("/data/system/");

CAR_OBJECT_IMPL(CBatteryService)

CBatteryService::CBatteryService()
    : mCriticalBatteryLevel(0)
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
    , mBatteryLevelLow(FALSE)
    , mDischargeStartTime(0)
    , mDischargeStartLevel(0)
    , mUpdatesStopped(FALSE)
    , mSentLowBatteryBroadcast(FALSE)
{
    mInvalidChargerObserver = new InvalidChargerObserver(this);
}

ECode CBatteryService::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context))

    mContext = context;
    CHandler::New(TRUE/*async*/, (IHandler**)&mHandler);

    AutoPtr<IInterface> lmObj = GetLocalService(EIID_ILightsManager);
    mLed = new Led(context, ILightsManager::Probe(lmObj), this);
    mBatteryStats = CBatteryStatsService::GetService();
    assert(0 && "TODO");
    // CBatteryProperties::New((IBatteryProperties**)&mLastBatteryProps);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_criticalBatteryWarningLevel, &mCriticalBatteryLevel);
    res->GetInteger(R::integer::config_lowBatteryWarningLevel, &mLowBatteryWarningLevel);
    res->GetInteger(R::integer::config_lowBatteryCloseWarningBump, &mLowBatteryCloseWarningLevel);
    mLowBatteryCloseWarningLevel += mLowBatteryWarningLevel;
    res->GetInteger(R::integer::config_shutdownBatteryTemperature, &mShutdownBatteryTemperature);

    // watch for invalid charger messages if the invalid_charger switch exists
    AutoPtr<IFile> f;
    CFile::New(String("/sys/devices/virtual/switch/invalid_charger/state"),(IFile**)&f);
    Boolean isExists;
    f->Exists(&isExists);
    if (isExists) {
        mInvalidChargerObserver->StartObserving(
            String("DEVPATH=/devices/virtual/switch/invalid_charger"));
    }

    return NOERROR;
}

// @Override
ECode CBatteryService::OnStart()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> obj;
    sm->GetService(String("batteryproperties"), (IInterface**)&obj);
    AutoPtr<IIBatteryPropertiesRegistrar> batteryPropertiesRegistrar =
        IIBatteryPropertiesRegistrar::Probe(obj);
    assert(0 && "TODO");
    // try {
    // batteryPropertiesRegistrar->RegisterListener(new BatteryListener());
    // } catch (RemoteException e) {
    //     // Should never happen.
    // }

    AutoPtr<IBinder> bindService;
    CBatteryBinderService::New(this, (IBinder**)&bindService);
    PublishBinderService(String("battery"), bindService);
    AutoPtr<LocalService> localService = new LocalService(this);
    PublishLocalService(EIID_IBatteryManagerInternal, TO_IINTERFACE(localService));
    return NOERROR;
}

// @Override
ECode CBatteryService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == PHASE_ACTIVITY_MANAGER_READY) {
        // check our power situation now that it is safe to display the shutdown dialog.
        synchronized(mLock) {
            AutoPtr<BootPhaseContentObserver> obs = new BootPhaseContentObserver(this);
            obs->constructor(mHandler);

            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            AutoPtr<IUri> uri;
            Settings::Global::GetUriFor(
                ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL, (IUri**)&uri);
            resolver->RegisterContentObserver(uri,
                FALSE, (ContentObserver*)obs.Get(), UserHandle::USER_ALL);
            UpdateBatteryWarningLevelLocked();
        }
    }
    return NOERROR;
}

ECode CBatteryService::UpdateBatteryWarningLevelLocked()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 defWarnLevel;
    res->GetInteger(R::integer::config_lowBatteryWarningLevel, &defWarnLevel);
    Settings::Global::GetInt32(resolver,
        ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL, defWarnLevel, &mLowBatteryWarningLevel);
    if (mLowBatteryWarningLevel == 0) {
        mLowBatteryWarningLevel = defWarnLevel;
    }
    if (mLowBatteryWarningLevel < mCriticalBatteryLevel) {
        mLowBatteryWarningLevel = mCriticalBatteryLevel;
    }
    res->GetInteger(R::integer::config_lowBatteryCloseWarningBump, &mLowBatteryCloseWarningLevel);
    mLowBatteryCloseWarningLevel += mLowBatteryWarningLevel;
    ProcessValuesLocked(TRUE);
    return NOERROR;
}

Boolean CBatteryService::IsPoweredLocked(
    /* [in] */ Int32 plugTypeSet)
{
    // assume we are powered if battery state is unknown so
    // the "stay on while plugged in" option will work.
    Int32 status;
    mBatteryProps->GetBatteryStatus(&status);
    if (status == IBatteryManager::BATTERY_STATUS_UNKNOWN) {
        return TRUE;
    }

    Boolean bval;
    mBatteryProps->GetChargerAcOnline(&bval);
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_AC) != 0 && bval) {
        return TRUE;
    }
    mBatteryProps->GetChargerUsbOnline(&bval);
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_USB) != 0 && bval) {
        return TRUE;
    }
    mBatteryProps->GetChargerWirelessOnline(&bval);
    if ((plugTypeSet & IBatteryManager::BATTERY_PLUGGED_WIRELESS) != 0 && bval) {
        return TRUE;
    }
    return FALSE;
}

Boolean CBatteryService::ShouldSendBatteryLowLocked()
{
    Boolean plugged = mPlugType != BATTERY_PLUGGED_NONE;
    Boolean oldPlugged = mLastPlugType != BATTERY_PLUGGED_NONE;

    /* The ACTION_BATTERY_LOW broadcast is sent in these situations:
     * - is just un-plugged (previously was plugged) and battery level is
     *   less than or equal to WARNING, or
     * - is not plugged and battery level falls to WARNING boundary
     *   (becomes <= mLowBatteryWarningLevel).
     */
    Int32 status, level;
    mBatteryProps->GetBatteryStatus(&status);
    mBatteryProps->GetBatteryLevel(&level);
    return !plugged
            && status != IBatteryManager::BATTERY_STATUS_UNKNOWN
            && level <= mLowBatteryWarningLevel
            && (oldPlugged || mLastBatteryLevel > mLowBatteryWarningLevel);
}

void CBatteryService::ShutdownIfNoPowerLocked()
{
    // shut down gracefully if our battery is critically low and we are not powered.
    // wait until the system has booted before attempting to display the shutdown dialog.
    Boolean isPoweredLocked = IsPoweredLocked(IBatteryManager::BATTERY_PLUGGED_ANY);
    Int32 batteryLevel;
    mBatteryProps->GetBatteryLevel(&batteryLevel);
    if (batteryLevel == 0 && !isPoweredLocked) {
        AutoPtr<ShutdownIfNoPowerLockedRunnable> run = new ShutdownIfNoPowerLockedRunnable(this);
        Boolean b;
        mHandler->Post(run, &b);
    }
}


void CBatteryService::ShutdownIfOverTempLocked()
{
    // shut down gracefully if temperature is too high (> 68.0C by default)
    // wait until the system has booted before attempting to display the
    // shutdown dialog.
    Int32 batteryTemperature;
    mBatteryProps->GetBatteryTemperature(&batteryTemperature);
    if (batteryTemperature > mShutdownBatteryTemperature) {
        AutoPtr<ShutdownIfOverTempRunnable> run = new ShutdownIfOverTempRunnable(this);
        Boolean b;
        mHandler->Post(run, &b);
    }
}

void CBatteryService::Update(
    /* [in] */ IBatteryProperties* props)
{
    synchronized (mLock) {
        if (!mUpdatesStopped) {
            mBatteryProps = props;
            // Process the new values.
            ProcessValuesLocked(FALSE);
        } else {
            mLastBatteryProps->Set(props);
        }
    }
}

void CBatteryService::ProcessValuesLocked(
    /* [in] */ Boolean force)
{
    Boolean logOutlier = FALSE;
    Int64 dischargeDuration = 0;

    Int32 status, health, level, temperature, voltage;
    Boolean present;
    mBatteryProps->GetBatteryLevel(&level);
    mBatteryProps->GetBatteryStatus(&status);
    mBatteryProps->GetBatteryHealth(&health);
    mBatteryProps->GetBatteryTemperature(&temperature);
    mBatteryProps->GetBatteryVoltage(&voltage);
    mBatteryProps->GetBatteryPresent(&present);

    mBatteryLevelCritical = (level <= mCriticalBatteryLevel);

    Boolean bval;
    if (mBatteryProps->GetChargerAcOnline(&bval), bval) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_AC;
    }
    if (mBatteryProps->GetChargerUsbOnline(&bval), bval) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_USB;
    }
    if (mBatteryProps->GetChargerWirelessOnline(&bval), bval) {
        mPlugType = IBatteryManager::BATTERY_PLUGGED_WIRELESS;
    }
    else {
        mPlugType = BATTERY_PLUGGED_NONE;
    }

    if (DEBUG) {
        Slogger::D(TAG,  "Processing new values: %s", TO_CSTR(mBatteryProps));
    }

    // TODO:
    // Let the battery stats keep track of the current level.
    // try {

    mBatteryStats->SetBatteryState(status, health,
        mPlugType, level, temperature, voltage);
    // } catch (RemoteException e) {
    //     Should never happen.
    // }

    ShutdownIfNoPowerLocked();
    ShutdownIfOverTempLocked();

    if (force || (status != mLastBatteryStatus ||
            health != mLastBatteryHealth ||
            present != mLastBatteryPresent ||
            level != mLastBatteryLevel ||
            mPlugType != mLastPlugType ||
            voltage != mLastBatteryVoltage ||
            temperature != mLastBatteryTemperature ||
            mInvalidCharger != mLastInvalidCharger)) {

        if (mPlugType != mLastPlugType) {
            if (mLastPlugType == BATTERY_PLUGGED_NONE) {
                // discharging -> charging

                // There's no value in this data unless we've discharged at least once and the
                // battery level has changed; so don't log until it does.
                if (mDischargeStartTime != 0 && mDischargeStartLevel != level) {
                    dischargeDuration = SystemClock::GetElapsedRealtime() - mDischargeStartTime;
                    logOutlier = TRUE;
                    //EventLog::WriteEvent(EventLogTags::BATTERY_DISCHARGE, dischargeDuration,
                    //        mDischargeStartLevel, level);
                    // make sure we see a discharge event before logging again
                    mDischargeStartTime = 0;
                }
            }
            else if (mPlugType == BATTERY_PLUGGED_NONE) {
                // charging -> discharging or we just powered up
                mDischargeStartTime = SystemClock::GetElapsedRealtime();
                mDischargeStartLevel = level;
            }
        }
        if (status != mLastBatteryStatus ||
            health != mLastBatteryHealth ||
            present != mLastBatteryPresent ||
            mPlugType != mLastPlugType) {
            //EventLog::WriteEvent(EventLogTags::BATTERY_STATUS,
            //    status, health, present ? 1 : 0,
            //    mPlugType, mBatteryTechnology);
        }
        if (level != mLastBatteryLevel) {
            // Don't do this just from voltage or temperature changes, that is
            // too noisy.
            //EventLog::WriteEvent(EventLogTags::BATTERY_LEVEL,
            //        level, mBatteryVoltage, mBatteryTemperature);
        }
        if (mBatteryLevelCritical && !mLastBatteryLevelCritical &&
                mPlugType == BATTERY_PLUGGED_NONE) {
            // We want to make sure we log discharge cycle outliers
            // if the battery is about to die.
            dischargeDuration = SystemClock::GetElapsedRealtime() - mDischargeStartTime;
            logOutlier = TRUE;
        }

        if (!mBatteryLevelLow) {
            // Should we now switch in to low battery mode?
            if (mPlugType == BATTERY_PLUGGED_NONE
                    && level <= mLowBatteryWarningLevel) {
                mBatteryLevelLow = TRUE;
            }
        }
        else {
            // Should we now switch out of low battery mode?
            if (mPlugType != BATTERY_PLUGGED_NONE) {
                mBatteryLevelLow = FALSE;
            }
            else if (level >= mLowBatteryCloseWarningLevel)  {
                mBatteryLevelLow = FALSE;
            }
            else if (force && level >= mLowBatteryWarningLevel) {
                // If being forced, the previous state doesn't matter, we will just
                // absolutely check to see if we are now above the warning level.
                mBatteryLevelLow = FALSE;
            }
        }

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

        if (ShouldSendBatteryLowLocked()) {
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

        mLastBatteryStatus = status;
        mLastBatteryHealth = health;
        mLastBatteryPresent = present;
        mLastBatteryLevel = level;
        mLastPlugType = mPlugType;
        mLastBatteryVoltage = voltage;
        mLastBatteryTemperature = temperature;
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

    Boolean present;
    Int32 status, health, level, temperature, voltage, technology;
    mBatteryProps->GetBatteryLevel(&level);
    mBatteryProps->GetBatteryStatus(&status);
    mBatteryProps->GetBatteryHealth(&health);
    mBatteryProps->GetBatteryTemperature(&temperature);
    mBatteryProps->GetBatteryVoltage(&voltage);
    mBatteryProps->GetBatteryPresent(&present);
    mBatteryProps->GetBatteryTechnology(&technology);

    Int32 icon = GetIconLocked(level);

    intent->PutExtra(IBatteryManager::EXTRA_STATUS, status);
    intent->PutExtra(IBatteryManager::EXTRA_HEALTH, health);
    intent->PutBooleanExtra(IBatteryManager::EXTRA_PRESENT, present);
    intent->PutExtra(IBatteryManager::EXTRA_LEVEL, level);
    intent->PutExtra(IBatteryManager::EXTRA_SCALE, BATTERY_SCALE);
    intent->PutExtra(IBatteryManager::EXTRA_ICON_SMALL, icon);
    intent->PutExtra(IBatteryManager::EXTRA_PLUGGED, mPlugType);
    intent->PutExtra(IBatteryManager::EXTRA_VOLTAGE, voltage);
    intent->PutExtra(IBatteryManager::EXTRA_TEMPERATURE, temperature);
    intent->PutExtra(IBatteryManager::EXTRA_TECHNOLOGY, technology);
    intent->PutExtra(IBatteryManager::EXTRA_INVALID_CHARGER, mInvalidCharger);

    if (DEBUG) {
        Slogger::D(TAG, "Sending ACTION_BATTERY_CHANGED. icon:%d, invalid charger:%d. %s",
            icon, mInvalidCharger, TO_CSTR(mBatteryProps));
    }

    AutoPtr<SendIntentRunnable> run = new SendIntentRunnable(this, intent);
    Boolean b;
    mHandler->Post(run, &b);
}

void CBatteryService::LogBatteryStatsLocked()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> service;
    sm->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&service);
    AutoPtr<IBinder> batteryInfoService = IBinder::Probe(service);
    if (batteryInfoService == NULL) return;

    service = NULL;
    mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&service);
    AutoPtr<IDropBoxManager> db = IDropBoxManager::Probe(service);
    Boolean isTagEnabled;
    if (db == NULL
            || (db->IsTagEnabled(String("BATTERY_DISCHARGE_INFO"), &isTagEnabled), !isTagEnabled)) {
        return;
    }

    AutoPtr<IFile> dumpFile;
    AutoPtr<IFileOutputStream> dumpStream;
    // try {
    // dump the service to a file
    StringBuilder sb(DUMPSYS_DATA_PATH);
    sb += IBatteryStats::SERVICE_NAME;
    sb += ".dump";
    CFile::New(sb.ToString(), (IFile**)&dumpFile);
    CFileOutputStream::New(dumpFile, (IFileOutputStream**)&dumpStream);
    AutoPtr<IFileDescriptor> fd;
    dumpStream->GetFD((IFileDescriptor**)&fd);
    // batteryInfoService->Dump(fd, DUMPSYS_ARGS);
    FileUtils::Sync(dumpStream);

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
        ICloseable::Probe(dumpStream)->Close();
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
    Int32  level;
    mBatteryProps->GetBatteryLevel(&level);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String dischargeThresholdString;
    Settings::Global::GetString(cr,
            ISettingsGlobal::BATTERY_DISCHARGE_THRESHOLD, &dischargeThresholdString);
    String durationThresholdString;
    Settings::Global::GetString(cr,
            ISettingsGlobal::BATTERY_DISCHARGE_DURATION_THRESHOLD, &durationThresholdString);

    if (!dischargeThresholdString.IsNull() && !durationThresholdString.IsNull()) {
        //try {
        Int64 durationThreshold;
        Int32 dischargeThreshold;
        ECode ec = StringUtils::Parse(durationThresholdString, &durationThreshold);
        if (FAILED(ec)) goto EXCEPTION;
        ec = StringUtils::Parse(dischargeThresholdString, &dischargeThreshold);
        if (FAILED(ec)) goto EXCEPTION;
        if (duration <= durationThreshold &&
                mDischargeStartLevel - level >= dischargeThreshold) {
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
            sb += (mDischargeStartLevel - level);
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
    /* [in] */ Int32 inLevel)
{
    Int32 status, level;
    mBatteryProps->GetBatteryLevel(&level);
    mBatteryProps->GetBatteryStatus(&status);

    if (status == IBatteryManager::BATTERY_STATUS_CHARGING) {
        return R::drawable::stat_sys_battery_charge;
    }
    else if (status == IBatteryManager::BATTERY_STATUS_DISCHARGING) {
        return R::drawable::stat_sys_battery;
    }
    else if (status == IBatteryManager::BATTERY_STATUS_NOT_CHARGING
            || status == IBatteryManager::BATTERY_STATUS_FULL) {
        if (IsPoweredLocked(IBatteryManager::BATTERY_PLUGGED_ANY)
                && level >= 100) {
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
    return Object::ToString(str);
}

ECode CBatteryService::DumpInternal(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // synchronized (mLock) {
    //     if (args == null || args.length == 0 || "-a".equals(args[0])) {
    //         pw.println("Current Battery Service state:");
    //         if (mUpdatesStopped) {
    //             pw.println("  (UPDATES STOPPED -- use 'reset' to restart)");
    //         }
    //         pw.println("  AC powered: " + mBatteryProps.chargerAcOnline);
    //         pw.println("  USB powered: " + mBatteryProps.chargerUsbOnline);
    //         pw.println("  Wireless powered: " + mBatteryProps.chargerWirelessOnline);
    //         pw.println("  status: " + mBatteryProps.batteryStatus);
    //         pw.println("  health: " + mBatteryProps.batteryHealth);
    //         pw.println("  present: " + mBatteryProps.batteryPresent);
    //         pw.println("  level: " + mBatteryProps.batteryLevel);
    //         pw.println("  scale: " + BATTERY_SCALE);
    //         pw.println("  voltage: " + mBatteryProps.batteryVoltage);
    //         pw.println("  temperature: " + mBatteryProps.batteryTemperature);
    //         pw.println("  technology: " + mBatteryProps.batteryTechnology);
    //     } else if (args.length == 3 && "set".equals(args[0])) {
    //         String key = args[1];
    //         String value = args[2];
    //         try {
    //             if (!mUpdatesStopped) {
    //                 mLastBatteryProps.set(mBatteryProps);
    //             }
    //             boolean update = true;
    //             if ("ac".equals(key)) {
    //                 mBatteryProps.chargerAcOnline = Integer.parseInt(value) != 0;
    //             } else if ("usb".equals(key)) {
    //                 mBatteryProps.chargerUsbOnline = Integer.parseInt(value) != 0;
    //             } else if ("wireless".equals(key)) {
    //                 mBatteryProps.chargerWirelessOnline = Integer.parseInt(value) != 0;
    //             } else if ("status".equals(key)) {
    //                 mBatteryProps.batteryStatus = Integer.parseInt(value);
    //             } else if ("level".equals(key)) {
    //                 mBatteryProps.batteryLevel = Integer.parseInt(value);
    //             } else if ("invalid".equals(key)) {
    //                 mInvalidCharger = Integer.parseInt(value);
    //             } else {
    //                 pw.println("Unknown set option: " + key);
    //                 update = false;
    //             }
    //             if (update) {
    //                 long ident = Binder.clearCallingIdentity();
    //                 try {
    //                     mUpdatesStopped = true;
    //                     processValuesLocked(false);
    //                 } finally {
    //                     Binder.restoreCallingIdentity(ident);
    //                 }
    //             }
    //         } catch (NumberFormatException ex) {
    //             pw.println("Bad value: " + value);
    //         }
    //     } else if (args.length == 1 && "reset".equals(args[0])) {
    //         long ident = Binder.clearCallingIdentity();
    //         try {
    //             if (mUpdatesStopped) {
    //                 mUpdatesStopped = false;
    //                 mBatteryProps.set(mLastBatteryProps);
    //                 processValuesLocked(false);
    //             }
    //         } finally {
    //             Binder.restoreCallingIdentity(ident);
    //         }
    //     } else {
    //         pw.println("Dump current battery state, or:");
    //         pw.println("  set [ac|usb|wireless|status|level|invalid] <value>");
    //         pw.println("  reset");
    //     }
    // }
    return NOERROR;
}


} // namespace Server
} // namespace Droid
} // namespace Elastos
