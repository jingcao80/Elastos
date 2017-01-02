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

#ifndef __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTEXTENDEDPROPERTIES_H__
#define __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTEXTENDEDPROPERTIES_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::IUri;

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
class CalendarContractExtendedProperties
{
public:
    /**
     * The content:// style URL for the top-level calendar authority
     */
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CALENDARCONTRACTEXTENDEDPROPERTIES_H__
