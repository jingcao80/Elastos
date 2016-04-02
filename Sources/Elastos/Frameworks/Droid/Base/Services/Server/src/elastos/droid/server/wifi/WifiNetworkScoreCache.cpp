
#include "elastos/droid/server/wifi/WifiNetworkScoreCache.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                        WifiNetworkScoreCache
//=====================================================================
Int32 WifiNetworkScoreCache::INVALID_NETWORK_SCORE;
String WifiNetworkScoreCache::TAG = "WifiNetworkScoreCache";

WifiNetworkScoreCache::WifiNetworkScoreCache(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context;
    // mNetworkCache = new HashMap<String, ScoredNetwork>();
}

WifiNetworkScoreCache::Log.e(
    /* [in] */  TAG)
{
}

WifiNetworkScoreCache::Synchronized(
    /* [in] */  mNetworkCache)
    : networks)
{
    // ==================before translated======================
    // for (ScoredNetwork network : networks) {
    //     String networkKey = buildNetworkKey(network);
    //     if (networkKey == null) continue;
    //     mNetworkCache.put(networkKey, network);
    // }
}

)
{
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


