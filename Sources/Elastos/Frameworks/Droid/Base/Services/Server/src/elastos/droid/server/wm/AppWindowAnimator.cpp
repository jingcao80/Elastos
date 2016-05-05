
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/wm/AppWindowAnimator.h"
#include "elastos/droid/server/wm/ScreenRotationAnimation.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::Animation::CTransformation;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  AppWindowAnimator::DummyAnimation
//==============================================================================

ECode AppWindowAnimator::DummyAnimation::GetTransformation(
    /* [in] */ Int64 currentTime,
    /* [in, out] */ ITransformation* outTransformation,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode AppWindowAnimator::DummyAnimation::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<DummyAnimation> animation = new DummyAnimation();
    animation->CloneImpl((IAnimation*)animation.Get());
    *obj = (IAnimation*)animation.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}


//==============================================================================
//                  AppWindowAnimator
//==============================================================================

const String AppWindowAnimator::TAG("AppWindowAnimator");
const AutoPtr<IAnimation> AppWindowAnimator::sDummyAnimation = (IAnimation*)new DummyAnimation();

AppWindowAnimator::AppWindowAnimator(
    /* [in] */ AppWindowToken* atoken)
    : mService(atoken->mService)
    , mAnimator(atoken->mAnimator)
    , mAnimating(FALSE)
    , mHasTransformation(FALSE)
    , mFreezingScreen(FALSE)
    , mLastFreezeDuration(0)
    , mAnimLayerAdjustment(0)
    , mAllDrawn(FALSE)
    , mThumbnailTransactionSeq(0)
    , mThumbnailX(0)
    , mThumbnailY(0)
    , mThumbnailLayer(0)
    , mThumbnailForceAboveLayer(0)
    , mDeferThumbnailDestruction(FALSE)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)atoken;
    assert(wrs != NULL);
    wrs->GetWeakReference((IWeakReference**)&mWeakAppToken);

    CTransformation::New((ITransformation**)&mTransformation);
    CTransformation::New((ITransformation**)&mThumbnailTransformation);
}

AutoPtr<AppWindowToken> AppWindowAnimator::GetAppToken()
{
    AutoPtr<AppWindowToken> appWinToken;
    if (mWeakAppToken != NULL) {
        AutoPtr<IInterface> obj;
        mWeakAppToken->Resolve(EIID_IInterface, (IInterface**)&obj);
        appWinToken = (AppWindowToken*)(IObject*)obj.Get();
    }
    return appWinToken;
}

void AppWindowAnimator::SetAnimation(
    /* [in] */ IAnimation* anim,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<AppWindowToken> appToken = GetAppToken();
    if (appToken == NULL) return;

    if (CWindowManagerService::localLOGV) {
        Slogger::V(TAG, "Setting animation in %p: %p wxh=%dx%d isVisible=%d", appToken.Get()
                , anim, width, height, appToken->IsVisible());
    }

    mAnimation = anim;
    mAnimating = FALSE;
    Boolean isInitialized;
    if (anim->IsInitialized(&isInitialized), !isInitialized) {
        anim->Initialize(width, height, width, height);
    }
    anim->RestrictDuration(CWindowManagerService::MAX_ANIMATION_DURATION);
    anim->ScaleCurrentDuration(mService->GetTransitionAnimationScaleLocked());
    Int32 zorder;
    anim->GetZAdjustment(&zorder);
    Int32 adj = 0;
    if (zorder == IAnimation::ZORDER_TOP) {
        adj = CWindowManagerService::TYPE_LAYER_OFFSET;
    }
    else if (zorder == IAnimation::ZORDER_BOTTOM) {
        adj = -CWindowManagerService::TYPE_LAYER_OFFSET;
    }

    if (mAnimLayerAdjustment != adj) {
        mAnimLayerAdjustment = adj;
        UpdateLayers();
    }
    // Start out animation gone if window is gone, or visible if window is visible.
    mTransformation->Clear();
    mTransformation->SetAlpha(appToken->IsVisible() ? 1 : 0);
    mHasTransformation = TRUE;

    if (!appToken->mAppFullscreen) {
        anim->SetBackgroundColor(0);
    }
}

void AppWindowAnimator::SetDummyAnimation()
{
    AutoPtr<AppWindowToken> appToken = GetAppToken();
    if (appToken == NULL) return;

    if (CWindowManagerService::localLOGV) {
        Slogger::V(TAG, "Setting dummy animation in %p isVisible=%d", appToken.Get(), appToken->IsVisible());
    }

    mAnimation = sDummyAnimation;
    mHasTransformation = TRUE;
    mTransformation->Clear();
    mTransformation->SetAlpha(appToken->IsVisible() ? 1 : 0);
}

void AppWindowAnimator::ClearAnimation()
{
    if (mAnimation != NULL) {
        mAnimation = NULL;
        mAnimating = TRUE;
    }
    if (!mDeferThumbnailDestruction) {
        ClearThumbnail();
    }
    AutoPtr<AppWindowToken> appToken = GetAppToken();
    if (appToken == NULL) return;
    if (appToken->mDeferClearAllDrawn) {
        appToken->mAllDrawn = FALSE;
        appToken->mDeferClearAllDrawn = FALSE;
    }
}

void AppWindowAnimator::ClearThumbnail()
{
    if (mThumbnail != NULL) {
        mThumbnail->Destroy();
        mThumbnail = NULL;
    }
}

void AppWindowAnimator::ClearDeferredThumbnail()
{
    if (mDeferredThumbnail != NULL) {
        mDeferredThumbnail->Destroy();
        mDeferredThumbnail = NULL;
    }
}

void AppWindowAnimator::UpdateLayers()
{
    AutoPtr<AppWindowToken> appToken = GetAppToken();
    Int32 N;
    appToken->mAllAppWindows->GetSize(&N);
    Int32 adj = mAnimLayerAdjustment;
    mThumbnailLayer = -1;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        appToken->mAllAppWindows->Get(i, (IInterface**)&obj);
        AutoPtr<WindowState> w = To_WindowState(obj);
        AutoPtr<WindowStateAnimator> winAnimator = w->mWinAnimator;
        winAnimator->mAnimLayer = w->mLayer + adj;
        if (winAnimator->mAnimLayer > mThumbnailLayer) {
            mThumbnailLayer = winAnimator->mAnimLayer;
        }
        // if (WindowManagerService.DEBUG_LAYERS) Slog.v(TAG, "Updating layer " + w + ": "
        //         + winAnimator.mAnimLayer);
        if (w == mService->mInputMethodTarget && !mService->mInputMethodTargetWaitingAnim) {
            mService->SetInputMethodAnimLayerAdjustment(adj);
        }
        if (w == mService->mWallpaperTarget && mService->mLowerWallpaperTarget == NULL) {
            mService->SetWallpaperAnimLayerAdjustmentLocked(adj);
        }
    }
}

void AppWindowAnimator::StepThumbnailAnimation(
    /* [in] */ Int64 currentTime)
{
    mThumbnailTransformation->Clear();
    Boolean result;
    mThumbnailAnimation->GetTransformation(currentTime, mThumbnailTransformation, &result);
    AutoPtr<IMatrix> matrix;
    mThumbnailTransformation->GetMatrix((IMatrix**)&matrix);
    matrix->PreTranslate((Float)mThumbnailX, (Float)mThumbnailY, &result);

    AutoPtr<ScreenRotationAnimation> screenRotationAnimation =
            mAnimator->GetScreenRotationAnimationLocked(IDisplay::DEFAULT_DISPLAY);
    Boolean screenAnimation = screenRotationAnimation != NULL
            && screenRotationAnimation->IsAnimating();
    if (screenAnimation) {
        mThumbnailTransformation->PostCompose(
                screenRotationAnimation->GetEnterTransformation());
    }
    // cache often used attributes locally
    AutoPtr<ArrayOf<Float> > tmpFloats = mService->mTmpFloats;
    matrix->GetValues(tmpFloats.Get());
    // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(thumbnail,
    //         "thumbnail", "POS " + tmpFloats[Matrix.MTRANS_X]
    //         + ", " + tmpFloats[Matrix.MTRANS_Y], null);
    mThumbnail->SetPosition((*tmpFloats)[IMatrix::MTRANS_X],
            (*tmpFloats)[IMatrix::MTRANS_Y]);
    // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(thumbnail,
    //         "thumbnail", "alpha=" + thumbnailTransformation.getAlpha()
    //         + " layer=" + thumbnailLayer
    //         + " matrix=[" + tmpFloats[Matrix.MSCALE_X]
    //         + "," + tmpFloats[Matrix.MSKEW_Y]
    //         + "][" + tmpFloats[Matrix.MSKEW_X]
    //         + "," + tmpFloats[Matrix.MSCALE_Y] + "]", null);
    Float alpha;
    mThumbnailTransformation->GetAlpha(&alpha);
    mThumbnail->SetAlpha(alpha);
    if (mThumbnailForceAboveLayer > 0) {
        mThumbnail->SetLayer(mThumbnailForceAboveLayer + 1);
    }
    else {
        // The thumbnail is layered below the window immediately above this
        // token's anim layer.
        mThumbnail->SetLayer(mThumbnailLayer + CWindowManagerService::WINDOW_LAYER_MULTIPLIER
                - CWindowManagerService::LAYER_OFFSET_THUMBNAIL);
    }
    mThumbnail->SetMatrix((*tmpFloats)[IMatrix::MSCALE_X], (*tmpFloats)[IMatrix::MSKEW_Y],
            (*tmpFloats)[IMatrix::MSKEW_X], (*tmpFloats)[IMatrix::MSCALE_Y]);
}

Boolean AppWindowAnimator::StepAnimation(
    /* [in] */ Int64 currentTime)
{
    if (mAnimation == NULL) {
        return FALSE;
    }
    mTransformation->Clear();
    Boolean more;
    mAnimation->GetTransformation(currentTime, mTransformation, &more);
    // if (false && WindowManagerService.DEBUG_ANIM) Slog.v(
    //     TAG, "Stepped animation in " + mAppToken + ": more=" + more + ", xform=" + transformation);
    if (!more) {
        mAnimation = NULL;
        if (!mDeferThumbnailDestruction) {
            ClearThumbnail();
        }
        // if (WindowManagerService.DEBUG_ANIM) Slog.v(
        //     TAG, "Finished animation in " + mAppToken + " @ " + currentTime);
    }
    mHasTransformation = more;
    return more;
}

Boolean AppWindowAnimator::StepAnimationLocked(
    /* [in] */ Int64 currentTime)
{
    AutoPtr<AppWindowToken> appToken = GetAppToken();
    if (appToken == NULL) return FALSE;

    if (mService->OkToDisplay()) {
        // We will run animations as long as the display isn't frozen.

        if (mAnimation == sDummyAnimation) {
            // This guy is going to animate, but not yet.  For now count
            // it as not animating for purposes of scheduling transactions;
            // when it is really time to animate, this will be set to
            // a real animation and the next call will execute normally.
            return FALSE;
        }

        if ((appToken->mAllDrawn || mAnimating || appToken->mStartingDisplayed)
                && mAnimation != NULL) {
            if (!mAnimating) {
                // if (WindowManagerService.DEBUG_ANIM) Slog.v(
                //         TAG, "Starting animation in " + mAppToken +
                //         " @ " + currentTime + " scale="
                //         + mService.getTransitionAnimationScaleLocked()
                //         + " allDrawn=" + mAppToken.allDrawn + " animating=" + animating);
                mAnimation->SetStartTime(currentTime);
                mAnimating = TRUE;
                if (mThumbnail != NULL) {
                    mThumbnail->Show();
                    mThumbnailAnimation->SetStartTime(currentTime);
                }
            }
            if (StepAnimation(currentTime)) {
                // animation isn't over, step any thumbnail and that's
                // it for now.
                if (mThumbnail != NULL) {
                    StepThumbnailAnimation(currentTime);
                }
                return TRUE;
            }
        }
    }
    else if (mAnimation != NULL) {
        // If the display is frozen, and there is a pending animation,
        // clear it and make sure we run the cleanup code.
        mAnimating = TRUE;
        mAnimation = NULL;
    }

    mHasTransformation = FALSE;

    if (!mAnimating && mAnimation == NULL) {
        return FALSE;
    }

    mAnimator->SetAppLayoutChanges(this, IWindowManagerPolicy::FINISH_LAYOUT_REDO_ANIM,
            String("AppWindowToken"));

    ClearAnimation();
    mAnimating = FALSE;
    if (mAnimLayerAdjustment != 0) {
        mAnimLayerAdjustment = 0;
        UpdateLayers();
    }
    if (mService->mInputMethodTarget != NULL
            && mService->mInputMethodTarget->mAppToken == appToken) {
        mService->MoveInputMethodWindowsIfNeededLocked(TRUE);
    }

    // if (WindowManagerService.DEBUG_ANIM) Slog.v(
    //         TAG, "Animation done in " + mAppToken
    //         + ": reportedVisible=" + mAppToken.reportedVisible);

    mTransformation->Clear();

    List<AutoPtr<WindowStateAnimator> >::Iterator it = mAllAppWinAnimators.Begin();
    for (; it != mAllAppWinAnimators.End(); ++it) {
        AutoPtr<WindowStateAnimator> winAnim = *it;
        if (appToken->mLaunchTaskBehind) {
            winAnim->mWin->mExiting = TRUE;
        }
        winAnim->FinishExit();
    }
    if (appToken->mLaunchTaskBehind) {
        // try {
        mService->mActivityManager->NotifyLaunchTaskBehindComplete(appToken->mToken);
        // } catch (RemoteException e) {
        // }
        appToken->mLaunchTaskBehind = FALSE;
    }
    else {
        appToken->UpdateReportedVisibilityLocked();
        if (appToken->mEnteringAnimation) {
            appToken->mEnteringAnimation = FALSE;
            // try {
            mService->mActivityManager->NotifyEnterAnimationComplete(appToken->mToken);
            // } catch (RemoteException e) {
            // }
        }
    }

    return FALSE;
}

Boolean AppWindowAnimator::ShowAllWindowsLocked()
{
    Boolean isAnimating = FALSE;
    List<AutoPtr<WindowStateAnimator> >::Iterator it = mAllAppWinAnimators.Begin();
    for (; it != mAllAppWinAnimators.End(); ++it) {
        AutoPtr<WindowStateAnimator> winAnimator = *it;
        // if (WindowManagerService.DEBUG_VISIBILITY) Slog.v(TAG,
        //         "performing show on: " + winAnimator);
        winAnimator->PerformShowLocked();
        isAnimating |= winAnimator->IsAnimating();
    }
    return isAnimating;
}

} // Wm
} // Server
} // Droid
} // Elastos
