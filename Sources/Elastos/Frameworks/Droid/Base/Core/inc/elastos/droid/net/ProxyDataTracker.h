
#ifndef __ELASTOS_DROID_NET_PROXYDATATRACKER_H__
#define __ELASTOS_DROID_NET_PROXYDATATRACKER_H__

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/net/BaseNetworkStateTracker.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IHandler;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A data tracker responsible for bringing up and tearing down the system proxy server.
 *
 * {@hide}
 */
class ProxyDataTracker
    : public BaseNetworkStateTracker
    , public IProxyDataTracker
{
private:
    class InnerSub_BroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InnerSub_BroadcastReceiver(ProxyDataTracker*);
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        ProxyDataTracker* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ProxyDataTracker();

    /**
     * Create a new ProxyDataTracker
     */
    CARAPI constructor();

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** result);

    // @Override
    CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    /**
     * Disable connectivity to the network.
     */
    CARAPI Teardown(
        /* [out] */ Boolean* result);

    /**
     * Re-enable proxy data connectivity after a {@link #teardown()}.
     */
    CARAPI Reconnect(
        /* [out] */ Boolean* result);

    /**
     * Fetch default gateway address for the network
     */
    CARAPI GetDefaultGatewayAddr(
        /* [out] */ Int32* result);

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    CARAPI GetTcpBufferSizesPropName(
        /* [out] */ String* result);

private:
    /**
     * Record the detailed state of a network, and if it is a
     * change from the previous state, send a notification to
     * any listeners.
     * @param state the new @{code DetailedState}
     * @param reason a {@code String} indicating a reason for the state change,
     * if one was supplied. May be {@code null}.
     * @param extraInfo optional {@code String} providing extra information about the state change
     */
    CARAPI SetDetailedState(
        /* [in] */ NetworkInfoDetailedState state,
        /* [in] */ const String& reason,
        /* [in] */ const String& extraInfo);

private:
    AutoPtr<IAtomicBoolean> mReconnectRequested;

    AutoPtr<IAtomicBoolean> mIsProxyAvailable;

    AutoPtr<IAtomicInteger32> mDefaultGatewayAddr;

    AutoPtr<IBroadcastReceiver> mProxyStatusServiceListener;

    static const String TAG;

    static const String NETWORK_TYPE;

    // TODO: investigate how to get these DNS addresses from the system.
    static const String DNS1;

    static const String DNS2;

    static const String INTERFACE_NAME;

    static const String REASON_ENABLED;

    static const String REASON_DISABLED;

    static const String REASON_PROXY_DOWN;

    static const Int32 MSG_TEAR_DOWN_REQUEST;

    static const Int32 MSG_SETUP_REQUEST;

    static const String PERMISSION_PROXY_STATUS_SENDER;

    static const String ACTION_PROXY_STATUS_CHANGE;

    static const String KEY_IS_PROXY_AVAILABLE;

    static const String KEY_REPLY_TO_MESSENGER_BINDER;

    static const String KEY_REPLY_TO_MESSENGER_BINDER_BUNDLE;

    AutoPtr<IHandler> mTarget;

    AutoPtr<IMessenger> mProxyStatusService;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_PROXYDATATRACKER_H__
