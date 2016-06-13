#include "elastos/droid/view/GLES20Canvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/GraphicsNative.h"

#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/MinikinUtils.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/graphics/TypefaceImpl.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/text/TextUtils.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

#include <androidfw/ResourceTypes.h>
#include <private/hwui/DrawGlInfo.h>
#include <cutils/properties.h>
#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkImageInfo.h>
#include <SkMatrix.h>
#include <SkPorterDuff.h>
#include <SkRegion.h>
#include <SkScalerContext.h>
#include <SkTemplates.h>
#include <SkXfermode.h>
#include <DisplayListRenderer.h>
#include <Rect.h>
#include <RenderNode.h>
#include <CanvasProperty.h>


using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::Matrix;
using Elastos::Droid::Graphics::MinikinUtils;
using Elastos::Droid::Graphics::NativePaint;
using Elastos::Droid::Graphics::NinePatch;
using Elastos::Droid::Graphics::TemporaryBuffer;
using Elastos::Droid::Graphics::TypefaceImpl;
using Elastos::Droid::Graphics::Paint;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_OVER;
using Elastos::Droid::Graphics::RegionOp_INTERSECT;
using Elastos::Droid::Graphics::kBidi_Force_LTR;
using Elastos::Droid::Graphics::kBidi_Force_RTL;

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::IGraphicsOperations;

using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;

using namespace android;
using namespace android::uirenderer;

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
{
    GLES20Canvas::nDestroyRenderer(mRenderer);
}

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

ECode GLES20Canvas::constructor()
{
    FAIL_RETURN(HardwareCanvas::constructor())

    mOpaque = FALSE;
    mRenderer = nCreateDisplayListRenderer();
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

    nSetViewport(mRenderer, width, height);
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
    VALIDATE_NOT_NULL(res)
    if (dirty != NULL) {
        Int32 left ,top ,right ,bottom;
        dirty->Get(&left, &top, &right, &bottom);
        *res = nPrepareDirty(mRenderer, left, top, right, bottom, mOpaque);
    } else {
        *res = nPrepare(mRenderer, mOpaque);
    }
    return NOERROR;
}

ECode GLES20Canvas::OnPostDraw()
{
    nFinish(mRenderer);
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
    /* [in] */ IRect* rect)
{
    Int32 left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRect* rect,
    /* [in] */ RegionOp op)
{
    Int32 left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    nClipRect(mRenderer, left, top, right, bottom, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRectF* rect)
{
    Float left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    nClipRect(mRenderer, left, top, right, bottom, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRect(
    /* [in] */ IRectF* rect,
    /* [in] */ RegionOp op)
{
    Float left ,top ,right ,bottom;
    rect->Get(&left, &top, &right, &bottom);
    nClipRect(mRenderer, left, top, right, bottom, op);
    return NOERROR;
}

ECode GLES20Canvas::ClipRegion(
    /* [in] */ IRegion* region)
{
    Handle64 nRegion;
    region->GetNativeRegion(&nRegion);
    nClipRegion(mRenderer, nRegion, RegionOp_INTERSECT);
    return NOERROR;
}

ECode GLES20Canvas::ClipRegion(
    /* [in] */ IRegion* region,
    /* [in] */ RegionOp op)
{
    Handle64 nRegion;
    region->GetNativeRegion(&nRegion);
    nClipRegion(mRenderer, nRegion, op);
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
    if (bounds != NULL) {
        Float left, top, right, bottom;
        bounds->Get(&left, &top, &right, &bottom);
        return SaveLayer(left, top, right, bottom, paint, saveFlags, res);
    }

    Paint* p = (Paint*)paint;
    Int64 nativePaint = p == NULL ? 0 : p->mNativePaint;
    *res = nSaveLayer(mRenderer, nativePaint, saveFlags);
    return NOERROR;
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
    if (bounds != NULL) {
        Float left, top, right, bottom;
        bounds->Get(&left, &top, &right, &bottom);
        return SaveLayerAlpha(left, top, right, bottom,
                alpha, saveFlags, res);
    }
    *res = nSaveLayerAlpha(mRenderer, alpha, saveFlags);
    return NOERROR;
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
    mFilter = filter;
    if (filter == NULL) {
        nResetPaintFilter(mRenderer);
    } else if (IPaintFlagsDrawFilter::Probe(filter) != NULL) {
        IPaintFlagsDrawFilter* flagsFilter = IPaintFlagsDrawFilter::Probe(filter);
        CPaintFlagsDrawFilter* impl = (CPaintFlagsDrawFilter*)flagsFilter;
        nSetupPaintFilter(mRenderer, impl->mClearBits, impl->mSetBits);
    }
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
    nDrawPatch(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, nPatch->mNativeChunk,
            left, top, right, bottom, nativePaint);
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
    nDrawPatch(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, nPatch->mNativeChunk,
            left, top, right, bottom, nativePaint);
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
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, left, top, nativePaint);
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
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer,
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
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, left, top, right, bottom,
            dstLeft, dstTop, dstRight, dstBottom, nativePaint);
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
    nDrawBitmap(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, left, top, right, bottom,
            dstLeft, dstTop, dstRight, dstBottom, nativePaint);
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
    nDrawBitmapMesh(mRenderer, nBitmap->mNativeBitmap, nBitmap->mBuffer, meshWidth, meshHeight,
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
            p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
                p->mNativePaint, p->mNativeTypeface);
    } else if (IGraphicsOperations::Probe(text) != NULL) {
        AutoPtr<IGraphicsOperations> gop = IGraphicsOperations::Probe(text);
        return gop->DrawText(this, start, end, x, y, paint);
    } else {
        AutoPtr<ArrayOf<Char32> > buf = TemporaryBuffer::Obtain(end - start);
        TextUtils::GetChars(text, start, end, buf, 0);
        Paint* p = (Paint*)paint;
        nDrawText(mRenderer, buf, 0, end - start, x, y,
                p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
            p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
        p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
            p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
    nDrawTextOnPath(mRenderer, text, 0, text.GetLength(), pathImpl->mNativePath, hOffset, vOffset,
            p->mBidiFlags, p->mNativePaint, p->mNativeTypeface);
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
            p->mNativePaint, p->mNativeTypeface);
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
                contextEnd, x, y, isRtl, p->mNativePaint, p->mNativeTypeface);
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
                x, y, isRtl, p->mNativePaint, p->mNativeTypeface);
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
    nSetProperty(name, value);
    return NOERROR;
}


//.......................Native......................


// ----------------------------------------------------------------------------
// Text
// ----------------------------------------------------------------------------

class RenderTextFunctor {
public:
    RenderTextFunctor(const Layout& layout, DisplayListRenderer* renderer, float x, float y,
                NativePaint* paint, uint16_t* glyphs, float* pos, float totalAdvance,
                android::uirenderer::Rect& bounds)
            : layout(layout), renderer(renderer), x(x), y(y), paint(paint), glyphs(glyphs),
            pos(pos), totalAdvance(totalAdvance), bounds(bounds) { }
    void operator()(size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            glyphs[i] = layout.getGlyphId(i);
            pos[2 * i] = layout.getX(i);
            pos[2 * i + 1] = layout.getY(i);
        }
        size_t glyphsCount = end - start;
        int bytesCount = glyphsCount * sizeof(char);
        renderer->drawText((const char*) (glyphs + start), bytesCount, glyphsCount,
            x, y, pos + 2 * start, paint, totalAdvance, bounds);
    }
private:
    const Layout& layout;
    DisplayListRenderer* renderer;
    float x;
    float y;
    NativePaint* paint;
    uint16_t* glyphs;
    float* pos;
    float totalAdvance;
    android::uirenderer::Rect& bounds;
};

static void renderTextLayout(DisplayListRenderer* renderer, Layout* layout,
    float x, float y, NativePaint* paint) {
    size_t nGlyphs = layout->nGlyphs();
    float* pos = new float[nGlyphs * 2];
    uint16_t* glyphs = new uint16_t[nGlyphs];
    MinikinRect b;
    layout->getBounds(&b);
    android::uirenderer::Rect bounds(b.mLeft, b.mTop, b.mRight, b.mBottom);
    bounds.translate(x, y);
    float totalAdvance = layout->getAdvance();

    RenderTextFunctor f(*layout, renderer, x, y, paint, glyphs, pos, totalAdvance, bounds);
    MinikinUtils::forFontRun(*layout, paint, f);
    delete[] glyphs;
    delete[] pos;
}

static void renderText(DisplayListRenderer* renderer, const Char16* text, int count,
        float x, float y, int bidiFlags, NativePaint* paint, TypefaceImpl* typeface) {
    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, 0, count, count);
    x += MinikinUtils::xOffsetForTextAlign(paint, layout);
    renderTextLayout(renderer, &layout, x, y, paint);
}

class RenderTextOnPathFunctor {
public:
    RenderTextOnPathFunctor(const Layout& layout, DisplayListRenderer* renderer, float hOffset,
                float vOffset, NativePaint* paint, SkPath* path)
            : layout(layout), renderer(renderer), hOffset(hOffset), vOffset(vOffset),
                paint(paint), path(path) {
    }
    void operator()(size_t start, size_t end) {
        uint16_t glyphs[1];
        for (size_t i = start; i < end; i++) {
            glyphs[0] = layout.getGlyphId(i);
            float x = hOffset + layout.getX(i);
            float y = vOffset + layout.getY(i);
            renderer->drawTextOnPath((const char*) glyphs, sizeof(glyphs), 1, path, x, y, paint);
        }
    }
private:
    const Layout& layout;
    DisplayListRenderer* renderer;
    float hOffset;
    float vOffset;
    NativePaint* paint;
    SkPath* path;
};

static void renderTextOnPath(DisplayListRenderer* renderer, const Char16* text, int count,
        SkPath* path, float hOffset, float vOffset, int bidiFlags, NativePaint* paint,
        TypefaceImpl* typeface) {
    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, 0, count, count);
    hOffset += MinikinUtils::hOffsetForTextAlign(paint, layout, *path);
    NativePaint::Align align = paint->getTextAlign();
    paint->setTextAlign(NativePaint::kLeft_Align);

    RenderTextOnPathFunctor f(layout, renderer, hOffset, vOffset, paint, path);
    MinikinUtils::forFontRun(layout, paint, f);
    paint->setTextAlign(align);
}

static void renderTextRun(DisplayListRenderer* renderer, const Char16* text,
        int start, int count, int contextCount, float x, float y,
        int bidiFlags, NativePaint* paint, TypefaceImpl* typeface) {
    Layout layout;
    MinikinUtils::doLayout(&layout, paint, bidiFlags, typeface, text, start, count, contextCount);
    x += MinikinUtils::xOffsetForTextAlign(paint, layout);
    renderTextLayout(renderer, &layout, x, y, paint);
}

void GLES20Canvas::nSetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (!Caches::hasInstance()) {
        Logger::W("GLES20Canvas", "can't set property %s, value:%s, no Caches instance.",
            name.string(), value.string());
        return;
    }

    if (name == NULL || value == NULL) {
        Logger::W("GLES20Canvas", "can't set prop, null passed");
    }

    Caches::getInstance().setTempProperty(name.string(), value.string());
}

ECode GLES20Canvas::nFinishRecording(
    /* [in] */ Int64 rendererPtr,
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res)
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    *res = reinterpret_cast<Int64>(renderer->finishRecording());
    return NOERROR;
}

AutoPtr<IRect> GLES20Canvas::GetInternalClipBounds()
{
    if (mClipBounds == NULL)
        CRect::New((IRect**)&mClipBounds);
    return mClipBounds;
}

AutoPtr<IRectF> GLES20Canvas::GetPathBounds()
{
    if (mPathBounds == NULL)
        CRectF::New((IRectF**)&mPathBounds);
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

Int64 GLES20Canvas::nCreateDisplayListRenderer()
{
    return reinterpret_cast<Int64>(new DisplayListRenderer);
}

void GLES20Canvas::nResetDisplayListRenderer(
    /* [in] */ Int64 rendererPtr)
{
    // TODO implements
}

void GLES20Canvas::nDestroyRenderer(
    /* [in] */ Int64 rendererPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    if (renderer != NULL) {
        delete renderer;
    }
}

Int32 GLES20Canvas::nGetMaximumTextureWidth()
{
    return Caches::getInstance().maxTextureSize;
}

Int32 GLES20Canvas::nGetMaximumTextureHeight()
{
    return Caches::getInstance().maxTextureSize;
}

void GLES20Canvas::nSetViewport(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->setViewport(width, height);
}

void GLES20Canvas::nSetHighContrastText(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Boolean highContrastText)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->setHighContrastText(highContrastText);
}

void GLES20Canvas::nInsertReorderBarrier(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Boolean reorderEnable)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->insertReorderBarrier(reorderEnable);
}

Int32 GLES20Canvas::nPrepare(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Boolean opaque)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    return renderer->prepare(opaque);
}

Int32 GLES20Canvas::nPrepareDirty(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Boolean opaque)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    return renderer->prepareDirty(left, top, right, bottom, opaque);
}

void GLES20Canvas::nFinish(
    /* [in] */ Int64 rendererPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->finish();
}

Int32 GLES20Canvas::nCallDrawGLFunction(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 drawGLFunction)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    Functor* functor = reinterpret_cast<Functor*>(drawGLFunction);
    android::uirenderer::Rect dirty;
    return renderer->callDrawGLFunction(functor, dirty);
}

Int32 GLES20Canvas::nDrawRenderNode(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ IRect* dirty,
    /* [in] */ Int32 flags)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    RenderNode* renderNode = reinterpret_cast<RenderNode*>(renderNodePtr);
    android::uirenderer::Rect bounds;
    status_t status = renderer->drawRenderNode(renderNode, bounds, flags);
    if (status != DrawGlInfo::kStatusDone && dirty != NULL) {
        dirty->Set(int(bounds.left), int(bounds.top), int(bounds.right), int(bounds.bottom));
    }
    return status;
}

void GLES20Canvas::nDrawLayer(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 layerPtr,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    Layer* layer = reinterpret_cast<Layer*>(layerPtr);
    renderer->drawLayer(layer, x, y);
}

Boolean GLES20Canvas::nClipPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Int32 op)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    const bool result = renderer->clipPath(path, static_cast<SkRegion::Op>(op));
    return result ? TRUE : FALSE;
}

Boolean GLES20Canvas::nClipRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 op)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    const bool result = renderer->clipRect(left, top, right, bottom,
                                           static_cast<SkRegion::Op>(op));
    return result ? TRUE : FALSE;
}

Boolean GLES20Canvas::nClipRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 op)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    const bool result = renderer->clipRect(float(left), float(top), float(right),
                                           float(bottom),
                                           static_cast<SkRegion::Op>(op));
    return result ? TRUE : FALSE;
}

Boolean GLES20Canvas::nClipRegion(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 regionPtr,
    /* [in] */ Int32 op)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkRegion* region = reinterpret_cast<SkRegion*>(regionPtr);
    const bool result = renderer->clipRegion(region, static_cast<SkRegion::Op>(op));
    return result ? TRUE : FALSE;
}

Boolean GLES20Canvas::nGetClipBounds(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ IRect* rect)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    const android::uirenderer::Rect& bounds(renderer->getLocalClipBounds());

    rect->Set(int(bounds.left), int(bounds.top), int(bounds.right), int(bounds.bottom));
    return !bounds.isEmpty() ? TRUE : FALSE;
}

Boolean GLES20Canvas::nQuickReject(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    const bool result = renderer->quickRejectConservative(left, top, right, bottom);
    return result ? TRUE : FALSE;
}

void GLES20Canvas::nTranslate(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->translate(dx, dy);
}

void GLES20Canvas::nSkew(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->skew(sx, sy);
}

void GLES20Canvas::nRotate(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float degrees)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->rotate(degrees);
}


void GLES20Canvas::nScale(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->scale(sx, sy);
}

void GLES20Canvas::nSetMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    renderer->setMatrix(matrix ? *matrix : SkMatrix::I());
}

void GLES20Canvas::nGetMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    renderer->getMatrix(matrix);
}

void GLES20Canvas::nConcatMatrix(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 matrixPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    renderer->concatMatrix(*matrix);
}

Int32 GLES20Canvas::nSave(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 flags)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    return renderer->save(flags);
}

Int32 GLES20Canvas::nSaveLayer(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int32 saveFlags)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    const android::uirenderer::Rect& bounds(renderer->getLocalClipBounds());
    return renderer->saveLayer(bounds.left, bounds.top, bounds.right, bounds.bottom,
            paint, saveFlags);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    return renderer->saveLayer(left, top, right, bottom, paint, saveFlags);
}

Int32 GLES20Canvas::nSaveLayerAlpha(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 saveFlags)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    const android::uirenderer::Rect& bounds(renderer->getLocalClipBounds());
    return renderer->saveLayerAlpha(bounds.left, bounds.top, bounds.right, bounds.bottom,
            alpha, saveFlags);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    return renderer->saveLayerAlpha(left, top, right, bottom, alpha, saveFlags);
}

void GLES20Canvas::nRestore(
    /* [in] */ Int64 rendererPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->restore();
}

void GLES20Canvas::nRestoreToCount(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 saveCount)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->restoreToCount(saveCount);
}

Int32 GLES20Canvas::nGetSaveCount(
    /* [in] */ Int64 rendererPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    return renderer->getSaveCount();
}

void GLES20Canvas::nResetPaintFilter(
    /* [in] */ Int64 rendererPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->resetPaintFilter();
}

void GLES20Canvas::nSetupPaintFilter(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 clearBits,
    /* [in] */ Int32 setBits)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    renderer->setupPaintFilter(clearBits, setBits);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawArc(left, top, right, bottom, startAngle, sweepAngle, useCenter, paint);
}

void GLES20Canvas::nDrawPatch(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int64 patchPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    GraphicsNative::ElastosHeapBitmapRef bitmapRef(bitmap, buffer);

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    Res_png_9patch* patch = reinterpret_cast<Res_png_9patch*>(patchPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawPatch(bitmap, patch, left, top, right, bottom, paint);
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Int64 paintPtr)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    GraphicsNative::ElastosHeapBitmapRef bitmapRef(bitmap, buffer);

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);

    // apply transform directly to canvas, so it affects shaders correctly
    renderer->save(SkCanvas::kMatrix_SaveFlag);
    renderer->translate(left, top);
    renderer->drawBitmap(bitmap, paint);
    renderer->restore();
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int64 matrixPtr,
    /* [in] */ Int64 paintPtr)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    GraphicsNative::ElastosHeapBitmapRef bitmapRef(bitmap, buffer);

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);

    // apply transform directly to canvas, so it affects shaders correctly
    renderer->save(SkCanvas::kMatrix_SaveFlag);
    renderer->concatMatrix(*matrix);
    renderer->drawBitmap(bitmap, paint);
    renderer->restore();
}

void GLES20Canvas::nDrawBitmap(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Float srcLeft,
    /* [in] */ Float srcTop,
    /* [in] */ Float srcRight,
    /* [in] */ Float srcBottom,
    /* [in] */ Float dstLeft,
    /* [in] */ Float dstTop,
    /* [in] */ Float dstRight,
    /* [in] */ Float dstBottom,
    /* [in] */ Int64 paintPtr)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    GraphicsNative::ElastosHeapBitmapRef bitmapRef(bitmap, buffer);

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
            dstLeft, dstTop, dstRight, dstBottom, paint);
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
    SkBitmap* bitmap = new SkBitmap;
    if (!bitmap->allocPixels(info)) {
        delete bitmap;
        return;
    }

    if (!GraphicsNative::SetPixels(colors, offset, stride, 0, 0, width, height, *bitmap)) {
        delete bitmap;
        return;
    }

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);

    // apply transform directly to canvas, so it affects shaders correctly
    renderer->save(SkCanvas::kMatrix_SaveFlag);
    renderer->translate(left, top);
    renderer->drawBitmapData(bitmap, paint);
    renderer->restore();

    // Note - bitmap isn't deleted as DisplayListRenderer owns it now
}

void GLES20Canvas::nDrawBitmapMesh(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 bitmapPtr,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 meshWidth,
    /* [in] */ Int32 meshHeight,
    /* [in] */ ArrayOf<Float>* vertices,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 colorOffset,
    /* [in] */ Int64 paintPtr)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    GraphicsNative::ElastosHeapBitmapRef bitmapRef(bitmap, buffer);

    Float* verticesArray = vertices ? vertices->GetPayload() + offset : NULL;
    Int32* colorsArray = colors ? colors->GetPayload() + colorOffset : NULL;

    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawBitmapMesh(bitmap, meshWidth, meshHeight, verticesArray, colorsArray, paint);
}

void GLES20Canvas::nDrawCircle(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawCircle(x, y, radius, paint);
}

void GLES20Canvas::nDrawCircle(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 xPropPtr,
    /* [in] */ Int64 yPropPtr,
    /* [in] */ Int64 radiusPropPtr,
    /* [in] */ Int64 paintPropPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    CanvasPropertyPrimitive* xProp = reinterpret_cast<CanvasPropertyPrimitive*>(xPropPtr);
    CanvasPropertyPrimitive* yProp = reinterpret_cast<CanvasPropertyPrimitive*>(yPropPtr);
    CanvasPropertyPrimitive* radiusProp = reinterpret_cast<CanvasPropertyPrimitive*>(radiusPropPtr);
    CanvasPropertyPaint* paintProp = reinterpret_cast<CanvasPropertyPaint*>(paintPropPtr);
    renderer->drawCircle(xProp, yProp, radiusProp, paintProp);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    CanvasPropertyPrimitive* leftProp = reinterpret_cast<CanvasPropertyPrimitive*>(leftPropPtr);
    CanvasPropertyPrimitive* topProp = reinterpret_cast<CanvasPropertyPrimitive*>(topPropPtr);
    CanvasPropertyPrimitive* rightProp = reinterpret_cast<CanvasPropertyPrimitive*>(rightPropPtr);
    CanvasPropertyPrimitive* bottomProp = reinterpret_cast<CanvasPropertyPrimitive*>(bottomPropPtr);
    CanvasPropertyPrimitive* rxProp = reinterpret_cast<CanvasPropertyPrimitive*>(rxPropPtr);
    CanvasPropertyPrimitive* ryProp = reinterpret_cast<CanvasPropertyPrimitive*>(ryPropPtr);
    CanvasPropertyPaint* paintProp = reinterpret_cast<CanvasPropertyPaint*>(paintPropPtr);
    renderer->drawRoundRect(leftProp, topProp, rightProp, bottomProp, rxProp, ryProp, paintProp);
}

void GLES20Canvas::nDrawColor(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int32 color,
    /* [in] */ Int32 modeHandle)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkPorterDuff::Mode mode = static_cast<SkPorterDuff::Mode>(modeHandle);
    renderer->drawColor(color, SkPorterDuff::ToXfermodeMode(mode));
}

void GLES20Canvas::nDrawLines(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawLines(points->GetPayload() + offset, count, paint);
}

void GLES20Canvas::nDrawOval(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawOval(left, top, right, bottom, paint);
}

void GLES20Canvas::nDrawPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawPath(path, paint);
}

void GLES20Canvas::nDrawRects(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Int64 regionPtr,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkRegion* region = reinterpret_cast<SkRegion*>(regionPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    if (paint->getStyle() != NativePaint::kFill_Style ||
            (paint->isAntiAlias() && !renderer->isCurrentTransformSimple())) {
        SkRegion::Iterator it(*region);
        while (!it.done()) {
            const SkIRect& r = it.rect();
            renderer->drawRect(r.fLeft, r.fTop, r.fRight, r.fBottom, paint);
            it.next();
        }
    } else {
        int count = 0;
        Vector<float> rects;
        SkRegion::Iterator it(*region);
        while (!it.done()) {
            const SkIRect& r = it.rect();
            rects.push(r.fLeft);
            rects.push(r.fTop);
            rects.push(r.fRight);
            rects.push(r.fBottom);
            count += 4;
            it.next();
        }
        renderer->drawRects(rects.array(), count, paint);
    }
}

void GLES20Canvas::nDrawPoints(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawPoints(points->GetPayload() + offset, count, paint);
}

void GLES20Canvas::nDrawRect(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int64 paintPtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawRect(left, top, right, bottom, paint);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    renderer->drawRoundRect(left, top, right, bottom, rx, ry, paint);
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
    String str(*text);
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);

    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    renderText(renderer, str.GetChar16s()->GetPayload() + index, count, x, y, bidiFlags, paint, typeface);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);

    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    renderText(renderer, text.GetChar16s()->GetPayload() + start, end - start, x, y, bidiFlags, paint, typeface);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    String str(*text);
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    renderTextOnPath(renderer, str.GetChar16s()->GetPayload() + index, count, path,
            hOffset, vOffset, bidiFlags, paint, typeface);
}

void GLES20Canvas::nDrawTextOnPath(
    /* [in] */ Int64 rendererPtr,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int64 pathPtr,
    /* [in] */ Float hOffset,
    /* [in] */ Float vOffset,
    /* [in] */ Int32 bidiFlags,
    /* [in] */ Int64 paintPtr,
    /* [in] */ Int64 typefacePtr)
{
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    SkPath* path = reinterpret_cast<SkPath*>(pathPtr);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    renderTextOnPath(renderer, text.GetChar16s()->GetPayload() + start, end - start, path,
            hOffset, vOffset, bidiFlags, paint, typeface);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    String str(*text);
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    int bidiFlags = isRtl ? kBidi_Force_RTL : kBidi_Force_LTR;
    renderTextRun(renderer, str.GetChar16s()->GetPayload() + contextIndex, index - contextIndex,
            count, contextCount, x, y, bidiFlags, paint, typeface);
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
    DisplayListRenderer* renderer = reinterpret_cast<DisplayListRenderer*>(rendererPtr);
    Int32 count = end - start;
    Int32 contextCount = contextEnd - contextStart;
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    TypefaceImpl* typeface = reinterpret_cast<TypefaceImpl*>(typefacePtr);

    int bidiFlags = isRtl ? kBidi_Force_RTL : kBidi_Force_LTR;
    renderTextRun(renderer, text.GetChar16s()->GetPayload() + contextStart, start - contextStart,
            count, contextCount, x, y, bidiFlags, paint, typeface);
}

} // namespace View
} // namespace Droid
} // namespace Elastos