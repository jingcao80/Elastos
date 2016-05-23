#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/server/wifi/WifiNetworkScoreCache.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Net::EIID_IINetworkScoreCache;
using Elastos::Droid::Net::IRssiCurve;
using Elastos::Droid::Net::IWifiKey;
using Elastos::Droid::Net::INetworkKey;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                        WifiNetworkScoreCache
//=====================================================================
Int32 WifiNetworkScoreCache::INVALID_NETWORK_SCORE = 100000;
String WifiNetworkScoreCache::TAG("WifiNetworkScoreCache");

CAR_INTERFACE_IMPL_2(WifiNetworkScoreCache, Object, IINetworkScoreCache, IBinder);

WifiNetworkScoreCache::WifiNetworkScoreCache()
{
    DBG = TRUE;
}

ECode WifiNetworkScoreCache::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHashMap::New((IMap**)&mNetworkCache);
    return NOERROR;
}

ECode WifiNetworkScoreCache::UpdateScores(
    /* [in] */ IList* networks)//ScoredNetwork
{
    if (networks == NULL) {
        return NOERROR;
    }
    Int32 size;
    networks->GetSize(&size);
    Logger::E(TAG, "updateScores list size=%d", size);

    {    AutoLock syncLock(mNetworkCache);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            networks->Get(i, (IInterface**)&obj);
            IScoredNetwork* network = IScoredNetwork::Probe(obj);
            String networkKey = BuildNetworkKey(network);
            if (networkKey.IsNull()) continue;
            mNetworkCache->Put(CoreUtils::Convert(networkKey), network);
        }
    }
    return NOERROR;
}

ECode WifiNetworkScoreCache::ClearScores()
{
    {    AutoLock syncLock(mNetworkCache);
         mNetworkCache->Clear();
    }
    return NOERROR;
}

ECode WifiNetworkScoreCache::IsScoredNetwork(
    /* [in] */ IScanResult* result,
    /* [out] */ Boolean* isScoreNetwork)
{
    VALIDATE_NOT_NULL(isScoreNetwork);
    String key = BuildNetworkKey(result);
    if (key.IsNull()) return FALSE;

    //find it
    {    AutoLock syncLock(mNetworkCache);
        AutoPtr<IInterface> obj;
        mNetworkCache->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        IScoredNetwork* network = IScoredNetwork::Probe(obj);
        if (network != NULL) {
            *isScoreNetwork = TRUE;
            return NOERROR;
        }
    }
    *isScoreNetwork = FALSE;
    return NOERROR;
}

ECode WifiNetworkScoreCache::GetNetworkScore(
    /* [in] */ IScanResult* result,
    /* [out] */ Int32* rValue)
{
    VALIDATE_NOT_NULL(rValue);
    Int32 score = INVALID_NETWORK_SCORE;

    String key = BuildNetworkKey(result);
    if (key.IsNull()) {
        *rValue = score;
        return NOERROR;
    }

    //find it
    {    AutoLock syncLock(mNetworkCache);
        AutoPtr<IInterface> obj;
        mNetworkCache->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        IScoredNetwork* network = IScoredNetwork::Probe(obj);
        if (network != NULL) {
            AutoPtr<IRssiCurve> rssiCurve;
            network->GetRssiCurve((IRssiCurve**)&rssiCurve);
            if(rssiCurve != NULL) {
                Int32 level;
                result->GetLevel(&level);
                Byte lookupScore = 0;
                rssiCurve->LookupScore(level, &lookupScore);
                score = lookupScore;
                if (DBG) {
                    Logger::E(TAG, "getNetworkScore found scored network %s, score: %d, RSSI: %d",
                            key.string(), score, level);
                }
            }
        }
    }
    *rValue = score;
    return NOERROR;
}

ECode WifiNetworkScoreCache::GetNetworkScore(
    /* [in] */ IScanResult* result,
    /* [in] */ Int32 rssiBoost,
    /* [out] */ Int32* rValue)
{
    VALIDATE_NOT_NULL(rValue);
    Int32 score = INVALID_NETWORK_SCORE;

    String key = BuildNetworkKey(result);
    if (key.IsNull()) {
        *rValue = score;
        return NOERROR;
    }

    //find it
    {    AutoLock syncLock(mNetworkCache);
        AutoPtr<IInterface> obj;
        mNetworkCache->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        IScoredNetwork* network = IScoredNetwork::Probe(obj);
        if (network != NULL) {
            AutoPtr<IRssiCurve> rssiCurve;
            network->GetRssiCurve((IRssiCurve**)&rssiCurve);
            if(rssiCurve != NULL) {
                Int32 level;
                result->GetLevel(&level);
                Byte lookupScore = 0;
                rssiCurve->LookupScore(level + rssiBoost, &lookupScore);
                score = lookupScore;
                if (DBG) {
                    Logger::E(TAG, "getNetworkScore found scored network %s, score: %d, RSSI: %d, boost: %d",
                            key.string(), score, level, rssiBoost);
                }
            }
        }
    }
    *rValue = score;
    return NOERROR;
}

void WifiNetworkScoreCache::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(permission.DUMP, TAG);
    // writer.println("WifiNetworkScoreCache");
    // writer.println("  All score curves:");
    // for (Map.Entry<String, ScoredNetwork> entry : mNetworkCache.entrySet()) {
    //     writer.println("    " + entry.getKey() + ": " + entry.getValue().rssiCurve);
    // }
    // writer.println("  Current network scores:");
    // WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
    // for (ScanResult scanResult : wifiManager.getScanResults()) {
    //     writer.println("    " + buildNetworkKey(scanResult) + ": " + getNetworkScore(scanResult));
    // }
    assert(0);
}

String WifiNetworkScoreCache::BuildNetworkKey(
    /* [in] */ IScoredNetwork* network)
{
    AutoPtr<INetworkKey> networkKey;
    network->GetNetworkKey((INetworkKey**)&networkKey);
    if (networkKey == NULL) return String(NULL);

    AutoPtr<IWifiKey> wifiKey;
    networkKey->GetWifiKey((IWifiKey**)&wifiKey);
    if (wifiKey == NULL) return String(NULL);
    Int32 type;
    networkKey->GetType(&type);
    if (type == INetworkKey::TYPE_WIFI) {
        String key;
        wifiKey->GetSsid(&key);
        if (key.IsNull()) return String(NULL);
        String bssid;
        wifiKey->GetBssid(&bssid);
        if (!bssid.IsNull()) {
            key = key + bssid;
        }
        return key;
    }
    return String(NULL);
}

String WifiNetworkScoreCache::BuildNetworkKey(
    /* [in] */ IScanResult* result)
{
    String ssid;
    result->GetSSID(&ssid);
    if (ssid.IsNull()) {
        return String(NULL);
    }
    AutoPtr<StringBuilder> key = new StringBuilder("\"");
    key->Append(ssid);
    key->Append("\"");
    result->GetSSID(&ssid);
    if (!ssid.IsNull()) {
        key->Append(ssid);
    }
    return key->ToString();
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
