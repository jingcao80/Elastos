//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __ELASTOS_GRAFIX_REGION_H__
#define __ELASTOS_GRAFIX_REGION_H__

#define _NO_INCLIST
#include <eltypes.h>
#include <elasys_server.h>
#undef _NO_INCLIST

_ELASTOS_NAMESPACE_USING

#include <linknode.h>

#define REGION_VALS(r)                  (r)->left, (r)->top,            \
                                        (r)->right, (r)->bottom

#define REGION_VALS_FROM_RECTANGLE(r)   (r)->x, (r)->y,                 \
                                        (r)->x + (r)->width,            \
                                        (r)->y + (r)->height

#define REGION_INIT_FROM_RECTANGLE(r)   {REGION_VALS_FROM_RECTANGLE(r)}

#define REGION_VALS_FROM_RECTANGLE_VALS(x,y,w,h)    (x), (y),           \
                                                    (x) + (w), (y) + (h)

#define REGION_INIT_FROM_RECTANGLE_VALS(x,y,w,h)                        \
                                    {REGION_VALS_FROM_RECTANGLE_VALS(x,y,w,h)}

#define REGION_VALS_TRANSLATED(r,x,y)   (r)->left + (x), (r)->top + (y),   \
                                        (r)->right + (x), (r)->bottom + (y)

#define REGION_INIT_TRANSLATED(r,x,y)   {REGION_VALS_TRANSLATED(r,x,y)}

#define REGION_WIDTH(r)                 ((r)->right - (r)->left)
#define REGION_HEIGHT(r)                ((r)->bottom - (r)->top)

#define E_REGION_TOO_SMALL              E_FAIL
#define E_REGION_TRANSPARENTED          E_FAIL
#define E_REGION_NOT_REGULAR            E_FAIL

class RegionLink : public DLinkNode {
public:
    RegionLink () {
        Initialize();
        m_free = TRUE;
    }

public:
    Region m_region;
    Boolean m_free;
};

CAR_INLINE Boolean RegionRegular(
    /* [in] */ const Region* region)
{
    assert(region);

    return (region->left <= region->right) && (region->top <= region->bottom);
}

CAR_INLINE Boolean RegionEqual(
    /* [in] */ const Region* region1,
    /* [in] */ const Region* region2)
{
    return (region1->left == region2->left
        && region1->top == region2->top
        && region1->right == region2->right
        && region1->bottom == region2->bottom);
}

CAR_INLINE Boolean RegionIntersects(
    /* [in] */ const Region* region1,
    /* [in] */ const Region* region2)
{
    return (region1->left < region2->right
        && region1->top < region2->bottom
        && region1->right > region2->left
        && region1->bottom > region2->top);
}

CAR_INLINE Void RegionFromRectangle(
    /* [out] */ Region* region,
    /* [in] */ const Rectangle* rectangle)
{
    region->left = rectangle->x;
    region->top = rectangle->y;
    region->right = rectangle->x + rectangle->width;
    region->bottom = rectangle->y + rectangle->height;
}

CAR_INLINE Boolean PointInRegion(
    /* [in] */ const Region* region,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    assert(RegionRegular(region));

    return (x >= region->left && x < region->right
        && y >= region->top && y < region->bottom);
}

CAR_INLINE Void RegionTranslate(
    /* [in, out] */ Region* region,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    region->left += dx;
    region->top += dy;
    region->right += dx;
    region->bottom += dy;
}

CAR_INLINE Boolean RegionInRegion(
    /* [in] */ const Region* region1,
    /* [in] */ const Region* region2)
{
    assert(RegionRegular(region1));
    assert(RegionRegular(region2));

    return (region1->left >= region2->left
            && region1->top >= region2->top
            && region1->right <= region2->right
            && region1->bottom <= region2->bottom);
}

CAR_INLINE Boolean RegionInRectangle(
    /* [in] */ const Region* region,
    /* [in] */ const Rectangle* rectangle)
{
    assert(RegionRegular(region));

    return (region->left >= rectangle->x
        && region->top >= rectangle->y
        && region->right <= (rectangle->x + rectangle->width)
        && region->bottom <= (rectangle->y + rectangle->height));
}

CAR_INLINE Boolean RegionPrecisionUnion(
    /* [in] */ Region* region1,
    /* [in] */ Region* region2,
    /* [out] */ Region* regionOut)
{
    assert(region1 && region2 && regionOut);
    assert(RegionRegular(region1));
    assert(RegionRegular(region2));

    if (RegionInRegion(region1, region2)) {
        regionOut->left = region1->left > region2->left
                            ? region2->left : region1->left;
        regionOut->top = region1->top > region2->top
                            ? region2->top : region1->top;
        regionOut->right = region1->right > region2->right
                            ? region1->right : region2->right;
        regionOut->bottom = region1->bottom > region2->bottom
                            ? region1->bottom : region2->bottom;

        return TRUE;
    }
    else if (region1->left == region2->left
            && region1->right == region2->right) {
        if (region1->top >= region2->top
                && region1->top <= region2->bottom) {
            regionOut->left = region2->left;
            regionOut->top = region2->top;
            regionOut->right = region1->right;
            regionOut->bottom = region1->bottom;

            return TRUE;
        }
        else if (region1->bottom >= region2->top
                && region1->bottom <= region2->bottom) {
            regionOut->left = region1->left;
            regionOut->top = region1->top;
            regionOut->right = region2->right;
            regionOut->bottom = region2->bottom;

            return TRUE;
        }
    }
    else if (region1->top == region2->top
            && region1->bottom == region2->bottom) {
        if (region1->left >= region2->left
                && region1->left <= region2->right) {
            regionOut->left = region2->left;
            regionOut->top = region2->top;
            regionOut->right = region1->right;
            regionOut->bottom = region1->bottom;

            return TRUE;
        }
        else if (region1->right >= region2->left
                && region1->right <= region2->right) {
            regionOut->left = region1->left;
            regionOut->top = region1->top;
            regionOut->right = region2->right;
            regionOut->bottom = region2->bottom;

            return TRUE;
        }
    }

    return FALSE;
}

CAR_INLINE Void RegionUnionRegion(
    /* [in, out] */ Region* destRegion,
    /* [in] */ const Region* srcRegion)
{
//    assert(RegionRegular(destRegion));
    assert(RegionRegular(srcRegion));

    if (destRegion->left > srcRegion->left) {
        destRegion->left = srcRegion->left;
    }

    if (destRegion->top > srcRegion->top) {
        destRegion->top = srcRegion->top;
    }

    if (destRegion->right < srcRegion->right) {
        destRegion->right = srcRegion->right;
    }

    if (destRegion->bottom < srcRegion->bottom) {
        destRegion->bottom = srcRegion->bottom;
    }
}

CAR_INLINE Void RegionUnionRectagnle(
    /* [in, out] */ Region* region,
    /* [in] */ const Rectangle* rectangle)
{
    Region      increment = REGION_INIT_FROM_RECTANGLE(rectangle);

    RegionUnionRegion(region, &increment);
}

CAR_INLINE Boolean RegionRectangleIntersect(
    /* [in, out] */ Region* region,
    /* [in] */ const Rectangle* rectangle)
{
    int right = rectangle->x + rectangle->width;
    int bottom = rectangle->y + rectangle->height;

    assert(RegionRegular(region));

    if (region->right <= rectangle->x
            || region->bottom <= rectangle->y
            || region->left >= right
            || region->top >= bottom) {
        return FALSE;
    }

    if (region->left < rectangle->x)
        region->left = rectangle->x;

    if (region->top < rectangle->y)
        region->top = rectangle->y;

    if (region->right > right)
        region->right = right;

    if (region->bottom > bottom)
        region->bottom = bottom;

    return TRUE;
}

CAR_INLINE Boolean RegionIntersect(
    /* [in] */ Region* region,
    /* [in] */ const Region* clip)
{
    assert(RegionRegular(region));
    assert(RegionRegular(clip));

    if (region->right <= clip->left
            || region->bottom <= clip->top
            || region->left >= clip->right
            || region->top >= clip->bottom) {
        return FALSE;
    }

    if (region->left < clip->left)
        region->left = clip->left;

    if (region->top < clip->top)
        region->top = clip->top;

    if (region->right > clip->right)
        region->right = clip->right;

    if (region->bottom > clip->bottom)
        region->bottom = clip->bottom;

    return TRUE;
}

CAR_INLINE Int32 RegionSubRegion(
    /* [in] */ Region region1,
    /* [in] */ Region region2,
    /* [out] */ Region* regionRemain)
{
    Int32 count = 0;

    assert(RegionRegular(&region1));
    assert(RegionRegular(&region2));
    assert(regionRemain);

    // be not intersect
    //
    if (!RegionIntersects(&region1, &region2)) {
        regionRemain[count++] = region1;
    }
    else {
        if (region2.top > region1.top) {
            regionRemain[count].left = region1.left;
            regionRemain[count].top = region1.top;
            regionRemain[count].right = region1.right;
            regionRemain[count].bottom = region2.top;
            count++;
            region1.top = region2.top;
        }

        if (region2.bottom < region1.bottom) {
            regionRemain[count].left = region1.left;
            regionRemain[count].top = region2.bottom;
            regionRemain[count].right = region1.right;
            regionRemain[count].bottom = region1.bottom;
            count++;
            region1.bottom = region2.bottom;
        }

        if (region2.left > region1.left) {
            if (region1.top > region2.top) {
                if (region1.bottom > region2.bottom) {
                    regionRemain[count].left = region1.left;
                    regionRemain[count].top = region1.top;
                    regionRemain[count].right = region2.left;
                    regionRemain[count].bottom = region2.bottom;
                    count++;
                }
                else {
                    regionRemain[count].left = region1.left;
                    regionRemain[count].top = region1.top;
                    regionRemain[count].right = region2.left;
                    regionRemain[count].bottom = region1.bottom;
                    count++;
                }
            }
            else {
                if (region1.bottom > region2.bottom) {
                    regionRemain[count].left = region1.left;
                    regionRemain[count].top = region2.top;
                    regionRemain[count].right = region2.left;
                    regionRemain[count].bottom = region2.bottom;
                    count++;
                }
                else {
                    regionRemain[count].left = region1.left;
                    regionRemain[count].top = region2.top;
                    regionRemain[count].right = region2.left;
                    regionRemain[count].bottom = region1.bottom;
                    count++;
                }
            }
        }

        if (region2.right < region1.right) {
            if (region1.top > region2.top) {
                if (region1.bottom > region2.bottom) {
                    regionRemain[count].left = region2.right;
                    regionRemain[count].top = region1.top;
                    regionRemain[count].right = region1.right;
                    regionRemain[count].bottom = region2.bottom;
                    count++;
                }
                else {
                    regionRemain[count].left = region2.right;
                    regionRemain[count].top = region1.top;
                    regionRemain[count].right = region1.right;
                    regionRemain[count].bottom = region1.bottom;
                    count++;
                }
            }
            else {
                if (region1.bottom > region2.bottom) {
                    regionRemain[count].left = region2.right;
                    regionRemain[count].top = region2.top;
                    regionRemain[count].right = region1.right;
                    regionRemain[count].bottom = region2.bottom;
                    count++;
                }
                else {
                    regionRemain[count].left = region2.right;
                    regionRemain[count].top = region2.top;
                    regionRemain[count].right = region1.right;
                    regionRemain[count].bottom = region1.bottom;
                    count++;
                }
            }
        }
    }

    assert(count <= 4);

    return count;
}

CAR_INLINE Boolean UnsafeRegionIntersect(
    /* [in, out] */ Region* region,
    /* [in] */ const Region* clip)
{
    assert(RegionRegular(clip));

    Region temp = *region;

    if (temp.left > temp.right) {
        temp.left = temp.left + temp.right;
        temp.right = temp.left - temp.right;
        temp.left = temp.left - temp.right;
    }

    if (temp.top > temp.bottom) {
        temp.top = temp.top + temp.bottom;
        temp.bottom = temp.top - temp.bottom;
        temp.top = temp.top - temp.bottom;
    }

    if (!RegionIntersects(&temp, clip)) {
        return FALSE;
    }

    if (region->left <= region->right) {
        if (region->top <= region->bottom) {
            return RegionIntersect(region, clip);
        }
        else {
            if (region->left < clip->left) {
                region->left = clip->left;
            }

            if (region->right > clip->right) {
                region->right = clip->right;
            }

            if (region->top > clip->bottom) {
                region->top = clip->bottom;
            }

            if (region->bottom < clip->top) {
                region->bottom = clip->top;
            }
        }
    }
    else {
        if (region->left > clip->right) {
            region->left = clip->right;
        }

        if (region->right < clip->left) {
            region->right = clip->left;
        }

        if (region->top <= region->bottom) {
            if (region->top < clip->top) {
                region->top = clip->top;
            }

            if (region->bottom > clip->bottom) {
                region->bottom = clip->bottom;
            }
        }
        else {
            if (region->top > clip->bottom) {
                region->top = clip->bottom;
            }

            if (region->bottom < clip->top) {
                region->bottom = clip->top;
            }
        }
    }

    return TRUE;
}

CAR_INLINE Boolean UnsafeRegionRectangleIntersect(
    /* [in, out] */ Region* region,
    /* [in] */ Rectangle* rectangle)
{
    Region  temp = REGION_INIT_FROM_RECTANGLE(rectangle);

    return UnsafeRegionIntersect(region, &temp);
}


//Region Relative
//
CAR_INLINE Boolean RegionIsEmpty(const Region *region)
{
    return !(region->left < region->right) && (region->top < region->bottom);
}

CAR_INLINE Boolean XIsInRegion(Int32 x, const Region *region)
{
    return (x >= region->left) && (x < region->right);
}

CAR_INLINE Boolean YIsInRegion(Int32 y, const Region *region)
{
    return (y >= region->top) && (y < region->bottom);
}

CAR_INLINE Int32 RegionCenterX(const Region * region)
{
    return (region->left + region->right) >> 1;
}

CAR_INLINE Int32 RegionCenterY(const Region * region)
{
    return (region->top + region->bottom) >> 1;
}


#endif  // __ELASTOS_GRAFIX_REGION_H__
