
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif

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
class GeofenceState : public ElRefBase
{
public:
    const static Int32 FLAG_ENTER;// = 0x01;
    const static Int32 FLAG_EXIT;// = 0x02;

public:
    GeofenceState(
        /* [in] */ IGeofence* fence,
        /* [in] */ Int64 expireAt,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* intent);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *object,
        /* [out] */ InterfaceID *IID);

    /**
     * Process a new location.
     * @return FLAG_ENTER or FLAG_EXIT if the fence was crossed, 0 otherwise
     */
    CARAPI_(Int32) ProcessLocation(
        /* [in] */ ILocation* location);

    /**
     * Gets the distance from the current location to the fence's boundary.
     * @return The distance or {@link Double#MAX_VALUE} if unknown.
     */
    CARAPI_(Double) GetDistanceToBoundary();

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

public:
    AutoPtr<IGeofence> mFence;

    Int64 mExpireAt;
    String mPackageName;
    AutoPtr<IPendingIntent> mIntent;

    Int32 mState;  // current state
    Double mDistanceToCenter;  // current distance to center of fence

private:

    static const Int32 STATE_UNKNOWN;// = 0;
    static const Int32 STATE_INSIDE;// = 1;
    static const Int32 STATE_OUTSIDE;// = 2;

    AutoPtr<ILocation> mLocation;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCESTATE_H__
