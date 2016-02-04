
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/server/location/GeofenceState.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Location::CLocation;
using Elastos::Core::CDouble;
using Elastos::Core::IComparable;
using Elastos::Core::IDouble;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const Int32 GeofenceState::FLAG_ENTER;
const Int32 GeofenceState::FLAG_EXIT;

const Int32 GeofenceState::STATE_UNKNOWN;
const Int32 GeofenceState::STATE_INSIDE;
const Int32 GeofenceState::STATE_OUTSIDE;

GeofenceState::GeofenceState(
    /* [in] */ IGeofence* fence,
    /* [in] */ Int64 expireAt,
    /* [in] */ Int32 allowedResolutionLevel,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* intent)
{
    mState = STATE_UNKNOWN;
    mDistanceToCenter = Elastos::Core::Math::DOUBLE_MAX_VALUE;
    mFence = fence;
    mExpireAt = expireAt;
    mAllowedResolutionLevel = allowedResolutionLevel;
    mUid = uid;
    mPackageName = packageName;
    mIntent = intent;
    CLocation::New(String(""), (ILocation**)&mLocation);
    Double latitude;
    fence->GetLatitude(&latitude);
    mLocation->SetLatitude(latitude);
    fence->GetLongitude(&latitude);
    mLocation->SetLongitude(latitude);
}

ECode GeofenceState::ProcessLocation(
    /* [in] */ ILocation* location,
    /* [out] */ Int32* v)
{
    VALIDATE_NOT_NULL(v)
    Float f;
    mLocation->DistanceTo(location, &f);
    mDistanceToCenter = (Double)f;

    Int32 prevState = mState;
    //TODO: inside/outside detection could be made more rigorous
    Float radius, accuracy;
    mFence->GetRadius(&radius);
    location->GetAccuracy(&accuracy);
    Boolean inside = mDistanceToCenter <= Elastos::Core::Math::Max(radius, accuracy);
    if (inside) {
        mState = STATE_INSIDE;
        if (prevState != STATE_INSIDE) {
            *v = GeofenceState::FLAG_ENTER; // return enter if previously exited or unknown
            return NOERROR;
        }
    }
    else {
        mState = STATE_OUTSIDE;
        if (prevState == STATE_INSIDE) {
            *v = GeofenceState::FLAG_EXIT; // return exit only if previously entered
            return NOERROR;
        }
    }
    *v = 0;
    return NOERROR;
}

ECode GeofenceState::GetDistanceToBoundary(
    /* [out] */ Double* v)
{
    VALIDATE_NOT_NULL(v)
    AutoPtr<IDouble> v1, v2;
    CDouble::New(mDistanceToCenter, (IDouble**)&v1);
    CDouble::New(Elastos::Core::Math::DOUBLE_MAX_VALUE, (IDouble**)&v2);
    Int32 result;
    IComparable::Probe(v1)->CompareTo(v2.Get(), &result);
    if (result == 0) {
        *v = Elastos::Core::Math::DOUBLE_MAX_VALUE;
    }
    else {
        Float radius;
        mFence->GetRadius(&radius);
        *v = Elastos::Core::Math::Abs(radius - mDistanceToCenter);
    }
    return NOERROR;
}

ECode GeofenceState::ToString(
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut)
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
    String fence;
    IObject::Probe(mFence)->ToString(&fence);
    StringBuilder sb(fence);
    sb += " d=";
    sb += mDistanceToCenter;
    sb += " ";
    sb += state;
    *strOut = sb.ToString();
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
