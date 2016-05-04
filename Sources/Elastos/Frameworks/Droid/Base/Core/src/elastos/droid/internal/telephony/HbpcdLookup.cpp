/*
**
** Copyright 2014, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
package com.android.internal.telephony;

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IBaseColumns;

/**
 * @hide
 */
public class HbpcdLookup {
    public static const String AUTHORITY = "hbpcd_lookup";

    public static const Uri CONTENT_URI =
        Uri->Parse("content://" + AUTHORITY);

    public static const String PATH_MCC_IDD = "idd";
    public static const String PATH_MCC_LOOKUP_TABLE = "lookup";
    public static const String PATH_MCC_SID_CONFLICT = "conflict";
    public static const String PATH_MCC_SID_RANGE = "range";
    public static const String PATH_NANP_AREA_CODE = "nanp";
    public static const String PATH_ARBITRARY_MCC_SID_MATCH = "arbitrary";
    public static const String PATH_USERADD_COUNTRY = "useradd";

    public static const String ID = "_id";
    public static const Int32 IDINDEX = 0;

    /**
     * @hide
     */
    public static class MccIdd implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_MCC_IDD);
        public static const String DEFAULT_SORT_ORDER = "MCC ASC";

        public static const String MCC = "MCC";
        public static const String IDD = "IDD";

    }

    /**
     * @hide
     */
    public static class MccLookup implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_MCC_LOOKUP_TABLE);
        public static const String DEFAULT_SORT_ORDER = "MCC ASC";

        public static const String MCC = "MCC";
        public static const String COUNTRY_CODE = "Country_Code";
        public static const String COUNTRY_NAME = "Country_Name";
        public static const String NDD = "NDD";
        public static const String NANPS = "NANPS";
        public static const String GMT_OFFSET_LOW = "GMT_Offset_Low";
        public static const String GMT_OFFSET_HIGH = "GMT_Offset_High";
        public static const String GMT_DST_LOW = "GMT_DST_Low";
        public static const String GMT_DST_HIGH = "GMT_DST_High";

    }

    /**
     * @hide
     */
    public static class MccSidConflicts implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_MCC_SID_CONFLICT);
        public static const String DEFAULT_SORT_ORDER = "MCC ASC";

        public static const String MCC = "MCC";
        public static const String SID_CONFLICT = "SID_Conflict";

    }

    /**
     * @hide
     */
    public static class MccSidRange implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_MCC_SID_RANGE);
        public static const String DEFAULT_SORT_ORDER = "MCC ASC";

        public static const String MCC = "MCC";
        public static const String RANGE_LOW = "SID_Range_Low";
        public static const String RANGE_HIGH = "SID_Range_High";
    }

    /**
     * @hide
     */
    public static class ArbitraryMccSidMatch implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_ARBITRARY_MCC_SID_MATCH);
        public static const String DEFAULT_SORT_ORDER = "MCC ASC";

        public static const String MCC = "MCC";
        public static const String SID = "SID";

    }

    /**
     * @hide
     */
    public static class NanpAreaCode implements BaseColumns {
        public static const Uri CONTENT_URI =
            Uri->Parse("content://" + AUTHORITY + "/" + PATH_NANP_AREA_CODE);
        public static const String DEFAULT_SORT_ORDER = "Area_Code ASC";

        public static const String AREA_CODE = "Area_Code";
    }
}
