#include "elastos/droid/view/OrientationListener.h"

using Elastos::Droid::Hardware::EIID_ISensorListener;

namespace Elastos {
namespace Droid {
namespace View {

//==========================================================================
// OrientationListener::SensorListenerInternal
//==========================================================================
CAR_INTERFACE_IMPL(OrientationListener::SensorListenerInternal, Object, ISensorListener)

ECode OrientationListener::SensorListenerInternal::constructor(
    /* [in] */ IWeakReference* host)
{
    mWeakHost = host;
    return NOERROR;
}

ECode OrientationListener::SensorListenerInternal::OnAccuracyChanged(
    /* [in] */ Int32 sensor,
    /* [in] */ Int32 accuracy)
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj) {
        OrientationListener* host = (OrientationListener*)obj.Get();
        return host->OnAccuracyChanged(sensor, accuracy);
    }
    return NOERROR;
}

ECode OrientationListener::SensorListenerInternal::OnSensorChanged(
    /* [in] */ Int32 sensor,
    /* [fl] */ ArrayOf<Float>* values)
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj) {
        OrientationListener* host = (OrientationListener*)obj.Get();
        return host->OnSensorChanged(sensor, values);
    }
    return NOERROR;
}

//==========================================================================
// OrientationListener::OrientationEventListenerInternal
//==========================================================================
ECode OrientationListener::OrientationEventListenerInternal::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWeakReference* host)
{
    mWeakHost = host;
    return OrientationEventListener::constructor(context);
}

ECode OrientationListener::OrientationEventListenerInternal::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 rate,
    /* [in] */ IWeakReference* host)
{
    mWeakHost = host;
    OrientationEventListener::constructor(context, rate);

    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj) {
        OrientationListener* hostObj = (OrientationListener*)obj.Get();
        return RegisterListener(hostObj->mOrientationListener);
    }

    return NOERROR;
}

ECode OrientationListener::OrientationEventListenerInternal::OnOrientationChanged(
    /* [in] */ Int32 orientation)
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj) {
        OrientationListener* host = (OrientationListener*)obj.Get();
        return host->OnOrientationChanged(orientation);
    }
    return NOERROR;
}

//==========================================================================
// OrientationListener
//==========================================================================
OrientationListener::OrientationListener()
{}

ECode OrientationListener::constructor(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IWeakReference> wr;
    mOrientationListener = new SensorListenerInternal();
    mOrientationListener->constructor(wr);
    mOrientationEventLis = new OrientationEventListenerInternal();
    mOrientationEventLis->constructor(ctx, wr);
    return NOERROR;
}

ECode OrientationListener::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 rate)
{
    AutoPtr<IWeakReference> wr;
    mOrientationListener = new SensorListenerInternal();
    mOrientationListener->constructor(wr);
    mOrientationEventLis = new OrientationEventListenerInternal();
    mOrientationEventLis->constructor(ctx, rate, wr);
    return NOERROR;
}

ECode OrientationListener::Enable()
{
    return mOrientationEventLis->Enable();
}

ECode OrientationListener::Disable()
{
    return mOrientationEventLis->Disable();
}

ECode OrientationListener::OnAccuracyChanged(
    /* [in] */ Int32 sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}

ECode OrientationListener::OnSensorChanged(
    /* [in] */ Int32 sensor,
    /* [fl] */ ArrayOf<Float>* values)
{
    return NOERROR;
}

ECode OrientationListener::OnOrientationChanged(
    /* [in] */ Int32 orientation)
{
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos