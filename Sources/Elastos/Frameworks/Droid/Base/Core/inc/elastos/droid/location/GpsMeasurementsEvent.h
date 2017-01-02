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

#ifndef __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__
#define __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::Object;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class implementing a container for data associated with a measurement event.
 * Events are delivered to registered instances of {@link Listener}.
 *
 * @hide
 */
class GpsMeasurementsEvent
    : public Object
    , public IGpsMeasurementsEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsMeasurementsEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGpsClock* clock,
        /* [in] */ ArrayOf<IGpsMeasurement*>* measurements);

    // @NonNull
    CARAPI GetClock(
        /* [out] */ IGpsClock** clock);

    /**
     * Gets a read-only collection of measurements associated with the current event.
     */
    // @NonNull
    CARAPI GetMeasurements(
        /* [out] */ ICollection** coll);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IGpsClock> mClock;
    AutoPtr<ICollection> mReadOnlyMeasurements;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSMEASUREMENTEVENT_H__
