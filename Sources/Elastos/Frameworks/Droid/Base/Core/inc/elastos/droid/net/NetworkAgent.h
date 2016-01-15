
#ifndef __ELASTOS_DROID_NET_NETWORKAGENT_H__
#define __ELASTOS_DROID_NET_NETWORKAGENT_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A Utility class for handling for communicating between bearer-specific
 * code and ConnectivityService.
 *
 * A bearer may have more than one NetworkAgent if it can simultaneously
 * support separate networks (IMS / Internet / MMS Apns on cellular, or
 * perhaps connections with different SSID or P2P for Wi-Fi).
 *
 * @hide
 */
class ECO_PUBLIC NetworkAgent
    : public Handler
    , public INetworkAgent
{
public:
    CAR_INTERFACE_DECL()

    NetworkAgent();

    virtual ~NetworkAgent();

    CARAPI constructor(
        /* [in] */ ILooper* looper,
        /* [in] */ IContext* context,
        /* [in] */ const String& logTag,
        /* [in] */ INetworkInfo* ni,
        /* [in] */ INetworkCapabilities* nc,
        /* [in] */ ILinkProperties* lp,
        /* [in] */ Int32 score);

    CARAPI constructor(
        /* [in] */ ILooper* looper,
        /* [in] */ IContext* context,
        /* [in] */ const String& logTag,
        /* [in] */ INetworkInfo* ni,
        /* [in] */ INetworkCapabilities* nc,
        /* [in] */ ILinkProperties* lp,
        /* [in] */ Int32 score,
        /* [in] */ INetworkMisc* misc);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * Called by the bearer code when it has new LinkProperties data.
     */
    CARAPI SendLinkProperties(
        /* [in] */ ILinkProperties* linkProperties);

    /**
     * Called by the bearer code when it has new NetworkInfo data.
     */
    CARAPI SendNetworkInfo(
        /* [in] */ INetworkInfo* networkInfo);

    /**
     * Called by the bearer code when it has new NetworkCapabilities data.
     */
    CARAPI SendNetworkCapabilities(
        /* [in] */ INetworkCapabilities* networkCapabilities);

    /**
     * Called by the bearer code when it has a new score for this network.
     */
    CARAPI SendNetworkScore(
        /* [in] */ Int32 score);

    /**
     * Called by the VPN code when it wants to add ranges of UIDs to be routed
     * through the VPN network.
     */
    CARAPI AddUidRanges(
        /* [in] */ ArrayOf<IUidRange*>* ranges);

    /**
     * Called by the VPN code when it wants to remove ranges of UIDs from being routed
     * through the VPN network.
     */
    CARAPI RemoveUidRanges(
        /* [in] */ ArrayOf<IUidRange*>* ranges);

    /**
     * Called by the bearer to indicate this network was manually selected by the user.
     * This should be called before the NetworkInfo is marked CONNECTED so that this
     * Network can be given special treatment at that time.
     */
    CARAPI ExplicitlySelected();

    /**
     * Called when ConnectivityService has indicated they no longer want this network.
     * The parent factory should (previously) have received indication of the change
     * as well, either canceling NetworkRequests or altering their score such that this
     * network won't be immediately requested again.
     */
    virtual CARAPI Unwanted() = 0;

    /**
     * Called when the system determines the usefulness of this network.
     *
     * Networks claiming internet connectivity will have their internet
     * connectivity verified.
     *
     * Currently there are two possible values:
     * {@code VALID_NETWORK} if the system is happy with the connection,
     * {@code INVALID_NETWORK} if the system is not happy.
     * TODO - add indications of captive portal-ness and related success/failure,
     * ie, CAPTIVE_SUCCESS_NETWORK, CAPTIVE_NETWORK for successful login and detection
     *
     * This may be called multiple times as the network status changes and may
     * generate false negatives if we lose ip connectivity before the link is torn down.
     */
    CARAPI NetworkStatus(
        /* [in] */ Int32 status);

    CARAPI Log(
        /* [in] */ const char* fmt, ...);

private:
    ECO_LOCAL CARAPI QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // to initialize mPreConnectedQueue
    ECO_LOCAL CARAPI_(AutoPtr<IArrayList>) CreatePreConnectQueue();

    /* const */ AutoPtr<IArrayList> mPreConnectedQueue;

    /* volatile */ AutoPtr<IAsyncChannel> mAsyncChannel;

    // LOG_TAG has duplicated macro
    String mLOG_TAG;

    ECO_LOCAL static const Boolean DBG;

    ECO_LOCAL static const Boolean VDBG;

    /* const */ AutoPtr<IContext> mContext;

    ECO_LOCAL static const Int32 BASE;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKAGENT_H__
