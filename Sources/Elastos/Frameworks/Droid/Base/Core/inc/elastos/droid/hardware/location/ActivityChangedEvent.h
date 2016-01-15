
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYCHANGEEVENT_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYCHANGEEVENT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;
using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class ActivityChangedEvent
    : public Object
    , public IActivityChangedEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ActivityChangedEvent();

    virtual ~ActivityChangedEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IActivityRecognitionEvent*>* activityRecognitionEvents);

    CARAPI GetActivityRecognitionEvents(
        /* [out] */ IIterable** result);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Creator<ActivityChangedEvent> CREATOR =
    //         new Creator<ActivityChangedEvent>() {
    //     @Override
    //     public ActivityChangedEvent createFromParcel(Parcel source) {
    //         int activityRecognitionEventsLength = source.readInt();
    //         ActivityRecognitionEvent[] activityRecognitionEvents =
    //                 new ActivityRecognitionEvent[activityRecognitionEventsLength];
    //         source.readTypedArray(activityRecognitionEvents, ActivityRecognitionEvent.CREATOR);

    //         return new ActivityChangedEvent(activityRecognitionEvents);
    //     }

    //     @Override
    //     public ActivityChangedEvent[] newArray(int size) {
    //         return new ActivityChangedEvent[size];
    //     }
    // };

private:
    AutoPtr<IList> mActivityRecognitionEvents;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYCHANGEEVENT_H__
