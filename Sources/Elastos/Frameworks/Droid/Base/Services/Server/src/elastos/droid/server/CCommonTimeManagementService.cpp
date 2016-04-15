#include "elastos/droid/server/CCommonTimeManagementService.h"
#include "elastos/droid/server/CCommonTimeManagementServiceIfaceObserver.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
//using Elastos::Droid::Os::ICommonTimeConfig;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::EIID_IINetworkManagementEventObserver;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

CCommonTimeManagementService::StaticInitializer::StaticInitializer()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Int32 value;
    sysProp->GetInt32(CCommonTimeManagementService::AUTO_DISABLE_PROP, 1, &value);
    CCommonTimeManagementService::AUTO_DISABLE = (0 != value);
    sysProp->GetInt32(CCommonTimeManagementService::ALLOW_WIFI_PROP, 0, &value);
    CCommonTimeManagementService::ALLOW_WIFI = (0 != value);
    Int32 tmp;
    sysProp->GetInt32(CCommonTimeManagementService::NO_INTERFACE_TIMEOUT_PROP, 60000, &tmp);
    CCommonTimeManagementService::NO_INTERFACE_TIMEOUT = tmp;

    sysProp->GetInt32(CCommonTimeManagementService::SERVER_PRIO_PROP, 1, &tmp);
    if (tmp < 1) {
        CCommonTimeManagementService::BASE_SERVER_PRIO = 1;
    }
    else if (tmp > 30) {
        CCommonTimeManagementService::BASE_SERVER_PRIO = 30;
    }
    else {
        CCommonTimeManagementService::BASE_SERVER_PRIO = (Byte)tmp;
    }

    if (CCommonTimeManagementService::ALLOW_WIFI) {
        IFACE_SCORE_RULES = ArrayOf<CCommonTimeManagementService::InterfaceScoreRule*>::Alloc(2);
        AutoPtr<CCommonTimeManagementService::InterfaceScoreRule> isr1, isr2;
        isr1 = new CCommonTimeManagementService::InterfaceScoreRule(String("wlan"), (Byte)1);
        isr2 = new CCommonTimeManagementService::InterfaceScoreRule(String("eth"), (Byte)2);
        IFACE_SCORE_RULES->Set(0, isr1);
        IFACE_SCORE_RULES->Set(1, isr2);
    }
    else {
        IFACE_SCORE_RULES = ArrayOf<CCommonTimeManagementService::InterfaceScoreRule* >::Alloc(1);
        AutoPtr<CCommonTimeManagementService::InterfaceScoreRule> isr1;
        isr1 = new CCommonTimeManagementService::InterfaceScoreRule(String("eth"), (Byte)2);
        IFACE_SCORE_RULES->Set(0, isr1);
    }
}


const String CCommonTimeManagementService::TAG("CCommonTimeManagementService");
const Int32 CCommonTimeManagementService::NATIVE_SERVICE_RECONNECT_TIMEOUT;
const String CCommonTimeManagementService::AUTO_DISABLE_PROP("ro.common_time.auto_disable");
const String CCommonTimeManagementService::ALLOW_WIFI_PROP("ro.common_time.allow_wifi");
const String CCommonTimeManagementService::SERVER_PRIO_PROP("ro.common_time.server_prio");
const String CCommonTimeManagementService::NO_INTERFACE_TIMEOUT_PROP("ro.common_time.no_iface_timeout");
Boolean CCommonTimeManagementService::AUTO_DISABLE = FALSE;
Boolean CCommonTimeManagementService::ALLOW_WIFI = FALSE;
Byte CCommonTimeManagementService::BASE_SERVER_PRIO = 0x0;
Int32 CCommonTimeManagementService::NO_INTERFACE_TIMEOUT = 0;
AutoPtr<ArrayOf<CCommonTimeManagementService::InterfaceScoreRule *> > CCommonTimeManagementService::IFACE_SCORE_RULES;
const CCommonTimeManagementService::StaticInitializer CCommonTimeManagementService::sInitializer;

//====================================================================
// CCommonTimeManagementService::ReconnectRunnable
//====================================================================
ECode CCommonTimeManagementService::ReconnectRunnable::Run()
{
    mHost->ConnectToTimeConfig();
    return NOERROR;
}

//====================================================================
// CCommonTimeManagementService::NoInterfaceRunnable
//====================================================================
ECode CCommonTimeManagementService::NoInterfaceRunnable::Run()
{
    mHost->HandleNoInterfaceTimeout();
    return NOERROR;
}

//====================================================================
// CCommonTimeManagementService::IfaceObserver
//====================================================================
CCommonTimeManagementService::IfaceObserver::IfaceObserver()
{
}

ECode CCommonTimeManagementService::IfaceObserver::constructor(
    /* [in] */ IBinder* commonTimeManagementService)
{
    FAIL_RETURN(BaseNetworkObserver::constructor())

    mHost = (CCommonTimeManagementService*)commonTimeManagementService;
    return NOERROR;
}

ECode CCommonTimeManagementService::IfaceObserver::InterfaceStatusChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    mHost->ReevaluateServiceState();
    return NOERROR;
}

ECode CCommonTimeManagementService::IfaceObserver::InterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    mHost->ReevaluateServiceState();
    return NOERROR;
}

ECode CCommonTimeManagementService::IfaceObserver::InterfaceAdded(
    /* [in] */ const String& iface)
{
    mHost->ReevaluateServiceState();
    return NOERROR;
}

ECode CCommonTimeManagementService::IfaceObserver::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    mHost->ReevaluateServiceState();
    return NOERROR;
}

//====================================================================
// CCommonTimeManagementService::ConnectivityMangerObserver
//====================================================================
CCommonTimeManagementService::ConnectivityMangerObserver::ConnectivityMangerObserver(
    /* [in] */ CCommonTimeManagementService* owner)
{
    mHost = owner;
}

ECode CCommonTimeManagementService::ConnectivityMangerObserver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->ReevaluateServiceState();
    return NOERROR;
}


//====================================================================
// CCommonTimeManagementService::CTServerDiedListener
//====================================================================
CCommonTimeManagementService::CTServerDiedListener::CTServerDiedListener(
    /* [in] */ CCommonTimeManagementService* owner)
{
    mHost = owner;
}

ECode CCommonTimeManagementService::CTServerDiedListener::OnServerDied()
{
    mHost->ScheduleTimeConfigReconnect();
    return NOERROR;
}


//====================================================================
// CCommonTimeManagementService::InterfaceScoreRule
//====================================================================
CCommonTimeManagementService::InterfaceScoreRule::InterfaceScoreRule(
    /* [in] */ const String& prefix,
    /* [in] */ Byte score)
    : mPrefix(prefix)
    , mScore(score)
{
}


//====================================================================
// CCommonTimeManagementService
//====================================================================
CAR_INTERFACE_IMPL(CCommonTimeManagementService, Object, IBinder)

CAR_OBJECT_IMPL(CCommonTimeManagementService)

CCommonTimeManagementService::CCommonTimeManagementService()
    : mDetectedAtStartup(FALSE)
    , mEffectivePrio(BASE_SERVER_PRIO)
{
}

ECode CCommonTimeManagementService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHandler::New((IHandler**)&mReconnectHandler);
    CHandler::New((IHandler**)&mNoInterfaceHandler);

    AutoPtr<IINetworkManagementEventObserver> nmeo;
    CCommonTimeManagementServiceIfaceObserver::New(this, (IINetworkManagementEventObserver**)&nmeo);
    mIfaceObserver = (IfaceObserver*)nmeo.Get();
    mConnectivityMangerObserver = new ConnectivityMangerObserver(this);

    mCTServerDiedListener = new CTServerDiedListener(this);

    return NOERROR;
}

ECode CCommonTimeManagementService::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCommonTimeManagementService::SystemReady()
{
    String CommonTimeConfig_SERVICE_NAME("common_time.config");
    AutoPtr<IInterface> service;
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    assert(0 && "TODO");
    // serviceManager::CheckService(ICommonTimeConfig::SERVICE_NAME, (IInterface**)&service);
    if (service == NULL) {
        Logger::I(TAG, "No common time service detected on this platform.  ", "Common time services will be unavailable.");
        return E_NULL_POINTER_EXCEPTION;
    }
    mDetectedAtStartup = TRUE;

    AutoPtr<IInterface> tmpObj;
    serviceManager->GetService(IContext::NETWORKMANAGEMENT_SERVICE, (IInterface**)&tmpObj);
    mNetMgr = IINetworkManagementService::Probe(tmpObj.Get());
    assert(mNetMgr != NULL);

    // Network manager is running along-side us, so we should never receiver a remote exception
    // while trying to register this observer.
    ECode ec = mNetMgr->RegisterObserver(mIfaceObserver);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }

    // Register with the connectivity manager for connectivity changed intents.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver((IBroadcastReceiver*)mConnectivityMangerObserver, filter, (IIntent**)&intent);

    // Connect to the common time config service and apply the initial configuration.
    ConnectToTimeConfig();

    return NOERROR;
}

ECode CCommonTimeManagementService::Dump(
    /* [in] */ IFileDescriptor *fd,
    /* [in] */ IPrintWriter *pw,
    /* [in] */ ArrayOf<String> *args)
{
    Int32 result = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("Permission Denial: can't dump CommonTimeManagement service from from ");
        sb += "pid=";
        sb += Binder::GetCallingPid();
        sb += ", uid=";
        sb += Binder::GetCallingUid();
        pw->Println(sb.ToString());
        return -1;
    }

    if (!mDetectedAtStartup) {
        pw->Println(String("Native Common Time service was not detected at startup.  Service is unavailable"));
        return -2;
    }

    AutoLock Lock(mLock);
    pw->Println(String("Current Common Time Management Service Config:"));

    StringBuilder builder("  Native service     : ");
//TODO
/*
    if (NULL == mCTConfig) {
        builder += "reconnecting";
    }
    else
    {
        builder += "alive";
    }
*/
    pw->Println(builder.ToString());

    StringBuilder builderTwo("  Bound interface    : ");
    if (mCurIface.IsNull())
        builderTwo += "unbound";
    else
        builderTwo += mCurIface;
    pw->Println(builderTwo.ToString());

    StringBuilder builderThr("  Allow WiFi         : ");
    if (ALLOW_WIFI)
        builderThr += "yes";
    else
        builderThr += "no";
    pw->Println(builderThr.ToString());

    StringBuilder builderFour("  Allow Auto Disable : ");
    if (AUTO_DISABLE)
        builderFour += "yes";
    else
        builderFour += "no";
    pw->Println(builderFour.ToString());

    StringBuilder builderFive("  Server Priority    : ");
    builderFive += mEffectivePrio;
    pw->Println(builderFive.ToString());

    StringBuilder builderSix("  No iface timeout    : ");
    builderSix += NO_INTERFACE_TIMEOUT;
    pw->Println(builderSix.ToString());

    return NOERROR;
}

void CCommonTimeManagementService::CleanupTimeConfig()
{
    mReconnectHandler->RemoveCallbacks(mReconnectRunnable);
    mNoInterfaceHandler->RemoveCallbacks(mNoInterfaceRunnable);

//TODO
/*
    if (null != mCTConfig) {
        mCTConfig.release();
        mCTConfig = null;
    }
*/
}

void CCommonTimeManagementService::ConnectToTimeConfig()
{
    // Get access to the common time service configuration interface.  If we catch a remote
    // exception in the process (service crashed or no running for w/e reason), schedule an
    // attempt to reconnect in the future.
    CleanupTimeConfig();
    //try {
    AutoLock lock(mLock);
    //TODO
    /*
    mCTConfig = new CommonTimeConfig();
    mCTConfig.setServerDiedListener(mCTServerDiedListener);
    mCurIface = mCTConfig.getInterfaceBinding();
    mCTConfig.setAutoDisable(AUTO_DISABLE);
    mCTConfig.setMasterElectionPriority(mEffectivePrio);
    */

    if (NO_INTERFACE_TIMEOUT >= 0) {
        Boolean result;
        mNoInterfaceHandler->PostDelayed(mNoInterfaceRunnable, NO_INTERFACE_TIMEOUT, &result);
    }

    ReevaluateServiceState();
    //}
    //catch (RemoteException e) {
    //    scheduleTimeConfigReconnect();
    //}
}

void CCommonTimeManagementService::ScheduleTimeConfigReconnect()
{
    CleanupTimeConfig();
    Logger::W(TAG, "Native service died, will reconnect in %d mSec", NATIVE_SERVICE_RECONNECT_TIMEOUT);

    Boolean result;
    mReconnectHandler->PostDelayed(mReconnectRunnable, NATIVE_SERVICE_RECONNECT_TIMEOUT, &result);
}

void CCommonTimeManagementService::HandleNoInterfaceTimeout()
{
    //TODO
    /*
    if (NULL != mCTConfig) {
        Logger::I(TAG, "Timeout waiting for interface to come up.  Forcing networkless master mode.");
        Int32 result;
        mCTConfig->ForceNetworklessMasterMode(&result);
        if (ICommonTimeConfig::ERROR_DEAD_OBJECT == result)
            ScheduleTimeConfigReconnect();
    }
    */
}

void CCommonTimeManagementService::ReevaluateServiceState()
{
    String bindIface;
    Byte bestScore = -1;
    //try {
        AutoPtr<ArrayOf<String> > ifaceList;
        mNetMgr->ListInterfaces((ArrayOf<String>**)&ifaceList);

        if (NULL != ifaceList) {
            Int32 len1 = ifaceList->GetLength();
            Int32 len2 = IFACE_SCORE_RULES->GetLength();
            String iface;
            AutoPtr<InterfaceScoreRule> r;

            for (Int32 i = 0; i < len1; i++) {
                Byte thisScore = -1;
                iface = (*ifaceList)[i];

                for (Int32 j = 0; j < len2; j++) {
                    r = (*IFACE_SCORE_RULES)[j];
                    if (iface.Contains(r->mPrefix)) {
                        thisScore = r->mScore;
                        break;
                    }
                }

                if (thisScore <= bestScore)
                    continue;

                AutoPtr<IInterfaceConfiguration> config;
                mNetMgr->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config);

                if (NULL == config)
                    continue;

                Boolean isActive = FALSE;
                config->IsActive(&isActive);
                if (isActive) {
                    bindIface = iface;
                    bestScore = thisScore;
                }
            }
        }
    //}
    //catch (RemoteException e) {
    //    bindIface = null;
    //}

    Boolean doRebind = TRUE;
    AutoLock Lock(mLock);
    if ((!bindIface.IsNull()) && (mCurIface.IsNull())) {
        Logger::E(TAG, "Binding common time service to %s.", bindIface.string());
        mCurIface = bindIface;
    }
    else if ((bindIface.IsNull()) && (!mCurIface.IsNull())) {
        Logger::E(TAG, "Unbinding common time service.");
        mCurIface = String(NULL);
    }
    else if ((!bindIface.IsNull()) && (!mCurIface.IsNull()) && !bindIface.Equals(mCurIface)) {
        Logger::E(TAG, "Switching common time service binding from %s to %s.",
                                 mCurIface.string(), bindIface.string());
        mCurIface = bindIface;
    }
    else {
        doRebind = FALSE;
    }

    assert(0 && "TODO");
//TODO
/*
    if (doRebind && (NULL != mCTConfig)) {
        Byte newPrio = (bestScore > 0)
                     ? (Byte)(bestScore * BASE_SERVER_PRIO)
                     : BASE_SERVER_PRIO;
        if (newPrio != mEffectivePrio) {
            mEffectivePrio = newPrio;
            Int32 result = 0;
            mCTConfig->SetMasterElectionPriority(mEffectivePrio, &result);
        }

        Int32 res = 0;
        mCTConfig->SetNetworkBinding(mCurIface, &res);
        if (res != ICommonTimeConfig::SUCCESS)
            ScheduleTimeConfigReconnect();

        else if (NO_INTERFACE_TIMEOUT >= 0) {
            mNoInterfaceHandler->RemoveCallbacks(mNoInterfaceRunnable);
            if (mCurIface.IsNull()) {
                Boolean result;
                mNoInterfaceHandler->PostDelayed(mNoInterfaceRunnable, NO_INTERFACE_TIMEOUT, &result);
            }
        }
    }
*/
}

}//namespace Server
}//namespace Droid
}//namespace Elastos
