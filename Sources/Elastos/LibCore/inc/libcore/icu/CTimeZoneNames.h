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

#ifndef __LIBCORE_ICU_CTIMEZONENAMES_H__
#define __LIBCORE_ICU_CTIMEZONENAMES_H__

#include "_Libcore_ICU_CTimeZoneNames.h"
#include "Singleton.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CTimeZoneNames)
    , public Singleton
    , public ITimeZoneNames
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns the appropriate string from 'zoneStrings'. Used with getZoneStrings.
     */
    CARAPI GetDisplayName(
        /* [in] */ ArrayOf<IArrayOf*>*  zoneStrings,
        /* [in] */ const String& id,
        /* [in] */ Boolean daylight,
        /* [in] */ Int32 style ,
        /* [out] */ String * str);

    /**
     * Returns an array of time zone strings, as used by DateFormatSymbols.getZoneStrings.
     */
    CARAPI GetZoneStrings(
        /* [in] */ ILocale * locale,
        /* [out, callee] */ ArrayOf<IArrayOf*> ** outarray);

    /**
     * Returns an array containing the time zone ids in use in the country corresponding to
     * the given locale. This is not necessary for Java API, but is used by telephony as a
     * fallback. We retrieve these strings from zone.tab rather than icu4c because the latter
     * supplies them in alphabetical order where zone.tab has them in a kind of "importance"
     * order (as defined in the zone.tab header).
     */
    CARAPI ForLocale(
        /* [in] */ ILocale * locale ,
        /* [out, callee] */ ArrayOf<String> ** outarray);

    CARAPI GetExemplarLocation(
        /* [in] */ const String& locale,
        /* [in] */ const String& tz,
        /* [out] */ String* location);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CTIMEZONENAMES_H__
