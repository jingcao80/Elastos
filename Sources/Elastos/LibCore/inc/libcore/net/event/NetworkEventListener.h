
#ifndef __LIBCORE_NET_EVENT_NETWORKEVENTLISTENER_H__
#define __LIBCORE_NET_EVENT_NETWORKEVENTLISTENER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Libcore {
namespace Net {
namespace Event {

/**
 * A base class for objects interested in network events.
 */
class NetworkEventListener
    : public Object
    , public INetworkEventListener
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI OnNetworkConfigurationChanged();
};

} // namespace Event
} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_EVENT_NETWORKEVENTLISTENER_H__