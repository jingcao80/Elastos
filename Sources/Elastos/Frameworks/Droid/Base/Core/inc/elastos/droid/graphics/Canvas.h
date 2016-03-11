
#ifndef __ELASTOS_DROID_GRAPHICS_CANVAS_H__
#define __ELASTOS_DROID_GRAPHICS_CANVAS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmap.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastosx::Microedition::Khronos::Opengles::IGL;

namespace Elastos {
namespace Droid {
namespace View {
    class GraphicBuffer;
    class Surface;
}
}
}

namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_Canvas;

class Canvas
    : public Object
    , public ICanvas
{
    friend class Elastos::Droid::View::GraphicBuffer;
    friend class Elastos::Droid::View::Surface;

public:
    CAR_INTERFACE_DECL();

    Canvas();

    virtual ~Canvas();

public:
    CARAPI IsRecordingFor(
        /* [in] */ IInterface* it,
        /* [out] */ Boolean* res);

    CARAPI IsHardwareAccelerated(
        /* [out] */ Boolean* isAccelerated);

    CARAPI SetBitmap(
        /* [in] */ IBitmap* bitmap);

    CARAPI SetViewport(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /** @hide */
    CARAPI SetHighContrastText(
        /* [in] */ Boolean highContrastText);

    /** @hide */
    CARAPI InsertReorderBarrier();

    /** @hide */
    CARAPI InsertInorderBarrier();

    CARAPI IsOpaque(
        /* [out] */ Boolean* isOpaque);

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI GetDensity(
        /* [out] */ Int32* density);

    CARAPI SetDensity(
        /* [in] */ Int32 density);

    CARAPI SetScreenDensity(
        /* [in] */ Int32 density);

    CARAPI GetMaximumBitmapWidth(
        /* [out] */ Int32* width);

    CARAPI GetMaximumBitmapHeight(
        /* [out] */ Int32* height);

    CARAPI Save(
        /* [out] */ Int32* count);

    CARAPI Save(
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* count);

    CARAPI SaveLayer(
        /* [in] */ IRectF* bounds,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* count);

    /**
     * Convenience for saveLayer(bounds, paint, {@link #ALL_SAVE_FLAG})
     */
    CARAPI SaveLayer(
        /* [in] */ /*@Nullable*/ IRectF* bounds,
        /* [in] */ /*@Nullable*/ IPaint* paint,
        /* [out] */ Int32* count);

    CARAPI SaveLayer(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* count);

    /**
     * Convenience for saveLayer(left, top, right, bottom, paint, {@link #ALL_SAVE_FLAG})
     */
    CARAPI SaveLayer(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ /*@Nullable*/ IPaint* paint,
        /* [out] */ Int32* count);

    CARAPI SaveLayerAlpha(
        /* [in] */ IRectF* bounds,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* count);

    /**
     * Convenience for saveLayerAlpha(bounds, alpha, {@link #ALL_SAVE_FLAG})
     */
    CARAPI SaveLayerAlpha(
        /* [in] */ /*@Nullable*/ IRectF* bounds,
        /* [in] */ Int32 alpha,
        /* [out] */ Int32* count);

    CARAPI SaveLayerAlpha(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 saveFlags,
        /* [out] */ Int32* count);

    /**
     * Helper for saveLayerAlpha(left, top, right, bottom, alpha, {@link #ALL_SAVE_FLAG})
     */
    CARAPI SaveLayerAlpha(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int32 alpha,
        /* [out] */ Int32* count);

    CARAPI Restore();

    CARAPI GetSaveCount(
        /* [out] */ Int32* count);

    CARAPI RestoreToCount(
        /* [in] */ Int32 saveCount);

    CARAPI Translate(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI Scale(
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    CARAPI Scale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);

    CARAPI Rotate(
        /* [in] */ Float degrees);

    CARAPI Rotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);

    CARAPI Skew(
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    CARAPI Concat(
        /* [in] */ IMatrix* matrix);

    CARAPI SetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI GetMatrix(
        /* [in] */ IMatrix* ctm);

    CARAPI GetMatrix(
        /* [out] */ IMatrix** metrix);

    CARAPI ClipRect(
        /* [in] */ IRectF* rect,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ IRect* rect,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ IRectF* rect,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ IRect* rect,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipPath(
        /* [in] */ IPath* path,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipPath(
        /* [in] */ IPath* path,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRegion(
        /* [in] */ IRegion* region,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI ClipRegion(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI GetDrawFilter(
        /* [out] */ IDrawFilter** filter);

    CARAPI SetDrawFilter(
        /* [in] */ IDrawFilter* filter);

    CARAPI QuickReject(
        /* [in] */ IRectF* rect,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* isNotIntersect);

    CARAPI QuickReject(
        /* [in] */ IPath* path,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* isNotIntersect);

    CARAPI QuickReject(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ CanvasEdgeType type,
        /* [out] */ Boolean* isNotIntersect);

    CARAPI GetClipBounds(
        /* [in] */ IRect* bounds,
        /* [out] */ Boolean* isNotEmpty);

    CARAPI GetClipBounds(
        /* [out] */ IRect** bounds);

    CARAPI DrawRGB(
        /* [in] */ Int32 r,
        /* [in] */ Int32 g,
        /* [in] */ Int32 b);

    CARAPI DrawARGB(
        /* [in] */ Int32 a,
        /* [in] */ Int32 r,
        /* [in] */ Int32 g,
        /* [in] */ Int32 b);

    CARAPI DrawColor(
        /* [in] */ Int32 color);

    CARAPI DrawColor(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    CARAPI DrawPaint(
        /* [in] */ IPaint* paint);

    CARAPI DrawPoints(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ IPaint* paint);

    CARAPI DrawPoints(
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ IPaint* paint);

    CARAPI DrawPoint(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

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

    CARAPI DrawRect(
        /* [in] */ IRectF* rect,
        /* [in] */ IPaint* paint);

    CARAPI DrawRect(
        /* [in] */ IRect* r,
        /* [in] */ IPaint* paint);

    CARAPI DrawRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ IPaint* paint);

    CARAPI DrawOval(
        /* [in] */ IRectF* oval,
        /* [in] */ IPaint* paint);

    /**
     * Draw the specified oval using the specified paint. The oval will be
     * filled or framed based on the Style in the paint.
     */
    CARAPI DrawOval(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ /*@NonNull*/ IPaint* paint);

    CARAPI DrawCircle(
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ Float radius,
        /* [in] */ IPaint* paint);

    CARAPI DrawArc(
        /* [in] */ IRectF* oval,
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle,
        /* [in] */ Boolean useCenter,
        /* [in] */ IPaint* paint);

    /**
     * <p>Draw the specified arc, which will be scaled to fit inside the
     * specified oval.</p>
     *
     * <p>If the start angle is negative or >= 360, the start angle is treated
     * as start angle modulo 360.</p>
     *
     * <p>If the sweep angle is >= 360, then the oval is drawn
     * completely. Note that this differs slightly from SkPath::arcTo, which
     * treats the sweep angle modulo 360. If the sweep angle is negative,
     * the sweep angle is treated as sweep angle modulo 360</p>
     *
     * <p>The arc is drawn clockwise. An angle of 0 degrees correspond to the
     * geometric angle of 0 degrees (3 o'clock on a watch.)</p>
     *
     * @param startAngle Starting angle (in degrees) where the arc begins
     * @param sweepAngle Sweep angle (in degrees) measured clockwise
     * @param useCenter If true, include the center of the oval in the arc, and
                        close it if it is being stroked. This will draw a wedge
     * @param paint      The paint used to draw the arc
     */
    CARAPI DrawArc(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle,
        /* [in] */ Boolean useCenter,
        /* [in] */ /*@NonNull*/ IPaint* paint);

    CARAPI DrawRoundRect(
        /* [in] */ IRectF* rect,
        /* [in] */ Float rx,
        /* [in] */ Float ry,
        /* [in] */ IPaint* paint);

    /**
     * Draw the specified round-rect using the specified paint. The roundrect
     * will be filled or framed based on the Style in the paint.
     *
     * @param rx    The x-radius of the oval used to round the corners
     * @param ry    The y-radius of the oval used to round the corners
     * @param paint The paint used to draw the roundRect
     */
    CARAPI DrawRoundRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float rx,
        /* [in] */ Float ry,
        /* [in] */ /*@NonNull*/ IPaint* paint);

    CARAPI DrawPath(
        /* [in] */ IPath* path,
        /* [in] */ IPaint* paint);

    /**
     * Draws the specified bitmap as an N-patch (most often, a 9-patches.)
     *
     * @param patch The ninepatch object to render
     * @param dst The destination rectangle.
     * @param paint The paint to draw the bitmap with. may be null
     *
     * @hide
     */
    CARAPI DrawPatch(
        /* [in] */ /*@NonNull*/ INinePatch* patch,
        /* [in] */ /*@NonNull*/ IRect* dst,
        /* [in] */ /*@Nullable*/ IPaint* paint);

    CARAPI DrawPatch(
        /* [in] */ /*@NonNull*/ INinePatch* patch,
        /* [in] */ IRectF* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IRect* src,
        /* [in] */ IRectF* dst,
        /* [in] */ IPaint* paint);

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IRect* src,
        /* [in] */ IRect* dst,
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

    CARAPI DrawBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ IMatrix* matrix,
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

    CARAPI DrawText(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPaint* paint);

    CARAPI DrawText(
        /* [in] */ const String& text,
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
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
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

    CARAPI DrawPosText(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* pos,
        /* [in] */ IPaint* paint);

    CARAPI DrawPosText(
        /* [in] */ const String& text,
        /* [in] */ ArrayOf<Float>* pos,
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

    CARAPI DrawPicture(
        /* [in] */ IPicture* picture);

    CARAPI DrawPicture(
        /* [in] */ IPicture* picture,
        /* [in] */ IRectF* dst);

    CARAPI DrawPicture(
        /* [in] */ IPicture* picture,
        /* [in] */ IRect* dst);

    CARAPI SetSurfaceFormat(
        /* [in] */ Int32 format);

    CARAPI GetNativeCanvas(
        /* [out] */ Int64* natvieCanvas);

    /**
     * Releases the resources associated with this canvas.
     *
     * @hide
     */
    CARAPI ReleaseResources();

    static CARAPI_(void) FreeCaches();

    static CARAPI_(void) FreeTextLayoutCaches();

    /** @hide */
    CARAPI_(Int64) GetNativeCanvasWrapper();

    /** @hide */
    CARAPI_(Boolean) IsRecordingFor(
        /* [in] */ IInterface* o);

protected:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ Int64 nativeCanvas);

    virtual CARAPI_(AutoPtr<IGL>) GetGL();

    static CARAPI CheckRange(
        /* [in] */ Int32 length,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    static CARAPI ThrowIfCannotDraw(
        /* [in] */ IBitmap* bitmap);

private:
    CARAPI_(Int64) InitRaster(
        /* [in] */ Int64 bitmap);

    static CARAPI_(void) NativeSetBitmap(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeBitmap,
        /* [in] */ Boolean copyState);

    static CARAPI_(Boolean) NativeIsOpaque(
        /* [in] */ Int64 canvasHandle);

    static CARAPI_(Int32) NativeGetWidth(
        /* [in] */ Int64 canvasHandle);

    static CARAPI_(Int32) NativeGetHeight(
        /* [in] */ Int64 canvasHandle);

    static CARAPI_(Int32) NativeSave(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Int32 saveFlags);

    static CARAPI_(Int32) NativeSaveLayer(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float l,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float b,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int32 flags);

    static CARAPI_(Int32) NativeSaveLayerAlpha(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float l,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float b,
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 flags);

    static CARAPI_(void) NativeRestore(
        /* [in] */ Int64 canvasHandle);

    static CARAPI_(void) NativeRestoreToCount(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Int32 saveCount);

    static CARAPI_(Int32) NativeGetSaveCount(
        /* [in] */ Int64 canvasHandle);

    static CARAPI_(void) NativeTranslate(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    static CARAPI_(void) NativeScale(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) NativeRotate(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Float degrees);

    static CARAPI_(void) NativeSkew(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) NativeConcat(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(void) NativeSetMatrix(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(Boolean) NativeClipRect(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ RegionOp op);

    static CARAPI_(Boolean) NativeClipPath(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativePath,
        /* [in] */ RegionOp op);

    static CARAPI_(Boolean) NativeClipRegion(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeRegion,
        /* [in] */ RegionOp op);

    static CARAPI_(void) NativeSetDrawFilter(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeFilter);

    static CARAPI_(Boolean) NativeGetClipBounds(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ IRect* bounds);

    static CARAPI_(void) NativeGetCTM(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(Boolean) NativeQuickReject(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativePath);

    static CARAPI_(Boolean) NativeQuickReject(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    static CARAPI_(void) NativeDrawColor(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode);

    static CARAPI_(void) NativeDrawPaint(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawPoint(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int64 paintHandle);

    static CARAPI_(void) NativeDrawPoints(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int64 paintHandle);

    static CARAPI_(void) NativeDrawLine(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float stopX,
        /* [in] */ Float stopY,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawLines(
        /* [in] */ Int64 canvasHandle,
        /* [in] */ ArrayOf<Float>* pts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int64 paintHandle);

    static CARAPI_(void) NativeDrawRect(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawOval(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawCircle(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float cx,
        /* [in] */ Float cy,
        /* [in] */ Float radius,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawArc(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float startAngle,
        /* [in] */ Float sweep,
        /* [in] */ Boolean useCenter,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawRoundRect(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float rx,
        /* [in] */ Float ry,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawRoundRect(
        /* [in] */ Int32 nativeCanvas,
        /* [in] */ IRectF* rect,
        /* [in] */ Float rx,
        /* [in] */ Float ry,
        /* [in] */ Int32 nativePaint);

    static CARAPI_(void) NativeDrawPath(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativePath,
        /* [in] */ Int64 nativePaint);

    CARAPI_(void) NativeDrawBitmap(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeBitmap,
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int32 canvasDensity,
        /* [in] */ Int32 screenDensity,
        /* [in] */ Int32 bitmapDensity);

    CARAPI_(void) NativeDrawBitmap(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeBitmap,
        /* [in] */ Float srcLeft,
        /* [in] */ Float srcTop,
        /* [in] */ Float srcRight,
        /* [in] */ Float srcBottom,
        /* [in] */ Float dstLeft,
        /* [in] */ Float dstTop,
        /* [in] */ Float dstRight,
        /* [in] */ Float dstBottom,
        /* [in] */ Int64 nativePaintOrZero,
        /* [in] */ Int32 screenDensity,
        /* [in] */ Int32 bitmapDensity);

    static CARAPI_(void) NativeDrawBitmap(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean hasAlpha,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawBitmapMatrix(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeBitmap,
        /* [in] */ Int64 nativeMatrix,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawBitmapMesh(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ Int64 nativeBitmap,
        /* [in] */ Int32 meshWidth,
        /* [in] */ Int32 meshHeight,
        /* [in] */ ArrayOf<Float>* verts,
        /* [in] */ Int32 vertIndex,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 colorIndex,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawVertices(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ CanvasVertexMode mode,
        /* [in] */ Int32 vertexCount,
        /* [in] */ ArrayOf<Float>* verts,
        /* [in] */ Int32 vertIndex,
        /* [in] */ ArrayOf<Float>* texs,
        /* [in] */ Int32 texIndex,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 colorIndex,
        /* [in] */ ArrayOf<Int16>* indices,
        /* [in] */ Int32 indexIndex,
        /* [in] */ Int32 indexCount,
        /* [in] */ Int64 nativePaint);

    static CARAPI_(void) NativeDrawText(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) NativeDrawText(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) NativeDrawTextRun(
        /* [in] */ Int64 nativeCanvas,
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

    static CARAPI_(void) NativeDrawTextRun(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextCount,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Boolean isRtl,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) NativeDrawTextOnPath(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count,
        /* [in] */ Int64 nativePath,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) NativeDrawTextOnPath(
        /* [in] */ Int64 nativeCanvas,
        /* [in] */ const String& text,
        /* [in] */ Int64 nativePath,
        /* [in] */ Float hOffset,
        /* [in] */ Float vOffset,
        /* [in] */ Int32 bidiFlags,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Int64 nativeTypeface);

    static CARAPI_(void) NativeFinalizer(
        /* [in] */ Int64 nativeCanvas);

    /**
     * setBitmap() variant for native callers with a raw bitmap handle.
     */
    CARAPI SetNativeBitmap(
        /* [in] */ Int64 bitmapHandle);

    CARAPI Dispose();

public:
    Int64 mNativeCanvasWrapper;

    Int32 mDensity;

    Int32 mScreenDensity;

private:
    AutoPtr<CBitmap> mBitmap;

    // optional field set by the caller
    AutoPtr<IDrawFilter> mDrawFilter;

    Int32 mSurfaceFormat;

    static const Int32 MAXMIMUM_BITMAP_SIZE = 32766;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CANVAS_H__
