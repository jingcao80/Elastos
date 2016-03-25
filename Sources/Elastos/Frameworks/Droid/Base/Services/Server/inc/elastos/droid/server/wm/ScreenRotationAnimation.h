#ifndef __ELASTOS_DROID_SERVER_WM_SCREENROTATIONANIMATION_H__
#define __ELASTOS_DROID_SERVER_WM_SCREENROTATIONANIMATION_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/wm/BlackFrame.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::Animation::ITransformation;
using Elastos::Droid::View::Animation::IAnimation;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class DisplayContent;

class ScreenRotationAnimation : public Object
{
public:
    class H : public Handler
    {
    public:
        H(
            /* [in] */ ILooper* looper,
            /* [in] */ ScreenRotationAnimation* host);

        ECode HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SCREENSHOT_FREEZE_TIMEOUT = 2;

        //Set the freeze timeout value to 6sec (which is greater than
        //APP_FREEZE_TIMEOUT value in WindowManagerService)
        static const Int32 FREEZE_TIMEOUT_VAL = 6000;

    private:
        ScreenRotationAnimation* mHost;
    };

public:
    ScreenRotationAnimation(
        /* [in] */ IContext* context,
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Boolean inTransaction,
        /* [in] */ Boolean forceDefaultOrientation,
        /* [in] */ Boolean isSecure);

    CARAPI_(Boolean) HasScreenshot();

    static CARAPI_(Int32) DeltaRotation(
        /* [in] */ Int32 oldRotation,
        /* [in] */ Int32 newRotation);

    static CARAPI_(void) CreateRotationMatrix(
        /* [in] */ Int32 rotation,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IMatrix* outMatrix);

    // Must be called while in a transaction.
    CARAPI_(Boolean) SetRotationInTransaction(
        /* [in] */ Int32 rotation,
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Int64 maxAnimationDuration,
        /* [in] */ Float animationScale,
        /* [in] */ Int32 finalWidth,
        /* [in] */ Int32 finalHeight);

    /**
     * Returns true if animating.
     */
    CARAPI_(Boolean) Dismiss(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Int64 maxAnimationDuration,
        /* [in] */ Float animationScale,
        /* [in] */ Int32 finalWidth,
        /* [in] */ Int32 finalHeight,
        /* [in] */ Int32 exitAnim,
        /* [in] */ Int32 enterAnim);

    CARAPI_(void) Kill();

    CARAPI_(Boolean) IsAnimating();

    CARAPI_(Boolean) IsRotating();

    CARAPI_(void) UpdateSurfacesInTransaction();

    CARAPI_(Boolean) StepAnimationLocked(
        /* [in] */ Int64 now);

    CARAPI_(AutoPtr<ITransformation>) GetEnterTransformation();

private:
    CARAPI_(void) SetSnapshotTransformInTransaction(
        /* [in] */ IMatrix* matrix,
        /* [in] */ Float alpha);

    // Must be called while in a transaction.
    CARAPI_(void) SetRotationInTransaction(
        /* [in] */ Int32 rotation);

    /**
     * Returns true if animating.
     */
    CARAPI_(Boolean) StartAnimation(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Int64 maxAnimationDuration,
        /* [in] */ Float animationScale,
        /* [in] */ Int32 finalWidth,
        /* [in] */ Int32 finalHeight,
        /* [in] */ Boolean dismissing,
        /* [in] */ Int32 exitAnim,
        /* [in] */ Int32 enterAnim);

    CARAPI_(Boolean) HasAnimations();

    CARAPI_(Boolean) StepAnimation(
        /* [in] */ Int64 now);

public:
    static const String TAG;
    static const Boolean DEBUG_STATE = FALSE;
    static const Boolean DEBUG_TRANSFORMS = FALSE;
    static const Boolean TWO_PHASE_ANIMATION = FALSE;
    static const Boolean USE_CUSTOM_BLACK_FRAME = FALSE;

    static const Int32 FREEZE_LAYER;

public:
    AutoPtr<IContext> mContext;
    AutoPtr<DisplayContent> mDisplayContent;
    AutoPtr<ISurfaceControl> mSurfaceControl;
    AutoPtr<BlackFrame> mCustomBlackFrame;
    AutoPtr<BlackFrame> mExitingBlackFrame;
    AutoPtr<BlackFrame> mEnteringBlackFrame;
    Int32 mWidth;
    Int32 mHeight;
    Int32 mSnapshotRotation;

    Int32 mOriginalRotation;
    Int32 mOriginalWidth;
    Int32 mOriginalHeight;
    Int32 mCurRotation;
    AutoPtr<IRect> mOriginalDisplayRect;
    AutoPtr<IRect> mCurrentDisplayRect;

    // For all animations, "exit" is for the UI elements that are going
    // away (that is the snapshot of the old screen), and "enter" is for
    // the new UI elements that are appearing (that is the active windows
    // in their final orientation).

    // The starting animation for the exiting and entering elements.  This
    // animation applies a transformation while the rotation is in progress.
    // It is started immediately, before the new entering UI is ready.
    AutoPtr<IAnimation> mStartExitAnimation;
    AutoPtr<ITransformation> mStartExitTransformation;
    AutoPtr<IAnimation> mStartEnterAnimation;
    AutoPtr<ITransformation> mStartEnterTransformation;
    AutoPtr<IAnimation> mStartFrameAnimation;
    AutoPtr<ITransformation> mStartFrameTransformation;

    // The finishing animation for the exiting and entering elements.  This
    // animation needs to undo the transformation of the starting animation.
    // It starts running once the new rotation UI elements are ready to be
    // displayed.
    AutoPtr<IAnimation> mFinishExitAnimation;
    AutoPtr<ITransformation> mFinishExitTransformation;
    AutoPtr<IAnimation> mFinishEnterAnimation;
    AutoPtr<ITransformation> mFinishEnterTransformation;
    AutoPtr<IAnimation> mFinishFrameAnimation;
    AutoPtr<ITransformation> mFinishFrameTransformation;

    // The current active animation to move from the old to the new rotated
    // state.  Which animation is run here will depend on the old and new
    // rotations.
    AutoPtr<IAnimation> mRotateExitAnimation;
    AutoPtr<ITransformation> mRotateExitTransformation;
    AutoPtr<IAnimation> mRotateEnterAnimation;
    AutoPtr<ITransformation> mRotateEnterTransformation;
    AutoPtr<IAnimation> mRotateFrameAnimation;
    AutoPtr<ITransformation> mRotateFrameTransformation;

    // A previously running rotate animation.  This will be used if we need
    // to switch to a new rotation before finishing the previous one.
    AutoPtr<IAnimation> mLastRotateExitAnimation;
    AutoPtr<ITransformation> mLastRotateExitTransformation;
    AutoPtr<IAnimation> mLastRotateEnterAnimation;
    AutoPtr<ITransformation> mLastRotateEnterTransformation;
    AutoPtr<IAnimation> mLastRotateFrameAnimation;
    AutoPtr<ITransformation> mLastRotateFrameTransformation;

    // Complete transformations being applied.
    AutoPtr<ITransformation> mExitTransformation;
    AutoPtr<ITransformation> mEnterTransformation;
    AutoPtr<ITransformation> mFrameTransformation;

    Boolean mStarted;
    Boolean mAnimRunning;
    Boolean mFinishAnimReady;
    Int64 mFinishAnimStartTime;
    Boolean mForceDefaultOrientation;

    AutoPtr<IMatrix> mFrameInitialMatrix;
    AutoPtr<IMatrix> mSnapshotInitialMatrix;
    AutoPtr<IMatrix> mSnapshotFinalMatrix;
    AutoPtr<IMatrix> mExitFrameFinalMatrix;
    AutoPtr<IMatrix> mTmpMatrix;
    AutoPtr<ArrayOf<Float> > mTmpFloats;

    Int64 mHalfwayPoint;

    AutoPtr<H> mHandler;

private:
    Boolean mMoreRotateEnter;
    Boolean mMoreRotateExit;
    Boolean mMoreRotateFrame;
    Boolean mMoreFinishEnter;
    Boolean mMoreFinishExit;
    Boolean mMoreFinishFrame;
    Boolean mMoreStartEnter;
    Boolean mMoreStartExit;
    Boolean mMoreStartFrame;

    static Int32 mHwrotation;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_SCREENROTATIONANIMATION_H__
