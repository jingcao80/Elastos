

package com.android.settings.search;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::IMergeCursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Provider::ISearchIndexableData;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexablesContract;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::IXml;
using Elastos::Droid::Settings::IR;
using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Lang::Reflect::IField;
using Elastos::Text::INormalizer;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Regex::IPattern;

using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_NON_INDEXABLE_KEYS_KEY_VALUE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_RANK;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_TITLE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_SUMMARY_ON;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_SUMMARY_OFF;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_ENTRIES;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_KEYWORDS;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_SCREEN_TITLE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_CLASS_NAME;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_ICON_RESID;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_INTENT_ACTION;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_INTENT_TARGET_PACKAGE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_INTENT_TARGET_CLASS;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_KEY;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_RAW_USER_ID;

using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_RANK;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_RESID;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_CLASS_NAME;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_ICON_RESID;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_ACTION;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE;
using static android::Provider::SearchIndexablesContract::ICOLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS;

using static com::Android::Settings::Search::IndexDatabaseHelper::ITables;
using static com::Android::Settings::Search::IndexDatabaseHelper::IIndexColumns;

public class Index {

    private static const String LOG_TAG = "Index";

    // Those indices should match the indices of SELECT_COLUMNS !
    public static const Int32 COLUMN_INDEX_RANK = 0;
    public static const Int32 COLUMN_INDEX_TITLE = 1;
    public static const Int32 COLUMN_INDEX_SUMMARY_ON = 2;
    public static const Int32 COLUMN_INDEX_SUMMARY_OFF = 3;
    public static const Int32 COLUMN_INDEX_ENTRIES = 4;
    public static const Int32 COLUMN_INDEX_KEYWORDS = 5;
    public static const Int32 COLUMN_INDEX_CLASS_NAME = 6;
    public static const Int32 COLUMN_INDEX_SCREEN_TITLE = 7;
    public static const Int32 COLUMN_INDEX_ICON = 8;
    public static const Int32 COLUMN_INDEX_INTENT_ACTION = 9;
    public static const Int32 COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE = 10;
    public static const Int32 COLUMN_INDEX_INTENT_ACTION_TARGET_CLASS = 11;
    public static const Int32 COLUMN_INDEX_ENABLED = 12;
    public static const Int32 COLUMN_INDEX_KEY = 13;
    public static const Int32 COLUMN_INDEX_USER_ID = 14;

    public static const String ENTRIES_SEPARATOR = "|";

    // If you change the order of columns here, you SHOULD change the COLUMN_INDEX_XXX values
    private static const String[] SELECT_COLUMNS = new String[] {
            IndexColumns.DATA_RANK,               // 0
            IndexColumns.DATA_TITLE,              // 1
            IndexColumns.DATA_SUMMARY_ON,         // 2
            IndexColumns.DATA_SUMMARY_OFF,        // 3
            IndexColumns.DATA_ENTRIES,            // 4
            IndexColumns.DATA_KEYWORDS,           // 5
            IndexColumns.CLASS_NAME,              // 6
            IndexColumns.SCREEN_TITLE,            // 7
            IndexColumns.ICON,                    // 8
            IndexColumns.INTENT_ACTION,           // 9
            IndexColumns.INTENT_TARGET_PACKAGE,   // 10
            IndexColumns.INTENT_TARGET_CLASS,     // 11
            IndexColumns.ENABLED,                 // 12
            IndexColumns.DATA_KEY_REF             // 13
    };

    private static const String[] MATCH_COLUMNS_PRIMARY = {
            IndexColumns.DATA_TITLE,
            IndexColumns.DATA_TITLE_NORMALIZED,
            IndexColumns.DATA_KEYWORDS
    };

    private static const String[] MATCH_COLUMNS_SECONDARY = {
            IndexColumns.DATA_SUMMARY_ON,
            IndexColumns.DATA_SUMMARY_ON_NORMALIZED,
            IndexColumns.DATA_SUMMARY_OFF,
            IndexColumns.DATA_SUMMARY_OFF_NORMALIZED,
            IndexColumns.DATA_ENTRIES
    };

    // Max number of saved search queries (who will be used for proposing suggestions)
    private static Int64 MAX_SAVED_SEARCH_QUERY = 64;
    // Max number of proposed suggestions
    private static const Int32 MAX_PROPOSED_SUGGESTIONS = 5;

    private static const String BASE_AUTHORITY = "com.android.settings";

    private static const String EMPTY = "";
    private static const String NON_BREAKING_HYPHEN = "\u2011";
    private static const String HYPHEN = "-";

    private static const String FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER =
            "SEARCH_INDEX_DATA_PROVIDER";

    private static const String NODE_NAME_PREFERENCE_SCREEN = "PreferenceScreen";
    private static const String NODE_NAME_CHECK_BOX_PREFERENCE = "CheckBoxPreference";
    private static const String NODE_NAME_LIST_PREFERENCE = "ListPreference";

    private static const List<String> EMPTY_LIST = Collections.<String>EmptyList();

    private static Index sInstance;

    private static const Pattern REMOVE_DIACRITICALS_PATTERN
            = Pattern->Compile("\\p{InCombiningDiacriticalMarks}+");

    /**
     * A private class to describe the update data for the Index database
     */
    private static class UpdateData {
        public List<SearchIndexableData> dataToUpdate;
        public List<SearchIndexableData> dataToDelete;
        public Map<String, List<String>> nonIndexableKeys;

        public Boolean forceUpdate = FALSE;

        public UpdateData() {
            dataToUpdate = new ArrayList<SearchIndexableData>();
            dataToDelete = new ArrayList<SearchIndexableData>();
            nonIndexableKeys = new HashMap<String, List<String>>();
        }

        public UpdateData(UpdateData other) {
            dataToUpdate = new ArrayList<SearchIndexableData>(other.dataToUpdate);
            dataToDelete = new ArrayList<SearchIndexableData>(other.dataToDelete);
            nonIndexableKeys = new HashMap<String, List<String>>(other.nonIndexableKeys);
            forceUpdate = other.forceUpdate;
        }

        public UpdateData Copy() {
            return new UpdateData(this);
        }

        CARAPI Clear() {
            dataToUpdate->Clear();
            dataToDelete->Clear();
            nonIndexableKeys->Clear();
            forceUpdate = FALSE;
        }
    }

    private final AtomicBoolean mIsAvailable = new AtomicBoolean(FALSE);
    private final UpdateData mDataToProcess = new UpdateData();
    private Context mContext;
    private final String mBaseAuthority;

    /**
     * A basic singleton
     */
    public static Index GetInstance(Context context) {
        if (sInstance == NULL) {
            sInstance = new Index(context, BASE_AUTHORITY);
        } else {
            sInstance->SetContext(context);
        }
        return sInstance;
    }

    public Index(Context context, String baseAuthority) {
        mContext = context;
        mBaseAuthority = baseAuthority;
    }

    CARAPI SetContext(Context context) {
        mContext = context;
    }

    public Boolean IsAvailable() {
        return mIsAvailable->Get();
    }

    public Cursor Search(String query) {
        final SQLiteDatabase database = GetReadableDatabase();
        final Cursor[] cursors = new Cursor[2];

        final String primarySql = BuildSearchSQL(query, MATCH_COLUMNS_PRIMARY, TRUE);
        Logger::D(LOG_TAG, "Search primary query: " + primarySql);
        cursors[0] = database->RawQuery(primarySql, NULL);

        // We need to use an EXCEPT operator as negate MATCH queries do not work.
        StringBuilder sql = new StringBuilder(
                BuildSearchSQL(query, MATCH_COLUMNS_SECONDARY, FALSE));
        sql->Append(" EXCEPT ");
        sql->Append(primarySql);

        final String secondarySql = sql->ToString();
        Logger::D(LOG_TAG, "Search secondary query: " + secondarySql);
        cursors[1] = database->RawQuery(secondarySql, NULL);

        return new MergeCursor(cursors);
    }

    public Cursor GetSuggestions(String query) {
        final String sql = BuildSuggestionsSQL(query);
        Logger::D(LOG_TAG, "Suggestions query: " + sql);
        return GetReadableDatabase()->RawQuery(sql, NULL);
    }

    private String BuildSuggestionsSQL(String query) {
        StringBuilder sb = new StringBuilder();

        sb->Append("SELECT ");
        sb->Append(IndexDatabaseHelper.SavedQueriesColums.QUERY);
        sb->Append(" FROM ");
        sb->Append(Tables.TABLE_SAVED_QUERIES);

        if (TextUtils->IsEmpty(query)) {
            sb->Append(" ORDER BY rowId DESC");
        } else {
            sb->Append(" WHERE ");
            sb->Append(IndexDatabaseHelper.SavedQueriesColums.QUERY);
            sb->Append(" LIKE ");
            sb->Append("'");
            sb->Append(query);
            sb->Append("%");
            sb->Append("'");
        }

        sb->Append(" LIMIT ");
        sb->Append(MAX_PROPOSED_SUGGESTIONS);

        return sb->ToString();
    }

    public Int64 AddSavedQuery(String query){
        final SaveSearchQueryTask task = new SaveSearchQueryTask();
        task->Execute(query);
        try {
            return task->Get();
        } catch (InterruptedException e) {
            Logger::E(LOG_TAG, "Cannot insert saved query: " + query, e);
            return -1 ;
        } catch (ExecutionException e) {
            Logger::E(LOG_TAG, "Cannot insert saved query: " + query, e);
            return -1;
        }
    }

    CARAPI Update() {
        final Intent intent = new Intent(SearchIndexablesContract.PROVIDER_INTERFACE);
        List<ResolveInfo> list =
                mContext->GetPackageManager()->QueryIntentContentProviders(intent, 0);

        final Int32 size = list->Size();
        for (Int32 n = 0; n < size; n++) {
            final ResolveInfo info = list->Get(n);
            if (!IsWellKnownProvider(info)) {
                continue;
            }
            final String authority = info.providerInfo.authority;
            final String packageName = info.providerInfo.packageName;

            AddIndexablesFromRemoteProvider(packageName, authority);
            AddNonIndexablesKeysFromRemoteProvider(packageName, authority);
        }

        UpdateInternal();
    }

    private Boolean AddIndexablesFromRemoteProvider(String packageName, String authority) {
        try {
            final Int32 baseRank = Ranking->GetBaseRankForAuthority(authority);

            final Context context = mBaseAuthority->Equals(authority) ?
                    mContext : mContext->CreatePackageContext(packageName, 0);

            final Uri uriForResources = BuildUriForXmlResources(authority);
            AddIndexablesForXmlResourceUri(context, packageName, uriForResources,
                    SearchIndexablesContract.INDEXABLES_XML_RES_COLUMNS, baseRank);

            final Uri uriForRawData = BuildUriForRawData(authority);
            AddIndexablesForRawDataUri(context, packageName, uriForRawData,
                    SearchIndexablesContract.INDEXABLES_RAW_COLUMNS, baseRank);
            return TRUE;
        } catch (PackageManager.NameNotFoundException e) {
            Logger::W(LOG_TAG, "Could not create context for " + packageName + ": "
                    + Log->GetStackTraceString(e));
            return FALSE;
        }
    }

    private void AddNonIndexablesKeysFromRemoteProvider(String packageName,
                                                        String authority) {
        final List<String> keys =
                GetNonIndexablesKeysFromRemoteProvider(packageName, authority);
        AddNonIndexableKeys(packageName, keys);
    }

    private List<String> GetNonIndexablesKeysFromRemoteProvider(String packageName,
                                                                String authority) {
        try {
            final Context packageContext = mContext->CreatePackageContext(packageName, 0);

            final Uri uriForNonIndexableKeys = BuildUriForNonIndexableKeys(authority);
            return GetNonIndexablesKeys(packageContext, uriForNonIndexableKeys,
                    SearchIndexablesContract.NON_INDEXABLES_KEYS_COLUMNS);
        } catch (PackageManager.NameNotFoundException e) {
            Logger::W(LOG_TAG, "Could not create context for " + packageName + ": "
                    + Log->GetStackTraceString(e));
            return EMPTY_LIST;
        }
    }

    private List<String> GetNonIndexablesKeys(Context packageContext, Uri uri,
                                              String[] projection) {

        final ContentResolver resolver = packageContext->GetContentResolver();
        final Cursor cursor = resolver->Query(uri, projection, NULL, NULL, NULL);

        if (cursor == NULL) {
            Logger::W(LOG_TAG, "Cannot add index data for Uri: " + uri->ToString());
            return EMPTY_LIST;
        }

        List<String> result = new ArrayList<String>();
        try {
            final Int32 count = cursor->GetCount();
            if (count > 0) {
                while (cursor->MoveToNext()) {
                    final String key = cursor->GetString(COLUMN_INDEX_NON_INDEXABLE_KEYS_KEY_VALUE);
                    result->Add(key);
                }
            }
            return result;
        } finally {
            cursor->Close();
        }
    }

    CARAPI AddIndexableData(SearchIndexableData data) {
        synchronized(mDataToProcess) {
            mDataToProcess.dataToUpdate->Add(data);
        }
    }

    CARAPI AddIndexableData(SearchIndexableResource[] array) {
        synchronized(mDataToProcess) {
            final Int32 count = array.length;
            for (Int32 n = 0; n < count; n++) {
                mDataToProcess.dataToUpdate->Add(array[n]);
            }
        }
    }

    CARAPI DeleteIndexableData(SearchIndexableData data) {
        synchronized(mDataToProcess) {
            mDataToProcess.dataToDelete->Add(data);
        }
    }

    CARAPI AddNonIndexableKeys(String authority, List<String> keys) {
        synchronized(mDataToProcess) {
            mDataToProcess.nonIndexableKeys->Put(authority, keys);
        }
    }

    /**
     * Only allow a "well known" SearchIndexablesProvider. The provider should:
     *
     * - have read/write {@link Manifest::permission::READ_SEARCH_INDEXABLES}
     * - be from a privileged package
     */
    private Boolean IsWellKnownProvider(ResolveInfo info) {
        final String authority = info.providerInfo.authority;
        final String packageName = info.providerInfo.applicationInfo.packageName;

        if (TextUtils->IsEmpty(authority) || TextUtils->IsEmpty(packageName)) {
            return FALSE;
        }

        final String readPermission = info.providerInfo.readPermission;
        final String writePermission = info.providerInfo.writePermission;

        if (TextUtils->IsEmpty(readPermission) || TextUtils->IsEmpty(writePermission)) {
            return FALSE;
        }

        if (!Manifest::permission::READ_SEARCH_INDEXABLES->Equals(readPermission) ||
            !Manifest::permission::READ_SEARCH_INDEXABLES->Equals(writePermission)) {
            return FALSE;
        }

        return IsPrivilegedPackage(packageName);
    }

    private Boolean IsPrivilegedPackage(String packageName) {
        final PackageManager pm = mContext->GetPackageManager();
        try {
            PackageInfo packInfo = pm->GetPackageInfo(packageName, 0);
            return ((packInfo.applicationInfo.flags & ApplicationInfo.FLAG_PRIVILEGED) != 0);
        } catch (PackageManager.NameNotFoundException e) {
            return FALSE;
        }
    }

    private void UpdateFromRemoteProvider(String packageName, String authority) {
        if (AddIndexablesFromRemoteProvider(packageName, authority)) {
            UpdateInternal();
        }
    }

    /**
     * Update the Index for a specific class name resources
     *
     * @param className the class name (typically a fragment name).
     * @param rebuild TRUE means that you want to delete the data from the Index first.
     * @param includeInSearchResults TRUE means that you want the bit "enabled" set so that the
     *                               data will be seen included into the search results
     */
    CARAPI UpdateFromClassNameResource(String className, Boolean rebuild,
            Boolean includeInSearchResults) {
        if (className == NULL) {
            throw new IllegalArgumentException("class name cannot be NULL!");
        }
        final SearchIndexableResource res = SearchIndexableResources->GetResourceByName(className);
        if (res == NULL ) {
            Logger::E(LOG_TAG, "Cannot find SearchIndexableResources for class name: " + className);
            return;
        }
        res.context = mContext;
        res.enabled = includeInSearchResults;
        if (rebuild) {
            DeleteIndexableData(res);
        }
        AddIndexableData(res);
        mDataToProcess.forceUpdate = TRUE;
        UpdateInternal();
        res.enabled = FALSE;
    }

    CARAPI UpdateFromSearchIndexableData(SearchIndexableData data) {
        AddIndexableData(data);
        mDataToProcess.forceUpdate = TRUE;
        UpdateInternal();
    }

    private SQLiteDatabase GetReadableDatabase() {
        return IndexDatabaseHelper->GetInstance(mContext).GetReadableDatabase();
    }

    private SQLiteDatabase GetWritableDatabase() {
        return IndexDatabaseHelper->GetInstance(mContext).GetWritableDatabase();
    }

    private static Uri BuildUriForXmlResources(String authority) {
        return Uri->Parse("content://" + authority + "/" +
                SearchIndexablesContract.INDEXABLES_XML_RES_PATH);
    }

    private static Uri BuildUriForRawData(String authority) {
        return Uri->Parse("content://" + authority + "/" +
                SearchIndexablesContract.INDEXABLES_RAW_PATH);
    }

    private static Uri BuildUriForNonIndexableKeys(String authority) {
        return Uri->Parse("content://" + authority + "/" +
                SearchIndexablesContract.NON_INDEXABLES_KEYS_PATH);
    }

    private void UpdateInternal() {
        synchronized(mDataToProcess) {
            final UpdateIndexTask task = new UpdateIndexTask();
            UpdateData copy = mDataToProcess->Copy();
            task->Execute(copy);
            mDataToProcess->Clear();
        }
    }

    private void AddIndexablesForXmlResourceUri(Context packageContext, String packageName,
            Uri uri, String[] projection, Int32 baseRank) {

        final ContentResolver resolver = packageContext->GetContentResolver();
        final Cursor cursor = resolver->Query(uri, projection, NULL, NULL, NULL);

        if (cursor == NULL) {
            Logger::W(LOG_TAG, "Cannot add index data for Uri: " + uri->ToString());
            return;
        }

        try {
            final Int32 count = cursor->GetCount();
            if (count > 0) {
                while (cursor->MoveToNext()) {
                    final Int32 providerRank = cursor->GetInt(COLUMN_INDEX_XML_RES_RANK);
                    final Int32 rank = (providerRank > 0) ? baseRank + providerRank : baseRank;

                    final Int32 xmlResId = cursor->GetInt(COLUMN_INDEX_XML_RES_RESID);

                    final String className = cursor->GetString(COLUMN_INDEX_XML_RES_CLASS_NAME);
                    final Int32 iconResId = cursor->GetInt(COLUMN_INDEX_XML_RES_ICON_RESID);

                    final String action = cursor->GetString(COLUMN_INDEX_XML_RES_INTENT_ACTION);
                    final String targetPackage = cursor->GetString(
                            COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE);
                    final String targetClass = cursor->GetString(
                            COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS);

                    SearchIndexableResource sir = new SearchIndexableResource(packageContext);
                    sir.rank = rank;
                    sir.xmlResId = xmlResId;
                    sir.className = className;
                    sir.packageName = packageName;
                    sir.iconResId = iconResId;
                    sir.intentAction = action;
                    sir.intentTargetPackage = targetPackage;
                    sir.intentTargetClass = targetClass;

                    AddIndexableData(sir);
                }
            }
        } finally {
            cursor->Close();
        }
    }

    private void AddIndexablesForRawDataUri(Context packageContext, String packageName,
            Uri uri, String[] projection, Int32 baseRank) {

        final ContentResolver resolver = packageContext->GetContentResolver();
        final Cursor cursor = resolver->Query(uri, projection, NULL, NULL, NULL);

        if (cursor == NULL) {
            Logger::W(LOG_TAG, "Cannot add index data for Uri: " + uri->ToString());
            return;
        }

        try {
            final Int32 count = cursor->GetCount();
            if (count > 0) {
                while (cursor->MoveToNext()) {
                    final Int32 providerRank = cursor->GetInt(COLUMN_INDEX_RAW_RANK);
                    final Int32 rank = (providerRank > 0) ? baseRank + providerRank : baseRank;

                    final String title = cursor->GetString(COLUMN_INDEX_RAW_TITLE);
                    final String summaryOn = cursor->GetString(COLUMN_INDEX_RAW_SUMMARY_ON);
                    final String summaryOff = cursor->GetString(COLUMN_INDEX_RAW_SUMMARY_OFF);
                    final String entries = cursor->GetString(COLUMN_INDEX_RAW_ENTRIES);
                    final String keywords = cursor->GetString(COLUMN_INDEX_RAW_KEYWORDS);

                    final String screenTitle = cursor->GetString(COLUMN_INDEX_RAW_SCREEN_TITLE);

                    final String className = cursor->GetString(COLUMN_INDEX_RAW_CLASS_NAME);
                    final Int32 iconResId = cursor->GetInt(COLUMN_INDEX_RAW_ICON_RESID);

                    final String action = cursor->GetString(COLUMN_INDEX_RAW_INTENT_ACTION);
                    final String targetPackage = cursor->GetString(
                            COLUMN_INDEX_RAW_INTENT_TARGET_PACKAGE);
                    final String targetClass = cursor->GetString(
                            COLUMN_INDEX_RAW_INTENT_TARGET_CLASS);

                    final String key = cursor->GetString(COLUMN_INDEX_RAW_KEY);
                    final Int32 userId = cursor->GetInt(COLUMN_INDEX_RAW_USER_ID);

                    SearchIndexableRaw data = new SearchIndexableRaw(packageContext);
                    data.rank = rank;
                    data.title = title;
                    data.summaryOn = summaryOn;
                    data.summaryOff = summaryOff;
                    data.entries = entries;
                    data.keywords = keywords;
                    data.screenTitle = screenTitle;
                    data.className = className;
                    data.packageName = packageName;
                    data.iconResId = iconResId;
                    data.intentAction = action;
                    data.intentTargetPackage = targetPackage;
                    data.intentTargetClass = targetClass;
                    data.key = key;
                    data.userId = userId;

                    AddIndexableData(data);
                }
            }
        } finally {
            cursor->Close();
        }
    }

    private String BuildSearchSQL(String query, String[] colums, Boolean withOrderBy) {
        StringBuilder sb = new StringBuilder();
        sb->Append(BuildSearchSQLForColumn(query, colums));
        if (withOrderBy) {
            sb->Append(" ORDER BY ");
            sb->Append(IndexColumns.DATA_RANK);
        }
        return sb->ToString();
    }

    private String BuildSearchSQLForColumn(String query, String[] columnNames) {
        StringBuilder sb = new StringBuilder();
        sb->Append("SELECT ");
        for (Int32 n = 0; n < SELECT_COLUMNS.length; n++) {
            sb->Append(SELECT_COLUMNS[n]);
            if (n < SELECT_COLUMNS.length - 1) {
                sb->Append(", ");
            }
        }
        sb->Append(" FROM ");
        sb->Append(Tables.TABLE_PREFS_INDEX);
        sb->Append(" WHERE ");
        sb->Append(BuildSearchWhereStringForColumns(query, columnNames));

        return sb->ToString();
    }

    private String BuildSearchWhereStringForColumns(String query, String[] columnNames) {
        final StringBuilder sb = new StringBuilder(Tables.TABLE_PREFS_INDEX);
        sb->Append(" MATCH ");
        DatabaseUtils->AppendEscapedSQLString(sb,
                BuildSearchMatchStringForColumns(query, columnNames));
        sb->Append(" AND ");
        sb->Append(IndexColumns.LOCALE);
        sb->Append(" = ");
        DatabaseUtils->AppendEscapedSQLString(sb, Locale->GetDefault()->ToString());
        sb->Append(" AND ");
        sb->Append(IndexColumns.ENABLED);
        sb->Append(" = 1");
        return sb->ToString();
    }

    private String BuildSearchMatchStringForColumns(String query, String[] columnNames) {
        final String value = query + "*";
        StringBuilder sb = new StringBuilder();
        final Int32 count = columnNames.length;
        for (Int32 n = 0; n < count; n++) {
            sb->Append(columnNames[n]);
            sb->Append(":");
            sb->Append(value);
            if (n < count - 1) {
                sb->Append(" OR ");
            }
        }
        return sb->ToString();
    }

    private void IndexOneSearchIndexableData(SQLiteDatabase database, String localeStr,
            SearchIndexableData data, Map<String, List<String>> nonIndexableKeys) {
        if (data instanceof SearchIndexableResource) {
            IndexOneResource(database, localeStr, (SearchIndexableResource) data, nonIndexableKeys);
        } else if (data instanceof SearchIndexableRaw) {
            IndexOneRaw(database, localeStr, (SearchIndexableRaw) data);
        }
    }

    private void IndexOneRaw(SQLiteDatabase database, String localeStr,
                             SearchIndexableRaw raw) {
        // Should be the same locale as the one we are processing
        if (!raw.locale->ToString()->EqualsIgnoreCase(localeStr)) {
            return;
        }

        UpdateOneRowWithFilteredData(database, localeStr,
                raw.title,
                raw.summaryOn,
                raw.summaryOff,
                raw.entries,
                raw.className,
                raw.screenTitle,
                raw.iconResId,
                raw.rank,
                raw.keywords,
                raw.intentAction,
                raw.intentTargetPackage,
                raw.intentTargetClass,
                raw.enabled,
                raw.key,
                raw.userId);
    }

    private static Boolean IsIndexableClass(final Class<?> clazz) {
        return (clazz != NULL) && Indexable.class->IsAssignableFrom(clazz);
    }

    private static Class<?> GetIndexableClass(String className) {
        final Class<?> clazz;
        try {
            clazz = Class->ForName(className);
        } catch (ClassNotFoundException e) {
            Logger::D(LOG_TAG, "Cannot find class: " + className);
            return NULL;
        }
        return IsIndexableClass(clazz) ? clazz : NULL;
    }

    private void IndexOneResource(SQLiteDatabase database, String localeStr,
            SearchIndexableResource sir, Map<String, List<String>> nonIndexableKeysFromResource) {

        if (sir == NULL) {
            Logger::E(LOG_TAG, "Cannot index a NULL resource!");
            return;
        }

        final List<String> nonIndexableKeys = new ArrayList<String>();

        if (sir.xmlResId > SearchIndexableResources.NO_DATA_RES_ID) {
            List<String> resNonIndxableKeys = nonIndexableKeysFromResource->Get(sir.packageName);
            if (resNonIndxableKeys != NULL && resNonIndxableKeys->Size() > 0) {
                nonIndexableKeys->AddAll(resNonIndxableKeys);
            }

            IndexFromResource(sir.context, database, localeStr,
                    sir.xmlResId, sir.className, sir.iconResId, sir.rank,
                    sir.intentAction, sir.intentTargetPackage, sir.intentTargetClass,
                    nonIndexableKeys);
        } else {
            if (TextUtils->IsEmpty(sir.className)) {
                Logger::W(LOG_TAG, "Cannot index an empty Search Provider name!");
                return;
            }

            final Class<?> clazz = GetIndexableClass(sir.className);
            if (clazz == NULL) {
                Logger::D(LOG_TAG, "SearchIndexableResource '" + sir.className +
                        "' should implement the " + Indexable.class->GetName() + " interface!");
                return;
            }

            // Will be non NULL only for a Local provider implementing a
            // SEARCH_INDEX_DATA_PROVIDER field
            final Indexable.SearchIndexProvider provider = GetSearchIndexProvider(clazz);
            if (provider != NULL) {
                List<String> providerNonIndexableKeys = provider->GetNonIndexableKeys(sir.context);
                if (providerNonIndexableKeys != NULL && providerNonIndexableKeys->Size() > 0) {
                    nonIndexableKeys->AddAll(providerNonIndexableKeys);
                }

                IndexFromProvider(mContext, database, localeStr, provider, sir.className,
                        sir.iconResId, sir.rank, sir.enabled, nonIndexableKeys);
            }
        }
    }

    private Indexable.SearchIndexProvider GetSearchIndexProvider(final Class<?> clazz) {
        try {
            final Field f = clazz->GetField(FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER);
            return (Indexable.SearchIndexProvider) f->Get(NULL);
        } catch (NoSuchFieldException e) {
            Logger::D(LOG_TAG, "Cannot find field '" + FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER + "'");
        } catch (SecurityException se) {
            Logger::D(LOG_TAG,
                    "Security exception for field '" + FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER + "'");
        } catch (IllegalAccessException e) {
            Logger::D(LOG_TAG,
                    "Illegal access to field '" + FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER + "'");
        } catch (IllegalArgumentException e) {
            Logger::D(LOG_TAG,
                    "Illegal argument when accessing field '" +
                            FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER + "'");
        }
        return NULL;
    }

    private void IndexFromResource(Context context, SQLiteDatabase database, String localeStr,
           Int32 xmlResId, String fragmentName, Int32 iconResId, Int32 rank,
           String intentAction, String intentTargetPackage, String intentTargetClass,
           List<String> nonIndexableKeys) {

        XmlResourceParser parser = NULL;
        try {
            parser = context->GetResources()->GetXml(xmlResId);

            Int32 type;
            while ((type = parser->Next()) != XmlPullParser.END_DOCUMENT
                    && type != XmlPullParser.START_TAG) {
                // Parse next until start tag is found
            }

            String nodeName = parser->GetName();
            if (!NODE_NAME_PREFERENCE_SCREEN->Equals(nodeName)) {
                throw new RuntimeException(
                        "XML document must start with <PreferenceScreen> tag; found"
                                + nodeName + " at " + parser->GetPositionDescription());
            }

            final Int32 outerDepth = parser->GetDepth();
            final AttributeSet attrs = Xml->AsAttributeSet(parser);

            final String screenTitle = GetDataTitle(context, attrs);

            String key = GetDataKey(context, attrs);

            String title;
            String summary;
            String keywords;

            // Insert rows for the main PreferenceScreen node. Rewrite the data for removing
            // hyphens.
            if (!nonIndexableKeys->Contains(key)) {
                title = GetDataTitle(context, attrs);
                summary = GetDataSummary(context, attrs);
                keywords = GetDataKeywords(context, attrs);

                UpdateOneRowWithFilteredData(database, localeStr, title, summary, NULL, NULL,
                        fragmentName, screenTitle, iconResId, rank,
                        keywords, intentAction, intentTargetPackage, intentTargetClass, TRUE,
                        key, -1 /* default user id */);
            }

            while ((type = parser->Next()) != XmlPullParser.END_DOCUMENT
                    && (type != XmlPullParser.END_TAG || parser->GetDepth() > outerDepth)) {
                if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                    continue;
                }

                nodeName = parser->GetName();

                key = GetDataKey(context, attrs);
                if (nonIndexableKeys->Contains(key)) {
                    continue;
                }

                title = GetDataTitle(context, attrs);
                keywords = GetDataKeywords(context, attrs);

                if (!nodeName->Equals(NODE_NAME_CHECK_BOX_PREFERENCE)) {
                    summary = GetDataSummary(context, attrs);

                    String entries = NULL;

                    if (nodeName->EndsWith(NODE_NAME_LIST_PREFERENCE)) {
                        entries = GetDataEntries(context, attrs);
                    }

                    // Insert rows for the child nodes of PreferenceScreen
                    UpdateOneRowWithFilteredData(database, localeStr, title, summary, NULL, entries,
                            fragmentName, screenTitle, iconResId, rank,
                            keywords, intentAction, intentTargetPackage, intentTargetClass,
                            TRUE, key, -1 /* default user id */);
                } else {
                    String summaryOn = GetDataSummaryOn(context, attrs);
                    String summaryOff = GetDataSummaryOff(context, attrs);

                    if (TextUtils->IsEmpty(summaryOn) && TextUtils->IsEmpty(summaryOff)) {
                        summaryOn = GetDataSummary(context, attrs);
                    }

                    UpdateOneRowWithFilteredData(database, localeStr, title, summaryOn, summaryOff,
                            NULL, fragmentName, screenTitle, iconResId, rank,
                            keywords, intentAction, intentTargetPackage, intentTargetClass,
                            TRUE, key, -1 /* default user id */);
                }
            }

        } catch (XmlPullParserException e) {
            throw new RuntimeException("Error parsing PreferenceScreen", e);
        } catch (IOException e) {
            throw new RuntimeException("Error parsing PreferenceScreen", e);
        } finally {
            if (parser != NULL) parser->Close();
        }
    }

    private void IndexFromProvider(Context context, SQLiteDatabase database, String localeStr,
            Indexable.SearchIndexProvider provider, String className, Int32 iconResId, Int32 rank,
            Boolean enabled, List<String> nonIndexableKeys) {

        if (provider == NULL) {
            Logger::W(LOG_TAG, "Cannot find provider: " + className);
            return;
        }

        final List<SearchIndexableRaw> rawList = provider->GetRawDataToIndex(context, enabled);

        if (rawList != NULL) {
            final Int32 rawSize = rawList->Size();
            for (Int32 i = 0; i < rawSize; i++) {
                SearchIndexableRaw raw = rawList->Get(i);

                // Should be the same locale as the one we are processing
                if (!raw.locale->ToString()->EqualsIgnoreCase(localeStr)) {
                    continue;
                }

                if (nonIndexableKeys->Contains(raw.key)) {
                    continue;
                }

                UpdateOneRowWithFilteredData(database, localeStr,
                        raw.title,
                        raw.summaryOn,
                        raw.summaryOff,
                        raw.entries,
                        className,
                        raw.screenTitle,
                        iconResId,
                        rank,
                        raw.keywords,
                        raw.intentAction,
                        raw.intentTargetPackage,
                        raw.intentTargetClass,
                        raw.enabled,
                        raw.key,
                        raw.userId);
            }
        }

        final List<SearchIndexableResource> resList =
                provider->GetXmlResourcesToIndex(context, enabled);
        if (resList != NULL) {
            final Int32 resSize = resList->Size();
            for (Int32 i = 0; i < resSize; i++) {
                SearchIndexableResource item = resList->Get(i);

                // Should be the same locale as the one we are processing
                if (!item.locale->ToString()->EqualsIgnoreCase(localeStr)) {
                    continue;
                }

                final Int32 itemIconResId = (item.iconResId == 0) ? iconResId : item.iconResId;
                final Int32 itemRank = (item.rank == 0) ? rank : item.rank;
                String itemClassName = (TextUtils->IsEmpty(item.className))
                        ? className : item.className;

                IndexFromResource(context, database, localeStr,
                        item.xmlResId, itemClassName, itemIconResId, itemRank,
                        item.intentAction, item.intentTargetPackage,
                        item.intentTargetClass, nonIndexableKeys);
            }
        }
    }

    private void UpdateOneRowWithFilteredData(SQLiteDatabase database, String locale,
            String title, String summaryOn, String summaryOff, String entries,
            String className,
            String screenTitle, Int32 iconResId, Int32 rank, String keywords,
            String intentAction, String intentTargetPackage, String intentTargetClass,
            Boolean enabled, String key, Int32 userId) {

        final String updatedTitle = NormalizeHyphen(title);
        final String updatedSummaryOn = NormalizeHyphen(summaryOn);
        final String updatedSummaryOff = NormalizeHyphen(summaryOff);

        final String normalizedTitle = NormalizeString(updatedTitle);
        final String normalizedSummaryOn = NormalizeString(updatedSummaryOn);
        final String normalizedSummaryOff = NormalizeString(updatedSummaryOff);

        UpdateOneRow(database, locale,
                updatedTitle, normalizedTitle, updatedSummaryOn, normalizedSummaryOn,
                updatedSummaryOff, normalizedSummaryOff, entries,
                className, screenTitle, iconResId,
                rank, keywords, intentAction, intentTargetPackage, intentTargetClass, enabled,
                key, userId);
    }

    private static String NormalizeHyphen(String input) {
        return (input != NULL) ? input->ReplaceAll(NON_BREAKING_HYPHEN, HYPHEN) : EMPTY;
    }

    private static String NormalizeString(String input) {
        final String nohyphen = (input != NULL) ? input->ReplaceAll(HYPHEN, EMPTY) : EMPTY;
        final String normalized = Normalizer->Normalize(nohyphen, Normalizer.Form.NFD);

        return REMOVE_DIACRITICALS_PATTERN->Matcher(normalized).ReplaceAll("").ToLowerCase();
    }

    private void UpdateOneRow(SQLiteDatabase database, String locale,
            String updatedTitle, String normalizedTitle,
            String updatedSummaryOn, String normalizedSummaryOn,
            String updatedSummaryOff, String normalizedSummaryOff, String entries,
            String className, String screenTitle, Int32 iconResId, Int32 rank, String keywords,
            String intentAction, String intentTargetPackage, String intentTargetClass,
            Boolean enabled, String key, Int32 userId) {

        if (TextUtils->IsEmpty(updatedTitle)) {
            return;
        }

        // The DocID should contains more than the title string itself (you may have two settings
        // with the same title). So we need to use a combination of the title and the screenTitle.
        StringBuilder sb = new StringBuilder(updatedTitle);
        sb->Append(screenTitle);
        Int32 docId = sb->ToString()->HashCode();

        ContentValues values = new ContentValues();
        values->Put(IndexColumns.DOCID, docId);
        values->Put(IndexColumns.LOCALE, locale);
        values->Put(IndexColumns.DATA_RANK, rank);
        values->Put(IndexColumns.DATA_TITLE, updatedTitle);
        values->Put(IndexColumns.DATA_TITLE_NORMALIZED, normalizedTitle);
        values->Put(IndexColumns.DATA_SUMMARY_ON, updatedSummaryOn);
        values->Put(IndexColumns.DATA_SUMMARY_ON_NORMALIZED, normalizedSummaryOn);
        values->Put(IndexColumns.DATA_SUMMARY_OFF, updatedSummaryOff);
        values->Put(IndexColumns.DATA_SUMMARY_OFF_NORMALIZED, normalizedSummaryOff);
        values->Put(IndexColumns.DATA_ENTRIES, entries);
        values->Put(IndexColumns.DATA_KEYWORDS, keywords);
        values->Put(IndexColumns.CLASS_NAME, className);
        values->Put(IndexColumns.SCREEN_TITLE, screenTitle);
        values->Put(IndexColumns.INTENT_ACTION, intentAction);
        values->Put(IndexColumns.INTENT_TARGET_PACKAGE, intentTargetPackage);
        values->Put(IndexColumns.INTENT_TARGET_CLASS, intentTargetClass);
        values->Put(IndexColumns.ICON, iconResId);
        values->Put(IndexColumns.ENABLED, enabled);
        values->Put(IndexColumns.DATA_KEY_REF, key);
        values->Put(IndexColumns.USER_ID, userId);

        database->ReplaceOrThrow(Tables.TABLE_PREFS_INDEX, NULL, values);
    }

    private String GetDataKey(Context context, AttributeSet attrs) {
        return GetData(context, attrs,
                R.styleable.Preference,
                R.styleable.Preference_key);
    }

    private String GetDataTitle(Context context, AttributeSet attrs) {
        return GetData(context, attrs,
                R.styleable.Preference,
                R.styleable.Preference_title);
    }

    private String GetDataSummary(Context context, AttributeSet attrs) {
        return GetData(context, attrs,
                R.styleable.Preference,
                R.styleable.Preference_summary);
    }

    private String GetDataSummaryOn(Context context, AttributeSet attrs) {
        return GetData(context, attrs,
                R.styleable.CheckBoxPreference,
                R.styleable.CheckBoxPreference_summaryOn);
    }

    private String GetDataSummaryOff(Context context, AttributeSet attrs) {
        return GetData(context, attrs,
                R.styleable.CheckBoxPreference,
                R.styleable.CheckBoxPreference_summaryOff);
    }

    private String GetDataEntries(Context context, AttributeSet attrs) {
        return GetDataEntries(context, attrs,
                R.styleable.ListPreference,
                R.styleable.ListPreference_entries);
    }

    private String GetDataKeywords(Context context, AttributeSet attrs) {
        return GetData(context, attrs, R.styleable.Preference, R.styleable.Preference_keywords);
    }

    private String GetData(Context context, AttributeSet set, Int32[] attrs, Int32 resId) {
        final TypedArray sa = context->ObtainStyledAttributes(set, attrs);
        final TypedValue tv = sa->PeekValue(resId);

        CharSequence data = NULL;
        if (tv != NULL && tv.type == TypedValue.TYPE_STRING) {
            if (tv.resourceId != 0) {
                data = context->GetText(tv.resourceId);
            } else {
                data = tv.string;
            }
        }
        return (data != NULL) ? data->ToString() : NULL;
    }

    private String GetDataEntries(Context context, AttributeSet set, Int32[] attrs, Int32 resId) {
        final TypedArray sa = context->ObtainStyledAttributes(set, attrs);
        final TypedValue tv = sa->PeekValue(resId);

        String[] data = NULL;
        if (tv != NULL && tv.type == TypedValue.TYPE_REFERENCE) {
            if (tv.resourceId != 0) {
                data = context->GetResources()->GetStringArray(tv.resourceId);
            }
        }
        final Int32 count = (data == NULL ) ? 0 : data.length;
        if (count == 0) {
            return NULL;
        }
        final StringBuilder result = new StringBuilder();
        for (Int32 n = 0; n < count; n++) {
            result->Append(data[n]);
            result->Append(ENTRIES_SEPARATOR);
        }
        return result->ToString();
    }

    private Int32 GetResId(Context context, AttributeSet set, Int32[] attrs, Int32 resId) {
        final TypedArray sa = context->ObtainStyledAttributes(set, attrs);
        final TypedValue tv = sa->PeekValue(resId);

        if (tv != NULL && tv.type == TypedValue.TYPE_STRING) {
            return tv.resourceId;
        } else {
            return 0;
        }
   }

    /**
     * A private class for updating the Index database
     */
    private class UpdateIndexTask extends AsyncTask<UpdateData, Integer, Void> {

        //@Override
        protected void OnPreExecute() {
            super->OnPreExecute();
            mIsAvailable->Set(FALSE);
        }

        //@Override
        protected void OnPostExecute(Void aVoid) {
            super->OnPostExecute(aVoid);
            mIsAvailable->Set(TRUE);
        }

        //@Override
        protected Void DoInBackground(UpdateData... params) {
            final List<SearchIndexableData> dataToUpdate = params[0].dataToUpdate;
            final List<SearchIndexableData> dataToDelete = params[0].dataToDelete;
            final Map<String, List<String>> nonIndexableKeys = params[0].nonIndexableKeys;

            final Boolean forceUpdate = params[0].forceUpdate;

            final SQLiteDatabase database = GetWritableDatabase();
            final String localeStr = Locale->GetDefault()->ToString();

            try {
                database->BeginTransaction();
                if (dataToDelete->Size() > 0) {
                    ProcessDataToDelete(database, localeStr, dataToDelete);
                }
                if (dataToUpdate->Size() > 0) {
                    ProcessDataToUpdate(database, localeStr, dataToUpdate, nonIndexableKeys,
                            forceUpdate);
                }
                database->SetTransactionSuccessful();
            } finally {
                database->EndTransaction();
            }

            return NULL;
        }

        private Boolean ProcessDataToUpdate(SQLiteDatabase database, String localeStr,
                List<SearchIndexableData> dataToUpdate, Map<String, List<String>> nonIndexableKeys,
                Boolean forceUpdate) {

            if (!forceUpdate && IsLocaleAlreadyIndexed(database, localeStr)) {
                Logger::D(LOG_TAG, "Locale '" + localeStr + "' is already indexed");
                return TRUE;
            }

            Boolean result = FALSE;
            final Int64 current = System->CurrentTimeMillis();

            final Int32 count = dataToUpdate->Size();
            for (Int32 n = 0; n < count; n++) {
                final SearchIndexableData data = dataToUpdate->Get(n);
                try {
                    IndexOneSearchIndexableData(database, localeStr, data, nonIndexableKeys);
                } catch (Exception e) {
                    Logger::E(LOG_TAG,
                            "Cannot index: " + data.className + " for locale: " + localeStr, e);
                }
            }

            final Int64 now = System->CurrentTimeMillis();
            Logger::D(LOG_TAG, "Indexing locale '" + localeStr + "' took " +
                    (now - current) + " millis");
            return result;
        }

        private Boolean ProcessDataToDelete(SQLiteDatabase database, String localeStr,
                List<SearchIndexableData> dataToDelete) {

            Boolean result = FALSE;
            final Int64 current = System->CurrentTimeMillis();

            final Int32 count = dataToDelete->Size();
            for (Int32 n = 0; n < count; n++) {
                final SearchIndexableData data = dataToDelete->Get(n);
                if (data == NULL) {
                    continue;
                }
                if (!TextUtils->IsEmpty(data.className)) {
                    Delete(database, IndexColumns.CLASS_NAME, data.className);
                } else  {
                    if (data instanceof SearchIndexableRaw) {
                        final SearchIndexableRaw raw = (SearchIndexableRaw) data;
                        if (!TextUtils->IsEmpty(raw.title)) {
                            Delete(database, IndexColumns.DATA_TITLE, raw.title);
                        }
                    }
                }
            }

            final Int64 now = System->CurrentTimeMillis();
            Logger::D(LOG_TAG, "Deleting data for locale '" + localeStr + "' took " +
                    (now - current) + " millis");
            return result;
        }

        private Int32 Delete(SQLiteDatabase database, String columName, String value) {
            final String whereClause = columName + "=?";
            final String[] whereArgs = new String[] { value };

            return database->Delete(Tables.TABLE_PREFS_INDEX, whereClause, whereArgs);
        }

        private Boolean IsLocaleAlreadyIndexed(SQLiteDatabase database, String locale) {
            Cursor cursor = NULL;
            Boolean result = FALSE;
            final StringBuilder sb = new StringBuilder(IndexColumns.LOCALE);
            sb->Append(" = ");
            DatabaseUtils->AppendEscapedSQLString(sb, locale);
            try {
                // We care only for 1 row
                cursor = database->Query(Tables.TABLE_PREFS_INDEX, NULL,
                        sb->ToString(), NULL, NULL, NULL, NULL, "1");
                final Int32 count = cursor->GetCount();
                result = (count >= 1);
            } finally {
                if (cursor != NULL) {
                    cursor->Close();
                }
            }
            return result;
        }
    }

    /**
     * A basic AsyncTask for saving a Search query into the database
     */
    private class SaveSearchQueryTask extends AsyncTask<String, Void, Long> {

        //@Override
        protected Long DoInBackground(String... params) {
            final Int64 now = new Date()->GetTime();

            final ContentValues values = new ContentValues();
            values->Put(IndexDatabaseHelper.SavedQueriesColums.QUERY, params[0]);
            values->Put(IndexDatabaseHelper.SavedQueriesColums.TIME_STAMP, now);

            final SQLiteDatabase database = GetWritableDatabase();

            Int64 lastInsertedRowId = -1;
            try {
                // First, delete all saved queries that are the same
                database->Delete(Tables.TABLE_SAVED_QUERIES,
                        IndexDatabaseHelper.SavedQueriesColums.QUERY + " = ?",
                        new String[] { params[0] });

                // Second, insert the saved query
                lastInsertedRowId =
                        database->InsertOrThrow(Tables.TABLE_SAVED_QUERIES, NULL, values);

                // Last, remove "old" saved queries
                final Int64 delta = lastInsertedRowId - MAX_SAVED_SEARCH_QUERY;
                if (delta > 0) {
                    Int32 count = database->Delete(Tables.TABLE_SAVED_QUERIES, "rowId <= ?",
                            new String[] { Long->ToString(delta) });
                    Logger::D(LOG_TAG, "Deleted '" + count + "' saved Search Query(ies)");
                }
            } catch (Exception e) {
                Logger::D(LOG_TAG, "Cannot update saved Search queries", e);
            }

            return lastInsertedRowId;
        }
    }
}
