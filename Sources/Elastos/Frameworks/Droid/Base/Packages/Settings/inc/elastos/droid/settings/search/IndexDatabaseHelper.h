

package com.android.settings.search;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Utility::ILog;

public class IndexDatabaseHelper extends SQLiteOpenHelper {

    private static const String TAG = "IndexDatabaseHelper";

    private static const String DATABASE_NAME = "search_index.db";
    private static const Int32 DATABASE_VERSION = 115;

    public interface Tables {
        public static const String TABLE_PREFS_INDEX = "prefs_index";
        public static const String TABLE_META_INDEX = "meta_index";
        public static const String TABLE_SAVED_QUERIES = "saved_queries";
    }

    public interface IndexColumns {
        public static const String DOCID = "docid";
        public static const String LOCALE = "locale";
        public static const String DATA_RANK = "data_rank";
        public static const String DATA_TITLE = "data_title";
        public static const String DATA_TITLE_NORMALIZED = "data_title_normalized";
        public static const String DATA_SUMMARY_ON = "data_summary_on";
        public static const String DATA_SUMMARY_ON_NORMALIZED = "data_summary_on_normalized";
        public static const String DATA_SUMMARY_OFF = "data_summary_off";
        public static const String DATA_SUMMARY_OFF_NORMALIZED = "data_summary_off_normalized";
        public static const String DATA_ENTRIES = "data_entries";
        public static const String DATA_KEYWORDS = "data_keywords";
        public static const String CLASS_NAME = "class_name";
        public static const String SCREEN_TITLE = "screen_title";
        public static const String INTENT_ACTION = "intent_action";
        public static const String INTENT_TARGET_PACKAGE = "intent_target_package";
        public static const String INTENT_TARGET_CLASS = "intent_target_class";
        public static const String ICON = "icon";
        public static const String ENABLED = "enabled";
        public static const String DATA_KEY_REF = "data_key_reference";
        public static const String USER_ID = "user_id";
    }

    public interface MetaColumns {
        public static const String BUILD = "build";
    }

    public interface SavedQueriesColums  {
        public static const String QUERY = "query";
        public static const String TIME_STAMP = "timestamp";
    }

    private static const String CREATE_INDEX_TABLE =
            "CREATE VIRTUAL TABLE " + Tables.TABLE_PREFS_INDEX + " USING fts4" +
                    "(" +
                    IndexColumns.LOCALE +
                    ", " +
                    IndexColumns.DATA_RANK +
                    ", " +
                    IndexColumns.DATA_TITLE +
                    ", " +
                    IndexColumns.DATA_TITLE_NORMALIZED +
                    ", " +
                    IndexColumns.DATA_SUMMARY_ON +
                    ", " +
                    IndexColumns.DATA_SUMMARY_ON_NORMALIZED +
                    ", " +
                    IndexColumns.DATA_SUMMARY_OFF +
                    ", " +
                    IndexColumns.DATA_SUMMARY_OFF_NORMALIZED +
                    ", " +
                    IndexColumns.DATA_ENTRIES +
                    ", " +
                    IndexColumns.DATA_KEYWORDS +
                    ", " +
                    IndexColumns.SCREEN_TITLE +
                    ", " +
                    IndexColumns.CLASS_NAME +
                    ", " +
                    IndexColumns.ICON +
                    ", " +
                    IndexColumns.INTENT_ACTION +
                    ", " +
                    IndexColumns.INTENT_TARGET_PACKAGE +
                    ", " +
                    IndexColumns.INTENT_TARGET_CLASS +
                    ", " +
                    IndexColumns.ENABLED +
                    ", " +
                    IndexColumns.DATA_KEY_REF +
                    ", " +
                    IndexColumns.USER_ID +
                    ");";

    private static const String CREATE_META_TABLE =
            "CREATE TABLE " + Tables.TABLE_META_INDEX +
                    "(" +
                    MetaColumns.BUILD + " VARCHAR(32) NOT NULL" +
                    ")";

    private static const String CREATE_SAVED_QUERIES_TABLE =
            "CREATE TABLE " + Tables.TABLE_SAVED_QUERIES +
                    "(" +
                    SavedQueriesColums.QUERY + " VARCHAR(64) NOT NULL" +
                    ", " +
                    SavedQueriesColums.TIME_STAMP + " INTEGER" +
                    ")";

    private static const String INSERT_BUILD_VERSION =
            "INSERT INTO " + Tables.TABLE_META_INDEX +
                    " VALUES ('" + Build.VERSION.INCREMENTAL + "');";

    private static const String SELECT_BUILD_VERSION =
            "SELECT " + MetaColumns.BUILD + " FROM " + Tables.TABLE_META_INDEX + " LIMIT 1;";

    private static IndexDatabaseHelper sSingleton;

    public static synchronized IndexDatabaseHelper GetInstance(Context context) {
        if (sSingleton == NULL) {
            sSingleton = new IndexDatabaseHelper(context);
        }
        return sSingleton;
    }

    public IndexDatabaseHelper(Context context) {
        Super(context, DATABASE_NAME, NULL, DATABASE_VERSION);
    }

    //@Override
    CARAPI OnCreate(SQLiteDatabase db) {
        BootstrapDB(db);
    }

    private void BootstrapDB(SQLiteDatabase db) {
        db->ExecSQL(CREATE_INDEX_TABLE);
        db->ExecSQL(CREATE_META_TABLE);
        db->ExecSQL(CREATE_SAVED_QUERIES_TABLE);
        db->ExecSQL(INSERT_BUILD_VERSION);
        Logger::I(TAG, "Bootstrapped database");
    }

    //@Override
    CARAPI OnOpen(SQLiteDatabase db) {
        super->OnOpen(db);

        Logger::I(TAG, "Using schema version: " + db->GetVersion());

        if (!Build.VERSION.INCREMENTAL->Equals(GetBuildVersion(db))) {
            Logger::W(TAG, "Index needs to be rebuilt as build-version is not the same");
            // We need to drop the tables and recreate them
            Reconstruct(db);
        } else {
            Logger::I(TAG, "Index is fine");
        }
    }

    //@Override
    CARAPI OnUpgrade(SQLiteDatabase db, Int32 oldVersion, Int32 newVersion) {
        if (oldVersion < DATABASE_VERSION) {
            Logger::W(TAG, "Detected schema version '" +  oldVersion + "'. " +
                    "Index needs to be rebuilt for schema version '" + newVersion + "'.");
            // We need to drop the tables and recreate them
            Reconstruct(db);
        }
    }

    //@Override
    CARAPI OnDowngrade(SQLiteDatabase db, Int32 oldVersion, Int32 newVersion) {
        Logger::W(TAG, "Detected schema version '" +  oldVersion + "'. " +
                "Index needs to be rebuilt for schema version '" + newVersion + "'.");
        // We need to drop the tables and recreate them
        Reconstruct(db);
    }

    private void Reconstruct(SQLiteDatabase db) {
        DropTables(db);
        BootstrapDB(db);
    }

    private String GetBuildVersion(SQLiteDatabase db) {
        String version = NULL;
        Cursor cursor = NULL;
        try {
            cursor = db->RawQuery(SELECT_BUILD_VERSION, NULL);
            if (cursor->MoveToFirst()) {
                version = cursor->GetString(0);
            }
        }
        catch (Exception e) {
            Logger::E(TAG, "Cannot get build version from Index metadata");
        }
        finally {
            if (cursor != NULL) {
                cursor->Close();
            }
        }
        return version;
    }

    private void DropTables(SQLiteDatabase db) {
        db->ExecSQL("DROP TABLE IF EXISTS " + Tables.TABLE_META_INDEX);
        db->ExecSQL("DROP TABLE IF EXISTS " + Tables.TABLE_PREFS_INDEX);
        db->ExecSQL("DROP TABLE IF EXISTS " + Tables.TABLE_SAVED_QUERIES);
    }
}
