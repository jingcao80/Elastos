#include "elastos/droid/service/gesture/CEdgeGestureActivationListenerDelegator.h"
#include "elastos/droid/service/gesture/EdgeGestureManager.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

CAR_INTERFACE_IMPL_2(CEdgeGestureActivationListenerDelegator, Object, IIEdgeGestureActivationListener, IBinder)

CAR_OBJECT_IMPL(CEdgeGestureActivationListenerDelegator)

ECode CEdgeGestureActivationListenerDelegator::constructor(
    /* [in] */ IEdgeGestureActivationListener* weakHost)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(weakHost);
    wrs->GetWeakReference((IWeakReference**)&mWeakHost);
    return NOERROR;
}

ECode CEdgeGestureActivationListenerDelegator::OnEdgeGestureActivation(
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY,
    /* [in] */ Int32 positionIndex,
    /* [in] */ Int32 flags)
{
    AutoPtr<IEdgeGestureActivationListener> egal;
    mWeakHost->Resolve(EIID_IEdgeGestureActivationListener, (IInterface**)&egal);
    if (egal != NULL) {
        EdgeGestureManager::EdgeGestureActivationListener* listener;
        listener = (EdgeGestureManager::EdgeGestureActivationListener*)egal.Get();
        listener->OnEdgeGestureActivationInner(touchX, touchY, positionIndex, flags);
    }
    return NOERROR;
}

ECode CEdgeGestureActivationListenerDelegator::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}


} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos