#ifndef __ELASTOS_DROID_VIEW_ORIENTATIONLISTENER_H__
#define __ELASTOS_DROID_VIEW_ORIENTATIONLISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/OrientationEventListener.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace View {

class OrientationListener
    : public Object
    , public IOrientationListener
{
public:
    class OrientationEventListenerInternal
        : public OrientationEventListener
    {
    public:
        OrientationEventListenerInternal(
            /* [in] */ IContext* context,
            /* [in] */ OrientationListener* host);

        OrientationEventListenerInternal(
            /* [in] */ IContext* context,
            /* [in] */ Int32 rate,
            /* [in] */ OrientationListener* host);

        CARAPI OnOrientationChanged(
            /* [in] */ Int32 orientation);
    private:
        OrientationListener* mHost;
    };
public:
    OrientationListener();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 rate);

    CARAPI Enable();

    CARAPI Disable();

    CARAPI OnAccuracyChanged(
        /* [in] */ Int32 sensor,
        /* [in] */ Int32 accuracy);

    CARAPI OnSensorChanged(
        /* [in] */ Int32 sensor,
        /* [fl] */ ArrayOf<Float>* values);

private:
    AutoPtr<OrientationEventListener> mOrientationEventLis;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif