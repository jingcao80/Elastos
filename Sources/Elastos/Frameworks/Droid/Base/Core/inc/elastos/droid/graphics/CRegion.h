
#ifndef __ELASTOS_DROID_GRAPHICS_CREGION_H__
#define __ELASTOS_DROID_GRAPHICS_CREGION_H__

#include "_Elastos_Droid_Graphics_CRegion.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CRegion)
    , public Object
    , public IRegion
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    ~CRegion();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IRegion* region);

    CARAPI constructor(
        /* [in] */ IRect* r);

    CARAPI constructor(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI constructor(
        /* [in] */ Handle64 ni);

    CARAPI constructor(
        /* [in] */ Handle64 ni,
        /* [in] */ Int32 dummy);

    /** Set the region to the empty region
     */
    CARAPI SetEmpty();

    /** Set the region to the specified region.
     */
    CARAPI Set(
        /* [in] */ IRegion* src,
        /* [out] */ Boolean* result);

    /** Set the region to the specified rectangle
     */
    CARAPI Set(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /** Set the region to the specified rectangle
     */
    CARAPI Set(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Set the region to the area described by the path and clip.
     * Return true if the resulting region is non-empty. This produces a region
     * that is identical to the pixels that would be drawn by the path
     * (with no antialiasing).
     */
    CARAPI SetPath(
        /* [in] */ IPath* path,
        /* [in] */ IRegion* clip,
        /* [out] */ Boolean* result);

    /**
     * Return true if this region is empty
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* isEmpty);

    /**
     * Return true if the region contains a single rectangle
     */
    CARAPI IsRect(
        /* [out] */ Boolean* isRect);

    /**
     * Return true if the region contains more than one rectangle
     */
    CARAPI IsComplex(
        /* [out] */ Boolean* isComplex);

    /**
     * Return a new Rect set to the bounds of the region. If the region is
     * empty, the Rect will be set to [0, 0, 0, 0]
     */
    CARAPI GetBounds(
        /* [out] */ IRect** bounds);

    /**
     * Set the Rect to the bounds of the region. If the region is empty, the
     * Rect will be set to [0, 0, 0, 0]
     */
    CARAPI GetBounds(
        /* [in, out] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * Return the boundary of the region as a new Path. If the region is empty,
     * the path will also be empty.
     */
    CARAPI GetBoundaryPath(
        /* [out] */ IPath** path);

    /**
     * Set the path to the boundary of the region. If the region is empty, the
     * path will also be empty.
     */
    CARAPI GetBoundaryPath(
        /* [in, out] */ IPath* path,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region contains the specified point
     */
    CARAPI Contains(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region is a single rectangle (not complex) and it
     * contains the specified rectangle. Returning false is not a guarantee
     * that the rectangle is not contained by this region, but return true is a
     * guarantee that the rectangle is contained by this region.
     */
    CARAPI QuickContains(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region is a single rectangle (not complex) and it
     * contains the specified rectangle. Returning false is not a guarantee
     * that the rectangle is not contained by this region, but return true is a
     * guarantee that the rectangle is contained by this region.
     */
    CARAPI QuickContains(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region is empty, or if the specified rectangle does
     * not intersect the region. Returning false is not a guarantee that they
     * intersect, but returning true is a guarantee that they do not.
     */
    CARAPI QuickReject(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region is empty, or if the specified rectangle does
     * not intersect the region. Returning false is not a guarantee that they
     * intersect, but returning true is a guarantee that they do not.
     */
    CARAPI QuickReject(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Return true if the region is empty, or if the specified region does not
     * intersect the region. Returning false is not a guarantee that they
     * intersect, but returning true is a guarantee that they do not.
     */
    CARAPI QuickReject(
        /* [in] */ IRegion* rgn,
        /* [out] */ Boolean* result);

    /**
     * Translate the region by [dx, dy]. If the region is empty, do nothing.
     */
    CARAPI Translate(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Set the dst region to the result of translating this region by [dx, dy].
     * If this region is empty, then dst will be set to empty.
     */
    CARAPI Translate(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ IRegion* dst);

    /**
     * Scale the region by the given scale amount. This re-constructs new region by
     * scaling the rects that this region consists of. New rectis are computed by scaling
     * coordinates by float, then rounded by roundf() function to integers. This may results
     * in less internal rects if 0 < scale < 1. Zero and Negative scale result in
     * an empty region. If this region is empty, do nothing.
     *
     * @hide
     */
    CARAPI Scale(
        /* [in] */ Float scale);

    /**
     * Set the dst region to the result of scaling this region by the given scale amount.
     * If this region is empty, then dst will be set to empty.
     * @hide
     */
    CARAPI Scale(
        /* [in] */ Float scale,
        /* [in] */ IRegion* dst);

    CARAPI Union(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * Perform the specified Op on this region and the specified rect. Return
     * true if the result of the op is not empty.
     */
    CARAPI Op(
        /* [in] */ IRect* r,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* result);

    /**
     * Perform the specified Op on this region and the specified rect. Return
     * true if the result of the op is not empty.
     */
    CARAPI Op(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* result);

    /**
     * Perform the specified Op on this region and the specified region. Return
     * true if the result of the op is not empty.
     */
    CARAPI Op(
        /* [in] */ IRegion* region,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* result);

    /**
     * Set this region to the result of performing the Op on the specified rect
     * and region. Return true if the result is not empty.
     */
    CARAPI Op(
        /* [in] */ IRect* rect,
        /* [in] */ IRegion* region,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* result);

    /**
     * Set this region to the result of performing the Op on the specified
     * regions. Return true if the result is not empty.
     */
    CARAPI Op(
        /* [in] */ IRegion* region1,
        /* [in] */ IRegion* region2,
        /* [in] */ RegionOp op,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* region,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * @return An instance from a pool if such or a new one.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<IRegion>) Obtain();

    /**
     * @return An instance from a pool if such or a new one.
     *
     * @param other Region to copy values from for initialization.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<IRegion>) Obtain(
        /* [in] */ IRegion* other);

    /**
     * Recycles an instance.
     *
     * @hide
     */
    CARAPI Recycle();

    CARAPI GetNativeRegion(
        /* [out] */ Handle64* region);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static CARAPI_(Boolean) NativeEquals(
        /* [in] */ Handle64 region1,
        /* [in] */ Handle64 region2);

    static CARAPI_(Handle64) NativeConstructor();

    static CARAPI_(void) NativeDestructor(
        /* [in] */ Handle64 region);

    static CARAPI_(void) NativeSetRegion(
        /* [in] */ Handle64 dst,
        /* [in] */ Handle64 src);

    static CARAPI_(Boolean) NativeSetRect(
        /* [in] */ Handle64 dst,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    static CARAPI_(Boolean) NativeSetPath(
        /* [in] */ Handle64 dst,
        /* [in] */ Handle64 path,
        /* [in] */ Handle64 clip);

    static CARAPI_(Boolean) NativeGetBounds(
        /* [in] */ Handle64 region,
        /* [out] */ IRect* rect);

    static CARAPI_(Boolean) NativeGetBoundaryPath(
        /* [in] */ Handle64 region,
        /* [out] */ Handle64 path);

    static CARAPI_(Boolean) NativeOp(
        /* [in] */ Handle64 dst,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ RegionOp op);

    static CARAPI_(Boolean) NativeOp(
        /* [in] */ Handle64 dst,
        /* [in] */ IRect* rect,
        /* [in] */ Handle64 region,
        /* [in] */ RegionOp op);

    static CARAPI_(Boolean) NativeOp(
        /* [in] */ Handle64 dst,
        /* [in] */ Handle64 region1,
        /* [in] */ Handle64 region2,
        /* [in] */ RegionOp op);

    static CARAPI_(Handle64) NativeCreateFromParcel(
        /* [in] */ IParcel* parcel);

    static CARAPI_(Boolean) NativeWriteToParcel(
        /* [in] */ Handle64 region,
        /* [out] */ IParcel* parcel);

    static CARAPI_(String) NativeToString(
        /* [in] */ Handle64 region);

public:
    Handle64 mNativeRegion;

private:
    static const Int32 MAX_POOL_SIZE;

    static AutoPtr<Pools::SynchronizedPool<IRegion> > sPool;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CREGION_H__
