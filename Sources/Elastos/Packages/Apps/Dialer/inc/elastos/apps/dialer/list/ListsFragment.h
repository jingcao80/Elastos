#ifndef __ELASTOS_APPS_DIALER_LIST_LISTSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_LISTSFRAGMENT_H__

using Elastos::Droid::App::IFragmentManager;

namespace Elastos{
namespace Apps{
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
    // TODO:android.support.v4.view.ViewPager.OnPageChangeListener
    // , public IViewPagerOnPageChangeListener
{
public:
    class ViewPagerAdapter
        // TODO:android.support.v13.app.FragmentPagerAdapter
        // : public FragmentPagerAdapter
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
        , public IOverlappingPaneLayoutPanelSlideCallbacks
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
    CAR_INTERFACE_DECL()

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

    // TODO:android.support.v4.view.ViewPager.OnPageChangeListener
    // CARAPI AddOnPageChangeListener(
    //     /* [in] */ IViewPagerOnPageChangeListener* onPageChangeListener);

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

    CARAPI_(void) SetupPaneLayout(
        /* [in] */ IOverlappingPaneLayout* paneLayout);

private:
    static const Boolean DEBUG; // = DialtactsActivity.DEBUG;
    static const String TAG; // = "ListsFragment";

    static const Int32 MAX_RECENTS_ENTRIES; // = 20;
    // Oldest recents entry to display is 2 weeks old.
    static const Int64 OLDEST_RECENTS_DATE; // = 1000L * 60 * 60 * 24 * 14;

    static const String KEY_LAST_DISMISSED_CALL_SHORTCUT_DATE; // =
            // "key_last_dismissed_call_shortcut_date";

    AutPtr<IActionBar> mActionBar;
    AutPtr<IViewPager> mViewPager;
    AutPtr<IViewPagerTabs> mViewPagerTabs;
    AutPtr<ViewPagerAdapter> mViewPagerAdapter;
    AutPtr<IListView> mShortcutCardsListView;
    AutPtr<IRemoveView> mRemoveView;
    AutPtr<IView> mRemoveViewContent;
    AutPtr<ISpeedDialFragment> mSpeedDialFragment;
    AutPtr<ICallLogFragment> mRecentsFragment;
    AutPtr<IAllContactsFragment> mAllContactsFragment;
    AutPtr<IArrayList> mOnPageChangeListeners;

    AutPtr<ArrayOf<String> > mTabTitles;

    AutPtr<IShortcutCardsAdapter> mMergedAdapter;
    AutPtr<ICallLogAdapter> mCallLogAdapter;
    AutPtr<ICallLogQueryHandler> mCallLogQueryHandler;

    Boolean mIsPanelOpen; // = true;

    /**
     * Call shortcuts older than this date (persisted in shared preferences) will not show up in
     * at the top of the screen
     */
    Int64 mLastCallShortcutDate; // = 0;

    /**
     * The date of the current call shortcut that is showing on screen.
     */
    Int64 mCurrentCallShortcutDate; // = 0;

    AutPtr<PanelSlideCallbacks> mPanelSlideCallbacks;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_LISTSFRAGMENT_H__
