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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Opengl.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/Canvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/CPicture.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/DrawFilter.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkBlendMode.h>
#include <skia/core/SkPaint.h>
#include <skia/core/SkRect.h>
#include <skia/core/SkScalar.h>
#include <skia/core/SkMatrix.h>
#include <skia/core/SkGraphics.h>
#include <skia/core/SkShader.h>
#include <skia/core/SkDrawFilter.h>
#include <skia/core/SkRegion.h>
#include <skia/private/SkTemplates.h>
#include <utils/RefBase.h>
#include <minikin/Layout.h>
#include <minikin/MinikinFont.h>
#include <hwui/Canvas.h>
#include <hwui/hwui/Paint.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::IGraphicsOperations;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;
using minikin::Layout;

namespace Elastos {
namespace Droid {
namespace Graphics {

#ifndef TO_PAINT
#define TO_PAINT(obj) ((Paint*)obj)
#endif

//=======================================================================
// Canvas
//=======================================================================
CAR_INTERFACE_IMPL(Canvas, Object, ICanvas)

Canvas::Canvas()
    : mDensity(IBitmap::DENSITY_NONE)
    , mScreenDensity(IBitmap::DENSITY_NONE)
    , mSurfaceFormat(0)
{}

Canvas::~Canvas()
{
    // If the constructor threw an exception before setting mNativeCanvas, the native finalizer
    // must not be invoked.
    Dispose();
}

ECode Canvas::constructor()
{
    Boolean is = FALSE;
    if (IsHardwareAccelerated(&is), !is) {
        // 0 means no native bitmap
        mNativeCanvas = InitRaster(NULL);
    }
    return NOERROR;
}

ECode Canvas::constructor(
    /* [in] */ IBitmap* bitmap)
{
    Boolean isMutable = FALSE;
    if (bitmap->IsMutable(&isMutable), !isMutable) {
        Logger::E("Canvas", "Immutable bitmap passed to Canvas constructor");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(ThrowIfCannotDraw(bitmap));
    mNativeCanvas = InitRaster(bitmap);
    mBitmap = bitmap;
    mBitmap->GetDensity(&mDensity);
    return NOERROR;
}

ECode Canvas::constructor(
    /* [in] */ Int64 nativeCanvas)
{
    if (nativeCanvas == 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mNativeCanvas = nativeCanvas;
    mDensity = CBitmap::GetDefaultDensity();
    return NOERROR;
}

AutoPtr<IGL> Canvas::GetGL()
{
    return NULL;
}

ECode Canvas::IsHardwareAccelerated(
    /* [out] */ Boolean* isAccelerated)
{
    VALIDATE_NOT_NULL(isAccelerated)
    *isAccelerated = FALSE;
    return NOERROR;
}

/**
 * Specify a bitmap for the canvas to draw into.  As a side-effect, also
 * updates the canvas's target density to match that of the bitmap.
 *
 * @param bitmap Specifies a mutable bitmap for the canvas to draw into.
 *
 * @see #setDensity(int)
 * @see #getDensity()
 */
ECode Canvas::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    Boolean result;
    if (IsHardwareAccelerated(&result), result) {
        Logger::E("Canvas", "RuntimeException: Can't set a bitmap device on a HW accelerated canvas");
        return E_RUNTIME_EXCEPTION;
    }


    if (bitmap == NULL) {
        NativeSetBitmap(mNativeCanvas, NULL, FALSE);
        mDensity = IBitmap::DENSITY_NONE;
    }
    else {
        Boolean isMutable;
        bitmap->IsMutable(&isMutable);
        if (!isMutable) {
            Logger::E("Canvas", "IllegalStateException");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        FAIL_RETURN(ThrowIfCannotDraw(bitmap));

        NativeSetBitmap(mNativeCanvas, bitmap, TRUE);
        mDensity = ((CBitmap*)bitmap)->mDensity;
    }

    mBitmap = bitmap;
    return NOERROR;
}

/**
 * Set the viewport dimensions if this canvas is GL based. If it is not,
 * this method is ignored and no exception is thrown.
 *
 *  @param width    The width of the viewport
 *  @param height   The height of the viewport
 */
ECode Canvas::SetViewport(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode Canvas::SetHighContrastText(
    /* [in] */ Boolean highContrastText)
{
    return NOERROR;
}

ECode Canvas::InsertReorderBarrier()
{
    return NOERROR;
}

ECode Canvas::InsertInorderBarrier()
{
    return NOERROR;
}

/**
 * Return true if the device that the current layer draws into is opaque
 * (i.e. does not support per-pixel alpha).
 *
 * @return true if the device that the current layer draws into is opaque
 */
ECode Canvas::IsOpaque(
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque)
    *isOpaque = NativeIsOpaque(mNativeCanvas);
    return NOERROR;
}

/**
 * Returns the width of the current drawing layer
 *
 * @return the width of the current drawing layer
 */
ECode Canvas::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = NativeGetWidth(mNativeCanvas);
    return NOERROR;
}

/**
 * Returns the height of the current drawing layer
 *
 * @return the height of the current drawing layer
 */
ECode Canvas::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = NativeGetHeight(mNativeCanvas);
    return NOERROR;
}

/**
 * <p>Returns the target density of the canvas.  The default density is
 * derived from the density of its backing bitmap, or
 * {@link Bitmap#DENSITY_NONE} if there is not one.</p>
 *
 * @return Returns the current target density of the canvas, which is used
 * to determine the scaling factor when drawing a bitmap into it.
 *
 * @see #setDensity(int)
 * @see Bitmap#getDensity()
 */
ECode Canvas::GetDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density)
    *density = mDensity;
    return NOERROR;
}

/**
 * <p>Specifies the density for this Canvas' backing bitmap.  This modifies
 * the target density of the canvas itself, as well as the density of its
 * backing bitmap via {@link Bitmap#setDensity(int) Bitmap.setDensity(int)}.
 *
 * @param density The new target density of the canvas, which is used
 * to determine the scaling factor when drawing a bitmap into it.  Use
 * {@link Bitmap#DENSITY_NONE} to disable bitmap scaling.
 *
 * @see #getDensity()
 * @see Bitmap#setDensity(int)
 */
ECode Canvas::SetDensity(
    /* [in] */ Int32 density)
{
    if (mBitmap != NULL) {
        mBitmap->SetDensity(density);
    }
    mDensity = density;
    return NOERROR;
}

ECode Canvas::SetScreenDensity(
    /* [in] */ Int32 density)
{
    mScreenDensity = density;
    return NOERROR;
}

/**
 * Returns the maximum allowed width for bitmaps drawn with this canvas.
 * Attempting to draw with a bitmap wider than this value will result
 * in an error.
 *
 * @see #getMaximumBitmapHeight()
 */
ECode Canvas::GetMaximumBitmapWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = MAXMIMUM_BITMAP_SIZE;
    return NOERROR;
}

/**
 * Returns the maximum allowed height for bitmaps drawn with this canvas.
 * Attempting to draw with a bitmap taller than this value will result
 * in an error.
 *
 * @see #getMaximumBitmapWidth()
 */
ECode Canvas::GetMaximumBitmapHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = MAXMIMUM_BITMAP_SIZE;
    return NOERROR;
}

/**
 * Saves the current matrix and clip onto a private stack. Subsequent
 * calls to translate,scale,rotate,skew,concat or clipRect,clipPath
 * will all operate as usual, but when the balancing call to restore()
 * is made, those calls will be forgotten, and the settings that existed
 * before the save() will be reinstated.
 *
 * @return The value to pass to restoreToCount() to balance this save()
 */
ECode Canvas::Save(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = NativeSave(mNativeCanvas, MATRIX_SAVE_FLAG | CLIP_SAVE_FLAG);;
    return NOERROR;
}

/**
 * Based on saveFlags, can save the current matrix and clip onto a private
 * stack. Subsequent calls to translate,scale,rotate,skew,concat or
 * clipRect,clipPath will all operate as usual, but when the balancing
 * call to restore() is made, those calls will be forgotten, and the
 * settings that existed before the save() will be reinstated.
 *
 * @param saveFlags flag bits that specify which parts of the Canvas state
 *                  to save/restore
 * @return The value to pass to restoreToCount() to balance this save()
 */
ECode Canvas::Save(
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = NativeSave(mNativeCanvas, saveFlags);
    return NOERROR;
}

ECode Canvas::SaveLayer(
    /* [in] */ IRectF* _bounds,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr<IRectF> bounds = _bounds;
    if (bounds == NULL) {
        AutoPtr<IRect> r;
        GetClipBounds((IRect**)&r);
        CRectF::New(r, (IRectF**)&bounds);
    }
    Float left = 0, top = 0, right = 0, bottom = 0;
    bounds->Get(&left, &top, &right, &bottom);
    return SaveLayer(left, top, right, bottom, paint, saveFlags, count);
}

ECode Canvas::SaveLayer(
    /* [in] */ /*@Nullable*/ IRectF* bounds,
    /* [in] */ /*@Nullable*/ IPaint* paint,
    /* [out] */ Int32* count)
{
    return SaveLayer(bounds, paint, ALL_SAVE_FLAG, count);
}

/**
 * Helper version of saveLayer() that takes 4 values rather than a RectF.
 */
ECode Canvas::SaveLayer(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    *count = NativeSaveLayer(mNativeCanvas, left, top, right, bottom,
            paint != NULL ? TO_PAINT(paint)->mNativePaint : 0,
            saveFlags);
    return NOERROR;
}

ECode Canvas::SaveLayer(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ /*@Nullable*/ IPaint* paint,
    /* [out] */ Int32* count)
{
    return SaveLayer(left, top, right, bottom, paint, ALL_SAVE_FLAG, count);
}

ECode Canvas::SaveLayerAlpha(
    /* [in] */ IRectF* _bounds,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    AutoPtr<IRectF> bounds = _bounds;
    if (bounds == NULL) {
        AutoPtr<IRect> r;
        GetClipBounds((IRect**)&r);
        CRectF::New(r, (IRectF**)&bounds);
    }
    Float left = 0, top = 0, right = 0, bottom = 0;
    bounds->Get(&left, &top, &right, &bottom);
    return SaveLayerAlpha(left, top, right, bottom, alpha, saveFlags, count);
}

ECode Canvas::SaveLayerAlpha(
    /* [in] */ /*@Nullable*/ IRectF* bounds,
    /* [in] */ Int32 alpha,
    /* [out] */ Int32* count)
{
    return SaveLayerAlpha(bounds, alpha, ALL_SAVE_FLAG, count);
}

ECode Canvas::SaveLayerAlpha(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    alpha = Elastos::Core::Math::Min(255, Elastos::Core::Math::Max(0, alpha));
    *count = NativeSaveLayerAlpha(mNativeCanvas, left, top, right, bottom,
                                 alpha, saveFlags);
    return NOERROR;
}

ECode Canvas::SaveLayerAlpha(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 alpha,
    /* [out] */ Int32* count)
{
    return SaveLayerAlpha(left, top, right, bottom, alpha, ALL_SAVE_FLAG, count);
}

ECode Canvas::Restore()
{
    NativeRestore(mNativeCanvas);
    return NOERROR;
}

ECode Canvas::GetSaveCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = NativeGetSaveCount(mNativeCanvas);
    return NOERROR;
}

ECode Canvas::RestoreToCount(
    /* [in] */ Int32 saveCount)
{
    NativeRestoreToCount(mNativeCanvas, saveCount);
    return NOERROR;
}

ECode Canvas::Translate(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeTranslate(mNativeCanvas, dx, dy);
    return NOERROR;
}

ECode Canvas::Scale(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    NativeScale(mNativeCanvas, sx, sy);
    return NOERROR;
}

ECode Canvas::Scale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    Translate(px, py);
    Scale(sx, sy);
    Translate(-px, -py);
    return NOERROR;
}

ECode Canvas::Rotate(
    /* [in] */ Float degrees)
{
    NativeRotate(mNativeCanvas, degrees);
    return NOERROR;
}

ECode Canvas::Rotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    Translate(px, py);
    Rotate(degrees);
    Translate(-px, -py);
    return NOERROR;
}

ECode Canvas::Skew(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    NativeSkew(mNativeCanvas, sx, sy);
    return NOERROR;
}

ECode Canvas::Concat(
    /* [in] */ IMatrix* matrix)
{
    if (matrix != NULL) {
        Int64 nativeMatrix = ((Matrix*)matrix)->mNativeMatrix;
        NativeConcat(mNativeCanvas, nativeMatrix);
    }
    return NOERROR;
}

ECode Canvas::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    Int64 nativeMatrix = 0;
    if (matrix != NULL) {
        nativeMatrix = ((Matrix*)matrix)->mNativeMatrix;
    }
    NativeSetMatrix(mNativeCanvas, nativeMatrix);
    return NOERROR;
}

ECode Canvas::GetMatrix(
    /* [in, out] */ IMatrix* ctm)
{
    VALIDATE_NOT_NULL(ctm)

    Int64 nativeMatrix = ((Matrix*)ctm)->mNativeMatrix;
    NativeGetCTM(mNativeCanvas, nativeMatrix);
    return NOERROR;
}

ECode Canvas::GetMatrix(
    /* [out] */ IMatrix** metrix)
{
    VALIDATE_NOT_NULL(metrix)

    FAIL_RETURN(CMatrix::New(metrix));
    return GetMatrix(*metrix);
}

ECode Canvas::ClipRect(
    /* [in] */ IRectF* rect,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty)
    assert(rect != NULL);

    CRectF* rect_ = (CRectF*)rect;
    *isNotEmpty = NativeClipRect(mNativeCanvas,
        rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom, op);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ IRect* rect,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty)
    assert(rect != NULL);

    CRect* rect_ = (CRect*)rect;
    *isNotEmpty = NativeClipRect(mNativeCanvas,
        rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom, op);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ IRectF* rect,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);
    assert(rect != NULL);

    CRectF* rect_ = (CRectF*)rect;
    *isNotEmpty = NativeClipRect(mNativeCanvas,
        rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ IRect* rect,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);
    assert(rect != NULL);

    CRect* rect_ = (CRect*)rect;
    *isNotEmpty = NativeClipRect(mNativeCanvas,
        rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);

    *isNotEmpty = NativeClipRect(mNativeCanvas, left, top, right, bottom, op);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);

    *isNotEmpty = NativeClipRect(mNativeCanvas, left, top, right, bottom,
        RegionOp_INTERSECT);
    return NOERROR;
}

ECode Canvas::ClipRect(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);

    *isNotEmpty = NativeClipRect(mNativeCanvas, left, top, right, bottom,
        RegionOp_INTERSECT);
    return NOERROR;
}

ECode Canvas::ClipPath(
    /* [in] */ IPath* path,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty);
    assert(path != NULL);

    Int64 nativePath = ((CPath*)path)->mNativePath;
    *isNotEmpty = NativeClipPath(mNativeCanvas, nativePath, op);
    return NOERROR;
}

ECode Canvas::ClipPath(
    /* [in] */ IPath* path,
    /* [out] */ Boolean* isNotEmpty)
{
    return ClipPath(path, RegionOp_INTERSECT, isNotEmpty);
}

ECode Canvas::ClipRegion(
    /* [in] */ IRegion* region,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty)
    *isNotEmpty = FALSE;
    return NOERROR;
}

ECode Canvas::ClipRegion(
    /* [in] */ IRegion* region,
    /* [out] */ Boolean* isNotEmpty)
{
    return ClipRegion(region, RegionOp_INTERSECT, isNotEmpty);
}

ECode Canvas::GetDrawFilter(
    /* [out] */ IDrawFilter** filter)
{
    VALIDATE_NOT_NULL(filter)

    *filter = mDrawFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode Canvas::SetDrawFilter(
    /* [in] */ IDrawFilter* filter)
{
    Int64 nativeFilter = 0;
    if (filter != NULL) {
        nativeFilter = ((DrawFilter*)filter)->mNativeInstance;
    }
    mDrawFilter = filter;
    NativeSetDrawFilter(mNativeCanvas, nativeFilter);
    return NOERROR;
}

ECode Canvas::QuickReject(
    /* [in] */ IRectF* rect,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* isNotIntersect)
{
    VALIDATE_NOT_NULL(isNotIntersect);

    CRectF* rect_ = (CRectF*)rect;
    *isNotIntersect = NativeQuickReject(mNativeCanvas,
        rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom);
    return NOERROR;
}

ECode Canvas::QuickReject(
    /* [in] */ IPath* path,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* isNotIntersect)
{
    VALIDATE_NOT_NULL(isNotIntersect);
    assert(path != NULL);

    Int64 nativePath = ((CPath*)path)->mNativePath;
    *isNotIntersect = NativeQuickReject(mNativeCanvas, nativePath);
    return NOERROR;
}

ECode Canvas::QuickReject(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ CanvasEdgeType type,
    /* [out] */ Boolean* isNotIntersect)
{
    VALIDATE_NOT_NULL(isNotIntersect)

    *isNotIntersect = NativeQuickReject(mNativeCanvas, left, top, right, bottom);
    return NOERROR;
}

ECode Canvas::GetClipBounds(
    /* [in] */ IRect* bounds,
    /* [out] */ Boolean* isNotEmpty)
{
    VALIDATE_NOT_NULL(isNotEmpty)

    *isNotEmpty = NativeGetClipBounds(mNativeCanvas, bounds);
    return NOERROR;
}

ECode Canvas::GetClipBounds(
    /* [out] */ IRect** bounds)
{
    VALIDATE_NOT_NULL(bounds)

    FAIL_RETURN(CRect::New(bounds));
    Boolean isNotEmpty;
    return GetClipBounds(*bounds, &isNotEmpty);
}

ECode Canvas::DrawRGB(
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return DrawColor(Color::Rgb(r, g, b));
}

ECode Canvas::DrawARGB(
    /* [in] */ Int32 a,
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return DrawColor(Color::Argb(a, r, g, b));
}

ECode Canvas::DrawColor(
    /* [in] */ Int32 color)
{
    NativeDrawColor(mNativeCanvas, color, PorterDuffMode_SRC_OVER);
    return NOERROR;
}

ECode Canvas::DrawColor(
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    NativeDrawColor(mNativeCanvas, color, mode);
    return NOERROR;
}

ECode Canvas::DrawPaint(
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    NativeDrawPaint(mNativeCanvas, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawPoints(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ IPaint* paint)
{
    NativeDrawPoints(mNativeCanvas, pts, offset, count, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawPoints(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ IPaint* paint)
{
    return DrawPoints(pts, 0, pts->GetLength(), paint);
}

ECode Canvas::DrawPoint(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);
    NativeDrawPoint(mNativeCanvas, x, y, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawLine(
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float stopX,
    /* [in] */ Float stopY,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);
    Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
    NativeDrawLine(mNativeCanvas, startX, startY, stopX, stopY, nativePaint);
    return NOERROR;
}

ECode Canvas::DrawLines(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ IPaint* paint)
{
    NativeDrawLines(mNativeCanvas, pts, offset, count, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawLines(
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ IPaint* paint)
{
    return DrawLines(pts, 0, pts->GetLength(), paint);
}

ECode Canvas::DrawRect(
    /* [in] */ IRectF* rect,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    CRectF* rect_ = (CRectF*)rect;
    NativeDrawRect(mNativeCanvas,
            rect_->mLeft, rect_->mTop, rect_->mRight, rect_->mBottom, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawRect(
    /* [in] */ IRect* r,
    /* [in] */ IPaint* paint)
{
    assert(r != NULL);

    CRect* r_ = (CRect*)r;
    return DrawRect(r_->mLeft, r_->mTop, r_->mRight, r_->mBottom, paint);
}

ECode Canvas::DrawRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
    NativeDrawRect(mNativeCanvas, left, top, right, bottom, nativePaint);
    return NOERROR;
}

ECode Canvas::DrawOval(
    /* [in] */ IRectF* oval,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    if (oval == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    CRectF* _oval = (CRectF*)oval;
    return DrawOval(_oval->mLeft, _oval->mTop, _oval->mRight, _oval->mBottom, paint);
}

ECode Canvas::DrawOval(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ /*@NonNull*/ IPaint* paint)
{
    NativeDrawOval(mNativeCanvas, left, top, right, bottom, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawCircle(
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float radius,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
    NativeDrawCircle(mNativeCanvas, cx, cy, radius, nativePaint);
    return NOERROR;
}

ECode Canvas::DrawArc(
    /* [in] */ IRectF* oval,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean useCenter,
    /* [in] */ IPaint* paint)
{
    CRectF* _oval = (CRectF*)oval;
    return DrawArc(_oval->mLeft, _oval->mTop, _oval->mRight, _oval->mBottom, startAngle, sweepAngle, useCenter,
            paint);
}

ECode Canvas::DrawArc(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean useCenter,
    /* [in] */ /*@NonNull*/ IPaint* paint)
{
    assert(paint != NULL);
    NativeDrawArc(mNativeCanvas, left, top, right, bottom, startAngle, sweepAngle,
            useCenter, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawRoundRect(
    /* [in] */ IRectF* rect,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    CRectF* _rect = (CRectF*)rect;
    return DrawRoundRect(_rect->mLeft, _rect->mTop, _rect->mRight, _rect->mBottom, rx, ry, paint);
}

ECode Canvas::DrawRoundRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ /*@NonNull*/ IPaint* paint)
{
    NativeDrawRoundRect(mNativeCanvas, left, top, right, bottom, rx, ry, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawPath(
    /* [in] */ IPath* path,
    /* [in] */ IPaint* paint)
{
    assert(path != NULL);
    assert(paint != NULL);

    Int64 nativePath = ((CPath*)path)->mNativePath;
    Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
    NativeDrawPath(mNativeCanvas, nativePath, nativePaint);
    return NOERROR;
}

ECode Canvas::ThrowIfCannotDraw(
    /* [in] */ IBitmap* bitmap)
{
    assert(bitmap != NULL);

    Boolean isRecycled;
    bitmap->IsRecycled(&isRecycled);
    if (isRecycled) {
        Logger::E("Canvas", "RuntimeException: trying to use a recycled bitmap %s", TO_CSTR(bitmap));
        return E_RUNTIME_EXCEPTION;
    }

    Boolean bv = FALSE;
    BitmapConfig config;
    if (!(bitmap->IsPremultiplied(&bv), bv)
        && (bitmap->GetConfig(&config), config) == BitmapConfig_ARGB_8888 &&
            (bitmap->HasAlpha(&bv), bv)) {
        Logger::E("Canvas", "RuntimeException: trying to use a non-premultiplied bitmap %s", TO_CSTR(bitmap));
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode Canvas::DrawPatch(
    /* [in] */ /*@NonNull*/ INinePatch* patch,
    /* [in] */ /*@NonNull*/ IRect* dst,
    /* [in] */ /*@Nullable*/ IPaint* paint)
{
    assert(patch != NULL && dst != NULL);
    ((NinePatch*)patch)->DrawSoftware(this, dst, paint);
    return NOERROR;
}

ECode Canvas::DrawPatch(
    /* [in] */ /*@NonNull*/ INinePatch* patch,
    /* [in] */ IRectF* dst,
    /* [in] */ IPaint* paint)
{
    assert(patch != NULL && dst != NULL);
    ((NinePatch*)patch)->DrawSoftware(this, dst, paint);
    return NOERROR;
}

ECode Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ IPaint* paint)
{
    assert(bitmap != NULL);

    FAIL_RETURN(ThrowIfCannotDraw(bitmap));
    Handle64 nativeBitmap = ((CBitmap*)bitmap)->mNativeBitmap;
    Int32 density;
    bitmap->GetDensity(&density);
    Int64 nativePaint = paint ? (TO_PAINT(paint)->mNativePaint) : 0;
    NativeDrawBitmap(mNativeCanvas, nativeBitmap, left, top,
        nativePaint, mDensity, mScreenDensity, density);
    return NOERROR;
}

ECode Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IRect* src,
    /* [in] */ IRectF* dst,
    /* [in] */ IPaint* paint)
{
    assert(bitmap != NULL);

    if (dst == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(ThrowIfCannotDraw(bitmap));
    Int64 nativePaint = paint == NULL ? 0 : TO_PAINT(paint)->mNativePaint;

    Float left, top, right, bottom;
    Int32 iv = 0;
    if (src == NULL) {
      left = top = 0;
      bitmap->GetWidth(&iv);
      right = iv;
      bitmap->GetHeight(&iv);
      bottom = iv;
    }
    else {
      src->GetLeft(&iv);
      left = iv;
      src->GetRight(&iv);
      right = iv;
      src->GetTop(&iv);
      top = iv;
      src->GetBottom(&iv);
      bottom = iv;
    }

    CRectF* _dst = (CRectF*)dst;
    Int32 density = 0;
    bitmap->GetDensity(&density);
    Handle64 nativeBitmap = ((CBitmap*)bitmap)->mNativeBitmap;
    NativeDrawBitmap(mNativeCanvas, nativeBitmap, left, top, right, bottom,
          _dst->mLeft, _dst->mTop, _dst->mRight, _dst->mBottom, nativePaint, mScreenDensity,
          density);
    return NOERROR;
}

ECode Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IRect* src,
    /* [in] */ IRect* dst,
    /* [in] */ IPaint* paint)
{
    assert(bitmap != NULL);

    if (dst == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(ThrowIfCannotDraw(bitmap));
    Int64 nativePaint = paint == NULL ? 0 : TO_PAINT(paint)->mNativePaint;

    Int32 left, top, right, bottom;
    if (src == NULL) {
        left = top = 0;
        bitmap->GetWidth(&right);
        bitmap->GetHeight(&bottom);
    } else {
        src->GetLeft(&left);
        src->GetRight(&right);
        src->GetTop(&top);
        src->GetBottom(&bottom);
    }

    CRect* _dst = (CRect*)dst;
    Int32 density = 0;
    bitmap->GetDensity(&density);
    Handle64 nativeBitmap = ((CBitmap*)bitmap)->mNativeBitmap;
    NativeDrawBitmap(mNativeCanvas, nativeBitmap, left, top, right, bottom,
        _dst->mLeft, _dst->mTop, _dst->mRight, _dst->mBottom, nativePaint, mScreenDensity,
        density);
    return NOERROR;
}

ECode Canvas::DrawBitmap(
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
    // check for valid input
    if (width < 0) {
//        throw new IllegalArgumentException("width must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (height < 0) {
//        throw new IllegalArgumentException("height must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (Elastos::Core::Math::Abs(stride) < width) {
//        throw new IllegalArgumentException("abs(stride) must be >= width");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 lastScanline = offset + (height - 1) * stride;
    Int32 length = colors->GetLength();
    if (offset < 0 || (offset + width > length) || lastScanline < 0
            || (lastScanline + width > length)) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // quick escape if there's nothing to draw
    if (width == 0 || height == 0) {
        return NOERROR;
    }
    Int64 nativePaint = paint ? TO_PAINT(paint)->mNativePaint : 0;
    // punch down to native for the actual draw
    NativeDrawBitmap(mNativeCanvas, colors, offset, stride, x, y, width,
            height, hasAlpha, nativePaint);
    return NOERROR;
}

ECode Canvas::DrawBitmap(
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
    // call through to the common float version
    return DrawBitmap(colors, offset, stride, (Float)x, (Float)y, width, height,
            hasAlpha, paint);
}

ECode Canvas::DrawBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IMatrix* matrix,
    /* [in] */ IPaint* paint)
{
    assert(bitmap != NULL);
    assert(matrix != NULL);

    Handle64 nativeBitmap = ((CBitmap*)bitmap)->mNativeBitmap;
    Int64 nativeMatrix = ((CMatrix*)matrix)->mNativeMatrix;
    Int64 nativePaint = paint ? (TO_PAINT(paint)->mNativePaint) : 0;
    NativeDrawBitmapMatrix(mNativeCanvas, nativeBitmap, nativeMatrix, nativePaint);
    return NOERROR;
}

ECode Canvas::CheckRange(
    /* [in] */ Int32 length,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if ((offset | count) < 0 || offset + count > length) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Canvas::DrawBitmapMesh(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 meshWidth,
    /* [in] */ Int32 meshHeight,
    /* [in] */ ArrayOf<Float>* verts,
    /* [in] */ Int32 vertOffset,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorOffset,
    /* [in] */ IPaint* paint)
{
    assert(bitmap != NULL);

    if ((meshWidth | meshHeight | vertOffset | colorOffset) < 0) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (meshWidth == 0 || meshHeight == 0) {
        return NOERROR;
    }
    Int32 count = (meshWidth + 1) * (meshHeight + 1);
    // we mul by 2 since we need two floats per vertex
    FAIL_RETURN(CheckRange(verts->GetLength(), vertOffset, count * 2));
    if (colors != NULL) {
        // no mul by 2, since we need only 1 color per vertex
        FAIL_RETURN(CheckRange(colors->GetLength(), colorOffset, count));
    }
    Handle64 nativeBitmap = ((CBitmap*)bitmap)->mNativeBitmap;
    Int64 nativePaint = paint ? (TO_PAINT(paint)->mNativePaint) : 0;
    NativeDrawBitmapMesh(mNativeCanvas, nativeBitmap, meshWidth, meshHeight,
            verts, vertOffset, colors, colorOffset, nativePaint);
    return NOERROR;
}

ECode Canvas::DrawVertices(
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
    assert(paint != NULL);

    FAIL_RETURN(CheckRange(verts->GetLength(), vertOffset, vertexCount));
    if (texs != NULL) {
        FAIL_RETURN(CheckRange(texs->GetLength(), texOffset, vertexCount));
    }
    if (colors != NULL) {
        FAIL_RETURN(CheckRange(colors->GetLength(), colorOffset, vertexCount));
    }
    if (indices != NULL) {
        FAIL_RETURN(CheckRange(indices->GetLength(), indexOffset, indexCount));
    }

    NativeDrawVertices(mNativeCanvas, mode,
            vertexCount, verts, vertOffset, texs, texOffset, colors,
            colorOffset, indices, indexOffset, indexCount, TO_PAINT(paint)->mNativePaint);
    return NOERROR;
}

ECode Canvas::DrawText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    if ((index | count | (index + count) |
        (text->GetLength() - index - count)) < 0) {
//        throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    NativeDrawText(mNativeCanvas, text, index, count, x, y,
            TO_PAINT(paint)->mBidiFlags, TO_PAINT(paint)->mNativePaint, TO_PAINT(paint)->GetNativeTypeface());
    return NOERROR;
}

ECode Canvas::DrawText(
    /* [in] */ const String& text,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    if (text.IsNullOrEmpty()) {
        return NOERROR;
    }

    assert(paint != NULL && "Paint cannot be null.");
    NativeDrawText(mNativeCanvas, text, 0, text.GetLength(), x, y,
            TO_PAINT(paint)->mBidiFlags, TO_PAINT(paint)->mNativePaint, TO_PAINT(paint)->GetNativeTypeface());
    return NOERROR;
}

ECode Canvas::DrawText(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    if (text.IsNullOrEmpty()) {
        return NOERROR;
    }

    if ((start | end | (end - start) | (text.GetLength() - end)) < 0) {
//        throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    NativeDrawText(mNativeCanvas, text, start, end, x, y,
            TO_PAINT(paint)->mBidiFlags, TO_PAINT(paint)->mNativePaint,
            TO_PAINT(paint)->GetNativeTypeface());
    return NOERROR;
}

ECode Canvas::DrawText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    if (IString::Probe(text) != NULL || ISpannedString::Probe(text) != NULL ||
        ISpannableString::Probe(text) != NULL) {
        NativeDrawText(mNativeCanvas, CoreUtils::Unbox(text), start, end, x, y,
                TO_PAINT(paint)->mBidiFlags, TO_PAINT(paint)->mNativePaint,
                TO_PAINT(paint)->GetNativeTypeface());
    }
    else if (IGraphicsOperations::Probe(text) != NULL) {
        IGraphicsOperations::Probe(text)->DrawText(this, start, end, x, y, paint);
    }
    else {
        AutoPtr< ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);
        TextUtils::GetChars(text, start, end, buf, 0);
        NativeDrawText(mNativeCanvas, buf, 0, end - start, x, y,
                TO_PAINT(paint)->mBidiFlags, TO_PAINT(paint)->mNativePaint,
                TO_PAINT(paint)->GetNativeTypeface());
        TemporaryBuffer::Recycle(buf);
    }

    return NOERROR;
}

ECode Canvas::DrawTextRun(
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
    if (text == NULL) {
        // throw new NullPointerException("text is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (paint == NULL) {
//        throw new NullPointerException("paint is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((index | count | (text->GetLength() - index - count)) < 0) {
//        throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    NativeDrawTextRun(mNativeCanvas, text, index, count,
            contextIndex, contextCount, x, y, isRtl, TO_PAINT(paint)->mNativePaint,
            TO_PAINT(paint)->GetNativeTypeface());
     return NOERROR;
}

ECode Canvas::DrawTextRun(
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
    if (text == NULL) {
//        throw new NullPointerException("text is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (paint == NULL) {
//        throw new NullPointerException("paint is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 length;
    if ((start | end | (end - start) | (text->GetLength(&length), length - end)) < 0) {
//        throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (IString::Probe(text) != NULL || ISpannedString::Probe(text) != NULL ||
            ISpannableString::Probe(text) != NULL) {
        NativeDrawTextRun(mNativeCanvas, CoreUtils::Unbox(text), start, end,
                contextStart, contextEnd, x, y, isRtl, TO_PAINT(paint)->mNativePaint,
                TO_PAINT(paint)->GetNativeTypeface());
    }
    else if (IGraphicsOperations::Probe(text) != NULL) {
        IGraphicsOperations::Probe(text)->DrawTextRun(this, start, end,
                contextStart, contextEnd, x, y, isRtl, paint);
    }
    else {
        Int32 contextLen = contextEnd - contextStart;
        Int32 len = end - start;
        AutoPtr< ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(contextLen);
        TextUtils::GetChars(text, contextStart, contextEnd, buf, 0);
        NativeDrawTextRun(mNativeCanvas, buf, start - contextStart, len,
                0, contextLen, x, y, isRtl, TO_PAINT(paint)->mNativePaint,
                TO_PAINT(paint)->GetNativeTypeface());
        TemporaryBuffer::Recycle(buf);
    }

    return NOERROR;
}

ECode Canvas::DrawPosText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* pos,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    if (index < 0 || index + count > text->GetLength()
            || count * 2 > pos->GetLength()) {
//        throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    for (Int32 i = 0; i < count; i++) {
        DrawText(text, index + i, 1, (*pos)[i * 2], (*pos)[i * 2 + 1], paint);
    }
    return NOERROR;
}

ECode Canvas::DrawPosText(
    /* [in] */ const String& text,
    /* [in] */ ArrayOf<Float>* pos,
    /* [in] */ IPaint* paint)
{
    assert(paint != NULL);

    return DrawPosText(text.GetChars(), 0, text.GetLength(), pos, paint);
}

ECode Canvas::DrawTextOnPath(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ IPath* path,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ IPaint* paint)
{
    assert(path != NULL);
    assert(paint != NULL);

    if (index < 0 || index + count > text->GetLength()) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Int64 nativePath = ((CPath*)path)->mNativePath;
    Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
    NativeDrawTextOnPath(mNativeCanvas, text, index, count,
            nativePath, hOffset, vOffset, TO_PAINT(paint)->mBidiFlags, nativePaint, TO_PAINT(paint)->GetNativeTypeface());
    return NOERROR;
}

ECode Canvas::DrawTextOnPath(
    /* [in] */ const String& text,
    /* [in] */ IPath* path,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ IPaint* paint)
{
    assert(path != NULL);
    assert(paint != NULL);

    if (!text.IsNullOrEmpty()) {
        Int64 nativePath = ((CPath*)path)->mNativePath;
        Int64 nativePaint = TO_PAINT(paint)->mNativePaint;
        NativeDrawTextOnPath(mNativeCanvas, text, nativePath,
                hOffset, vOffset, TO_PAINT(paint)->mBidiFlags, nativePaint, TO_PAINT(paint)->GetNativeTypeface());
    }
    return NOERROR;
}

ECode Canvas::DrawPicture(
    /* [in] */ IPicture* picture)
{
    assert(picture != NULL);
    picture->EndRecording();
    Int32 restoreCount = 0;
    Save(&restoreCount);
    picture->Draw(this);
    return RestoreToCount(restoreCount);
}

ECode Canvas::DrawPicture(
    /* [in] */ IPicture* picture,
    /* [in] */ IRectF* dst)
{
    Int32 count;
    Save(&count);
    Translate(((CRectF*)dst)->mLeft, ((CRectF*)dst)->mTop);
    Int32 pictureWidth, pictureHeight;
    picture->GetWidth(&pictureWidth);
    picture->GetHeight(&pictureHeight);
    if (pictureWidth > 0 && pictureHeight > 0) {
        Float rectfWidth, rectfHeight;
        dst->GetWidth(&rectfWidth);
        dst->GetHeight(&rectfHeight);
        Scale(rectfWidth / pictureWidth, rectfHeight / pictureHeight);
    }
    DrawPicture(picture);
    Restore();
    return NOERROR;
}

ECode Canvas::DrawPicture(
    /* [in] */ IPicture* picture,
    /* [in] */ IRect* dst)
{
    Int32 count;
    Save(&count);
    Translate(((CRect*)dst)->mLeft, ((CRect*)dst)->mTop);
    Int32 pictureWidth, pictureHeight;
    picture->GetWidth(&pictureWidth);
    picture->GetHeight(&pictureHeight);
    if (pictureWidth > 0 && pictureHeight > 0) {
        Int32 rectWidth, rectHeight;
        dst->GetWidth(&rectWidth);
        dst->GetHeight(&rectHeight);
        Scale((Float)rectWidth / pictureWidth,
                (Float)rectHeight / pictureHeight);
    }
    DrawPicture(picture);
    Restore();
    return NOERROR;
}

ECode Canvas::ReleaseResources() {
    return Dispose();
}

ECode Canvas::Dispose()
{
    if (mNativeCanvas != 0) {
        NativeFinalizer(mNativeCanvas);
        mNativeCanvas = 0;
    }
    return NOERROR;
}

void Canvas::FreeCaches()
{
    SkGraphics::PurgeFontCache();
}

void Canvas::FreeTextLayoutCaches()
{
    Layout::purgeCaches();
}

Boolean Canvas::IsRecordingFor(
    /* [in] */ IInterface* o)
{
    return FALSE;
}

Int64 Canvas::InitRaster(
    /* [in] */ IBitmap* bitmapObj)
{
    SkBitmap bitmap;
    if (bitmapObj != 0) {
        GraphicsNative::GetNativeBitmap(bitmapObj, &bitmap);
    }
    return reinterpret_cast<Int64>(android::Canvas::create_canvas(bitmap));
}

static android::Canvas* get_canvas(
    /* [in] */ Int64 canvasHandle)
{
    return reinterpret_cast<android::Canvas*>(canvasHandle);
}

ECode Canvas::SetNativeBitmap(
    /* [in] */ Int64 bitmapHandle)
{
    SkBitmap bitmap;
    if (bitmapHandle != 0) {
        reinterpret_cast<BitmapWrapper*>(bitmapHandle)->getSkBitmap(&bitmap);
    }
    get_canvas(mNativeCanvas)->setBitmap(bitmap);
    return NOERROR;
}

void Canvas::NativeSetBitmap(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ IBitmap* bitmapObj,
    /* [in] */ Boolean copyState)
{
    SkBitmap bitmap;
    if (bitmapObj != NULL) {
        GraphicsNative::GetNativeBitmap(bitmapObj, &bitmap);
    }
    get_canvas(canvasHandle)->setBitmap(bitmap);
}

Boolean Canvas::NativeIsOpaque(
    /* [in] */ Int64 canvasHandle)
{
    return get_canvas(canvasHandle)->isOpaque() ? TRUE : FALSE;
}

Int32 Canvas::NativeGetWidth(
    /* [in] */ Int64 canvasHandle)
{
    return static_cast<Int32>(get_canvas(canvasHandle)->width());
}

Int32 Canvas::NativeGetHeight(
    /* [in] */ Int64 canvasHandle)
{
    return static_cast<Int32>(get_canvas(canvasHandle)->height());
}

Int32 Canvas::NativeSave(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int32 saveFlags)
{
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(saveFlags);
    return static_cast<Int32>(get_canvas(canvasHandle)->save(flags));
}

Int32 Canvas::NativeSaveLayer(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float l,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float b,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int32 flagsHandle)
{
    android::Paint* paint  = reinterpret_cast<android::Paint*>(paintHandle);
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(flagsHandle);
    return static_cast<Int32>(get_canvas(canvasHandle)->saveLayer(l, t, r, b, paint, flags));
}

Int32 Canvas::NativeSaveLayerAlpha(
    /* [in] */Int64 canvasHandle,
    /* [in] */ Float l,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float b,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 flagsHandle)
{
    android::SaveFlags::Flags flags = static_cast<android::SaveFlags::Flags>(flagsHandle);
    return static_cast<Int32>(get_canvas(canvasHandle)->saveLayerAlpha(l, t, r, b, alpha, flags));
}

void Canvas::NativeRestore(
    /* [in] */ Int64 canvasHandle)
{
    android::Canvas* canvas = get_canvas(canvasHandle);
    if (canvas->getSaveCount() <= 1) {  // cannot restore anymore
        // doThrowISE(env, "Underflow in restore");
        return;
    }
    canvas->restore();
}

void Canvas::NativeRestoreToCount(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int32 restoreCount)
{
    android::Canvas* canvas = get_canvas(canvasHandle);
    if (restoreCount < 1 || restoreCount > canvas->getSaveCount()) {
        // doThrowIAE(env, "Underflow in restoreToCount");
        return;
    }
    canvas->restoreToCount(restoreCount);
}

Int32 Canvas::NativeGetSaveCount(
    /* [in] */ Int64 canvasHandle)
{
    return static_cast<Int32>(get_canvas(canvasHandle)->getSaveCount());
}

void Canvas::NativeTranslate(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    get_canvas(canvasHandle)->translate(dx, dy);
}

void Canvas::NativeScale(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    get_canvas(canvasHandle)->scale(sx, sy);
}

void Canvas::NativeRotate(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float degrees)
{
    get_canvas(canvasHandle)->rotate(degrees);
}

void Canvas::NativeSkew(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    get_canvas(canvasHandle)->skew(sx, sy);
}

void Canvas::NativeConcat(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 matrixHandle)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    get_canvas(canvasHandle)->concat(*matrix);
}

void Canvas::NativeSetMatrix(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 matrixHandle)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    get_canvas(canvasHandle)->setMatrix(matrix ? *matrix : SkMatrix::I());
}

Boolean Canvas::NativeClipRect(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float l,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float b,
    /* [in] */ RegionOp opHandle)
{
    bool nonEmptyClip = get_canvas(canvasHandle)->clipRect(l, t, r, b,
            GraphicsNative::OpHandleToClipOp(opHandle));
    return nonEmptyClip ? TRUE : FALSE;
}

Boolean Canvas::NativeClipPath(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 pathHandle,
    /* [in] */ RegionOp opHandle)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    bool nonEmptyClip = get_canvas(canvasHandle)->clipPath(path,
            GraphicsNative::OpHandleToClipOp(opHandle));
    return nonEmptyClip ? TRUE : FALSE;
}

void Canvas::NativeSetDrawFilter(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 filterHandle)
{
    get_canvas(canvasHandle)->setDrawFilter(reinterpret_cast<SkDrawFilter*>(filterHandle));
}

Boolean Canvas::NativeGetClipBounds(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ IRect* bounds)
{
    SkRect   r;
    SkIRect ir;
    bool result = get_canvas(canvasHandle)->getClipBounds(&r);

    if (!result) {
        r.setEmpty();
    }
    r.round(&ir);

    (void)GraphicsNative::SkIRect2IRect(ir, bounds);
    return result ? TRUE : FALSE;
}

void Canvas::NativeGetCTM(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 matrixHandle)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    get_canvas(canvasHandle)->getMatrix(matrix);
}

Boolean Canvas::NativeQuickReject(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 pathHandle)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    bool result = get_canvas(canvasHandle)->quickRejectPath(*path);
    return result ? TRUE : FALSE;
}

Boolean Canvas::NativeQuickReject(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    bool result = get_canvas(canvasHandle)->quickRejectRect(left, top, right, bottom);
    return result ? TRUE : FALSE;
}

void Canvas::NativeDrawColor(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int32 color,
    /* [in] */ Int32 modeHandle)
{
    SkBlendMode mode = static_cast<SkBlendMode>(modeHandle);
    get_canvas(canvasHandle)->drawColor(color, mode);
}

void Canvas::NativeDrawPaint(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 paintHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawPaint(*paint);
}

void Canvas::NativeDrawPoint(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawPoint(x, y, *paint);
}

void Canvas::NativeDrawPoints(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintHandle)
{
    // NPE_CHECK_RETURN_VOID(env, pts);
    if (pts == NULL) return;
    // AutoJavaFloatArray autoPts(env, pts);
    float* floats = pts->GetPayload();
    const int length = pts->GetLength();

    if ((offset | count) < 0 || offset + count > length) {
        // doThrowAIOOBE(env);
        return;
    }

    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawPoints(floats + offset, count, *paint);
}

void Canvas::NativeDrawLine(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float stopX,
    /* [in] */ Float stopY,
    /* [in] */ Int64 paintHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawLine(startX, startY, stopX, stopY, *paint);
}

void Canvas::NativeDrawLines(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Float>* pts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintHandle)
{
    // NPE_CHECK_RETURN_VOID(env, pts);
    if (pts == NULL) return;
    // AutoJavaFloatArray autoPts(env, pts);
    float* floats = pts->GetPayload();
    const int length = pts->GetLength();

    if ((offset | count) < 0 || offset + count > length) {
        // doThrowAIOOBE(env);
        return;
    }

    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawLines(floats + offset, count, *paint);
}

void Canvas::NativeDrawRect(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawRect(left, top, right, bottom, *paint);
}

void Canvas::NativeDrawOval(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawOval(left, top, right, bottom, *paint);
}

void Canvas::NativeDrawCircle(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float radius,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawCircle(cx, cy, radius, *paint);
}

void Canvas::NativeDrawArc(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean useCenter,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawArc(left, top, right, bottom, startAngle, sweepAngle,
                                       useCenter, *paint);
}

void Canvas::NativeDrawRoundRect(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ Int64 paintHandle)
{
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawRoundRect(left, top, right, bottom, rx, ry, *paint);
}

void Canvas::NativeDrawPath(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 pathHandle,
    /* [in] */ Int64 paintHandle)
{
    const SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawPath(*path, *paint);
}

void Canvas::NativeDrawBitmap(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int32 canvasDensity,
    /* [in] */ Int32 screenDensity,
    /* [in] */ Int32 bitmapDensity)
{
    android::Canvas* canvas = get_canvas(canvasHandle);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapHandle)->bitmap();
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);

    if (canvasDensity == bitmapDensity || canvasDensity == 0 || bitmapDensity == 0) {
        if (screenDensity != 0 && screenDensity != bitmapDensity) {
            android::Paint filteredPaint;
            if (paint) {
                filteredPaint = *paint;
            }
            filteredPaint.setFilterQuality(kLow_SkFilterQuality);
            canvas->drawBitmap(bitmap, left, top, &filteredPaint);
        }
        else {
            canvas->drawBitmap(bitmap, left, top, paint);
        }
    }
    else {
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

void Canvas::NativeDrawBitmap(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Float srcLeft,
    /* [in] */ Float srcTop,
    /* [in] */ Float srcRight,
    /* [in] */ Float srcBottom,
    /* [in] */ Float dstLeft,
    /* [in] */ Float dstTop,
    /* [in] */ Float dstRight,
    /* [in] */ Float dstBottom,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int32 screenDensity,
    /* [in] */ Int32 bitmapDensity)
{
    android::Canvas* canvas = get_canvas(canvasHandle);
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);

    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapHandle)->bitmap();
    if (screenDensity != 0 && screenDensity != bitmapDensity) {
        android::Paint filteredPaint;
        if (paint) {
            filteredPaint = *paint;
        }
        filteredPaint.setFilterQuality(kLow_SkFilterQuality);
        canvas->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
                           dstLeft, dstTop, dstRight, dstBottom, &filteredPaint);
    } else {
        canvas->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
                           dstLeft, dstTop, dstRight, dstBottom, paint);
    }
}

void Canvas::NativeDrawBitmap(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean hasAlpha,
    /* [in] */ Int64 paintHandle)
{
    // Note: If hasAlpha is false, kRGB_565_SkColorType will be used, which will
    // correct the alphaType to kOpaque_SkAlphaType.
    SkImageInfo info = SkImageInfo::MakeN32(width, height, kPremul_SkAlphaType,
           GraphicsNative::DefaultColorSpace());
    SkBitmap bitmap;
    bitmap.setInfo(info);
    sk_sp<android::Bitmap> androidBitmap = android::Bitmap::allocateHeapBitmap(&bitmap, NULL);
    if (!androidBitmap) {
        return;
    }

    if (!GraphicsNative::SetPixels(colors, offset, stride, 0, 0, width, height, bitmap)) {
        return;
    }

    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawBitmap(*androidBitmap, x, y, paint);
}

void Canvas::NativeDrawBitmapMatrix(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Int64 paintHandle)
{
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapHandle)->bitmap();
    get_canvas(canvasHandle)->drawBitmap(bitmap, *matrix, paint);
}

void Canvas::NativeDrawBitmapMesh(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Int32 meshWidth,
    /* [in] */ Int32 meshHeight,
    /* [in] */ ArrayOf<Float>* _verts,
    /* [in] */ Int32 vertIndex,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorIndex,
    /* [in] */ Int64 paintHandle)
{
    const int ptCount = (meshWidth + 1) * (meshHeight + 1);
    AutoFloatArray vertA(_verts, vertIndex + (ptCount << 1));
    AutoInt32Array colorA(colors, colorIndex + ptCount);

    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Bitmap& bitmap = reinterpret_cast<BitmapWrapper*>(bitmapHandle)->bitmap();
    get_canvas(canvasHandle)->drawBitmapMesh(bitmap, meshWidth, meshHeight,
                                             vertA.ptr(), colorA.ptr(), paint);
}

void Canvas::NativeDrawVertices(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ CanvasVertexMode modeHandle,
    /* [in] */ Int32 vertexCount,
    /* [in] */ ArrayOf<Float>* _verts,
    /* [in] */ Int32 vertIndex,
    /* [in] */ ArrayOf<Float>* _texs,
    /* [in] */ Int32 texIndex,
    /* [in] */ ArrayOf<Int32>* _colors,
    /* [in] */ Int32 colorIndex,
    /* [in] */ ArrayOf<Int16>* _indices,
    /* [in] */ Int32 indexIndex,
    /* [in] */ Int32 indexCount,
    /* [in] */ Int64 paintHandle)
{
    AutoFloatArray  vertA(_verts, vertIndex + vertexCount);
    AutoFloatArray  texA(_texs, texIndex + vertexCount);
    AutoInt32Array    colorA(_colors, colorIndex + vertexCount);
    AutoShortArray  indexA(_indices, indexIndex + indexCount);

    const float* verts = vertA.ptr() + vertIndex;
    const float* texs = texA.ptr() + vertIndex;
    const int* colors = NULL;
    const uint16_t* indices = NULL;

    if (_colors != NULL) {
        colors = colorA.ptr() + colorIndex;
    }
    if (_indices != NULL) {
        indices = (const uint16_t*)(indexA.ptr() + indexIndex);
    }

    SkCanvas::VertexMode mode = static_cast<SkCanvas::VertexMode>(modeHandle);
    const android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    get_canvas(canvasHandle)->drawVertices(mode, vertexCount, verts, texs, colors,
                                           indices, indexCount, *paint);
}

void Canvas::NativeDrawText(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);

    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text);
    get_canvas(canvasHandle)->drawText(chars->GetPayload() + index, 0, count, count, x, y,
                                       bidiFlags, *paint, typeface);
}

void Canvas::NativeDrawText(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);
    const int count = end - start;
    const AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    get_canvas(canvasHandle)->drawText(chars->GetPayload() + start, 0, count, count, x, y,
                                       bidiFlags, *paint, typeface);
}

void Canvas::NativeDrawTextRun(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);

    int bidiFlags = isRtl ? minikin::kBidi_Force_RTL : minikin::kBidi_Force_LTR;
    Int32 count = end - start;
    Int32 contextCount = contextEnd - contextStart;
    AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    get_canvas(canvasHandle)->drawText(chars->GetPayload() + contextStart, start - contextStart, count,
                                       contextCount, x, y, bidiFlags, *paint, typeface);
}

void Canvas::NativeDrawTextRun(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);

    const int bidiFlags = isRtl ? minikin::kBidi_Force_RTL : minikin::kBidi_Force_LTR;
    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text);
    get_canvas(canvasHandle)->drawText(chars->GetPayload() + contextIndex, index - contextIndex, count,
                                       contextCount, x, y, bidiFlags, *paint, typeface);
}

void Canvas::NativeDrawTextOnPath(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int64 pathHandle,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);

    AutoPtr<ArrayOf<Char16> > chars = ArrayUtils::ToChar16Array(text, index, count);
    get_canvas(canvasHandle)->drawTextOnPath(chars->GetPayload(), count, bidiFlags, *path,
                   hOffset, vOffset, *paint, typeface);
}

void Canvas::NativeDrawTextOnPath(
    /* [in] */ Int64 canvasHandle,
    /* [in] */ const String& text,
    /* [in] */ Int64 pathHandle,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Int64 typefaceHandle)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    android::Paint* paint = reinterpret_cast<android::Paint*>(paintHandle);
    android::Typeface* typeface = reinterpret_cast<android::Typeface*>(typefaceHandle);

    const AutoPtr<ArrayOf<Char16> > chars = text.GetChar16s();
    int count = text.GetLength();

    get_canvas(canvasHandle)->drawTextOnPath(chars->GetPayload(), count, bidiFlags, *path,
                   hOffset, vOffset, *paint, typeface);
}

void Canvas::NativeFinalizer(
    /* [in] */ Int64 canvasHandle)
{
    delete get_canvas(canvasHandle);
}

ECode Canvas::SetSurfaceFormat(
    /* [in] */ Int32 format)
{
    mSurfaceFormat = format;
    return NOERROR;
}

ECode Canvas::GetNativeCanvas(
    /* [out] */ Int64* nativeCanvas)
{
    VALIDATE_NOT_NULL(nativeCanvas)
    *nativeCanvas = mNativeCanvas;
    return NOERROR;
}

ECode Canvas::IsRecordingFor(
    /* [in] */ IInterface* it,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

