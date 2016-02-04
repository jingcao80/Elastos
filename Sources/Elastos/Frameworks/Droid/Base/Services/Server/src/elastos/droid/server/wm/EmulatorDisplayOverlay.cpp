
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/wm/EmulatorDisplayOverlay.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String EmulatorDisplayOverlay::TAG("EmulatorDisplayOverlay");

EmulatorDisplayOverlay::EmulatorDisplayOverlay(
    /* [in] */ IContext* context,
    /* [in] */ IDisplay* display,
    /* [in] */ ISurfaceSession* session,
    /* [in] */ Int32 zOrder)
    : mLastDW(0)
    , mLastDH(0)
    , mDrawNeeded(FALSE)
    , mRotation(0)
    , mVisible(FALSE)
{
    CSurface::New((ISurface**)&mSurface);

    CPoint::New((IPoint**)&mScreenSize);
    display->GetSize(mScreenSize);

    AutoPtr<ISurfaceControl> ctrl;
    // try {
    // if (WindowManagerService.DEBUG_SURFACE_TRACE) {
    //     ctrl = new WindowStateAnimator.SurfaceTrace(session, "EmulatorDisplayOverlay",
    //             mScreenSize.x, mScreenSize.y, PixelFormat.TRANSLUCENT,
    //             SurfaceControl.HIDDEN);
    // } else {
    Int32 x, y;
    mScreenSize->GetX(&x);
    mScreenSize->GetY(&y);
    CSurfaceControl::New(session, String("EmulatorDisplayOverlay"), x,
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
    context->GetDrawable(R::drawable::emulator_circular_window_overlay, (IDrawable**)&mOverlay);
}

void EmulatorDisplayOverlay::DrawIfNeeded()
{
    if (!mDrawNeeded || !mVisible) {
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
    // } catch (OutOfResourcesException e) {
    // }
    if (c == NULL) {
        return;
    }
    c->DrawColor(IColor::TRANSPARENT, PorterDuffMode_SRC);
    mSurfaceControl->SetPosition(0, 0);
    mScreenSize->GetX(&x);
    mScreenSize->GetY(&y);
    mOverlay->SetBounds(0, 0, x, y);
    mOverlay->Draw(c);
    mSurface->UnlockCanvasAndPost(c);
}

void EmulatorDisplayOverlay::SetVisibility(
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

void EmulatorDisplayOverlay::PositionSurface(
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
