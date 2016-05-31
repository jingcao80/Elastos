#ifndef __ELASTOS_APPS_DIALER_LIST_SPEEDDIALFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_SPEEDDIALFRAGMENT_H__

#include "SearchFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

/**
 * This fragment displays the user's favorite/frequent contacts in a grid.
 */
class SpeedDialFragment
    : public AnalyticsFragment
    , public ISpeedDialFragment
    , public IAdapterViewOnItemClickListener
    , public IOnDataSetChangedForAnimationListener
{
private:
    class ContactTileLoaderListener
        : public Object
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        ContactTileLoaderListener(
            /* [in] */ SpeedDialFragment* host);

        // @Override
        CARAPI OnCreateLoader(
            /* [in] */ Int32 id,
            /* [in] */ IBundle* args,
            /* [out] */ ICursorLoader** loader);

        // @Override
        CARAPI OnLoadFinished(
            /* [in] */ ILoader* loader,
            /* [in] */ ICursor* data);

        // @Override
        CARAPI OnLoaderReset(
            /* [in] */ ILoader* loader);

    private:
        SpeedDialFragment* mHost;
    };

    class ContactTileAdapterListener
        : public Object
        : public IContactTileViewListener
    {
    public:
        CAR_INTERFACE_DECL()

        ContactTileAdapterListener(
            /* [in] */ SpeedDialFragment* host);

        // @Override
        CARAPI OnContactSelected(
            /* [in] */ IUri* contactUri,
            /* [in] */ IRect* targetRect);

        // @Override
        CARAPI OnCallNumberDirectly(
            /* [in] */ const String& phoneNumber);

        // @Override
        CARAPI GetApproximateTileWidth(
            /* [out] */ Int32* width);

    private:
        SpeedDialFragment* mHost;
    };

    class ScrollListener
        : public Object
        : public IListViewOnScrollListener
    {
    public:
        CAR_INTERFACE_DECL()

        ScrollListener(
            /* [in] */ SpeedDialFragment* host);

        // @Override
        CARAPI OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

        // @Override
        CARAPI OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

    private:
        SpeedDialFragment* mHost;
    };

    class MyViewTreeObserverOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewTreeObserverOnPreDrawListener(
            /* [in] */ SpeedDialFragment* host,
            /* [in] */ IViewTreeObserver* observer,
            /* [in] */ ArrayOf<Int64>* idsInPlace);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        SpeedDialFragment* mHost;
        AutoPtr<IViewTreeObserver> mObserver;
        AutoPtr<ArrayOf<Int64> > mIdsInPlace;
    };

public:
    CAR_INTERFACE_DECL()

    SpeedDialFragment();

    // @Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedState);

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    CARAPI HasFrequents(
        /* [out] */ Boolean* result);

    /* package */ CARAPI_(void) SetEmptyViewVisibility(
        /* [in] */ Boolean visible);

    // @Override
    CARAPI OnStart();

    /**
     * {@inheritDoc}
     *
     * This is only effective for elements provided by {@link #mContactTileAdapter}.
     * {@link #mContactTileAdapter} has its own logic for click events.
     */
    // @Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // @Override
    CARAPI OnDataSetChangedForAnimation(
        /* [in] */ ArrayOf<Int64>* idsInPlace);

    // @Override
    CARAPI CacheOffsetsForDatasetChange();

    CARAPI GetListView(
        /* [out] */ IAbsListView** listView);

private:
    /**
     * Cache the current view offsets into memory. Once a relayout of views in the ListView
     * has happened due to a dataset change, the cached offsets are used to create animations
     * that slide views from their previous positions to their new ones, to give the appearance
     * that the views are sliding into their new positions.
     */
    CARAPI_(void) SaveOffsets(
        /* [in] */ Int32 removedItemHeight);

    /*
     * Performs animations for the gridView
     */
    CARAPI_(void) AnimateGridView(
        /* [in] */ ArrayOf<Int64>* idsInPlace);

    CARAPI_(Boolean) ContainsId(
        /* [in] */ ArrayOf<Int64>* ids,
        /* [in] */ Int64 target);

private:
    /**
     * By default, the animation code assumes that all items in a list view are of the same height
     * when animating new list items into view (e.g. from the bottom of the screen into view).
     * This can cause incorrect translation offsets when a item that is larger or smaller than
     * other list item is removed from the list. This key is used to provide the actual height
     * of the removed object so that the actual translation appears correct to the user.
     */
    static const Int64 KEY_REMOVED_ITEM_HEIGHT; // = Long.MAX_VALUE;

    static const String TAG; // = SpeedDialFragment.class.getSimpleName();
    static const Boolean DEBUG; // = false;

    Int32 mAnimationDuration;

    /**
     * Used with LoaderManager.
     */
    static Int32 LOADER_ID_CONTACT_TILE; // = 1;

    AutoPtr<IOnPhoneNumberPickerActionListener> mPhoneNumberPickerActionListener;

    AutoPtr<IOnListFragmentScrolledListener> mActivityScrollListener;
    AutoPtr<IPhoneFavoritesTileAdapter> mContactTileAdapter;

    AutoPtr<IView> mParentView;

    AutoPtr<IPhoneFavoriteListView> mListView;

    AutoPtr<IView> mContactTileFrame;

    AutoPtr<ITileInteractionTeaserView> mTileInteractionTeaserView;

    AutoPtr<IHashMap> mItemIdTopMap; // = new HashMap<Long, Integer>();
    AutoPtr<IHashMap> mItemIdLeftMap; // = new HashMap<Long, Integer>();

    /**
     * Layout used when there are no favorites.
     */
    AutoPtr<IView> mEmptyView;

    AutoPtr<IContactTileViewListener> mContactTileAdapterListener;
    AutoPtr<ILoaderManagerLoaderCallbacks> mContactTileLoaderListener;
    AutoPtr<ScrollListener> mScrollListener;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_SPEEDDIALFRAGMENT_H__
