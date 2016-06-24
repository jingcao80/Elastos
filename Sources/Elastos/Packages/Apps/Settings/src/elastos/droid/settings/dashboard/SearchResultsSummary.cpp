
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/dashboard/SearchResultsSummary.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "../R.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Settings::Search::Index;
using Elastos::Droid::Settings::Utils;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const String SearchResultsSummary::TAG("SearchResultsSummary");
const String SearchResultsSummary::EMPTY_QUERY("");
Char32 SearchResultsSummary::ELLIPSIS = 0x2026;
const String SearchResultsSummary::SAVE_KEY_SHOW_RESULTS(":settings:show_results");

const Int32 SearchResultsSummary::SuggestionsAdapter::COLUMN_SUGGESTION_QUERY = 0;
const Int32 SearchResultsSummary::SuggestionsAdapter::COLUMN_SUGGESTION_TIMESTAMP = 1;

const String SearchResultsSummary::SearchResultsAdapter::PERCENT_RECLACE("%s");
const String SearchResultsSummary::SearchResultsAdapter::DOLLAR_REPLACE("$s");

//===============================================================================
//                  SearchResultsSummary::UpdateSearchResultsTask
//===============================================================================

SearchResultsSummary::UpdateSearchResultsTask::UpdateSearchResultsTask(
    /* [in] */ SearchResultsSummary* host)
    : mHost(host)
{}

SearchResultsSummary::UpdateSearchResultsTask::~UpdateSearchResultsTask()
{}

ECode SearchResultsSummary::UpdateSearchResultsTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    String str;
    ICharSequence::Probe((*params)[0])->ToString(&str);
    AutoPtr<ICursor> cursor = Index::GetInstance(
            IContext::Probe(activity))->Search(str);

    *result = cursor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SearchResultsSummary::UpdateSearchResultsTask::OnPostExecute(
    /* [in] */ IInterface* cursor)
{
    if (!IsCancelled()) {
        mHost->SetResultsCursor(ICursor::Probe(cursor));
        Int32 count;
        ICursor::Probe(cursor)->GetCount(&count);
        mHost->SetResultsVisibility(count > 0);
    }
    else if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    return NOERROR;
}

//===============================================================================
//                  SearchResultsSummary::UpdateSuggestionsTask
//===============================================================================

SearchResultsSummary::UpdateSuggestionsTask::UpdateSuggestionsTask(
    /* [in] */ SearchResultsSummary* host)
    : mHost(host)
{}

SearchResultsSummary::UpdateSuggestionsTask::~UpdateSuggestionsTask()
{}

ECode SearchResultsSummary::UpdateSuggestionsTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    String str;
    ICharSequence::Probe((*params)[0])->ToString(&str);
    AutoPtr<ICursor> cursor = Index::GetInstance(
            IContext::Probe(activity))->GetSuggestions(str);

    *result = cursor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SearchResultsSummary::UpdateSuggestionsTask::OnPostExecute(
    /* [in] */ IInterface* cursor)
{
    if (!IsCancelled()) {
        mHost->SetSuggestionsCursor(ICursor::Probe(cursor));
        Int32 count;
        ICursor::Probe(cursor)->GetCount(&count);
        mHost->SetSuggestionsVisibility(count > 0);
    }
    else if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    return NOERROR;
}

//===============================================================================
//                  SearchResultsSummary::SuggestionItem
//===============================================================================

SearchResultsSummary::SuggestionItem::SuggestionItem(
    /* [in] */ const String& query)
{
    mQuery = query;
}

SearchResultsSummary::SuggestionItem::~SuggestionItem()
{}

//===============================================================================
//                  SearchResultsSummary::SuggestionsAdapter
//===============================================================================

SearchResultsSummary::SuggestionsAdapter::SuggestionsAdapter(
    /* [in] */ IContext* context)
    : mDataValid(FALSE)
{
    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

SearchResultsSummary::SuggestionsAdapter::~SuggestionsAdapter()
{}

ECode SearchResultsSummary::SuggestionsAdapter::SwapCursor(
    /* [in] */ ICursor* newCursor,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = NULL;

    if (newCursor == mCursor) {
        return NOERROR;
    }
    AutoPtr<ICursor> oldCursor = mCursor;
    mCursor = newCursor;
    if (newCursor != NULL) {
        mDataValid = TRUE;
        NotifyDataSetChanged();
    }
    else {
        mDataValid = FALSE;
        NotifyDataSetInvalidated();
    }
    *cursor = oldCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode SearchResultsSummary::SuggestionsAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Boolean res;
    if (!mDataValid || mCursor == NULL || (mCursor->IsClosed(&res), res)) {
        return NOERROR;
    }

    return mCursor->GetCount(result);
}

ECode SearchResultsSummary::SuggestionsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    *item = NULL;

    Boolean res;
    if (mDataValid && (mCursor->MoveToPosition(position, &res), res)) {
        String query;
        mCursor->GetString(COLUMN_SUGGESTION_QUERY, &query);

        AutoPtr<SuggestionItem> si = new SuggestionItem(query);
        *item = (IObject*)si;
        REFCOUNT_ADD(*item);
        return NOERROR;
    }
    return NOERROR;
}

ECode SearchResultsSummary::SuggestionsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode SearchResultsSummary::SuggestionsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (!mDataValid && convertView == NULL) {
        Logger::E("SearchResultsSummary", "this should only be called when the cursor is valid");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException(
        //         "this should only be called when the cursor is valid");
    }
    Boolean res;
    if (mCursor->MoveToPosition(position, &res), !res) {
        Logger::E("SearchResultsSummary", "couldn't move cursor to position %d", position);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("couldn't move cursor to position " + position);
    }

    AutoPtr<IView> view;

    if (convertView == NULL) {
        mInflater->Inflate(R::layout::search_suggestion_item, parent, FALSE, (IView**)&view);
    }
    else {
        view = convertView;
    }

    AutoPtr<IView> otherView;
    view->FindViewById(R::id::title, (IView**)&otherView);
    AutoPtr<ITextView> query = ITextView::Probe(otherView);

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<SuggestionItem> item = (SuggestionItem*)IObject::Probe(obj);
    query->SetText(CoreUtils::Convert(item->mQuery));

    *result = view;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================================
//                  SearchResultsSummary::SearchResult
//===============================================================================

SearchResultsSummary::SearchResult::SearchResult(
    /* [in] */ IContext* context,
    /* [in] */ const String& title,
    /* [in] */ const String& summaryOn,
    /* [in] */ const String& summaryOff,
    /* [in] */ const String& entries,
    /* [in] */ Int32 iconResId,
    /* [in] */ const String& key)
    : mIconResId(0)
{
    mContext = context;
    mTitle = title;
    mSummaryOn = summaryOn;
    mSummaryOff = summaryOff;
    mEntries = entries;
    mIconResId = iconResId;
    mKey = key;
}

SearchResultsSummary::SearchResult::~SearchResult()
{}

//===============================================================================
//                  SearchResultsSummary::SearchResultsAdapter
//===============================================================================

SearchResultsSummary::SearchResultsAdapter::SearchResultsAdapter(
    /* [in] */ IContext* context)
    : mDataValid(FALSE)
{
    CHashMap::New((IHashMap**)&mContextMap);

    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

SearchResultsSummary::SearchResultsAdapter::~SearchResultsAdapter()
{}

ECode SearchResultsSummary::SearchResultsAdapter::SwapCursor(
    /* [in] */ ICursor* newCursor,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = NULL;

    if (newCursor == mCursor) {
        return NOERROR;
    }
    AutoPtr<ICursor> oldCursor = mCursor;
    mCursor = newCursor;
    if (newCursor != NULL) {
        mDataValid = TRUE;
        NotifyDataSetChanged();
    }
    else {
        mDataValid = FALSE;
        NotifyDataSetInvalidated();
    }
    *cursor = oldCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}


ECode SearchResultsSummary::SearchResultsAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Boolean res;
    if (!mDataValid || mCursor == NULL || (mCursor->IsClosed(&res), res)) {
        return NOERROR;
    }
    return mCursor->GetCount(result);
}


ECode SearchResultsSummary::SearchResultsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    *item = NULL;

    Boolean res;
    if (mDataValid && (mCursor->MoveToPosition(position, &res), res)) {
        String title;
        mCursor->GetString(Index::COLUMN_INDEX_TITLE, &title);
        String summaryOn;
        mCursor->GetString(Index::COLUMN_INDEX_SUMMARY_ON, &summaryOn);
        String summaryOff;
        mCursor->GetString(Index::COLUMN_INDEX_SUMMARY_OFF, &summaryOff);
        String entries;
        mCursor->GetString(Index::COLUMN_INDEX_ENTRIES, &entries);
        String iconResStr;
        mCursor->GetString(Index::COLUMN_INDEX_ICON, &iconResStr);
        String className;
        mCursor->GetString(Index::COLUMN_INDEX_CLASS_NAME, &className);
        String packageName;
        mCursor->GetString(
                Index::COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE, &packageName);
        String key;
        mCursor->GetString(Index::COLUMN_INDEX_KEY, &key);

        AutoPtr<IContext> packageContext;
        if (TextUtils::IsEmpty(className) && !TextUtils::IsEmpty(packageName)) {
            AutoPtr<IInterface> obj;
            mContextMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
            packageContext = IContext::Probe(obj);
            if (packageContext == NULL) {
                // try {
                ECode ec = mContext->CreatePackageContext(packageName, 0, (IContext**)&packageContext);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Cannot create Context for package: %s", packageName.string());
                    return NOERROR;
                }
                // } catch (PackageManager.NameNotFoundException e) {
                //     Logger::E(TAG, "Cannot create Context for package: " + packageName);
                //     return NULL;
                // }
                mContextMap->Put(CoreUtils::Convert(packageName), packageContext);
            }
        }
        else {
            packageContext = mContext;
        }

        const Int32 iconResId = TextUtils::IsEmpty(iconResStr) ?
                R::drawable::empty_icon : StringUtils::ParseInt32(iconResStr);

        AutoPtr<SearchResult> result = new SearchResult(packageContext, title,
                summaryOn, summaryOff, entries, iconResId, key);
        *item = (IObject*)result;
        REFCOUNT_ADD(*item);
        return NOERROR;
    }
    return NOERROR;
}


ECode SearchResultsSummary::SearchResultsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}


ECode SearchResultsSummary::SearchResultsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** _result)
{
    VALIDATE_NOT_NULL(_result);
    *_result = NULL;

    if (!mDataValid && convertView == NULL) {
        Logger::E("SearchResultsSummary", "this should only be called when the cursor is valid");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException(
        //         "this should only be called when the cursor is valid");
    }
    Boolean res;
    if (mCursor->MoveToPosition(position, &res), !res) {
        Logger::E("SearchResultsSummary", "couldn't move cursor to position %d", position);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("couldn't move cursor to position " + position);
    }

    AutoPtr<IView> view;
    AutoPtr<ITextView> textTitle;
    AutoPtr<IImageView> imageView;

    if (convertView == NULL) {
        mInflater->Inflate(R::layout::search_result_item, parent, FALSE, (IView**)&view);
    }
    else {
        view = convertView;
    }

    AutoPtr<IView> otherView;
    view->FindViewById(R::id::title, (IView**)&otherView);
    textTitle = ITextView::Probe(otherView);
    otherView = NULL;
    view->FindViewById(R::id::icon, (IView**)&otherView);
    imageView = IImageView::Probe(otherView);

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);

    AutoPtr<SearchResult> result = (SearchResult*)IObject::Probe(obj);
    textTitle->SetText(CoreUtils::Convert(result->mTitle));

    if (result->mIconResId != R::drawable::empty_icon) {
        AutoPtr<IContext> packageContext = result->mContext;
        AutoPtr<IDrawable> drawable;
        // try {
        ECode ec = packageContext->GetDrawable(result->mIconResId, (IDrawable**)&drawable);
        if (SUCCEEDED(ec)) {
            imageView->SetImageDrawable(drawable);
        }
        else {
            // Not much we can do except logging
            Logger::E(TAG, "Cannot load Drawable for %s", result->mTitle.string());
        }
        // } catch (Resources.NotFoundException nfe) {
        //     // Not much we can do except logging
        //     Logger::E(TAG, "Cannot load Drawable for " + result.title);
        // }
    }
    else {
        imageView->SetImageDrawable(NULL);
        IView::Probe(imageView)->SetBackgroundResource(R::drawable::empty_icon);
    }

    *_result = view;
    REFCOUNT_ADD(*_result);
    return NOERROR;
}

//===============================================================================
//                  SearchResultsSummary::AdapterViewOnItemClickListener
//===============================================================================

CAR_INTERFACE_IMPL(SearchResultsSummary::AdapterViewOnItemClickListener, Object, IAdapterViewOnItemClickListener);

SearchResultsSummary::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ SearchResultsSummary* host,
    /* [in] */ Int32 num)
    : mHost(host)
    , mNum(num)
{}

SearchResultsSummary::AdapterViewOnItemClickListener::~AdapterViewOnItemClickListener()
{}

ECode SearchResultsSummary::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    switch (mNum) {
        case 1: {
            // We have a header, so we need to decrement the position by one
            position--;

            // Some Monkeys could create a case where they were probably clicking on the
            // List Header and thus the position passed was "0" and then by decrement was "-1"
            if (position < 0) {
                return NOERROR;
            }

            AutoPtr<ICursor> cursor = mHost->mResultsAdapter->mCursor;
            Boolean res;
            cursor->MoveToPosition(position, &res);

            String className;
            cursor->GetString(Index::COLUMN_INDEX_CLASS_NAME, &className);
            String screenTitle;
            cursor->GetString(Index::COLUMN_INDEX_SCREEN_TITLE, &screenTitle);
            String action;
            cursor->GetString(Index::COLUMN_INDEX_INTENT_ACTION, &action);
            String key;
            cursor->GetString(Index::COLUMN_INDEX_KEY, &key);

            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            AutoPtr<CSettingsActivity> sa = (CSettingsActivity*)activity.Get();
            sa->NeedToRevertToInitialFragment();

            if (TextUtils::IsEmpty(action)) {
                AutoPtr<IBundle> args;
                CBundle::New((IBundle**)&args);
                args->PutString(CSettingsActivity::EXTRA_FRAGMENT_ARG_KEY, key);

                Utils::StartWithFragment(sa, className, args, NULL, 0, -1,
                        CoreUtils::Convert(screenTitle));
            }
            else {
                AutoPtr<IIntent> intent;
                CIntent::New(action, (IIntent**)&intent);

                String targetPackage;
                cursor->GetString(
                        Index::COLUMN_INDEX_INTENT_ACTION_TARGET_PACKAGE, &targetPackage);
                String targetClass;
                cursor->GetString(
                        Index::COLUMN_INDEX_INTENT_ACTION_TARGET_CLASS, &targetClass);
                if (!TextUtils::IsEmpty(targetPackage) && !TextUtils::IsEmpty(targetClass)) {
                    AutoPtr<IComponentName> component;
                    CComponentName::New(targetPackage, targetClass, (IComponentName**)&component);
                    intent->SetComponent(component);
                }
                intent->PutExtra(CSettingsActivity::EXTRA_FRAGMENT_ARG_KEY, key);

                sa->StartActivity(intent);
            }

            mHost->SaveQueryToDatabase();
            break;
        }
        case 2: {
            // We have a header, so we need to decrement the position by one
            position--;
            // Some Monkeys could create a case where they were probably clicking on the
            // List Header and thus the position passed was "0" and then by decrement was "-1"
            if (position < 0) {
                return NOERROR;
            }
            AutoPtr<ICursor> cursor = mHost->mSuggestionsAdapter->mCursor;
            Boolean res;
            cursor->MoveToPosition(position, &res);

            mHost->mShowResults = TRUE;
            cursor->GetString(0, &mHost->mQuery);
            mHost->mSearchView->SetQuery(CoreUtils::Convert(mHost->mQuery), FALSE);
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  SearchResultsSummary
//===============================================================================

CAR_INTERFACE_IMPL(SearchResultsSummary, Fragment, ISearchResultsSummary)

SearchResultsSummary::SearchResultsSummary()
    : mShowResults(FALSE)
{}

SearchResultsSummary::~SearchResultsSummary()
{}

ECode SearchResultsSummary::constructor()
{
    return NOERROR;
}

ECode SearchResultsSummary::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mResultsAdapter = new SearchResultsAdapter(IContext::Probe(activity));
    mSuggestionsAdapter = new SuggestionsAdapter(IContext::Probe(activity));

    if (savedInstanceState != NULL) {
        savedInstanceState->GetBoolean(SAVE_KEY_SHOW_RESULTS, &mShowResults);
    }
    return NOERROR;
}

ECode SearchResultsSummary::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);

    outState->PutBoolean(SAVE_KEY_SHOW_RESULTS, mShowResults);
    return NOERROR;
}

ECode SearchResultsSummary::OnStop()
{
    Fragment::OnStop();

    ClearSuggestions();
    ClearResults();
    return NOERROR;
}

ECode SearchResultsSummary::OnDestroy()
{
    mResultsListView = NULL;
    mResultsAdapter = NULL;
    mUpdateSearchResultsTask = NULL;

    mSuggestionsListView = NULL;
    mSuggestionsAdapter = NULL;
    mUpdateSuggestionsTask = NULL;

    mSearchView = NULL;

    Fragment::OnDestroy();
    return NOERROR;
}

ECode SearchResultsSummary::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::search_panel, container, FALSE, (IView**)&view);

    AutoPtr<IView> otherView;
    view->FindViewById(R::id::layout_suggestions, (IView**)&otherView);
    mLayoutSuggestions = IViewGroup::Probe(otherView);
    otherView = NULL;
    view->FindViewById(R::id::layout_results, (IView**)&otherView);
    mLayoutResults = IViewGroup::Probe(otherView);

    otherView = NULL;
    view->FindViewById(R::id::list_results, (IView**)&otherView);
    mResultsListView = IListView::Probe(otherView);
    IAdapterView::Probe(mResultsListView)->SetAdapter(mResultsAdapter);
    AutoPtr<AdapterViewOnItemClickListener> listener1 = new AdapterViewOnItemClickListener(this, 1);
    IAdapterView::Probe(mResultsListView)->SetOnItemClickListener(listener1);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> _inflater;
    LayoutInflater::From(IContext::Probe(activity), (ILayoutInflater**)&_inflater);
    otherView = NULL;
    _inflater->Inflate(
            R::layout::search_panel_results_header, IViewGroup::Probe(mResultsListView),
            FALSE, (IView**)&otherView);
    mResultsListView->AddHeaderView(otherView, NULL, FALSE);

    otherView = NULL;
    view->FindViewById(R::id::list_suggestions, (IView**)&otherView);
    mSuggestionsListView = IListView::Probe(otherView);
    IAdapterView::Probe(mSuggestionsListView)->SetAdapter(mSuggestionsAdapter);
    AutoPtr<AdapterViewOnItemClickListener> listener2 = new AdapterViewOnItemClickListener(this, 2);
    IAdapterView::Probe(mSuggestionsListView)->SetOnItemClickListener(listener2);
    otherView = NULL;
    _inflater->Inflate(
            R::layout::search_panel_suggestions_header, IViewGroup::Probe(mSuggestionsListView),
            FALSE, (IView**)&otherView);
    mSuggestionsListView->AddHeaderView(otherView, NULL, FALSE);

    *result = view;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SearchResultsSummary::OnResume()
{
    Fragment::OnResume();

    if (!mShowResults) {
        ShowSomeSuggestions();
    }
    return NOERROR;
}

ECode SearchResultsSummary::SetSearchView(
    /* [in] */ ISearchView* searchView)
{
    mSearchView = searchView;
    return NOERROR;
}

void SearchResultsSummary::SetSuggestionsVisibility(
    /* [in] */ Boolean visible)
{
    if (mLayoutSuggestions != NULL) {
        IView::Probe(mLayoutSuggestions)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    }
}

void SearchResultsSummary::SetResultsVisibility(
    /* [in] */ Boolean visible)
{
    if (mLayoutResults != NULL) {
        IView::Probe(mLayoutResults)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    }
}

void SearchResultsSummary::SaveQueryToDatabase()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Int64 data;
    Index::GetInstance(IContext::Probe(activity))->AddSavedQuery(mQuery, &data);
}

Boolean SearchResultsSummary::OnQueryTextSubmit(
    /* [in] */ const String& query)
{
    mQuery = GetFilteredQueryString(CoreUtils::Convert(query));
    mShowResults = TRUE;
    SetSuggestionsVisibility(FALSE);
    UpdateSearchResults();
    SaveQueryToDatabase();
    return TRUE;
}

Boolean SearchResultsSummary::OnQueryTextChange(
    /* [in] */ const String& query)
{
    String newQuery = GetFilteredQueryString(CoreUtils::Convert(query));

    mQuery = newQuery;

    if (TextUtils::IsEmpty(mQuery)) {
        mShowResults = FALSE;
        SetResultsVisibility(FALSE);
        UpdateSuggestions();
    }
    else {
        mShowResults = TRUE;
        SetSuggestionsVisibility(FALSE);
        UpdateSearchResults();
    }

    return TRUE;
}

ECode SearchResultsSummary::ShowSomeSuggestions()
{
    SetResultsVisibility(FALSE);
    mQuery = EMPTY_QUERY;
    UpdateSuggestions();
    return NOERROR;
}

void SearchResultsSummary::ClearSuggestions()
{
    if (mUpdateSuggestionsTask != NULL) {
        mUpdateSuggestionsTask->Cancel(FALSE);
        mUpdateSuggestionsTask = NULL;
    }
    SetSuggestionsCursor(NULL);
}

void SearchResultsSummary::SetSuggestionsCursor(
    /* [in] */ ICursor* cursor)
{
    if (mSuggestionsAdapter == NULL) {
        return;
    }
    AutoPtr<ICursor> oldCursor;
    mSuggestionsAdapter->SwapCursor(cursor, (ICursor**)&oldCursor);
    if (oldCursor != NULL) {
        ICloseable::Probe(oldCursor)->Close();
    }
}

void SearchResultsSummary::ClearResults()
{
    if (mUpdateSearchResultsTask != NULL) {
        mUpdateSearchResultsTask->Cancel(FALSE);
        mUpdateSearchResultsTask = NULL;
    }
    SetResultsCursor(NULL);
}

void SearchResultsSummary::SetResultsCursor(
    /* [in] */ ICursor* cursor)
{
    if (mResultsAdapter == NULL) {
        return;
    }
    AutoPtr<ICursor> oldCursor;
    mResultsAdapter->SwapCursor(cursor, (ICursor**)&oldCursor);
    if (oldCursor != NULL) {
        ICloseable::Probe(oldCursor)->Close();
    }
}

String SearchResultsSummary::GetFilteredQueryString(
    /* [in] */ ICharSequence* query)
{
    if (query == NULL) {
        return String(NULL);
    }
    AutoPtr<IStringBuilder> filtered = new StringBuilder();;
    Int32 len;
    query->GetLength(&len);
    for (Int32 n = 0; n < len; n++) {
        Char32 c;
        query->GetCharAt(n, &c);
        if (!Character::IsLetterOrDigit(c) && !Character::IsSpaceChar(c)) {
            continue;
        }
        filtered->AppendChar(c);
    }
    return Object::ToString(filtered);
}

void SearchResultsSummary::ClearAllTasks()
{
    if (mUpdateSearchResultsTask != NULL) {
        mUpdateSearchResultsTask->Cancel(FALSE);
        mUpdateSearchResultsTask = NULL;
    }
    if (mUpdateSuggestionsTask != NULL) {
        mUpdateSuggestionsTask->Cancel(FALSE);
        mUpdateSuggestionsTask = NULL;
    }
}

void SearchResultsSummary::UpdateSuggestions()
{
    ClearAllTasks();
    if (mQuery == NULL) {
        SetSuggestionsCursor(NULL);
    }
    else {
        mUpdateSuggestionsTask = new UpdateSuggestionsTask(this);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(mQuery));
        mUpdateSuggestionsTask->Execute(args);
    }
}

void SearchResultsSummary::UpdateSearchResults()
{
    ClearAllTasks();
    if (TextUtils::IsEmpty(mQuery)) {
        SetResultsVisibility(FALSE);
        SetResultsCursor(NULL);
    }
    else {
        mUpdateSearchResultsTask = new UpdateSearchResultsTask(this);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(mQuery));
        mUpdateSearchResultsTask->Execute(args);
    }
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
