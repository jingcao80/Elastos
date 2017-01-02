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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDLOOKUP_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDLOOKUP_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * @hide
 */
class HbpcdLookup
{
public:
    /**
     * @hide
     */
    class MccIdd //implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String MCC;
        static const String IDD;
    };

    /**
     * @hide
     */
    class MccLookup //implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String MCC;
        static const String COUNTRY_CODE;
        static const String COUNTRY_NAME;
        static const String NDD;
        static const String NANPS;
        static const String GMT_OFFSET_LOW;
        static const String GMT_OFFSET_HIGH;
        static const String GMT_DST_LOW;
        static const String GMT_DST_HIGH;
    };

    /**
     * @hide
     */
    class MccSidConflicts //implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String MCC;
        static const String SID_CONFLICT;
    };

    /**
     * @hide
     */
    class MccSidRange //implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String MCC;
        static const String RANGE_LOW;
        static const String RANGE_HIGH;
    };

    /**
     * @hide
     */
    class ArbitraryMccSidMatch //implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String MCC;
        static const String SID;
    };

    /**
     * @hide
     */
    class NanpAreaCode// implements BaseColumns
    {
    public:
        static AutoPtr<IUri> CONTENT_URI;
        static const String DEFAULT_SORT_ORDER;

        static const String AREA_CODE;
    };

public:
    static const String AUTHORITY;

    static AutoPtr<IUri> CONTENT_URI;

    static const String PATH_MCC_IDD;
    static const String PATH_MCC_LOOKUP_TABLE;
    static const String PATH_MCC_SID_CONFLICT;
    static const String PATH_MCC_SID_RANGE;
    static const String PATH_NANP_AREA_CODE;
    static const String PATH_ARBITRARY_MCC_SID_MATCH;
    static const String PATH_USERADD_COUNTRY;

    static const String ID;
    static const Int32 IDINDEX;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_HBPCDLOOKUP_H__
