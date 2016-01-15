
#ifndef __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__
#define __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;

namespace Libcore {
namespace Net {
namespace Event {

/**
 * A singleton used to dispatch network events to registered listeners.
 */
class NetworkEventDispatcher
    : public Object
    , public INetworkEventDispatcher
{
public:
    CAR_INTERFACE_DECL();

    /**
    * Returns the shared {@link NetworkEventDispatcher} instance.
    */
    static CARAPI GetInstance(
        /* [out] */ INetworkEventDispatcher** result);

    /**
    * Registers a listener to be notified when network events occur.
    * It can be deregistered using {@link #removeListener(NetworkEventListener)}
    */
    CARAPI AddListener(
        /* [in] */ INetworkEventListener* toAdd);

    /**
    * De-registers a listener previously added with {@link #addListener(NetworkEventListener)}. If
    * the listener was not previously registered this is a no-op.
    */
    CARAPI RemoveListener(
        /* [in] */ INetworkEventListener* toRemove);

    /**
    * Notifies registered listeners of a network configuration change.
    */
    CARAPI OnNetworkConfigurationChanged();

protected:
    /** Visible for testing. Use {@link #getInstance()} instead. */
    NetworkEventDispatcher();

private:
    static AutoPtr<INetworkEventDispatcher> sInstance;

    // private final List<NetworkEventListener> listeners =
    //   new CopyOnWriteArrayList<NetworkEventListener>();
    AutoPtr<ICopyOnWriteArrayList> mListeners;
};

} // namespace Event
} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__