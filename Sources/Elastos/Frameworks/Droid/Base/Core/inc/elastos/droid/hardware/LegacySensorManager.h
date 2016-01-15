
#ifndef __ELASTOS_DROID_HARDWARE_LEGACYSENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_LEGACYSENSORMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {

class LegacyListener;

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::ISensorListener);

namespace Elastos {
namespace Droid {
namespace Hardware {

class CLegacySensorManagerRotationWatcher;

/**
 * Helper class for implementing the legacy sensor manager API.
 * @hide
 */
class LegacySensorManager
    : public Object
    , public ILegacySensorManager
{
    friend class CLegacySensorManagerRotationWatcher;
public:
    class LmsFilter : public Object
    {
    public:
        LmsFilter();

        Float Filter(
            /* [in] */ Int64 time,
            /* [in] */ Float in);

    private:
        static const Int32 SENSORS_RATE_MS;
        static const Int32 COUNT;
        static const Float PREDICTION_RATIO;
        static const Float PREDICTION_TIME;
        AutoPtr<ArrayOf<Float> > mV;
        AutoPtr<ArrayOf<Int64> > mT;
        Int32 mIndex;
    };

public:
    CAR_INTERFACE_DECL()

    LegacySensorManager(
        /* [in] */ ISensorManager* sensorManager);

    CARAPI GetSensors(
        /* [out] */ Int32* num);

    CARAPI RegisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [in] */ Int32 rate,
        /* [out] */ Boolean* result);

    CARAPI UnregisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors);

    static CARAPI_(Int32) GetRotation();

protected:
    static CARAPI_(void) OnRotationChanged(
        /* [in] */ Int32 rotation);

private:
    CARAPI_(Boolean) RegisterLegacyListener(
        /* [in] */ Int32 legacyType,
        /* [in] */ Int32 type,
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [in] */ Int32 rate);

    CARAPI_(void) UnregisterLegacyListener(
        /* [in] */ Int32 legacyType,
        /* [in] */ Int32 type,
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors);


private:
    static Boolean sInitialized;
    static AutoPtr<IIWindowManager> sWindowManager;
    static Int32 sRotation;

    AutoPtr<ISensorManager> mSensorManager;

    // List of legacy listeners.  Guarded by mLegacyListenersMap.
    HashMap<AutoPtr<ISensorListener>, AutoPtr<LegacyListener> > mLegacyListenersMap/* =
            new HashMap<SensorListener, LegacyListener>()*/;
    Object mLegacyListenersMapLock;
};

class LegacyListener
    : public Object
    , public ISensorEventListener
{
public:
    CAR_INTERFACE_DECL()

    LegacyListener(
        /* [in] */ ISensorListener* target);

    CARAPI_(Boolean) RegisterSensor(
        /* [in] */ Int32 legacyType);

    CARAPI_(Boolean) UnregisterSensor(
        /* [in] */ Int32 legacyType);

    CARAPI_(Boolean) HasSensors();

    CARAPI OnAccuracyChanged(
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 accuracy);

    CARAPI OnSensorChanged(
        /* [in] */ ISensorEvent* event);

private:
    static CARAPI_(Boolean) HasOrientationSensor(
        /* [in] */ Int32 sensors);

    /*
     * Helper function to convert the specified sensor's data to the windows's
     * coordinate space from the device's coordinate space.
     *
     * output: 3,4,5: values in the old API format
     *         0,1,2: transformed values in the old API format
     *
     */
    CARAPI_(void) MapSensorDataToWindow(
        /* [in] */ Int32 sensor,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ Int32 orientation);

    static CARAPI_(Int32) GetLegacySensorType(
        /* [in] */ Int32 type);


private:
    AutoPtr<ArrayOf<Float> > mValues;
    AutoPtr<ISensorListener> mTarget;
    Int32 mSensors;
    AutoPtr<LegacySensorManager::LmsFilter> mYawfilter;

    friend class LegacySensorManager;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LEGACYSENSORMANAGER_H__
