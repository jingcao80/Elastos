
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONEVENT_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONEVENT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class ActivityRecognitionEvent
    : public Object
    , public IActivityRecognitionEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ActivityRecognitionEvent();

    virtual ~ActivityRecognitionEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& activity,
        /* [in] */ Int32 eventType,
        /* [in] */ Int64 timestampNs);

    CARAPI GetActivity(
        /* [out] */ String* activity);

    CARAPI GetEventType(
        /* [out] */ Int32* type);

    CARAPI GetTimestampNs(
        /* [out] */ Int64* times);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Creator<ActivityRecognitionEvent> CREATOR =
    //         new Creator<ActivityRecognitionEvent>() {
    //     @Override
    //     public ActivityRecognitionEvent createFromParcel(Parcel source) {
    //         String activity = source.readString();
    //         int eventType = source.readInt();
    //         long timestampNs = source.readLong();

    //         return new ActivityRecognitionEvent(activity, eventType, timestampNs);
    //     }

    //     @Override
    //     public ActivityRecognitionEvent[] newArray(int size) {
    //         return new ActivityRecognitionEvent[size];
    //     }
    // };

private:
    String mActivity;

    Int32 mEventType;

    Int64 mTimestampNs;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONEVENT_H__
