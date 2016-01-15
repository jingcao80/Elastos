
#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_AGENT_INFO_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_AGENT_INFO_H__

#include <elastos/droid/server/connectivity/NetworkMonitor.h>
#include "_Elastos.Droid.Server.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::INetworkMisc;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Server::Connectivity::NetworkMonitor;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

/**
 * A bag class used by ConnectivityService for holding a collection of most recent
 * information published by a particular NetworkAgent as well as the
 * AsyncChannel/messenger for reaching that NetworkAgent and lists of NetworkRequests
 * interested in using it.
 */
class NetworkAgentInfo
    : public Object
    , public INetworkAgentInfo
{
public:
    CAR_INTERFACE_DECL()

    NetworkAgentInfo(
        /* [in] */ IMessenger* messenger,
        /* [in] */ IAsyncChannel* ac,
        /* [in] */ INetworkInfo* info,
        /* [in] */ ILinkProperties* lp,
        /* [in] */ INetworkCapabilities* nc,
        /* [in] */ Int32 score,
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ INetworkMisc* misc);

    CARAPI AddRequest(
        /* [in] */ INetworkRequest* networkRequest);

    CARAPI IsVPN(
        /* [out] */ Boolean* vpn);

    // Get the current score for this Network.  This may be modified from what the
    // NetworkAgent sent, as it has modifiers applied to it.
    CARAPI GetCurrentScore(
        /* [out] */ Int32* score);

    CARAPI SetCurrentScore(
        /* [in] */ Int32 newScore);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Name(
        /* [out] */ String* str);

    CARAPI_(String) Name();

public:
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<INetwork> mNetwork;
    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;
    AutoPtr<NetworkMonitor> mNetworkMonitor;
    AutoPtr<INetworkMisc> mNetworkMisc;
    Boolean mCreated;
    Boolean mValidated;

    // The list of NetworkRequests being satisfied by this Network.
    AutoPtr<ISparseArray> mNetworkRequests;// = new SparseArray<NetworkRequest>();
    AutoPtr<IArrayList> mNetworkLingered; //= new ArrayList<NetworkRequest>();

    AutoPtr<IMessenger> mMessenger;
    AutoPtr<IAsyncChannel> mAsyncChannel;

private:
    // This represents the last score received from the NetworkAgent.
    Int32 mCurrentScore;

    // Penalty applied to scores of Networks that have not been validated.
    static const Int32 UNVALIDATED_SCORE_PENALTY;// = 40;

    // Score for explicitly connected network.
    static const Int32 EXPLICITLY_SELECTED_NETWORK_SCORE;// = 100;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_AGENT_INFO_H__
