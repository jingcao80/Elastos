
#ifndef __ELASTOS_DROID_WIDGET_SLIDINGDRAWER_H__
#define __ELASTOS_DROID_WIDGET_SLIDINGDRAWER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::VelocityTracker;

/**
 * SlidingDrawer hides content out of the screen and allows the user to drag a handle
 * to bring the content on screen. SlidingDrawer can be used vertically or horizontally.
 *
 * A special widget composed of two children views: the handle, that the users drags,
 * and the content, attached to the handle and dragged with it.
 *
 * SlidingDrawer should be used as an overlay inside layouts. This means SlidingDrawer
 * should only be used inside of a FrameLayout or a RelativeLayout for instance. The
 * size of the SlidingDrawer defines how much space the content will occupy once slid
 * out so SlidingDrawer should usually use match_parent for both its dimensions.
 *
 * Inside an XML layout, SlidingDrawer must define the id of the handle and of the
 * content:
 *
 * <pre class="prettyprint">
 * &lt;SlidingDrawer
 *     android:id="@+id/drawer"
 *     android:layout_width="match_parent"
 *     android:layout_height="match_parent"
 *
 *     android:handle="@+id/handle"
 *     android:content="@+id/content"&gt;
 *
 *     &lt;ImageView
 *         android:id="@id/handle"
 *         android:layout_width="88dip"
 *         android:layout_height="44dip" /&gt;
 *
 *     &lt;GridView
 *         android:id="@id/content"
 *         android:layout_width="match_parent"
 *         android:layout_height="match_parent" /&gt;
 *
 * &lt;/SlidingDrawer&gt;
 * </pre>
 *
 * @attr ref android.R.styleable#SlidingDrawer_content
 * @attr ref android.R.styleable#SlidingDrawer_handle
 * @attr ref android.R.styleable#SlidingDrawer_topOffset
 * @attr ref android.R.styleable#SlidingDrawer_bottomOffset
 * @attr ref android.R.styleable#SlidingDrawer_orientation
 * @attr ref android.R.styleable#SlidingDrawer_allowSingleTap
 * @attr ref android.R.styleable#SlidingDrawer_animateOnClick
 *
 * @deprecated This class is not supported anymore. It is recommended you
 * base your own implementation on the source code for the Android Open
 * Source Project if you must use it in your application.
 */

namespace Elastos {
namespace Droid {
namespace Widget {

class SlidingDrawer
    : public ViewGroup
    , public ISlidingDrawer
{
private:
    class SlidingHandler : public Handler
    {
    public:
        SlidingHandler(
            /* [in] */ SlidingDrawer* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        SlidingDrawer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SlidingDrawer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI OnFinishInflate();

    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    /**
     * Toggles the drawer open and close. Takes effect immediately.
     *
     * @see #open()
     * @see #close()
     * @see #animateClose()
     * @see #animateOpen()
     * @see #animateToggle()
     */
    virtual CARAPI Toggle();

    /**
     * Toggles the drawer open and close with an animation.
     *
     * @see #open()
     * @see #close()
     * @see #animateClose()
     * @see #animateOpen()
     * @see #toggle()
     */
    virtual CARAPI AnimateToggle();
    /**
     * Opens the drawer immediately.
     *
     * @see #toggle()
     * @see #close()
     * @see #animateOpen()
     */
    virtual CARAPI Open();

    /**
     * Closes the drawer immediately.
     *
     * @see #toggle()
     * @see #open()
     * @see #animateClose()
     */
    virtual CARAPI Close();

    /**
     * Closes the drawer with an animation.
     *
     * @see #close()
     * @see #open()
     * @see #animateOpen()
     * @see #animateToggle()
     * @see #toggle()
     */
    virtual CARAPI AnimateClose();

    /**
     * Opens the drawer with an animation.
     *
     * @see #close()
     * @see #open()
     * @see #animateClose()
     * @see #animateToggle()
     * @see #toggle()
     */
    virtual CARAPI AnimateOpen();

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
    /**
     * Sets the listener that receives a notification when the drawer becomes open.
     *
     * @param onDrawerOpenListener The listener to be notified when the drawer is opened.
     */
    virtual CARAPI SetOnDrawerOpenListener(
        /* [in] */ IOnDrawerOpenListener* onDrawerOpenListener);

    /**
     * Sets the listener that receives a notification when the drawer becomes close.
     *
     * @param onDrawerCloseListener The listener to be notified when the drawer is closed.
     */
    virtual CARAPI SetOnDrawerCloseListener(
        /* [in] */ IOnDrawerCloseListener* onDrawerCloseListener);

    /**
     * Sets the listener that receives a notification when the drawer starts or ends
     * a scroll. A fling is considered as a scroll. A fling will also trigger a
     * drawer opened or drawer closed event.
     *
     * @param onDrawerScrollListener The listener to be notified when scrolling
     *        starts or stops.
     */
    virtual CARAPI SetOnDrawerScrollListener(
        /* [in] */ IOnDrawerScrollListener* onDrawerScrollListener);

    /**
     * Returns the handle of the drawer.
     *
     * @return The AutoPtr<IView> reprenseting the handle of the drawer, identified by
     *         the "handle" id in XML.
     */
    virtual CARAPI GetHandle(
        /* [out] */ IView** handle);

    /**
     * Returns the content of the drawer.
     *
     * @return The AutoPtr<IView> reprenseting the content of the drawer, identified by
     *         the "content" id in XML.
     */
    virtual CARAPI GetContent(
        /* [out] */ IView** handle);

    /**
     * Unlocks the SlidingDrawer so that touch events are processed.
     *
     * @see #lock()
     */
    virtual CARAPI Unlock();
    /**
     * Locks the SlidingDrawer so that touch events are ignores.
     *
     * @see #unlock()
     */
    virtual CARAPI Lock();

    /**
     * Indicates whether the drawer is currently fully opened.
     *
     * @return True if the drawer is opened, false otherwise.
     */
    virtual CARAPI IsOpened(
        /* [out] */ Boolean* opened);

    /**
     * Indicates whether the drawer is scrolling or flinging.
     *
     * @return True if the drawer is scroller or flinging, false otherwise.
     */
    virtual CARAPI IsMoving(
        /* [out] */ Boolean* moving);

    CARAPI AnimateClose(
        /* [in] */ Int32 position);

    CARAPI AnimateOpen(
        /* [in] */ Int32 position);

    CARAPI PerformFling(
        /* [in] */ Int32 position,
        /* [in] */ Float velocity,
        /* [in] */ Boolean always);

    CARAPI PrepareTracking(
        /* [in] */ Int32 position);

    CARAPI MoveHandle(
        /* [in] */ Int32 position);

    CARAPI PrepareContent();

    CARAPI StopTracking();

    CARAPI DoAnimation();

    CARAPI IncrementAnimation();


    CARAPI CloseDrawer();

    CARAPI OpenDrawer();

    CARAPI HandleMessage();

private:
    class DrawerToggler
        : public IViewOnClickListener
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        DrawerToggler(
            /* [in] */ SlidingDrawer* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        SlidingDrawer* mHost;
    };

private:
    static const String TAG;
    static const Int32 TAP_THRESHOLD = 6;
    static const Float MAXIMUM_TAP_VELOCITY = 100.0f;
    static const Float MAXIMUM_MINOR_VELOCITY = 150.0f;
    static const Float MAXIMUM_MAJOR_VELOCITY = 200.0f;
    static const Float MAXIMUM_ACCELERATION = 2000.0f;
    static const Int32 VELOCITY_UNITS = 1000;
    static const Int32 MSG_ANIMATE = 1000;
    static const Int32 ANIMATION_FRAME_DURATION = 1000 / 60;

    static const Int32 EXPANDED_FULL_OPEN = -10001;
    static const Int32 COLLAPSED_FULL_CLOSED = -10002;

    Int32 mHandleId;
    Int32 mContentId;

    AutoPtr<IView> mHandle;
    AutoPtr<IView> mContent;

    AutoPtr<IRect> mFrame;// = new Rect();
    AutoPtr<IRect> mInvalidate;// = new Rect();
    Boolean mTracking;
    Boolean mLocked;

    AutoPtr<VelocityTracker> mVelocityTracker;

    Boolean mVertical;
    Boolean mExpanded;
    Int32 mBottomOffset;
    Int32 mTopOffset;
    Int32 mHandleHeight;
    Int32 mHandleWidth;

    AutoPtr<IOnDrawerOpenListener> mOnDrawerOpenListener;
    AutoPtr<IOnDrawerCloseListener> mOnDrawerCloseListener;
    AutoPtr<IOnDrawerScrollListener> mOnDrawerScrollListener;

    AutoPtr<IHandler> mHandler;// = new SlidingHandler();
    Float mAnimatedAcceleration;
    Float mAnimatedVelocity;
    Float mAnimationPosition;
    Int64 mAnimationLastTime;
    Int64 mCurrentAnimationTime;
    Int32 mTouchDelta;
    Boolean mAnimating;
    Boolean mAllowSingleTap;
    Boolean mAnimateOnClick;

    Int32 mTapThreshold;
    Int32 mMaximumTapVelocity;
    Int32 mMaximumMinorVelocity;
    Int32 mMaximumMajorVelocity;
    Int32 mMaximumAcceleration;
    Int32 mVelocityUnits;

};

} // namespace Widget
} // namespace Droid
} // namespace Elastos
#endif

