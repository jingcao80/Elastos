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

/**
 * Helper class for receiving notifications from the SensorManager when
 * the orientation of the device has changed.
 *  @deprecated use {@link android.view.OrientationEventListener} instead.
 *  This class internally uses the OrientationEventListener.
 */
class OrientationListener
    : public Object
{
private:
    class SensorListenerInternal
        : public Object
        , public ISensorListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IWeakReference* host);

        CARAPI OnAccuracyChanged(
            /* [in] */ Int32 sensor,
            /* [in] */ Int32 accuracy);

        CARAPI OnSensorChanged(
            /* [in] */ Int32 sensor,
            /* [fl] */ ArrayOf<Float>* values);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class OrientationEventListenerInternal
        : public OrientationEventListener
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IWeakReference* host);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 rate,
            /* [in] */ IWeakReference* host);

        CARAPI OnOrientationChanged(
            /* [in] */ Int32 orientation);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    OrientationListener();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 rate);

    virtual CARAPI Enable();

    virtual CARAPI Disable();

    virtual CARAPI OnAccuracyChanged(
        /* [in] */ Int32 sensor,
        /* [in] */ Int32 accuracy);

    virtual CARAPI OnSensorChanged(
        /* [in] */ Int32 sensor,
        /* [fl] */ ArrayOf<Float>* values);

    virtual CARAPI OnOrientationChanged(
        /* [in] */ Int32 orientation);

private:
    AutoPtr<SensorListenerInternal> mOrientationListener;
    AutoPtr<OrientationEventListenerInternal> mOrientationEventLis;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif