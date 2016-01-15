
#include "elastos/droid/hardware/location/GeofenceHardwareRequestParcelable.h"
#include "elastos/droid/hardware/location/GeofenceHardwareRequest.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_2(GeofenceHardwareRequestParcelable, Object, IGeofenceHardwareRequestParcelable, IParcelable)

GeofenceHardwareRequestParcelable::GeofenceHardwareRequestParcelable()
    : mId(0)
{
}

GeofenceHardwareRequestParcelable::~GeofenceHardwareRequestParcelable()
{
}

ECode GeofenceHardwareRequestParcelable::constructor()
{
    return NOERROR;
}

ECode GeofenceHardwareRequestParcelable::constructor(
    /* [in] */ Int32 id,
    /* [in] */ GeofenceHardwareRequest* request)
{
    mId = id;
    mRequest = request;
    return NOERROR;
}

ECode GeofenceHardwareRequestParcelable::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode GeofenceHardwareRequestParcelable::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude);

    return mRequest->GetLatitude(latitude);
}

ECode GeofenceHardwareRequestParcelable::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude);

    return mRequest->GetLongitude(longitude);
}

ECode GeofenceHardwareRequestParcelable::GetRadius(
    /* [out] */ Double* radius)
{
    VALIDATE_NOT_NULL(radius);

    return mRequest->GetRadius(radius);
}

ECode GeofenceHardwareRequestParcelable::GetMonitorTransitions(
    /* [out] */ Int32* transitions)
{
    VALIDATE_NOT_NULL(transitions);

    return mRequest->GetMonitorTransitions(transitions);
}

ECode GeofenceHardwareRequestParcelable::GetUnknownTimer(
    /* [out] */ Int32* timer)
{
    VALIDATE_NOT_NULL(timer);

    return mRequest->GetUnknownTimer(timer);
}

ECode GeofenceHardwareRequestParcelable::GetNotificationResponsiveness(
    /* [out] */ Int32* responsiveness)
{
    VALIDATE_NOT_NULL(responsiveness);

    return mRequest->GetNotificationResponsiveness(responsiveness);
}

ECode GeofenceHardwareRequestParcelable::GetLastTransition(
    /* [out] */ Int32* transition)
{
    VALIDATE_NOT_NULL(transition);

    return mRequest->GetLastTransition(transition);
}

ECode GeofenceHardwareRequestParcelable::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    return mRequest->GetType(type);
}

ECode GeofenceHardwareRequestParcelable::GetSourceTechnologies(
    /* [out] */ Int32* technologies)
{
    VALIDATE_NOT_NULL(technologies);

    return mRequest->GetSourceTechnologies(technologies);
}

ECode GeofenceHardwareRequestParcelable::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 type;
    GetType(&type);
    dest->WriteInt32(type);

    Double latitude;
    GetLatitude(&latitude);
    dest->WriteDouble(latitude);

    Double longitude;
    GetLongitude(&longitude);
    dest->WriteDouble(longitude);

    Double radius;
    GetRadius(&radius);
    dest->WriteDouble(radius);

    Int32 transition;
    GetLastTransition(&transition);
    dest->WriteInt32(transition);

    Int32 transitions;
    GetMonitorTransitions(&transitions);
    dest->WriteInt32(transitions);

    Int32 timer;
    GetUnknownTimer(&timer);
    dest->WriteInt32(timer);

    Int32 responsiveness;
    GetNotificationResponsiveness(&responsiveness);
    dest->WriteInt32(responsiveness);

    Int32 technologies;
    GetSourceTechnologies(&technologies);
    dest->WriteInt32(technologies);

    dest->WriteInt32(mId);

    return NOERROR;
}

ECode GeofenceHardwareRequestParcelable::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 type;
    source->ReadInt32(&type);
    mRequest->mType = type;

    Double latitude;
    source->ReadDouble(&latitude);
    mRequest->mLatitude = latitude;

    Double longitude;
    source->ReadDouble(&longitude);
    mRequest->mLongitude = longitude;

    Double radius;
    source->ReadDouble(&radius);
    mRequest->mRadius = radius;

    Int32 transition;
    source->ReadInt32(&transition);
    mRequest->mLastTransition = transition;

    Int32 transitions;
    source->ReadInt32(&transitions);
    mRequest->mMonitorTransitions = transitions;

    Int32 timer;
    source->ReadInt32(&timer);
    mRequest->mUnknownTimer = timer;

    Int32 responsiveness;
    source->ReadInt32(&responsiveness);
    mRequest->mNotificationResponsiveness = responsiveness;

    Int32 technologies;
    source->ReadInt32(&technologies);
    mRequest->mSourceTechnologies = technologies;

    source->ReadInt32(&mId);

    return NOERROR;
}

ECode GeofenceHardwareRequestParcelable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "id=";
    builder += mId;
    builder += ", type=";
    Int32 type;
    mRequest->GetType(&type);
    builder += type;
    builder += ", latitude=";
    Double latitude;
    mRequest->GetLatitude(&latitude);
    builder += latitude;
    builder += ", longitude=";
    Double longitude;
    mRequest->GetLongitude(&longitude);
    builder += longitude;
    builder += ", radius=";
    Double radius;
    mRequest->GetRadius(&radius);
    builder += radius;
    builder += ", lastTransition=";
    Int32 transition;
    mRequest->GetLastTransition(&transition);
    builder += transition;
    builder += ", unknownTimer=";
    Int32 timer;
    mRequest->GetUnknownTimer(&timer);
    builder += timer;
    builder += ", monitorTransitions=";
    Int32 transitions;
    mRequest->GetMonitorTransitions(&transitions);
    builder += transitions;
    builder += ", notificationResponsiveness=";
    Int32 responsiveness;
    mRequest->GetNotificationResponsiveness(&responsiveness);
    builder += responsiveness;
    builder += ", sourceTechnologies=";
    Int32 technologies;
    mRequest->GetSourceTechnologies(&technologies);
    builder += technologies;

    return builder.ToString(str);
}

} //Location
} //Hardware
} //Droid
} //Elastos