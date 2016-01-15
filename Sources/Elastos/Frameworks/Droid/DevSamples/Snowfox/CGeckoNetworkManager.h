
#ifndef __CGECKONETWORKMANAGER_H__
#define __CGECKONETWORKMANAGER_H__

#include "_CGeckoNetworkManager.h"
//#include "BroadcastReceiver.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

CarClass(CGeckoNetworkManager) //, public BroadcastReceiver
{
public:

    CGeckoNetworkManager();

    CARAPI Init();

    CARAPI Start();

    CARAPI Stop();

    CARAPI GetCurrentInformation(
        /* [out, callee] */ ArrayOf<Double> ** ppInfo);

    CARAPI EnableNotifications();

    CARAPI DisableNotifications();

    static CGeckoNetworkManager* GetInstance();
protected:
    CARAPI OnRecieve(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent);


private:
    static AutoPtr<CGeckoNetworkManager> sInstance;

    enum NetworkType {
        NETWORK_NONE,
        NETWORK_ETHERNET,
        NETWORK_WIFI,
        NETWORK_WIMAX,
        NETWORK_2_G,    // 2G
        NETWORK_2_5_G,  // 2.5G
        NETWORK_2_75_G, // 2.75G
        NETWORK_3_G,    // 3G
        NETWORK_3_5_G,  // 3.5G
        NETWORK_3_75_G, // 3.75G
        NETWORK_3_9_G,  // 3.9G
        NETWORK_UNKNOWN
    };

    NetworkType  mNetworkType;
      AutoPtr<IIntentFilter> mNetworkFilter;
    // Whether the manager should be listening to Network Information changes.
    Boolean mShouldBeListening ;
    // Whether the manager should notify Gecko that a change in Network
    // Information happened.
    Boolean mShouldNotify;

    ECode StartListening();

    ECode StopListening();

    ECode UpdateNetworkType();

    NetworkType GetNetworkType();

    Double GetNetworkSpeed(NetworkType aType);

    Boolean IsNetworkUsuallyMetered(NetworkType aType);
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CGECKONETWORKMANAGER_H__
