#ifndef __ELASTOS_DROID_DIALER_LIST_LISTSFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_LISTSFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/dialerbind/analytics/AnalyticsFragment.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Support::V4::View::IViewPagerOnPageChangeListener;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
// using Elastos::Droid::Dialer::CallLog::ICallLogAdapter;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterCallFetcher;
// using Elastos::Droid::Dialer::CallLog::ICallLogFragment;
// using Elastos::Droid::Dialer::CallLog::ICallLogQueryHandler;
using Elastos::Droid::Dialer::CallLog::ICallLogQueryHandlerListener;
// using Elastos::Droid::Dialer::Widget::IOverlappingPaneLayout;
using Elastos::Droid::Dialer::Widget::IPanelSlideCallbacks;
using Elastos::Droid::DialerBind::Analytics::AnalyticsFragment;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Fragment that is used as the main screen of the Dialer.
 *
 * Contains a ViewPager that contains various contact lists like the Speed Dial list and the
 * All Contacts list. This will also eventually contain the logic that allows sliding the
 * ViewPager containing the lists up above the shortcut cards and pin it against the top of the
 * screen.
 */
class ListsFragment
    : public AnalyticsFragment
    , public IListsFragment
    , public ICallLogQueryHandlerListener
    , public ICallLogAdapterCallFetcher
    , public IViewPagerOnPageChangeListener
{
public:
    class ViewPagerAdapter
        // TODO:android.support.v13.app.FragmentPagerAdapter
        // : public FragmentPagerAdapter
        : public Object
    {
    public:
        ViewPagerAdapter(
            /* [in] */ ListsFragment* host);

        CARAPI constructor(
            /* [in] */ IFragmentManager* fm);

        // @Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        // @Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IFragment** item);

        // @Override
        CARAPI InstantiateItem(
            /* [in] */ IViewGroup* container,
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        // @Override
        CARAPI GetCount(
            /* [out] */ Int32* count);

        // @Override
        CARAPI GetPageTitle(
            /* [in] */ Int32 position,
            /* [out] */ ICharSequence** title);

    private:
        ListsFragment* mHost;
    };

private:
    class PanelSlideCallbacks
        : public Object
        , public IPanelSlideCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        PanelSlideCallbacks(
            /* [in] */ ListsFragment* host);

        // @Override
        CARAPI OnPanelSlide(
            /* [in] */ IView* panel,
            /* [in] */ Float slideOffset);

        // @Override
        CARAPI OnPanelOpened(
            /* [in] */ IView* panel);

        // @Override
        CARAPI OnPanelClosed(
            /* [in] */ IView* panel);

        // @Override
        CARAPI OnPanelFlingReachesEdge(
            /* [in] */ Int32 velocityY);

        // @Override
        CARAPI IsScrollableChildUnscrolled(
            /* [out] */ Boolean* result);

    private:
        ListsFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ListsFragment();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnVoicemailStatusFetched(
        /* [in] */ ICursor* statusCursor);

    // @Override
    CARAPI OnCallsFetched(
        /* [in] */ ICursor* cursor,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI FetchCalls();

    CARAPI DismissShortcut(
        /* [in] */ IView* view);

    CARAPI AddOnPageChangeListener(
        /* [in] */ IViewPagerOnPageChangeListener* onPageChangeListener);

    // @Override
    CARAPI OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Int32 positionOffsetPixels);

    // @Override
    CARAPI OnPageSelected(
        /* [in] */ Int32 position);

    // @Override
    CARAPI OnPageScrollStateChanged(
        /* [in] */ Int32 state);

    CARAPI ShowRemoveView(
        /* [in] */ Boolean show);

    CARAPI ShouldShowActionBar(
        /* [out] */ Boolean* result);

    CARAPI IsPaneOpen(
        /* [out] */ Boolean* result);

    CARAPI GetSpeedDialFragment(
        /* [out] */ ISpeedDialFragment** fragment);

    CARAPI GetRemoveView(
        /* [out] */ IRemoveView** removeView);

    CARAPI GetRtlPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* result);

private:
    CARAPI GetCurrentListView(
        /* [out] */ IAbsListView** listView);

    // CARAPI_(void) SetupPaneLayout(
    //     /* [in] */ IOverlappingPaneLayout* paneLayout);

private:
    static const Boolean DEBUG;
    static const String TAG;

    static const Int32 MAX_RECENTS_ENTRIES;
    // Oldest recents entry to display is 2 weeks old.
    static const Int64 OLDEST_RECENTS_DATE;

    static const String KEY_LAST_DISMISSED_CALL_SHORTCUT_DATE;

    AutoPtr<IActionBar> mActionBar;
    // TODO:
    // AutoPtr<IViewPager> mViewPager;
    // AutoPtr<IViewPagerTabs> mViewPagerTabs;
    AutoPtr<ViewPagerAdapter> mViewPagerAdapter;
    AutoPtr<IListView> mShortcutCardsListView;
    // AutoPtr<IRemoveView> mRemoveView;
    AutoPtr<IView> mRemoveViewContent;
    // AutoPtr<ISpeedDialFragment> mSpeedDialFragment;
    // AutoPtr<ICallLogFragment> mRecentsFragment;
    // AutoPtr<IAllContactsFragment> mAllContactsFragment;
    AutoPtr<IArrayList> mOnPageChangeListeners;

    AutoPtr<ArrayOf<String> > mTabTitles;

    // AutoPtr<IShortcutCardsAdapter> mMergedAdapter;
    // AutoPtr<ICallLogAdapter> mCallLogAdapter;
    // AutoPtr<ICallLogQueryHandler> mCallLogQueryHandler;

    Boolean mIsPanelOpen;

    /**
     * Call shortcuts older than this date (persisted in shared preferences) will not show up in
     * at the top of the screen
     */
    Int64 mLastCallShortcutDate;

    /**
     * The date of the current call shortcut that is showing on screen.
     */
    Int64 mCurrentCallShortcutDate;

    AutoPtr<IPanelSlideCallbacks> mPanelSlideCallbacks;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_LISTSFRAGMENT_H__
