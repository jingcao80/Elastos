
#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pDemo {

class CActivityOne;

class CWifiBroadcastReceiver
    : public BroadcastReceiver
{
public:
    CWifiBroadcastReceiver(
        /* [in] */ IWifiP2pManager* wpm,
        /* [in] */ IWifiP2pManagerChannel* wpmc,
        /* [in] */ CActivityOne* activity);

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

public:
    AutoPtr<IWifiP2pManager> mP2pManager;
    AutoPtr<IWifiP2pManagerChannel> mChannel;
    CActivityOne* mActivity;
};

} // namespace WifiP2pDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
