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

#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/wm/CircularDisplayMask.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String CircularDisplayMask::TAG("CircularDisplayMask");

const Int32 CircularDisplayMask::STROKE_WIDTH;

CircularDisplayMask::CircularDisplayMask(
    /* [in] */ IDisplay* display,
    /* [in] */ ISurfaceSession* session,
    /* [in] */ Int32 zOrder,
    /* [in] */ Int32 screenOffset)
    : mScreenOffset(0)
    , mLastDW(0)
    , mLastDH(0)
    , mDrawNeeded(FALSE)
    , mRotation(0)
    , mVisible(FALSE)
    , mDimensionsUnequal(FALSE)
{
    CSurface::New((ISurface**)&mSurface);
    CPoint::New((IPoint**)&mScreenSize);
    display->GetSize(mScreenSize);
    Int32 x, y;
    mScreenSize->GetX(&x);
    mScreenSize->GetY(&y);
    if (x != y) {
        Int32 id;
        display->GetDisplayId(&id);
        Slogger::W(TAG, "Screen dimensions of displayId = %dare not equal, circularMask will not be drawn.", id);
        mDimensionsUnequal = TRUE;
    }

    AutoPtr<ISurfaceControl> ctrl;
    // try {
    // if (WindowManagerService.DEBUG_SURFACE_TRACE) {
    //     ctrl = new WindowStateAnimator.SurfaceTrace(session, "CircularDisplayMask",
    //             mScreenSize.x, mScreenSize.y, PixelFormat.TRANSLUCENT,
    //             SurfaceControl.HIDDEN);
    // } else {
    CSurfaceControl::New(session, String("CircularDisplayMask"), x,
            y, IPixelFormat::TRANSLUCENT, ISurfaceControl::HIDDEN, (ISurfaceControl**)&ctrl);
    // }
    Int32 stack;
    display->GetLayerStack(&stack);
    ctrl->SetLayerStack(stack);
    ctrl->SetLayer(zOrder);
    ctrl->SetPosition(0, 0);
    ctrl->Show();
    mSurface->CopyFrom(ctrl);
    // } catch (OutOfResourcesException e) {
    // }
    mSurfaceControl = ctrl;
    mDrawNeeded = TRUE;
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetColor(IColor::BLACK);
    mPaint->SetStrokeWidth(STROKE_WIDTH);
    mScreenOffset = screenOffset;
}

void CircularDisplayMask::DrawIfNeeded()
{
    if (!mDrawNeeded || !mVisible || mDimensionsUnequal) {
        return;
    }
    mDrawNeeded = FALSE;

    Int32 x, y;
    mScreenSize->GetX(&x);
    mScreenSize->GetY(&y);
    AutoPtr<IRect> dirty;
    CRect::New(0, 0, x, y, (IRect**)&dirty);
    AutoPtr<ICanvas> c;
    // try {
    mSurface->LockCanvas(dirty, (ICanvas**)&c);
    // } catch (IllegalArgumentException e) {
    // } catch (Surface.OutOfResourcesException e) {
    // }
    if (c == NULL) {
        return;
    }
    c->DrawColor(IColor::TRANSPARENT, PorterDuffMode_SRC);
    switch (mRotation) {
        case ISurface::ROTATION_0:
        case ISurface::ROTATION_90:
            // chin bottom or right
            mSurfaceControl->SetPosition(0, 0);
            break;
        case ISurface::ROTATION_180:
            // chin top
            mSurfaceControl->SetPosition(0, -mScreenOffset);
            break;
        case ISurface::ROTATION_270:
            // chin left
            mSurfaceControl->SetPosition(-mScreenOffset, 0);
            break;
    }

    Int32 circleRadius = x / 2;
    c->DrawCircle(circleRadius, circleRadius, circleRadius, mPaint);
    mSurface->UnlockCanvasAndPost(c);
}

void CircularDisplayMask::SetVisibility(
    /* [in] */ Boolean on)
{
    if (mSurfaceControl == NULL) {
        return;
    }
    mVisible = on;
    DrawIfNeeded();
    if (on) {
        mSurfaceControl->Show();
    }
    else {
        mSurfaceControl->Hide();
    }
}

void CircularDisplayMask::PositionSurface(
    /* [in] */ Int32 dw,
    /* [in] */ Int32 dh,
    /* [in] */ Int32 rotation)
{
    if (mLastDW == dw && mLastDH == dh && mRotation == rotation) {
        return;
    }
    mLastDW = dw;
    mLastDH = dh;
    mDrawNeeded = TRUE;
    mRotation = rotation;
    DrawIfNeeded();
}

} // Wm
} // Server
} // Droid
} // Elastos
