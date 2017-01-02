//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/server/wm/BlurLayer.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String BlurLayer::TAG("BlurLayer");
const Boolean BlurLayer::DEBUG;

BlurLayer::BlurLayer(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ TaskStack* stack,
    /* [in] */ DisplayContent* displayContent)
    : mDisplayContent(displayContent)
    , mBlur(0)
    , mLayer(-1)
    , mStartBlur(0)
    , mTargetBlur(0)
    , mStartTime(0)
    , mDuration(0)
    , mStack(stack)
{
    CRect::New((IRect**)&mBounds);
    CRect::New((IRect**)&mLastBounds);

    Int32 displayId = mDisplayContent->GetDisplayId();
    if (DEBUG) Slogger::V(TAG, "Ctor: displayId=%d", displayId);
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->OpenTransaction();
    // try {
    if (CWindowManagerService::DEBUG_SURFACE_TRACE) {
        assert(0 && "TODO");
        // mBlurSurface = new WindowStateAnimator.SurfaceTrace(service.mFxSession,
        //     "BlurSurface",
        //     16, 16, PixelFormat.OPAQUE,
        //     SurfaceControl.FX_SURFACE_BLUR | SurfaceControl.HIDDEN);
    }
    else {
        ECode ec = CSurfaceControl::New(service->mFxSession, TAG,
                16, 16, IPixelFormat::OPAQUE,
                ISurfaceControl::FX_SURFACE_BLUR | ISurfaceControl::HIDDEN, (ISurfaceControl**)&mBlurSurface);
        if (FAILED(ec)) {
            Slogger::E(CWindowManagerService::TAG, "Exception creating Blur surface, 0x%08x", ec);
        }
    }
    if (CWindowManagerService::SHOW_TRANSACTIONS ||
            CWindowManagerService::SHOW_SURFACE_ALLOC)
        Slogger::I(TAG, "  BLUR %s: CREATE", TO_CSTR(mBlurSurface));
    mBlurSurface->SetLayerStack(displayId);
    // } catch (Exception e) {
    //     Slog.e(WindowManagerService.TAG, "Exception creating Blur surface", e);
    // } finally {
    //     SurfaceControl.closeTransaction();
    // }
    helper->CloseTransaction();
}

Boolean BlurLayer::IsBlurring()
{
    return mTargetBlur != 0;
}

Boolean BlurLayer::IsAnimating()
{
    return mTargetBlur != mBlur;
}

Float BlurLayer::GetTargetBlur()
{
    return mTargetBlur;
}

void BlurLayer::SetLayer(
    /* [in] */ Int32 layer)
{
    if (mLayer != layer) {
        mLayer = layer;
        mBlurSurface->SetLayer(layer);
    }
}

Int32 BlurLayer::GetLayer()
{
    return mLayer;
}

void BlurLayer::SetBlur(
    /* [in] */ Float blur)
{
    if (mBlur != blur) {
        if (DEBUG) Slogger::V(TAG, "setBlur blur=%d", blur);
        // try {
        mBlurSurface->SetBlur(blur);
        if (blur == 0 && mShowing) {
            if (DEBUG) Slogger::V(TAG, "setBlur hiding");
            mBlurSurface->Hide();
            mShowing = FALSE;
        }
        else if (blur > 0 && !mShowing) {
            if (DEBUG) Slogger::V(TAG, "setBlur showing");
            mBlurSurface->Show();
            mShowing = TRUE;
        }
        // } catch (RuntimeException e) {
        //     Slog.w(TAG, "Failure setting blur immediately", e);
        // }
        mBlur = blur;
    }
}

void BlurLayer::AdjustSurface(
    /* [in] */ Int32 layer,
    /* [in] */ Boolean inTransaction)
{
    Int32 dw, dh;
    Float xPos, yPos;
    if (!mStack->IsFullscreen()) {
        mBounds->GetWidth(&dw);
        mBounds->GetHeight(&dh);
        Int32 value;
        mBounds->GetLeft(&value);
        xPos = (Float)value;
        mBounds->GetTop(&value);
        yPos = (Float)value;
    }
    else {
        // Set surface size to screen size.
        AutoPtr<IDisplayInfo> info = mDisplayContent->GetDisplayInfo();
        info->GetLogicalWidth(&dw);
        info->GetLogicalHeight(&dh);
        xPos = 0;
        yPos = 0;
    }

    // try {
    AutoPtr<ISurfaceControlHelper> helper;
    if (!inTransaction) {
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
        helper->OpenTransaction();
    }
    mBlurSurface->SetPosition(xPos, yPos);
    mBlurSurface->SetSize(dw, dh);
    mBlurSurface->SetLayer(layer);
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

void BlurLayer::SetBounds(
    /* [in] */ IRect* bounds)
{
    mBounds->Set(bounds);
    Boolean equals;
    if (IsBlurring() && (IObject::Probe(mLastBounds)->Equals(bounds, &equals), !equals)) {
        AdjustSurface(mLayer, FALSE);
    }
}

Boolean BlurLayer::DurationEndsEarlier(
    /* [in] */ Int64 duration)
{
    return SystemClock::GetUptimeMillis() + duration < mStartTime + mDuration;
}

void BlurLayer::Show()
{
    if (IsAnimating()) {
        if (DEBUG) Slogger::V(TAG, "show: immediate");
        Show(mLayer, mTargetBlur, 0);
    }
}

void BlurLayer::Show(
    /* [in] */ Int32 layer,
    /* [in] */ Float blur,
    /* [in] */ Int64 duration)
{
    if (DEBUG) Slogger::V(TAG, "show: layer=%d blur=%d duration=%d", layer, blur, duration);
    if (mBlurSurface == NULL) {
        Slogger::E(TAG, "show: no Surface");
        // Make sure isAnimating() returns false.
        mTargetBlur = mBlur = 0;
        return;
    }

    Boolean equals;
    if ((IObject::Probe(mLastBounds)->Equals(mBounds, &equals), !equals) || mLayer != layer) {
        AdjustSurface(layer, TRUE);
    }

    Int64 curTime = SystemClock::GetUptimeMillis();
    Boolean animating = IsAnimating();
    if ((animating && (mTargetBlur != blur || DurationEndsEarlier(duration)))
            || (!animating && mBlur != blur)) {
        if (duration <= 0) {
            // No animation required, just set values.
            SetBlur(blur);
        }
        else {
            // Start or continue animation with new parameters.
            mStartBlur = mBlur;
            mStartTime = curTime;
            mDuration = duration;
        }
    }
    if (DEBUG) Slogger::V(TAG, "show: mStartBlur=%d mStartTime=%d", mStartBlur, mStartTime);
    mTargetBlur = blur;
}

void BlurLayer::Hide()
{
    if (mShowing) {
        if (DEBUG) Slogger::V(TAG, "hide: immediate");
        Hide(0);
    }
}

void BlurLayer::Hide(
    /* [in] */ Int64 duration)
{
    if (mShowing && (mTargetBlur != 0 || DurationEndsEarlier(duration))) {
        if (DEBUG) Slogger::V(TAG, "hide: duration=%d", duration);
        Show(mLayer, 0, duration);
    }
}

Boolean BlurLayer::StepAnimation()
{
    if (mBlurSurface == NULL) {
        Slogger::E(TAG, "stepAnimation: null Surface");
        // Ensure that isAnimating() returns false;
        mTargetBlur = mBlur = 0;
        return FALSE;
    }

    if (IsAnimating()) {
        Int64 curTime = SystemClock::GetUptimeMillis();
        Float blurDelta = mTargetBlur - mStartBlur;
        Float blur = mStartBlur + blurDelta * (curTime - mStartTime) / mDuration;
        if ((blurDelta > 0 && blur > mTargetBlur) ||
                (blurDelta < 0 && blur < mTargetBlur)) {
            // Don't exceed limits.
            blur = mTargetBlur;
        }
        if (DEBUG) Slogger::V(TAG, "stepAnimation: curTime=%d blur=%d", curTime, blur);
        SetBlur(blur);
    }

    return IsAnimating();
}

void BlurLayer::DestroySurface()
{
    if (DEBUG) Slogger::V(TAG, "destroySurface.");
    if (mBlurSurface != NULL) {
        mBlurSurface->Destroy();
        mBlurSurface = NULL;
    }
}

void BlurLayer::PrintTo(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("mBlurSurface="));
    pw->Print(mBlurSurface);
    pw->Print(String(" mLayer="));
    pw->Print(mLayer);
    pw->Print(String(" mBlur="));
    pw->Println(mBlur);
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
    pw->Print(String(" mStartBlur="));
    pw->Print(mStartBlur);
    pw->Print(String(" mTargetBlur="));
    pw->Println(mTargetBlur);
}


} // Wm
} // Server
} // Droid
} // Elastos
