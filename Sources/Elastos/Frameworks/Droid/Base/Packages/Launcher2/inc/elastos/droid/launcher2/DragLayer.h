#ifndef  __ELASTOS_DROID_LAUNCHER2_DRAGLAYER_H__
#define  __ELASTOS_DROID_LAUNCHER2_DRAGLAYER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class DragLayer
    : public FrameLayout
    , public IDragLayer
    , public IViewGroupOnHierarchyChangeListener
{
private:
    class MyAnimatorUpdateListener:
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener(
            /* [in] */ IDragView* view,
            /* [in] */ IInterpolator* alphaInterpolator,
            /* [in] */ IInterpolator* motionInterpolator,
            /* [in] */ Float initScaleX,
            /* [in] */ Float initScaleY,
            /* [in] */ Float dropViewScale,
            /* [in] */ Float finalScaleX,
            /* [in] */ Float finalScaleY,
            /* [in] */ Float finalAlpha,
            /* [in] */ Float initAlpha,
            /* [in] */ IRect* from,
            /* [in] */ IRect* to,
            /* [in] */ LayoutParams* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IDragView> mView;
        AutoPtr<IInterpolator> mAlphaInterpolator;
        AutoPtr<IInterpolator> mMotionInterpolator;
        Float mInitScaleX;
        Float mInitScaleY;
        Float mDropViewScale;
        Float mFinalScaleX;
        Float mFinalScaleY;
        Float mFinalAlpha;
        Float mInitAlpha;
        AutoPtr<IRect> mFrom;
        AutoPtr<IRect> mTo;
        AutoPtr<LayoutParams> mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* onCompleteRunnable,
            /* [in] */ Int32 animationEndStyle,
            /* [in] */ LayoutParams* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IIRunnable> mOnCompleteRunnable;
        Int32 mAnimationEndStyle;
        AutoPtr<ILayoutParams> mHost;
    };

    class MyAnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener2(
            /* [in] */ LayoutParams* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<ILayoutParams> mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ LayoutParams* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<ILayoutParams> mHost;
    };

public:
    class LayoutParams
        : public FrameLayout::LayoutParams
        , public ILayoutParams
    {
    public:
        /**
         * {@inheritDoc}
         */
        LayoutParams(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetWidth(
            /* [in] */ Int32 width);

        CARAPI GetWidth(
            /* [out] */ Int32* value);

        CARAPI SetHeight(
            /* [in] */ Int32 height);

        CARAPI GetHeight(
            /* [out] */ Int32* value);

        CARAPI SetX(
            /* [in] */ Int32 x);

        CARAPI GetX(
            /* [out] */ Int32* value);

        CARAPI SetY(
            /* [in] */ Int32 y);

        CARAPI GetY(
            /* [out] */ Int32* value);

    public:
        Int32 mX, mY;
        Boolean mCustomPosition;
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IView* child,
            /* [in] */ IRunnable* onFinishAnimationRunnable);

        CARAPI Run();

    private:
        AutoPtr<IView> mChild;
        AutoPtr<IRunnable> mOnFinishAnimationRunnable;
    };

public:
    CAR_INTERFACE_DECL();

    DragLayer();

    /**
     * Used to create a new DragLayer from XML.
     *
     * @param context The application's context.
     * @param attrs The attributes set containing the Workspace's customization values.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Setup(
        /* [in] */ ILauncher* launcher,
        /* [in] */ IDragController* controller);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnInterceptHoverEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI AddChildrenForAccessibility(
        /* [in] */ IArrayList* childrenForAccessibility);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Determine the rect of the descendant in this DragLayer's coordinates
     *
     * @param descendant The descendant whose coordinates we want to find.
     * @param r The rect into which to place the results.
     * @return The factor by which this descendant is scaled relative to this DragLayer.
     */
    CARAPI GetDescendantRectRelativeToSelf(
        /* [in] */ IView* descendant,
        /* [in] */ IRect* r,
        /* [out] */ Float* result);

    CARAPI GetLocationInDragLayer(
        /* [in] */ IView* child,
        /* [in] */ ArrayOf<Int32>* loc,
        /* [out] */ Float* result);

    /**
     * Given a coordinate relative to the descendant, find the coordinate in this DragLayer's
     * coordinates.
     *
     * @param descendant The descendant to which the passed coordinate is relative.
     * @param coord The coordinate that we want mapped.
     * @return The factor by which this descendant is scaled relative to this DragLayer. Caution
     *         this scale factor is assumed to be equal in X and Y, and so if at any point this
     *         assumption fails, we will need to return a pair of scale factors.
     */
    CARAPI GetDescendantCoordRelativeToSelf(
        /* [in] */ IView* descendant,
        /* [in] */ ArrayOf<Int32>* coord,
        /* [out] */ Float* result);

    CARAPI GetViewRectRelativeToSelf(
        /* [in] */ IView* v,
        /* [in] */ IRect* r);

    //@Override
    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    CARAPI ClearAllResizeFrames();

    CARAPI HasResizeFrames(
        /* [out] */ Boolean* result);

    CARAPI IsWidgetBeingResized(
        /* [out] */ Boolean* result);

    CARAPI AddResizeFrame(
        /* [in] */ IItemInfo* itemInfo,
        /* [in] */ ILauncherAppWidgetHostView* widget,
        /* [in] */ ICellLayout* cellLayout);

    CARAPI AnimateViewIntoPosition(
        /* [in] */ IDragView* dragView,
        /* [in] */ IView* child);

    CARAPI AnimateViewIntoPosition(
        /* [in] */ IDragView* dragView,
        /* [in] */ ArrayOf<Int32>* pos,
        /* [in] */ Float alpha,
        /* [in] */ Float scaleX,
        /* [in] */ Float scaleY,
        /* [in] */ Int32 animationEndStyle,
        /* [in] */ IRunnable* onFinishRunnable,
        /* [in] */ Int32 duration);

    CARAPI AnimateViewIntoPosition(
        /* [in] */ IDragView* dragView,
        /* [in] */ IView* child,
        /* [in] */ IRunnable* onFinishAnimationRunnable);

    CARAPI AnimateViewIntoPosition(
        /* [in] */ IDragView* dragView,
        /* [in] */ IView* child,
        /* [in] */ Int32 duration,
        /* [in] */ IRunnable* onFinishAnimationRunnable,
        /* [in] */ IView* anchorView);

    CARAPI AnimateViewIntoPosition(
        /* [in] */ IDragView* view,
        /* [in] */ Int32 fromX,
        /* [in] */ Int32 fromY,
        /* [in] */ Int32 toX,
        /* [in] */ Int32 toY,
        /* [in] */ Float finalAlpha,
        /* [in] */ Float initScaleX,
        /* [in] */ Float initScaleY,
        /* [in] */ Float finalScaleX,
        /* [in] */ Float finalScaleY,
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [in] */ Int32 animationEndStyle,
        /* [in] */ Int32 duration,
        /* [in] */ IView* anchorView);

    /**
     * This method animates a view at the end of a drag and drop animation.
     *
     * @param view The view to be animated. This view is drawn directly into DragLayer, and so
     *        doesn't need to be a child of DragLayer.
     * @param from The initial location of the view. Only the left and top parameters are used.
     * @param to The final location of the view. Only the left and top parameters are used. This
     *        location doesn't account for scaling, and so should be centered about the desired
     *        final location (including scaling).
     * @param finalAlpha The final alpha of the view, in case we want it to fade as it animates.
     * @param finalScale The final scale of the view. The view is scaled about its center.
     * @param duration The duration of the animation.
     * @param motionInterpolator The interpolator to use for the location of the view.
     * @param alphaInterpolator The interpolator to use for the alpha of the view.
     * @param onCompleteRunnable Optional runnable to run on animation completion.
     * @param fadeOut Whether or not to fade out the view once the animation completes. If true,
     *        the runnable will execute after the view is faded out.
     * @param anchorView If not null, this represents the view which the animated view stays
     *        anchored to in case scrolling is currently taking place. Note: currently this is
     *        only used for the X dimension for the case of the workspace.
     */
    CARAPI AnimateView(
        /* [in] */ IDragView* view,
        /* [in] */ IRect* from,
        /* [in] */ IRect* to,
        /* [in] */ Float finalAlpha,
        /* [in] */ Float initScaleX,
        /* [in] */ Float initScaleY,
        /* [in] */ Float finalScaleX,
        /* [in] */ Float finalScaleY,
        /* [in] */ Int32 duration,
        /* [in] */ IInterpolator* motionInterpolator,
        /* [in] */ IInterpolator* alphaInterpolator,
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [in] */ Int32 animationEndStyle,
        /* [in] */ IView* anchorView);

    CARAPI AnimateView(
        /* [in] */ IDragView* view,
        /* [in] */ IAnimatorUpdateListener* updateCb,
        /* [in] */ Int32 duration,
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [in] */ Int32 animationEndStyle,
        /* [in] */ IView* anchorView);

    CARAPI ClearAnimatedView();

    CARAPI GetAnimatedView(
        /* [out] */ IView** view);

    //@Override
    CARAPI OnChildViewAdded(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    //@Override
    CARAPI OnChildViewRemoved(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    CARAPI OnEnterScrollArea(
        /* [in] */ Int32 direction);

    CARAPI OnExitScrollArea();

protected:
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i,
        /* [out] */ Int32* order);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Boolean) IsEventOverFolderTextRegion(
        /* [in] */ IFolder* folder,
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) IsEventOverFolder(
        /* [in] */ IFolder* folder,
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) HandleTouchDown(
        /* [in] */ IMotionEvent* ev,
        /* [in] */ Boolean intercept);

    CARAPI_(void) SendTapOutsideFolderAccessibilityEvent(
        /* [in] */ Boolean isEditingName);

    CARAPI_(void) FadeOutDragView();

    CARAPI_(void) UpdateChildIndices();

    /**
     * Note: this is a reimplementation of View.isLayoutRtl() since that is currently hidden api.
     */
    CARAPI_(Boolean) IsLayoutDirectionRtl();

private:
    AutoPtr<IDragController> mDragController;
    AutoPtr<ArrayOf<Int32> > mTmpXY;

    Int32 mXDown, mYDown;
    AutoPtr<ILauncher> mLauncher;

    // Variables relating to resizing widgets
    AutoPtr<IArrayList> mResizeFrames;
    AutoPtr<IAppWidgetResizeFrame> mCurrentResizeFrame;

    // Variables relating to animation of views after drop
    AutoPtr<IValueAnimator> mDropAnim;
    AutoPtr<IValueAnimator> mFadeOutAnim;
    AutoPtr<ITimeInterpolator> mCubicEaseOutInterpolator;
    AutoPtr<IDragView> mDropView;
    Int32 mAnchorViewInitialScrollX;
    AutoPtr<IView> mAnchorView;

    Boolean mHoverPointClosesFolder;
    AutoPtr<IRect> mHitRect;
    Int32 mWorkspaceIndex;
    Int32 mQsbIndex;

    Boolean mInScrollArea;
    AutoPtr<IDrawable> mLeftHoverDrawable;
    AutoPtr<IDrawable> mRightHoverDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DRAGLAYER_H__