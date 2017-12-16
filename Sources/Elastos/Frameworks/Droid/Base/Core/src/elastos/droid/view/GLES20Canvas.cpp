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

#include "elastos/droid/view/GLES20Canvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/DrawFilter.h"
#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/view/RenderNode.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

#include <androidfw/ResourceTypes.h>
#include <private/hwui/DrawGlInfo.h>
#include <cutils/properties.h>
#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkImageInfo.h>
#include <SkMatrix.h>
#include <SkRegion.h>
#include <SkTemplates.h>
#include <hwui/Canvas.h>
#include <hwui/CanvasProperty.h>
#include <hwui/RenderNode.h>
#include <hwui/Typeface.h>
#include <hwui/hwui/Paint.h>
#include <hwui/renderthread/RenderProxy.h>
#include <minikin/Layout.h>

using Elastos::Droid::Graphics::BitmapWrapper;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::DrawFilter;
using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::Matrix;
using Elastos::Droid::Graphics::NinePatch;
using Elastos::Droid::Graphics::TemporaryBuffer;
using Elastos::Droid::Graphics::Paint;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_OVER;
using Elastos::Droid::Graphics::RegionOp_INTERSECT;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::IGraphicsOperations;
using Elastos::Droid::Internal::Utility::ArrayUtils;

using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {


static Boolean NativeIsAvailable()
{
#ifdef USE_OPENGL_RENDERER
    char prop[PROPERTY_VALUE_MAX];
    if (property_get("ro.kernel.qemu", prop, NULL) == 0) {
        // not in the emulator
        return TRUE;
    }
    // In the emulator this property will be set to 1 when hardware GLES is
    // enabled, 0 otherwise. On old emulator versions it will be undefined.
    property_get("ro.kernel.qemu.gles", prop, "0");
    return atoi(prop) == 1 ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

//============================================================================
// GLES20Canvas::CanvasFinalizer
//============================================================================
GLES20Canvas::CanvasFinalizer::CanvasFinalizer(
    /* [in] */ Int64 rendererPtr)
    : mRenderer(rendererPtr)
{}

GLES20Canvas::CanvasFinalizer::~CanvasFinalizer()
{}

//============================================================================
// GLES20Canvas
//============================================================================

CAR_INTERFACE_IMPL(GLES20Canvas, HardwareCanvas, IGLES20Canvas)

Boolean GLES20Canvas::sIsAvailable = NativeIsAvailable();

GLES20Canvas::GLES20Canvas()
    : mRenderer(0)
    , mOpaque(FALSE)
    , mWidth(0)
    , mHeight(0)
{}

ECode GLES20Canvas::constructor(
    /* [in] */ IRenderNode* node,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(HardwareCanvas::constructor())

    mOpaque = FALSE;
    mRenderer = nCreateDisplayListRenderer(((RenderNode*)node)->mNativeRenderNode, width, height);
    SetupFinalizer();
    return NOERROR;
}

ECode GLES20Canvas::GetWidth(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mWidth;
    return NOERROR;
}

ECode GLES20Canvas::GetHeight(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mHeight;
    return NOERROR;
}

ECode GLES20Canvas::GetMaximumBitmapWidth(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nGetMaximumTextureWidth();
    return NOERROR;
}

ECode GLES20Canvas::GetMaximumBitmapHeight(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nGetMaximumTextureHeight();
    return NOERROR;
}

ECode GLES20Canvas::SetViewport(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mWidth = width;
    mHeight = height;
    return NOERROR;
}

ECode GLES20Canvas::SetHighContrastText(
    /* [in] */ Boolean highContrastText)
{
    nSetHighContrastText(mRenderer, highContrastText);
    return NOERROR;
}

ECode GLES20Canvas::InsertReorderBarrier()
{
    nInsertReorderBarrier(mRenderer, TRUE);
    return NOERROR;
}

ECode GLES20Canvas::InsertInorderBarrier()
{
    nInsertReorderBarrier(mRenderer, FALSE);
    return NOERROR;
}

ECode GLES20Canvas::OnPreDraw(
    /* [in] */ IRect* dirty,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode GLES20Canvas::OnPostDraw()
{
    return NOERROR;
}

ECode GLES20Canvas::CallDrawGLFunction(
    /* [in] */ Int64 drawGLFunction,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nCallDrawGLFunction(mRenderer, drawGLFunction);
    return NOERROR;
}

ECode GLES20Canvas::DrawRenderNode(
    /* [in] */ IRenderNode* renderNode,
    /* [in] */ IRect* dirty,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    Int64 list;
    renderNode->GetNativeDisplayList(&list);
    *res = nDrawRenderNode(mRenderer, list, dirty, flags);
    return NOERROR;
}

ECode GLES20Canvas::DrawHardwareLayer(
    /* [in] */ IHardwareLayer* layer,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    layer->SetLayerPaint(paint);
    Int64 l;
    layer->GetLayer(&l);
    nDrawLayer(mRenderer, l, x, y);
    return NOERROR;
}

ECode GLES20Canvas::ClipPath(
    /* [in] */ IPath* path,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    CPath* p = (CPath*)path;
    *res = nClipPath(mRenderer, p->mNativePath, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipPath(
    /* [in] */ IPath* path,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    CPath* p = (CPath*)path;
    *res = nClipPath(mRenderer, p->mNativePath, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nClipRect(mRenderer, left, top, right, bottom, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRect* rect,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    *res = nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRect* rect,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    *res = nClipRect(mRenderer, left, top, right, bottom, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRectF* rect,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Float left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    *res = nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRectF* rect,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Float left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    *res = nClipRect(mRenderer, left, top, right, bottom, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRegion(
    /* [in] */ IRegion* region,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

ECode GLES20Canvas::ClipRegion(
    /* [in] */ IRegion* region,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

ECode GLES20Canvas::GetClipBounds(
    /* [in] */ IRect* bounds,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nGetClipBounds(mRenderer, bounds);
    return NOERROR;
}

ECode GLES20Canvas::QuickReject(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nQuickReject(mRenderer, left, top, right, bottom);
    return NOERROR;
}

ECode GLES20Canvas::QuickReject(
    /* [in] */ IPath* path,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IRectF> pathBounds = GetPathBounds();
    path->ComputeBounds(pathBounds, TRUE);
    Float left, top, right, bottom;
    pathBounds->Get(&left, &top, &right, &bottom);
    *res = nQuickReject(mRenderer, left, top, right, bottom);
    return NOERROR;
}

ECode GLES20Canvas::QuickReject(
    /* [in] */ IRectF* rect,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Float left, top, right, bottom;
    rect->Get(&left, &top, &right, &bottom);
    nQuickReject(mRenderer, left, top, right, bottom);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////
// Transformations
///////////////////////////////////////////////////////////////////////////

ECode GLES20Canvas::Translate(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    if (dx != 0.0f || dy != 0.0f) nTranslate(mRenderer, dx, dy);
    return NOERROR;
}

ECode GLES20Canvas::Skew(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    nSkew(mRenderer, sx, sy);
    return NOERROR;
}


ECode GLES20Canvas::Rotate(
    /* [in] */ Float degrees)
{
    nRotate(mRenderer, degrees);
    return NOERROR;
}

ECode GLES20Canvas::Scale(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    nScale(mRenderer, sx, sy);
    return NOERROR;
}

ECode GLES20Canvas::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    Matrix* m = (Matrix*)matrix;
    nSetMatrix(mRenderer, m == NULL ? 0 : m->mNativeMatrix);
    return NOERROR;
}

ECode GLES20Canvas::GetMatrix(
    /* [in] */ IMatrix* matrix)
{
    Matrix* m = (Matrix*)matrix;
    nGetMatrix(mRenderer, m->mNativeMatrix);
    return NOERROR;
}

ECode GLES20Canvas::Concat(
    /* [in] */ IMatrix* matrix)
{
    Matrix* m = (Matrix*)matrix;
    if (matrix != NULL) nConcatMatrix(mRenderer, m->mNativeMatrix);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////
// State management
///////////////////////////////////////////////////////////////////////////

ECode GLES20Canvas::Save(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nSave(mRenderer, ICanvas::CLIP_SAVE_FLAG | ICanvas::MATRIX_SAVE_FLAG);
    return NOERROR;
}

ECode GLES20Canvas::Save(
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nSave(mRenderer, saveFlags);
    return NOERROR;
}

ECode GLES20Canvas::SaveLayer(
    /* [in] */ IRectF* bounds,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (bounds == NULL) {
        AutoPtr<IRect> r;
        CRect::New((IRect**)&r);
        Boolean result;
        GetClipBounds(r, &result);
        CRectF::New(r, (IRectF**)&bounds);
    }
    Float left, top, right, bottom;
    bounds->Get(&left, &top, &right, &bottom);
    return SaveLayer(left, top, right, bottom, paint, saveFlags, res);
}

ECode GLES20Canvas::SaveLayer(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (left < right && top < bottom) {
        Paint* p = (Paint*)paint;
        Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
        *res = nSaveLayer(mRenderer, left, top, right, bottom, nativePaint, saveFlags);
        return NOERROR;
    }
    return Save(saveFlags, res);
}

ECode GLES20Canvas::SaveLayerAlpha(
    /* [in] */ IRectF* bounds,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (bounds == NULL) {
        AutoPtr<IRect> r;
        CRect::New((IRect**)&r);
        Boolean result;
        GetClipBounds(r, &result);
        CRectF::New(r, (IRectF**)&bounds);
    }
    Float left, top, right, bottom;
    bounds->Get(&left, &top, &right, &bottom);
    return SaveLayerAlpha(left, top, right, bottom,
            alpha, saveFlags, res);
}

ECode GLES20Canvas::SaveLayerAlpha(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (left < right && top < bottom) {
        *res = nSaveLayerAlpha(mRenderer, left, top, right, bottom, alpha, saveFlags);
        return NOERROR;
    }
    return Save(saveFlags, res);
}

ECode GLES20Canvas::Restore()
{
    nRestore(mRenderer);
    return NOERROR;
}

ECode GLES20Canvas::RestoreToCount(
    /* [in] */ Int32 saveCount)
{
    nRestoreToCount(mRenderer, saveCount);
    return NOERROR;
}

ECode GLES20Canvas::GetSaveCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = nGetSaveCount(mRenderer);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////
// Filtering
///////////////////////////////////////////////////////////////////////////

ECode GLES20Canvas::SetDrawFilter(
    /* [in] */ IDrawFilter* filter)
{
    Int64 nativeFilter = 0;
    if (filter != NULL) {
        nativeFilter = ((DrawFilter*)filter)->mNativeInstance;
    }
    mFilter = filter;
    nSetDrawFilter(mRenderer, nativeFilter);
    return NOERROR;
}

ECode GLES20Canvas::GetDrawFilter(
    /* [out] */ IDrawFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter)

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////
// Drawing
///////////////////////////////////////////////////////////////////////////

ECode GLES20Canvas::DrawArc(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean useCenter,
    /* [in] */ IPaint* paint)
{
    Paint* p = (Paint*)paint;
    nDrawArc(mRenderer, left, top, right, bottom,
            startAngle, sweepAngle, useCenter, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawARGB(
    /* [in] */ Int32 a,
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return DrawColor((a & 0xFF) << 24 | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF));
}

ECode GLES20Canvas::DrawPatch(
    /* [in] */ INinePatch* patch,
    /* [in] */ IRect* dst,
    /* [in] */ IPaint* paint)
{
    AutoPtr<IBitmap> bitmap;
    patch->GetBitmap((IBitmap**)&bitmap);
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    CBitmap* nBitmap = (CBitmap*)bitmap.Get();
    NinePatch* nPatch = (NinePatch*)patch;
    Int32 left, top, right, bottom;
    dst->Get(&left, &top, &right, &bottom);
    Int32 density;
    patch->GetDensity(&density);
    nDrawPatch(mRenderer, nBitmap->mNativeBitmap, nPatch->mNativeChunk,
            left, top, right, bottom, nativePaint, mDensity, density);
    return NOERROR;
}

ECode GLES20Canvas::DrawPatch(
    /* [in] */ INinePatch* patch,
    /* [in] */ IRectF* dst,
    /* [in] */ IPaint* paint)
{
    AutoPtr<IBitmap> bitmap;
    patch->GetBitmap((IBitmap**)&bitmap);
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    CBitmap* nBitmap = (CBitmap*)bitmap.Get();
    NinePatch* nPatch = (NinePatch*)patch;
    Float left, top, right, bottom;
    dst->Get(&left, &top, &right, &bottom);
    Int32 density;
    patch->GetDensity(&density);
    nDrawPatch(mRenderer, nBitmap->mNativeBitmap, nPatch->mNativeChunk,
            left, top, right, bottom, nativePaint, mDensity, density);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ IPaint* paint)
{
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    CBitmap* nBitmap = (CBitmap*)bitmap;
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, left, top, nativePaint, mDensity,
            mScreenDensity, nBitmap->mDensity);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IMatrix* matrix,
    /* [in] */ IPaint* paint)
{
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    CBitmap* nBitmap = (CBitmap*)bitmap;
    Matrix* m = (Matrix*)matrix;
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap,
            m->mNativeMatrix, nativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IRect* src,
    /* [in] */ IRect* dst,
    /* [in] */ IPaint* paint)
{
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;

    Int32 left, top, right, bottom;
    if (src == NULL) {
        left = top = 0;
        bitmap->GetWidth(&right);
        bitmap->GetHeight(&bottom);
    } else {
        src->Get(&left, &top, &right, &bottom);
    }

    Int32 dstLeft, dstTop, dstRight, dstBottom;
    dst->Get(&dstLeft, &dstTop, &dstRight, &dstBottom);
    CBitmap* nBitmap = (CBitmap*)bitmap;
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, left, top, right, bottom,
            dstLeft, dstTop, dstRight, dstBottom, nativePaint, mScreenDensity, nBitmap->mDensity);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IRect* src,
    /* [in] */ IRectF* dst,
    /* [in] */ IPaint* paint)
{
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;

    Int32 left, top, right, bottom;
    if (src == NULL) {
        left = top = 0;
        bitmap->GetWidth(&right);
        bitmap->GetHeight(&bottom);
    } else {
        src->Get(&left, &top, &right, &bottom);
    }

    Float dstLeft, dstTop, dstRight, dstBottom;
    dst->Get(&dstLeft, &dstTop, &dstRight, &dstBottom);
    CBitmap* nBitmap = (CBitmap*)bitmap;
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, left, top, right, bottom,
            dstLeft, dstTop, dstRight, dstBottom, nativePaint, mScreenDensity, nBitmap->mDensity);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean hasAlpha,
    /* [in] */ IPaint* paint)
{
    if (width < 0) {
        Logger::E("GLES20Canvas", "width must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (height < 0) {
        Logger::E("GLES20Canvas", "height must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (Elastos::Core::Math::Abs(stride) < width) {
        Logger::E("GLES20Canvas", "abs(stride) must be >= width");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 lastScanline = offset + (height - 1) * stride;
    Int32 length = colors->GetLength();

    if (offset < 0 || (offset + width > length) || lastScanline < 0 ||
            (lastScanline + width > length)) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    nDrawBitmap(mRenderer, colors, offset, stride, x, y,
            width, height, hasAlpha, nativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean hasAlpha,
    /* [in] */ IPaint* paint)
{
    return DrawBitmap(colors, offset, stride, (Float) x, (Float) y, width, height, hasAlpha, paint);
}

ECode GLES20Canvas::DrawBitmapMesh(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 meshWidth,
    /* [in] */ Int32 meshHeight,
    /* [in] */ ArrayOf<Float>* verts,
    /* [in] */ Int32 vertOffset,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorOffset,
    /* [in] */ IPaint* paint)
{
    FAIL_RETURN(ThrowIfCannotDraw(bitmap))
    if (meshWidth < 0 || meshHeight < 0 || vertOffset < 0 || colorOffset < 0) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (meshWidth == 0 || meshHeight == 0) {
        return NOERROR;
    }

    Int32 count = (meshWidth + 1) * (meshHeight + 1);
    FAIL_RETURN(CheckRange(verts->GetLength(), vertOffset, count * 2))

    if (colors != NULL) {
        FAIL_RETURN(CheckRange(colors->GetLength(), colorOffset, count))
    }

    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    CBitmap* nBitmap = (CBitmap*)bitmap;
    nDrawBitmapMesh(mRenderer, nBitmap->mNativeBitmap, meshWidth, meshHeight,
            verts, vertOffset, colors, colorOffset, nativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawCircle(
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float radius,
    /* [in] */ IPaint* paint)
{
    Paint* p = (Paint*)paint;
    nDrawCircle(mRenderer, cx, cy, radius, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawCircle(
    /* [in] */ ICanvasProperty* cx,
    /* [in] */ ICanvasProperty* cy,
    /* [in] */ ICanvasProperty* radius,
    /* [in] */ ICanvasProperty* paint)
{
    Int64 nCx, nCy, nRadius, nPaint;
    cx->GetNativeContainer(&nCx);
    cy->GetNativeContainer(&nCy);
    radius->GetNativeContainer(&nRadius);
    paint->GetNativeContainer(&nPaint);
    nDrawCircle(mRenderer, nCx, nCy, nRadius, nPaint);

    return NOERROR;
}

ECode GLES20Canvas::DrawRoundRect(
    /* [in] */ ICanvasProperty* left,
    /* [in] */ ICanvasProperty* top,
    /* [in] */ ICanvasProperty* right,
    /* [in] */ ICanvasProperty* bottom,
    /* [in] */ ICanvasProperty* rx,
    /* [in] */ ICanvasProperty* ry,
    /* [in] */ ICanvasProperty* paint)
{
    Int64 nLeft, nTop, nRight, nBottom, nRx, nRy, nPaint;
    left->GetNativeContainer(&nLeft);
    top->GetNativeContainer(&nTop);
    right->GetNativeContainer(&nRight);
    bottom->GetNativeContainer(&nBottom);
    rx->GetNativeContainer(&nRx);
    ry->GetNativeContainer(&nRy);
    paint->GetNativeContainer(&nPaint);
    nDrawRoundRect(mRenderer, nLeft, nTop, nRight, nBottom, nRx, nRy, nPaint);

    return NOERROR;
}

ECode GLES20Canvas::DrawColor(
    /* [in] */ Int32 color)
{
    return DrawColor(color, PorterDuffMode_SRC_OVER);
}

ECode GLES20Canvas::DrawColor(
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    nDrawColor(mRenderer, color, mode);
    return NOERROR;
}

ECode GLES20Canvas::DrawLine(
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float stopX,
    /* [in] */ Float stopY,
    /* [in] */ IPaint* paint)
{
    AutoPtr<ArrayOf<Float> > line = GetLineStorage();
    (*line)[0] = startX;
    (*line)[1] = startY;
    (*line)[2] = stopX;
    (*line)[3] = stopY;
    return DrawLines(line, 0, 4, paint);
}

ECode GLES20Canvas::DrawLines(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ IPaint* paint)
{
    if (count < 4) return NOERROR;

    if ((offset | count) < 0 || offset + count > pts->GetLength()) {
        Logger::E("GLES20Canvas", "The lines array must contain 4 elements per line.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Paint* p = (Paint*)paint;
    nDrawLines(mRenderer, pts, offset, count, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawLines(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ IPaint* paint)
{
    return DrawLines(pts, 0, pts->GetLength(), paint);
}

ECode GLES20Canvas::DrawOval(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ IPaint* paint)
{
    Paint* p = (Paint*)paint;
    nDrawOval(mRenderer, left, top, right, bottom, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawPaint(
    /* [in] */ IPaint* paint)
{
    AutoPtr<IRect> r = GetInternalClipBounds();
    nGetClipBounds(mRenderer, r);
    Int32 left, top, right, bottom;
    r->Get(&left, &top, &right, &bottom);
    return DrawRect(left, top, right, bottom, paint);
}

ECode GLES20Canvas::DrawPath(
    /* [in] */ IPath* path,
    /* [in] */ IPaint* paint)
{
    CPath* pathImpl = (CPath*)path;
    Paint* p = (Paint*)paint;
    if (pathImpl->mIsSimplePath) {
        if (pathImpl->mRects != NULL) {
            Handle64 nativeRegion;
            pathImpl->mRects->GetNativeRegion(&nativeRegion);
            nDrawRects(mRenderer, nativeRegion, p->mNativePaint);
        }
    } else {
        nDrawPath(mRenderer, pathImpl->mNativePath, p->mNativePaint);
    }
    return NOERROR;
}

ECode GLES20Canvas::DrawPicture(
    /* [in] */ IPicture* picture)
{
    picture->EndRecording();
    return NOERROR;
}

ECode GLES20Canvas::DrawPoint(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    AutoPtr<ArrayOf<Float> > point = GetPointStorage();
    (*point)[0] = x;
    (*point)[1] = y;
    return DrawPoints(point, 0, 2, paint);
}

ECode GLES20Canvas::DrawPoints(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ IPaint* paint)
{
    return DrawPoints(pts, 0, pts->GetLength(), paint);
}

ECode GLES20Canvas::DrawPoints(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ IPaint* paint)
{
    if (count < 2) return NOERROR;

    Paint* p = (Paint*)paint;
    nDrawPoints(mRenderer, pts, offset, count, p->mNativePaint);
    return NOERROR;
}

// Note: drawPosText just uses implementation in Canvas

ECode GLES20Canvas::DrawRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ IPaint* paint)
{
    if (left == right || top == bottom) return NOERROR;

    Paint* p = (Paint*)paint;
    nDrawRect(mRenderer, left, top, right, bottom, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawRect(
    /* [in] */ IRect* r,
    /* [in] */ IPaint* paint)
{
    Int32 left, top, right, bottom;
    r->Get(&left, &top, &right, &bottom);
    return DrawRect(left, top, right, bottom, paint);
}

ECode GLES20Canvas::DrawRect(
    /* [in] */ IRectF* r,
    /* [in] */ IPaint* paint)
{
    Float left, top, right, bottom;
    r->Get(&left, &top, &right, &bottom);
    return DrawRect(left, top, right, bottom, paint);
}

ECode GLES20Canvas::DrawRGB(
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return DrawColor(0xFF000000 | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF));
}

ECode GLES20Canvas::DrawRoundRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ IPaint* paint)
{
    Paint* p = (Paint*)paint;
    nDrawRoundRect(mRenderer, left, top, right, bottom, rx, ry, p->mNativePaint);
    return NOERROR;
}

ECode GLES20Canvas::DrawText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    if ((index | count | (index + count) | (text->GetLength() - index - count)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Paint* p = (Paint*)paint;
    nDrawText(mRenderer, text, index, count, x, y,
            p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    if (IString::Probe(text) != NULL || ISpannedString::Probe(text) != NULL ||
            ISpannableString::Probe(text) != NULL) {
        String str;
        text->ToString(&str);
        Paint* p = (Paint*)paint;
        nDrawText(mRenderer, str, start, end, x, y, p->mBidiFlags,
                p->mNativePaint, p->GetNativeTypeface());
    } else if (IGraphicsOperations::Probe(text) != NULL) {
        AutoPtr<IGraphicsOperations> gop = IGraphicsOperations::Probe(text);
        return gop->DrawText(this, start, end, x, y, paint);
    } else {
        AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);
        TextUtils::GetChars(text, start, end, buf, 0);
        Paint* p = (Paint*)paint;
        nDrawText(mRenderer, buf, 0, end - start, x, y,
                p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
        TemporaryBuffer::Recycle(buf);
    }
    return NOERROR;
}

ECode GLES20Canvas::DrawText(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Paint* p = (Paint*)paint;
    nDrawText(mRenderer, text, start, end, x, y,
            p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawText(
    /* [in] */ const String& text,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    Paint* p = (Paint*)paint;
    nDrawText(mRenderer, text, 0, text.GetLength(), x, y,
        p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawTextOnPath(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ IPath* path,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ IPaint* paint)
{
    if (index < 0 || index + count > text->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Paint* p = (Paint*)paint;
    CPath* pathImpl = (CPath*)path;
    nDrawTextOnPath(mRenderer, text, index, count, pathImpl->mNativePath, hOffset, vOffset,
            p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawTextOnPath(
    /* [in] */ const String& text,
    /* [in] */ IPath* path,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ IPaint* paint)
{
    if (text.GetLength() == 0) return NOERROR;

    Paint* p = (Paint*)paint;
    CPath* pathImpl = (CPath*)path;
    nDrawTextOnPath(mRenderer, text, pathImpl->mNativePath, hOffset, vOffset,
            p->mBidiFlags, p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawTextRun(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ IPaint* paint)
{
    if ((index | count | (text->GetLength() - index - count)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Paint* p = (Paint*)paint;
    nDrawTextRun(mRenderer, text, index, count, contextIndex, contextCount, x, y, isRtl,
            p->mNativePaint, p->GetNativeTypeface());
    return NOERROR;
}

ECode GLES20Canvas::DrawTextRun(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ IPaint* paint)
{
    Int32 textLen;
    text->GetLength(&textLen);
    if ((start | end | (end - start) | (textLen - end)) < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (IString::Probe(text) != NULL || ISpannedString::Probe(text) != NULL ||
            ISpannableString::Probe(text) != NULL) {
        String str;
        text->ToString(&str);
        Paint* p = (Paint*)paint;
        nDrawTextRun(mRenderer, str, start, end, contextStart,
                contextEnd, x, y, isRtl, p->mNativePaint, p->GetNativeTypeface());
    } else if (IGraphicsOperations::Probe(text) != NULL) {
        AutoPtr<IGraphicsOperations> gop = IGraphicsOperations::Probe(text);
        return gop->DrawTextRun(this, start, end,
                contextStart, contextEnd, x, y, isRtl, paint);
    } else {
        Int32 contextLen = contextEnd - contextStart;
        Int32 len = end - start;
        AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(contextLen);
        TextUtils::GetChars(text, contextStart, contextEnd, buf, 0);
        Paint* p = (Paint*)paint;
        nDrawTextRun(mRenderer, buf, start - contextStart, len, 0, contextLen,
                x, y, isRtl, p->mNativePaint, p->GetNativeTypeface());
        TemporaryBuffer::Recycle(buf);
    }

    return NOERROR;
}

ECode GLES20Canvas::DrawVertices(
    /* [in] */ CanvasVertexMode mode,
    /* [in] */ Int32 vertexCount,
    /* [in] */ ArrayOf<Float>* verts,
    /* [in] */ Int32 vertOffset,
    /* [in] */ ArrayOf<Float>* texs,
    /* [in] */ Int32 texOffset,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorOffset,
    /* [in] */ ArrayOf<Int16>* indices,
    /* [in] */ Int32 indexOffset,
    /* [in] */ Int32 indexCount,
    /* [in] */ IPaint* paint)
{
    // TODO: Implement
    return NOERROR;
}

ECode GLES20Canvas::GetRenderer(
    /* [out] */ Int64* renderer)
{
    VALIDATE_NOT_NULL(renderer)
    *renderer = mRenderer;
    return NOERROR;
}

ECode GLES20Canvas::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode GLES20Canvas::ResetDisplayListRenderer(
    /* [in] */ IRenderNode* node,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    nResetDisplayListRenderer(mRenderer, ((RenderNode*)node)->mNativeRenderNode,
            width, height);
    return NOERROR;
}


//.......................Native......................

static android::Canvas* get_canvas(Int64 canvasHandle)
{
    return reinterpret_cast<android::Canvas*>(canvasHandle);
}

typedef android::uirenderer::RenderNode NRenderNode;

ECode GLES20Canvas::nFinishRecording(
    /* [in] */ Int64 rendererPtr,
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res)
    *res = reinterpret_cast<Int64>(get_canvas(rendererPtr)->finishRecording());
    return NOERROR;
}

AutoPtr<IRect> GLES20Canvas::GetInternalClipBounds()
{
    if (mClipBounds == NULL) {
        CRect::New((IRect**)&mClipBounds);
    }
    return mClipBounds;
}

AutoPtr<IRectF> GLES20Canvas::GetPathBounds()
{
    if (mPathBounds == NULL) {
        CRectF::New((IRectF**)&mPathBounds);
    }
    return mPathBounds;
}

void GLES20Canvas::SetupFinalizer()
{
    if (mRenderer == 0) {
        Logger::E("GLES20Canvas", "Could not create GLES20Canvas renderer");
    }
    else {
        mFinalizer = new CanvasFinalizer(mRenderer);
    }
}

AutoPtr<ArrayOf<Float> > GLES20Canvas::GetPointStorage()
{
    if (mPoint == NULL) mPoint = ArrayOf<Float>::Alloc(2);
    return mPoint;
}

AutoPtr<ArrayOf<Float> > GLES20Canvas::GetLineStorage()
{
    if (mLine == NULL) mLine = ArrayOf<Float>::Alloc(4);
    return mLine;
}

Boolean GLES20Canvas::nIsAvailable()
{
    return NativeIsAvailable();
}

GLES20Canvas::~GLES20Canvas()
{
    mFinalizer = NULL;
}

Boolean GLES20Canvas::IsAvailable()
{
    return sIsAvailable;
}

Int64 GLES20Canvas::nCreateDisplayListRenderer(
    /* [in] */ Int64 nativeNode,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(nativeNode);
    return reinterpret_cast<Int64>(android::Canvas::create_recording_canvas(width, height, renderNode));
}

void GLES20Canvas::nResetDisplayListRenderer(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 nativeNode,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(nativeNode);
    get_canvas(rendererPtr)->resetRecording(width, height, renderNode);
}

Int32 GLES20Canvas::nGetMaximumTextureWidth()
{
    if (!android::uirenderer::Caches::hasInstance()) {
        android::uirenderer::renderthread::RenderProxy::staticFence();
    }
    return android::uirenderer::Caches::getInstance().maxTextureSize;
}

Int32 GLES20Canvas::nGetMaximumTextureHeight()
{
    if (!android::uirenderer::Caches::hasInstance()) {
        android::uirenderer::renderthread::RenderProxy::staticFence();
    }
    return android::uirenderer::Caches::getInstance().maxTextureSize;
}

void GLES20Canvas::nSetHighContrastText(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Boolean highContrastText)
{
    get_canvas(rendererPtr)->setHighContrastText(highContrastText);
}

void GLES20Canvas::nInsertReorderBarrier(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Boolean reorderEnable)
{
    get_canvas(rendererPtr)->insertReorderBarrier(reorderEnable);
}

Int32 GLES20Canvas::nCallDrawGLFunction(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 drawGLFunction)
{
    android::Functor* functor = reinterpret_cast<android::Functor*>(drawGLFunction);
    get_canvas(rendererPtr)->callDrawGLFunction(functor, NULL);
    return android::NO_ERROR;
}

Int32 GLES20Canvas::nDrawRenderNode(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ IRect* dirty,
    /* [in] */ Int32 flags)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    get_canvas(rendererPtr)->drawRenderNode(renderNode);
    return 0;
}

void GLES20Canvas::nDrawLayer(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 layerPtr,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    android::uirenderer::DeferredLayerUpdater* layer = reinterpret_cast<android::uirenderer::DeferredLayerUpdater*>(layerPtr);
    get_canvas(rendererPtr)->drawLayer(layer);
}

Boolean GLES20Canvas::nClipPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Int32 op)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    bool nonEmptyClip = get_canvas(rendererPtr)->clipPath(path, GraphicsNative::OpHandleToClipOp(op));
    return nonEmptyClip ? TRUE : FALSE;
}

Boolean GLES20Canvas::nClipRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 op)
{
    bool nonEmptyClip = get_canvas(rendererPtr)->clipRect(left, top, right, bottom,
            GraphicsNative::OpHandleToClipOp(op));
    return nonEmptyClip ? TRUE : FALSE;
}

Boolean GLES20Canvas::nClipRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 op)
{
    bool nonEmptyClip = get_canvas(rendererPtr)->clipRect(float(left), float(top),
            float(right), float(bottom), GraphicsNative::OpHandleToClipOp(op));
    return nonEmptyClip ? TRUE : FALSE;
}

Boolean GLES20Canvas::nGetClipBounds(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ IRect* rect)
{
    SkRect   r;
    SkIRect ir;
    bool result = get_canvas(rendererPtr)->getClipBounds(&r);

    if (!result) {
        r.setEmpty();
    }
    r.round(&ir);

    GraphicsNative::SkIRect2IRect(ir, rect);
    return result ? TRUE : FALSE;
}

Boolean GLES20Canvas::nQuickReject(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    bool result = get_canvas(rendererPtr)->quickRejectRect(left, top, right, bottom);
    return result ? TRUE : FALSE;
}

void GLES20Canvas::nTranslate(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    get_canvas(rendererPtr)->translate(dx, dy);
}

void GLES20Canvas::nSkew(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    get_canvas(rendererPtr)->skew(sx, sy);
}

void GLES20Canvas::nRotate(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float degrees)
{
    get_canvas(rendererPtr)->rotate(degrees);
}

void GLES20Canvas::nScale(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    get_canvas(rendererPtr)->scale(sx, sy);
}

void GLES20Canvas::nSetMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    get_canvas(rendererPtr)->setMatrix(matrix ? *matrix : SkMatrix::I());
}

void GLES20Canvas::nGetMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    get_canvas(rendererPtr)->getMatrix(matrix);
}

void GLES20Canvas::nConcatMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    get_canvas(rendererPtr)->concat(*matrix);
}

Int32 GLES20Canvas::nSave(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 flagsHandle)
{
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(flagsHandle);
    return static_cast<Int32>(get_canvas(rendererPtr)->save(flags));
}

Int32 GLES20Canvas::nSaveLayer(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int32 saveFlags)
{
    android::Paint* paint  = reinterpret_cast<android::Paint*>(paintPtr);
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(saveFlags);
    return static_cast<Int32>(get_canvas(rendererPtr)->saveLayer(left, top, right, bottom, paint, flags));
}

Int32 GLES20Canvas::nSaveLayerAlpha(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags)
{
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(saveFlags);
    return static_cast<Int32>(get_canvas(rendererPtr)->saveLayerAlpha(left, top, right, bottom, alpha, flags));
}

void GLES20Canvas::nRestore(
    /* [in] */ Int64 rendererPtr)
{
    android::Canvas* canvas = get_canvas(rendererPtr);
    if (canvas->getSaveCount() <= 1) {
        return; // cannot restore anymore
    }
    canvas->restore();
}

void GLES20Canvas::nRestoreToCount(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 saveCount)
{
    get_canvas(rendererPtr)->restoreToCount(saveCount);
}

Int32 GLES20Canvas::nGetSaveCount(
    /* [in] */ Int64 rendererPtr)
{
    return static_cast<Int32>(get_canvas(rendererPtr)->getSaveCount());
}

void GLES20Canvas::nSetDrawFilter(
    /* [in] */ Int64 renderer,
    /* [in] */ Int64 filter)
{
    get_canvas(renderer)->setDrawFilter(reinterpret_cast<SkDrawFilter*>(filter));
}

void GLES20Canvas::nDrawArc(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean useCenter,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawArc(left, top, right, bottom, startAngle, sweepAngle,
                                       useCenter, *paint);
}

void GLES20Canvas::nDrawPatch(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Int64 patchPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int32 screenDensity,
    /* [in] */ Int32 bitmapDensity)
{
    android::Canvas* canvas = get_canvas(rendererPtr);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapPtr)->bitmap();
    const android::Res_png_9patch* chunk = reinterpret_cast<android::Res_png_9patch*>(patchPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);

    if (CC_LIKELY(screenDensity == bitmapDensity || screenDensity == 0 || bitmapDensity == 0)) {
        canvas->drawNinePatch(bitmap, *chunk, left, top, right, bottom, paint);
    } else {
        canvas->save(android::SaveFlags::MatrixClip);

        SkScalar scale = screenDensity / (float)bitmapDensity;
        canvas->translate(left, top);
        canvas->scale(scale, scale);

        android::Paint filteredPaint;
        if (paint) {
            filteredPaint = *paint;
        }
        filteredPaint.setFilterQuality(kLow_SkFilterQuality);

        canvas->drawNinePatch(bitmap, *chunk, 0, 0, (right-left)/scale, (bottom-top)/scale,
                &filteredPaint);

        canvas->restore();
    }
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int32 canvasDensity,
    /* [in] */ Int32 screenDensity,
    /* [in] */ Int32 bitmapDensity)
{
    android::Canvas* canvas = get_canvas(rendererPtr);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapPtr)->bitmap();
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);

    if (canvasDensity == bitmapDensity || canvasDensity == 0 || bitmapDensity == 0) {
        if (screenDensity != 0 && screenDensity != bitmapDensity) {
            android::Paint filteredPaint;
            if (paint) {
                filteredPaint = *paint;
            }
            filteredPaint.setFilterQuality(kLow_SkFilterQuality);
            canvas->drawBitmap(bitmap, left, top, &filteredPaint);
        } else {
            canvas->drawBitmap(bitmap, left, top, paint);
        }
    } else {
        canvas->save(android::SaveFlags::MatrixClip);
        SkScalar scale = canvasDensity / (float)bitmapDensity;
        canvas->translate(left, top);
        canvas->scale(scale, scale);

        android::Paint filteredPaint;
        if (paint) {
            filteredPaint = *paint;
        }
        filteredPaint.setFilterQuality(kLow_SkFilterQuality);

        canvas->drawBitmap(bitmap, 0, 0, &filteredPaint);
        canvas->restore();
    }
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Int64 matrixPtr,
    /* [in] */ Int64 paintPtr)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapPtr)->bitmap();
    get_canvas(rendererPtr)->drawBitmap(bitmap, *matrix, paint);
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Float srcLeft,
    /* [in] */ Float srcTop,
    /* [in] */ Float srcRight,
    /* [in] */ Float srcBottom,
    /* [in] */ Float dstLeft,
    /* [in] */ Float dstTop,
    /* [in] */ Float dstRight,
    /* [in] */ Float dstBottom,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int32 screenDensity,
    /* [in] */ Int32 bitmapDensity)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);

    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapPtr)->bitmap();
    if (screenDensity != 0 && screenDensity != bitmapDensity) {
        android::Paint filteredPaint;
        if (paint) {
            filteredPaint = *paint;
        }
        filteredPaint.setFilterQuality(kLow_SkFilterQuality);
        get_canvas(rendererPtr)->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
                           dstLeft, dstTop, dstRight, dstBottom, &filteredPaint);
    } else {
        get_canvas(rendererPtr)->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
                           dstLeft, dstTop, dstRight, dstBottom, paint);
    }
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean hasAlpha,
    /* [in] */ Int64 paintPtr)
{
    // Note: If hasAlpha is false, kRGB_565_SkColorType will be used, which will
    // correct the alphaType to kOpaque_SkAlphaType.
    const SkImageInfo info = SkImageInfo::Make(width, height,
                               hasAlpha ? kN32_SkColorType : kRGB_565_SkColorType,
                               kPremul_SkAlphaType);
    SkBitmap bitmap;
    bitmap.setInfo(info);
    sk_sp<android::Bitmap> androidBitmap = android::Bitmap::allocateHeapBitmap(&bitmap, NULL);
    if (!androidBitmap) {
        return;
    }

    if (!GraphicsNative::SetPixels(colors, offset, stride, 0, 0, width, height, bitmap)) {
        return;
    }

    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawBitmap(*androidBitmap, left, top, paint);
}

void GLES20Canvas::nDrawBitmapMesh(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ Int32 meshWidth,
    /* [in] */ Int32 meshHeight,
    /* [in] */ ArrayOf<Float>* vertices,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorOffset,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapPtr)->bitmap();

    Float* verticesArray = vertices ? vertices->GetPayload() + offset : NULL;
    Int32* colorsArray = colors ? colors->GetPayload() + colorOffset : NULL;

    get_canvas(rendererPtr)->drawBitmapMesh(bitmap, meshWidth, meshHeight,
                                             verticesArray, colorsArray, paint);
}

void GLES20Canvas::nDrawCircle(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawCircle(x, y, radius, *paint);
}

void GLES20Canvas::nDrawCircle(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 xPropPtr,
    /* [in] */ Int64 yPropPtr,
    /* [in] */ Int64 radiusPropPtr,
    /* [in] */ Int64 paintPropPtr)
{
    android::uirenderer::CanvasPropertyPrimitive* xProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(xPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* yProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(yPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* radiusProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(radiusPropPtr);
    android::uirenderer::CanvasPropertyPaint* paintProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPaint*>(paintPropPtr);
    get_canvas(rendererPtr)->drawCircle(xProp, yProp, radiusProp, paintProp);
}

void GLES20Canvas::nDrawRoundRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 leftPropPtr,
    /* [in] */ Int64 topPropPtr,
    /* [in] */ Int64 rightPropPtr,
    /* [in] */ Int64 bottomPropPtr,
    /* [in] */ Int64 rxPropPtr,
    /* [in] */ Int64 ryPropPtr,
    /* [in] */ Int64 paintPropPtr)
{
    android::uirenderer::CanvasPropertyPrimitive* leftProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(leftPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* topProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(topPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* rightProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(rightPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* bottomProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(bottomPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* rxProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(rxPropPtr);
    android::uirenderer::CanvasPropertyPrimitive* ryProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPrimitive*>(ryPropPtr);
    android::uirenderer::CanvasPropertyPaint* paintProp =
            reinterpret_cast<android::uirenderer::CanvasPropertyPaint*>(paintPropPtr);
    get_canvas(rendererPtr)->drawRoundRect(leftProp, topProp, rightProp, bottomProp, rxProp, ryProp, paintProp);
}

void GLES20Canvas::nDrawColor(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 color,
    /* [in] */ Int32 modeHandle)
{
    SkBlendMode mode = static_cast<SkBlendMode>(modeHandle);
    get_canvas(rendererPtr)->drawColor(color, mode);
}

void GLES20Canvas::nDrawLines(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintPtr)
{
    if (points == NULL) return;

    Float* floats = points->GetPayload();
    const Int32 length = points->GetLength();

    if ((offset | count) < 0 || offset + count > length) {
        return;
    }

    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawLines(floats + offset, count, *paint);
}

void GLES20Canvas::nDrawOval(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawOval(left, top, right, bottom, *paint);
}

void GLES20Canvas::nDrawPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Int64 paintPtr)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawPath(*path, *paint);
}

void GLES20Canvas::nDrawRects(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 regionPtr,
    /* [in] */ Int64 paintPtr)
{
    SkRegion* region = reinterpret_cast<SkRegion*>(regionPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawRegion(*region, *paint);
}

void GLES20Canvas::nDrawPoints(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintPtr)
{
    if (points == NULL) return;

    Float* floats = points->GetPayload();
    Int32 length = points->GetLength();

    if ((offset | count) < 0 || offset + count > length) {
        return;
    }

    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawPoints(floats + offset, count, *paint);
}

void GLES20Canvas::nDrawRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawRect(left, top, right, bottom, *paint);
}

void GLES20Canvas::nDrawRoundRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ Int64 paintPtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    get_canvas(rendererPtr)->drawRoundRect(left, top, right, bottom, rx, ry, *paint);
}

void GLES20Canvas::nDrawText(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);
    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text);
    get_canvas(rendererPtr)->drawText(chars->GetPayload() + index, 0, count, count, x, y,
                                       bidiFlags, *paint, typeface);
}

void GLES20Canvas::nDrawText(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);
    const int count = end - start;
    const AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    get_canvas(rendererPtr)->drawText(chars->GetPayload() + start, 0, count, count, x, y,
                                       bidiFlags, *paint, typeface);
}

void GLES20Canvas::nDrawTextOnPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);

    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text);

    get_canvas(rendererPtr)->drawTextOnPath(chars->GetPayload() + index, count, bidiFlags, *path,
                   hOffset, vOffset, *paint, typeface);
}

void GLES20Canvas::nDrawTextOnPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ const String& text,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);

    const AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    int count = text.GetLength();

    get_canvas(rendererPtr)->drawTextOnPath(chars->GetPayload(), count, bidiFlags, *path,
                   hOffset, vOffset, *paint, typeface);
}

void GLES20Canvas::nDrawTextRun(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);

    const int bidiFlags = isRtl ? minikin::kBidi_Force_RTL : minikin::kBidi_Force_LTR;
    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text);
    get_canvas(rendererPtr)->drawText(chars->GetPayload() + contextIndex, index - contextIndex, count,
                                       contextCount, x, y, bidiFlags, *paint, typeface);
}

void GLES20Canvas::nDrawTextRun(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintPtr);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefacePtr);

    int bidiFlags = isRtl ? minikin::kBidi_Force_RTL : minikin::kBidi_Force_LTR;
    Int32 count = end - start;
    Int32 contextCount = contextEnd - contextStart;
    const AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    get_canvas(rendererPtr)->drawText(chars->GetPayload() + contextStart, start - contextStart,
            count, contextCount, x, y, bidiFlags, *paint, typeface);
}

} // namespace View
} // namespace Droid
} // namespace Elastos