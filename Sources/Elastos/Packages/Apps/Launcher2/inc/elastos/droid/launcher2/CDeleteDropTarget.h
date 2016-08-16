#ifndef  __ELASTOS_DROID_LAUNCHER2_CDELETEDROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_CDELETEDROPTARGET_H__

#include "_Elastos_Droid_Launcher2_CDeleteDropTarget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ButtonDropTarget.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "elastos/droid/launcher2/LauncherAppWidgetHost.h"
#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Launcher2::DragObject;
using Elastos::Droid::Launcher2::LauncherAppWidgetHost;
using Elastos::Droid::Launcher2::LauncherAppWidgetInfo;
using Elastos::Droid::Launcher2::ButtonDropTarget;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Core::Object;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CDeleteDropTarget)
    , public ButtonDropTarget
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
        CAR_INTERFACE_DECL();

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
            /* [in] */ CDeleteDropTarget* host,
            /* [in] */ DragObject* d);

        CARAPI Run();

    private:
        AutoPtr<CDeleteDropTarget> mHost;
        AutoPtr<DragObject> mD;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ LauncherAppWidgetHost* appWidgetHost,
            /* [in] */ LauncherAppWidgetInfo* launcherAppWidgetInfo);

        CARAPI Run();

    private:
        AutoPtr<LauncherAppWidgetHost> mAppWidgetHost;
        AutoPtr<LauncherAppWidgetInfo> mLauncherAppWidgetInfo;
    };

    class FlingToTrashAnimatorInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float t,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);
    };

    class FlingToTrashAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        FlingToTrashAnimatorUpdateListener(
            /* [in] */ IDragLayer* dragLayer,
            /* [in] */ ITimeInterpolator* scaleAlphaInterpolator,
            /* [in] */ Float x1,
            /* [in] */ Float x2,
            /* [in] */ Float x3,
            /* [in] */ Float y1,
            /* [in] */ Float y2,
            /* [in] */ Float y3);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IDragLayer> mDragLayer;
        AutoPtr<ITimeInterpolator> mScaleAlphaInterpolator;
        Float mX1;
        Float mX2;
        Float mX3;
        Float mY1;
        Float mY2;
        Float mY3;
    };

    class FlingToDeleteAnimatorInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        FlingToDeleteAnimatorInterpolator(
            /* [in] */ Int64 startTime,
            /* [in] */ Int32 duration);

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float t,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Int32 mCount;
        Float mOffset;
        Int64 mStartTime;
        Int32 mDuration;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ CDeleteDropTarget* host,
            /* [in] */ Boolean isAllApps,
            /* [in] */ DragObject* d);

        CARAPI Run();

    private:
        AutoPtr<CDeleteDropTarget> mHost;
        Boolean mIsAllApps;
        AutoPtr<DragObject> mD;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CDeleteDropTarget();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI AcceptDrop(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    //@Override
    CARAPI OnDragEnd();

    CARAPI OnDragEnter(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnDragExit(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnDrop(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnFlingToDelete(
        /* [in] */ IDropTargetDragObject* d,
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

#endif // __ELASTOS_DROID_LAUNCHER2_CDELETEDROPTARGET_H__