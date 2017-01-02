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

#ifndef __ELASTOS_DROID_DIALER_LIST_SPEEDDIALFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_SPEEDDIALFRAGMENT_H__

#include "elastos/droid/dialer/list/PhoneFavoritesTileAdapter.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsFragment.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Contacts::Common::List::IContactTileViewListener;
using Elastos::Droid::Contacts::Common::List::IOnPhoneNumberPickerActionListener;
using Elastos::Droid::Content::ICursorLoader;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::DialerBind::Analytics::AnalyticsFragment;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAbsListViewOnScrollListener;
using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * This fragment displays the user's favorite/frequent contacts in a grid.
 */
class SpeedDialFragment
    : public AnalyticsFragment
    , public ISpeedDialFragment
{
private:
    class InnerOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerOnItemClickListener(
            /* [in] */ SpeedDialFragment* host)
            : mHost(host)
        {}

        CARAPI OnItemClick(
                /* [in] */ IAdapterView* parent,
                /* [in] */ IView* view,
                /* [in] */ Int32 position,
                /* [in] */ Int64 id);
    private:
        SpeedDialFragment* mHost;
    };

    class InnerOnDataSetChangedListener
        : public Object
        , public IOnDataSetChangedForAnimationListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnDataSetChangedListener(
            /* [in] */ SpeedDialFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnDataSetChangedForAnimation(
            /* [in] */ ArrayOf<Int64>* idsInPlace);

        // @Override
        CARAPI CacheOffsetsForDatasetChange();

    private:
        SpeedDialFragment* mHost;
    };

    class ContactTileLoaderListener
        : public Object
        , public ILoaderManagerLoaderCallbacks
    {
    public:
        ContactTileLoaderListener(
            /* [in] */ SpeedDialFragment* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnCreateLoader(
            /* [in] */ Int32 id,
            /* [in] */ IBundle* args,
            /* [out] */ ILoader** loader);

        // @Override
        CARAPI OnLoadFinished(
            /* [in] */ ILoader* loader,
            /* [in] */ IInterface* data);

        // @Override
        CARAPI OnLoaderReset(
            /* [in] */ ILoader* loader);

    private:
        SpeedDialFragment* mHost;
    };

    class ContactTileAdapterListener
        : public Object
        , public IContactTileViewListener
    {
    public:
        CAR_INTERFACE_DECL()

        ContactTileAdapterListener(
            /* [in] */ SpeedDialFragment* host)
            : mHost(host)
        {}

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
        , public IAbsListViewOnScrollListener
    {
    public:
        CAR_INTERFACE_DECL()

        ScrollListener(
            /* [in] */ SpeedDialFragment* host)
            : mHost(host)
        {}

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
            /* [in] */ ArrayOf<Int64>* idsInPlace)
            : mHost(host)
            , mObserver(observer)
            , mIdsInPlace(idsInPlace)
        {}

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

    CARAPI constructor();

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

    CARAPI_(void) SetEmptyViewVisibility(
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
    virtual CARAPI OnItemClick(
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
    static const Int64 KEY_REMOVED_ITEM_HEIGHT;

    static const String TAG;
    static const Boolean DEBUG = FALSE;

    /**
     * Used with LoaderManager.
     */
    static const Int32 LOADER_ID_CONTACT_TILE = 1;

    Int32 mAnimationDuration;

    AutoPtr<IOnPhoneNumberPickerActionListener> mPhoneNumberPickerActionListener;

    AutoPtr<IOnListFragmentScrolledListener> mActivityScrollListener;
    AutoPtr<PhoneFavoritesTileAdapter> mContactTileAdapter;

    AutoPtr<IView> mParentView;

    AutoPtr<IPhoneFavoriteListView> mListView;

    AutoPtr<IView> mContactTileFrame;

    AutoPtr<ITileInteractionTeaserView> mTileInteractionTeaserView;

    HashMap<Int64, AutoPtr<IInteger32> > mItemIdTopMap;
    HashMap<Int64, AutoPtr<IInteger32> > mItemIdLeftMap;

    /**
     * Layout used when there are no favorites.
     */
    AutoPtr<IView> mEmptyView;

    AutoPtr<IContactTileViewListener> mContactTileAdapterListener;
    AutoPtr<ILoaderManagerLoaderCallbacks> mContactTileLoaderListener;
    AutoPtr<ScrollListener> mScrollListener;

    friend class ContactTileLoaderListener;
    friend class ContactTileAdapterListener;
    friend class ScrollListener;
    friend class MyViewTreeObserverOnPreDrawListener;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SPEEDDIALFRAGMENT_H__
