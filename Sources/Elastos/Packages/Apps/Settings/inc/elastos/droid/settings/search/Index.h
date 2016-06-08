#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_INDEX_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_INDEX_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Provider::ISearchIndexableData;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

class Index
    : public Object
{
private:
    /**
     * A private class to describe the update data for the Index database
     */
    class UpdateData
        : public Object
    {
    public:
        UpdateData();

        UpdateData(
            /* [in] */ UpdateData* other);

        ~UpdateData();

        CARAPI_(AutoPtr<Index::UpdateData>) Copy();

        CARAPI Clear();

    public:
        AutoPtr<IList> mDataToUpdate; //List<SearchIndexableData>
        AutoPtr<IList> mDataToDelete; // List<SearchIndexableData>
        AutoPtr<IMap> mNonIndexableKeys; //Map<String, List<String>>

        Boolean mForceUpdate;
    };

    /**
     * A private class for updating the Index database
     */
    class UpdateIndexTask
        //<UpdateData, Integer, Void>
        : public AsyncTask
    {
    public:
        UpdateIndexTask(
            /* [in] */ Index* host);

        ~UpdateIndexTask();

    protected:
        //@Override
        CARAPI OnPreExecute();

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result); // Void

        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        CARAPI_(Boolean) ProcessDataToUpdate(
            /* [in] */ ISQLiteDatabase* database,
            /* [in] */ const String& localeStr,
            /* [in] */ IList* dataToUpdate, //List<SearchIndexableData>
            /* [in] */ IMap* nonIndexableKeys, //Map<String, List<String>>
            /* [in] */ Boolean forceUpdate);

        CARAPI_(Boolean) ProcessDataToDelete(
            /* [in] */ ISQLiteDatabase* database,
            /* [in] */ const String& localeStr,
            /* [in] */ IList* dataToDelete); // List<SearchIndexableData>

        CARAPI_(Int32) Delete(
            /* [in] */ ISQLiteDatabase* database,
            /* [in] */ const String& columName,
            /* [in] */ const String& value);

        CARAPI_(Boolean) IsLocaleAlreadyIndexed(
            /* [in] */ ISQLiteDatabase* database,
            /* [in] */ const String& locale);

    private:
        Index* mHost;
    };

    /**
     * A basic AsyncTask for saving a Search query into the database
     */
    class SaveSearchQueryTask
        //<String, Void, Long>
        : public AsyncTask
    {
    public:
        SaveSearchQueryTask(
            /* [in] */ Index* host);

        ~SaveSearchQueryTask();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        Index* mHost;
    };

public:
    Index(
        /* [in] */ IContext* context,
        /* [in] */ const String& baseAuthority);

    ~Index();

    /**
     * A basic singleton
     */
    static CARAPI_(AutoPtr<Index>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsAvailable();

    CARAPI_(AutoPtr<ICursor>) Search(
        /* [in] */ const String& query);

    CARAPI_(AutoPtr<ICursor>) GetSuggestions(
        /* [in] */ const String& query);

    CARAPI AddSavedQuery(
        /* [in] */ const String& query,
        /* [out] */ Int64* result);

    CARAPI Update();

    CARAPI_(void) AddIndexableData(
        /* [in] */ ISearchIndexableData* data);

    CARAPI_(void) AddIndexableData(
        /* [in] */ ArrayOf<ISearchIndexableResource*>* array);

    CARAPI_(void) DeleteIndexableData(
        /* [in] */ ISearchIndexableData* data);

    CARAPI_(void) AddNonIndexableKeys(
        /* [in] */ const String& authority,
        /* [in] */ IList* keys); //List<String>

    /**
     * Update the Index for a specific class name resources
     *
     * @param className the class name (typically a fragment name).
     * @param rebuild TRUE means that you want to delete the data from the Index first.
     * @param includeInSearchResults TRUE means that you want the bit "enabled" set so that the
     *                               data will be seen included into the search results
     */
    CARAPI UpdateFromClassNameResource(
        /* [in] */ const String& className,
        /* [in] */ Boolean rebuild,
        /* [in] */ Boolean includeInSearchResults);

    CARAPI UpdateFromSearchIndexableData(
        /* [in] */ ISearchIndexableData* data);

private:
    CARAPI_(String) BuildSuggestionsSQL(
        /* [in] */ const String& query);

    CARAPI_(Boolean) AddIndexablesFromRemoteProvider(
        /* [in] */ const String& packageName,
        /* [in] */ const String& authority);

    CARAPI_(void) AddNonIndexablesKeysFromRemoteProvider(
        /* [in] */ const String& packageName,
        /* [in] */ const String& authority);

    CARAPI GetNonIndexablesKeysFromRemoteProvider(
        /* [in] */ const String& packageName,
        /* [in] */ const String& authority,
        /* [out] */ IList** list);

    CARAPI GetNonIndexablesKeys(
        /* [in] */ IContext* packageContext,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ IList** list);

    /**
     * Only allow a "well known" SearchIndexablesProvider. The provider should:
     *
     * - have read/write {@link Manifest::permission::READ_SEARCH_INDEXABLES}
     * - be from a privileged package
     */
    CARAPI_(Boolean) IsWellKnownProvider(
        /* [in] */ IResolveInfo* info);

    CARAPI IsPrivilegedPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean* result);

    CARAPI_(void) UpdateFromRemoteProvider(
        /* [in] */ const String& packageName,
        /* [in] */ const String& authority);

    CARAPI_(AutoPtr<ISQLiteDatabase>) GetReadableDatabase();

    CARAPI_(AutoPtr<ISQLiteDatabase>) GetWritableDatabase();

    static CARAPI_(AutoPtr<IUri>) BuildUriForXmlResources(
        /* [in] */ const String& authority);

    static CARAPI_(AutoPtr<IUri>) BuildUriForRawData(
        /* [in] */ const String& authority);

    static CARAPI_(AutoPtr<IUri>) BuildUriForNonIndexableKeys(
        /* [in] */ const String& authority);

    void UpdateInternal();

    void AddIndexablesForXmlResourceUri(
        /* [in] */ IContext* packageContext,
        /* [in] */ const String& packageName,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 baseRank);

    void AddIndexablesForRawDataUri(
        /* [in] */ IContext* packageContext,
        /* [in] */ const String& packageName,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 baseRank);

    CARAPI_(String) BuildSearchSQL(
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* colums,
        /* [in] */ Boolean withOrderBy);

    CARAPI_(String) BuildSearchSQLForColumn(
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* columnNames);

    CARAPI_(String) BuildSearchWhereStringForColumns(
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* columnNames);

    CARAPI_(String) BuildSearchMatchStringForColumns(
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* columnNames);

    CARAPI_(void) IndexOneSearchIndexableData(
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& localeStr,
        /* [in] */ ISearchIndexableData* data,
        /* [in] */ IMap* nonIndexableKeys); //Map<String, List<String>>

    CARAPI_(void) IndexOneRaw(
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& localeStr,
        /* [in] */ ISearchIndexableRaw* raw);

    static CARAPI_(Boolean) IsIndexableClass(
        /* [in] */ const String& className);

    // static Class<?> GetIndexableClass(
    //     /* [in] */ const String& className);

     CARAPI_(void) IndexOneResource(
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& localeStr,
        /* [in] */ ISearchIndexableResource* sir,
        /* [in] */ IMap* nonIndexableKeysFromResource); //Map<String, List<String>>

    CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSearchIndexProvider(
        /* [in] */ const String& className);

    CARAPI IndexFromResource(
        /* [in] */ IContext* context,
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& localeStr,
        /* [in] */ Int32 xmlResId,
        /* [in] */ const String& fragmentName,
        /* [in] */ Int32 iconResId,
        /* [in] */ Int32 rank,
        /* [in] */ const String& intentAction,
        /* [in] */ const String& intentTargetPackage,
        /* [in] */ const String& intentTargetClass,
        /* [in] */ IList* nonIndexableKeys); //List<String>

    CARAPI_(void) IndexFromProvider(
        /* [in] */ IContext* context,
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& localeStr,
        /* [in] */ IIndexableSearchIndexProvider* provider,
        /* [in] */ const String& className,
        /* [in] */ Int32 iconResId,
        /* [in] */ Int32 rank,
        /* [in] */ Boolean enabled,
        /* [in] */ IList* nonIndexableKeys); //List<String>

    CARAPI_(void) UpdateOneRowWithFilteredData(
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& locale,
        /* [in] */ const String& title,
        /* [in] */ const String& summaryOn,
        /* [in] */ const String& summaryOff,
        /* [in] */ const String& entries,
        /* [in] */ const String& className,
        /* [in] */ const String& screenTitle,
        /* [in] */ Int32 iconResId,
        /* [in] */ Int32 rank,
        /* [in] */ const String& keywords,
        /* [in] */ const String& intentAction,
        /* [in] */ const String& intentTargetPackage,
        /* [in] */ const String& intentTargetClass,
        /* [in] */ Boolean enabled,
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

    static CARAPI_(String) NormalizeHyphen(
        /* [in] */ const String& input);

    static CARAPI_(String) NormalizeString(
        /* [in] */ const String& input);

    CARAPI_(void) UpdateOneRow(
        /* [in] */ ISQLiteDatabase* database,
        /* [in] */ const String& locale,
        /* [in] */ const String& updatedTitle,
        /* [in] */ const String& normalizedTitle,
        /* [in] */ const String& updatedSummaryOn,
        /* [in] */ const String& normalizedSummaryOn,
        /* [in] */ const String& updatedSummaryOff,
        /* [in] */ const String& normalizedSummaryOff,
        /* [in] */ const String& entries,
        /* [in] */ const String& className,
        /* [in] */ const String& screenTitle,
        /* [in] */ Int32 iconResId,
        /* [in] */ Int32 rank,
        /* [in] */ const String& keywords,
        /* [in] */ const String& intentAction,
        /* [in] */ const String& intentTargetPackage,
        /* [in] */ const String& intentTargetClass,
        /* [in] */ Boolean enabled,
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

    CARAPI_(String) GetDataKey(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataTitle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataSummary(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataSummaryOn(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataSummaryOff(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataEntries(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetDataKeywords(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(String) GetData(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [in] */ Int32 resId);

    CARAPI_(String) GetDataEntries(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [in] */ Int32 resId);

    CARAPI_(Int32) GetResId(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [in] */ Int32 resId);

public:
    // Those indices should match the indices of SELECT_COLUMNS !
    static const Int32 COLUMN_INDEX_RANK;
    static const Int32 COLUMN_INDEX_TITLE;
    static const Int32 COLUMN_INDEX_SUMMARY_ON;
    static const Int32 COLUMN_INDEX_SUMMARY_OFF;
    static const Int32 COLUMN_INDEX_ENTRIES;
    static const Int32 COLUMN_INDEX_KEYWORDS;
    static const Int32 COLUMN_INDEX_CLASS_NAME;
    static const Int32 COLUMN_INDEX_SCREEN_TITLE;
    static const Int32 COLUMN_INDEX_ICON;
    static const Int32 COLUMN_INDEX_INTENT_ACTION;
    static const Int32 COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE;
    static const Int32 COLUMN_INDEX_INTENT_ACTION_TARGET_CLASS;
    static const Int32 COLUMN_INDEX_ENABLED;
    static const Int32 COLUMN_INDEX_KEY;
    static const Int32 COLUMN_INDEX_USER_ID;

    static const String ENTRIES_SEPARATOR;

private:
    static const String TAG;

    // If you change the order of columns here, you SHOULD change the COLUMN_INDEX_XXX values
    static const AutoPtr< ArrayOf<String> > SELECT_COLUMNS;

    static const AutoPtr< ArrayOf<String> > MATCH_COLUMNS_PRIMARY;

    static const AutoPtr< ArrayOf<String> > MATCH_COLUMNS_SECONDARY;

    // Max number of saved search queries (who will be used for proposing suggestions)
    static Int64 MAX_SAVED_SEARCH_QUERY;
    // Max number of proposed suggestions
    static const Int32 MAX_PROPOSED_SUGGESTIONS;

    static const String BASE_AUTHORITY;

    static const String EMPTY;
    static const String NON_BREAKING_HYPHEN;
    static const String HYPHEN;

    static const String FIELD_NAME_SEARCH_INDEX_DATA_PROVIDER;

    static const String NODE_NAME_PREFERENCE_SCREEN;
    static const String NODE_NAME_CHECK_BOX_PREFERENCE;
    static const String NODE_NAME_LIST_PREFERENCE;

    static const AutoPtr<IList> EMPTY_LIST;

    static AutoPtr<Index> sInstance;

    static const AutoPtr<IPattern> REMOVE_DIACRITICALS_PATTERN;

    AutoPtr<IAtomicBoolean> mIsAvailable;
    AutoPtr<UpdateData> mDataToProcess;
    AutoPtr<IContext> mContext;
    String mBaseAuthority;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_INDEX_H__
