#ifndef __ELASTOS_DROID_VIEW_ORIENTATIONEVENTLISTENER_H__
#define __ELASTOS_DROID_VIEW_ORIENTATIONEVENTLISTENER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ISensorListener;
using Elastos::Droid::Hardware::ISensorEventListener;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC OrientationEventListener
    : public Object
    , public IOrientationEventListener
{
private:
    class ECO_LOCAL SensorEventListenerImpl
        : public Object
        , public ISensorEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        SensorEventListenerImpl(
            /* [in] */ IWeakReference* hosts);

        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

    private:
        static const Int32 _DATA_X = 0;
        static const Int32 _DATA_Y = 1;
        static const Int32 _DATA_Z = 2;

        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    OrientationEventListener();

    virtual ~OrientationEventListener();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 rate);

    CARAPI RegisterListener(
        /* [in] */ ISensorListener* lis);

    CARAPI Enable();

    CARAPI Disable();

    CARAPI CanDetectOrientation(
        /* [out] */ Boolean* result);

    virtual CARAPI OnOrientationChanged(
        /* [in] */ Int32 orientation);

private:
    static const String TAG;// = "OrientationEventListener";
    static const Boolean DEBUG;// = FALSE;
    static const Boolean localLOGV;// = FALSE;
    Int32 mOrientation;// = ORIENTATION_UNKNOWN;
    AutoPtr<ISensorManager> mSensorManager;
    Boolean mEnabled;// = false;
    Int32 mRate;
    AutoPtr<ISensor> mSensor;
    AutoPtr<ISensorEventListener> mSensorEventListener;
    AutoPtr<ISensorListener> mOldListener;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ORIENTATIONEVENTLISTENER_H__
