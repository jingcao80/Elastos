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
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <elastos/utility/etl/Map.h>
#include <hwui/Caches.h>
#include <skia/core/SkPath.h>
#include <skia/core/SkMatrix.h>
#include <skia/pathops/SkPathOps.h>

using Elastos::Utility::Etl::Map;

namespace Elastos {
namespace Droid {
namespace Graphics {

// these must be in the same order as their native values
const PathFillType CPath::sFillTypeArray[4] = {
    PathFillType_WINDING,
    PathFillType_EVEN_ODD,
    PathFillType_INVERSE_WINDING,
    PathFillType_INVERSE_EVEN_ODD
};

CAR_OBJECT_IMPL(CPath);
CAR_INTERFACE_IMPL(CPath, Object, IPath);
CPath::CPath()
    : mNativePath(0)
    , mIsSimplePath(FALSE)
    , mLastDirection(-1)
{}

CPath::~CPath()
{
    NativeFinalizer(mNativePath);
}

ECode CPath::constructor()
{
    mNativePath = reinterpret_cast<Int64>(new SkPath());
    return NOERROR;
}

ECode CPath::constructor(
    /* [in] */ IPath* src)
{
    Int64 valNative = 0;
    if (src != NULL) {
        valNative = ((CPath*)src)->mNativePath;
        mIsSimplePath = ((CPath*)src)->mIsSimplePath;
        if (((CPath*)src)->mRects != NULL) {
            CRegion::New(((CPath*)src)->mRects, (IRegion**)&mRects);
        }

        SkPath* val = reinterpret_cast<SkPath*>(valNative);
        mNativePath = reinterpret_cast<Int64>(new SkPath(*val));
    }

    return NOERROR;
}

/**
 * Clear any lines and curves from the path, making it empty.
 * This does NOT change the fill-type setting.
 */
ECode CPath::Reset()
{
    mIsSimplePath = TRUE;
    mLastDirection = -1;
    if (mRects != NULL) mRects->SetEmpty();
    // We promised not to change this, so preserve it around the native
    // call, which does now reset fill type.
    PathFillType fillType;
    GetFillType(&fillType);
    NativeReset(mNativePath);
    return SetFillType(fillType);
}

/**
 * Rewinds the path: clears any lines and curves from the path but
 * keeps the internal data structure for faster reuse.
 */
ECode CPath::Rewind()
{
    mIsSimplePath = TRUE;
    mLastDirection = -1;
    if (mRects != NULL) mRects->SetEmpty();
    NativeRewind(mNativePath);
    return NOERROR;
}

/** Replace the contents of this with the contents of src.
*/
ECode CPath::Set(
    /* [in] */ IPath* src)
{
    CPath* path = (CPath*)src;
    if (this != path) {
        mIsSimplePath = path->mIsSimplePath;
        NativeSet(mNativePath, path->mNativePath);
    }
    return NOERROR;
}

ECode CPath::Op(
    /* [in] */ IPath* path,
    /* [in] */ PathOp op,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    return Op(this, path, op, succeeded);
}

ECode CPath::Op(
    /* [in] */ IPath* path1,
    /* [in] */ IPath* path2,
    /* [in] */ PathOp op,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    if (NativeOp(((CPath*)path1)->mNativePath, ((CPath*)path2)->mNativePath, op, mNativePath)) {
        mIsSimplePath = FALSE;
        mRects = NULL;
        *succeeded = TRUE;
        return NOERROR;
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode CPath::IsConvex(
    /* [out] */ Boolean* isConvex)
{
    return NativeIsConvex(mNativePath);
}

/**
 * Return the path's fill type. This defines how "inside" is
 * computed. The default value is WINDING.
 *
 * @return the path's fill type
 */
ECode CPath::GetFillType(
    /* [out] */ PathFillType* type)
{
    VALIDATE_NOT_NULL(type);

    *type = sFillTypeArray[NativeGetFillType(mNativePath)];
    return NOERROR;
}

/**
 * Set the path's fill type. This defines how "inside" is computed.
 *
 * @param ft The new fill type for this path
 */
ECode CPath::SetFillType(
    /* [in] */ PathFillType ft)
{
    NativeSetFillType(mNativePath, ft);
    return NOERROR;
}

/**
 * Returns true if the filltype is one of the INVERSE variants
 *
 * @return true if the filltype is one of the INVERSE variants
 */
ECode CPath::IsInverseFillType(
    /* [out] */ Boolean* isInverseFillType)
{
    VALIDATE_NOT_NULL(isInverseFillType);

    Int32 ft = NativeGetFillType(mNativePath);
    *isInverseFillType = (ft & PathFillType_INVERSE_WINDING) != 0;
    return NOERROR;
}

/**
 * Toggles the INVERSE state of the filltype
 */
ECode CPath::ToggleInverseFillType()
{
    Int32 ft = NativeGetFillType(mNativePath);
    ft ^= PathFillType_INVERSE_WINDING;
    NativeSetFillType(mNativePath, ft);
    return NOERROR;
}

/**
 * Returns true if the path is empty (contains no lines or curves)
 *
 * @return true if the path is empty (contains no lines or curves)
 */
ECode CPath::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);

    *isEmpty = NativeIsEmpty(mNativePath);
    return NOERROR;
}

/**
 * Returns true if the path specifies a rectangle. If so, and if rect is
 * not null, set rect to the bounds of the path. If the path does not
 * specify a rectangle, return false and ignore rect.
 *
 * @param rect If not null, returns the bounds of the path if it specifies
 *             a rectangle
 * @return     true if the path specifies a rectangle
 */
ECode CPath::IsRect(
    /* [in] */ IRectF* rect,
    /* [out] */ Boolean* isRect)
{
    VALIDATE_NOT_NULL(isRect);

    *isRect = NativeIsRect(mNativePath, rect);
    return NOERROR;
}

/**
 * Compute the bounds of the control points of the path, and write the
 * answer into bounds. If the path contains 0 or 1 points, the bounds is
 * set to (0,0,0,0)
 *
 * @param bounds Returns the computed bounds of the path's control points.
 * @param exact This parameter is no longer used.
 */
ECode CPath::ComputeBounds(
    /* [in] */ IRectF* bounds,
    /* [in] */ Boolean exact)
{
    NativeComputeBounds(mNativePath, bounds);
    return NOERROR;
}

/**
 * Hint to the path to prepare for adding more points. This can allow the
 * path to more efficiently allocate its storage.
 *
 * @param extraPtCount The number of extra points that may be added to this
 *                     path
 */
ECode CPath::IncReserve(
    /* [in] */ Int32 extraPtCount)
{
    NativeIncReserve(mNativePath, extraPtCount);
    return NOERROR;
}

/**
 * Set the beginning of the next contour to the point (x,y).
 *
 * @param x The x-coordinate of the start of a new contour
 * @param y The y-coordinate of the start of a new contour
 */
ECode CPath::MoveTo(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    NativeMoveTo(mNativePath, x, y);
    return NOERROR;
}

/**
 * Set the beginning of the next contour relative to the last point on the
 * previous contour. If there is no previous contour, this is treated the
 * same as moveTo().
 *
 * @param dx The amount to add to the x-coordinate of the end of the
 *           previous contour, to specify the start of a new contour
 * @param dy The amount to add to the y-coordinate of the end of the
 *           previous contour, to specify the start of a new contour
 */
ECode CPath::RMoveTo(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeRMoveTo(mNativePath, dx, dy);
    return NOERROR;
}

/**
 * Add a line from the last point to the specified point (x,y).
 * If no moveTo() call has been made for this contour, the first point is
 * automatically set to (0,0).
 *
 * @param x The x-coordinate of the end of a line
 * @param y The y-coordinate of the end of a line
 */
ECode CPath::LineTo(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mIsSimplePath = FALSE;
    NativeLineTo(mNativePath, x, y);
    return NOERROR;
}

/**
 * Same as lineTo, but the coordinates are considered relative to the last
 * point on this contour. If there is no previous point, then a moveTo(0,0)
 * is inserted automatically.
 *
 * @param dx The amount to add to the x-coordinate of the previous point on
 *           this contour, to specify a line
 * @param dy The amount to add to the y-coordinate of the previous point on
 *           this contour, to specify a line
 */
ECode CPath::RLineTo(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mIsSimplePath = FALSE;
    NativeRLineTo(mNativePath, dx, dy);
    return NOERROR;
}

/**
 * Add a quadratic bezier from the last point, approaching control point
 * (x1,y1), and ending at (x2,y2). If no moveTo() call has been made for
 * this contour, the first point is automatically set to (0,0).
 *
 * @param x1 The x-coordinate of the control point on a quadratic curve
 * @param y1 The y-coordinate of the control point on a quadratic curve
 * @param x2 The x-coordinate of the end point on a quadratic curve
 * @param y2 The y-coordinate of the end point on a quadratic curve
 */
ECode CPath::QuadTo(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2)
{
    mIsSimplePath = FALSE;
    NativeQuadTo(mNativePath, x1, y1, x2, y2);
    return NOERROR;
}

/**
 * Same as quadTo, but the coordinates are considered relative to the last
 * point on this contour. If there is no previous point, then a moveTo(0,0)
 * is inserted automatically.
 *
 * @param dx1 The amount to add to the x-coordinate of the last point on
 *            this contour, for the control point of a quadratic curve
 * @param dy1 The amount to add to the y-coordinate of the last point on
 *            this contour, for the control point of a quadratic curve
 * @param dx2 The amount to add to the x-coordinate of the last point on
 *            this contour, for the end point of a quadratic curve
 * @param dy2 The amount to add to the y-coordinate of the last point on
 *            this contour, for the end point of a quadratic curve
 */
ECode CPath::RQuadTo(
    /* [in] */ Float dx1,
    /* [in] */ Float dy1,
    /* [in] */ Float dx2,
    /* [in] */ Float dy2)
{
    mIsSimplePath = FALSE;
    NativeRQuadTo(mNativePath, dx1, dy1, dx2, dy2);
    return NOERROR;
}

/**
 * Add a cubic bezier from the last point, approaching control points
 * (x1,y1) and (x2,y2), and ending at (x3,y3). If no moveTo() call has been
 * made for this contour, the first point is automatically set to (0,0).
 *
 * @param x1 The x-coordinate of the 1st control point on a cubic curve
 * @param y1 The y-coordinate of the 1st control point on a cubic curve
 * @param x2 The x-coordinate of the 2nd control point on a cubic curve
 * @param y2 The y-coordinate of the 2nd control point on a cubic curve
 * @param x3 The x-coordinate of the end point on a cubic curve
 * @param y3 The y-coordinate of the end point on a cubic curve
 */
ECode CPath::CubicTo(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float x3,
    /* [in] */ Float y3)
{
    mIsSimplePath = FALSE;
    NativeCubicTo(mNativePath, x1, y1, x2, y2, x3, y3);
    return NOERROR;
}

/**
 * Same as cubicTo, but the coordinates are considered relative to the
 * current point on this contour. If there is no previous point, then a
 * moveTo(0,0) is inserted automatically.
 */
ECode CPath::RCubicTo(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float x3,
    /* [in] */ Float y3)
{
    mIsSimplePath = FALSE;
    NativeRCubicTo(mNativePath, x1, y1, x2, y2, x3, y3);
    return NOERROR;
}

/**
 * Append the specified arc to the path as a new contour. If the start of
 * the path is different from the path's current last point, then an
 * automatic lineTo() is added to connect the current contour to the
 * start of the arc. However, if the path is empty, then we call moveTo()
 * with the first point of the arc. The sweep angle is tread mod 360.
 *
 * @param oval        The bounds of oval defining shape and size of the arc
 * @param startAngle  Starting angle (in degrees) where the arc begins
 * @param sweepAngle  Sweep angle (in degrees) measured clockwise, treated
 *                    mod 360.
 * @param forceMoveTo If true, always begin a new contour with the arc
 */
ECode CPath::ArcTo(
    /* [in] */ IRectF* oval,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean forceMoveTo)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    oval->Get(&left, &top, &right, &bottom);
    return ArcTo(left, top, right, bottom, startAngle, sweepAngle, forceMoveTo);
}

/**
 * Append the specified arc to the path as a new contour. If the start of
 * the path is different from the path's current last point, then an
 * automatic lineTo() is added to connect the current contour to the
 * start of the arc. However, if the path is empty, then we call moveTo()
 * with the first point of the arc.
 *
 * @param oval        The bounds of oval defining shape and size of the arc
 * @param startAngle  Starting angle (in degrees) where the arc begins
 * @param sweepAngle  Sweep angle (in degrees) measured clockwise
 */
ECode CPath::ArcTo(
    /* [in] */ IRectF* oval,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    oval->Get(&left, &top, &right, &bottom);
    return ArcTo(left, top, right, bottom, startAngle, sweepAngle, FALSE);
}

ECode CPath::ArcTo(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean forceMoveTo)
{
    mIsSimplePath = FALSE;
    NativeArcTo(mNativePath, left, top, right, bottom, startAngle, sweepAngle, forceMoveTo);
    return NOERROR;
}

/**
 * Close the current contour. If the current point is not equal to the
 * first point of the contour, a line segment is automatically added.
 */
ECode CPath::Close()
{
    mIsSimplePath = FALSE;
    NativeClose(mNativePath);
    return NOERROR;
}

void CPath::DetectSimplePath(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ PathDirection dir)
{
    if (mLastDirection == -1) {
        mLastDirection = dir;
    }
    if (mLastDirection != dir) {
        mIsSimplePath = FALSE;
    }
    else {
        if (mRects == NULL) {
            CRegion::New((IRegion**)&mRects);
        }
        Boolean result;
        mRects->Op((Int32)left, (Int32)top, (Int32)right, (Int32)bottom, RegionOp_UNION, &result);
    }
}

/**
 * Add a closed rectangle contour to the path
 *
 * @param rect The rectangle to add as a closed contour to the path
 * @param dir  The direction to wind the rectangle's contour
 */
ECode CPath::AddRect(
    /* [in] */ IRectF* rect,
    /* [in] */ PathDirection dir)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    rect->Get(&left, &top, &right, &bottom);
    return AddRect(left, top, right, bottom, dir);
}

/**
 * Add a closed rectangle contour to the path
 *
 * @param left   The left side of a rectangle to add to the path
 * @param top    The top of a rectangle to add to the path
 * @param right  The right side of a rectangle to add to the path
 * @param bottom The bottom of a rectangle to add to the path
 * @param dir    The direction to wind the rectangle's contour
 */
ECode CPath::AddRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ PathDirection dir)
{
    DetectSimplePath(left, top, right, bottom, dir);
    NativeAddRect(mNativePath, left, top, right, bottom, dir);
    return NOERROR;
}

ECode CPath::AddOval(
    /* [in] */ IRectF* oval,
    /* [in] */ PathDirection dir)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    oval->Get(&left, &top, &right, &bottom);
    return AddOval(left, top, right, bottom, dir);
}

ECode CPath::AddOval(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ PathDirection dir)
{
    mIsSimplePath = FALSE;
    NativeAddOval(mNativePath, left, top, right, bottom, dir);
    return NOERROR;
}

/**
 * Add a closed circle contour to the path
 *
 * @param x   The x-coordinate of the center of a circle to add to the path
 * @param y   The y-coordinate of the center of a circle to add to the path
 * @param radius The radius of a circle to add to the path
 * @param dir    The direction to wind the circle's contour
 */
ECode CPath::AddCircle(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ PathDirection dir)
{
    mIsSimplePath = FALSE;
    NativeAddCircle(mNativePath, x, y, radius, dir);
    return NOERROR;
}

/**
 * Add the specified arc to the path as a new contour.
 *
 * @param oval The bounds of oval defining the shape and size of the arc
 * @param startAngle Starting angle (in degrees) where the arc begins
 * @param sweepAngle Sweep angle (in degrees) measured clockwise
 */
ECode CPath::AddArc(
    /* [in] */ IRectF* oval,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    oval->Get(&left, &top, &right, &bottom);
    return AddArc(left, top, right, bottom, startAngle, sweepAngle);
}

ECode CPath::AddArc(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    mIsSimplePath = FALSE;
    NativeAddArc(mNativePath, left, top, right, bottom, startAngle, sweepAngle);
    return NOERROR;
}

/**
    * Add a closed round-rectangle contour to the path
 *
 * @param rect The bounds of a round-rectangle to add to the path
 * @param rx   The x-radius of the rounded corners on the round-rectangle
 * @param ry   The y-radius of the rounded corners on the round-rectangle
 * @param dir  The direction to wind the round-rectangle's contour
 */
ECode CPath::AddRoundRect(
    /* [in] */ IRectF* rect,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ PathDirection dir)
{
    Float left = 0, top = 0, right = 0, bottom = 0;
    rect->Get(&left, &top, &right, &bottom);
    return AddRoundRect(left, top, right, bottom, rx, ry, dir);
}

ECode CPath::AddRoundRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ PathDirection dir)
{
    mIsSimplePath = FALSE;
    NativeAddRoundRect(mNativePath, left, top, right, bottom, rx, ry, dir);
    return NOERROR;
}

/**
 * Add a closed round-rectangle contour to the path. Each corner receives
 * two radius values [X, Y]. The corners are ordered top-left, top-right,
 * bottom-right, bottom-left
 *
 * @param rect The bounds of a round-rectangle to add to the path
 * @param radii Array of 8 values, 4 pairs of [X,Y] radii
 * @param dir  The direction to wind the round-rectangle's contour
 */
ECode CPath::AddRoundRect(
    /* [in] */ IRectF* rect,
    /* [in] */ ArrayOf<Float>* radii,
    /* [in] */ PathDirection dir)
{
    if (rect == NULL) {
//        throw new NullPointerException("need rect parameter");
        return E_NULL_POINTER_EXCEPTION;
    }

    Float left = 0, top = 0, right = 0, bottom = 0;
    rect->Get(&left, &top, &right, &bottom);
    return AddRoundRect(left, top, right, bottom, radii, dir);
}

ECode CPath::AddRoundRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ ArrayOf<Float>* radii,
    /* [in] */ PathDirection dir)
{
    if (radii->GetLength() < 8) {
        // throw new ArrayIndexOutOfBoundsException("radii[] needs 8 values");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mIsSimplePath = FALSE;
    NativeAddRoundRect(mNativePath, left, top, right, bottom, radii, dir);
    return NOERROR;
}

/**
 * Add a copy of src to the path, offset by (dx,dy)
 *
 * @param src The path to add as a new contour
 * @param dx  The amount to translate the path in X as it is added
 */
ECode CPath::AddPath(
    /* [in] */ IPath* src,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mIsSimplePath = FALSE;
    NativeAddPath(mNativePath, ((CPath*)src)->mNativePath, dx, dy);
    return NOERROR;
}

/**
 * Add a copy of src to the path
 *
 * @param src The path that is appended to the current path
 */
ECode CPath::AddPath(
    /* [in] */ IPath* src)
{
    mIsSimplePath = FALSE;
    NativeAddPath(mNativePath, ((CPath*)src)->mNativePath);
    return NOERROR;
}

/**
 * Add a copy of src to the path, transformed by matrix
 *
 * @param src The path to add as a new contour
 */
ECode CPath::AddPath(
    /* [in] */ IPath* src,
    /* [in] */ IMatrix* matrix)
{
    if (!((CPath*)src)->mIsSimplePath) mIsSimplePath = FALSE;
    NativeAddPath(mNativePath, ((CPath*)src)->mNativePath,
            ((Matrix*)matrix)->mNativeMatrix);
    return NOERROR;
}

/**
 * Offset the path by (dx,dy), returning true on success
 *
 * @param dx  The amount in the X direction to offset the entire path
 * @param dy  The amount in the Y direction to offset the entire path
 * @param dst The translated path is written here. If this is null, then
 *            the original path is modified.
 */
ECode CPath::Offset(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ IPath* dst)
{
    Int64 dstNative = 0;
    if (dst != NULL) {
        dstNative = ((CPath*)dst)->mNativePath;
        ((CPath*)dst)->mIsSimplePath = FALSE;
    }
    NativeOffset(mNativePath, dx, dy, dstNative);
    return NOERROR;
}

/**
 * Offset the path by (dx,dy), returning true on success
 *
 * @param dx The amount in the X direction to offset the entire path
 * @param dy The amount in the Y direction to offset the entire path
 */
ECode CPath::Offset(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mIsSimplePath = FALSE;
    NativeOffset(mNativePath, dx, dy);
    return NOERROR;
}

/**
 * Sets the last point of the path.
 *
 * @param dx The new X coordinate for the last point
 * @param dy The new Y coordinate for the last point
 */
ECode CPath::SetLastPoint(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mIsSimplePath = FALSE;
    NativeSetLastPoint(mNativePath, dx, dy);
    return NOERROR;
}

/**
 * Transform the points in this path by matrix, and write the answer
 * into dst. If dst is null, then the the original path is modified.
 *
 * @param matrix The matrix to apply to the path
 * @param dst    The transformed path is written here. If dst is null,
 *               then the the original path is modified
 */
ECode CPath::Transform(
    /* [in] */ IMatrix* matrix,
    /* [in] */ IPath* dst)
{
    Int64 dstNative = 0;
    if (dst != NULL) {
        ((CPath*)dst)->mIsSimplePath = FALSE;
        dstNative = ((CPath*)dst)->mNativePath;
    }
    NativeTransform(mNativePath, ((Matrix*)matrix)->mNativeMatrix, dstNative);
    return NOERROR;
}

/**
 * Transform the points in this path by matrix.
 *
 * @param matrix The matrix to apply to the path
 */
ECode CPath::Transform(
    /* [in] */ IMatrix* matrix)
{
    mIsSimplePath = FALSE;
    NativeTransform(mNativePath, ((Matrix*)matrix)->mNativeMatrix);
    return NOERROR;
}

ECode CPath::Approximate(
    /* [in] */ Float acceptableError,
    /* [out] */ ArrayOf<Float>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr<ArrayOf<Float> > a = NativeApproximate(mNativePath, acceptableError);
    *array = a;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

void CPath::NativeReset(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->reset();
}

void CPath::NativeRewind(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->rewind();
}

void CPath::NativeSet(
    /* [in] */ Int64 dstHandle,
    /* [in] */ Int64 srcHandle)
{
    SkPath* dst = reinterpret_cast<SkPath*>(dstHandle);
    const SkPath* src = reinterpret_cast<SkPath*>(srcHandle);
    *dst = *src;
}

Boolean CPath::NativeIsConvex(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    return obj->isConvex();
}

Int32 CPath::NativeGetFillType(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    return obj->getFillType();
}

void CPath::NativeSetFillType(
    /* [in] */ Int64 pathHandle,
    /* [in] */ PathFillType ftHandle)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    SkPath::FillType ft = static_cast<SkPath::FillType>(ftHandle);
    path->setFillType(ft);
}

Boolean CPath::NativeIsEmpty(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    return obj->isEmpty();
}

Boolean CPath::NativeIsRect(
    /* [in] */ Int64 nPath,
    /* [in] */ IRectF* rect)
{
    SkRect rect_;
    Boolean result = ((SkPath*)nPath)->isRect(&rect_);
    GraphicsNative::SkRect2IRectF(rect_, rect);
    return result;
}

void CPath::NativeComputeBounds(
    /* [in] */ Int64 nPath,
    /* [in] */ IRectF* bounds)
{
    const SkRect& bounds_ = ((SkPath*)nPath)->getBounds();
    GraphicsNative::SkRect2IRectF(bounds_, bounds);
}

void CPath::NativeIncReserve(
    /* [in] */ Int64 nPath,
    /* [in] */ Int32 extraPtCount)
{
    ((SkPath*)nPath)->incReserve(extraPtCount);
}

void CPath::NativeMoveTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->moveTo(x, y);
}

void CPath::NativeRMoveTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->rMoveTo(dx, dy);
}

void CPath::NativeLineTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->lineTo(x, y);
}

void CPath::NativeRLineTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->rLineTo(dx, dy);
}

void CPath::NativeQuadTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->quadTo(x1, y1, x2, y2);
}

void CPath::NativeRQuadTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx1,
    /* [in] */ Float dy1,
    /* [in] */ Float dx2,
    /* [in] */ Float dy2)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->rQuadTo(dx1, dy1, dx2, dy2);
}

void CPath::NativeCubicTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float x3,
    /* [in] */ Float y3)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->cubicTo(x1, y1, x2, y2, x3, y3);
}

void CPath::NativeRCubicTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float x3,
    /* [in] */ Float y3)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->rCubicTo(x1, y1, x2, y2, x3, y3);
}

void CPath::NativeArcTo(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle,
    /* [in] */ Boolean forceMoveTo)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkRect oval = SkRect::MakeLTRB(left, top, right, bottom);
    obj->arcTo(oval, startAngle, sweepAngle, forceMoveTo);
}

void CPath::NativeClose(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->close();
}

void CPath::NativeAddRect(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ PathDirection dirHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath::Direction dir = static_cast<SkPath::Direction>(dirHandle);
    obj->addRect(left, top, right, bottom, dir);
}

void CPath::NativeAddOval(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 dirHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath::Direction dir = static_cast<SkPath::Direction>(dirHandle);
    SkRect oval = SkRect::MakeLTRB(left, top, right, bottom);
    obj->addOval(oval, dir);
}

void CPath::NativeAddCircle(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ PathDirection dirHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath::Direction dir = static_cast<SkPath::Direction>(dirHandle);
    obj->addCircle(x, y, radius, dir);
}

void CPath::NativeAddArc(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    SkRect oval = SkRect::MakeLTRB(left, top, right, bottom);
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->addArc(oval, startAngle, sweepAngle);
}

void CPath::NativeAddRoundRect(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float rx,
    /* [in] */ Float ry,
    /* [in] */ Int32 dirHandle)
{
    SkRect rect = SkRect::MakeLTRB(left, top, right, bottom);
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath::Direction dir = static_cast<SkPath::Direction>(dirHandle);
    obj->addRoundRect(rect, rx, ry, dir);
}

void CPath::NativeAddRoundRect(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ ArrayOf<Float>* radii,
    /* [in] */ Int32 dirHandle)
{
    SkRect rect = SkRect::MakeLTRB(left, top, right, bottom);
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath::Direction dir = static_cast<SkPath::Direction>(dirHandle);
    AutoFloatArray afa(radii, 8);
#ifdef SK_SCALAR_IS_FLOAT
    const float* src = afa.ptr();
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif
    obj->addRoundRect(rect, src, dir);
}

void CPath::NativeAddPath(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 srcHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath* src = reinterpret_cast<SkPath*>(srcHandle);
    obj->addPath(*src, dx, dy);
}

void CPath::NativeAddPath(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 srcHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath* src = reinterpret_cast<SkPath*>(srcHandle);
    obj->addPath(*src);
}

void CPath::NativeAddPath(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 srcHandle,
    /* [in] */ Int64 matrixHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath* src = reinterpret_cast<SkPath*>(srcHandle);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    obj->addPath(*src, *matrix);
}

void CPath::NativeOffset(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int64 dstHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkPath* dst = reinterpret_cast<SkPath*>(dstHandle);
    obj->offset(dx, dy, dst);
}

void CPath::NativeOffset(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->offset(dx, dy);
}

void CPath::NativeSetLastPoint(
    /* [in] */ Int64 objHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    obj->setLastPt(dx, dy);
}

void CPath::NativeTransform(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Int64 dstHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    SkPath* dst = reinterpret_cast<SkPath*>(dstHandle);
    obj->transform(*matrix, dst);
}

void CPath::NativeTransform(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 matrixHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    obj->transform(*matrix);
}

void CPath::NativeFinalizer(
    /* [in] */ Int64 objHandle)
{
    SkPath* obj = reinterpret_cast<SkPath*>(objHandle);
    // Purge entries from the HWUI path cache if this path's data is unique
    if (obj->unique() && android::uirenderer::Caches::hasInstance()) {
        android::uirenderer::Caches::getInstance().pathCache.removeDeferred(obj);
    }
    delete obj;
}

Boolean CPath::NativeOp(
    /* [in] */ Int64 p1Handle,
    /* [in] */ Int64 p2Handle,
    /* [in] */ Int32 opHandle,
    /* [in] */ Int64 rHandle)
{
    SkPath* p1  = reinterpret_cast<SkPath*>(p1Handle);
    SkPath* p2  = reinterpret_cast<SkPath*>(p2Handle);
    SkPathOp op = static_cast<SkPathOp>(opHandle);
    SkPath* r   = reinterpret_cast<SkPath*>(rHandle);
    return ::Op(*p1, *p2, op, r);
}

static void addMove(
    /* [in] */ std::vector<SkPoint>& segmentPoints,
    /* [in] */ std::vector<float>& lengths,
    /* [in] */ const SkPoint& point)
{
    float length = 0;
    if (!lengths.empty()) {
        length = lengths.back();
    }
    segmentPoints.push_back(point);
    lengths.push_back(length);
}

static void addLine(
    /* [in] */ std::vector<SkPoint>& segmentPoints,
    /* [in] */ std::vector<float>& lengths,
    /* [in] */ const SkPoint& toPoint)
{
    if (segmentPoints.empty()) {
        segmentPoints.push_back(SkPoint::Make(0, 0));
        lengths.push_back(0);
    } else if (segmentPoints.back() == toPoint) {
        return; // Empty line
    }
    float length = lengths.back() + SkPoint::Distance(segmentPoints.back(), toPoint);
    segmentPoints.push_back(toPoint);
    lengths.push_back(length);
}

static float cubicCoordinateCalculation(
    /* [in] */ float t,
    /* [in] */ float p0,
    /* [in] */ float p1,
    /* [in] */ float p2,
    /* [in] */ float p3)
{
    float oneMinusT = 1 - t;
    float oneMinusTSquared = oneMinusT * oneMinusT;
    float oneMinusTCubed = oneMinusTSquared * oneMinusT;
    float tSquared = t * t;
    float tCubed = tSquared * t;
    return (oneMinusTCubed * p0) + (3 * oneMinusTSquared * t * p1)
            + (3 * oneMinusT * tSquared * p2) + (tCubed * p3);
}

static SkPoint cubicBezierCalculation(
    /* [in] */ float t,
    /* [in] */ const SkPoint* points)
{
    float x = cubicCoordinateCalculation(t, points[0].x(), points[1].x(),
        points[2].x(), points[3].x());
    float y = cubicCoordinateCalculation(t, points[0].y(), points[1].y(),
        points[2].y(), points[3].y());
    return SkPoint::Make(x, y);
}

static float quadraticCoordinateCalculation(
    /* [in] */ float t,
    /* [in] */ float p0,
    /* [in] */ float p1,
    /* [in] */ float p2)
{
    float oneMinusT = 1 - t;
    return oneMinusT * ((oneMinusT * p0) + (t * p1)) + t * ((oneMinusT * p1) + (t * p2));
}

static SkPoint quadraticBezierCalculation(
    /* [in] */ float t,
    /* [in] */ const SkPoint* points)
{
    float x = quadraticCoordinateCalculation(t, points[0].x(), points[1].x(), points[2].x());
    float y = quadraticCoordinateCalculation(t, points[0].y(), points[1].y(), points[2].y());
    return SkPoint::Make(x, y);
}

typedef SkPoint (*bezierCalculation)(float t, const SkPoint* points);
// Subdivide a section of the Bezier curve, set the mid-point and the mid-t value.
// Returns true if further subdivision is necessary as defined by errorSquared.
static bool subdividePoints(
    /* [in] */ const SkPoint* points,
    /* [in] */ bezierCalculation bezierFunction,
    /* [in] */ float t0,
    /* [in] */ const SkPoint &p0,
    /* [in] */ float t1,
    /* [in] */ const SkPoint &p1,
    /* [in] */ float& midT,
    /* [in] */ SkPoint &midPoint,
    /* [in] */ float errorSquared)
{
    midT = (t1 + t0) / 2;
    float midX = (p1.x() + p0.x()) / 2;
    float midY = (p1.y() + p0.y()) / 2;

    midPoint = (*bezierFunction)(midT, points);
    float xError = midPoint.x() - midX;
    float yError = midPoint.y() - midY;
    float midErrorSquared = (xError * xError) + (yError * yError);
    return midErrorSquared > errorSquared;
}

// Divides Bezier curves until linear interpolation is very close to accurate, using
// errorSquared as a metric. Cubic Bezier curves can have an inflection point that improperly
// short-circuit subdivision. If you imagine an S shape, the top and bottom points being the
// starting and end points, linear interpolation would mark the center where the curve places
// the point. It is clearly not the case that we can linearly interpolate at that point.
// doubleCheckDivision forces a second examination between subdivisions to ensure that linear
// interpolation works.
static void addBezier(
    /* [in] */ const SkPoint* points,
    /* [in] */ bezierCalculation bezierFunction,
    /* [in] */ std::vector<SkPoint>& segmentPoints,
    /* [in] */ std::vector<float>& lengths,
    /* [in] */ float errorSquared,
    /* [in] */ bool doubleCheckDivision)
{
    typedef Map<float, SkPoint> PointMap;
    PointMap tToPoint;

    tToPoint[0] = (*bezierFunction)(0, points);
    tToPoint[1] = (*bezierFunction)(1, points);

    PointMap::Iterator iter = tToPoint.Begin();
    PointMap::Iterator next = iter;
    ++next;
    while (next != tToPoint.End()) {
        bool needsSubdivision = true;
        SkPoint midPoint;
        do {
            float midT;
            needsSubdivision = subdividePoints(points, bezierFunction, iter->mFirst,
                iter->mSecond, next->mFirst, next->mSecond, midT, midPoint, errorSquared);
            if (!needsSubdivision && doubleCheckDivision) {
                SkPoint quarterPoint;
                float quarterT;
                needsSubdivision = subdividePoints(points, bezierFunction, iter->mFirst,
                    iter->mSecond, midT, midPoint, quarterT, quarterPoint, errorSquared);
                if (needsSubdivision) {
                    // Found an inflection point. No need to double-check.
                    doubleCheckDivision = false;
                }
            }
            if (needsSubdivision) {
                next = tToPoint.Insert(iter, PointMap::ValueType(midT, midPoint));
            }
        } while (needsSubdivision);
        iter = next;
        next++;
    }

    // Now that each division can use linear interpolation with less than the allowed error
    for (iter = tToPoint.Begin(); iter != tToPoint.End(); ++iter) {
        addLine(segmentPoints, lengths, iter->mSecond);
    }
}

static void createVerbSegments(
    /* [in] */ SkPath::Verb verb,
    /* [in] */ const SkPoint* points,
    /* [in] */ std::vector<SkPoint>& segmentPoints,
    /* [in] */ std::vector<float>& lengths,
    /* [in] */ float errorSquared)
{
    switch (verb) {
        case SkPath::kMove_Verb:
            addMove(segmentPoints, lengths, points[0]);
            break;
        case SkPath::kClose_Verb:
            addLine(segmentPoints, lengths, points[0]);
            break;
        case SkPath::kLine_Verb:
            addLine(segmentPoints, lengths, points[1]);
            break;
        case SkPath::kQuad_Verb:
            addBezier(points, quadraticBezierCalculation, segmentPoints, lengths,
                errorSquared, false);
            break;
        case SkPath::kCubic_Verb:
            addBezier(points, cubicBezierCalculation, segmentPoints, lengths,
                errorSquared, true);
            break;
        default:
            // Leave element as NULL, Conic sections are not supported.
            break;
    }
}

AutoPtr<ArrayOf<Float> > CPath::NativeApproximate(
    /* [in] */ Int64 pathHandle,
    /* [in] */ Float acceptableError)
{
    SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    SkASSERT(path);
    SkPath::Iter pathIter(*path, false);
    SkPath::Verb verb;
    SkPoint points[4];
    std::vector<SkPoint> segmentPoints;
    std::vector<float> lengths;
    float errorSquared = acceptableError * acceptableError;

    while ((verb = pathIter.next(points, false)) != SkPath::kDone_Verb) {
        createVerbSegments(verb, points, segmentPoints, lengths, errorSquared);
    }

    if (segmentPoints.empty()) {
        int numVerbs = path->countVerbs();
        if (numVerbs == 1) {
            addMove(segmentPoints, lengths, path->getPoint(0));
        } else {
            // Invalid or empty path. Fall back to point(0,0)
            addMove(segmentPoints, lengths, SkPoint());
        }
    }

    float totalLength = lengths.back();
    if (totalLength == 0) {
        // Lone Move instructions should still be able to animate at the same value.
        segmentPoints.push_back(segmentPoints.back());
        lengths.push_back(1);
        totalLength = 1;
    }

    size_t numPoints = segmentPoints.size();
    size_t approximationArraySize = numPoints * 3;

    float* approximation = new float[approximationArraySize];

    int approximationIndex = 0;
    for (size_t i = 0; i < numPoints; i++) {
        const SkPoint& point = segmentPoints[i];
        approximation[approximationIndex++] = lengths[i] / totalLength;
        approximation[approximationIndex++] = point.x();
        approximation[approximationIndex++] = point.y();
    }

    AutoPtr<ArrayOf<Float> > result = ArrayOf<Float>::Alloc(approximationArraySize);
    result->Copy(approximation, approximationArraySize);
    delete[] approximation;
    return result;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
