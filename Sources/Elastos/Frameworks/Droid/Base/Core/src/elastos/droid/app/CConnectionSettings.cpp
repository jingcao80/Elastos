
#include "Elastos.CoreLibrary.External.h"
#include "_Elastos.Droid.Bluetooth.h"
#include "_Elastos.Droid.Content.h"
#include "_Elastos.Droid.Location.h"
#include "_Elastos.Droid.Net.h"
#include "_Elastos.Droid.Nfc.h"
#include "_Elastos.Droid.Provider.h"
#include "_Elastos.Droid.Wifi.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CConnectionSettings.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ContentResolver;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace App {

const String CConnectionSettings::ACTION_MODIFY_NETWORK_MODE("com.android.internal.telephony.MODIFY_NETWORK_MODE");
const String CConnectionSettings::EXTRA_NETWORK_MODE("networkMode");

const Int32 CConnectionSettings::CM_MODE_2G = 0;
const Int32 CConnectionSettings::CM_MODE_3G = 1;
const Int32 CConnectionSettings::CM_MODE_4G = 2;
const Int32 CConnectionSettings::CM_MODE_2G3G = 3;
const Int32 CConnectionSettings::CM_MODE_ALL = 4;

CAR_INTERFACE_IMPL_2(CConnectionSettings, Object, IConnectionSettings, IParcelable)

CAR_OBJECT_IMPL(CConnectionSettings)

CConnectionSettings::CConnectionSettings()
    : mConnectionId(0)
    , mValue(0)
    , mOverride(FALSE)
    , mDirty(FALSE)
{}

ECode CConnectionSettings::constructor()
{
    return NOERROR;
}

ECode CConnectionSettings::constructor(
    /* [in] */ Int32 connectionId)
{
    return constructor(connectionId, 0, FALSE);
}

ECode CConnectionSettings::constructor(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 value,
    /* [in] */ Boolean override)
{
    mConnectionId = connectionId;
    mValue = value;
    mOverride = override;
    mDirty = FALSE;
    return NOERROR;
}

ECode CConnectionSettings::GetConnectionId(
    /* [out] */ Int32* connectionId)
{
    VALIDATE_NOT_NULL(connectionId);
    *connectionId = mConnectionId;
    return NOERROR;
}

ECode CConnectionSettings::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CConnectionSettings::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    mDirty = TRUE;
    return NOERROR;
}

ECode CConnectionSettings::SetOverride(
    /* [in] */ Boolean override)
{
    mOverride = override;
    mDirty = TRUE;
    return NOERROR;
}

ECode CConnectionSettings::IsOverride(
    /* [out] */ Boolean* override)
{
    VALIDATE_NOT_NULL(override);
    *override = mOverride;
    return NOERROR;
}

ECode CConnectionSettings::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

ECode CConnectionSettings::ProcessOverride(
    /* [in] */ IContext* context)
{
    AutoPtr<IBluetoothAdapter> bta = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&obj);
    AutoPtr<ILocationManager> lm = ILocationManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    AutoPtr<IWifiManager> wm = IWifiManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    AutoPtr<INfcAdapter> nfcAdapter;
    // TODO:
//    try {
//        nfcAdapter = NfcAdapter.getNfcAdapter(context);
//    } catch (UnsupportedOperationException e) {
//        //Nfc not available
//    }

    Int32 value;
    GetValue(&value);
    Boolean forcedState = value == 1;
    Boolean currentState;

    switch (GetConnectionId(&value), value) {
        case PROFILE_CONNECTION_MOBILEDATA:
            cm->GetMobileDataEnabled(&currentState);
            if (forcedState != currentState) {
// hharte                    cm.setMobileDataEnabled(forcedState);
            }
            break;
        case PROFILE_CONNECTION_2G3G4G: {
            AutoPtr<IIntent> intent;
            CIntent::New(ACTION_MODIFY_NETWORK_MODE, (IIntent**)&intent);
            switch(GetValue(&value), value) {
                case CM_MODE_2G:
                    intent->PutExtra(EXTRA_NETWORK_MODE, IRILConstants::NETWORK_MODE_GSM_ONLY);
                    break;
                case CM_MODE_3G:
                    intent->PutExtra(EXTRA_NETWORK_MODE, IRILConstants::NETWORK_MODE_WCDMA_ONLY);
                    break;
                case CM_MODE_4G:
                    intent->PutExtra(EXTRA_NETWORK_MODE, IRILConstants::NETWORK_MODE_LTE_ONLY);
                    break;
                case CM_MODE_2G3G:
                    intent->PutExtra(EXTRA_NETWORK_MODE, IRILConstants::NETWORK_MODE_WCDMA_PREF);
                    break;
                case CM_MODE_ALL:
                    intent->PutExtra(EXTRA_NETWORK_MODE, IRILConstants::NETWORK_MODE_LTE_GSM_WCDMA);
                    break;
                default:
                    return NOERROR;
            }
            context->SendBroadcast(intent);
            break;
        }
        case PROFILE_CONNECTION_BLUETOOTH: {
            Int32 btstate;
            bta->GetState(&btstate);
            if (forcedState && (btstate == IBluetoothAdapter::STATE_OFF
                    || btstate == IBluetoothAdapter::STATE_TURNING_OFF)) {
                Boolean result;
                bta->Enable(&result);
            }
            else if (!forcedState && (btstate == IBluetoothAdapter::STATE_ON
                    || btstate == IBluetoothAdapter::STATE_TURNING_ON)) {
                Boolean result;
                bta->Disable(&result);
            }
            break;
        }
        case PROFILE_CONNECTION_GPS: {
            lm->IsProviderEnabled(ILocationManager::GPS_PROVIDER, &currentState);
            if (currentState != forcedState) {
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);
                AutoPtr<ISettingsSecure> ss;
                CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
                ss->SetLocationProviderEnabled(resolver, ILocationManager::GPS_PROVIDER, forcedState);
            }
            break;
        }
        case PROFILE_CONNECTION_SYNC:
            ContentResolver::GetMasterSyncAutomatically(&currentState);
            if (forcedState != currentState) {
                ContentResolver::SetMasterSyncAutomatically(forcedState);
            }
            break;
        case PROFILE_CONNECTION_WIFI: {
            Int32 wifiApState;
            wm->GetWifiApState(&wifiApState);
            wm->IsWifiEnabled(&currentState);
            if (currentState != forcedState) {
                // Disable wifi tether
                Boolean result;
                if ((forcedState && (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLING)) ||
                        (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLED)) {
                    wm->SetWifiApEnabled(NULL, FALSE, &result);
                }
                wm->SetWifiEnabled(forcedState, &result);
            }
            break;
        }
        case PROFILE_CONNECTION_WIFIAP: {
            Int32 wifiState;
            wm->GetWifiState(&wifiState);
            wm->IsWifiApEnabled(&currentState);
            if (currentState != forcedState) {
                // Disable wifi
                Boolean result;
                if ((forcedState && (wifiState == IWifiManager::WIFI_STATE_ENABLING)) ||
                        (wifiState == IWifiManager::WIFI_STATE_ENABLED)) {
                    wm->SetWifiEnabled(FALSE, &result);
                }
                wm->SetWifiApEnabled(NULL, forcedState, &result);
            }
            break;
        }
        case PROFILE_CONNECTION_WIMAX:
            // TODO:
            // if (WimaxHelper.isWimaxSupported(context)) {
            //     currentState = WimaxHelper.isWimaxEnabled(context);
            //     if (currentState != forcedState) {
            //         WimaxHelper.setWimaxEnabled(context, forcedState);
            //     }
            // }
            break;
        case PROFILE_CONNECTION_NFC:
            if (nfcAdapter != NULL) {
                Int32 adapterState;
                nfcAdapter->GetAdapterState(&adapterState);
                currentState = (adapterState == INfcAdapter::STATE_ON ||
                        adapterState == INfcAdapter::STATE_TURNING_ON);
                if (currentState != forcedState) {
                    if (forcedState) {
                        Boolean result;
                        nfcAdapter->Enable(&result);
                    }
                    else if (!forcedState && adapterState != INfcAdapter::STATE_TURNING_OFF) {
                        Boolean result;
                        nfcAdapter->Disable(&result);
                    }
                }
            }
            break;
    }
    return NOERROR;
}

ECode CConnectionSettings::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IConnectionSettings** obj)
{
    VALIDATE_NOT_NULL(obj);

    Int32 event;
    xpp->Next(&event);
    AutoPtr<CConnectionSettings> connectionDescriptor;
    CConnectionSettings::NewByFriend(0, (CConnectionSettings**)&connectionDescriptor);
    String name;
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("connectionDescriptor"))) {
        xpp->GetName(&name);
        if (event == IXmlPullParser::START_TAG) {
            if (name.Equals("connectionId")) {
                String text;
                xpp->NextText(&text);
                connectionDescriptor->mConnectionId = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("value")) {
                String text;
                xpp->NextText(&text);
                connectionDescriptor->mValue = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("override")) {
                String text;
                xpp->NextText(&text);
                connectionDescriptor->mOverride = StringUtils::ParseBoolean(text);
            }
        }
        xpp->Next(&event);
    }
    *obj = (IConnectionSettings*)connectionDescriptor.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CConnectionSettings::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<connectionDescriptor>\n<connectionId>"));
    builder->Append(mConnectionId);
    builder->Append(String("</connectionId>\n<value>"));
    builder->Append(mValue);
    builder->Append(String("</value>\n<override>"));
    builder->Append(mOverride);
    builder->Append(String("</override>\n</connectionDescriptor>\n"));
    return NOERROR;
}

ECode CConnectionSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mConnectionId);
    source->ReadBoolean(&mOverride);
    source->ReadInt32(&mValue);
    source->ReadBoolean(&mDirty);
    return NOERROR;
}

ECode CConnectionSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mConnectionId);
    dest->WriteBoolean(mOverride);
    dest->WriteInt32(mValue);
    dest->WriteBoolean(mDirty);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
