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

#ifndef __ELASTOS_DROID_OS_CBATTERYMANAGER_H__
#define __ELASTOS_DROID_OS_CBATTERYMANAGER_H__

#include "_Elastos_Droid_Os_CBatteryManager.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * The BatteryManager class contains strings and constants used for values
 * in the {@link android.content.Intent#ACTION_BATTERY_CHANGED} Intent, and
 * provides a method for querying battery and charging properties.
 */
CarClass(CBatteryManager)
    , public Object
    , public IBatteryManager
{
public:
    CBatteryManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Return the value of a battery property of integer type.  If the
     * platform does not provide the property queried, this value will
     * be Integer.MIN_VALUE.
     *
     * @param id identifier of the requested property
     *
     * @return the property value, or Integer.MIN_VALUE if not supported.
     */
    CARAPI GetInt32Property(
        /* [in] */ Int32 id,
        /* [out] */ Int32* result);

    /**
     * Return the value of a battery property of long type If the
     * platform does not provide the property queried, this value will
     * be Long.MIN_VALUE.
     *
     * @param id identifier of the requested property
     *
     * @return the property value, or Long.MIN_VALUE if not supported.
     */
    CARAPI GetInt64Property(
        /* [in] */ Int32 id,
        /* [out] */ Int64* result);

private:
    /**
     * Query a battery property from the batteryproperties service.
     *
     * Returns the requested value, or Long.MIN_VALUE if property not
     * supported on this system or on other error.
     */
    CARAPI_(Int64) QueryProperty(
        /* [in] */ Int32 id);

private:
    AutoPtr<IIBatteryPropertiesRegistrar> mBatteryPropertiesRegistrar;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBATTERYMANAGER_H__
