#ifndef  __ELASTOS_DROID_LAUNCHER2_DELETEDROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_DELETEDROPTARGET_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class DeleteDropTarget
    : public ButtonDropTarget
    , public IDeleteDropTarget
{
private:
    /**
     * Creates an animation from the current drag view along its current velocity vector.
     * For this animation, the alpha runs for a fixed duration and we update the position
     * progressively.
     */
    class FlingAlongVectorAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        FlingAlongVectorAnimatorUpdateListener(
            /* [in] */ IDragLayer* dragLayer,
            /* [in] */ IPointF* vel,
            /* [in] */ IRect* from,
            /* [in] */ Int64 startTime,
            /* [in] */ Float friction);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IDragLayer> mDragLayer;
        AutoPtr<IPointF> mVelocity;
        AutoPtr<IRect> mFrom;
        Int64 mPrevTime;
        Boolean mHasOffsetForScale;
        Float mFriction;

        AutoPtr<ITimeInterpolator> mAlphaInterpolator;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ DeleteDropTarget* host);

        CARAPI Run();

    private:
        DeleteDropTarget* mHost;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ ILauncherAppWidgetHost* appWidgetHost,
            /* [in] */ ILauncherAppWidgetInfo* launcherAppWidgetInfo);

        CARAPI Run();

    private:
        AutoPtr<ILauncherAppWidgetHost> mAppWidgetHost;
        AutoPtr<ILauncherAppWidgetInfo> mLauncherAppWidgetInfo;
    };

    class MyTimeInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float t,
            /* [out] */ Float* result);
    };

    class MyIAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyIAnimatorUpdateListener(
            /* [in] */ IDragLayer* dragLayer,
            /* [in] */ ITimeInterpolator* scaleAlphaInterpolator);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IDragLayer> mDragLayer;
        AutoPtr<ITimeInterpolator> mScaleAlphaInterpolator;
    };

    class MyTimeInterpolator2
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        MyTimeInterpolator2();

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float t,
            /* [out] */ Float* result);

    private:
        Int32 mCount = -1;
        Float mOffset = 0f;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ DeleteDropTarget* host,
            /* [in] */ Boolean isAllApps,
            /* [in] */ DragObject* d);

        CARAPI Run();

    private:
        DeleteDropTarget* mHost;
        Boolean mIsAllApps;
        DragObject* mD;
    };

public:
    CAR_INTERFACE_DECL();

    DeleteDropTarget();

    CARAPI construcor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI construcor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI AcceptDrop(
        /* [in] */ DragObject* d,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    //@Override
    CARAPI OnDragEnd();

    CARAPI OnDragEnter(
        /* [in] */ DragObject* d);

    CARAPI OnDragExit(
        /* [in] */ DragObject* d);

    CARAPI OnDrop(
        /* [in] */ DragObject* d);

    CARAPI OnFlingToDelete(
        /* [in] */ DragObject* d,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPointF* vel);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) IsAllAppsApplication(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info);

    CARAPI_(Boolean) IsAllAppsWidget(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info);

    CARAPI_(Boolean) IsDragSourceWorkspaceOrFolder(
        /* [in] */ DragObject* d);

    CARAPI_(Boolean) IsWorkspaceOrFolderApplication(
        /* [in] */ DragObject* d);

    CARAPI_(Boolean) IsWorkspaceOrFolderWidget(
        /* [in] */ DragObject* d);

    CARAPI_(Boolean) IsWorkspaceFolder(
        /* [in] */ DragObject* d);

    CARAPI_(void) SetHoverColor();

    CARAPI_(void) ResetHoverColor();

    CARAPI_(void) AnimateToTrashAndCompleteDrop(
        /* [in] */ DragObject* d);

    CARAPI_(void) CompleteDrop(
        /* [in] */ DragObject* d);

    /**
     * Creates an animation from the current drag view to the delete trash icon.
     */
    CARAPI_(AutoPtr<IAnimatorUpdateListener>) CreateFlingToTrashAnimatorListener(
        /* [in] */ IDragLayer* dragLayer,
        /* [in] */ DragObject* d,
        /* [in] */ IPointF* vel,
        /* [in] */ IViewConfiguration* config);

    CARAPI_(AutoPtr<IAnimatorUpdateListener>) CreateFlingAlongVectorAnimatorListener(
        /* [in] */ IDragLayer* dragLayer,
        /* [in] */ DragObject* d,
        /* [in] */ IPointF* vel,
        /* [in] */ Int64 startTime,
        /* [in] */ Int32 duration,
        /* [in] */ IViewConfiguration* config);


private:
    static Int32 DELETE_ANIMATION_DURATION;
    static Int32 FLING_DELETE_ANIMATION_DURATION;
    static Float FLING_TO_DELETE_FRICTION;
    static Int32 MODE_FLING_DELETE_TO_TRASH;
    static Int32 MODE_FLING_DELETE_ALONG_VECTOR;

    Int32 mFlingDeleteMode;

    AutoPtr<IColorStateList> mOriginalTextColor;
    AutoPtr<ITransitionDrawable> mUninstallDrawable;
    AutoPtr<ITransitionDrawable> mRemoveDrawable;
    AutoPtr<ITransitionDrawable> mCurrentDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DELETEDROPTARGET_H__