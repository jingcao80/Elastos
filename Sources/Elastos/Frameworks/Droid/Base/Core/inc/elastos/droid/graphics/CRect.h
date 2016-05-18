
#ifndef __ELASTOS_DROID_GRAPHICS_CRECT_H__
#define __ELASTOS_DROID_GRAPHICS_CRECT_H__

#include "_Elastos_Droid_Graphics_CRect.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Rect holds four integer coordinates for a rectangle. The rectangle is
 * represented by the coordinates of its 4 edges (left, top, right bottom).
 * These fields can be accessed directly. Use width() and height() to retrieve
 * the rectangle's width and height. Note: most methods do not check to see that
 * the coordinates are sorted correctly (i.e. left <= right and top <= bottom).
 */
CarClass(CRect)
    , public Object
    , public IRect
    , public IParcelable
{
private:
    /**
     * A helper class for flattened rectange pattern recognition. A separate
     * class to avoid an initialization dependency on a regular expression
     * causing Rect to not be initializable with an ahead-of-time compilation
     * scheme.
     */
    class UnflattenHelper
    {
    public:
        static CARAPI_(AutoPtr<IMatcher>) GetMatcher(
            /* [in] */ const String& str);

    private:
        static CARAPI_(AutoPtr<IPattern>) InitStatic();

    private:
        static AutoPtr<IPattern> FLATTENED_PATTERN;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Create a new empty Rect. All coordinates are initialized to 0.
     */
    CARAPI constructor();

    /**
     * Create a new rectangle with the specified coordinates. Note: no range
     * checking is performed, so the caller must ensure that left <= right and
     * top <= bottom.
     *
     * @param left   The X coordinate of the left side of the rectangle
     * @param top    The Y coordinate of the top of the rectangle
     * @param right  The X coordinate of the right side of the rectangle
     * @param bottom The Y coordinate of the bottom of the rectangle
     */
    CARAPI constructor(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Create a new rectangle, initialized with the values in the specified
     * rectangle (which is left unmodified).
     *
     * @param r The rectangle whose coordinates are copied into the new
     *          rectangle.
     */
    CARAPI constructor(
        /* [in] */ IRect* r);

    CARAPI Equals(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* r,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Return a string representation of the rectangle in a compact form.
     */
    CARAPI ToShortString(
        /* [out] */ String* str);

    /**
     * Return a string representation of the rectangle in a well-defined format.
     *
     * <p>You can later recover the Rect from this string through
     * {@link #unflattenFromString(String)}.
     *
     * @return Returns a new String of the form "left top right bottom"
     */
    CARAPI FlattenToString(
        /* [out] */ String* str);

    /**
     * Returns a Rect from a string of the form returned by {@link #flattenToString},
     * or null if the string is not of that form.
     */
    static CARAPI_(AutoPtr<IRect>) UnflattenFromString(
        /* [in] */ const String& str);

    /**
     * Returns true if the rectangle is empty (left >= right or top >= bottom)
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * @return the rectangle's width. This does not check for a valid rectangle
     * (i.e. left <= right) so the result may be negative.
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * @return the rectangle's height. This does not check for a valid rectangle
     * (i.e. top <= bottom) so the result may be negative.
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * @return the horizontal center of the rectangle. If the computed value
     *         is fractional, this method returns the largest integer that is
     *         less than the computed value.
     */
    CARAPI GetCenterX(
        /* [out] */ Int32* x);

    /**
     * @return the vertical center of the rectangle. If the computed value
     *         is fractional, this method returns the largest integer that is
     *         less than the computed value.
     */
    CARAPI GetCenterY(
        /* [out] */ Int32* y);

    /**
     * @return the exact horizontal center of the rectangle as a float.
     */
    CARAPI GetExactCenterX(
        /* [out] */ Float* x);

    /**
     * @return the exact vertical center of the rectangle as a float.
     */
    CARAPI GetExactCenterY(
        /* [out] */ Float* y);

    /**
     * Set the rectangle to (0,0,0,0)
     */
    CARAPI SetEmpty();

    /**
     * Set the rectangle's coordinates to the specified values. Note: no range
     * checking is performed, so it is up to the caller to ensure that
     * left <= right and top <= bottom.
     *
     * @param left   The X coordinate of the left side of the rectangle
     * @param top    The Y coordinate of the top of the rectangle
     * @param right  The X coordinate of the right side of the rectangle
     * @param bottom The Y coordinate of the bottom of the rectangle
     */
    CARAPI Set(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI Get(
        /* [out] */ Int32* left,
        /* [out] */ Int32* top,
        /* [out] */ Int32* right,
        /* [out] */ Int32* bottom);

    /**
     * Copy the coordinates from src into this rectangle.
     *
     * @param src The rectangle whose coordinates are copied into this
     *           rectangle.
     */
    CARAPI Set(
        /* [in] */ IRect* src);

    /**
     * Offset the rectangle by adding dx to its left and right coordinates, and
     * adding dy to its top and bottom coordinates.
     *
     * @param dx The amount to add to the rectangle's left and right coordinates
     * @param dy The amount to add to the rectangle's top and bottom coordinates
     */
    CARAPI Offset(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Offset the rectangle to a specific (left, top) position,
     * keeping its width and height the same.
     *
     * @param newLeft   The new "left" coordinate for the rectangle
     * @param newTop    The new "top" coordinate for the rectangle
     */
    CARAPI OffsetTo(
        /* [in] */ Int32 newLeft,
        /* [in] */ Int32 newTop);

    /**
     * Inset the rectangle by (dx,dy). If dx is positive, then the sides are
     * moved inwards, making the rectangle narrower. If dx is negative, then the
     * sides are moved outwards, making the rectangle wider. The same holds true
     * for dy and the top and bottom.
     *
     * @param dx The amount to add(subtract) from the rectangle's left(right)
     * @param dy The amount to add(subtract) from the rectangle's top(bottom)
     */
    CARAPI Inset(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Returns true if (x,y) is inside the rectangle. The left and top are
     * considered to be inside, while the right and bottom are not. This means
     * that for a x,y to be contained: left <= x < right and top <= y < bottom.
     * An empty rectangle never contains any point.
     *
     * @param x The X coordinate of the point being tested for containment
     * @param y The Y coordinate of the point being tested for containment
     * @return true iff (x,y) are contained by the rectangle, where containment
     *              means left <= x < right and top <= y < bottom
     */
    CARAPI Contains(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    /**
     * Returns true iff the 4 specified sides of a rectangle are inside or equal
     * to this rectangle. i.e. is this rectangle a superset of the specified
     * rectangle. An empty rectangle never contains another rectangle.
     *
     * @param left The left side of the rectangle being tested for containment
     * @param top The top of the rectangle being tested for containment
     * @param right The right side of the rectangle being tested for containment
     * @param bottom The bottom of the rectangle being tested for containment
     * @return true iff the the 4 specified sides of a rectangle are inside or
     *              equal to this rectangle
     */
    CARAPI Contains(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Returns true iff the specified rectangle r is inside or equal to this
     * rectangle. An empty rectangle never contains another rectangle.
     *
     * @param r The rectangle being tested for containment.
     * @return true iff the specified rectangle r is inside or equal to this
     *              rectangle
     */
    CARAPI Contains(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * If the rectangle specified by left,top,right,bottom intersects this
     * rectangle, return true and set this rectangle to that intersection,
     * otherwise return false and do not change this rectangle. No check is
     * performed to see if either rectangle is empty. Note: To just test for
     * intersection, use {@link #intersects(Rect, Rect)}.
     *
     * @param left The left side of the rectangle being intersected with this
     *             rectangle
     * @param top The top of the rectangle being intersected with this rectangle
     * @param right The right side of the rectangle being intersected with this
     *              rectangle.
     * @param bottom The bottom of the rectangle being intersected with this
     *             rectangle.
     * @return true if the specified rectangle and this rectangle intersect
     *              (and this rectangle is then set to that intersection) else
     *              return false and do not change this rectangle.
     */
    CARAPI Intersect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * If the specified rectangle intersects this rectangle, return true and set
     * this rectangle to that intersection, otherwise return false and do not
     * change this rectangle. No check is performed to see if either rectangle
     * is empty. To just test for intersection, use intersects()
     *
     * @param r The rectangle being intersected with this rectangle.
     * @return true if the specified rectangle and this rectangle intersect
     *              (and this rectangle is then set to that intersection) else
     *              return false and do not change this rectangle.
     */
    CARAPI Intersect(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

    /**
     * Returns true if this rectangle intersects the specified rectangle.
     * In no event is this rectangle modified. No check is performed to see
     * if either rectangle is empty. To record the intersection, use intersect()
     * or setIntersect().
     *
     * @param left The left side of the rectangle being tested for intersection
     * @param top The top of the rectangle being tested for intersection
     * @param right The right side of the rectangle being tested for
     *              intersection
     * @param bottom The bottom of the rectangle being tested for intersection
     * @return true iff the specified rectangle intersects this rectangle. In
     *              no event is this rectangle modified.
     */
    CARAPI Intersects(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Returns true iff the two specified rectangles intersect. In no event are
     * either of the rectangles modified. To record the intersection,
     * use {@link #intersect(Rect)} or {@link #setIntersect(Rect, Rect)}.
     *
     * @param a The first rectangle being tested for intersection
     * @param b The second rectangle being tested for intersection
     * @return true iff the two specified rectangles intersect. In no event are
     *              either of the rectangles modified.
     */
    static CARAPI_(Boolean) Intersects(
        /* [in] */ IRect* a,
        /* [in] */ IRect* b);

    /**
     * If rectangles a and b intersect, return true and set this rectangle to
     * that intersection, otherwise return false and do not change this
     * rectangle. No check is performed to see if either rectangle is empty.
     * To just test for intersection, use intersects()
     *
     * @param a The first rectangle being intersected with
     * @param b The second rectangle being intersected with
     * @return true iff the two specified rectangles intersect. If they do, set
     *              this rectangle to that intersection. If they do not, return
     *              false and do not change this rectangle.
     */
    CARAPI SetIntersect(
        /* [in] */ IRect* a,
        /* [in] */ IRect* b,
        /* [out] */ Boolean* result);

    /**
     * Update this Rect to enclose itself and the specified rectangle. If the
     * specified rectangle is empty, nothing is done. If this rectangle is empty
     * it is set to the specified rectangle.
     *
     * @param left The left edge being unioned with this rectangle
     * @param top The top edge being unioned with this rectangle
     * @param right The right edge being unioned with this rectangle
     * @param bottom The bottom edge being unioned with this rectangle
     */
    CARAPI Union(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Update this Rect to enclose itself and the specified rectangle. If the
     * specified rectangle is empty, nothing is done. If this rectangle is empty
     * it is set to the specified rectangle.
     *
     * @param r The rectangle being unioned with this rectangle
     */
    CARAPI Union(
        /* [in] */ IRect* r);

    /**
     * Update this Rect to enclose itself and the [x,y] coordinate. There is no
     * check to see that this rectangle is non-empty.
     *
     * @param x The x coordinate of the point to add to the rectangle
     * @param y The y coordinate of the point to add to the rectangle
     */
    CARAPI Union(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Swap top/bottom or left/right if there are flipped (i.e. left > right
     * and/or top > bottom). This can be called if
     * the edges are computed separately, and may have crossed over each other.
     * If the edges are already correct (i.e. left <= right and top <= bottom)
     * then nothing is done.
     */
    CARAPI Sort();

    /**
     * Scales up the rect by the given scale.
     * @hide
     */
    CARAPI Scale(
        /* [in] */ Float scale);

    /**
     * Write this rectangle to the specified parcel. To restore a rectangle from
     * a parcel, use readFromParcel()
     * @param out The parcel to write the rectangle's coordinates into
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Set the rectangle's coordinates from the data stored in the specified
     * parcel. To write a rectangle to a parcel, call writeToParcel().
     *
     * @param in The parcel to read the rectangle's coordinates from
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetTop(
        /* [out] */ Int32* top);

    CARAPI SetTop(
        /* [in] */ Int32 top);

    CARAPI GetBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetLeft(
        /* [out] */ Int32* left);

    CARAPI SetLeft(
        /* [in] */ Int32 left);

    CARAPI GetRight(
        /* [out] */ Int32* right);

    CARAPI SetRight(
        /* [in] */ Int32 right);

    /**
     * Scales up the rect by the given scale, rounding values toward the inside.
     * @hide
     */
    CARAPI_(void) ScaleRoundIn(
        /* [in] */ Float scale);

public:
    Int32 mLeft;
    Int32 mTop;
    Int32 mRight;
    Int32 mBottom;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CRECT_H__
