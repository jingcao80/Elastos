
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONHELPER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONHELPER_H__

#include "_Elastos_Droid_Location_CLocationHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationHelper)
    , public Singleton
    , public ILocationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Converts a coordinate to a String representation. The outputType
     * may be one of FORMAT_DEGREES, FORMAT_MINUTES, or FORMAT_SECONDS.
     * The coordinate must be a valid double between -180.0 and 180.0.
     *
     * @throws IllegalArgumentException if coordinate is less than
     * -180.0, greater than 180.0, or is not a number.
     * @throws IllegalArgumentException if outputType is not one of
     * FORMAT_DEGREES, FORMAT_MINUTES, or FORMAT_SECONDS.
     */
    CARAPI Convert(
        /* [in] */ Double coordinate,
        /* [in] */ Int32 outputType,
        /* [out] */ String* str);

    /**
     * Converts a String in one of the formats described by
     * FORMAT_DEGREES, FORMAT_MINUTES, or FORMAT_SECONDS into a
     * double.
     *
     * @throws NullPointerException if coordinate is null
     * @throws IllegalArgumentException if the coordinate is not
     * in one of the valid formats.
     */
    CARAPI Convert(
        /* [in] */ const String& coordinate,
        /* [out] */ Double* d);

    /**
     * Computes the approximate distance in meters between two
     * locations, and optionally the initial and final bearings of the
     * shortest path between them.  Distance and bearing are defined using the
     * WGS84 ellipsoid.
     *
     * <p> The computed distance is stored in results[0].  If results has length
     * 2 or greater, the initial bearing is stored in results[1]. If results has
     * length 3 or greater, the final bearing is stored in results[2].
     *
     * @param startLatitude the starting latitude
     * @param startLongitude the starting longitude
     * @param endLatitude the ending latitude
     * @param endLongitude the ending longitude
     * @param results an array of floats to hold the results
     *
     * @throws IllegalArgumentException if results is null or has length < 1
     */
    CARAPI DistanceBetween(
        /* [in] */ Double startLatitude,
        /* [in] */ Double startLongitude,
        /* [in] */ Double endLatitude,
        /* [in] */ Double endLongitude,
        /* [in] */ ArrayOf<Float>* results);
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONHELPER_H__
