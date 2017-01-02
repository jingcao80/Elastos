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

#ifndef __ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__
#define __ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__

#include "_Elastos_Droid_Hardware_CCarrierFrequencyRange.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CCarrierFrequencyRange)
    , public Object
    , public ICarrierFrequencyRange
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

     /**
     * Create a segment of a carrier frequency range.
     *
     * @param min The minimum transmittable frequency in this range segment.
     * @param max The maximum transmittable frequency in this range segment.
     */
    CARAPI constructor(
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    /**
     * Get the minimum (inclusive) frequency in this range segment.
     */
    CARAPI GetMinFrequency(
        /* [out] */ Int32* value);

    /**
     * Get the maximum (inclusive) frequency in this range segment.
     */
    CARAPI GetMaxFrequency(
        /* [out] */ Int32* value);

private:
    Int32 mMinFrequency;
    Int32 mMaxFrequency;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__
