#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Net::IScoredNetwork;
using Elastos::Droid::Net::IINetworkScoreCache;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiNetworkScoreCache
    : public Object
    , public IINetworkScoreCache
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    WifiNetworkScoreCache(
        /* [in] */ IContext* context);

    // @Override
    CARAPI UpdateScores(
        /* [in] */ IList* networks);//android::net::ScoredNetwork

    // @Override
    CARAPI ClearScores();

    virtual CARAPI IsScoredNetwork(
        /* [in] */ IScanResult* result,
        /* [out] */ Boolean* isScoreNetwork);

    virtual CARAPI GetNetworkScore(
        /* [in] */ IScanResult* result,
        /* [out] */ Int32* rValue);

    virtual CARAPI GetNetworkScore(
        /* [in] */ IScanResult* result,
        /* [in] */ Int32 rssiBoost,
        /* [out] */ Int32* rValue);

protected:
    // @Override
    const CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(String) BuildNetworkKey(
        /* [in] */ IScoredNetwork* network);

    CARAPI_(String) BuildNetworkKey(
        /* [in] */ IScanResult* result);

public:
    // A Network scorer returns a score in the range [-127, +127]
    static Int32 INVALID_NETWORK_SCORE;// = 100000;

private:
    static String TAG;
    Boolean DBG;
    /*const*/ AutoPtr<IContext> mContext;
    // The key is of the form "<ssid>"<bssid>
    // TODO: What about SSIDs that can't be encoded as UTF-8?
    /*const*/ AutoPtr<IMap> mNetworkCache;//String, ScoredNetwork
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINETWORKSCORECACHE_H__
