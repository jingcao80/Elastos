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

#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUT_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * An abstraction of the original CellLayout which supports laying out items
 * which span multiple cells into a grid-like layout.  Also supports dimming
 * to give a preview of its contents.
 */
class PagedViewCellLayout
    : public ViewGroup
    , public IPagedViewCellLayout
    , public IPage
{
public:
    class PagedViewCellLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IPagedViewCellLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL();

        PagedViewCellLayoutLayoutParams();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ PagedViewCellLayoutLayoutParams* source);

        CARAPI constructor(
            /* [in] */ Int32 cellX,
            /* [in] */ Int32 cellY,
            /* [in] */ Int32 cellHSpan,
            /* [in] */ Int32 cellVSpan);

        CARAPI Setup(
            /* [in] */ Int32 cellWidth,
            /* [in] */ Int32 cellHeight,
            /* [in] */ Int32 widthGap,
            /* [in] */ Int32 heightGap,
            /* [in] */ Int32 hStartPadding,
            /* [in] */ Int32 vStartPadding);

        CARAPI GetTag(
            /* [out] */ IInterface** tag);

        CARAPI SetTag(
            /* [in] */ IInterface* tag);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        /**
         * Horizontal location of the item in the grid.
         */
        //@ViewDebug.ExportedProperty
        Int32 mCellX;

        /**
         * Vertical location of the item in the grid.
         */
        //@ViewDebug.ExportedProperty
        Int32 mCellY;

        /**
         * Number of cells spanned horizontally by the item.
         */
        //@ViewDebug.ExportedProperty
        Int32 mCellHSpan;

        /**
         * Number of cells spanned vertically by the item.
         */
        //@ViewDebug.ExportedProperty
        Int32 mCellVSpan;

        /**
         * Is this item currently being dragged
         */
        Boolean mIsDragging;

        // X coordinate of the view in the layout.
        //@ViewDebug.ExportedProperty
        Int32 mX;
        // Y coordinate of the view in the layout.
        //@ViewDebug.ExportedProperty
        Int32 mY;

    private:
        // a data object that you can bind to this layout params
        AutoPtr<IInterface> mTag;
    };

public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("PagedViewCellLayout")

    PagedViewCellLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI GetCellWidth(
        /* [out] */ Int32* width);

    CARAPI GetCellHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI CancelLongPress();

    CARAPI AddViewToCellLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ Int32 childId,
        /* [in] */ IPagedViewCellLayoutLayoutParams* params,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RemoveAllViewsOnPage();

    //@Override
    CARAPI RemoveViewOnPageAt(
        /* [in] */ Int32 index);

    /**
     * Clears all the key listeners for the individual icons.
     */
    CARAPI ResetChildrenOnKeyListeners();

    //@Override
    CARAPI GetPageChildCount(
        /* [out] */ Int32* count);

    CARAPI GetChildrenLayout(
        /* [out] */ IPagedViewCellLayoutChildren** children);

    //@Override
    CARAPI GetChildOnPageAt(
        /* [in] */ Int32 i,
        /* [out] */ IView** view);

    //@Override
    CARAPI IndexOfChildOnPage(
        /* [in] */ IView* v,
        /* [out] */ Int32* index);

    CARAPI GetCellCountX(
        /* [out] */ Int32* count);

    CARAPI GetCellCountY(
        /* [out] */ Int32* count);

    CARAPI GetContentWidth(
        /* [out] */ Int32* width);

    CARAPI GetContentHeight(
        /* [out] */ Int32* height);

    CARAPI GetWidthBeforeFirstLayout(
        /* [out] */ Int32* width);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI EnableCenteredContent(
        /* [in] */ Boolean enabled);

    CARAPI SetCellCount(
        /* [in] */ Int32 xCount,
        /* [in] */ Int32 yCount);

    CARAPI SetGap(
        /* [in] */ Int32 widthGap,
        /* [in] */ Int32 heightGap);

    CARAPI GetCellCountForDimensions(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out, callee] */ ArrayOf<Int32>** dimensions);

    /**
     * Start dragging the specified child
     *
     * @param child The child that is being dragged
     */
    CARAPI OnDragChild(
        /* [in] */ IView* child);

    /**
     * Estimates the number of cells that the specified width would take up.
     */
    CARAPI EstimateCellHSpan(
        /* [in] */ Int32 width,
        /* [out] */ Int32* span);

    /**
     * Estimates the number of cells that the specified height would take up.
     */
    CARAPI EstimateCellVSpan(
        /* [in] */ Int32 height,
        /* [out] */ Int32* span);

    /** Returns an estimated center position of the cell at the specified index */
    CARAPI EstimateCellPosition(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out, callee] */ ArrayOf<Int32>** position);

    CARAPI CalculateCellCount(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 maxCellCountX,
        /* [in] */ Int32 maxCellCountY);

    /**
     * Estimates the width that the number of hSpan cells will take up.
     */
    CARAPI EstimateCellWidth(
        /* [in] */ Int32 hSpan,
        /* [out] */ Int32* width);

    /**
     * Estimates the height that the number of vSpan cells will take up.
     */
    CARAPI EstimateCellHeight(
        /* [in] */ Int32 vSpan,
        /* [out] */ Int32* height);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** param);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI SetChildrenDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

public:
    static const String TAG;

protected:
    AutoPtr<IPagedViewCellLayoutChildren> mChildren;

private:
    Int32 mCellCountX;
    Int32 mCellCountY;
    Int32 mOriginalCellWidth;
    Int32 mOriginalCellHeight;
    Int32 mCellWidth;
    Int32 mCellHeight;
    Int32 mOriginalWidthGap;
    Int32 mOriginalHeightGap;
    Int32 mWidthGap;
    Int32 mHeightGap;
    Int32 mMaxGap;

};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUT_H__