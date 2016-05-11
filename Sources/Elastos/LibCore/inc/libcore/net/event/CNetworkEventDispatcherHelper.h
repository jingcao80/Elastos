
#ifndef __LIBCORE_NET_EVENT_CNETWORKEVENTDISPATCHERHELPER_H__
#define __LIBCORE_NET_EVENT_CNETWORKEVENTDISPATCHERHELPER_H__

#include "_Libcore_Net_Event_CNetworkEventDispatcherHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Libcore {
namespace Net {
namespace Event {

/**
 * A singleton used to dispatch network events to registered listeners.
 */
CarClass(CNetworkEventDispatcherHelper)
    , public Singleton
    , public INetworkEventDispatcherHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
    * Returns the shared {@link NetworkEventDispatcher} instance.
    */
    CARAPI GetInstance(
        /* [out] */ INetworkEventDispatcher** result);
};

} // namespace Event
} // namespace Net
} // namespace Libcore

#endif //  __LIBCORE_NET_EVENT_CNETWORKEVENTDISPATCHERHELPER_H__
