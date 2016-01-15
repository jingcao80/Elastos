
#include "wm/StrictModeFlash.h"
#include "wm/CWindowManagerService.h"

using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::RegionOp;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String StrictModeFlash::TAG("StrictModeFlash");

StrictModeFlash::StrictModeFlash(
    /* [in] */ IDisplay* display,
    /* [in] */ ISurfaceSession* session)
    : mLastDW(0)
    , mLastDH(0)
    , mThickness(20)
{
    AutoPtr<ISurfaceControl> ctrl;
    // try {
    CSurfaceControl::New(session, String("StrictModeFlash"),
            1, 1, IPixelFormat::TRANSLUCENT, ISurfaceControl::HIDDEN, (ISurfaceControl**)&ctrl);
    ctrl = new SurfaceControl();
    Int32 stack;
    display->GetLayerStack(&stack);
    ctrl->SetLayerStack(stack);
    ctrl->SetLayer(CWindowManagerService::TYPE_LAYER_MULTIPLIER * 101);  // one more than Watermark? arbitrary.
    ctrl->SetPosition(0, 0);
    ctrl->Show();
    mSurface->CopyFrom(ctrl);
    // } catch (OutOfResourcesException e) {
    // }
    mSurfaceControl = ctrl;
    mDrawNeeded = TRUE;
}

void StrictModeFlash::DrawIfNeeded()
{
    if (!mDrawNeeded) {
        return;
    }
    mDrawNeeded = FALSE;
    Int32 dw = mLastDW;
    Int32 dh = mLastDH;

    AutoPtr<IRect> dirty;
    CRect::New(0, 0, dw, dh, (IRect**)&dirty);
    AutoPtr<ICanvas> c;
    // try {
    mSurface->LockCanvas(dirty, (ICanvas**)&c);
    // } catch (IllegalArgumentException e) {
    // } catch (Surface.OutOfResourcesException e) {
    // }
    if (c == NULL) {
        return;
    }

    // Top
    Boolean result;
    AutoPtr<IRect> r;
    CRect::New(0, 0, dw, mThickness, (IRect**)&r);
    c->ClipRect(r , Elastos::Droid::Graphics::RegionOp_REPLACE, &result);
    c->DrawColor(IColor::RED);
    // Left
    r = NULL;
    CRect::New(0, 0, mThickness, dh, (IRect**)&r);
    c->ClipRect(r, Elastos::Droid::Graphics::RegionOp_REPLACE, &result);
    c->DrawColor(IColor::RED);
    // Right
    r = NULL;
    CRect::New(dw - mThickness, 0, dw, dh, (IRect**)&r);
    c->ClipRect(r, Elastos::Droid::Graphics::RegionOp_REPLACE, &result);
    c->DrawColor(IColor::RED);
    // Bottom
    r = NULL;
    CRect::New(0, dh - mThickness, dw, dh, (IRect**)&r);
    c->ClipRect(r, Elastos::Droid::Graphics::RegionOp_REPLACE, &result);
    c->DrawColor(IColor::RED);

    mSurface->UnlockCanvasAndPost(c);
}

void StrictModeFlash::SetVisibility(
    /* [in] */ Boolean on)
{
    if (mSurfaceControl == NULL) {
        return;
    }
    DrawIfNeeded();
    if (on) {
        mSurfaceControl->Show();
    }
    else {
        mSurfaceControl->Hide();
    }
}

void StrictModeFlash::PositionSurface(
    /* [in] */ Int32 dw,
    /* [in] */ Int32 dh)
{
    if (mLastDW == dw && mLastDH == dh) {
        return;
    }
    mLastDW = dw;
    mLastDH = dh;
    mSurfaceControl->SetSize(dw, dh);
    mDrawNeeded = TRUE;
}


} // Wm
} // Server
} // Droid
} // Elastos
