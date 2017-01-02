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

#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEXTENDEDPROPERTIES_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEXTENDEDPROPERTIES_H__

#include "_Elastos_Droid_Provider_CCalendarContractExtendedProperties.h"
#include "elastos/droid/provider/CalendarContractExtendedProperties.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields for accessing the Extended Properties. This is a generic set of
 * name/value pairs for use by sync adapters to add extra
 * information to events. There are three writable columns and all three
 * must be present when inserting a new value. They are:
 * <ul>
 * <li>{@link #EVENT_ID}</li>
 * <li>{@link #NAME}</li>
 * <li>{@link #VALUE}</li>
 * </ul>
 */
CarClass(CCalendarContractExtendedProperties)
    , public Singleton
    , public ICalendarContractExtendedProperties
    , public IBaseColumns
    , public ICalendarContractExtendedPropertiesColumns
    , public ICalendarContractEventsColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    // TODO: fill out this class when we actually start utilizing extendedproperties
    // in the calendar application.

    /**
     * The content:// style URL for the top-level calendar authority
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEXTENDEDPROPERTIES_H__
