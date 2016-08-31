#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__

#include "_Elastos_Droid_Contacts_Common_List_CPinnedHeaderListView.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/contacts/common/list/AutoScrollListView.h"

using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A ListView that maintains a header pinned at the top of the list. The
 * pinned header can be pushed up and dissolved as needed.
 */
CarClass(CPinnedHeaderListView)
    , public AutoScrollListView
    , public IPinnedHeaderListView
    , public IAbsListViewOnScrollListener
    , public IAdapterViewOnItemSelectedListener
{
private:
    class PinnedHeader : public Object
    {
    public:
        PinnedHeader()
            : mY(0)
            , mHeight(0)
            , mAlpha(0)
            , mState(0)
            , mAnimating(FALSE)
            , mTargetVisible(FALSE)
            , mSourceY(0)
            , mTargetY(0)
            , mTargetTime(0)
        {}

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
    CPinnedHeaderListView();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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
    static const Int32 MAX_ALPHA = 255;
    static const Int32 TOP = 0;
    static const Int32 BOTTOM = 1;
    static const Int32 FADING = 2;

    static const Int32 DEFAULT_ANIMATION_DURATION = 20;

    static const Int32 DEFAULT_SMOOTH_SCROLL_DURATION = 100;

    AutoPtr<IPinnedHeaderAdapter> mAdapter;
    Int32 mSize;
    AutoPtr<ArrayOf<PinnedHeader*> > mHeaders;
    AutoPtr<IRectF> mBounds;
    AutoPtr<IAbsListViewOnScrollListener> mOnScrollListener;
    AutoPtr<IAdapterViewOnItemSelectedListener> mOnItemSelectedListener;
    Int32 mScrollState;

    Boolean mScrollToSectionOnHeaderTouch;
    Boolean mHeaderTouched;

    Int32 mAnimationDuration;
    Boolean mAnimating;
    Int64 mAnimationTargetTime;
    Int32 mHeaderPaddingStart;
    Int32 mHeaderWidth;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Contacts::Common::List::CPinnedHeaderListView::PinnedHeader, IInterface)

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CPINNEDHEADERLISTVIEW_H__
