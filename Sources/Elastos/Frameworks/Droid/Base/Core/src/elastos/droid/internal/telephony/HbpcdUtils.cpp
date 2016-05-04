/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;

using Elastos::Droid::Internal::Telephony::IHbpcdLookup;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccIdd;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccLookup;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccSidConflicts;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccSidRange;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IArbitraryMccSidMatch;

public class HbpcdUtils {
    private static const String LOG_TAG = "HbpcdUtils";
    private static const Boolean DBG = FALSE;
    private ContentResolver resolver = NULL;

    public HbpcdUtils(Context context) {
        resolver = context->GetContentResolver();
    }

    /**
     *  Resolves the unknown MCC with SID and Timezone information.
    */
    public Int32 GetMcc(Int32 sid, Int32 tz, Int32 DSTflag, Boolean isNitzTimeZone) {
        Int32 tmpMcc = 0;

        // check if SID exists in arbitrary_mcc_sid_match table.
        // these SIDs are assigned to more than 1 operators, but they are known to
        // be used by a specific operator, other operators having the same SID are
        // not using it currently, if that SID is in this table, we don't need to
        // check other tables.
        String projection2[] = {ArbitraryMccSidMatch.MCC};
        Cursor c2 = resolver->Query(ArbitraryMccSidMatch.CONTENT_URI, projection2,
                            ArbitraryMccSidMatch.SID + "=" + sid, NULL, NULL);

        If (c2 != NULL) {
            Int32 c2Counter = c2->GetCount();
            If (DBG) {
                Logger::D(LOG_TAG, "Query unresolved arbitrary table, entries are " + c2Counter);
            }
            If (c2Counter == 1) {
                If (DBG) {
                    Logger::D(LOG_TAG, "Query Unresolved arbitrary returned the cursor " + c2 );
                }
                c2->MoveToFirst();
                tmpMcc = c2->GetInt(0);
                If (DBG) {
                    Logger::D(LOG_TAG, "MCC found in arbitrary_mcc_sid_match: " + tmpMcc);
                }
                c2->Close();
                return tmpMcc;
            }
            c2->Close();
        }

        // Then check if SID exists in mcc_sid_conflict table.
        // and use the timezone in mcc_lookup table to check which MCC matches.
        String projection3[] = {MccSidConflicts.MCC};
        Cursor c3 = resolver->Query(MccSidConflicts.CONTENT_URI, projection3,
                MccSidConflicts.SID_CONFLICT + "=" + sid + " And (((" +
                MccLookup.GMT_OFFSET_LOW + "<=" + tz + ") And (" + tz + "<=" +
                MccLookup.GMT_OFFSET_HIGH + ") And (" + "0=" + DSTflag + ")) Or ((" +
                MccLookup.GMT_DST_LOW + "<=" + tz + ") And (" + tz + "<=" +
                MccLookup.GMT_DST_HIGH + ") And (" + "1=" + DSTflag + ")))",
                        NULL, NULL);
        If (c3 != NULL) {
            Int32 c3Counter = c3->GetCount();
            If (c3Counter > 0) {
                If (c3Counter > 1) {
                    Logger::W(LOG_TAG, "something wrong, get more results for 1 conflict SID: " + c3);
                }
                If (DBG) Logger::D(LOG_TAG, "Query conflict sid returned the cursor " + c3 );
                c3->MoveToFirst();
                tmpMcc = c3->GetInt(0);
                If (DBG) Logger::D(LOG_TAG,
                        "MCC found in mcc_lookup_table. Return tmpMcc = " + tmpMcc);
                c3->Close();
                If (isNitzTimeZone) {
                    return tmpMcc;
                } else {
                    // time zone is not accurate, it may get wrong mcc, ignore it.
                    If (DBG) Logger::D(LOG_TAG, "time zone is not accurate, mcc may be "
                            + tmpMcc);
                        return 0;
                }
            }
        }

        // if there is no conflict, then check if SID is in mcc_sid_range.
        String projection5[] = {MccSidRange.MCC};
        Cursor c5 = resolver->Query(MccSidRange.CONTENT_URI, projection5,
                MccSidRange.RANGE_LOW + "<=" + sid + " and " +
                MccSidRange.RANGE_HIGH + ">=" + sid,
                NULL, NULL);
        If (c5 != NULL) {
            If (c5->GetCount() > 0) {
                If (DBG) Logger::D(LOG_TAG, "Query Range returned the cursor " + c5 );
                c5->MoveToFirst();
                tmpMcc = c5->GetInt(0);
                If (DBG) Logger::D(LOG_TAG, "SID found in mcc_sid_range. Return tmpMcc = " + tmpMcc);
                c5->Close();
                return tmpMcc;
            }
            c5->Close();
        }
        If (DBG) Logger::D(LOG_TAG, "SID NOT found in mcc_sid_range.");

        If (DBG) Logger::D(LOG_TAG, "Exit getMccByOtherFactors. Return tmpMcc =  " + tmpMcc );
        // If unknown MCC still could not be resolved,
        return tmpMcc;
    }

    /**
     *  Gets country information with given MCC.
    */
    public String GetIddByMcc(Int32 mcc) {
        If (DBG) Logger::D(LOG_TAG, "Enter getHbpcdInfoByMCC.");
        String idd = "";

        Cursor c = NULL;

        String projection[] = {MccIdd.IDD};
        Cursor cur = resolver->Query(MccIdd.CONTENT_URI, projection,
                MccIdd.MCC + "=" + mcc, NULL, NULL);
        If (cur != NULL) {
            If (cur->GetCount() > 0) {
                If (DBG) Logger::D(LOG_TAG, "Query Idd returned the cursor " + cur );
                // TODO: for those country having more than 1 IDDs, need more information
                // to decide which IDD would be used. currently just use the first 1.
                cur->MoveToFirst();
                idd = cur->GetString(0);
                If (DBG) Logger::D(LOG_TAG, "IDD = " + idd);

            }
            cur->Close();
        }
        If (c != NULL) c->Close();

        If (DBG) Logger::D(LOG_TAG, "Exit getHbpcdInfoByMCC.");
        return idd;
    }
}
