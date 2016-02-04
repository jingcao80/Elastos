
#include "elastos/droid/server/wm/DimLayer.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Droid::View::IDisplayInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String DimLayer::TAG("DimLayer");
const Boolean DimLayer::DEBUG;

DimLayer::DimLayer(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ TaskStack* stack,
    /* [in] */ DisplayContent* displayContent)
    : mDisplayContent(displayContent)
    , mAlpha(0)
    , mLayer(-1)
    , mStartAlpha(0)
    , mTargetAlpha(0)
    , mStartTime(0)
    , mDuration(0)
    , mStack(stack)
    , mShowing(FALSE)
{
    CRect::New((IRect**)&mBounds);
    CRect::New((IRect**)&mLastBounds);

    Int32 displayId = mDisplayContent->GetDisplayId();
    if (DEBUG) Slogger::V(TAG, "Ctor: displayId=" + displayId);
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->OpenTransaction();
    // try {
    // if (WindowManagerService.DEBUG_SURFACE_TRACE) {
    //     mDimSurface = new WindowStateAnimator.SurfaceTrace(service.mFxSession,
    //         "DimSurface",
    //         16, 16, PixelFormat.OPAQUE,
    //         SurfaceControl.FX_SURFACE_DIM | SurfaceControl.HIDDEN);
    // } else {
    ASSERT_SUCCEEDED(CSurfaceControl::New(service->mFxSession, TAG, 16, 16, IPixelFormat::OPAQUE,
            ISurfaceControl::FX_SURFACE_DIM | ISurfaceControl::HIDDEN, (ISurfaceControl**)&mDimSurface));
    // }
    if (CWindowManagerService::SHOW_TRANSACTIONS ||
            CWindowManagerService::SHOW_SURFACE_ALLOC) {
        Slogger::I(TAG, "  DIM %p: CREATE", mDimSurface.Get());
    }
    mDimSurface->SetLayerStack(displayId);
    // } catch (Exception e) {
    //     Slog.e(WindowManagerService.TAG, "Exception creating Dim surface", e);
    // } finally {
    //     SurfaceControl.closeTransaction();
    // }
    helper->CloseTransaction();
}

Boolean DimLayer::IsDimming()
{
    return mTargetAlpha != 0;
}

Boolean DimLayer::IsAnimating()
{
    return mTargetAlpha != mAlpha;
}

Float DimLayer::GetTargetAlpha()
{
    return mTargetAlpha;
}

void DimLayer::SetLayer(
    /* [in] */ Int32 layer)
{
    if (mLayer != layer) {
        mLayer = layer;
        mDimSurface->SetLayer(layer);
    }
}

Int32 DimLayer::GetLayer()
{
    return mLayer;
}

void DimLayer::SetAlpha(
    /* [in] */ Float alpha)
{
    if (mAlpha != alpha) {
        if (DEBUG) Slogger::V(TAG, "setAlpha alpha=%d", alpha);
        // try {
        mDimSurface->SetAlpha(alpha);
        if (alpha == 0 && mShowing) {
            if (DEBUG) Slogger::V(TAG, "setAlpha hiding");
            mDimSurface->Hide();
            mShowing = FALSE;
        }
        else if (alpha > 0 && !mShowing) {
            if (DEBUG) Slogger::V(TAG, "setAlpha showing");
            mDimSurface->Show();
            mShowing = TRUE;
        }
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Failure setting alpha immediately", e);
        // }
        mAlpha = alpha;
    }
}

void DimLayer::AdjustSurface(
    /* [in] */ Int32 layer,
    /* [in] */ Boolean inTransaction)
{
    Int32 dw, dh, xPos, yPos;
    if (!mStack->IsFullscreen()) {
        mBounds->GetWidth(&dw);
        mBounds->GetHeight(&dh);
        mBounds->GetLeft(&xPos);
        mBounds->GetTop(&yPos);
    }
    else {
        // Set surface size to screen size.
        AutoPtr<IDisplayInfo> info = mDisplayContent->GetDisplayInfo();
        // Multiply by 1.5 so that rotating a frozen surface that includes this does not expose
        // a corner.
        Int32 logicalW, logicalH;
        info->GetLogicalWidth(&logicalW);
        info->GetLogicalHeight(&logicalH);
        dw = (Int32)(logicalW * 1.5);
        dh = (Int32)(logicalH * 1.5);
        // back off position so 1/4 of Surface is before and 1/4 is after.
        xPos = -1 * dw / 6;
        yPos = -1 * dh / 6;
    }

    // try {
    AutoPtr<ISurfaceControlHelper> helper;
    if (!inTransaction) {
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
        helper->OpenTransaction();
    }
    mDimSurface->SetPosition(xPos, yPos);
    mDimSurface->SetSize(dw, dh);
    mDimSurface->SetLayer(layer);
    // } catch (RuntimeException e) {
    //     Slog.w(TAG, "Failure setting size or layer", e);
    // } finally {
    //     if (!inTransaction) {
    //         SurfaceControl.closeTransaction();
    //     }
    // }
    if (!inTransaction) {
        helper->CloseTransaction();
    }
    mLastBounds->Set(mBounds);
    mLayer = layer;
}

void DimLayer::SetBounds(
    /* [in] */ IRect* bounds)
{
    mBounds->Set(bounds);
    Boolean equals;
    if (IsDimming() && (IObject::Probe(mLastBounds)->Equals(bounds, &equals), !equals)) {
        AdjustSurface(mLayer, FALSE);
    }
}

Boolean DimLayer::DurationEndsEarlier(
    /* [in] */ Int64 duration)
{
    return SystemClock::GetUptimeMillis() + duration < mStartTime + mDuration;
}

void DimLayer::Show()
{
    if (IsAnimating()) {
        if (DEBUG) Slogger::V(TAG, "show: immediate");
        Show(mLayer, mTargetAlpha, 0);
    }
}

void DimLayer::Show(
    /* [in] */ Int32 layer,
    /* [in] */ Float alpha,
    /* [in] */ Int64 duration)
{
    if (DEBUG) Slogger::V(TAG, "show: layer=%d alpha=%d duration=%d", layer, alpha, duration);
    if (mDimSurface == NULL) {
        Slogger::E(TAG, "show: no Surface");
        // Make sure isAnimating() returns false.
        mTargetAlpha = mAlpha = 0;
        return;
    }

    Boolean equals;
    if ((IObject::Probe(mLastBounds)->Equals(mBounds, &equals), !equals) || mLayer != layer) {
        AdjustSurface(layer, TRUE);
    }

    Int64 curTime = SystemClock::GetUptimeMillis();
    Boolean animating = IsAnimating();
    if ((animating && (mTargetAlpha != alpha || DurationEndsEarlier(duration)))
            || (!animating && mAlpha != alpha)) {
        if (duration <= 0) {
            // No animation required, just set values.
            SetAlpha(alpha);
        }
        else {
            // Start or continue animation with new parameters.
            mStartAlpha = mAlpha;
            mStartTime = curTime;
            mDuration = duration;
        }
    }
    if (DEBUG) Slogger::V(TAG, "show: mStartAlpha=%d mStartTime=%d", mStartAlpha, mStartTime);
    mTargetAlpha = alpha;
}

void DimLayer::Hide()
{
    if (mShowing) {
        if (DEBUG) Slogger::V(TAG, "hide: immediate");
        Hide(0);
    }
}

void DimLayer::Hide(
    /* [in] */ Int64 duration)
{
    if (mShowing && (mTargetAlpha != 0 || DurationEndsEarlier(duration))) {
        if (DEBUG) Slogger::V(TAG, "hide: duration=%d", duration);
        Show(mLayer, 0, duration);
    }
}

Boolean DimLayer::StepAnimation()
{
    if (mDimSurface == NULL) {
        Slogger::E(TAG, "stepAnimation: null Surface");
        // Ensure that isAnimating() returns false;
        mTargetAlpha = mAlpha = 0;
        return FALSE;
    }

    if (IsAnimating()) {
        Int64 curTime = SystemClock::GetUptimeMillis();
        Float alphaDelta = mTargetAlpha - mStartAlpha;
        Float alpha = mStartAlpha + alphaDelta * (curTime - mStartTime) / mDuration;
        if ((alphaDelta > 0 && alpha > mTargetAlpha) ||
                (alphaDelta < 0 && alpha < mTargetAlpha)) {
            // Don't exceed limits.
            alpha = mTargetAlpha;
        }
        if (DEBUG) Slogger::V(TAG, "stepAnimation: curTime=%d alpha=%d", curTime, alpha);
        SetAlpha(alpha);
    }

    return IsAnimating();
}

void DimLayer::DestroySurface()
{
    if (DEBUG) Slogger::V(TAG, "destroySurface.");
    if (mDimSurface != NULL) {
        mDimSurface->Destroy();
        mDimSurface = NULL;
    }
}

void DimLayer::PrintTo(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("mDimSurface="));
    pw->Print(mDimSurface);
    pw->Print(String(" mLayer="));
    pw->Print(mLayer);
    pw->Print(String(" mAlpha="));
    pw->Println(mAlpha);
    pw->Print(prefix);
    pw->Print(String("mLastBounds="));
    String str;
    mLastBounds->ToShortString(&str);
    pw->Print(str);
    pw->Print(String(" mBounds="));
    mBounds->ToShortString(&str);
    pw->Println(str);
    pw->Print(prefix);
    pw->Print(String("Last animation: "));
    pw->Print(String(" mDuration="));
    pw->Print(mDuration);
    pw->Print(String(" mStartTime="));
    pw->Print(mStartTime);
    pw->Print(String(" curTime="));
    pw->Println(SystemClock::GetUptimeMillis());
    pw->Print(prefix);
    pw->Print(String(" mStartAlpha="));
    pw->Print(mStartAlpha);
    pw->Print(String(" mTargetAlpha="));
    pw->Println(mTargetAlpha);
}

} // Wm
} // Server
} // Droid
} // Elastos
