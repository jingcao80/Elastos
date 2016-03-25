
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/wm/FocusedStackFrame.h"
#include "elastos/droid/server/wm/WindowState.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/TaskStack.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::RegionOp_REPLACE;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String FocusedStackFrame::TAG("FocusedStackFrame");
const Int32 FocusedStackFrame::THICKNESS;
const Float FocusedStackFrame::ALPHA;

FocusedStackFrame::FocusedStackFrame(
    /* [in] */ IDisplay* display,
    /* [in] */ ISurfaceSession* session)
{
    CSurface::New((ISurface**)&mSurface);
    CRect::New((IRect**)&mLastBounds);
    CRect::New((IRect**)&mBounds);
    CRect::New((IRect**)&mTmpDrawRect);

    AutoPtr<ISurfaceControl> ctrl;
    // try {
    // if (DEBUG_SURFACE_TRACE) {
    //     ctrl = new SurfaceTrace(session, "FocusedStackFrame",
    //         1, 1, PixelFormat.TRANSLUCENT, SurfaceControl.HIDDEN);
    // } else {
    CSurfaceControl::New(session, String("FocusedStackFrame"),
        1, 1, IPixelFormat::TRANSLUCENT, ISurfaceControl::HIDDEN, (ISurfaceControl**)&ctrl);
    // }
    Int32 stack;
    display->GetLayerStack(&stack);
    ctrl->SetLayerStack(stack);
    ctrl->SetAlpha(ALPHA);
    mSurface->CopyFrom(ctrl);
    // } catch (OutOfResourcesException e) {
    // }
    mSurfaceControl = ctrl;
}

void FocusedStackFrame::Draw(
    /* [in] */ IRect* bounds,
    /* [in] */ Int32 color)
{
    if (FALSE && CWindowManagerService::DEBUG_STACK) {
        String str;
        bounds->ToShortString(&str);
        Slogger::I(TAG, "draw: bounds=%s color=%d", str.string(), color);
    }
    mTmpDrawRect->Set(bounds);
    AutoPtr<ICanvas> c;
    // try {
    mSurface->LockCanvas(mTmpDrawRect, (ICanvas**)&c);
    // } catch (IllegalArgumentException e) {
    // } catch (Surface.OutOfResourcesException e) {
    // }
    if (c == NULL) {
        return;
    }

    Int32 w, h;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);

    // Top
    mTmpDrawRect->Set(0, 0, w, THICKNESS);
    Boolean result;
    c->ClipRect(mTmpDrawRect, RegionOp_REPLACE, &result);
    c->DrawColor(color);
    // Left (not including Top or Bottom stripe).
    mTmpDrawRect->Set(0, THICKNESS, THICKNESS, h - THICKNESS);
    c->ClipRect(mTmpDrawRect, RegionOp_REPLACE, &result);
    c->DrawColor(color);
    // Right (not including Top or Bottom stripe).
    mTmpDrawRect->Set(w - THICKNESS, THICKNESS, w, h - THICKNESS);
    c->ClipRect(mTmpDrawRect, RegionOp_REPLACE, &result);
    c->DrawColor(color);
    // Bottom
    mTmpDrawRect->Set(0, h - THICKNESS, w, h);
    c->ClipRect(mTmpDrawRect, RegionOp_REPLACE, &result);
    c->DrawColor(color);

    mSurface->UnlockCanvasAndPost(c);
}

void FocusedStackFrame::PositionSurface(
    /* [in] */ IRect* bounds)
{
    if (FALSE && CWindowManagerService::DEBUG_STACK) {
        String str;
        bounds->ToShortString(&str);
        Slogger::I(TAG, "positionSurface: bounds=%s", str.string());
    }
    Int32 w, h, l, t;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    bounds->GetLeft(&l);
    bounds->GetTop(&t);
    mSurfaceControl->SetSize(w, h);
    mSurfaceControl->SetPosition(l, t);
}

void FocusedStackFrame::SetVisibility(
    /* [in] */ Boolean on)
{
    if (FALSE && CWindowManagerService::DEBUG_STACK) {
        String str1, str2;
        mLastBounds->ToShortString(&str1);
        mBounds->ToShortString(&str2);
        Slogger::I(TAG, "setVisibility: on=%d mLastBounds=%s mBounds=%s",
            on, str1.string(), str2.string());
    }
    if (mSurfaceControl == NULL) {
        return;
    }
    if (on) {
        if (Object::Equals(mLastBounds, mBounds)) {
            // Erase the previous rectangle.
            PositionSurface(mLastBounds);
            Draw(mLastBounds, IColor::TRANSPARENT);
            // Draw the latest rectangle.
            PositionSurface(mBounds);
            Draw(mBounds, IColor::WHITE);
            // Update the history.
            mLastBounds->Set(mBounds);
        }
        mSurfaceControl->Show();
    }
    else {
        mSurfaceControl->Hide();
    }
}

void FocusedStackFrame::SetBounds(
    /* [in] */ TaskStack* stack)
{
    stack->GetBounds(mBounds);
    if (FALSE && CWindowManagerService::DEBUG_STACK)
        Slogger::I(TAG, "setBounds: bounds=%p", mBounds.Get());
}

void FocusedStackFrame::SetLayer(
    /* [in] */ Int32 layer)
{
    mSurfaceControl->SetLayer(layer);
}

} // Wm
} // Server
} // Droid
} // Elastos
