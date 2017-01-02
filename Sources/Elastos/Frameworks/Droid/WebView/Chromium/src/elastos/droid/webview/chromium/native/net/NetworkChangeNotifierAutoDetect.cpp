//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Net.h"
//#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApplicationState.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApplicationStatus.h"
#include "elastos/droid/webkit/webview/chromium/native/net/NetworkChangeNotifier.h"
#include "elastos/droid/webkit/webview/chromium/native/net/NetworkChangeNotifierAutoDetect.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApplicationState;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApplicationStatus;
using Elastos::Droid::Webkit::Webview::Chromium::Net::NetworkChangeNotifier;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {
#ifndef CAR_INTERFACE_IMPL_NCNAD
#define CAR_INTERFACE_IMPL_NCNAD(ClassName, SupperClassName, InterfaceName)       \
                                                           \
    UInt32 ClassName::AddRef()                             \
    {                                                      \
        return SupperClassName::AddRef();                        \
    }                                                      \
                                                           \
    UInt32 ClassName::Release()                            \
    {                                                      \
        return SupperClassName::Release();                       \
    }                                                      \
                                                           \
    PInterface ClassName::Probe(                           \
        /* [in] */ REIID riid)                             \
    {                                                      \
        if (riid == EIID_IInterface) {                     \
            return (IInterface*)(InterfaceName*)this;      \
        }                                                  \
        else if (riid == EIID_##InterfaceName) {           \
            return (IInterface*)(InterfaceName*)this;                   \
        }                                                  \
        return SupperClassName::Probe(riid);               \
    }                                                      \
                                                           \
    ECode ClassName::GetInterfaceID(                       \
        /* [in] */ IInterface* object,                     \
        /* [out] */ InterfaceID* iid)                      \
    {                                                      \
        VALIDATE_NOT_NULL(iid);                            \
                                                           \
        if (object == (IInterface*)(InterfaceName*)this) { \
            *iid = EIID_##InterfaceName;                   \
            return NOERROR;                                \
        }                                                  \
        return SupperClassName::GetInterfaceID(object, iid); \
    }
#endif
CAR_INTERFACE_IMPL_NCNAD(NetworkChangeNotifierAutoDetect, BroadcastReceiver, IBroadcastReceiver);

//=====================================================================
//     NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate
//=====================================================================
NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::ConnectivityManagerDelegate(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mConnectivityManager =
    //         (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    mConnectivityManager = IConnectivityManager::Probe(obj);
}

NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::ConnectivityManagerDelegate()
{
    // ==================before translated======================
    // // All the methods below should be overridden.
    // mConnectivityManager = null;

    mConnectivityManager = NULL;
}

Boolean NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::ActiveNetworkExists()
{
    // ==================before translated======================
    // return mConnectivityManager.getActiveNetworkInfo() != null;

    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    return Boolean(info != NULL);
}

Boolean NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::IsConnected()
{
    // ==================before translated======================
    // return mConnectivityManager.getActiveNetworkInfo().isConnected();

    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    Boolean isConntected = FALSE;
    info->IsConnected(&isConntected);
    return isConntected;
}

Int32 NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::GetNetworkType()
{
    // ==================before translated======================
    // return mConnectivityManager.getActiveNetworkInfo().getType();

    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    Int32 type = 0;
    info->GetType(&type);
    return type;
}

Int32 NetworkChangeNotifierAutoDetect::ConnectivityManagerDelegate::GetNetworkSubtype()
{
    // ==================before translated======================
    // return mConnectivityManager.getActiveNetworkInfo().getSubtype();

    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    Int32 type = 0;
    info->GetSubtype(&type);
    return type;
}

//=====================================================================
//         NetworkChangeNotifierAutoDetect::WifiManagerDelegate
//=====================================================================
NetworkChangeNotifierAutoDetect::WifiManagerDelegate::WifiManagerDelegate(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);

    AutoPtr<IInterface> tmp;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&tmp);
    mWifiManager = IWifiManager::Probe(tmp);
}

NetworkChangeNotifierAutoDetect::WifiManagerDelegate::WifiManagerDelegate()
{
    // ==================before translated======================
    // // All the methods below should be overridden.
    // mWifiManager = null;

    mWifiManager = NULL;
}

String NetworkChangeNotifierAutoDetect::WifiManagerDelegate::GetWifiSSID()
{
    // ==================before translated======================
    // WifiInfo wifiInfo = mWifiManager.getConnectionInfo();
    // if (wifiInfo == null)
    //     return "";
    // String ssid = wifiInfo.getSSID();
    // return ssid == null ? "" : ssid;

    AutoPtr<IWifiInfo> wifiInfo;
    mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiInfo);
    if (wifiInfo == NULL)
        return String("");

    String ssid;
    wifiInfo->GetSSID(&ssid);
    return (ssid == String("") ? String("") : ssid);
}

//=====================================================================
//   NetworkChangeNotifierAutoDetect::NetworkConnectivityIntentFilter
//=====================================================================
NetworkChangeNotifierAutoDetect::NetworkConnectivityIntentFilter::NetworkConnectivityIntentFilter()
{
    // ==================before translated======================
    // addAction(ConnectivityManager.CONNECTIVITY_ACTION);

    AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
}

//=====================================================================
//                   NetworkChangeNotifierAutoDetect
//=====================================================================
const String NetworkChangeNotifierAutoDetect::TAG("NetworkChangeNotifierAutoDetect");

NetworkChangeNotifierAutoDetect::NetworkChangeNotifierAutoDetect(
    /* [in] */ Observer* observer,
    /* [in] */ IContext* context)
    : mIntentFilter(new NetworkConnectivityIntentFilter())
    , mObserver(observer)
    , mRegistered(FALSE)
    , mConnectionType(0)
{
    // ==================before translated======================
    // mObserver = observer;
    // mContext = context.getApplicationContext();
    // mConnectivityManagerDelegate = new ConnectivityManagerDelegate(context);
    // mWifiManagerDelegate = new WifiManagerDelegate(context);
    // mConnectionType = getCurrentConnectionType();
    // mWifiSSID = getCurrentWifiSSID();
    // ApplicationStatus.registerApplicationStateListener(this);

    context->GetApplicationContext((IContext**)&mContext);
    mConnectivityManagerDelegate = new ConnectivityManagerDelegate(context);
    mWifiManagerDelegate = new WifiManagerDelegate(context);
    mConnectionType = GetCurrentConnectionType();
    mWifiSSID = GetCurrentWifiSSID();
    ApplicationStatus::RegisterApplicationStateListener(this);
}

ECode NetworkChangeNotifierAutoDetect::SetConnectivityManagerDelegateForTests(
    /* [in] */ ConnectivityManagerDelegate* delegate)
{
    VALIDATE_NOT_NULL(delegate);
    // ==================before translated======================
    // mConnectivityManagerDelegate = delegate;

    mConnectivityManagerDelegate = delegate;
    return NOERROR;
}

ECode NetworkChangeNotifierAutoDetect::SetWifiManagerDelegateForTests(
    /* [in] */ WifiManagerDelegate* delegate)
{
    VALIDATE_NOT_NULL(delegate);
    // ==================before translated======================
    // mWifiManagerDelegate = delegate;

    mWifiManagerDelegate = delegate;
    return NOERROR;
}

ECode NetworkChangeNotifierAutoDetect::Destroy()
{
    // ==================before translated======================
    // unregisterReceiver();

    UnregisterReceiver();
    return NOERROR;
}

Int32 NetworkChangeNotifierAutoDetect::GetCurrentConnectionType()
{
    // ==================before translated======================
    // // Track exactly what type of connection we have.
    // if (!mConnectivityManagerDelegate.activeNetworkExists() ||
    //         !mConnectivityManagerDelegate.isConnected()) {
    //     return NetworkChangeNotifier.CONNECTION_NONE;
    // }
    //
    // switch (mConnectivityManagerDelegate.getNetworkType()) {
    //     case ConnectivityManager.TYPE_ETHERNET:
    //         return NetworkChangeNotifier.CONNECTION_ETHERNET;
    //     case ConnectivityManager.TYPE_WIFI:
    //         return NetworkChangeNotifier.CONNECTION_WIFI;
    //     case ConnectivityManager.TYPE_WIMAX:
    //         return NetworkChangeNotifier.CONNECTION_4G;
    //     case ConnectivityManager.TYPE_BLUETOOTH:
    //         return NetworkChangeNotifier.CONNECTION_BLUETOOTH;
    //     case ConnectivityManager.TYPE_MOBILE:
    //         // Use information from TelephonyManager to classify the connection.
    //         switch (mConnectivityManagerDelegate.getNetworkSubtype()) {
    //             case TelephonyManager.NETWORK_TYPE_GPRS:
    //             case TelephonyManager.NETWORK_TYPE_EDGE:
    //             case TelephonyManager.NETWORK_TYPE_CDMA:
    //             case TelephonyManager.NETWORK_TYPE_1xRTT:
    //             case TelephonyManager.NETWORK_TYPE_IDEN:
    //                 return NetworkChangeNotifier.CONNECTION_2G;
    //             case TelephonyManager.NETWORK_TYPE_UMTS:
    //             case TelephonyManager.NETWORK_TYPE_EVDO_0:
    //             case TelephonyManager.NETWORK_TYPE_EVDO_A:
    //             case TelephonyManager.NETWORK_TYPE_HSDPA:
    //             case TelephonyManager.NETWORK_TYPE_HSUPA:
    //             case TelephonyManager.NETWORK_TYPE_HSPA:
    //             case TelephonyManager.NETWORK_TYPE_EVDO_B:
    //             case TelephonyManager.NETWORK_TYPE_EHRPD:
    //             case TelephonyManager.NETWORK_TYPE_HSPAP:
    //                 return NetworkChangeNotifier.CONNECTION_3G;
    //             case TelephonyManager.NETWORK_TYPE_LTE:
    //                 return NetworkChangeNotifier.CONNECTION_4G;
    //             default:
    //                 return NetworkChangeNotifier.CONNECTION_UNKNOWN;
    //         }
    //     default:
    //         return NetworkChangeNotifier.CONNECTION_UNKNOWN;
    // }

    assert(0);
    // Track exactly what type of connection we have.
    if (!mConnectivityManagerDelegate->ActiveNetworkExists() ||
            !mConnectivityManagerDelegate->IsConnected()) {
        return NetworkChangeNotifier::CONNECTION_NONE;
    }

    switch (mConnectivityManagerDelegate->GetNetworkType()) {
        case IConnectivityManager::TYPE_ETHERNET:
            return NetworkChangeNotifier::CONNECTION_ETHERNET;
        case IConnectivityManager::TYPE_WIFI:
            return NetworkChangeNotifier::CONNECTION_WIFI;
        case IConnectivityManager::TYPE_WIMAX:
            return NetworkChangeNotifier::CONNECTION_4G;
        case IConnectivityManager::TYPE_BLUETOOTH:
            return NetworkChangeNotifier::CONNECTION_BLUETOOTH;
        case IConnectivityManager::TYPE_MOBILE:
            {
                // Use information from TelephonyManager to classify the connection.
                /*switch (mConnectivityManagerDelegate->GetNetworkSubtype()) {
                    case ITelephonyManager::NETWORK_TYPE_GPRS:
                    case ITelephonyManager::NETWORK_TYPE_EDGE:
                    case ITelephonyManager::NETWORK_TYPE_CDMA:
                    case ITelephonyManager::NETWORK_TYPE_1xRTT:
                    case ITelephonyManager::NETWORK_TYPE_IDEN:
                        {
                            return NetworkChangeNotifier::CONNECTION_2G;
                        }
                    case ITelephonyManager::NETWORK_TYPE_UMTS:
                    case ITelephonyManager::NETWORK_TYPE_EVDO_0:
                    case ITelephonyManager::NETWORK_TYPE_EVDO_A:
                    case ITelephonyManager::NETWORK_TYPE_HSDPA:
                    case ITelephonyManager::NETWORK_TYPE_HSUPA:
                    case ITelephonyManager::NETWORK_TYPE_HSPA:
                    case ITelephonyManager::NETWORK_TYPE_EVDO_B:
                    case ITelephonyManager::NETWORK_TYPE_EHRPD:
                    case ITelephonyManager::NETWORK_TYPE_HSPAP:
                        return NetworkChangeNotifier::CONNECTION_3G;
                    case ITelephonyManager::NETWORK_TYPE_LTE:
                        return NetworkChangeNotifier::CONNECTION_4G;
                    default:
                        return NetworkChangeNotifier::CONNECTION_UNKNOWN;
                }*/
            }
        default:
            return NetworkChangeNotifier::CONNECTION_UNKNOWN;
    }
}

ECode NetworkChangeNotifierAutoDetect::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // connectionTypeChanged();

    ConnectionTypeChanged();
    return NOERROR;
}

// Inconsistent with ApplicationStatus.ApplicationStateListener, note it temporarily
void NetworkChangeNotifierAutoDetect::OnApplicationStateChange(
    /* [in] */ Int32 newState)
{
    // ==================before translated======================
    // if (newState == ApplicationState.HAS_RUNNING_ACTIVITIES) {
    //     connectionTypeChanged();
    //     registerReceiver();
    // } else if (newState == ApplicationState.HAS_PAUSED_ACTIVITIES) {
    //     unregisterReceiver();
    // }

    if (newState == ApplicationState::HAS_RUNNING_ACTIVITIES) {
        ConnectionTypeChanged();
        RegisterReceiver();
    }
    else if (newState == ApplicationState::HAS_PAUSED_ACTIVITIES) {
        UnregisterReceiver();
    }
}

ECode NetworkChangeNotifierAutoDetect::RegisterReceiver()
{
    // ==================before translated======================
    // if (!mRegistered) {
    //     mRegistered = true;
    //     mContext.registerReceiver(this, mIntentFilter);
    // }

    if (!mRegistered) {
        mRegistered = TRUE;
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(this, mIntentFilter, (IIntent**)&intent);
    }
    return NOERROR;
}

ECode NetworkChangeNotifierAutoDetect::UnregisterReceiver()
{
    // ==================before translated======================
    // if (mRegistered) {
    //     mRegistered = false;
    //     mContext.unregisterReceiver(this);
    // }

    if (mRegistered) {
        mRegistered = FALSE;
        mContext->UnregisterReceiver(this);
    }
    return NOERROR;
}

String NetworkChangeNotifierAutoDetect::GetCurrentWifiSSID()
{
    // ==================before translated======================
    // if (getCurrentConnectionType() != NetworkChangeNotifier.CONNECTION_WIFI)
    //     return "";
    // return mWifiManagerDelegate.getWifiSSID();

    if (GetCurrentConnectionType() != NetworkChangeNotifier::CONNECTION_WIFI)
        return String("");
    return mWifiManagerDelegate->GetWifiSSID();
}

ECode NetworkChangeNotifierAutoDetect::ConnectionTypeChanged()
{
    // ==================before translated======================
    // int newConnectionType = getCurrentConnectionType();
    // String newWifiSSID = getCurrentWifiSSID();
    // if (newConnectionType == mConnectionType && newWifiSSID.equals(mWifiSSID))
    //     return;
    //
    // mConnectionType = newConnectionType;
    // mWifiSSID = newWifiSSID;
    // Log.d(TAG, "Network connectivity changed, type is: " + mConnectionType);
    // mObserver.onConnectionTypeChanged(newConnectionType);

    Int32 newConnectionType = GetCurrentConnectionType();
    String newWifiSSID = GetCurrentWifiSSID();
    if (newConnectionType == mConnectionType && newWifiSSID.Equals(mWifiSSID))
        return NOERROR;

    mConnectionType = newConnectionType;
    mWifiSSID = newWifiSSID;

    Logger::D(TAG, "Network connectivity changed, type is: %d", mConnectionType);
    mObserver->OnConnectionTypeChanged(newConnectionType);
    return NOERROR;
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


