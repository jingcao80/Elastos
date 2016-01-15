#include "EventListenerProxy.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL_2(EventListenerProxy, Object, IEventListenerProxy, IEventListener)

EventListenerProxy::EventListenerProxy(
    /* [in] */ IEventListener* listener)
    : mListener(listener)
{}

ECode EventListenerProxy::GetListener(
    /* [out] */ IEventListener** evl)
{
    VALIDATE_NOT_NULL(evl)
    *evl = mListener;
    REFCOUNT_ADD(*evl)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos