//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __ELASTOS_GRAFIX_RECTANGLE_H__
#define __ELASTOS_GRAFIX_RECTANGLE_H__

#define _NO_INCLIST
#include <eltypes.h>
#include <elasys_server.h>
#undef _NO_INCLIST

_ELASTOS_NAMESPACE_USING

#define RECTANGLE_VALS(r)                   (r)->x, (r)->y,             \
                                            (r)->width, (r)->height

#define RECTANGLE_VALS_FROM_REGION(r)       (r)->left, (r)->top,        \
                                            (r)->right - (r)->left,     \
                                            (r)->bottom - (r)->top

#define RECTANGLE_INIT_FROM_REGION(r)       {RECTANGLE_VALS_FROM_REGION(r)}

#define RECTANGLE_VALS_TRANSLATED(r,dx,dy)  (r)->x + dx, (r)->y + dy, \
                                            (r)->width, (r)->height

#define RECTANGLE_INIT_TRANSLATED(r,x,y)    {RECTANGLE_VALS_TRANSLATED(r,x,y)}

#define RECTANGLE_RIGHT(r)                  ((r)->x + (r)->width)
#define RECTANGLE_BOTTOM(r)                 ((r)->y + (r)->height)

#define E_RECTANGLE_NOT_INTERSECT   E_FAIL

CAR_INLINE Boolean RectangleValid(
    /* [in] */ Rectangle* rectangle)
{
    return (rectangle->width >= 0) && (rectangle->height >= 0);
}

CAR_INLINE Boolean RectangleIsEmpty(
    /* [in] */ Rectangle* rectangle)
{
    return (0 == rectangle->width) || (0 == rectangle->height);
}

CAR_INLINE Void RectangleTranslate(
    /* [in, out] */ Rectangle* rectangle,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    rectangle->x += dx;
    rectangle->y += dy;
}

CAR_INLINE Boolean RectangleEqual(
    /* [in] */ Rectangle* rectangle1,
    /* [in] */ Rectangle* rectangle2)
{
    assert(RectangleValid(rectangle1));
    assert(RectangleValid(rectangle2));

    return (rectangle1->x == rectangle2->x
        && rectangle1->y == rectangle2->y
        && rectangle1->width == rectangle2->width
        && rectangle1->height == rectangle2->height);
}

CAR_INLINE Boolean PointInRectangle(
    /* [in] */ Rectangle* rectangle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    assert(RectangleValid(rectangle));

    return (x >= rectangle->x && x < rectangle->x + rectangle->width
        && y >= rectangle->y && y < rectangle->y + rectangle->height);
}

CAR_INLINE Boolean RectangleInRectangle(
    /* [in] */ Rectangle * rectangle1,
    /* [in] */ Rectangle * rectangle2)
{
    assert(RectangleValid(rectangle1));
    assert(RectangleValid(rectangle2));

    return (rectangle1->x >= rectangle2->x
        && rectangle1->y >= rectangle2->y
        && (rectangle1->x + rectangle1->width)
            <= (rectangle2->x + rectangle2->width)
        && (rectangle1->y + rectangle1->height)
            <= (rectangle2->y + rectangle2->height));
}

CAR_INLINE Boolean RectangleIntersects(
    /* [in] */ Rectangle* rectangle,
    /* [in] */ Rectangle* clip)
{
    Region Region1 = REGION_INIT_FROM_RECTANGLE(rectangle);
    Region Region2 = REGION_INIT_FROM_RECTANGLE(clip);

    return RegionIntersects(&Region1, &Region2);
}

CAR_INLINE Void RectangleFromRegion(
    /* [out] */ Rectangle* rectangle,
    /* [in] */ Region* region)
{
    rectangle->x = region->left;
    rectangle->y = region->top;
    rectangle->width = region->right - region->left;
    rectangle->height = region->bottom - region->top;
}

CAR_INLINE Boolean RectangleRegionIntersect(
    /* [in, out] */ Rectangle* rectangle,
    /* [in] */ Region* region)
{
    // adjust position
    //
    if (region->left > rectangle->x) {
        rectangle->width -= region->left - rectangle->x;
        rectangle->x = region->left;
    }

    if (region->top > rectangle->y) {
        rectangle->height -= region->top - rectangle->y;
        rectangle->y = region->top;
    }

    // adjust size
    //
    if (region->right < rectangle->x + rectangle->width)
        rectangle->width = region->right - rectangle->x;

    if (region->bottom < rectangle->y + rectangle->height)
        rectangle->height = region->bottom - rectangle->y;

    // set size to zero if there's no intersection
    //
    if (rectangle->width <= 0 || rectangle->height <= 0) {
        rectangle->width = 0;
        rectangle->height = 0;

        return FALSE;
    }

    return TRUE;
}

CAR_INLINE Boolean RectanglePrecisionUnion(
    /* [in] */ Rectangle* rect1,
    /* [in] */ Rectangle* rect2,
    /* [out] */ Rectangle* rectOut)
{
    assert(rectOut);

    Region region1 = REGION_INIT_FROM_RECTANGLE(rect1);
    Region region2 = REGION_INIT_FROM_RECTANGLE(rect2);
    Region regionOut;

    if (RegionPrecisionUnion(&region1, &region2, &regionOut)) {
        RectangleFromRegion(rectOut, &regionOut);

        return TRUE;
    }

    return FALSE;
}

CAR_INLINE Void RectangleUnion(
    /* [in, out] */ Rectangle* rect1,
    /* [in] */ Rectangle* rect2)
{
    assert(RectangleValid(rect1));
    assert(RectangleValid(rect2));

    if (RectangleIsEmpty(rect2))
        return;

    if (rect1->width) {
        int temp = MIN(rect1->x, rect2->x);
        rect1->width = MAX(rect1->x + rect1->width, rect2->x + rect2->width)
                        - temp;
        rect1->x = temp;
    }
    else {
        rect1->x = rect2->x;
        rect1->width = rect2->width;
    }

    if (rect1->height) {
        int temp = MIN(rect1->y, rect2->y);
        rect1->height = MAX(rect1->y + rect1->height, rect2->y + rect2->height)
                        - temp;
        rect1->y = temp;
    }
    else {
        rect1->y = rect2->y;
        rect1->height = rect2->height;
    }
}

CAR_INLINE Boolean RectangleIntersect(
    /* [in] */ Rectangle* rectangle,
    /* [in] */ Rectangle* clip)
{
    Region region = REGION_INIT_FROM_RECTANGLE(clip);

    return RectangleRegionIntersect(rectangle, &region);
}

CAR_INLINE Int32 RectangleSubRectangle(
    /* [in] */ Rectangle rectangle1,
    /* [in] */ Rectangle rectangle2,
    /* [out] */ Rectangle* rectangleRemain)
{
    assert(RectangleValid(&rectangle1));
    assert(RectangleValid(&rectangle2));
    assert(rectangleRemain);

    Region region1 = REGION_INIT_FROM_RECTANGLE(&rectangle1);
    Region region2 = REGION_INIT_FROM_RECTANGLE(&rectangle2);
    Region regionRemain[4];
    Int32 count;

    count = RegionSubRegion(region1, region2, regionRemain);
    assert(count <= 4);
    for (int i = 0; i < count; i++) {
        RectangleFromRegion(&rectangleRemain[i], &regionRemain[i]);
    }

    return count;
}

#endif  // __ELASTOS_GRAFIX_RECTANGLE_H__
