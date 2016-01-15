
#include "elastos/droid/hardware/location/ActivityRecognitionEvent.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_2(ActivityRecognitionEvent, Object, IActivityRecognitionEvent, IParcelable)

ActivityRecognitionEvent::ActivityRecognitionEvent()
    : mActivity(NULL)
    , mEventType(0)
    , mTimestampNs(0)
{
}

ActivityRecognitionEvent::~ActivityRecognitionEvent()
{
}

ECode ActivityRecognitionEvent::constructor()
{
    return NOERROR;
}

ECode ActivityRecognitionEvent::constructor(
    /* [in] */ const String& activity,
    /* [in] */ Int32 eventType,
    /* [in] */ Int64 timestampNs)
{
    mActivity = activity;
    mEventType = eventType;
    mTimestampNs = timestampNs;
    return NOERROR;
}

ECode ActivityRecognitionEvent::GetActivity(
    /* [out] */ String* activity)
{
    VALIDATE_NOT_NULL(activity);

    *activity = mActivity;
    return NOERROR;
}

ECode ActivityRecognitionEvent::GetEventType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mEventType;
    return NOERROR;
}

ECode ActivityRecognitionEvent::GetTimestampNs(
    /* [out] */ Int64* times)
{
    VALIDATE_NOT_NULL(times);

    *times = mTimestampNs;
    return NOERROR;
}

ECode ActivityRecognitionEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mActivity);
    dest->WriteInt32(mEventType);
    dest->WriteInt64(mTimestampNs);
    return NOERROR;
}

ECode ActivityRecognitionEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mActivity);
    source->ReadInt32(&mEventType);
    source->ReadInt64(&mTimestampNs);
    return NOERROR;
}

ECode ActivityRecognitionEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "Activity=";
    builder += mActivity;
    builder += ", EventType=";
    builder += mEventType;
    builder += ", TimestampNs=";
    builder += mTimestampNs;

    return builder.ToString(str);
}

} //Location
} //Hardware
} //Droid
} //Elastos