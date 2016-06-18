#include "elastos/droid/graphics/CAtlas.h"
#include "elastos/droid/graphics/CAtlasEntry.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Graphics {


Boolean CAtlas::SlicePolicy::MinAreaSplitDecision::SplitHorizontal(
    /* [in] */ Int32 freeWidth,
    /* [in] */ Int32 freeHeight,
    /* [in] */ Int32 rectWidth,
    /* [in] */ Int32 rectHeight)
{
    return rectWidth * freeHeight > freeWidth * rectHeight;
}

Boolean CAtlas::SlicePolicy::MaxAreaSplitDecision::SplitHorizontal(
    /* [in] */ Int32 freeWidth,
    /* [in] */ Int32 freeHeight,
    /* [in] */ Int32 rectWidth,
    /* [in] */ Int32 rectHeight)
{
    return rectWidth * freeHeight <= freeWidth * rectHeight;
}

Boolean CAtlas::SlicePolicy::ShorterFreeAxisSplitDecision::SplitHorizontal(
    /* [in] */ Int32 freeWidth,
    /* [in] */ Int32 freeHeight,
    /* [in] */ Int32 rectWidth,
    /* [in] */ Int32 rectHeight)
{
    return freeWidth <= freeHeight;
}

Boolean CAtlas::SlicePolicy::LongerFreeAxisSplitDecision::SplitHorizontal(
    /* [in] */ Int32 freeWidth,
    /* [in] */ Int32 freeHeight,
    /* [in] */ Int32 rectWidth,
    /* [in] */ Int32 rectHeight)
{
    return freeWidth > freeHeight;
}

CAtlas::SlicePolicy::Cell::Cell()
    : mX(0)
    , mY(0)
    , mWidth(0)
    , mHeight(0)
{
}

ECode CAtlas::SlicePolicy::Cell::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("cell[x=");
    sb += mX;
    sb += " y=";
    sb += mY;
    sb += " width=";
    sb += mWidth;
    sb += " height=";
    sb += mHeight;
    *str = sb.ToString();
    return NOERROR;
}

CAtlas::SlicePolicy::SlicePolicy(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags,
    /* [in] */ CAtlas::SlicePolicy::SplitDecision* splitDecision)
{
    mRoot = new Cell();

    mAllowRotation = (flags & FLAG_ALLOW_ROTATIONS) != 0;
    mPadding = (flags & FLAG_ADD_PADDING) != 0 ? 1 : 0;

    // The entire atlas is empty at first, minus padding
    AutoPtr<Cell> first = new Cell();
    first->mX = first->mY = mPadding;
    first->mWidth = width - 2 * mPadding;
    first->mHeight = height - 2 * mPadding;

    mRoot->mNext = first;
    mSplitDecision = splitDecision;
}

AutoPtr<IAtlasEntry> CAtlas::SlicePolicy::Pack(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IAtlasEntry* entry)
{
    AutoPtr<Cell> cell = mRoot->mNext;
    AutoPtr<Cell> prev = mRoot;

    while (cell != NULL) {
        if (Insert(cell, prev, width, height, entry)) {
            return entry;
        }

        prev = cell;
        cell = cell->mNext;
    }

    return NULL;
}

Boolean CAtlas::SlicePolicy::Insert(
    /* [in] */ CAtlas::SlicePolicy::Cell* cell,
    /* [in] */ CAtlas::SlicePolicy::Cell* prev,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IAtlasEntry* entry)
{
    Boolean rotated = FALSE;

    // If the rectangle doesn't fit we'll try to rotate it
    // if possible before giving up
    if (cell->mWidth < width || cell->mHeight < height) {
        if (mAllowRotation) {
            if (cell->mWidth < height || cell->mHeight < width) {
                return FALSE;
            }

            // Rotate the rectangle
            Int32 temp = width;
            width = height;
            height = temp;
            rotated = TRUE;
        } else {
            return FALSE;
        }
    }

    // Remaining free space after packing the rectangle
    Int32 deltaWidth = cell->mWidth - width;
    Int32 deltaHeight = cell->mHeight - height;

    // Split the remaining free space into two new cells
    AutoPtr<Cell> first = new Cell();
    AutoPtr<Cell> second = new Cell();

    first->mX = cell->mX + width + mPadding;
    first->mY = cell->mY;
    first->mWidth = deltaWidth - mPadding;

    second->mX = cell->mX;
    second->mY = cell->mY + height + mPadding;
    second->mHeight = deltaHeight - mPadding;

    if (mSplitDecision->SplitHorizontal(deltaWidth, deltaHeight, width, height)) {
        first->mHeight = height;
        second->mWidth = cell->mWidth;
    }
    else {
        first->mHeight = cell->mHeight;
        second->mWidth = width;

        // The order of the cells matters for efficient packing
        // We want to give priority to the cell chosen by the
        // split decision heuristic
        AutoPtr<Cell> temp = first;
        first = second;
        second = temp;
    }

    // Remove degenerate cases to keep the free list as small as possible
    if (first->mWidth > 0 && first->mHeight > 0) {
        prev->mNext = first;
        prev = first;
    }

    if (second->mWidth > 0 && second->mHeight > 0) {
        prev->mNext = second;
        second->mNext = cell->mNext;
    }
    else {
        prev->mNext = cell->mNext;
    }

    // The cell is now completely removed from the free list
    cell->mNext = NULL;

    // Return the location and rotation of the packed rectangle
    entry->SetX(cell->mX);
    entry->SetY(cell->mY);
    entry->SetRotated(rotated);

    return TRUE;
}

//========================================================================
// CAtlas
//========================================================================

CAR_OBJECT_IMPL(CAtlas)

CAR_INTERFACE_IMPL(CAtlas, Object, IAtlas)

ECode CAtlas::constructor(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return constructor(type, width, height, IAtlas::FLAG_DEFAULTS);
}

ECode CAtlas::constructor(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags)
{
    mPolicy = FindPolicy(type, width, height, flags);
    return NOERROR;
}

ECode CAtlas::Pack(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IAtlasEntry** entry)
{
    return Pack(width, height, NULL, entry);
}

ECode CAtlas::Pack(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IAtlasEntry* inEntry,
    /* [out] */ IAtlasEntry** outEntry)
{
    VALIDATE_NOT_NULL(outEntry)
    *outEntry = NULL;

    AutoPtr<IAtlasEntry> entry = inEntry;
    if (entry == NULL) {
        CAtlasEntry::New((IAtlasEntry**)&entry);
    }

    AutoPtr<IAtlasEntry> result = mPolicy->Pack(width, height, entry);
    *outEntry = result;
    REFCOUNT_ADD(*outEntry)
    return NOERROR;
}


AutoPtr<CAtlas::Policy> CAtlas::FindPolicy(
    /* [in] */ AtlasType type,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags)
{
    switch (type) {
        case AtlasType_SliceMinArea: {
            AutoPtr<SlicePolicy::SplitDecision> decision = new SlicePolicy::MinAreaSplitDecision();
            AutoPtr<Policy> policy = new SlicePolicy(width, height, flags, decision);
            return policy;
        }

        case AtlasType_SliceMaxArea: {
            AutoPtr<SlicePolicy::SplitDecision> decision = new SlicePolicy::MaxAreaSplitDecision();
            AutoPtr<Policy> policy = new SlicePolicy(width, height, flags, decision);
            return policy;
        }

        case AtlasType_SliceShortAxis: {
            AutoPtr<SlicePolicy::SplitDecision> decision = new SlicePolicy::ShorterFreeAxisSplitDecision();
            AutoPtr<Policy> policy = new SlicePolicy(width, height, flags, decision);
            return policy;
        }

        case AtlasType_SliceLongAxis: {
            AutoPtr<SlicePolicy::SplitDecision> decision = new SlicePolicy::LongerFreeAxisSplitDecision();
            AutoPtr<Policy> policy = new SlicePolicy(width, height, flags, decision);
            return policy;
        }
    }
    return NULL;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos