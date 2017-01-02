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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__

#include "_Elastos_Apps_Contacts_Common_List_CPinnedHeaderListView.h"
#include "CAutoScrollListView.h"

using Elastos::Droid::Widget::IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A ListView that maintains a header pinned at the top of the list. The
 * pinned header can be pushed up and dissolved as needed.
 */
CarClass(CPinnedHeaderListView)
    , public CAutoScrollListView
    , public IPinnedHeaderListView
    , public IAbsListViewOnScrollListener
    , public IAdapterViewOnItemSelectedListener
{
private:
    class PinnedHeader
        : public Object
    {
    public:
        AutoPtr<IView> mView;
        Boolean mVisible;
        Int32 mY;
        Int32 mHeight;
        Int32 mAlpha;
        Int32 mState;

        Boolean mAnimating;
        Boolean mTargetVisible;
        Int32 mSourceY;
        Int32 mTargetY;
        Int64 mTargetTime;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPinnedHeaderListView();

    virtual ~CPinnedHeaderListView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    //@Override
    CARAPI SetOnScrollListener(
        /* [in] */ IAbsListViewOnScrollListener* onScrollListener);

    //@Override
    CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* listener);

    CARAPI SetScrollToSectionOnHeaderTouch(
        /* [in] */ Boolean value);

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

    /**
     * Ensures that the selected item is positioned below the top-pinned headers
     * and above the bottom-pinned ones.
     */
    // @Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // @Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    CARAPI GetPinnedHeaderHeight(
        /* [in] */ Int32 viewIndex,
        /* [out] */ Int32* height);

    /**
     * Set header to be pinned at the top.
     *
     * @param viewIndex index of the header view
     * @param y is position of the header in pixels.
     * @param animate true if the transition to the new coordinate should be animated
     */
    CARAPI SetHeaderPinnedAtTop(
        /* [in] */ Int32 viewIndex,
        /* [in] */ Int32 y,
        /* [in] */ Boolean animate);

    /**
     * Set header to be pinned at the bottom.
     *
     * @param viewIndex index of the header view
     * @param y is position of the header in pixels.
     * @param animate true if the transition to the new coordinate should be animated
     */
    CARAPI SetHeaderPinnedAtBottom(
        /* [in] */ Int32 viewIndex,
        /* [in] */ Int32 y,
        /* [in] */ Boolean animate);

    /**
     * Set header to be pinned at the top of the first visible item.
     *
     * @param viewIndex index of the header view
     * @param position is position of the header in pixels.
     */
     CARAPI SetFadingHeader(
        /* [in] */ Int32 viewIndex,
        /* [in] */ Int32 position,
        /* [in] */ Boolean fade);

    /**
     * Makes header invisible.
     *
     * @param viewIndex index of the header view
     * @param animate true if the transition to the new coordinate should be animated
     */
    CARAPI SetHeaderInvisible(
        /* [in] */ Int32 viewIndex,
        /* [in] */ Boolean animate);

    /**
     * Returns the sum of heights of headers pinned to the top.
     */
    CARAPI GetTotalTopPinnedHeaderHeight(
        /* [out] */ Int32* height);

    /**
     * Returns the list item position at the specified y coordinate.
     */
    CARAPI GetPositionAt(
        /* [in] */ Int32 y,
        /* [out] */ Int32* result);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);
protected:
    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI_(Float) GetTopFadingEdgeStrength();

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) EnsurePinnedHeaderLayout(
        /* [in] */ Int32 viewIndex);

    CARAPI_(Boolean) SmoothScrollToPartition(
        /* [in] */ Int32 partition);

    CARAPI_(void) InvalidateIfAnimating();

    CARAPI_(void) DrawHeader(
        /* [in] */ ICanvas* canvas,
        /* [in] */ PinnedHeader* header,
        /* [in] */ Int64 currentTime);

private:
    static const Int32 MAX_ALPHA; // = 255;
    static const Int32 TOP; // = 0;
    static const Int32 BOTTOM; // = 1;
    static const Int32 FADING; // = 2;

    static const Int32 DEFAULT_ANIMATION_DURATION; // = 20;

    static const Int32 DEFAULT_SMOOTH_SCROLL_DURATION; // = 100;

    PinnedHeaderAdapter mAdapter;
    Int32 mSize;
    AutoPtr<ArrayOf<PinnedHeader*> > mHeaders;
    AutoPtr<IRectF> mBounds; // = new RectF();
    AutoPtr<IAbsListViewOnScrollListener> mOnScrollListener;
    AutoPtr<IAdapterViewOnItemSelectedListener> mOnItemSelectedListener;
    Int32 mScrollState;

    Boolean mScrollToSectionOnHeaderTouch; // = false;
    Boolean mHeaderTouched; // = false;

    Int32 mAnimationDuration; // = DEFAULT_ANIMATION_DURATION;
    Boolean mAnimating;
    Int64 mAnimationTargetTime;
    Int32 mHeaderPaddingStart;
    Int32 mHeaderWidth;
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__
