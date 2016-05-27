#ifndef __ELASTOS_APPS_DIALER_LIST_PHONEFAVORITELISTVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_PHONEFAVORITELISTVIEW_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

/**
 * Viewgroup that presents the user's speed dial contacts in a grid.
 */
class PhoneFavoriteListView
    : public GridView
    , public IPhoneFavoriteListView
    , public IOnDragDropListener
    , public IDragItemContainer
{
private:
    class DragScroller
        : public Runnable
    {
    public:
        DragScroller(
            /* [in] */ PhoneFavoriteListView* host);

        // @Override
        CARAPI Run();

    private:
        PhoneFavoriteListView* mHost;
    };

    class DragShadowOverAnimatorListener
        : AnimatorListenerAdapter
    {
    public:
        DragShadowOverAnimatorListener(
            /* [in] */ PhoneFavoriteListView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PhoneFavoriteListView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PhoneFavoriteListView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    /**
     * TODO: This is all swipe to remove code (nothing to do with drag to remove). This should
     * be cleaned up and removed once drag to remove becomes the only way to remove contacts.
     */
    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetDragShadowOverlay(
        /* [in] */ IImageView* overlay);

    CARAPI GetDragDropController(
        /* [out] */ IDragDropController** controller);

    // @Override
    CARAPI OnDragStarted(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPhoneFavoriteSquareTileView* tileView);

    // @Override
    CARAPI OnDragHovered(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPhoneFavoriteSquareTileView* tileView);

    // @Override
    CARAPI OnDragFinished(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // @Override
    CARAPI OnDroppedOnRemove();

    // @Override
    CARAPI GetViewForLocation(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ IPhoneFavoriteSquareTileView* view);

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    /**
     * Find the view under the pointer.
     */
    CARAPI_(AutoPtr<IView>) GetViewAtPosition(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) EnsureScrollHandler();

    CARAPI_(AutoPtr<IBitmap>) CreateDraggedChildBitmap(
        /* [in] */ IView* view);

public:
    static const String LOG_TAG;

private:
    Float mTouchSlop;

    Int32 mTopScrollBound;
    Int32 mBottomScrollBound;
    Int32 mLastDragY;

    AutoPtr<IHandler> mScrollHandler;
    const Int64 SCROLL_HANDLER_DELAY_MILLIS; // = 5;
    const Int32 DRAG_SCROLL_PX_UNIT; // = 25;

    Boolean mIsDragScrollerRunning; // = false;
    Int32 mTouchDownForDragStartX;
    Int32 mTouchDownForDragStartY;

    AutoPtr<IBitmap> mDragShadowBitmap;
    AutoPtr<IImageView> mDragShadowOverlay;
    AutoPtr<IView> mDragShadowParent;
    Int32 mAnimationDuration;

    Int32 mLocationOnScreen[2];

    // X and Y offsets inside the item from where the user grabbed to the
    // child's left coordinate. This is used to aid in the drawing of the drag shadow.
    Int32 mTouchOffsetToChildLeft;
    Int32 mTouchOffsetToChildTop;

    Int32 mDragShadowLeft;
    Int32 mDragShadowTop;

    AutoPtr<IDragDropController> mDragDropController; // = new DragDropController(this);

    const Float DRAG_SHADOW_ALPHA; // = 0.7f;

    /**
     * {@link #mTopScrollBound} and {@link mBottomScrollBound} will be
     * offseted to the top / bottom by {@link #getHeight} * {@link #BOUND_GAP_RATIO} pixels.
     */
    const Float BOUND_GAP_RATIO; // = 0.2f;

    AutoPtr<IRunnable> mDragScroller;
    AutoPtr<IAnimatorListenerAdapter> mDragShadowOverAnimatorListener;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_PHONEFAVORITELISTVIEW_H__
