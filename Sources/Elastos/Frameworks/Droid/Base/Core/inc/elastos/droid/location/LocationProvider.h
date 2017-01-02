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

#ifndef __ELASTOS_DROID_LOCATION_LOCATIONPROVIDER_H__
#define __ELASTOS_DROID_LOCATION_LOCATIONPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * An abstract superclass for location providers.  A location provider
 * provides periodic reports on the geographical location of the
 * device.
 *
 * <p> Each provider has a set of criteria under which it may be used;
 * for example, some providers require GPS hardware and visibility to
 * a number of satellites; others require the use of the cellular
 * radio, or access to a specific carrier's network, or to the
 * internet.  They may also have different battery consumption
 * characteristics or monetary costs to the user.  The {@link
 * Criteria} class allows providers to be selected based on
 * user-specified criteria.
 */
class LocationProvider
    : public Object
    , public ILocationProvider
{
public:
    CAR_INTERFACE_DECL()

    LocationProvider();

    /**
     * Constructs a LocationProvider with the given name.   Provider names must
     * consist only of the characters [a-zA-Z0-9].
     *
     * @throws IllegalArgumentException if name contains an illegal character
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties);

    /**
     * Returns the name of this provider.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns true if this provider meets the given criteria,
     * false otherwise.
     */
    CARAPI MeetsCriteria(
        /* [in] */ ICriteria* criteria,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    static CARAPI_(Boolean) PropertiesMeetCriteria(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties,
        /* [in] */ ICriteria* criteria);

    /**
     * Returns true if the provider requires access to a
     * data network (e.g., the Internet), false otherwise.
     */
    CARAPI RequiresNetwork(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the provider requires access to a
     * satellite-based positioning system (e.g., GPS), false
     * otherwise.
     */
    CARAPI RequiresSatellite(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the provider requires access to an appropriate
     * cellular network (e.g., to make use of cell tower IDs), false
     * otherwise.
     */
    CARAPI RequiresCell(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the use of this provider may result in a
     * monetary charge to the user, false if use is free.  It is up to
     * each provider to give accurate information.
     */
    CARAPI HasMonetaryCost(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the provider is able to provide altitude
     * information, false otherwise.  A provider that reports altitude
     * under most circumstances but may occassionally not report it
     * should return true.
     */
    CARAPI SupportsAltitude(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the provider is able to provide speed
     * information, false otherwise.  A provider that reports speed
     * under most circumstances but may occassionally not report it
     * should return true.
     */
    CARAPI SupportsSpeed(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the provider is able to provide bearing
     * information, false otherwise.  A provider that reports bearing
     * under most circumstances but may occassionally not report it
     * should return true.
     */
    CARAPI SupportsBearing(
        /* [out] */ Boolean* result);

    /**
     * Returns the power requirement for this provider.
     *
     * @return the power requirement for this provider, as one of the
     * constants Criteria.POWER_REQUIREMENT_*.
     */
    CARAPI GetPowerRequirement(
        /* [out] */ Int32* pr);

    /**
     * Returns a constant describing horizontal accuracy of this provider.
     * If the provider returns finer grain or exact location,
     * {@link Criteria#ACCURACY_FINE} is returned, otherwise if the
     * location is only approximate then {@link Criteria#ACCURACY_COARSE}
     * is returned.
     */
    CARAPI GetAccuracy(
        /* [out] */ Int32* accuracy);

private:
    String mName;
    AutoPtr<IProviderProperties> mProperties;
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_LOCATIONPROVIDER_H__
