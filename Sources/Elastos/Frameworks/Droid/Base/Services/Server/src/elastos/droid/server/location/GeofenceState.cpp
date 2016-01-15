
#include "location/GeofenceState.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Location::CLocation;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const Int32 GeofenceState::FLAG_ENTER = 0x01;
const Int32 GeofenceState::FLAG_EXIT = 0x02;

const Int32 GeofenceState::STATE_UNKNOWN = 0;
const Int32 GeofenceState::STATE_INSIDE = 1;
const Int32 GeofenceState::STATE_OUTSIDE = 2;

GeofenceState::GeofenceState(
    /* [in] */ IGeofence* fence,
    /* [in] */ Int64 expireAt,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* intent)
{
    assert(fence != NULL);

    mState = STATE_UNKNOWN;
    mDistanceToCenter = Elastos::Core::Math::DOUBLE_MAX_VALUE;

    mFence = fence;
    mExpireAt = expireAt;
    mPackageName = packageName;
    mIntent = intent;

    //mLocation = new Location("");
    CLocation::New(String(""), (ILocation**)&mLocation);
    Double latitude;
    Double longitude;
    fence->GetLatitude(&latitude);
    fence->GetLongitude(&longitude);
    mLocation->SetLatitude(latitude);
    mLocation->SetLongitude(longitude);
}

UInt32 GeofenceState::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GeofenceState::Release()
{
    return ElRefBase::Release();
}

ECode GeofenceState::GetInterfaceID(
    /* [in] */ IInterface *object,
    /* [out] */ InterfaceID *IID)
{
    return E_NOT_IMPLEMENTED;
}

/**
 * Process a new location.
 * @return FLAG_ENTER or FLAG_EXIT if the fence was crossed, 0 otherwise
 */
Int32 GeofenceState::ProcessLocation(
    /* [in] */ ILocation* location)
{
    mLocation->DistanceTo(location, (Float*)&mDistanceToCenter);

    Int32 prevState = mState;
    //TODO: inside/outside detection could be made more rigorous
    Float radius, accuracy;
    mFence->GetRadius(&radius);
    location->GetAccuracy(&accuracy);
    Boolean inside = mDistanceToCenter <= Elastos::Core::Math::Max(radius, accuracy);
    if (inside) {
        mState = STATE_INSIDE;
        if (prevState != STATE_INSIDE) {
            return FLAG_ENTER; // return enter if previously exited or unknown
        }
    } else {
        mState = STATE_OUTSIDE;
        if (prevState == STATE_INSIDE) {
            return FLAG_EXIT; // return exit only if previously entered
        }
    }
    return 0;
}

/**
 * Gets the distance from the current location to the fence's boundary.
 * @return The distance or {@link Double#MAX_VALUE} if unknown.
 */
Double GeofenceState::GetDistanceToBoundary()
{
    if (Elastos::Core::Math::Compare(mDistanceToCenter, Elastos::Core::Math::DOUBLE_MAX_VALUE) == 0) {
        return Elastos::Core::Math::DOUBLE_MAX_VALUE;
    } else {
        Float radius;
        mFence->GetRadius(&radius);
        return Elastos::Core::Math::Abs(radius - mDistanceToCenter);
    }
}

//@Override
ECode GeofenceState::ToString(
    /* [out] */ String* strOut)
{
/*    VALIDATE_NOT_NULL(strOut);

    String state;
    switch (mState) {
        case STATE_INSIDE:
            state = "IN";
            break;
        case STATE_OUTSIDE:
            state = "OUT";
            break;
        default:
            state = "?";
    }
    return String.format("%s d=%.0f %s", mFence.toString(), mDistanceToCenter, state);
*/
    return E_NOT_IMPLEMENTED;//temp
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
