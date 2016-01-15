
#ifndef __ELASTOS_DROID_HARDWARE_CSENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CSENSORMANAGER_H__

#include "_Elastos_Droid_Hardware_CSensorManager.h"
#include "elastos/droid/hardware/LegacySensorManager.h"
#include "elastos/droid/hardware/SensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {


CarClass(CSensorManager), public SensorManager
{
public:
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetSensors(
        /* [out] */ Int32* sensors);

    CARAPI GetSensorList(
        /* [in] */ Int32 type,
        /* [out, callee] */ ArrayOf<ISensor*>** sensors);

    CARAPI GetDefaultSensor(
        /* [in] */ Int32 type,
        /* [out] */ ISensor** sensor);

    CARAPI RegisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [out] */ Boolean* supported);

    CARAPI RegisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [in] */ Int32 rate,
        /* [out] */ Boolean* supported);

    CARAPI UnregisterListener(
        /* [in] */ ISensorListener* listener);

    CARAPI UnregisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors);

    CARAPI UnregisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor);

    CARAPI UnregisterListener(
        /* [in] */ ISensorEventListener* listener);

    CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rate,
        /* [out] */ Boolean* supported);

    CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rate,
        /* [in] */ IHandler* handler,
        /* [out] */ Boolean* supported);

protected:
    /**
     * Gets the full list of sensors that are available.
     * @hide
     */
    virtual CARAPI GetFullSensorList(
        /* [out, callee] */ ArrayOf<ISensor*>** sensors);

    virtual CARAPI_(void) UnregisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor);

    virtual CARAPI_(Boolean) RegisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 delay,
        /* [in] */ IHandler* handler);

friend class LegacySensorManager;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CSENSORMANAGER_H__
