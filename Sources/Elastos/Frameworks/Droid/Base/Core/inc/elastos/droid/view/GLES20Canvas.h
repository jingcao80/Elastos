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

#ifndef __ELASTOS_DROID_VIEW_GLES20CANVAS_H__
#define __ELASTOS_DROID_VIEW_GLES20CANVAS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Text.h"

#include "elastos/droid/graphics/TemporaryBuffer.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/HardwareCanvas.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::ICanvasProperty;
using Elastos::Droid::Graphics::IDrawFilter;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::INinePatch;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::TemporaryBuffer;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::CanvasVertexMode;
using Elastos::Droid::Graphics::CanvasEdgeType;
using Elastos::Droid::Graphics::RegionOp;
using Elastos::Droid::Text::IGraphicsOperations;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace View {

class GLES20Canvas
    : public HardwareCanvas
    , public IGLES20Canvas
{
    friend class HardwareRenderer;
    class CanvasFinalizer
        : public Object
    {
    public:
        CanvasFinalizer(
        /* [in] */ Int64 renderer);

        ~CanvasFinalizer();

    private:
        Int64 mRenderer;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("GLES20Canvas")

    CARAPI constructor();

    virtual ~GLES20Canvas();

    CARAPI GetWidth(
        /* [out] */ Int32* res);

    CARAPI GetHeight(
        /* [out] */ Int32* res);

    CARAPI GetMaximumBitmapWidth(
        /* [out] */ Int32* res);

    CARAPI GetMaximumBitmapHeight(
        /* [out] */ Int32* res);

    CARAPI SetViewport(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SetHighContrastText(
        /* [in] */ Boolean highContrastText);

    CARAPI InsertReorderBarrier();

    CARAPI InsertInorderBarrier();

    CARAPI OnPreDraw(
        /* [in] */ IRect* dirty,
        /* [out] */ Int32* res);

    CARAPI OnPostDraw();

    CARAPI CallDrawGLFunction(
        /* [in] */ Int64 drawGLFunction,
        /* [out] */ Int32* res);

    CARAPI DrawRenderNode(
        /* [in] */ IRenderNode* renderNode,
        /* [in] */ IRect* dirty,
        /* [in] */ Int32 flags,
        /* [out] */ Int32* res);

    CARAPI DrawHardwareLayer(
        /* [in] */ IHardwareLayer* layer,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI ClipPath(
        /* [in] */ IPath* path,
        /* [out] */ Boolean* res);

    CARAPI ClipPath(
        /* [in] */ IPath* path,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ IRect* left,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ IRect* rect,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ IRectF* left,
        /* [out] */ Boolean* res);

    CARAPI ClipRect(
        /* [in] */ IRectF* rect,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* res);

    CARAPI ClipRegion(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* res);

    CARAPI ClipRegion(
        /* [in] */ IRegion* region,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* res);

    CARAPI GetClipBounds(
        /* [in] */ IRect* bounds,
        /* [out] */ Boolean* res);

    CARAPI QuickReject(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* res);

    CARAPI QuickReject(
        /* [in] */ IPath* path,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* res);

    CARAPI QuickReject(
        /* [in] */ IRectF* rect,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* res);

    ///////////////////////////////////////////////////////////////////////////
    // Transformations
    ///////////////////////////////////////////////////////////////////////////

    CARAPI Translate(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI Skew(
        /* [in] */ Float sx,
        /* [in] */ Float sy);


    CARAPI Rotate(
        /* [in] */ Float degrees);

    CARAPI Scale(
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    CARAPI SetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI GetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI Concat(
        /* [in] */ IMatrix* matrix);

    ///////////////////////////////////////////////////////////////////////////
    // State management
    ///////////////////////////////////////////////////////////////////////////

    CARAPI Save(
        /* [out] */ Int32* res);

    CARAPI Save(
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* res);

    CARAPI SaveLayer(
        /* [in] */ IRectF* bounds,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* res);

    CARAPI SaveLayer(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* res);

    CARAPI SaveLayerAlpha(
        /* [in] */ IRectF* bounds,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* res);

    CARAPI SaveLayerAlpha(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* res);

    CARAPI Restore();

    CARAPI RestoreToCount(
        /* [in] */ Int32 saveCount);

    CARAPI GetSaveCount(
        /* [out] */ Int32* res);

    ///////////////////////////////////////////////////////////////////////////
    // Filtering
    ///////////////////////////////////////////////////////////////////////////

    CARAPI SetDrawFilter(
        /* [in] */ IDrawFilter* filter);

    CARAPI GetDrawFilter(
        /* [out] */ IDrawFilter** filter);

    ///////////////////////////////////////////////////////////////////////////
    // Drawing
    ///////////////////////////////////////////////////////////////////////////

    CARAPI DrawArc(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle,
        /* [in] */ Boolean useCenter,
        /* [in] */ IPaint* paint);

    CARAPI DrawARGB(
        /* [in] */ Int32 a,
        /* [in] */ Int32 r,
        /* [in] */ Int32 g,
        /* [in] */ Int32 b);

    CARAPI DrawPatch(
        /* [in] */ INinePatch* patch,
        /* [in] */ IRect* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawPatch(
        /* [in] */ INinePatch* patch,
        /* [in] */ IRectF* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IMatrix* matrix,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IRect* src,
        /* [in] */ IRect* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IRect* src,
        /* [in] */ IRectF* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean hasAlpha,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean hasAlpha,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmapMesh(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 meshWidth,
        /* [in] */ Int32 meshHeight,
        /* [in] */ ArrayOf<Float>* verts,
        /* [in] */ Int32 vertOffset,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 colorOffset,
        /* [in] */ IPaint* paint);

    CARAPI DrawCircle(
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ Float radius,
        /* [in] */ IPaint* paint);

    CARAPI DrawCircle(
        /* [in] */ ICanvasProperty* cx,
        /* [in] */ ICanvasProperty* cy,
        /* [in] */ ICanvasProperty* radius,
        /* [in] */ ICanvasProperty* paint);

    using Canvas::DrawRoundRect;

    CARAPI DrawRoundRect(
        /* [in] */ ICanvasProperty* left,
        /* [in] */ ICanvasProperty* top,
        /* [in] */ ICanvasProperty* right,
        /* [in] */ ICanvasProperty* bottom,
        /* [in] */ ICanvasProperty* rx,
        /* [in] */ ICanvasProperty* ry,
        /* [in] */ ICanvasProperty* paint);

    CARAPI DrawColor(
        /* [in] */ Int32 color);

    CARAPI DrawColor(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    CARAPI DrawLine(
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float stopX,
        /* [in] */ Float stopY,
        /* [in] */ IPaint* paint);

    CARAPI DrawLines(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ IPaint* paint);

    CARAPI DrawLines(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ IPaint* paint);

    CARAPI DrawOval(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ IPaint* paint);

    CARAPI DrawPaint(
        /* [in] */ IPaint* paint);

    CARAPI DrawPath(
        /* [in] */ IPath* path,
        /* [in] */ IPaint* paint);

    CARAPI DrawPicture(
        /* [in] */ IPicture* picture);

    CARAPI DrawPoint(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawPoints(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ IPaint* paint);

    CARAPI DrawPoints(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ IPaint* paint);

    // Note: drawPosText just uses implementation in Canvas

    CARAPI DrawRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ IPaint* paint);

    CARAPI DrawRect(
        /* [in] */ IRect* r,
        /* [in] */ IPaint* paint);

    CARAPI DrawRect(
        /* [in] */ IRectF* r,
        /* [in] */ IPaint* paint);

    CARAPI DrawRGB(
        /* [in] */ Int32 r,
        /* [in] */ Int32 g,
        /* [in] */ Int32 b);

    CARAPI DrawRoundRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float rx,
        /* [in] */ Float ry,
        /* [in] */ IPaint* paint);

    CARAPI DrawText(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawText(
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawText(
        /* [in] */ const String& text,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawTextOnPath(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ IPath* path,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ IPaint* paint);

    CARAPI DrawTextOnPath(
        /* [in] */ const String& text,
        /* [in] */ IPath* path,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ IPaint* paint);

    CARAPI DrawTextRun(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Int32 contextIndex,
        /* [in] */ Int32 contextCount,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ IPaint* paint);

    CARAPI DrawTextRun(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ IPaint* paint);

    CARAPI DrawVertices(
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
        /* [in] */ IPaint* paint);

    CARAPI GetRenderer(
        /* [out] */ Int64* renderer);

    static CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    static CARAPI_(Boolean) nIsAvailable();

protected:
    GLES20Canvas();

    static CARAPI nFinishRecording(
        /* [in] */ Int64 renderer,
        /* [out] */ Int64* res);

private:
    CARAPI_(AutoPtr<IRect>) GetInternalClipBounds();

    CARAPI_(AutoPtr<IRectF>) GetPathBounds();

    CARAPI_(void) SetupFinalizer();

    CARAPI_(AutoPtr<ArrayOf<Float> >) GetPointStorage();

    CARAPI_(AutoPtr<ArrayOf<Float> >) GetLineStorage();

    static CARAPI_(void) nSetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    static CARAPI_(Boolean) IsAvailable();

    static CARAPI_(Int64) nCreateDisplayListRenderer();

    static CARAPI_(void) nResetDisplayListRenderer(
        /* [in] */ Int64 renderer);

    static CARAPI_(void) nDestroyRenderer(
        /* [in] */ Int64 renderer);

    static CARAPI_(Int32) nGetMaximumTextureWidth();

    static CARAPI_(Int32) nGetMaximumTextureHeight();

    static CARAPI_(void) nSetViewport(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(void) nSetHighContrastText(
        /* [in] */ Int64 renderer,
        /* [in] */ Boolean highContrastText);

    static CARAPI_(void) nInsertReorderBarrier(
        /* [in] */ Int64 renderer,
        /* [in] */ Boolean enableReorder);

    static CARAPI_(Int32) nPrepare(
        /* [in] */ Int64 renderer,
        /* [in] */ Boolean opaque);

    static CARAPI_(Int32) nPrepareDirty(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Boolean opaque);

    static CARAPI_(void) nFinish(
        /* [in] */ Int64 renderer);

    static CARAPI_(Int32) nCallDrawGLFunction(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 drawGLFunction);

    static CARAPI_(Int32) nDrawRenderNode(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 renderNode,
        /* [in] */ IRect* dirty,
        /* [in] */ Int32 flags);

    static CARAPI_(void) nDrawLayer(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 layer,
        /* [in] */ Float x,
        /* [in] */ Float y);

    static CARAPI_(Boolean) nClipPath(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 path,
        /* [in] */ Int32 op);

    static CARAPI_(Boolean) nClipRect(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int32 op);

    static CARAPI_(Boolean) nClipRect(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 op);

    static CARAPI_(Boolean) nClipRegion(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 region,
        /* [in] */ Int32 op);

    static CARAPI_(Boolean) nGetClipBounds(
        /* [in] */ Int64 renderer,
        /* [in] */ IRect* bounds);

    static CARAPI_(Boolean) nQuickReject(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    static CARAPI_(void) nTranslate(
        /* [in] */ Int64 renderer,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    static CARAPI_(void) nSkew(
        /* [in] */ Int64 renderer,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) nRotate(
        /* [in] */ Int64 renderer,
        /* [in] */ Float degrees);

    static CARAPI_(void) nScale(
        /* [in] */ Int64 renderer,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) nSetMatrix(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 matrix);

    static CARAPI_(void) nGetMatrix(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 matrix);

    static CARAPI_(void) nConcatMatrix(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 matrix);

    static CARAPI_(Int32) nSave(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 flags);

    static CARAPI_(Int32) nSaveLayer(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 paint,
        /* [in] */ Int32 saveFlags);

    static CARAPI_(Int32) nSaveLayer(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 paint,
        /* [in] */ Int32 saveFlags);

    static CARAPI_(Int32) nSaveLayerAlpha(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags);

    static CARAPI_(Int32) nSaveLayerAlpha(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags);

    static CARAPI_(void) nRestore(
        /* [in] */ Int64 renderer);

    static CARAPI_(void) nRestoreToCount(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 saveCount);

    static CARAPI_(Int32) nGetSaveCount(
        /* [in] */ Int64 renderer);

    static CARAPI_(void) nResetPaintFilter(
        /* [in] */ Int64 renderer);

    static CARAPI_(void) nSetupPaintFilter(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 clearBits,
        /* [in] */ Int32 setBits);

    static CARAPI_(void) nDrawArc(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle,
        /* [in] */ Boolean useCenter,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawPatch(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int64 chunk,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawBitmap(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawBitmap(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int64 matrix,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawBitmap(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Float srcLeft,
        /* [in] */ Float srcTop,
        /* [in] */ Float srcRight,
        /* [in] */ Float srcBottom,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 paint);


    static CARAPI_(void) nDrawBitmap(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean hasAlpha,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) nDrawBitmapMesh(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 meshWidth,
        /* [in] */ Int32 meshHeight,
        /* [in] */ ArrayOf<Float>* verts,
        /* [in] */ Int32 vertOffset,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 colorOffset,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawCircle(
        /* [in] */ Int64 renderer,
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ Float radius,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawCircle(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 propCx,
        /* [in] */ Int64 propCy,
        /* [in] */ Int64 propRadius,
        /* [in] */ Int64 propPaint);

    static CARAPI_(void) nDrawRoundRect(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 propLeft,
        /* [in] */ Int64 propTop,
        /* [in] */ Int64 propRight,
        /* [in] */ Int64 propBottom,
        /* [in] */ Int64 propRx,
        /* [in] */ Int64 propRy,
        /* [in] */ Int64 propPaint);

    static CARAPI_(void) nDrawColor(
        /* [in] */ Int64 renderer,
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode);

    static CARAPI_(void) nDrawLines(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawOval(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawPath(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 path,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawRects(
        /* [in] */ Int64 renderer,
        /* [in] */ Int64 region,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawPoints(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawRect(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawRoundRect(
        /* [in] */ Int64 renderer,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float rx,
        /* [in] */ Float y,
        /* [in] */ Int64 paint);

    static CARAPI_(void) nDrawText(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 paint,
        /* [in] */ Int64 typeface);

    static CARAPI_(void) nDrawText(
        /* [in] */ Int64 renderer,
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 paint,
        /* [in] */ Int64 typeface);

    static CARAPI_(void) nDrawTextOnPath(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Int64 path,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 typeface);

    static CARAPI_(void) nDrawTextOnPath(
        /* [in] */ Int64 renderer,
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int64 path,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 typeface);

    static CARAPI_(void) nDrawTextRun(
        /* [in] */ Int64 renderer,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Int32 contextIndex,
        /* [in] */ Int32 contextCount,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) nDrawTextRun(
        /* [in] */ Int64 renderer,
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

protected:
    Int64 mRenderer;

private:
    Boolean mOpaque;

    // The native renderer will be destroyed when this object dies.
    // DO NOT overwrite this reference once it is set.
    AutoPtr<CanvasFinalizer> mFinalizer;

    Int32 mWidth;
    Int32 mHeight;

    AutoPtr<ArrayOf<Float> > mPoint;
    AutoPtr<ArrayOf<Float> > mLine;

    AutoPtr<IRect> mClipBounds;
    AutoPtr<IRectF> mPathBounds;

    AutoPtr<IDrawFilter> mFilter;

    static Boolean sIsAvailable;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif