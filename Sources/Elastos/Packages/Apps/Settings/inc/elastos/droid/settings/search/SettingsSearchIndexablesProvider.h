

package com.android.settings.search;

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexablesProvider;

using Elastos::Utility::ICollection;

using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_RANK;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_RESID;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_CLASS_NAME;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_ICON_RESID;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_ACTION;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS;

using static android::Provider::SearchIndexablesContract::IINDEXABLES_XML_RES_COLUMNS;
using static android::Provider::SearchIndexablesContract::IINDEXABLES_RAW_COLUMNS;
using static android::Provider::SearchIndexablesContract::INON_INDEXABLES_KEYS_COLUMNS;

public class SettingsSearchIndexablesProvider extends SearchIndexablesProvider {
    private static const String TAG = "SettingsSearchIndexablesProvider";

    //@Override
    public Boolean OnCreate() {
        return TRUE;
    }

    //@Override
    public Cursor QueryXmlResources(String[] projection) {
        MatrixCursor cursor = new MatrixCursor(INDEXABLES_XML_RES_COLUMNS);
        Collection<SearchIndexableResource> values = SearchIndexableResources->Values();
        for (SearchIndexableResource val : values) {
            Object[] ref = new Object[7];
            ref[COLUMN_INDEX_XML_RES_RANK] = val.rank;
            ref[COLUMN_INDEX_XML_RES_RESID] = val.xmlResId;
            ref[COLUMN_INDEX_XML_RES_CLASS_NAME] = val.className;
            ref[COLUMN_INDEX_XML_RES_ICON_RESID] = val.iconResId;
            ref[COLUMN_INDEX_XML_RES_INTENT_ACTION] = NULL; // intent action
            ref[COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE] = NULL; // intent target package
            ref[COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS] = NULL; // intent target class
            cursor->AddRow(ref);
        }
        return cursor;
    }

    //@Override
    public Cursor QueryRawData(String[] projection) {
        MatrixCursor result = new MatrixCursor(INDEXABLES_RAW_COLUMNS);
        return result;
    }

    //@Override
    public Cursor QueryNonIndexableKeys(String[] projection) {
        MatrixCursor cursor = new MatrixCursor(NON_INDEXABLES_KEYS_COLUMNS);
        return cursor;
    }
}
