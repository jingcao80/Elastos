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

#include "elastos/droid/server/wm/BlackFrame.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  BlackFrame::BlackSurface
//==============================================================================

BlackFrame::BlackSurface::BlackSurface(
    /* [in] */ ISurfaceSession* session,
    /* [in] */ Int32 layer,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b,
    /* [in] */ Int32 layerStack,
    /* [in] */ BlackFrame* host)
    : mLeft(l)
    , mTop(t)
    , mLayer(layer)
    , mHost(host)
{
    Int32 w = r - l;
    Int32 h = b - t;
    // if (WindowManagerService.DEBUG_SURFACE_TRACE) {
            // surface = new WindowStateAnimator.SurfaceTrace(session, "BlackSurface("
            //         + l + ", " + t + ")",
            //         w, h, PixelFormat.OPAQUE, SurfaceControl.FX_SURFACE_DIM | SurfaceControl.HIDDEN);
    // } else {
    ASSERT_SUCCEEDED(CSurfaceControl::New(session, String("BlackSurface"),
            w, h, IPixelFormat::OPAQUE,
            ISurfaceControl::FX_SURFACE_DIM | ISurfaceControl::HIDDEN, (ISurfaceControl**)&mSurface));
    // }
    mSurface->SetAlpha(1);
    mSurface->SetLayerStack(layerStack);
    mSurface->SetLayer(layer);
    ASSERT_SUCCEEDED(mSurface->Show());
    if (CWindowManagerService::SHOW_TRANSACTIONS ||
            CWindowManagerService::SHOW_SURFACE_ALLOC) {
        Slogger::I(CWindowManagerService::TAG, "  BLACK %p: CREATE layer=%d", mSurface.Get(), layer);
    }
}

void BlackFrame::BlackSurface::SetAlpha(
    /* [in] */ Float alpha)
{
    mSurface->SetAlpha(alpha);
}

void BlackFrame::BlackSurface::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    mHost->mTmpMatrix->SetTranslate(mLeft, mTop);
    Boolean result;
    mHost->mTmpMatrix->PostConcat(matrix, &result);
    mHost->mTmpMatrix->GetValues(mHost->mTmpFloats);
    mSurface->SetPosition((*mHost->mTmpFloats)[IMatrix::MTRANS_X], (*mHost->mTmpFloats)[IMatrix::MTRANS_Y]);
    mSurface->SetMatrix(
            (*mHost->mTmpFloats)[IMatrix::MSCALE_X], (*mHost->mTmpFloats)[IMatrix::MSKEW_Y],
            (*mHost->mTmpFloats)[IMatrix::MSKEW_X], (*mHost->mTmpFloats)[IMatrix::MSCALE_Y]);
    // if (false) {
    //     Slog.i(WindowManagerService.TAG, "Black Surface @ (" + left + "," + top + "): ("
    //             + mTmpFloats[Matrix.MTRANS_X] + ","
    //             + mTmpFloats[Matrix.MTRANS_Y] + ") matrix=["
    //             + mTmpFloats[Matrix.MSCALE_X] + ","
    //             + mTmpFloats[Matrix.MSCALE_Y] + "]["
    //             + mTmpFloats[Matrix.MSKEW_X] + ","
    //             + mTmpFloats[Matrix.MSKEW_Y] + "]");
    // }
}

void BlackFrame::BlackSurface::ClearMatrix()
{
    mSurface->SetMatrix(1, 0, 0, 1);
}


//==============================================================================
//                  BlackFrame
//==============================================================================

BlackFrame::BlackFrame(
    /* [in] */ ISurfaceSession* session,
    /* [in] */ IRect* outer,
    /* [in] */ IRect* inner,
    /* [in] */ Int32 layer,
    /* [in] */ Int32 layerStack,
    /* [in] */ Boolean forceDefaultOrientation)
    : mForceDefaultOrientation(forceDefaultOrientation)
{
    CMatrix::New((IMatrix**)&mTmpMatrix);
    mTmpFloats = ArrayOf<Float>::Alloc(9);
    mBlackSurfaces = ArrayOf<BlackSurface*>::Alloc(4);

    Boolean success = FALSE;

    CRect::New(outer, (IRect**)&mOuterRect);
    CRect::New(inner, (IRect**)&mInnerRect);
    // try {
    Int32 outTop, inTop, outLeft, inLeft, outRight, inRight, outBottom, inBottom;
    outer->GetTop(&outTop);
    inner->GetTop(&inTop);
    outer->GetLeft(&outLeft);
    inner->GetLeft(&inLeft);
    outer->GetRight(&outRight);
    inner->GetRight(&inRight);
    outer->GetBottom(&outBottom);
    inner->GetBottom(&inBottom);
    if (outTop < inTop) {
        mBlackSurfaces->Set(0, new BlackSurface(session, layer,
                outLeft, outTop, inRight, inTop, layerStack, this));
    }
    if (outLeft < inLeft) {
        mBlackSurfaces->Set(1, new BlackSurface(session, layer,
                outLeft, inTop, inLeft, outBottom, layerStack, this));
    }
    if (outBottom > inBottom) {
        mBlackSurfaces->Set(2, new BlackSurface(session, layer,
                inLeft, inBottom, outRight, outBottom, layerStack, this));
    }
    if (outRight > inRight) {
        mBlackSurfaces->Set(3, new BlackSurface(session, layer,
                inRight, outTop, outRight, inBottom, layerStack, this));
    }
    success = TRUE;
    // } finally {
    if (!success) {
        Kill();
    }
    // }
}

void BlackFrame::Kill()
{
    if (mBlackSurfaces != NULL) {
        for (Int32 i = 0; i < mBlackSurfaces->GetLength(); i++) {
            if ((*mBlackSurfaces)[i] != NULL) {
                if (CWindowManagerService::SHOW_TRANSACTIONS ||
                        CWindowManagerService::SHOW_SURFACE_ALLOC) {
                    Slogger::I(CWindowManagerService::TAG,
                            "  BLACK %p: DESTROY", ((*mBlackSurfaces)[i]->mSurface).Get());
                }
                (*mBlackSurfaces)[i]->mSurface->Destroy();
                (*mBlackSurfaces)[i] = NULL;
            }
        }
    }
}

void BlackFrame::Hide()
{
    if (mBlackSurfaces != NULL) {
        for (Int32 i = 0; i < mBlackSurfaces->GetLength(); i++) {
            if ((*mBlackSurfaces)[i] != NULL) {
                (*mBlackSurfaces)[i]->mSurface->Hide();
            }
        }
    }
}

void BlackFrame::SetAlpha(
    /* [in] */ Float alpha)
{
    for (Int32 i = 0; i < mBlackSurfaces->GetLength(); i++) {
        if ((*mBlackSurfaces)[i] != NULL) {
            (*mBlackSurfaces)[i]->SetAlpha(alpha);
        }
    }
}

void BlackFrame::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    for (Int32 i = 0; i < mBlackSurfaces->GetLength(); i++) {
        if ((*mBlackSurfaces)[i] != NULL) {
            (*mBlackSurfaces)[i]->SetMatrix(matrix);
        }
    }
}

void BlackFrame::ClearMatrix()
{
    for (Int32 i = 0; i < mBlackSurfaces->GetLength(); i++) {
        if ((*mBlackSurfaces)[i] != NULL) {
            (*mBlackSurfaces)[i]->ClearMatrix();
        }
    }
}


} // Wm
} // Server
} // Droid
} // Elastos
