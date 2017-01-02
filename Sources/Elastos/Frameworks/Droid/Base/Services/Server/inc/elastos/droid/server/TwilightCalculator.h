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

#ifndef __ELASTOS_DROID_SERVER_TWILIGHTCALCULATOR_H__
#define __ELASTOS_DROID_SERVER_TWILIGHTCALCULATOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Math.h>
#include <elastos/core/Object.h>

using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Server {

/** @hide */
class TwilightCalculator
    : public Object
{
public:
    TwilightCalculator();

    /**
     * calculates the civil twilight bases on time and geo-coordinates.
     *
     * @param time time in milliseconds.
     * @param latiude latitude in degrees.
     * @param longitude latitude in degrees.
     */
    CARAPI_(void) CalculateTwilight(
        /* [in] */ Int64 time,
        /* [in] */ Double latiude,
        /* [in] */ Double longitude);

    CARAPI_(Int64) GetSunset();

    CARAPI_(Int64) GetSunrise();

    CARAPI_(Int32) GetState();

public:
    /** Value of {@link #mState} if it is currently day */
    static const Int32 DAY = 0;

    /** Value of {@link #mState} if it is currently night */
    static const Int32 NIGHT = 1;

private:
    static const Float DEGREES_TO_RADIANS;

    // element for calculating solar transit.
    static const Float J0;

    // correction for civil twilight
    static const Float ALTIDUTE_CORRECTION_CIVIL_TWILIGHT;

    // coefficients for calculating Equation of Center.
    static const Float C1;
    static const Float C2;
    static const Float C3;

    static const Float OBLIQUITY;

    // Java time on Jan 1, 2000 12:00 UTC.
    static const Int64 UTC_2000;

    /**
     * Time of sunset (civil twilight) in milliseconds or -1 in the case the day
     * or night never ends.
     */
    Int64 mSunset;

    /**
     * Time of sunrise (civil twilight) in milliseconds or -1 in the case the
     * day or night never ends.
     */
    Int64 mSunrise;

    /** Current state */
    Int32 mState;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TWILIGHTCALCULATOR_H__
