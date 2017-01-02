//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
