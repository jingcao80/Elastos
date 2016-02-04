
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Location::IGeofence;
using Elastos::Droid::Location::ILocation;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Represents state associated with a geofence
 */
class GeofenceState
    : public Object
{
public:
    GeofenceState(
        /* [in] */ IGeofence* fence,
        /* [in] */ Int64 expireAt,
        /* [in] */ Int32 allowedResolutionLevel,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* intent);

    /**
     * Process a new location.
     * @return FLAG_ENTER or FLAG_EXIT if the fence was crossed, 0 otherwise
     */
    CARAPI ProcessLocation(
        /* [in] */ ILocation* location,
        /* [out] */ Int32* v);

    /**
     * Gets the distance from the current location to the fence's boundary.
     * @return The distance or {@link Double#MAX_VALUE} if unknown.
     */
    CARAPI GetDistanceToBoundary(
        /* [out] */ Double* v);

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

public:
    AutoPtr<IGeofence> mFence;
    Int64 mExpireAt;
    Int32 mAllowedResolutionLevel;
    Int32 mUid;
    String mPackageName;
    AutoPtr<IPendingIntent> mIntent;

public:
    const static Int32 FLAG_ENTER = 0x01;
    const static Int32 FLAG_EXIT = 0x02;

protected:
    Int32 mState;  // current state
    Double mDistanceToCenter;  // current distance to center of fence

private:
    static const Int32 STATE_UNKNOWN = 0;
    static const Int32 STATE_INSIDE = 1;
    static const Int32 STATE_OUTSIDE = 2;
    AutoPtr<ILocation> mLocation;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__
