

package com.android.settings.dashboard;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IIndex;

using Elastos::Utility::IHashMap;

public class SearchResultsSummary extends Fragment {

    private static const String LOG_TAG = "SearchResultsSummary";

    private static const String EMPTY_QUERY = "";
    private static Char32 ELLIPSIS = '\u2026';

    private static const String SAVE_KEY_SHOW_RESULTS = ":settings:show_results";

    private SearchView mSearchView;

    private ListView mResultsListView;
    private SearchResultsAdapter mResultsAdapter;
    private UpdateSearchResultsTask mUpdateSearchResultsTask;

    private ListView mSuggestionsListView;
    private SuggestionsAdapter mSuggestionsAdapter;
    private UpdateSuggestionsTask mUpdateSuggestionsTask;

    private ViewGroup mLayoutSuggestions;
    private ViewGroup mLayoutResults;

    private String mQuery;

    private Boolean mShowResults;

    /**
     * A basic AsyncTask for updating the query results cursor
     */
    private class UpdateSearchResultsTask extends AsyncTask<String, Void, Cursor> {
        //@Override
        protected Cursor DoInBackground(String... params) {
            return Index->GetInstance(GetActivity()).Search(params[0]);
        }

        //@Override
        protected void OnPostExecute(Cursor cursor) {
            if (!IsCancelled()) {
                SetResultsCursor(cursor);
                SetResultsVisibility(cursor->GetCount() > 0);
            } else if (cursor != NULL) {
                cursor->Close();
            }
        }
    }

    /**
     * A basic AsyncTask for updating the suggestions cursor
     */
    private class UpdateSuggestionsTask extends AsyncTask<String, Void, Cursor> {
        //@Override
        protected Cursor DoInBackground(String... params) {
            return Index->GetInstance(GetActivity()).GetSuggestions(params[0]);
        }

        //@Override
        protected void OnPostExecute(Cursor cursor) {
            if (!IsCancelled()) {
                SetSuggestionsCursor(cursor);
                SetSuggestionsVisibility(cursor->GetCount() > 0);
            } else if (cursor != NULL) {
                cursor->Close();
            }
        }
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        mResultsAdapter = new SearchResultsAdapter(GetActivity());
        mSuggestionsAdapter = new SuggestionsAdapter(GetActivity());

        if (savedInstanceState != NULL) {
            mShowResults = savedInstanceState->GetBoolean(SAVE_KEY_SHOW_RESULTS);
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        outState->PutBoolean(SAVE_KEY_SHOW_RESULTS, mShowResults);
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();

        ClearSuggestions();
        ClearResults();
    }

    //@Override
    CARAPI OnDestroy() {
        mResultsListView = NULL;
        mResultsAdapter = NULL;
        mUpdateSearchResultsTask = NULL;

        mSuggestionsListView = NULL;
        mSuggestionsAdapter = NULL;
        mUpdateSuggestionsTask = NULL;

        mSearchView = NULL;

        super->OnDestroy();
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        final View view = inflater->Inflate(R.layout.search_panel, container, FALSE);

        mLayoutSuggestions = (ViewGroup) view->FindViewById(R.id.layout_suggestions);
        mLayoutResults = (ViewGroup) view->FindViewById(R.id.layout_results);

        mResultsListView = (ListView) view->FindViewById(R.id.list_results);
        mResultsListView->SetAdapter(mResultsAdapter);
        mResultsListView->SetOnItemClickListener(new AdapterView->OnItemClickListener() {
            //@Override
            CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
                // We have a header, so we need to decrement the position by one
                position--;

                // Some Monkeys could create a case where they were probably clicking on the
                // List Header and thus the position passed was "0" and then by decrement was "-1"
                if (position < 0) {
                    return;
                }

                final Cursor cursor = mResultsAdapter.mCursor;
                cursor->MoveToPosition(position);

                final String className = cursor->GetString(Index.COLUMN_INDEX_CLASS_NAME);
                final String screenTitle = cursor->GetString(Index.COLUMN_INDEX_SCREEN_TITLE);
                final String action = cursor->GetString(Index.COLUMN_INDEX_INTENT_ACTION);
                final String key = cursor->GetString(Index.COLUMN_INDEX_KEY);

                final SettingsActivity sa = (SettingsActivity) GetActivity();
                sa->NeedToRevertToInitialFragment();

                if (TextUtils->IsEmpty(action)) {
                    Bundle args = new Bundle();
                    args->PutString(SettingsActivity.EXTRA_FRAGMENT_ARG_KEY, key);

                    Utils->StartWithFragment(sa, className, args, NULL, 0, -1, screenTitle);
                } else {
                    final Intent intent = new Intent(action);

                    final String targetPackage = cursor->GetString(
                            Index.COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE);
                    final String targetClass = cursor->GetString(
                            Index.COLUMN_INDEX_INTENT_ACTION_TARGET_CLASS);
                    if (!TextUtils->IsEmpty(targetPackage) && !TextUtils->IsEmpty(targetClass)) {
                        final ComponentName component =
                                new ComponentName(targetPackage, targetClass);
                        intent->SetComponent(component);
                    }
                    intent->PutExtra(SettingsActivity.EXTRA_FRAGMENT_ARG_KEY, key);

                    sa->StartActivity(intent);
                }

                SaveQueryToDatabase();
            }
        });
        mResultsListView->AddHeaderView(
                LayoutInflater->From(GetActivity()).Inflate(
                        R.layout.search_panel_results_header, mResultsListView, FALSE),
                NULL, FALSE);

        mSuggestionsListView = (ListView) view->FindViewById(R.id.list_suggestions);
        mSuggestionsListView->SetAdapter(mSuggestionsAdapter);
        mSuggestionsListView->SetOnItemClickListener(new AdapterView->OnItemClickListener() {
            //@Override
            CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
                // We have a header, so we need to decrement the position by one
                position--;
                // Some Monkeys could create a case where they were probably clicking on the
                // List Header and thus the position passed was "0" and then by decrement was "-1"
                if (position < 0) {
                    return;
                }
                final Cursor cursor = mSuggestionsAdapter.mCursor;
                cursor->MoveToPosition(position);

                mShowResults = TRUE;
                mQuery = cursor->GetString(0);
                mSearchView->SetQuery(mQuery, FALSE);
            }
        });
        mSuggestionsListView->AddHeaderView(
                LayoutInflater->From(GetActivity()).Inflate(
                        R.layout.search_panel_suggestions_header, mSuggestionsListView, FALSE),
                NULL, FALSE);

        return view;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (!mShowResults) {
            ShowSomeSuggestions();
        }
    }

    CARAPI SetSearchView(SearchView searchView) {
        mSearchView = searchView;
    }

    private void SetSuggestionsVisibility(Boolean visible) {
        if (mLayoutSuggestions != NULL) {
            mLayoutSuggestions->SetVisibility(visible ? View.VISIBLE : View.GONE);
        }
    }

    private void SetResultsVisibility(Boolean visible) {
        if (mLayoutResults != NULL) {
            mLayoutResults->SetVisibility(visible ? View.VISIBLE : View.GONE);
        }
    }

    private void SaveQueryToDatabase() {
        Index->GetInstance(GetActivity()).AddSavedQuery(mQuery);
    }

    public Boolean OnQueryTextSubmit(String query) {
        mQuery = GetFilteredQueryString(query);
        mShowResults = TRUE;
        SetSuggestionsVisibility(FALSE);
        UpdateSearchResults();
        SaveQueryToDatabase();
        return TRUE;
    }

    public Boolean OnQueryTextChange(String query) {
        final String newQuery = GetFilteredQueryString(query);

        mQuery = newQuery;

        if (TextUtils->IsEmpty(mQuery)) {
            mShowResults = FALSE;
            SetResultsVisibility(FALSE);
            UpdateSuggestions();
        } else {
            mShowResults = TRUE;
            SetSuggestionsVisibility(FALSE);
            UpdateSearchResults();
        }

        return TRUE;
    }

    CARAPI ShowSomeSuggestions() {
        SetResultsVisibility(FALSE);
        mQuery = EMPTY_QUERY;
        UpdateSuggestions();
    }

    private void ClearSuggestions() {
        if (mUpdateSuggestionsTask != NULL) {
            mUpdateSuggestionsTask->Cancel(FALSE);
            mUpdateSuggestionsTask = NULL;
        }
        SetSuggestionsCursor(NULL);
    }

    private void SetSuggestionsCursor(Cursor cursor) {
        if (mSuggestionsAdapter == NULL) {
            return;
        }
        Cursor oldCursor = mSuggestionsAdapter->SwapCursor(cursor);
        if (oldCursor != NULL) {
            oldCursor->Close();
        }
    }

    private void ClearResults() {
        if (mUpdateSearchResultsTask != NULL) {
            mUpdateSearchResultsTask->Cancel(FALSE);
            mUpdateSearchResultsTask = NULL;
        }
        SetResultsCursor(NULL);
    }

    private void SetResultsCursor(Cursor cursor) {
        if (mResultsAdapter == NULL) {
            return;
        }
        Cursor oldCursor = mResultsAdapter->SwapCursor(cursor);
        if (oldCursor != NULL) {
            oldCursor->Close();
        }
    }

    private String GetFilteredQueryString(CharSequence query) {
        if (query == NULL) {
            return NULL;
        }
        final StringBuilder filtered = new StringBuilder();
        for (Int32 n = 0; n < query->Length(); n++) {
            Char32 c = query->CharAt(n);
            if (!Character->IsLetterOrDigit(c) && !Character->IsSpaceChar(c)) {
                continue;
            }
            filtered->Append(c);
        }
        return filtered->ToString();
    }

    private void ClearAllTasks() {
        if (mUpdateSearchResultsTask != NULL) {
            mUpdateSearchResultsTask->Cancel(FALSE);
            mUpdateSearchResultsTask = NULL;
        }
        if (mUpdateSuggestionsTask != NULL) {
            mUpdateSuggestionsTask->Cancel(FALSE);
            mUpdateSuggestionsTask = NULL;
        }
    }

    private void UpdateSuggestions() {
        ClearAllTasks();
        if (mQuery == NULL) {
            SetSuggestionsCursor(NULL);
        } else {
            mUpdateSuggestionsTask = new UpdateSuggestionsTask();
            mUpdateSuggestionsTask->Execute(mQuery);
        }
    }

    private void UpdateSearchResults() {
        ClearAllTasks();
        if (TextUtils->IsEmpty(mQuery)) {
            SetResultsVisibility(FALSE);
            SetResultsCursor(NULL);
        } else {
            mUpdateSearchResultsTask = new UpdateSearchResultsTask();
            mUpdateSearchResultsTask->Execute(mQuery);
        }
    }

    private static class SuggestionItem {
        public String query;

        public SuggestionItem(String query) {
            this.query = query;
        }
    }

    private static class SuggestionsAdapter extends BaseAdapter {

        private static const Int32 COLUMN_SUGGESTION_QUERY = 0;
        private static const Int32 COLUMN_SUGGESTION_TIMESTAMP = 1;

        private Context mContext;
        private Cursor mCursor;
        private LayoutInflater mInflater;
        private Boolean mDataValid = FALSE;

        public SuggestionsAdapter(Context context) {
            mContext = context;
            mInflater = (LayoutInflater) mContext->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mDataValid = FALSE;
        }

        public Cursor SwapCursor(Cursor newCursor) {
            if (newCursor == mCursor) {
                return NULL;
            }
            Cursor oldCursor = mCursor;
            mCursor = newCursor;
            if (newCursor != NULL) {
                mDataValid = TRUE;
                NotifyDataSetChanged();
            } else {
                mDataValid = FALSE;
                NotifyDataSetInvalidated();
            }
            return oldCursor;
        }

        //@Override
        public Int32 GetCount() {
            if (!mDataValid || mCursor == NULL || mCursor->IsClosed()) return 0;
            return mCursor->GetCount();
        }

        //@Override
        public Object GetItem(Int32 position) {
            if (mDataValid && mCursor->MoveToPosition(position)) {
                final String query = mCursor->GetString(COLUMN_SUGGESTION_QUERY);

                return new SuggestionItem(query);
            }
            return NULL;
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return 0;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            if (!mDataValid && convertView == NULL) {
                throw new IllegalStateException(
                        "this should only be called when the cursor is valid");
            }
            if (!mCursor->MoveToPosition(position)) {
                throw new IllegalStateException("couldn't move cursor to position " + position);
            }

            View view;

            if (convertView == NULL) {
                view = mInflater->Inflate(R.layout.search_suggestion_item, parent, FALSE);
            } else {
                view = convertView;
            }

            TextView query = (TextView) view->FindViewById(R.id.title);

            SuggestionItem item = (SuggestionItem) GetItem(position);
            query->SetText(item.query);

            return view;
        }
    }

    private static class SearchResult {
        public Context context;
        public String title;
        public String summaryOn;
        public String summaryOff;
        public String entries;
        public Int32 iconResId;
        public String key;

        public SearchResult(Context context, String title, String summaryOn, String summaryOff,
                            String entries, Int32 iconResId, String key) {
            this.context = context;
            this.title = title;
            this.summaryOn = summaryOn;
            this.summaryOff = summaryOff;
            this.entries = entries;
            this.iconResId = iconResId;
            this.key = key;
        }
    }

    private static class SearchResultsAdapter extends BaseAdapter {

        private Context mContext;
        private Cursor mCursor;
        private LayoutInflater mInflater;
        private Boolean mDataValid;
        private HashMap<String, Context> mContextMap = new HashMap<String, Context>();

        private static const String PERCENT_RECLACE = "%s";
        private static const String DOLLAR_REPLACE = "$s";

        public SearchResultsAdapter(Context context) {
            mContext = context;
            mInflater = (LayoutInflater) mContext->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mDataValid = FALSE;
        }

        public Cursor SwapCursor(Cursor newCursor) {
            if (newCursor == mCursor) {
                return NULL;
            }
            Cursor oldCursor = mCursor;
            mCursor = newCursor;
            if (newCursor != NULL) {
                mDataValid = TRUE;
                NotifyDataSetChanged();
            } else {
                mDataValid = FALSE;
                NotifyDataSetInvalidated();
            }
            return oldCursor;
        }

        //@Override
        public Int32 GetCount() {
            if (!mDataValid || mCursor == NULL || mCursor->IsClosed()) return 0;
            return mCursor->GetCount();
        }

        //@Override
        public Object GetItem(Int32 position) {
            if (mDataValid && mCursor->MoveToPosition(position)) {
                final String title = mCursor->GetString(Index.COLUMN_INDEX_TITLE);
                final String summaryOn = mCursor->GetString(Index.COLUMN_INDEX_SUMMARY_ON);
                final String summaryOff = mCursor->GetString(Index.COLUMN_INDEX_SUMMARY_OFF);
                final String entries = mCursor->GetString(Index.COLUMN_INDEX_ENTRIES);
                final String iconResStr = mCursor->GetString(Index.COLUMN_INDEX_ICON);
                final String className = mCursor->GetString(
                        Index.COLUMN_INDEX_CLASS_NAME);
                final String packageName = mCursor->GetString(
                        Index.COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE);
                final String key = mCursor->GetString(
                        Index.COLUMN_INDEX_KEY);

                Context packageContext;
                if (TextUtils->IsEmpty(className) && !TextUtils->IsEmpty(packageName)) {
                    packageContext = mContextMap->Get(packageName);
                    if (packageContext == NULL) {
                        try {
                            packageContext = mContext->CreatePackageContext(packageName, 0);
                        } catch (PackageManager.NameNotFoundException e) {
                            Logger::E(LOG_TAG, "Cannot create Context for package: " + packageName);
                            return NULL;
                        }
                        mContextMap->Put(packageName, packageContext);
                    }
                } else {
                    packageContext = mContext;
                }

                final Int32 iconResId = TextUtils->IsEmpty(iconResStr) ?
                        R.drawable.empty_icon : Integer->ParseInt(iconResStr);

                return new SearchResult(packageContext, title, summaryOn, summaryOff,
                        entries, iconResId, key);
            }
            return NULL;
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return 0;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            if (!mDataValid && convertView == NULL) {
                throw new IllegalStateException(
                        "this should only be called when the cursor is valid");
            }
            if (!mCursor->MoveToPosition(position)) {
                throw new IllegalStateException("couldn't move cursor to position " + position);
            }

            View view;
            TextView textTitle;
            ImageView imageView;

            if (convertView == NULL) {
                view = mInflater->Inflate(R.layout.search_result_item, parent, FALSE);
            } else {
                view = convertView;
            }

            textTitle = (TextView) view->FindViewById(R.id.title);
            imageView = (ImageView) view->FindViewById(R.id.icon);

            final SearchResult result = (SearchResult) GetItem(position);
            textTitle->SetText(result.title);

            if (result.iconResId != R.drawable.empty_icon) {
                final Context packageContext = result.context;
                final Drawable drawable;
                try {
                    drawable = packageContext->GetDrawable(result.iconResId);
                    imageView->SetImageDrawable(drawable);
                } catch (Resources.NotFoundException nfe) {
                    // Not much we can do except logging
                    Logger::E(LOG_TAG, "Cannot load Drawable for " + result.title);
                }
            } else {
                imageView->SetImageDrawable(NULL);
                imageView->SetBackgroundResource(R.drawable.empty_icon);
            }

            return view;
        }
    }
}
