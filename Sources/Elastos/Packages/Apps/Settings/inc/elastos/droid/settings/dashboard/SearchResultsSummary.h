

#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_SEARCHRESULTSSUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_SEARCHRESULTSSUMMARY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/os/AsyncTask.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

class SearchResultsSummary
    : public Fragment
    , public ISearchResultsSummary
{
private:
    /**
     * A basic AsyncTask for updating the query results cursor
     */
    class UpdateSearchResultsTask
        /* AsyncTask<String, Void, Cursor> */
        : public AsyncTask
    {
    public:
        UpdateSearchResultsTask(
            /* [in] */ SearchResultsSummary* host);

        ~UpdateSearchResultsTask();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* cursor);

    private:
        SearchResultsSummary* mHost;
    };

    /**
     * A basic AsyncTask for updating the suggestions cursor
     */
    class UpdateSuggestionsTask
        /* AsyncTask<String, Void, Cursor> */
        : public AsyncTask
    {
    public:
        UpdateSuggestionsTask(
            /* [in] */ SearchResultsSummary* host);

        ~UpdateSuggestionsTask();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* cursor);

    private:
        SearchResultsSummary* mHost;
    };

    class SuggestionItem
        : public Object
    {
    public:
        SuggestionItem(
            /* [in] */ const String& query);

        ~SuggestionItem();

    public:
        String mQuery;
    };

    class SuggestionsAdapter
        : public BaseAdapter
    {
        friend class SearchResultsSummary;
    public:
        SuggestionsAdapter(
            /* [in] */ IContext* context);

        ~SuggestionsAdapter();

        CARAPI SwapCursor(
            /* [in] */ ICursor* newCursor,
            /* [out] */ ICursor** cursor);

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        static const Int32 COLUMN_SUGGESTION_QUERY;
        static const Int32 COLUMN_SUGGESTION_TIMESTAMP;

        AutoPtr<IContext> mContext;
        AutoPtr<ICursor> mCursor;
        AutoPtr<ILayoutInflater> mInflater;
        Boolean mDataValid;
    };

    class SearchResult
        : public Object
    {
    public:
        SearchResult(
            /* [in] */ IContext* context,
            /* [in] */ const String& title,
            /* [in] */ const String& summaryOn,
            /* [in] */ const String& summaryOff,
            /* [in] */ const String& entries,
            /* [in] */ Int32 iconResId,
            /* [in] */ const String& key);

        ~SearchResult();

    public:
        AutoPtr<IContext> mContext;
        String mTitle;
        String mSummaryOn;
        String mSummaryOff;
        String mEntries;
        Int32 mIconResId;
        String mKey;
    };

    class SearchResultsAdapter
        : public BaseAdapter
    {
        friend class SearchResultsSummary;
    public:
        SearchResultsAdapter(
            /* [in] */ IContext* context);

        ~SearchResultsAdapter();

        CARAPI SwapCursor(
            /* [in] */ ICursor* newCursor,
            /* [out] */ ICursor** cursor);

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<ICursor> mCursor;
        AutoPtr<ILayoutInflater> mInflater;
        Boolean mDataValid;
        //HashMap<String, Context> mContextMap = new HashMap<String, Context>();
        AutoPtr<IHashMap> mContextMap;

        static const String PERCENT_RECLACE;
        static const String DOLLAR_REPLACE;
    };

    class AdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        AdapterViewOnItemClickListener(
            /* [in] */ SearchResultsSummary* host,
            /* [in] */ Int32 num);

        ~AdapterViewOnItemClickListener();

        //@Override
        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SearchResultsSummary* mHost;
        Int32 mNum;
    };

public:
    CAR_INTERFACE_DECL();

    SearchResultsSummary();

    ~SearchResultsSummary();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnResume();

    CARAPI SetSearchView(
        /* [in] */ ISearchView* searchView);

    CARAPI_(Boolean) OnQueryTextSubmit(
        /* [in] */ const String& query);

    CARAPI_(Boolean) OnQueryTextChange(
        /* [in] */ const String& query);

    CARAPI ShowSomeSuggestions();

private:
    CARAPI_(void) SetSuggestionsVisibility(
        /* [in] */ Boolean visible);

    CARAPI_(void) SetResultsVisibility(
        /* [in] */ Boolean visible);

    CARAPI_(void) SaveQueryToDatabase();

    CARAPI_(void) ClearSuggestions();

    CARAPI_(void) SetSuggestionsCursor(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) ClearResults();

    CARAPI_(void) SetResultsCursor(
        /* [in] */ ICursor* cursor);

    CARAPI_(String) GetFilteredQueryString(
        /* [in] */ ICharSequence* query);

    CARAPI_(void) ClearAllTasks();

    CARAPI_(void) UpdateSuggestions();

    CARAPI_(void) UpdateSearchResults();

private:
    static const String TAG;

    static const String EMPTY_QUERY;
    static Char32 ELLIPSIS;

    static const String SAVE_KEY_SHOW_RESULTS;

    AutoPtr<ISearchView> mSearchView;

    AutoPtr<IListView> mResultsListView;
    AutoPtr<SearchResultsAdapter> mResultsAdapter;
    AutoPtr<UpdateSearchResultsTask> mUpdateSearchResultsTask;

    AutoPtr<IListView> mSuggestionsListView;
    AutoPtr<SuggestionsAdapter> mSuggestionsAdapter;
    AutoPtr<UpdateSuggestionsTask> mUpdateSuggestionsTask;

    AutoPtr<IViewGroup> mLayoutSuggestions;
    AutoPtr<IViewGroup> mLayoutResults;

    String mQuery;

    Boolean mShowResults;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_SEARCHRESULTSSUMMARY_H__

