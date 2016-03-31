
#include "elastos/droid/server/wm/WindowStateAnimator.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/server/wm/ScreenRotationAnimation.h"
#include "elastos/droid/server/wm/AccessibilityController.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPixelFormat;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::View::Animation::CTransformation;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::CTransformation;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String WindowStateAnimator::TAG("WindowStateAnimator");
const Int32 WindowStateAnimator::BLUR_LAYER_OFFSET = CWindowManagerService::LAYER_OFFSET_BLUR_WITH_MASKING;
const Int32 WindowStateAnimator::NO_SURFACE ;
const Int32 WindowStateAnimator::DRAW_PENDING ;
const Int32 WindowStateAnimator::COMMIT_DRAW_PENDING ;
const Int32 WindowStateAnimator::READY_TO_SHOW ;
const Int32 WindowStateAnimator::HAS_DRAWN ;
const Int32 WindowStateAnimator::SYSTEM_UI_FLAGS_LAYOUT_STABLE_FULLSCREEN;

String WindowStateAnimator::DrawStateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case WindowStateAnimator::NO_SURFACE: return String("NO_SURFACE");
        case WindowStateAnimator::DRAW_PENDING: return String("DRAW_PENDING");
        case WindowStateAnimator::COMMIT_DRAW_PENDING: return String("COMMIT_DRAW_PENDING");
        case WindowStateAnimator::READY_TO_SHOW: return String("READY_TO_SHOW");
        case WindowStateAnimator::HAS_DRAWN: return String("HAS_DRAWN");
        default: return StringUtils::ToString(state);
    }
}

WindowStateAnimator::WindowStateAnimator(
    /* [in] */ WindowState* win)
    : mWin(win)
    , mAnimating(FALSE)
    , mLocalAnimating(FALSE)
    , mAnimationIsEntrance(FALSE)
    , mHasTransformation(FALSE)
    , mHasLocalTransformation(FALSE)
    , mWasAnimating(FALSE)
    , mAnimLayer(0)
    , mLastLayer(0)
    , mFullyTransparent(FALSE)
    , mLayerStack(0)
    , mSurfaceBlurShown(FALSE)
    , mSurfaceBlurScaleNeeded(FALSE)
    , mSurfaceResized(FALSE)
    , mSurfaceDestroyDeferred(FALSE)
    , mShownAlpha(0)
    , mAlpha(0)
    , mLastAlpha(0)
    , mHasClipRect(FALSE)
    , mDsDx(1)
    , mDtDx(0)
    , mDsDy(0)
    , mDtDy(1)
    , mLastDsDx(1)
    , mLastDtDx(0)
    , mLastDsDy(0)
    , mLastDtDy(1)
    , mHaveMatrix(FALSE)
    , mSurfaceShown(FALSE)
    , mSurfaceX(0)
    , mSurfaceY(0)
    , mSurfaceW(0)
    , mSurfaceH(0)
    , mSurfaceLayer(0)
    , mSurfaceAlpha(0)
    , mEnterAnimationPending(FALSE)
    , mKeyguardGoingAwayAnimation(FALSE)
    , mDrawState(0)
    , mLastHidden(FALSE)
{
    CTransformation::New((ITransformation**)&mUniverseTransform);
    CTransformation::New((ITransformation**)&mTransformation);

    CRect::New((IRect**)&mClipRect);
    CRect::New((IRect**)&mTmpClipRect);
    CRect::New((IRect**)&mLastClipRect);

    mService = win->mService;
    mAnimator = win->mService->mAnimator;
    mSession = win->mSession;
    mPolicy = win->mService->mPolicy;
    mContext = win->mService->mContext;

    AutoPtr<DisplayContent> displayContent = win->GetDisplayContent();
    if (displayContent != NULL) {
        AutoPtr<IDisplayInfo> displayInfo = displayContent->GetDisplayInfo();
        displayInfo->GetAppWidth(&mAnimDw);
        displayInfo->GetAppHeight(&mAnimDh);
    }
    else {
        Slogger::W(TAG, "WindowStateAnimator ctor: Display has been removed");
        // This is checked on return and dealt with.
    }

    mAttachedWinAnimator = win->mAttachedWindow == NULL
            ? NULL : win->mAttachedWindow->mWinAnimator;
    mAppAnimator = win->mAppToken == NULL ? NULL : win->mAppToken->mAppAnimator;
    win->mAttrs->GetType(&mAttrType);
    mIsWallpaper = win->mIsWallpaper;
    AutoPtr<IDisplay> display = win->mDisplayContent->GetDisplay();
}

void WindowStateAnimator::SetAnimation(
    /* [in] */ IAnimation* anim)
{
    // if (localLOGV) Slog.v(TAG, "Setting animation in " + this + ": " + anim);
    mAnimating = FALSE;
    mLocalAnimating = FALSE;
    mAnimation = anim;
    mAnimation->RestrictDuration(CWindowManagerService::MAX_ANIMATION_DURATION);
    mAnimation->ScaleCurrentDuration(mService->GetWindowAnimationScaleLocked());
    // Start out animation gone if window is gone, or visible if window is visible.
    mTransformation->Clear();
    mTransformation->SetAlpha(mLastHidden ? 0 : 1);
    mHasLocalTransformation = TRUE;
}

void WindowStateAnimator::ClearAnimation()
{
    if (mAnimation != NULL) {
        mAnimating = TRUE;
        mLocalAnimating = FALSE;
        mAnimation->Cancel();
        mAnimation = NULL;
        mKeyguardGoingAwayAnimation = FALSE;
    }
}

Boolean WindowStateAnimator::IsAnimating()
{
    Boolean isAnimating = mAnimation != NULL
        || (mAttachedWinAnimator != NULL && mAttachedWinAnimator->mAnimation != NULL);

    if (!isAnimating) {
        isAnimating = mAppAnimator != NULL;
        if (isAnimating) {
            isAnimating = mAppAnimator->mAnimation != NULL;
            if (!isAnimating) {
                AutoPtr<AppWindowToken> token = mAppAnimator->GetAppToken();
                if (token) {
                    isAnimating = token->mInPendingTransaction;
                }
            }
        }
    }
    return isAnimating;
}

Boolean WindowStateAnimator::IsDummyAnimation()
{
    return mAppAnimator != NULL
            && mAppAnimator->mAnimation == AppWindowAnimator::sDummyAnimation;
}

Boolean WindowStateAnimator::IsWindowAnimating()
{
    return mAnimation != NULL;
}

void WindowStateAnimator::CancelExitAnimationForNextAnimationLocked()
{
    if (mAnimation != NULL) {
        mAnimation->Cancel();
        mAnimation = NULL;
        mLocalAnimating = FALSE;
        DestroySurfaceLocked();
    }
}

Boolean WindowStateAnimator::StepAnimation(
    /* [in] */ Int64 currentTime)
{
    if ((mAnimation == NULL) || !mLocalAnimating) {
        return FALSE;
    }
    mTransformation->Clear();
    Boolean more;
    mAnimation->GetTransformation(currentTime, mTransformation, &more);
    // if (false && DEBUG_ANIM) Slog.v(
    //     TAG, "Stepped animation in " + this +
    //     ": more=" + more + ", xform=" + mTransformation);
    return more;
}

Boolean WindowStateAnimator::StepAnimationLocked(
    /* [in] */ Int64 currentTime)
{
    // Save the animation state as it was before this step so WindowManagerService can tell if
    // we just started or just stopped animating by comparing mWasAnimating with isAnimating().
    mWasAnimating = mAnimating;
    AutoPtr<DisplayContent> displayContent = mWin->GetDisplayContent();
    if (displayContent != NULL && mService->OkToDisplay()) {
        // We will run animations as long as the display isn't frozen.
        if (mWin->IsDrawnLw() && mAnimation != NULL) {
            mHasTransformation = TRUE;
            mHasLocalTransformation = TRUE;
            if (!mLocalAnimating) {
                // if (DEBUG_ANIM) Slog.v(
                //     TAG, "Starting animation in " + this +
                //     " @ " + currentTime + ": ww=" + mWin.mFrame.width() +
                //     " wh=" + mWin.mFrame.height() +
                //     " dw=" + mAnimDw + " dh=" + mAnimDh +
                //     " scale=" + mService.getWindowAnimationScaleLocked());
                Int32 w, h;
                mWin->mFrame->GetWidth(&w);
                mWin->mFrame->GetHeight(&h);
                mAnimation->Initialize(w, h, mAnimDw, mAnimDh);
                AutoPtr<IDisplayInfo> displayInfo = displayContent->GetDisplayInfo();
                displayInfo->GetAppWidth(&mAnimDw);
                displayInfo->GetAppHeight(&mAnimDh);
                mAnimation->SetStartTime(currentTime);
                mLocalAnimating = TRUE;
                mAnimating = TRUE;
            }
            if ((mAnimation != NULL) && mLocalAnimating) {
                if (StepAnimation(currentTime)) {
                    return TRUE;
                }
            }
            // if (DEBUG_ANIM) Slog.v(
            //     TAG, "Finished animation in " + this +
            //     " @ " + currentTime);
            //WindowManagerService.this.dump();
        }
        mHasLocalTransformation = FALSE;
        if ((!mLocalAnimating || mAnimationIsEntrance) && mAppAnimator != NULL
                && mAppAnimator->mAnimation != NULL) {
            // When our app token is animating, we kind-of pretend like
            // we are as well.  Note the mLocalAnimating mAnimationIsEntrance
            // part of this check means that we will only do this if
            // our window is not currently exiting, or it is not
            // locally animating itself.  The idea being that one that
            // is exiting and doing a local animation should be removed
            // once that animation is done.
            mAnimating = TRUE;
            mHasTransformation = TRUE;
            mTransformation->Clear();
            return FALSE;
        }
        else if (mHasTransformation) {
            // Little trick to get through the path below to act like
            // we have finished an animation.
            mAnimating = TRUE;
        }
        else if (IsAnimating()) {
            mAnimating = TRUE;
        }
    }
    else if (mAnimation != NULL) {
        // If the display is frozen, and there is a pending animation,
        // clear it and make sure we run the cleanup code.
        mAnimating = TRUE;
    }

    if (!mAnimating && !mLocalAnimating) {
        return FALSE;
    }

    // Done animating, clean up.
    // if (DEBUG_ANIM) Slog.v(
    //     TAG, "Animation done in " + this + ": exiting=" + mWin.mExiting
    //     + ", reportedVisible="
    //     + (mWin.mAppToken != null ? mWin.mAppToken.reportedVisible : false));

    mAnimating = FALSE;
    mKeyguardGoingAwayAnimation = FALSE;
    mLocalAnimating = FALSE;
    if (mAnimation != NULL) {
        mAnimation->Cancel();
        mAnimation = NULL;
    }
    if (mAnimator->mWindowDetachedWallpaper.Get() == mWin) {
        mAnimator->mWindowDetachedWallpaper = NULL;
    }
    mAnimLayer = mWin->mLayer;
    if (mWin->mIsImWindow) {
        mAnimLayer += mService->mInputMethodAnimLayerAdjustment;
    }
    else if (mIsWallpaper) {
        mAnimLayer += mService->mWallpaperAnimLayerAdjustment;
    }
    if (CWindowManagerService::DEBUG_LAYERS) Slogger::V(TAG, "Stepping win %p anim layer: %d", this, mAnimLayer);
    mHasTransformation = FALSE;
    mHasLocalTransformation = FALSE;
    if (mWin->mPolicyVisibility != mWin->mPolicyVisibilityAfterAnim) {
        if (CWindowManagerService::DEBUG_VISIBILITY) {
            Slogger::V(TAG, "Policy visibility changing after anim in %p: %d", this, mWin->mPolicyVisibilityAfterAnim);
        }
        mWin->mPolicyVisibility = mWin->mPolicyVisibilityAfterAnim;
        if (displayContent != NULL) {
            displayContent->mLayoutNeeded = TRUE;
        }
        if (!mWin->mPolicyVisibility) {
            if (mService->mCurrentFocus.Get() == mWin) {
                if (CWindowManagerService::DEBUG_FOCUS_LIGHT) Slogger::I(TAG,
                        "setAnimationLocked: setting mFocusMayChange true");
                mService->mFocusMayChange = TRUE;
            }
            // Window is no longer visible -- make sure if we were waiting
            // for it to be displayed before enabling the display, that
            // we allow the display to be enabled now.
            mService->EnableScreenIfNeededLocked();
        }
    }
    mTransformation->Clear();
    Int32 type;
    if (mDrawState == HAS_DRAWN
            && (mWin->mAttrs->GetType(&type), type == IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING)
            && mWin->mAppToken != NULL
            && mWin->mAppToken->mFirstWindowDrawn
            && mWin->mAppToken->mStartingData != NULL) {
        // if (DEBUG_STARTING_WINDOW) Slog.v(TAG, "Finish starting "
        //         + mWin.mToken + ": first real window done animating");
        mService->mFinishedStarting.PushBack(mWin->mAppToken);
        Boolean result;
        mService->mH->SendEmptyMessage(CWindowManagerService::H::FINISHED_STARTING, &result);
    }
    else if (mAttrType == IWindowManagerLayoutParams::TYPE_STATUS_BAR && mWin->mPolicyVisibility) {
        // Upon completion of a not-visible to visible status bar animation a relayout is
        // required.
        if (displayContent != NULL) {
            displayContent->mLayoutNeeded = TRUE;
        }
    }

    FinishExit();
    Int32 displayId = mWin->GetDisplayId();
    mAnimator->SetPendingLayoutChanges(displayId,
            IWindowManagerPolicy::FINISH_LAYOUT_REDO_ANIM);
    // if (WindowManagerService.DEBUG_LAYOUT_REPEATS) mService.debugLayoutRepeats(
    //         "WindowStateAnimator", mAnimator.getPendingLayoutChanges(displayId));

    if (mWin->mAppToken != NULL) {
        mWin->mAppToken->UpdateReportedVisibilityLocked();
    }

    return FALSE;
}

void WindowStateAnimator::FinishExit()
{
    // if (WindowManagerService.DEBUG_ANIM) Slog.v(
    //         TAG, "finishExit in " + this
    //         + ": exiting=" + mWin.mExiting
    //         + " remove=" + mWin.mRemoveOnExit
    //         + " windowAnimating=" + isWindowAnimating());

    List<AutoPtr<WindowState> >::Iterator it = mWin->mChildWindows.Begin();
    for (; it != mWin->mChildWindows.End(); ++it) {
        (*it)->mWinAnimator->FinishExit();
    }

    if (!mWin->mExiting) {
        return;
    }

    if (IsWindowAnimating()) {
        return;
    }

    // if (WindowManagerService.localLOGV) Slog.v(
    //         TAG, "Exit animation finished in " + this
    //         + ": remove=" + mWin.mRemoveOnExit);
    if (mSurfaceControl != NULL) {
        mService->mDestroySurface.PushBack(mWin);
        mWin->mDestroying = TRUE;
        // if (SHOW_TRANSACTIONS) WindowManagerService.logSurface(
        //     mWin, "HIDE (finishExit)", null);
        Hide();
    }
    mWin->mExiting = FALSE;
    if (mWin->mRemoveOnExit) {
        mService->mPendingRemove.PushBack(mWin);
        mWin->mRemoveOnExit = FALSE;
    }
    mAnimator->HideWallpapersLocked(mWin);
}

void WindowStateAnimator::Hide()
{
    if (!mLastHidden) {
        //dump();
        mLastHidden = TRUE;
        // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(mWin,
        //         "HIDE (performLayout)", null);
        if (mSurfaceControl != NULL) {
            mSurfaceShown = FALSE;
            // try {
            if (FAILED(mSurfaceControl->Hide())) {
                Slogger::W(TAG, "Exception hiding surface in %s", TO_CSTR(mWin));
            }
            // } catch (RuntimeException e) {
            //     Slog.w(TAG, "Exception hiding surface in " + mWin);
            // }
            if (mSurfaceControlBlur != NULL) {
                // try {
                if (FAILED(mSurfaceControlBlur->Hide())) {
                    Slogger::W(TAG, "Exception hiding surface blur in %s", TO_CSTR(mWin));
                }
                // }
                // catch (RuntimeException e) {
                //     Slog.w(TAG, "Exception hiding surface blur in " + mWin);
                // }
            }
        }
    }
}

Boolean WindowStateAnimator::FinishDrawingLocked()
{
    // if (DEBUG_STARTING_WINDOW &&
    //         mWin.mAttrs.type == WindowManager.LayoutParams.TYPE_APPLICATION_STARTING) {
    //     Slog.v(TAG, "Finishing drawing window " + mWin + ": mDrawState="
    //             + drawStateToString(mDrawState));
    // }
    if (mDrawState == DRAW_PENDING) {
        // if (DEBUG_SURFACE_TRACE || DEBUG_ANIM || SHOW_TRANSACTIONS || DEBUG_ORIENTATION)
        //     Slog.v(TAG, "finishDrawingLocked: mDrawState=COMMIT_DRAW_PENDING " + this + " in "
        //             + mSurfaceControl);
        // if (DEBUG_STARTING_WINDOW &&
        //         mWin.mAttrs.type == WindowManager.LayoutParams.TYPE_APPLICATION_STARTING) {
        //     Slog.v(TAG, "Draw state now committed in " + mWin);
        // }
        mDrawState = COMMIT_DRAW_PENDING;
        return TRUE;
    }
    return FALSE;
}

Boolean WindowStateAnimator::CommitFinishDrawingLocked(
    /* [in] */ Int64 currentTime)
{
    // if (DEBUG_STARTING_WINDOW &&
    //         mWin.mAttrs.type == WindowManager.LayoutParams.TYPE_APPLICATION_STARTING) {
    //     Slog.i(TAG, "commitFinishDrawingLocked: " + mWin + " cur mDrawState="
    //             + drawStateToString(mDrawState));
    // }
    if (mDrawState != COMMIT_DRAW_PENDING) {
        return FALSE;
    }
    // if (DEBUG_SURFACE_TRACE || DEBUG_ANIM) {
    //     Slog.i(TAG, "commitFinishDrawingLocked: mDrawState=READY_TO_SHOW " + mSurfaceControl);
    // }
    mDrawState = READY_TO_SHOW;
    Int32 type;
    mWin->mAttrs->GetType(&type);
    Boolean starting = type == IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING;
    AutoPtr<AppWindowToken> atoken = mWin->mAppToken;
    if (atoken == NULL || atoken->mAllDrawn || starting) {
        PerformShowLocked();
    }
    return TRUE;
}

AutoPtr<ISurfaceControl> WindowStateAnimator::CreateSurfaceLocked()
{
    AutoPtr<WindowState> w = mWin;
    if (mSurfaceControl == NULL) {
        if (CWindowManagerService::DEBUG_ANIM || CWindowManagerService::DEBUG_ORIENTATION) {
            Slogger::I(TAG, "createSurface %p: mDrawState=DRAW_PENDING", this);
        }
        mDrawState = DRAW_PENDING;
        if (w->mAppToken != NULL) {
            if (w->mAppToken->mAppAnimator->mAnimation == NULL) {
                w->mAppToken->mAllDrawn = FALSE;
                w->mAppToken->mDeferClearAllDrawn = FALSE;
            }
            else {
                // Currently animating, persist current state of allDrawn until animation
                // is complete.
                w->mAppToken->mDeferClearAllDrawn = TRUE;
            }
        }

        mService->MakeWindowFreezingScreenIfNeededLocked(w);

        Int32 flags = ISurfaceControl::HIDDEN;
        AutoPtr<IWindowManagerLayoutParams> attrs = w->mAttrs;

        Int32 attrsflags;
        if (attrs->GetFlags(&attrsflags), (attrsflags & IWindowManagerLayoutParams::FLAG_SECURE) != 0) {
            flags |= ISurfaceControl::SECURE;
        }

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId;
        helper->GetUserId(mWin->mOwnerUid, &userId);
        if (mService->IsScreenCaptureDisabledLocked(userId)) {
            flags |= ISurfaceControl::SECURE;
        }

        Int32 width;
        Int32 height;
        if (attrs->GetFlags(&attrsflags), (attrsflags & IWindowManagerLayoutParams::FLAG_SCALED) != 0) {
            // for a scaled surface, we always want the requested
            // size.
            width = w->mRequestedWidth;
            height = w->mRequestedHeight;
        }
        else {
            w->mCompatFrame->GetWidth(&width);
            w->mCompatFrame->GetHeight(&height);
        }

        // Something is wrong and SurfaceFlinger will not like this,
        // try to revert to sane values
        if (width <= 0) {
            width = 1;
        }
        if (height <= 0) {
            height = 1;
        }

        Int32 fl, ft;
        w->mFrame->GetLeft(&fl);
        w->mFrame->GetTop(&ft);
        Float left = fl + w->mXOffset;
        Float top = ft + w->mYOffset;

        // Adjust for surface insets.
        AutoPtr<IRect> surfaceInsets;
        attrs->GetSurfaceInsets((IRect**)&surfaceInsets);
        Int32 sil, sir, sit, sib;
        surfaceInsets->Get(&sil, &sit, &sir, &sib);
        width += sil + sir;
        height += sit + sib;
        left -= sil;
        top -= sit;

        if (CWindowManagerService::DEBUG_VISIBILITY) {
            Int32 format;
            attrs->GetFormat(&format);
            Slogger::V(TAG, "Creating surface in session %p window %p w=%d h=%d x=%d y=%d format=%d flags=%d"
                    , mSession->mSurfaceSession.Get(), this, width, height
                    , left, top, format, flags);
        }

        // We may abort, so initialize to defaults.
        mSurfaceShown = FALSE;
        mSurfaceLayer = 0;
        mSurfaceAlpha = 0;
        mSurfaceX = 0;
        mSurfaceY = 0;
        w->mLastSystemDecorRect->Set(0, 0, 0, 0);
        mLastClipRect->Set(0, 0, 0, 0);

        // Set up surface control with initial size.
        // try {
        mSurfaceW = width;
        mSurfaceH = height;

        attrs->GetFlags(&attrsflags);
        Boolean isHwAccelerated = (attrsflags &
                IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED) != 0;
        Int32 format = IPixelFormat::TRANSLUCENT;
        if (!isHwAccelerated) {
            attrs->GetFormat(&format);
        }
        //actions_code(ywwang, import starting window rendering)
        Int32 attrsformat, attrstype;
        if((attrs->GetFormat(&attrsformat), attrsformat == IPixelFormat::RGB_565) &&
            (attrs->GetType(&attrstype), attrstype == IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING)) {
                format = attrsformat;
        }
        AutoPtr<IPixelFormat> pf;
        CPixelFormat::AcquireSingleton((IPixelFormat**)&pf);
        attrs->GetFormat(&attrsformat);
        Boolean hasAlpha;
        if ((pf->FormatHasAlpha(attrsformat, &hasAlpha), !hasAlpha)
                && (surfaceInsets->GetLeft(&sil), sil == 0)
                && (surfaceInsets->GetTop(&sit), sit == 0)
                && (surfaceInsets->GetRight(&sir), sir == 0)
                && (surfaceInsets->GetBottom(&sib), sib == 0)) {
            flags |= ISurfaceControl::OPAQUE;
        }

        // if (DEBUG_SURFACE_TRACE) {
        //     mSurfaceControl = new SurfaceTrace(
        //             mSession.mSurfaceSession,
        //             attrs.getTitle().toString(),
        //             width, height, format, flags);
        // } else {
        AutoPtr<ICharSequence> title;
        attrs->GetTitle((ICharSequence**)&title);
        String titleStr;
        title->ToString(&titleStr);
        mSurfaceControl = NULL;
        CSurfaceControl::New(mSession->mSurfaceSession, titleStr, width, height,
                format, flags, (ISurfaceControl**)&mSurfaceControl);
        // }

        w->mHasSurface = TRUE;

        if (CWindowManagerService::SHOW_TRANSACTIONS || CWindowManagerService::SHOW_SURFACE_ALLOC) {
            Slogger::I(TAG, "  CREATE SURFACE %p IN SESSION %p: pid=%d format=%d flags=0x%d / %p",
                    mSurfaceControl.Get(), mSession->mSurfaceSession.Get() , mSession->mPid, attrsformat, flags, this);
        }
        // } catch (OutOfResourcesException e) {
        //     w.mHasSurface = false;
        //     Slog.w(TAG, "OutOfResourcesException creating surface");
        //     mService.reclaimSomeSurfaceMemoryLocked(this, "create", true);
        //     mDrawState = NO_SURFACE;
        //     return null;
        // } catch (Exception e) {
        //     w.mHasSurface = false;
        //     Slog.e(TAG, "Exception creating surface", e);
        //     mDrawState = NO_SURFACE;
        //     return null;
        // }

        // if (WindowManagerService::localLOGV) {
        //     Slog.v(TAG, "Got surface: " + mSurfaceControl
        //             + ", set left=" + w.mFrame.left + " top=" + w.mFrame.top
        //             + ", animLayer=" + mAnimLayer);
        // }

        // if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) {
        //     Slog.i(TAG, ">>> OPEN TRANSACTION createSurfaceLocked");
        //     WindowManagerService.logSurface(w, "CREATE pos=("
        //             + w.mFrame.left + "," + w.mFrame.top + ") ("
        //             + w.mCompatFrame.width() + "x" + w.mCompatFrame.height()
        //             + "), layer=" + mAnimLayer + " HIDE", null);
        // }

        // Start a new transaction and apply position & offset.
        AutoPtr<ISurfaceControlHelper> sch;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&sch);
        sch->OpenTransaction();
        // try {
        mSurfaceX = left;
        mSurfaceY = top;

        // try {
        mSurfaceControl->SetPosition(left, top);
        mSurfaceLayer = mAnimLayer;
        AutoPtr<DisplayContent> displayContent = w->GetDisplayContent();
        if (displayContent != NULL) {
            displayContent->GetDisplay()->GetLayerStack(&mLayerStack);
            mSurfaceControl->SetLayerStack(mLayerStack);
        }
        mSurfaceControl->SetLayer(mAnimLayer);
        mSurfaceControl->SetAlpha(0);
        mSurfaceShown = FALSE;
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Error creating surface in " + w, e);
        //     mService.reclaimSomeSurfaceMemoryLocked(this, "create-init", true);
        // }
        mLastHidden = TRUE;
        // } finally {
        //     SurfaceControl.closeTransaction();
        //     if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slog.i(TAG,
        //             "<<< CLOSE TRANSACTION createSurfaceLocked");
        // }
        sch->CloseTransaction();
        if (CWindowManagerService::localLOGV) Slogger::V(
                TAG, "Created surface %p", this);
        UpdateBlurWithMaskingState(attrs, FALSE);
    }
    return mSurfaceControl;
}

void WindowStateAnimator::DestroySurfaceLocked()
{
    if (mWin->mAppToken != NULL && mWin == mWin->mAppToken->mStartingWindow) {
        mWin->mAppToken->mStartingDisplayed = FALSE;
    }

    if (mSurfaceControl != NULL) {
        List<AutoPtr<WindowState> >::ReverseIterator rit = mWin->mChildWindows.RBegin();
        while (rit != mWin->mChildWindows.REnd()) {
            AutoPtr<WindowState> c = *rit;
            c->mAttachedHidden = TRUE;
            ++rit;
        }

        // try {
        // if (DEBUG_VISIBILITY) {
        //     RuntimeException e = null;
        //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
        //         e = new RuntimeException();
        //         e.fillInStackTrace();
        //     }
        //     Slog.w(TAG, "Window " + this + " destroying surface "
        //             + mSurfaceControl + ", session " + mSession, e);
        // }
        if (mSurfaceDestroyDeferred) {
            if (mSurfaceControl != NULL && mPendingDestroySurface != mSurfaceControl) {
                if (mPendingDestroySurface != NULL) {
                    // if (SHOW_TRANSACTIONS || SHOW_SURFACE_ALLOC) {
                    //     RuntimeException e = null;
                    //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
                    //         e = new RuntimeException();
                    //         e.fillInStackTrace();
                    //     }
                    //     WindowManagerService.logSurface(mWin, "DESTROY PENDING", e);
                    // }
                    ECode ec = mPendingDestroySurface->Destroy();
                    if (FAILED(ec)) {
                        Slogger::W(TAG,
                                "Exception thrown when destroying Window %s surface %s session %s : 0x%08x"
                                , this, mSurfaceControl.Get(), mSession.Get(), ec);
                        goto fail;
                    }
                }
                mPendingDestroySurface = mSurfaceControl;
            }

            if (mSurfaceControlBlur != NULL && mPendingDestroySurfaceBlur != mSurfaceControlBlur) {
                if (mPendingDestroySurfaceBlur != NULL) {
                    ECode ec = mPendingDestroySurfaceBlur->Destroy();
                    if (FAILED(ec)) {
                        Slogger::W(TAG,
                                "Exception thrown when destroying Window %s surface %s session %s: 0x%08x"
                                , TO_CSTR(this), TO_CSTR(mSurfaceControl), TO_CSTR(mSession), ec);
                        goto fail;
                    }
                }
                mPendingDestroySurfaceBlur = mSurfaceControlBlur;
            }
        }
        else {
            // if (SHOW_TRANSACTIONS || SHOW_SURFACE_ALLOC) {
            //     RuntimeException e = null;
            //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
            //         e = new RuntimeException();
            //         e.fillInStackTrace();
            //     }
            //     WindowManagerService.logSurface(mWin, "DESTROY", e);
            // }
            ECode ec = mSurfaceControl->Destroy();
            if (FAILED(ec)) {
                Slogger::W(TAG,
                        "Exception thrown when destroying Window %s surface %s session %s : 0x%08x"
                        , TO_CSTR(this), TO_CSTR(mSurfaceControl), TO_CSTR(mSession), ec);
                goto fail;
            }
            if (mSurfaceControlBlur != NULL) {
                ec = mSurfaceControlBlur->Destroy();
                if (FAILED(ec)) {
                    Slogger::W(TAG,
                            "Exception thrown when destroying Window %s surface %s session %s : 0x%08x"
                            , TO_CSTR(this), TO_CSTR(mSurfaceControl), TO_CSTR(mSession), ec);
                    goto fail;
                }
            }
        }
        mAnimator->HideWallpapersLocked(mWin);
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Exception thrown when destroying Window " + this
        //         + " surface " + mSurface + " session " + mSession
        //         + ": " + e.toString());
        // }

fail:
        mSurfaceShown = FALSE;
        mSurfaceControl = NULL;
        mSurfaceControlBlur = NULL;
        mWin->mHasSurface = FALSE;
        mDrawState = NO_SURFACE;
    }

    // Destroy any deferred thumbnail surfaces
    if (mAppAnimator != NULL) {
        mAppAnimator->ClearDeferredThumbnail();
    }
}

void WindowStateAnimator::DestroyDeferredSurfaceLocked()
{
    // try {
    if (mPendingDestroySurface != NULL) {
        // if (SHOW_TRANSACTIONS || SHOW_SURFACE_ALLOC) {
        //     RuntimeException e = null;
        //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
        //         e = new RuntimeException();
        //         e.fillInStackTrace();
        //     }
        //     WindowManagerService.logSurface(mWin, "DESTROY PENDING", e);
        // }
        mPendingDestroySurface->Destroy();
        if (mPendingDestroySurfaceBlur != NULL) {
            mPendingDestroySurfaceBlur->Destroy();
        }
        mAnimator->HideWallpapersLocked(mWin);
    }
    // } catch (RuntimeException e) {
    //     Slog.w(TAG, "Exception thrown when destroying Window "
    //             + this + " surface " + mPendingDestroySurface
    //             + " session " + mSession + ": " + e.toString());
    // }
    mSurfaceDestroyDeferred = FALSE;
    mPendingDestroySurface = NULL;
    mPendingDestroySurfaceBlur = NULL;
}

void WindowStateAnimator::ComputeShownFrameLocked()
{
    Boolean selfTransformation = mHasLocalTransformation;
    AutoPtr<ITransformation> attachedTransformation =
            (mAttachedWinAnimator != NULL && mAttachedWinAnimator->mHasLocalTransformation)
            ? mAttachedWinAnimator->mTransformation : NULL;
    AutoPtr<ITransformation> appTransformation = (mAppAnimator != NULL && mAppAnimator->mHasTransformation)
            ? mAppAnimator->mTransformation : NULL;

    // Wallpapers are animated based on the "real" window they
    // are currently targeting.
    AutoPtr<WindowState> wallpaperTarget = mService->mWallpaperTarget;
    if (mIsWallpaper && wallpaperTarget != NULL && mService->mAnimateWallpaperWithTarget) {
        AutoPtr<WindowStateAnimator> wallpaperAnimator = wallpaperTarget->mWinAnimator;
        Boolean result;
        if (wallpaperAnimator->mHasLocalTransformation
                && wallpaperAnimator->mAnimation != NULL
                && (wallpaperAnimator->mAnimation->GetDetachWallpaper(&result), !result)) {
                attachedTransformation = wallpaperAnimator->mTransformation;
                // if (WindowManagerService.DEBUG_WALLPAPER && attachedTransformation != null) {
                //     Slog.v(TAG, "WP target attached xform: " + attachedTransformation);
        }
        AutoPtr<AppWindowAnimator> wpAppAnimator = wallpaperTarget->mAppToken == NULL ?
                    NULL : wallpaperTarget->mAppToken->mAppAnimator;
        if (wpAppAnimator != NULL && wpAppAnimator->mHasTransformation
                && wpAppAnimator->mAnimation != NULL
                && (wpAppAnimator->mAnimation->GetDetachWallpaper(&result), !result)) {
                appTransformation = wpAppAnimator->mTransformation;
                // if (WindowManagerService.DEBUG_WALLPAPER && appTransformation != null) {
                //     Slog.v(TAG, "WP target app xform: " + appTransformation);
                // }
        }
    }

    Int32 displayId = mWin->GetDisplayId();
    AutoPtr<ScreenRotationAnimation> screenRotationAnimation =
            mAnimator->GetScreenRotationAnimationLocked(displayId);
    Boolean screenAnimation =
            screenRotationAnimation != NULL && screenRotationAnimation->IsAnimating();
    if (selfTransformation || attachedTransformation != NULL
            || appTransformation != NULL || screenAnimation) {
        // cache often used attributes locally
        AutoPtr<IRect> frame = mWin->mFrame;
        AutoPtr<ArrayOf<Float> > tmpFloats = mService->mTmpFloats;
        AutoPtr<IMatrix> tmpMatrix = mWin->mTmpMatrix;

        Boolean screenRotating =
            screenRotationAnimation != NULL && screenRotationAnimation->IsRotating();
        // Compute the desired transformation.
        if (screenAnimation && screenRotating) {
            // If we are doing a screen animation, the global rotation
            // applied to windows can result in windows that are carefully
            // aligned with each other to slightly separate, allowing you
            // to see what is behind them.  An unsightly mess.  This...
            // thing...  magically makes it call good: scale each window
            // slightly (two pixels larger in each dimension, from the
            // window's center).
            Int32 w, h;
            frame->GetWidth(&w);
            frame->GetHeight(&h);
            if (w >= 1 && h >= 1) {
                tmpMatrix->SetScale(1 + 2/w, 1 + 2/h, w/2, h/2);
            }
            else {
                tmpMatrix->Reset();
            }
        }
        else {
            tmpMatrix->Reset();
        }
        tmpMatrix->SetScale(mWin->mGlobalScale, mWin->mGlobalScale);
        AutoPtr<IMatrix> m;
        if (selfTransformation) {
            mTransformation->GetMatrix((IMatrix**)&m);
            Boolean r;
            tmpMatrix->PostConcat(m, &r);
        }
        Int32 left, top;
        frame->GetLeft(&left);
        frame->GetTop(&top);
        Boolean result;
        tmpMatrix->PostTranslate(left + mWin->mXOffset, top + mWin->mYOffset, &result);
        if (attachedTransformation != NULL) {
            attachedTransformation->GetMatrix((IMatrix**)&m);
            tmpMatrix->PostConcat(m, &result);
        }
        if (appTransformation != NULL) {
            m = NULL;
            appTransformation->GetMatrix((IMatrix**)&m);
            tmpMatrix->PostConcat(m, &result);
        }
        if (mAnimator->mUniverseBackground != NULL) {
            m = NULL;
            mAnimator->mUniverseBackground->mUniverseTransform->GetMatrix((IMatrix**)&m);
            tmpMatrix->PostConcat(m, &result);
        }
        if (screenAnimation) {
            m = NULL;
            screenRotationAnimation->GetEnterTransformation()->GetMatrix((IMatrix**)&m);
            tmpMatrix->PostConcat(m, &result);
        }
        //TODO (multidisplay): Magnification is supported only for the default display.
        if (mService->mAccessibilityController != NULL && displayId == IDisplay::DEFAULT_DISPLAY) {
            AutoPtr<IMagnificationSpec> spec =
                    mService->mAccessibilityController->GetMagnificationSpecForWindowLocked(mWin);
            Boolean isNop;
            if (spec != NULL && (spec->IsNop(&isNop), !isNop)) {
                Boolean result;
                Float scale, offsetX, offsetY;
                spec->GetScale(&scale);
                spec->GetOffsetX(&offsetX);
                spec->GetOffsetY(&offsetY);
                tmpMatrix->PostScale(scale, scale, &result);
                tmpMatrix->PostTranslate(offsetX, offsetY, &result);
            }
        }

        // "convert" it into SurfaceFlinger's format
        // (a 2x2 matrix + an offset)
        // Here we must not transform the position of the surface
        // since it is already included in the transformation.
        //Slog.i(TAG, "Transform: " + matrix);

        mHaveMatrix = TRUE;
        tmpMatrix->GetValues(tmpFloats.Get());
        mDsDx = (*tmpFloats)[IMatrix::MSCALE_X];
        mDtDx = (*tmpFloats)[IMatrix::MSKEW_Y];
        mDsDy = (*tmpFloats)[IMatrix::MSKEW_X];
        mDtDy = (*tmpFloats)[IMatrix::MSCALE_Y];
        Float x = (*tmpFloats)[IMatrix::MTRANS_X];
        Float y = (*tmpFloats)[IMatrix::MTRANS_Y];
        Int32 w, h;
        frame->GetWidth(&w);
        frame->GetHeight(&h);
        mWin->mShownFrame->Set(x, y, x+w, y+h);

        // Now set the alpha...  but because our current hardware
        // can't do alpha transformation on a non-opaque surface,
        // turn it off if we are running an animation that is also
        // transforming since it is more important to have that
        // animation be smooth.
        mShownAlpha = mAlpha;

        AutoPtr<IPixelFormat> pixelFormat;
        CPixelFormat::AcquireSingleton((IPixelFormat**)&pixelFormat);
        Int32 format;
        mWin->mAttrs->GetFormat(&format);
        pixelFormat->FormatHasAlpha(format, &result);
        if (!mService->mLimitedAlphaCompositing
                || (!result || (mWin->IsIdentityMatrix(mDsDx, mDtDx, mDsDy, mDtDy)
                        && x == left && y == top))) {
            //Slog.i(TAG, "Applying alpha transform");
            Float alpha;
            if (selfTransformation) {
                mTransformation->GetAlpha(&alpha);
                mShownAlpha *= alpha;
            }
            if (attachedTransformation != NULL) {
                attachedTransformation->GetAlpha(&alpha);
                mShownAlpha *= alpha;
            }
            if (appTransformation != NULL) {
                appTransformation->GetAlpha(&alpha);
                mShownAlpha *= alpha;
                Boolean hasClipRect;
                if (appTransformation->HasClipRect(&hasClipRect), hasClipRect) {
                    AutoPtr<IRect> clipRect;
                    appTransformation->GetClipRect((IRect**)&clipRect);
                    mClipRect->Set(clipRect);
                    if (mWin->mHScale > 0) {
                        Int32 crl, crr;
                        mClipRect->GetLeft(&crl);
                        mClipRect->GetRight(&crr);
                        mClipRect->SetLeft(crl / mWin->mHScale);
                        mClipRect->SetRight(crr / mWin->mHScale);
                    }
                    if (mWin->mVScale > 0) {
                        Int32 crt, crb;
                        mClipRect->GetTop(&crt);
                        mClipRect->GetBottom(&crb);
                        mClipRect->SetTop(crt / mWin->mVScale);
                        mClipRect->SetBottom(crb / mWin->mVScale);
                    }
                    mHasClipRect = TRUE;
                }
            }
            if (mAnimator->mUniverseBackground != NULL) {
                mAnimator->mUniverseBackground->mUniverseTransform->GetAlpha(&alpha);
                mShownAlpha *= alpha;
            }
            if (screenAnimation) {
                screenRotationAnimation->GetEnterTransformation()->GetAlpha(&alpha);
                mShownAlpha *= alpha;
            }
        }
        else {
            // Slog.i(TAG, "Not applying alpha transform");
        }

        // if ((DEBUG_SURFACE_TRACE || WindowManagerService.localLOGV)
        //         && (mShownAlpha == 1.0 || mShownAlpha == 0.0)) Slog.v(
        //         TAG, "computeShownFrameLocked: Animating " + this + " mAlpha=" + mAlpha
        //         + " self=" + (selfTransformation ? mTransformation.getAlpha() : "null")
        //         + " attached=" + (attachedTransformation == null ?
        //                 "null" : attachedTransformation.getAlpha())
        //         + " app=" + (appTransformation == null ? "null" : appTransformation.getAlpha())
        //         + " screen=" + (screenAnimation ?
        //                 screenRotationAnimation.getEnterTransformation().getAlpha() : "null"));
        return;
    }
    else if (mIsWallpaper && mService->mInnerFields->mWallpaperActionPending) {
        return;
    }

    // if (WindowManagerService.localLOGV) Slog.v(
    //         TAG, "computeShownFrameLocked: " + this +
    //         " not attached, mAlpha=" + mAlpha);

    Int32 type;
    Boolean applyUniverseTransformation = (mAnimator->mUniverseBackground != NULL
            && (mWin->mAttrs->GetType(&type), type != IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND)
            && mWin->mBaseLayer < mAnimator->mAboveUniverseLayer);
    AutoPtr<IMagnificationSpec> spec;
    //TODO (multidisplay): Magnification is supported only for the default display.
    if (mService->mAccessibilityController != NULL && displayId == IDisplay::DEFAULT_DISPLAY) {
        spec = mService->mAccessibilityController->GetMagnificationSpecForWindowLocked(mWin);
    }
    if (applyUniverseTransformation || spec != NULL) {
        AutoPtr<IRect> frame = mWin->mFrame;
        AutoPtr<ArrayOf<Float> > tmpFloats = mService->mTmpFloats;
        AutoPtr<IMatrix> tmpMatrix = mWin->mTmpMatrix;

        tmpMatrix->SetScale(mWin->mGlobalScale, mWin->mGlobalScale);
        Int32 left, top;
        frame->GetLeft(&left);
        frame->GetTop(&top);
        Boolean result;
        tmpMatrix->PostTranslate(left + mWin->mXOffset, top + mWin->mYOffset, &result);

        if (applyUniverseTransformation) {
            AutoPtr<IMatrix> m;
            mAnimator->mUniverseBackground->mUniverseTransform->GetMatrix((IMatrix**)&m);
            tmpMatrix->PostConcat(m, &result);
        }

        Boolean isNop;
        if (spec != NULL && (spec->IsNop(&isNop), !isNop)) {
            Boolean result;
            Float scale, offsetX, offsetY;
            spec->GetScale(&scale);
            spec->GetOffsetX(&offsetX);
            spec->GetOffsetY(&offsetY);
            tmpMatrix->PostScale(scale, scale, &result);
            tmpMatrix->PostTranslate(offsetX, offsetY, &result);
        }

        tmpMatrix->GetValues(tmpFloats.Get());

        mHaveMatrix = TRUE;
        mDsDx = (*tmpFloats)[IMatrix::MSCALE_X];
        mDtDx = (*tmpFloats)[IMatrix::MSKEW_Y];
        mDsDy = (*tmpFloats)[IMatrix::MSKEW_X];
        mDtDy = (*tmpFloats)[IMatrix::MSCALE_Y];
        Float x = (*tmpFloats)[IMatrix::MTRANS_X];
        Float y = (*tmpFloats)[IMatrix::MTRANS_Y];
        Int32 w, h;
        frame->GetWidth(&w);
        frame->GetHeight(&h);
        mWin->mShownFrame->Set(x, y, x + w, y + h);

        mShownAlpha = mAlpha;
        if (applyUniverseTransformation) {
            Float alpha;
            mAnimator->mUniverseBackground->mUniverseTransform->GetAlpha(&alpha);
            mShownAlpha *= alpha;
        }
    }
    else {
        mWin->mShownFrame->Set(mWin->mFrame);
        if (mWin->mXOffset != 0 || mWin->mYOffset != 0) {
            mWin->mShownFrame->Offset(mWin->mXOffset, mWin->mYOffset);
        }
        mShownAlpha = mAlpha;
        mHaveMatrix = FALSE;
        mDsDx = mWin->mGlobalScale;
        mDtDx = 0;
        mDsDy = 0;
        mDtDy = mWin->mGlobalScale;
    }
}

void WindowStateAnimator::ApplyDecorRect(
    /* [in] */ IRect* decorRect)
{
    AutoPtr<WindowState> w = mWin;
    Int32 width, height;
    w->mFrame->GetWidth(&width);
    w->mFrame->GetHeight(&height);

    // Compute the offset of the window in relation to the decor rect.
    Int32 fl, ft;
    w->mFrame->GetLeft(&fl);
    w->mFrame->GetTop(&ft);
    Int32 left = w->mXOffset + fl;
    Int32 top = w->mYOffset + ft;

    // Initialize the decor rect to the entire frame.
    w->mSystemDecorRect->Set(0, 0, width, height);

    // Intersect with the decor rect, offsetted by window position.
    Int32 dl, dt, dr, db;
    decorRect->Get(&dl, &dt, &dr, &db);
    Boolean result;
    w->mSystemDecorRect->Intersect(dl - left, dt - top,
            dr - left, db - top, &result);

    // If size compatibility is being applied to the window, the
    // surface is scaled relative to the screen.  Also apply this
    // scaling to the crop rect.  We aren't using the standard rect
    // scale function because we want to round things to make the crop
    // always round to a larger rect to ensure we don't crop too
    // much and hide part of the window that should be seen.
    if (w->mEnforceSizeCompat && w->mInvGlobalScale != 1.0f) {
        Float scale = w->mInvGlobalScale;
        Int32 l, t, r, b;
        w->mSystemDecorRect->Get(&l, &t, &r, &b);
        w->mSystemDecorRect->Set((Int32) (l * scale - 0.5f), (Int32) (t * scale - 0.5f),
            (Int32) ((r + 1) * scale - 0.5f), (Int32) ((+1) * scale - 0.5f));
    }
}

void WindowStateAnimator::UpdateSurfaceWindowCrop(
    /* [in] */ Boolean recoveringMemory)
{
    AutoPtr<WindowState> w = mWin;
    AutoPtr<DisplayContent> displayContent = w->GetDisplayContent();
    if (displayContent == NULL) {
        return;
    }

    // Need to recompute a new system decor rect each time.
    Int32 flags;
    w->mAttrs->GetFlags(&flags);
    Boolean isDisplay, isEmpty;
    Int32 type;
    if ((flags & IWindowManagerLayoutParams::FLAG_SCALED) != 0) {
        // Currently can't do this cropping for scaled windows.  We'll
        // just keep the crop rect the same as the source surface.
        w->mSystemDecorRect->Set(0, 0, w->mRequestedWidth, w->mRequestedHeight);
    }
    else if (w->IsDefaultDisplay(&isDisplay), !isDisplay) {
        // On a different display there is no system decor.  Crop the window
        // by the screen boundaries.
        AutoPtr<IDisplayInfo> displayInfo = displayContent->GetDisplayInfo();
        Int32 width, height, left, top, logicalW, logicalH;
        w->mCompatFrame->GetWidth(&width);
        w->mCompatFrame->GetHeight(&height);
        w->mCompatFrame->GetLeft(&left);
        w->mCompatFrame->GetTop(&top);

        w->mSystemDecorRect->Set(0, 0, width, height);
        displayInfo->GetLogicalWidth(&logicalW);
        displayInfo->GetLogicalHeight(&logicalH);
        Boolean result;
        w->mSystemDecorRect->Intersect(-left, -top,
                logicalW - left, logicalH - top, &result);
    }
    else if (w->mLayer >= mService->mSystemDecorLayer) {
        // Above the decor layer is easy, just use the entire window.
        // Unless we have a universe background...  in which case all the
        // windows need to be cropped by the screen, so they don't cover
        // the universe background.
        if (mAnimator->mUniverseBackground == NULL) {
            Int32 cfw, cfh;
            w->mCompatFrame->GetWidth(&cfw);
            w->mCompatFrame->GetHeight(&cfh);
            w->mSystemDecorRect->Set(0, 0, cfw, cfh);
        }
        else {
            ApplyDecorRect(mService->mScreenRect);
        }
    }
    else if ((w->mAttrs->GetType(&type), type == IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND)
            || (w->mDecorFrame->IsEmpty(&isEmpty), isEmpty)) {
        // The universe background isn't cropped, nor windows without policy decor.
        Int32 cfw, cfh;
        w->mCompatFrame->GetWidth(&cfw);
        w->mCompatFrame->GetHeight(&cfh);
        w->mSystemDecorRect->Set(0, 0, cfw, cfh);
    }
    else if ((w->mAttrs->GetType(&type), type == IWindowManagerLayoutParams::TYPE_WALLPAPER)
            && mAnimator->mAnimating) {
        // If we're animating, the wallpaper crop should only be updated at the end of the
        // animation.
        mTmpClipRect->Set(w->mSystemDecorRect);
        ApplyDecorRect(w->mDecorFrame);
        w->mSystemDecorRect->Union(mTmpClipRect);
    }
    else {
        // Crop to the system decor specified by policy.
        ApplyDecorRect(w->mDecorFrame);
    }

    // By default, the clip rect is the system decor.
    AutoPtr<IRect> clipRect = mTmpClipRect;
    clipRect->Set(w->mSystemDecorRect);

    // Expand the clip rect for surface insets.
    AutoPtr<IWindowManagerLayoutParams> attrs = w->mAttrs;
    AutoPtr<IRect> surfaceInsets;
    attrs->GetSurfaceInsets((IRect**)&surfaceInsets);
    Int32 crl, crt, crr, crb, sil, sit, sir, sib;
    surfaceInsets->Get(&sil, &sit, &sir, &sib);
    clipRect->Get(&crl, &crt, &crr, &crb);
    clipRect->Set(crl - sil, crt - sit, crr - sir, crb - sib);

    // If we have an animated clip rect, intersect it with the clip rect.
    if (mHasClipRect) {
        // NOTE: We are adding a temporary workaround due to the status bar
        // not always reporting the correct system decor rect. In such
        // cases, we take into account the specified content insets as well.
        if ((w->mSystemUiVisibility & SYSTEM_UI_FLAGS_LAYOUT_STABLE_FULLSCREEN)
                == SYSTEM_UI_FLAGS_LAYOUT_STABLE_FULLSCREEN
                || (w->mAttrs->GetFlags(&flags), (flags & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) != 0)) {
            // Don't apply the workaround to apps explicitly requesting
            // fullscreen layout or when the bars are transparent.
            Boolean result;
            clipRect->Intersect(mClipRect, &result);
        }
        else {
            clipRect->GetTop(&crt);
            Int32 cit;
            w->mContentInsets->GetTop(&cit);
            Int32 offsetTop = Elastos::Core::Math::Max(crt, cit);
            clipRect->Offset(0, -offsetTop);
            Boolean result;
            clipRect->Intersect(mClipRect, &result);
            clipRect->Offset(0, offsetTop);
        }
    }

    // The clip rect was generated assuming (0,0) as the window origin,
    // so we need to translate to match the actual surface coordinates.
    surfaceInsets->GetLeft(&sil);
    surfaceInsets->GetTop(&sit);
    clipRect->Offset(sil, sit);

    Boolean equals;
    if (IObject::Probe(clipRect)->Equals(mLastClipRect, &equals), !equals) {
        mLastClipRect->Set(clipRect);
        // try {
        // if (CWindowManagerService::SHOW_TRANSACTIONS) WindowManagerService.logSurface(w,
                // "CROP " + clipRect.toShortString(), null);
        if (FAILED(mSurfaceControl->SetWindowCrop(clipRect))) {
            String str;
            clipRect->ToShortString(&str);
            Slogger::W(TAG, "Error setting crop surface of %s crop=", TO_CSTR(w), str.string());
            if (!recoveringMemory) {
                mService->ReclaimSomeSurfaceMemoryLocked(this, String("crop"), TRUE);
            }
        }
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Error setting crop surface of " + w
        //             + " crop=" + clipRect.toShortString(), e);
        //     if (!recoveringMemory) {
        //         mService.reclaimSomeSurfaceMemoryLocked(this, "crop", true);
        //     }
        // }
    }
}

void WindowStateAnimator::SetSurfaceBoundariesLocked(
    /* [in] */ Boolean recoveringMemory)
{
    AutoPtr<WindowState> w = mWin;
    Int32 width, height;
    Int32 flags;
    w->mAttrs->GetFlags(&flags);
    if ((flags & IWindowManagerLayoutParams::FLAG_SCALED) != 0) {
        // for a scaled surface, we always want the requested
        // size.
        width  = w->mRequestedWidth;
        height = w->mRequestedHeight;
    }
    else {
        w->mCompatFrame->GetWidth(&width);
        w->mCompatFrame->GetHeight(&height);
    }

    // Something is wrong and SurfaceFlinger will not like this,
    // try to revert to sane values
    if (width < 1) {
        width = 1;
    }
    if (height < 1) {
        height = 1;
    }

    Float left = 0, top = 0;
    w->mShownFrame->GetLeft(&left);
    w->mShownFrame->GetTop(&top);

    // Adjust for surface insets.
    AutoPtr<IWindowManagerLayoutParams> attrs;
    w->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    AutoPtr<IRect> surfaceInsets;
    attrs->GetSurfaceInsets((IRect**)&surfaceInsets);
    Int32 sil, sir, sit, sib;
    surfaceInsets->Get(&sil, &sit, &sir, &sib);
    width += sil + sir;
    height += sit + sib;
    left -= sil;
    top -= sit;

    Boolean surfaceMoved = mSurfaceX != left || mSurfaceY != top;
    if (surfaceMoved) {
        mSurfaceX = left;
        mSurfaceY = top;

        // try {
        // if (CWindowManagerService::SHOW_TRANSACTIONS) WindowManagerService.logSurface(w,
        //         "POS " + left + ", " + top, null);
        if (FAILED(mSurfaceControl->SetPosition(left, top))) {
            Slogger::W(TAG, "Error positioning surface of %s pos=(%d,%d)", TO_CSTR(w), left, top);
            if (!recoveringMemory) {
                mService->ReclaimSomeSurfaceMemoryLocked(this, String("position"), TRUE);
            }
        }
        if (mSurfaceControlBlur != NULL) {
            if (FAILED(mSurfaceControlBlur->SetPosition(left, top))) {
                Slogger::W(TAG, "Error positioning surface of %s pos=(%d,%d)", TO_CSTR(w), left, top);
                if (!recoveringMemory) {
                    mService->ReclaimSomeSurfaceMemoryLocked(this, String("position"), TRUE);
                }
            }
        }
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Error positioning surface of " + w
        //             + " pos=(" + left + "," + top + ")", e);
        //     if (!recoveringMemory) {
        //         mService.reclaimSomeSurfaceMemoryLocked(this, "position", true);
        //     }
        // }
    }

    Boolean surfaceResized = mSurfaceW != width || mSurfaceH != height;
    if (surfaceResized) {
        mSurfaceW = width;
        mSurfaceH = height;
        mSurfaceResized = true;
        // try {
        // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(w,
        //         "SIZE " + width + "x" + height, null);
        if (FAILED(mSurfaceControl->SetSize(width, height))) {
            Slogger::W(TAG, "Error resizing surface of %s size=(%d,%d)", TO_CSTR(w), left, top);
            if (!recoveringMemory) {
                mService->ReclaimSomeSurfaceMemoryLocked(this, String("size"), TRUE);
            }
            goto fail;
        }
        if (mSurfaceControlBlur != NULL) {
            if (FAILED(mSurfaceControl->SetSize(width, height))) {
                Slogger::W(TAG, "Error resizing surface of %s size=(%d,%d)", TO_CSTR(w), left, top);
                if (!recoveringMemory) {
                    mService->ReclaimSomeSurfaceMemoryLocked(this, String("size"), TRUE);
                }
                goto fail;
            }
        }
        mAnimator->SetPendingLayoutChanges(w->GetDisplayId(),
                IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER);
        if (w->mAttrs->GetFlags(&flags), (flags & IWindowManagerLayoutParams::FLAG_DIM_BEHIND) != 0) {
            AutoPtr<TaskStack> stack = w->GetStack();
            if (stack != NULL) {
                stack->StartDimmingIfNeeded(this);
            }
        }
        if (w->mAttrs->GetFlags(&flags), (flags & IWindowManagerLayoutParams::FLAG_BLUR_BEHIND) != 0) {
            AutoPtr<TaskStack> stack = w->GetStack();
            if (stack != NULL) {
                stack->StartBlurringIfNeeded(this);
            }
        }
        // } catch (RuntimeException e) {
        //     // If something goes wrong with the surface (such
        //     // as running out of memory), don't take down the
        //     // entire system.
        //     Slog.e(TAG, "Error resizing surface of " + w
        //             + " size=(" + width + "x" + height + ")", e);
        //     if (!recoveringMemory) {
        //         mService.reclaimSomeSurfaceMemoryLocked(this, "size", true);
        //     }
        // }
    }

fail:
    UpdateSurfaceWindowCrop(recoveringMemory);
}

void WindowStateAnimator::PrepareSurfaceLocked(
    /* [in] */ Boolean recoveringMemory)
{
    AutoPtr<WindowState> w = mWin;
    if (mSurfaceControl == NULL) {
        if (w->mOrientationChanging) {
            // if (DEBUG_ORIENTATION) {
            //     Slog.v(TAG, "Orientation change skips hidden " + w);
            // }
            w->mOrientationChanging = FALSE;
        }
        return;
    }

    Boolean displayed = FALSE;

    ComputeShownFrameLocked();

    SetSurfaceBoundariesLocked(recoveringMemory);

    if (mIsWallpaper && !mWin->mWallpaperVisible) {
        // Wallpaper is no longer visible and there is no wp target => hide it.
        Hide();
    }
    else if (w->mAttachedHidden || !w->IsOnScreen()) {
        Hide();
        mAnimator->HideWallpapersLocked(w);

        // If we are waiting for this window to handle an
        // orientation change, well, it is hidden, so
        // doesn't really matter.  Note that this does
        // introduce a potential glitch if the window
        // becomes unhidden before it has drawn for the
        // new orientation.
        if (w->mOrientationChanging) {
            w->mOrientationChanging = FALSE;
            // if (DEBUG_ORIENTATION) Slog.v(TAG,
            //         "Orientation change skips hidden " + w);
        }
    }
    else if (mLastLayer != mAnimLayer
            || mLastAlpha != mShownAlpha
            || mLastDsDx != mDsDx
            || mLastDtDx != mDtDx
            || mLastDsDy != mDsDy
            || mLastDtDy != mDtDy
            || w->mLastHScale != w->mHScale
            || w->mLastVScale != w->mVScale
            || mLastHidden) {
        displayed = TRUE;
        mLastAlpha = mShownAlpha;
        mLastLayer = mAnimLayer;
        mLastDsDx = mDsDx;
        mLastDtDx = mDtDx;
        mLastDsDy = mDsDy;
        mLastDtDy = mDtDy;
        w->mLastHScale = w->mHScale;
        w->mLastVScale = w->mVScale;
        // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(w,
        //         "alpha=" + mShownAlpha + " layer=" + mAnimLayer
        //         + " matrix=[" + mDsDx + "*" + w.mHScale
        //         + "," + mDtDx + "*" + w.mVScale
        //         + "][" + mDsDy + "*" + w.mHScale
        //         + "," + mDtDy + "*" + w.mVScale + "]", null);
        if (mSurfaceControl != NULL) {
            // try {
            mSurfaceAlpha = mShownAlpha;
            mSurfaceControl->SetAlpha(mShownAlpha);
            mSurfaceLayer = mAnimLayer;
            mSurfaceControl->SetLayer(mAnimLayer);
            mSurfaceControl->SetMatrix(
                    mDsDx * w->mHScale, mDtDx * w->mVScale,
                    mDsDy * w->mHScale, mDtDy * w->mVScale);

            if (mSurfaceControlBlur != NULL) {
                mSurfaceControlBlur->SetAlpha(mShownAlpha);
                mSurfaceControlBlur->SetLayer(mAnimLayer - BLUR_LAYER_OFFSET);
                mSurfaceControlBlur->SetMatrix(
                    mDsDx * w->mHScale, mDtDx * w->mVScale,
                    mDsDy * w->mHScale, mDtDy * w->mVScale);
            }

            if (mLastHidden && mDrawState == HAS_DRAWN) {
                // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(w,
                //         "SHOW (performLayout)", null);
                // if (WindowManagerService.DEBUG_VISIBILITY) Slog.v(TAG, "Showing " + w
                //         + " during relayout");
                if (ShowSurfaceRobustlyLocked()) {
                    mLastHidden = FALSE;
                    if (mIsWallpaper) {
                        mService->DispatchWallpaperVisibility(w, TRUE);
                    }
                    // This draw means the difference between unique content and mirroring.
                    // Run another pass through performLayout to set mHasContent in the
                    // LogicalDisplay.
                    mAnimator->SetPendingLayoutChanges(w->GetDisplayId(),
                            IWindowManagerPolicy::FINISH_LAYOUT_REDO_ANIM);
                }
                else {
                    w->mOrientationChanging = FALSE;
                }
            }
            if (mSurfaceControl != NULL) {
                w->mToken->mHasVisible = TRUE;
            }
            // } catch (RuntimeException e) {
            //     Slog.w(TAG, "Error updating surface in " + w, e);
            //     if (!recoveringMemory) {
            //         mService.reclaimSomeSurfaceMemoryLocked(this, "update", true);
            //     }
            // }
        }
    }
    else {
        // if (DEBUG_ANIM && isAnimating()) {
        //     Slog.v(TAG, "prepareSurface: No changes in animation for " + this);
        // }
        displayed = TRUE;
    }

    if (displayed) {
        if (w->mOrientationChanging) {
            if (!w->IsDrawnLw()) {
                mAnimator->mBulkUpdateParams &= ~CWindowManagerService::LayoutFields::SET_ORIENTATION_CHANGE_COMPLETE;
                mAnimator->mLastWindowFreezeSource = w;
                // if (DEBUG_ORIENTATION) Slog.v(TAG,
                //         "Orientation continue waiting for draw in " + w);
            }
            else {
                w->mOrientationChanging = FALSE;
                // if (DEBUG_ORIENTATION) Slog.v(TAG, "Orientation change complete in " + w);
            }
        }
        w->mToken->mHasVisible = TRUE;
    }
}

void WindowStateAnimator::SetTransparentRegionHintLocked(
    /* [in] */ IRegion* region)
{
    if (mSurfaceControl == NULL) {
        Slogger::W(TAG, "setTransparentRegionHint: null mSurface after mHasSurface true");
        return;
    }
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS)
        Slogger::I(TAG, ">>> OPEN TRANSACTION setTransparentRegion");
    AutoPtr<ISurfaceControlHelper> surfaceHelper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceHelper);
    surfaceHelper->OpenTransaction();
    // try {
    // if (SHOW_TRANSACTIONS) WindowManagerService.logSurface(mWin,
    //         "transparentRegionHint=" + region, null);
    mSurfaceControl->SetTransparentRegionHint(region);
    // } finally {
    surfaceHelper->CloseTransaction();
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS)
        Slogger::I(TAG, "<<< CLOSE TRANSACTION setTransparentRegion");
    // }
}

void WindowStateAnimator::SetWallpaperOffset(
    /* [in] */ IRectF* shownFrame)
{
    AutoPtr<IWindowManagerLayoutParams> attrs;
    mWin->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    Float sfl;
    shownFrame->GetLeft(&sfl);
    AutoPtr<IRect> surfaceInsets;
    attrs->GetSurfaceInsets((IRect**)&surfaceInsets);
    Int32 sil;
    surfaceInsets->GetLeft(&sil);
    Int32 left = sfl - sil;
    Float sft;
    shownFrame->GetTop(&sft);
    Int32 sit;
    surfaceInsets->GetTop(&sit);
    Int32 top = sft - sit;
    if (mSurfaceX != left || mSurfaceY != top) {
        mSurfaceX = left;
        mSurfaceY = top;
        if (mAnimating) {
            // If this window (or its app token) is animating, then the position
            // of the surface will be re-computed on the next animation frame.
            // We can't poke it directly here because it depends on whatever
            // transformation is being applied by the animation.
            return;
        }
        if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, ">>> OPEN TRANSACTION setWallpaperOffset");
        AutoPtr<ISurfaceControlHelper> surfaceHelper;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceHelper);
        surfaceHelper->OpenTransaction();
        // try {
        // if (WindowManagerService.SHOW_TRANSACTIONS) WindowManagerService.logSurface(mWin,
        //         "POS " + left + ", " + top, null);
        Int32 winLeft, winTop;
        mWin->mFrame->GetLeft(&winLeft);
        mWin->mFrame->GetTop(&winTop);
        if (FAILED(mSurfaceControl->SetPosition(winLeft + left, winTop + top))) {
            Slogger::W(TAG, "Error positioning surface of %p size=(%d,%d)", mWin, left, top);
            surfaceHelper->CloseTransaction();
            if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, "<<< CLOSE TRANSACTION setWallpaperOffset");
            return;
        }
        if (mSurfaceControlBlur != NULL) {
            if (FAILED(mSurfaceControlBlur->SetPosition(winLeft + left, winTop + top))) {
                Slogger::W(TAG, "Error positioning surface of %p size=(%d,%d)", mWin, left, top);
                surfaceHelper->CloseTransaction();
                if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, "<<< CLOSE TRANSACTION setWallpaperOffset");
                return;
            }
        }
        UpdateSurfaceWindowCrop(FALSE);
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Error positioning surface of " + mWin
        //             + " pos=(" + left + "," + top + ")", e);
        // } finally {
        //     SurfaceControl.closeTransaction();
        //     if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slog.i(TAG,
        //             "<<< CLOSE TRANSACTION setWallpaperOffset");
        // }
        surfaceHelper->CloseTransaction();
        if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, "<<< CLOSE TRANSACTION setWallpaperOffset");
    }
}

void WindowStateAnimator::SetOpaqueLocked(
    /* [in] */ Boolean isOpaque)
{
    if (mSurfaceControl == NULL) {
        return;
    }
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, ">>> OPEN TRANSACTION setOpaqueLocked");
    AutoPtr<ISurfaceControlHelper> surfaceHelper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceHelper);
    surfaceHelper->OpenTransaction();
    // try {
    // if (SHOW_TRANSACTIONS) WindowManagerService.logSurface(mWin, "isOpaque=" + isOpaque,
    //         null);
    mSurfaceControl->SetOpaque(isOpaque);
    // } finally {
    //     SurfaceControl.closeTransaction();
    //     if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slog.i(TAG, "<<< CLOSE TRANSACTION setOpaqueLocked");
    // }
    surfaceHelper->CloseTransaction();
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(TAG, "<<< CLOSE TRANSACTION setWallpaperOffset");
}

Boolean WindowStateAnimator::PerformShowLocked()
{
    if (mWin->IsHiddenFromUserLocked()) {
        return FALSE;
    }
    // if (DEBUG_VISIBILITY || (DEBUG_STARTING_WINDOW &&
    //         mWin.mAttrs.type == WindowManager.LayoutParams.TYPE_APPLICATION_STARTING)) {
    //     RuntimeException e = null;
    //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
    //         e = new RuntimeException();
    //         e.fillInStackTrace();
    //     }
    //     Slog.v(TAG, "performShow on " + this
    //             + ": mDrawState=" + mDrawState + " readyForDisplay="
    //             + mWin.isReadyForDisplayIgnoringKeyguard()
    //             + " starting=" + (mWin.mAttrs.type == TYPE_APPLICATION_STARTING)
    //             + " during animation: policyVis=" + mWin.mPolicyVisibility
    //             + " attHidden=" + mWin.mAttachedHidden
    //             + " tok.hiddenRequested="
    //             + (mWin.mAppToken != null ? mWin.mAppToken.hiddenRequested : false)
    //             + " tok.hidden="
    //             + (mWin.mAppToken != null ? mWin.mAppToken.hidden : false)
    //             + " animating=" + mAnimating
    //             + " tok animating="
    //             + (mAppAnimator != null ? mAppAnimator.animating : false), e);
    // }
    if (mDrawState == READY_TO_SHOW && mWin->IsReadyForDisplayIgnoringKeyguard()) {
        // if (SHOW_TRANSACTIONS || DEBUG_ORIENTATION)
        //     WindowManagerService.logSurface(mWin, "SHOW (performShowLocked)", null);
        // if (DEBUG_VISIBILITY || (DEBUG_STARTING_WINDOW &&
        //         mWin.mAttrs.type == WindowManager.LayoutParams.TYPE_APPLICATION_STARTING)) {
        //     Slog.v(TAG, "Showing " + this
        //             + " during animation: policyVis=" + mWin.mPolicyVisibility
        //             + " attHidden=" + mWin.mAttachedHidden
        //             + " tok.hiddenRequested="
        //             + (mWin.mAppToken != null ? mWin.mAppToken.hiddenRequested : false)
        //             + " tok.hidden="
        //             + (mWin.mAppToken != null ? mWin.mAppToken.hidden : false)
        //             + " animating=" + mAnimating
        //             + " tok animating="
        //             + (mAppAnimator != null ? mAppAnimator.animating : false));
        // }

        mService->EnableScreenIfNeededLocked();

        ApplyEnterAnimationLocked();

        // Force the show in the next prepareSurfaceLocked() call.
        mLastAlpha = -1;
        // if (DEBUG_SURFACE_TRACE || DEBUG_ANIM)
        //     Slog.v(TAG, "performShowLocked: mDrawState=HAS_DRAWN in " + this);
        mDrawState = HAS_DRAWN;
        mService->ScheduleAnimationLocked();

        List<AutoPtr<WindowState> >::ReverseIterator rit = mWin->mChildWindows.RBegin();
        for (; rit != mWin->mChildWindows.REnd(); ++rit) {
            AutoPtr<WindowState> c = *rit;
            if (c->mAttachedHidden) {
                c->mAttachedHidden = FALSE;
                if (c->mWinAnimator->mSurfaceControl != NULL) {
                    c->mWinAnimator->PerformShowLocked();
                    // It hadn't been shown, which means layout not
                    // performed on it, so now we want to make sure to
                    // do a layout.  If called from within the transaction
                    // loop, this will cause it to restart with a new
                    // layout.
                    AutoPtr<DisplayContent> displayContent = c->GetDisplayContent();
                    if (displayContent != NULL) {
                        displayContent->mLayoutNeeded = TRUE;
                    }
                }
            }
        }

        Int32 type;
        mWin->mAttrs->GetType(&type);
        if (type != IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING
                && mWin->mAppToken != NULL) {
            mWin->mAppToken->mFirstWindowDrawn = TRUE;

            if (mWin->mAppToken->mStartingData != NULL) {
                // if (WindowManagerService.DEBUG_STARTING_WINDOW ||
                //         WindowManagerService.DEBUG_ANIM) Slog.v(TAG,
                //         "Finish starting " + mWin.mToken
                //         + ": first real window is shown, no animation");
                // If this initial window is animating, stop it -- we
                // will do an animation to reveal it from behind the
                // starting window, so there is no need for it to also
                // be doing its own stuff.
                ClearAnimation();
                mService->mFinishedStarting.PushBack(mWin->mAppToken);
                Boolean result;
                mService->mH->SendEmptyMessage(CWindowManagerService::H::FINISHED_STARTING, &result);
            }
            mWin->mAppToken->UpdateReportedVisibilityLocked();
        }

        return TRUE;
    }

    return FALSE;
}

Boolean WindowStateAnimator::ShowSurfaceRobustlyLocked()
{
    // try {
    if (mSurfaceControl != NULL) {
        mSurfaceShown = TRUE;
        ECode ec = mSurfaceControl->Show();
        if (FAILED(ec)) {
            Slogger::W(TAG, "Failure showing surface %p in %p 0x%08x",
                    mSurfaceControl.Get(), mWin, ec);
            mService->ReclaimSomeSurfaceMemoryLocked(this, String("show"), TRUE);
            return FALSE;
        }
        if (mSurfaceControlBlur != NULL) {
            ec = mSurfaceControlBlur->Show();
            if (FAILED(ec)) {
                Slogger::W(TAG, "Failure showing surface %p in %p 0x%08x",
                        mSurfaceControl.Get(), mWin, ec);
                mService->ReclaimSomeSurfaceMemoryLocked(this, String("show"), TRUE);
                return FALSE;
            }
        }
        if (mWin->mTurnOnScreen) {
            // if (DEBUG_VISIBILITY) Slog.v(TAG,
            //         "Show surface turning screen on: " + mWin);
            mWin->mTurnOnScreen = FALSE;
            mAnimator->mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_TURN_ON_SCREEN;
        }
    }
    return TRUE;
    // } catch (RuntimeException e) {
    //     Slog.w(TAG, "Failure showing surface " + mSurfaceControl + " in " + mWin, e);
    // }
}

void WindowStateAnimator::ApplyEnterAnimationLocked()
{
    Int32 transit;
    if (mEnterAnimationPending) {
        mEnterAnimationPending = FALSE;
        transit = IWindowManagerPolicy::TRANSIT_ENTER;
    }
    else {
        transit = IWindowManagerPolicy::TRANSIT_SHOW;
    }
    ApplyAnimationLocked(transit, TRUE);
    //TODO (multidisplay): Magnification is supported only for the default display.
    if (mService->mAccessibilityController != NULL
            && mWin->GetDisplayId() == IDisplay::DEFAULT_DISPLAY) {
        mService->mAccessibilityController->OnWindowTransitionLocked(mWin, transit);
    }
}

Boolean WindowStateAnimator::ApplyAnimationLocked(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean isEntrance)
{
    if (mLocalAnimating && mAnimationIsEntrance == isEntrance) {
        // If we are trying to apply an animation, but already running
        // an animation of the same type, then just leave that one alone.
        return TRUE;
    }

    // Only apply an animation if the display isn't frozen.  If it is
    // frozen, there is no reason to animate and it can cause strange
    // artifacts when we unfreeze the display if some different animation
    // is running.
    if (mService->OkToDisplay()) {
        Int32 anim;
        mPolicy->SelectAnimationLw(mWin, transit, &anim);
        Int32 attr = -1;
        AutoPtr<IAnimation> a;
        if (anim != 0) {
            if (anim != -1) {
                AutoPtr<IAnimationUtils> animationUtils;
                CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animationUtils);
                animationUtils->LoadAnimation(mContext, anim, (IAnimation**)&a);
            }
        }
        else {
            switch (transit) {
                case IWindowManagerPolicy::TRANSIT_ENTER:
                    attr = R::styleable::WindowAnimation_windowEnterAnimation;
                    break;
                case IWindowManagerPolicy::TRANSIT_EXIT:
                    attr = R::styleable::WindowAnimation_windowExitAnimation;
                    break;
                case IWindowManagerPolicy::TRANSIT_SHOW:
                    attr = R::styleable::WindowAnimation_windowShowAnimation;
                    break;
                case IWindowManagerPolicy::TRANSIT_HIDE:
                    attr = R::styleable::WindowAnimation_windowHideAnimation;
                    break;
            }
            if (attr >= 0) {
                a = mService->mAppTransition->LoadAnimationAttr(mWin->mAttrs, attr);
            }
        }
        // if (WindowManagerService.DEBUG_ANIM) Slog.v(TAG,
        //         "applyAnimation: win=" + this
        //         + " anim=" + anim + " attr=0x" + Integer.toHexString(attr)
        //         + " a=" + a
        //         + " transit=" + transit
        //         + " isEntrance=" + isEntrance + " Callers " + Debug.getCallers(3));
        if (a != NULL) {
            // if (WindowManagerService.DEBUG_ANIM) {
            //     RuntimeException e = null;
            //     if (!WindowManagerService.HIDE_STACK_CRAWLS) {
            //         e = new RuntimeException();
            //         e.fillInStackTrace();
            //     }
            //     Slog.v(TAG, "Loaded animation " + a + " for " + this, e);
            // }
            SetAnimation(a);
            mAnimationIsEntrance = isEntrance;
        }
    }
    else {
        ClearAnimation();
    }

    return mAnimation != NULL;
}

void WindowStateAnimator::UpdateFullyTransparent(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    Int32 privateFlags;
    Boolean fullyTransparent = (attrs->GetPrivateFlags(&privateFlags), (privateFlags &
            IWindowManagerLayoutParams::PRIVATE_FLAG_FULLY_TRANSPARENT)) != 0;
    if (fullyTransparent == mFullyTransparent) return;
    if (mSurfaceControl == NULL) return;
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->OpenTransaction();
    // try {
    if (FAILED(mSurfaceControl->SetTransparent(fullyTransparent))) {
        Slogger::W(TAG, "Error toggling transparency. ");
    }
    // } catch (RuntimeException e) {
    //     Slog.w(TAG, "Error toggling transparency. ", e);
    // } finally {
    //     SurfaceControl.closeTransaction();
    //     mFullyTransparent = fullyTransparent;
    // }
    helper->CloseTransaction();
    mFullyTransparent = fullyTransparent;
}

void WindowStateAnimator::UpdateBlurWithMaskingState(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Boolean hideForced)
{
    Int32 privateFlags;
    Boolean blurVisible = !hideForced && 0 != (attrs->GetPrivateFlags(&privateFlags), (privateFlags &
            (IWindowManagerLayoutParams::PRIVATE_FLAG_BLUR_WITH_MASKING |
            IWindowManagerLayoutParams::PRIVATE_FLAG_BLUR_WITH_MASKING_SCALED)));
    Boolean blurScaleNeeded = blurVisible && 0 != (attrs->GetPrivateFlags(&privateFlags), (privateFlags &
            IWindowManagerLayoutParams::PRIVATE_FLAG_BLUR_WITH_MASKING_SCALED));

    if (mSurfaceBlurShown == blurVisible && mSurfaceBlurScaleNeeded == blurScaleNeeded) return;
    mSurfaceBlurShown = blurVisible;
    mSurfaceBlurScaleNeeded = blurScaleNeeded;

    if (!blurVisible) {
        // we don't destroy mSurfaceControlBlur
        if (mSurfaceControlBlur != NULL) {
            mSurfaceControlBlur->Hide();
        }
        else {
            // nothing to do
        }
        return;
    }

    if (mSurfaceControl == NULL) return;

    if (blurVisible) {
        if (NULL == mSurfaceControlBlur) {
            Int32 flags = ISurfaceControl::HIDDEN | ISurfaceControl::FX_SURFACE_BLUR;
            Int32 attrsFlags;
            Boolean isHwAccelerated = (attrs->GetFlags(&attrsFlags), (attrsFlags &
                    IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED) != 0);
            Int32 format = IPixelFormat::TRANSLUCENT;
            if (!isHwAccelerated) {
                attrs->GetFormat(&format);
            }
            Int32 attrsFormat;
            attrs->GetFormat(&attrsFormat);
            AutoPtr<IPixelFormat> pixelFormat;
            CPixelFormat::AcquireSingleton((IPixelFormat**)&pixelFormat);
            Boolean result;
            pixelFormat->FormatHasAlpha(attrsFormat, &result);
            if (!result) {
                flags |= ISurfaceControl::OPAQUE;
            }

            AutoPtr<ICharSequence> title;
            attrs->GetTitle((ICharSequence**)&title);
            String str;
            title->ToString(&str);
            mSurfaceControlBlur = NULL;
            CSurfaceControl::New(mSession->mSurfaceSession,
                    str + " blur", (Int32)mSurfaceW, (Int32)mSurfaceH,
                    format, flags, (ISurfaceControl**)&mSurfaceControlBlur);
        }

        AutoPtr<ISurfaceControlHelper> helper;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
        helper->OpenTransaction();
        // try {
        mSurfaceControlBlur->SetPosition(mSurfaceX, mSurfaceY);
        mSurfaceControlBlur->SetLayerStack(mLayerStack);
        mSurfaceControlBlur->SetLayer(mAnimLayer - BLUR_LAYER_OFFSET);
        mSurfaceControlBlur->SetAlpha(mShownAlpha);
        mSurfaceControlBlur->SetBlur(1.0f);
        mSurfaceControlBlur->SetBlurMaskSurface(mSurfaceControl);
        Int32 BLUR_MASKING_SAMPLING = 4;
        mSurfaceControlBlur->SetBlurMaskSampling(blurScaleNeeded ? BLUR_MASKING_SAMPLING : 1);
        Float blurMaskAlphaThreshold;
        attrs->GetBlurMaskAlphaThreshold(&blurMaskAlphaThreshold);
        mSurfaceControlBlur->SetBlurMaskAlphaThreshold(blurMaskAlphaThreshold);
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Error creating blur surface", e);
        // } finally {
        //     SurfaceControl.closeTransaction();
        // }
        helper->CloseTransaction();

        if (mSurfaceShown) {
            mSurfaceControlBlur->Show();
        }
    }
}

} // Wm
} // Server
} // Droid
} // Elastos
