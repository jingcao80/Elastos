#include "Elastos.Droid.Os.h"
#include "elastos/droid/server/wifi/WifiNetworkScoreCache.h"

using Elastos::Droid::Net::EIID_IINetworkScoreCache;
using Elastos::Droid::Os::EIID_IBinder;

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
    // ==================before translated======================
    // mContext = context;
    // mNetworkCache = new HashMap<String, ScoredNetwork>();
    return NOERROR;
}

ECode WifiNetworkScoreCache::UpdateScores(
    /* [in] */ IList* networks)//ScoredNetwork
{
    VALIDATE_NOT_NULL(networks);
    // ==================before translated======================
    // if (networks == null) {
    //     return;
    // }
    // Log.e(TAG, "updateScores list size=" + networks.size());
    //
    // synchronized(mNetworkCache) {
    //     for (ScoredNetwork network : networks) {
    //         String networkKey = buildNetworkKey(network);
    //         if (networkKey == null) continue;
    //         mNetworkCache.put(networkKey, network);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiNetworkScoreCache::ClearScores()
{
    // ==================before translated======================
    // synchronized (mNetworkCache) {
    //     mNetworkCache.clear();
    // }
    assert(0);
    return NOERROR;
}

ECode WifiNetworkScoreCache::IsScoredNetwork(
    /* [in] */ IScanResult* result,
    /* [out] */ Boolean* isScoreNetwork)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(isScoreNetwork);
    // ==================before translated======================
    // String key = buildNetworkKey(result);
    // if (key == null) return false;
    //
    // //find it
    // synchronized(mNetworkCache) {
    //     ScoredNetwork network = mNetworkCache.get(key);
    //     if (network != null) {
    //         return true;
    //     }
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode WifiNetworkScoreCache::GetNetworkScore(
    /* [in] */ IScanResult* result,
    /* [out] */ Int32* rValue)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // int score = INVALID_NETWORK_SCORE;
    //
    // String key = buildNetworkKey(result);
    // if (key == null) return score;
    //
    // //find it
    // synchronized(mNetworkCache) {
    //     ScoredNetwork network = mNetworkCache.get(key);
    //     if (network != null && network.rssiCurve != null) {
    //         score = network.rssiCurve.lookupScore(result.level);
    //         if (DBG) {
    //             Log.e(TAG, "getNetworkScore found scored network " + key
    //                     + " score " + Integer.toString(score)
    //                     + " RSSI " + result.level);
    //         }
    //     }
    // }
    // return score;
    assert(0);
    return NOERROR;
}

ECode WifiNetworkScoreCache::GetNetworkScore(
    /* [in] */ IScanResult* result,
    /* [in] */ Int32 rssiBoost,
    /* [out] */ Int32* rValue)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // int score = INVALID_NETWORK_SCORE;
    //
    // String key = buildNetworkKey(result);
    // if (key == null) return score;
    //
    // //find it
    // synchronized(mNetworkCache) {
    //     ScoredNetwork network = mNetworkCache.get(key);
    //     if (network != null && network.rssiCurve != null) {
    //         score = network.rssiCurve.lookupScore(result.level + rssiBoost);
    //         if (DBG) {
    //             Log.e(TAG, "getNetworkScore found scored network " + key
    //                     + " score " + Integer.toString(score)
    //                     + " RSSI " + result.level
    //                     + " boost " + rssiBoost);
    //         }
    //     }
    // }
    // return score;
    assert(0);
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
    // ==================before translated======================
    // if (network.networkKey == null) return null;
    // if (network.networkKey.wifiKey == null) return null;
    // if (network.networkKey.type == NetworkKey.TYPE_WIFI) {
    //     String key = network.networkKey.wifiKey.ssid;
    //     if (key == null) return null;
    //     if (network.networkKey.wifiKey.bssid != null) {
    //         key = key + network.networkKey.wifiKey.bssid;
    //     }
    //     return key;
    // }
    // return null;
    assert(0);
    return String("");
}

String WifiNetworkScoreCache::BuildNetworkKey(
    /* [in] */ IScanResult* result)
{
    // ==================before translated======================
    // if (result.SSID == null) {
    //     return null;
    // }
    // StringBuilder key = new StringBuilder("\"");
    // key.append(result.SSID);
    // key.append("\"");
    // if (result.BSSID != null) {
    //     key.append(result.BSSID);
    // }
    // return key.toString();
    assert(0);
    return String("");
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


