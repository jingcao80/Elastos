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

#include "elastos/droid/internal/telephony/HbpcdUtils.h"
#include "elastos/droid/internal/telephony/HbpcdLookup.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

typedef HbpcdLookup::MccIdd MccIdd;
typedef HbpcdLookup::MccLookup MccLookup;
typedef HbpcdLookup::MccSidConflicts MccSidConflicts;
typedef HbpcdLookup::MccSidRange MccSidRange;
typedef HbpcdLookup::ArbitraryMccSidMatch ArbitraryMccSidMatch;

const String HbpcdUtils::TAG("HbpcdUtils");
const Boolean HbpcdUtils::DBG = FALSE;
CAR_INTERFACE_IMPL(HbpcdUtils, Object, IHbpcdUtils)
HbpcdUtils::HbpcdUtils(
    /* [in] */ IContext* context)
{
    context->GetContentResolver((IContentResolver**)&mResolver);
}

/**
 *  Resolves the unknown MCC with SID and Timezone information.
*/
ECode HbpcdUtils::GetMcc(
    /* [in] */ Int32 sid,
    /* [in] */ Int32 tz,
    /* [in] */ Int32 DSTflag,
    /* [in] */ Boolean isNitzTimeZone,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 tmpMcc = 0;

    // check if SID exists in arbitrary_mcc_sid_match table.
    // these SIDs are assigned to more than 1 operators, but they are known to
    // be used by a specific operator, other operators having the same SID are
    // not using it currently, if that SID is in this table, we don't need to
    // check other tables.
    AutoPtr<ArrayOf<String> > projection2 = ArrayOf<String>::Alloc(1);
    (*projection2)[0] = ArbitraryMccSidMatch::MCC;
    String sidStr = StringUtils::ToString(sid);
    AutoPtr<ICursor> c2;
    mResolver->Query(ArbitraryMccSidMatch::CONTENT_URI, projection2,
                ArbitraryMccSidMatch::SID + "=" + sidStr
                , NULL, String(NULL), (ICursor**)&c2);

    if (c2 != NULL) {
        Int32 c2Counter = 0;
        c2->GetCount(&c2Counter);
        if (DBG) {
            Logger::D(TAG, "Query unresolved arbitrary table, entries are %d", c2Counter);
        }
        if (c2Counter == 1) {
            if (DBG) {
                Logger::D(TAG, "Query Unresolved arbitrary returned the cursor %s", TO_CSTR(c2));
            }
            Boolean tmp = FALSE;
            c2->MoveToFirst(&tmp);
            c2->GetInt32(0, &tmpMcc);
            if (DBG) {
                Logger::D(TAG, "MCC found in arbitrary_mcc_sid_match: %d", tmpMcc);
            }
            ICloseable::Probe(c2)->Close();
            *result = tmpMcc;
            return NOERROR;
        }
        ICloseable::Probe(c2)->Close();
    }

    // Then check if SID exists in mcc_sid_conflict table.
    // and use the timezone in mcc_lookup table to check which MCC matches.
    AutoPtr<ArrayOf<String> > projection3 = ArrayOf<String>::Alloc(1);
    (*projection3)[0] = MccSidConflicts::MCC;
    String tzStr = StringUtils::ToString(tz);
    String DSTflagStr = StringUtils::ToString(DSTflag);
    AutoPtr<ICursor> c3;
    mResolver->Query(MccSidConflicts::CONTENT_URI, projection3,
            MccSidConflicts::SID_CONFLICT + "=" + sidStr + " and (((" +
            MccLookup::GMT_OFFSET_LOW + "<=" + tzStr + ") and (" + tzStr + "<=" +
            MccLookup::GMT_OFFSET_HIGH + ") and (" + "0=" + DSTflagStr + ")) or ((" +
            MccLookup::GMT_DST_LOW + "<=" + tzStr + ") and (" + tzStr + "<=" +
            MccLookup::GMT_DST_HIGH + ") and (" + "1=" + DSTflagStr + ")))",
                    NULL, String(NULL), (ICursor**)&c3);
    if (c3 != NULL) {
        Int32 c3Counter = 0;
        c3->GetCount(&c3Counter);
        if (c3Counter > 0) {
            if (c3Counter > 1) {
                Logger::W(TAG, "something wrong, get more results for 1 conflict SID: %s", TO_CSTR(c3));
            }
            if (DBG) Logger::D(TAG, "Query conflict sid returned the cursor %s", TO_CSTR(c3));
            Boolean tmp = FALSE;
            c3->MoveToFirst(&tmp);
            c3->GetInt32(0, &tmpMcc);
            if (DBG) Logger::D(TAG,
                    "MCC found in mcc_lookup_table. Return tmpMcc = %d", tmpMcc);
            ICloseable::Probe(c3)->Close();
            if (isNitzTimeZone) {
                *result = tmpMcc;
                return NOERROR;
            }
            else {
                // time zone is not accurate, it may get wrong mcc, ignore it.
                if (DBG) Logger::D(TAG, "time zone is not accurate, mcc may be %d", tmpMcc);
                *result = 0;
                return NOERROR;
            }
        }
    }

    // if there is no conflict, then check if SID is in mcc_sid_range.
    AutoPtr<ArrayOf<String> > projection5 = ArrayOf<String>::Alloc(1);
    (*projection5)[0] = MccSidRange::MCC;
    AutoPtr<ICursor> c5;
    mResolver->Query(MccSidRange::CONTENT_URI, projection5,
            MccSidRange::RANGE_LOW + "<=" + sidStr + " and " +
            MccSidRange::RANGE_HIGH + ">=" + sidStr,
            NULL, String(NULL), (ICursor**)&c5);
    if (c5 != NULL) {
        Int32 count = 0;
        if ((c5->GetCount(&count), count) > 0) {
            if (DBG) Logger::D(TAG, "Query Range returned the cursor %s", TO_CSTR(c5));
            Boolean tmp = FALSE;
            c5->MoveToFirst(&tmp);
            c5->GetInt32(0, &tmpMcc);
            if (DBG) Logger::D(TAG, "SID found in mcc_sid_range. Return tmpMcc = %d", tmpMcc);
            ICloseable::Probe(c5)->Close();
            *result = tmpMcc;
            return NOERROR;
        }
        ICloseable::Probe(c5)->Close();
    }
    if (DBG) Logger::D(TAG, "SID NOT found in mcc_sid_range.");

    if (DBG) Logger::D(TAG, "Exit getMccByOtherFactors. Return tmpMcc =  %d", tmpMcc);
    // If unknown MCC still could not be resolved,
    *result = tmpMcc;
    return NOERROR;
}

/**
 *  Gets country information with given MCC.
*/
ECode HbpcdUtils::GetIddByMcc(
    /* [in] */ Int32 mcc,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "Enter getHbpcdInfoByMCC.");
    String idd("");

    AutoPtr<ICursor> c;

    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = MccIdd::IDD;
    AutoPtr<ICursor> cur;
    mResolver->Query(MccIdd::CONTENT_URI, projection,
            MccIdd::MCC + "=" + StringUtils::ToString(mcc), NULL, String(NULL), (ICursor**)&cur);
    if (cur != NULL) {
        Int32 count = 0;
        if ((cur->GetCount(&count), count) > 0) {
            if (DBG) Logger::D(TAG, "Query Idd returned the cursor %s", TO_CSTR(cur));
            // TODO: for those country having more than 1 IDDs, need more information
            // to decide which IDD would be used. currently just use the first 1.
            Boolean tmp = FALSE;
            cur->MoveToFirst(&tmp);
            cur->GetString(0, &idd);
            if (DBG) Logger::D(TAG, "IDD = %s", idd.string());

        }
        ICloseable::Probe(cur)->Close();
    }
    if (c != NULL) ICloseable::Probe(c)->Close();

    if (DBG) Logger::D(TAG, "Exit getHbpcdInfoByMCC.");
    *result = idd;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
