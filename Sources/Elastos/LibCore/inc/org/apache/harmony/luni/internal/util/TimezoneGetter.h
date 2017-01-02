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

#ifndef __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_H__
#define __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_H__

//#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Apache.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

/**
 * This class provides a way to add an implementation specific way to
 * access the current timezone.
 */
class TimezoneGetter
    : public Object
    , public ITimezoneGetter
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Retrieves the singleton instance of this class.
     *
     * @return TimezoneGetter the single instance of this class.
     */
    static AutoPtr<ITimezoneGetter> GetInstance();

    /**
     * Sets the singleton instance of this class.
     *
     * @param instance
     *            TimezoneGetter the single instance of this class.
     */
    static CARAPI SetInstance(
        /* [in] */ ITimezoneGetter* getter);

    /**
     * Retrieves the ID of the current time zone.
     *
     * @return String the ID of the current time zone.
     */
    virtual CARAPI GetId(
        /* [out] */ String* result) = 0;

private:
    static AutoPtr<ITimezoneGetter> instance;

};

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif //__ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_H__
