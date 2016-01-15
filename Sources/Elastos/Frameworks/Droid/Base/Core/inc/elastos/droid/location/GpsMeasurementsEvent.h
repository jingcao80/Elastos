
#ifndef __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__
#define __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::Object;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class implementing a container for data associated with a measurement event.
 * Events are delivered to registered instances of {@link Listener}.
 *
 * @hide
 */
class GpsMeasurementsEvent
    : public Object
    , public IGpsMeasurementsEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsMeasurementsEvent();

    CARAPI constructor(
        /* [in] */ IGpsClock* clock,
        /* [in] */ ArrayOf<IGpsMeasurement*>* measurements);

    // @NonNull
    CARAPI GetClock(
        /* [out] */ IGpsClock** clock);

    /**
     * Gets a read-only collection of measurements associated with the current event.
     */
    // @NonNull
    CARAPI GetMeasurements(
        /* [out] */ ICollection** coll);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IGpsClock> mClock;
    AutoPtr<ICollection> mReadOnlyMeasurements;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__
