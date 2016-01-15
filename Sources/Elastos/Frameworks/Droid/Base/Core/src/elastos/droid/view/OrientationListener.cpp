#include "elastos/droid/view/OrientationListener.h"

namespace Elastos {
namespace Droid {
namespace View {

OrientationListener::OrientationEventListenerInternal::OrientationEventListenerInternal(
    /* [in] */ IContext* context,
    /* [in] */ OrientationListener* host)
{
    mHost = host;
    constructor(context);
}

OrientationListener::OrientationEventListenerInternal::OrientationEventListenerInternal(
    /* [in] */ IContext* context,
    /* [in] */ Int32 rate,
    /* [in] */ OrientationListener* host)
{
    mHost = host;
    constructor(context, rate);
    RegisterListener(mHost);
}

ECode OrientationListener::OrientationEventListenerInternal::OnOrientationChanged(
    /* [in] */ Int32 orientation)
{
    mHost->OnOrientationChanged(orientation);
    return NOERROR;
}

OrientationListener::OrientationListener()
{}

ECode OrientationListener::constructor(
    /* [in] */ IContext* ctx)
{
    mOrientationEventLis = new OrientationEventListenerInternal(ctx, this);
    return NOERROR;
}

ECode OrientationListener::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 rate)
{
    mOrientationEventLis = new OrientationEventListenerInternal(ctx, rate, this);
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

} // namespace View
} // namespace Droid
} // namespace Elastos