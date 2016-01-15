
#include "CGeckoNetworkManager.h"
#include "GeckoApp.h"
#include "GeckoAppShell.h"
#include "elastos/droid/ext/frameworkext.h"
#include "stdio.h"

using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

#define kDefaultBandwidth -1.0
#define kDefaultCanBeMetered false
#define kMaxBandwidth 20.0

#define kNetworkSpeedEthernet 20.0 // 20 Mb/s
#define kNetworkSpeedWifi 20.0 // 20 Mb/s
#define kNetworkSpeedWiMax 40.0 // 40 Mb/s
#define kNetworkSpeed_2_G 15.0 / 1024.0 // 15 kb/s
#define kNetworkSpeed_2_5_G 60.0 / 1024.0 // 60 kb/s
#define kNetworkSpeed_2_75_G 200.0 / 1024.0 // 200 kb/s
#define kNetworkSpeed_3_G 300.0 / 1024.0 // 300 kb/s
#define kNetworkSpeed_3_5_G 7.0 // 7 Mb/s
#define kNetworkSpeed_3_75_G 20.0 // 20 Mb/s
#define kNetworkSpeed_3_9_G 50.0 // 50 Mb/s


CGeckoNetworkManager* InitInstance()
{
    CGeckoNetworkManager* pInstance;
    CGeckoNetworkManager::NewByFriend((CGeckoNetworkManager**)&pInstance);
    return pInstance;
}

AutoPtr<CGeckoNetworkManager> CGeckoNetworkManager::sInstance = InitInstance();

CGeckoNetworkManager::CGeckoNetworkManager():
    mNetworkType(NETWORK_NONE),
    mShouldBeListening(FALSE),
    mShouldNotify(FALSE)
{

}

ECode CGeckoNetworkManager::OnRecieve(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent)
{
    FAIL_RETURN(UpdateNetworkType());
    return NOERROR;
}

CGeckoNetworkManager* CGeckoNetworkManager::GetInstance()
{
    return sInstance;
}

ECode CGeckoNetworkManager::Init()
{
    FAIL_RETURN(mNetworkFilter->AddAction(String(IConnectivityManager::CONNECTIVITY_ACTION)));
    mNetworkType = GetNetworkType();
    return NOERROR;
}

ECode CGeckoNetworkManager::Start()
{
    mShouldBeListening = TRUE;

    if (mShouldNotify) {
        FAIL_RETURN(StartListening());
    }
    return NOERROR;
}

ECode CGeckoNetworkManager::Stop()
{
    mShouldBeListening = FALSE;

    if (mShouldNotify) {
        FAIL_RETURN(StopListening());
    }
    return NOERROR;
}

ECode CGeckoNetworkManager::GetCurrentInformation(
    /* [out, callee] */ ArrayOf<Double> ** ppInfo)
{
    *ppInfo = ArrayOf<Double>::Alloc(2);
    (*(*ppInfo))[0] = GetNetworkSpeed(mNetworkType);
    (*(*ppInfo))[1] = IsNetworkUsuallyMetered(mNetworkType) ? 1.0 : 0.0;
    return NOERROR;
}

ECode CGeckoNetworkManager::EnableNotifications()
{
    // We set mShouldNotify *after* calling updateNetworkType() to make sure we
    // don't notify an eventual change in mNetworkType.
    FAIL_RETURN(UpdateNetworkType());
    mShouldNotify = true;

    if (mShouldBeListening) {
      FAIL_RETURN(StartListening());
    }

    return NOERROR;
}

ECode CGeckoNetworkManager::DisableNotifications()
{
    mShouldNotify = FALSE;

    if (mShouldBeListening) {
      FAIL_RETURN(StopListening());
    }

    return NOERROR;
}

ECode CGeckoNetworkManager::StartListening()
{
    return E_NOT_IMPLEMENTED;
    //return GeckoApp::sAppContext->RegisterReceiver(sInstance, mNetworkFilter);
}

ECode CGeckoNetworkManager::StopListening()
{
    return E_NOT_IMPLEMENTED;
    //return GeckoApp::sAppContext->UnregisterReceiver(sInstance);
}

ECode CGeckoNetworkManager::UpdateNetworkType()
{
    NetworkType previousNetworkType = mNetworkType;
    mNetworkType = GetNetworkType();

    if (mNetworkType == previousNetworkType || !mShouldNotify) {
        return NOERROR;
    }

    return GeckoAppShell::SendEventToGecko(new GeckoEvent(GetNetworkSpeed(mNetworkType),
                                              IsNetworkUsuallyMetered(mNetworkType)));
}


CGeckoNetworkManager::NetworkType CGeckoNetworkManager::GetNetworkType()
{
    // AutoPtr<IConnectivityManager> cm;
    // GeckoApp::sAppContext->GetSystemService(String(IContext::CONNECTIVITY_SERVICE),
    //   (IInterface**)&cm);

    // AutoPtr<INetworkInfo> netInfo;
    // cm->GetActiveNetworkInfo((INetworkInfo**)&netInfo);

    // if (netInfo == NULL) {
    //   return NETWORK_NONE;
    // }

    // Int32 type;
    // netInfo->GetType(&type);

    // switch (type) {
    //   case IConnectivityManager::TYPE_ETHERNET:
    //     return NETWORK_ETHERNET;
    //   case IConnectivityManager::TYPE_WIFI:
    //     return NETWORK_WIFI;
    //   case IConnectivityManager::TYPE_WIMAX:
    //     return NETWORK_WIMAX;
    //   case IConnectivityManager::TYPE_MOBILE:
    //     break; // We will handle sub-types after the switch.
    //   default:
    //     printf("GeckoNetworkManager"  "Ignoring the current network type.");
    //     return NETWORK_UNKNOWN;
    // }
    return NETWORK_WIFI;

/*
    AutoPtr<ITelephonyManager> tm;
    GeckoApp::sAppContext->GetSystemService(Context_TELEPHONY_SERVICE, (ITelephonyManager**)&tm);

    tm->GetNetworkType(type);

    switch (type) {
      case TelephonyManager_NETWORK_TYPE_IDEN:
      case TelephonyManager_NETWORK_TYPE_CDMA:
        return NetworkType_NETWORK_2_G;
      case TelephonyManager_NETWORK_TYPE_GPRS:
      case TelephonyManager_NETWORK_TYPE_1xRTT:
        return NetworkType_NETWORK_2_5_G;
      case TelephonyManager_NETWORK_TYPE_EDGE:
        return NetworkType_NETWORK_2_75_G;
      case TelephonyManager_NETWORK_TYPE_UMTS:
      case TelephonyManager_NETWORK_TYPE_EVDO_0:
        return NetworkType_NETWORK_3_G;
      case TelephonyManager_NETWORK_TYPE_HSPA:
      case TelephonyManager_NETWORK_TYPE_HSDPA:
      case TelephonyManager_NETWORK_TYPE_HSUPA:
      case TelephonyManager_NETWORK_TYPE_EVDO_A:
      case TelephonyManager_NETWORK_TYPE_EVDO_B:
      case TelephonyManager_NETWORK_TYPE_EHRPD:
        return NetworkType_NETWORK_3_5_G;
      case TelephonyManager_NETWORK_TYPE_HSPAP:
        return NetworkType_NETWORK_3_75_G;
      case TelephonyManager_NETWORK_TYPE_LTE:
        return NetworkType_NETWORK_3_9_G;
      case TelephonyManager_NETWORK_TYPE_UNKNOWN:
      default:
        printf("GeckoNetworkManager" "Connected to an unknown mobile network!");
        return NetworkType_NETWORK_UNKNOWN;
    }
*/
    return NETWORK_UNKNOWN;
}

Double CGeckoNetworkManager::GetNetworkSpeed(NetworkType aType)
{
    switch (aType) {
      case NETWORK_NONE:
        return 0.0;
      case NETWORK_ETHERNET:
        return kNetworkSpeedEthernet;
      case NETWORK_WIFI:
        return kNetworkSpeedWifi;
      case NETWORK_WIMAX:
        return kNetworkSpeedWiMax;
      case NETWORK_2_G:
        return kNetworkSpeed_2_G;
      case NETWORK_2_5_G:
        return kNetworkSpeed_2_5_G;
      case NETWORK_2_75_G:
        return kNetworkSpeed_2_75_G;
      case NETWORK_3_G:
        return kNetworkSpeed_3_G;
      case NETWORK_3_5_G:
        return kNetworkSpeed_3_5_G;
      case NETWORK_3_75_G:
        return kNetworkSpeed_3_75_G;
      case NETWORK_3_9_G:
        return kNetworkSpeed_3_9_G;
      case NETWORK_UNKNOWN:
      default:
        return kDefaultBandwidth;
    }
}

Boolean CGeckoNetworkManager::IsNetworkUsuallyMetered(
    /* [in] */ NetworkType aType)
{
    switch (aType) {
      case NETWORK_NONE:
      case NETWORK_UNKNOWN:
      case NETWORK_ETHERNET:
      case NETWORK_WIFI:
      case NETWORK_WIMAX:
        return FALSE;
      case NETWORK_2_G:
      case NETWORK_2_5_G:
      case NETWORK_2_75_G:
      case NETWORK_3_G:
      case NETWORK_3_5_G:
      case NETWORK_3_75_G:
      case NETWORK_3_9_G:
        return TRUE;
      default:
        printf("GeckoNetworkManager"  "Got an unexpected network type!");
        return FALSE;
    }
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
