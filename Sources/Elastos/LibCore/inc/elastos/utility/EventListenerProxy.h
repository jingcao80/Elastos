#ifndef __ELASTOS_UTILITY_EVENTLISTENERPROXY_H__
#define __ELASTOS_UTILITY_EVENTLISTENERPROXY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

namespace Elastos {
namespace Utility {

class EventListenerProxy
    : public Object
    , public IEventListenerProxy
    , public IEventListener
{
public:
    CAR_INTERFACE_DECL()

    EventListenerProxy(
        /* [in] */ IEventListener* listener);

    CARAPI GetListener(
        /* [out] */ IEventListener** evl);

private:
    const AutoPtr<IEventListener> mListener;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_EVENTLISTENERPROXY_H__