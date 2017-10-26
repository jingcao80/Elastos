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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTFRAGMENT_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTFRAGMENT_H__

#include "elastos/droid/contacts/common/preference/ContactsPreferences.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsFragment.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Contacts::Common::Preference::ContactsPreferences;
using Elastos::Droid::Contacts::Common::Preference::IContactsPreferencesChangeListener;
using Elastos::Droid::Content::ICursorLoader;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::DialerBind::Analytics::AnalyticsFragment;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ILayoutInflater;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Common base class for various contact-related list fragments.
 */
class ContactEntryListFragment
    : public AnalyticsFragment
    , public IContactEntryListFragment
    , public ILoaderManagerLoaderCallbacks
{
private:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IAbsListViewOnScrollListener
        , public IViewOnFocusChangeListener
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ ContactEntryListFragment* host);

        // @Override
        virtual CARAPI OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

        // @Override
        virtual CARAPI OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

        // @Override
        virtual CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        // @Override
        virtual CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);

        /**
         * Dismisses the soft keyboard when the list is touched.
         */
        // @Override
        virtual CARAPI OnTouch(
            /* [in] */ IView* view,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        ContactEntryListFragment* mHost;
    };

    class SIMStateReceiver : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ContactEntryListFragment::SIMStateReceiver")

        SIMStateReceiver(
            /* [in] */ ContactEntryListFragment* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ContactEntryListFragment* mHost;
    };

    class DelayedDirectorySearchHandler : public Handler
    {
    public:
        DelayedDirectorySearchHandler(
            /* [in] */ ContactEntryListFragment* host);

        TO_STRING_IMPL("ContactEntryListFragment::DelayedDirectorySearchHandler")

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ContactEntryListFragment* mHost;
    };

    class PreferencesChangeListener
        : public Object
        , public IContactsPreferencesChangeListener
    {
    public:
        PreferencesChangeListener(
            /* [in] */ ContactEntryListFragment* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnChange();

    private:
        ContactEntryListFragment* mHost;
    };

public:
    ContactEntryListFragment();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    /**
     * Sets a context for the fragment in the unit test environment.
     */
    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    using AnalyticsFragment::SetLoaderManager;

    /**
     * Overrides a loader manager for use in unit tests.
     */
    CARAPI SetLoaderManager(
        /* [in] */ ILoaderManager* loaderManager);

    CARAPI GetAdapter(
        /* [out] */ IInterface** adapter);

    CARAPI GetView(
        /* [out] */ IView** view);

    CARAPI GetListView(
        /* [out] */ IListView** listView);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI RestoreSavedState(
        /* [in] */ IBundle* savedState);

    // @Override
    CARAPI OnStart();

    // @Override
    virtual CARAPI OnCreateLoader(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ ILoader** loader);

    virtual CARAPI CreateCursorLoader(
        /* [in] */ IContext* context,
        /* [out] */ ICursorLoader** loader);

    // @Override
    virtual CARAPI OnLoadFinished(
        /* [in] */ ILoader* loader,
        /* [in] */ IInterface* data);

   virtual CARAPI OnLoaderReset(
        /* [in] */ ILoader* loader);

    CARAPI IsLoading(
        /* [out] */ Boolean* result);

    CARAPI IsLoadingDirectoryList(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnDestroy();

    CARAPI SetSectionHeaderDisplayEnabled(
        /* [in] */ Boolean flag);

    CARAPI IsSectionHeaderDisplayEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetVisibleScrollbarEnabled(
        /* [in] */ Boolean flag);

    CARAPI IsVisibleScrollbarEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetVerticalScrollbarPosition(
        /* [in] */ Int32 position);

    CARAPI SetPhotoLoaderEnabled(
        /* [in] */ Boolean flag);

    CARAPI IsPhotoLoaderEnabled(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the list is supposed to visually highlight the selected item.
     */
    CARAPI IsSelectionVisible(
        /* [out] */ Boolean* result);

    CARAPI SetSelectionVisible(
        /* [in] */ Boolean flag);

    CARAPI SetQuickContactEnabled(
        /* [in] */ Boolean flag);

    CARAPI SetAdjustSelectionBoundsEnabled(
        /* [in] */ Boolean flag);

    CARAPI SetQuickCallButtonEnabled(
        /* [in] */ Boolean flag);

    CARAPI SetIncludeProfile(
        /* [in] */ Boolean flag);

    CARAPI IsSearchMode(
        /* [out] */ Boolean* result);

    CARAPI GetQueryString(
        /* [out] */ String* str);

    CARAPI SetQueryString(
        /* [in] */ const String& queryString,
        /* [in] */ Boolean delaySelection);

    CARAPI SetShowEmptyListForNullQuery(
        /* [in] */ Boolean show);

    CARAPI GetDirectoryLoaderId(
        /* [out] */ Int32* id);

    CARAPI GetDirectorySearchMode(
        /* [out] */ Int32* mode);

    CARAPI SetDirectorySearchMode(
        /* [in] */ Int32 mode);

    CARAPI IsLegacyCompatibilityMode(
        /* [out] */ Boolean* result);

    CARAPI SetLegacyCompatibilityMode(
        /* [in] */ Boolean flag);

    CARAPI GetSortOrder(
        /* [out] */ Int32* sortOrder);

    CARAPI SetSortOrder(
        /* [in] */ Int32 sortOrder);

    CARAPI SetDirectoryResultLimit(
        /* [in] */ Int32 limit);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    virtual CARAPI OnHiddenChanged(
        /* [in] */ Boolean hidden);

    // @Override
    virtual CARAPI OnScroll(
        /* [in] */ IAbsListView* view,
        /* [in] */ Int32 firstVisibleItem,
        /* [in] */ Int32 visibleItemCount,
        /* [in] */ Int32 totalItemCount);

    // @Override
    virtual CARAPI OnScrollStateChanged(
        /* [in] */ IAbsListView* view,
        /* [in] */ Int32 scrollState);

    // @Override
    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // @Override
    virtual CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);

    /**
     * Dismisses the soft keyboard when the list is touched.
     */
    // @Override
    virtual CARAPI OnTouch(
        /* [in] */ IView* view,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPause();

    CARAPI SetDarkTheme(
        /* [in] */ Boolean value);

    /**
     * Processes a result returned by the contact picker.
     */
    CARAPI OnPickerResult(
        /* [in] */ IIntent* data);

protected:
    virtual CARAPI_(AutoPtr<IView>) InflateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container) = 0;

    virtual CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter() = 0;

    /**
     * @param position Please note that the position is already adjusted for
     *            header views, so "0" means the first list item below header
     *            views.
     */
    virtual CARAPI_(void) OnItemClick(
        /* [in] */ Int32 position,
        /* [in] */ Int64 id) = 0;

    CARAPI_(void) StartLoading();

    /**
     * Loads the directory partition.
     */
    CARAPI_(void) LoadDirectoryPartition(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ IDirectoryPartition* partition);

    CARAPI_(void) OnPartitionLoaded(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* data);

    CARAPI_(void) ReloadData();

    /**
     * Shows a view at the top of the list with a pseudo local profile prompting the user to add
     * a local profile. Default implementation does nothing.
     */
    CARAPI_(void) SetProfileHeader();

    /**
     * Provides logic that dismisses this fragment. The default implementation
     * does nothing.
     */
    CARAPI_(void) Finish();

    /**
     * Enter/exit search mode. This is method is tightly related to the current query, and should
     * only be called by {@link #setQueryString}.
     *
     * Also note this method doesn't call {@link #reloadData()}; {@link #setQueryString} does it.
     */
    CARAPI_(void) SetSearchMode(
        /* [in] */ Boolean flag);

    CARAPI_(Int32) GetContactNameDisplayOrder();

    CARAPI_(void) SetContactNameDisplayOrder(
        /* [in] */ Int32 displayOrder);

    CARAPI_(Boolean) LoadPreferences();

    virtual CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    CARAPI_(void) ConfigurePhotoLoader();

    CARAPI_(void) ConfigureAdapter();

    /**
     * Restore the list state after the adapter is populated.
     */
    CARAPI_(void) CompleteRestoreInstanceState();

private:
    CARAPI_(void) StartLoadingDirectoryPartition(
        /* [in] */ Int32 partitionIndex);

    /**
     * Queues up a delayed request to search the specified directory. Since
     * directory search will likely introduce a lot of network traffic, we want
     * to wait for a pause in the user's typing before sending a directory request.
     */
    CARAPI_(void) LoadDirectoryPartitionDelayed(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ IDirectoryPartition* partition);

    /**
     * Cancels all queued directory loading requests.
     */
    CARAPI_(void) RemovePendingDirectorySearchRequests();

    CARAPI_(void) ConfigureVerticalScrollbar();

    CARAPI_(void) HideSoftKeyboard();

    CARAPI_(Int32) GetDefaultVerticalScrollbarPosition();

protected:
    Boolean mUserProfileExists;

private:
    static const String KEY_LIST_STATE;
    static const String KEY_SECTION_HEADER_DISPLAY_ENABLED;
    static const String KEY_PHOTO_LOADER_ENABLED;
    static const String KEY_QUICK_CONTACT_ENABLED;
    static const String KEY_ADJUST_SELECTION_BOUNDS_ENABLED;
    static const String KEY_QUICK_CALL_BUTTON_ENABLED;
    static const String KEY_INCLUDE_PROFILE;
    static const String KEY_SEARCH_MODE;
    static const String KEY_VISIBLE_SCROLLBAR_ENABLED;
    static const String KEY_SCROLLBAR_POSITION;
    static const String KEY_QUERY_STRING;
    static const String KEY_DIRECTORY_SEARCH_MODE;
    static const String KEY_SELECTION_VISIBLE;
    static const String KEY_REQUEST;
    static const String KEY_DARK_THEME;
    static const String KEY_LEGACY_COMPATIBILITY;
    static const String KEY_DIRECTORY_RESULT_LIMIT;

    static const String DIRECTORY_ID_ARG_KEY;

    static const Int32 DIRECTORY_LOADER_ID = -1;

    static const Int32 DIRECTORY_SEARCH_DELAY_MILLIS = 300;
    static const Int32 DIRECTORY_SEARCH_MESSAGE = 1;

    static const Int32 DEFAULT_DIRECTORY_RESULT_LIMIT = 20;

    static const Int32 STATUS_NOT_LOADED = 0;
    static const Int32 STATUS_LOADING = 1;
    static const Int32 STATUS_LOADED = 2;

    Boolean mSectionHeaderDisplayEnabled;
    Boolean mPhotoLoaderEnabled;
    Boolean mQuickContactEnabled;
    Boolean mAdjustSelectionBoundsEnabled;
    Boolean mQuickCallButtonEnabled;
    Boolean mIncludeProfile;
    Boolean mSearchMode;
    Boolean mVisibleScrollbarEnabled;
    Boolean mShowEmptyListForEmptyQuery;
    Int32 mVerticalScrollbarPosition;
    String mQueryString;
    Int32 mDirectorySearchMode;
    Boolean mSelectionVisible;
    Boolean mLegacyCompatibility;

    Boolean mEnabled;

    AutoPtr<IContactEntryListAdapter> mAdapter;
    AutoPtr<IView> mView;
    AutoPtr<IListView> mListView;

    /**
     * Used for keeping track of the scroll state of the list.
     */
    AutoPtr<IParcelable> mListState;

    Int32 mDisplayOrder;
    Int32 mSortOrder;
    Int32 mDirectoryResultLimit;

    AutoPtr<IContactPhotoManager> mPhotoManager;
    AutoPtr<ContactsPreferences> mContactsPrefs;

    Boolean mForceLoad;

    Boolean mDarkTheme;

    Int32 mDirectoryListStatus;

    /**
     * Indicates whether we are doing the initial complete load of data (false) or
     * a refresh caused by a change notification (true)
     */
    Boolean mLoadPriorityDirectoriesOnly;

    AutoPtr<IContext> mContext;

    AutoPtr<ILoaderManager> mLoaderManager;

    AutoPtr<IBroadcastReceiver> mSIMStateReceiver;

    AutoPtr<IHandler> mDelayedDirectorySearchHandler;
    Int32 mDefaultVerticalScrollbarPosition;

    AutoPtr<IContactsPreferencesChangeListener> mPreferencesChangeListener;

    friend class SIMStateReceiver;
    friend class DelayedDirectorySearchHandler;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTFRAGMENT_H__
