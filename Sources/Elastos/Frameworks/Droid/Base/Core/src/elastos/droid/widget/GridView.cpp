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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionItemInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/animation/GridLayoutAnimationController.h"
#include "elastos/droid/widget/GridView.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoCollectionInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoCollectionItemInfo;
using Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionItemInfo;
using Elastos::Droid::View::Animation::GridLayoutAnimationController;
using Elastos::Droid::View::Animation::IGridLayoutAnimationParameters;
using Elastos::Droid::Widget::CAbsListViewLayoutParams;
using Elastos::Droid::Graphics::CRect;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const String GridView::GRIDVIEW_NAME = String("GridView");
const Int32 GridView::NO_STRETCH;
const Int32 GridView::STRETCH_SPACING;
const Int32 GridView::STRETCH_COLUMN_WIDTH;
const Int32 GridView::STRETCH_SPACING_UNIFORM;
const Int32 GridView::AUTO_FIT;

CAR_INTERFACE_IMPL(GridView, AbsListView, IGridView)

GridView::GridView()
    : mNumColumns(AUTO_FIT)
    , mHorizontalSpacing(0)
    , mRequestedHorizontalSpacing(0)
    , mVerticalSpacing(0)
    , mStretchMode(STRETCH_COLUMN_WIDTH)
    , mColumnWidth(0)
    , mRequestedColumnWidth(0)
    , mRequestedNumColumns(0)
    , mGravity(IGravity::START)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
}

ECode GridView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    assert(context != NULL);
    AbsListView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::GridView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    Int32 hSpacing;
    a->GetDimensionPixelOffset(
        R::styleable::GridView_horizontalSpacing, 0, &hSpacing);
    SetHorizontalSpacing(hSpacing);

    Int32 vSpacing;
    a->GetDimensionPixelOffset(
        R::styleable::GridView_verticalSpacing, 0, &vSpacing);
    SetVerticalSpacing(vSpacing);

    Int32 index;
    a->GetInt32(R::styleable::GridView_stretchMode, STRETCH_COLUMN_WIDTH, &index);
    if (index >= 0) {
        SetStretchMode(index);
    }

    Int32 columnWidth;
    a->GetDimensionPixelOffset(R::styleable::GridView_columnWidth, -1, &columnWidth);
    if (columnWidth > 0) {
        SetColumnWidth(columnWidth);
    }

    Int32 numColumns;
    a->GetInt32(R::styleable::GridView_numColumns, 1, &numColumns);
    SetNumColumns(numColumns);

    a->GetInt32(R::styleable::GridView_gravity, -1, &index);
    if (index >= 0) {
        SetGravity(index);
    }

    a->Recycle();
    return NOERROR;
}

ECode GridView::SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent)
{
    return AbsListView::SetRemoteViewsAdapter(intent);
}

ECode GridView::GetAdapter(
    /* [out] */ IAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    *adapter = IAdapter::Probe(mAdapter);
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode GridView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    if (mAdapter != NULL && mDataSetObserver != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(mDataSetObserver);
    }

    ResetList();
    mRecycler->Clear();
    mAdapter = IListAdapter::Probe(adapter);

    mOldSelectedPosition = IAdapterView::INVALID_POSITION;
    mOldSelectedRowId = IAdapterView::INVALID_ROW_ID;

    AbsListView::SetAdapter(adapter);

    if (mAdapter != NULL) {
        mOldItemCount = mItemCount;
        IAdapter::Probe(mAdapter)->GetCount(&mItemCount);
        mDataChanged = TRUE;
        CheckFocus();

        mDataSetObserver = new AdapterDataSetObserver(this);
        IAdapter::Probe(mAdapter)->RegisterDataSetObserver(mDataSetObserver);

        Int32 count;
        IAdapter::Probe(mAdapter)->GetViewTypeCount(&count);
        mRecycler->SetViewTypeCount(count);

        Int32 position;
        if (mStackFromBottom) {
            position = LookForSelectablePosition(mItemCount - 1, FALSE);
        } else {
            position = LookForSelectablePosition(0, TRUE);
        }
        SetSelectedPositionInt(position);
        SetNextSelectedPositionInt(position);
        CheckSelectionChanged();
    } else {
        CheckFocus();
        CheckSelectionChanged();
    }

    RequestLayout();

    return NOERROR;
}

Int32 GridView::LookForSelectablePosition(
    /* [in] */ Int32 position,
    /* [in] */ Boolean lookDown)
{
    AutoPtr<IListAdapter> adapter = mAdapter;
    Boolean isInTouchMode;
    if (adapter == NULL || (IsInTouchMode(&isInTouchMode), isInTouchMode)) {
        return IAdapterView::INVALID_POSITION;
    }

    if (position < 0 || position >= mItemCount) {
        return IAdapterView::INVALID_POSITION;
    }
    return position;
}

ECode GridView::FillGap(
    /* [in] */ Boolean down)
{
    Int32 numColumns = mNumColumns;
    Int32 verticalSpacing = mVerticalSpacing;

    Int32 count;
    GetChildCount(&count);

    if (down) {
        Int32 paddingTop = 0;

        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            GetListPaddingTop(&paddingTop);
        }

        AutoPtr<IView> child;
        GetChildAt(count - 1, (IView**)&child);
        Int32 bottom;
        child->GetBottom(&bottom);
        Int32 startOffset = count > 0 ?
                bottom + verticalSpacing : paddingTop;
        Int32 position = mFirstPosition + count;
        if (mStackFromBottom) {
            position += numColumns - 1;
        }
        FillDown(position, startOffset);
        Int32 childCount;
        GetChildCount(&childCount);
        CorrectTooHigh(numColumns, verticalSpacing, childCount);
    } else {
        Int32 paddingBottom = 0;
        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            GetListPaddingBottom(&paddingBottom);
        }

        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 top;
        child->GetTop(&top);
        Int32 startOffset;
        if (count > 0) {
            startOffset = top - verticalSpacing;
        } else {
            Int32 height;
            GetHeight(&height);
            startOffset = height - paddingBottom;
        }
        Int32 position = mFirstPosition;
        if (!mStackFromBottom) {
            position -= numColumns;
        } else {
            position--;
        }
        FillUp(position, startOffset);
        Int32 lastCount;
        GetChildCount(&lastCount);
        CorrectTooLow(numColumns, verticalSpacing, lastCount);
    }
    return NOERROR;
}

AutoPtr<IView> GridView::FillDown(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 nextTop)
{
    AutoPtr<IView> selectedView;

    Int32 end = (mBottom - mTop);
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        end -= ((CRect*)mListPadding.Get())->mBottom;
    }

    while (nextTop < end && pos < mItemCount) {
        AutoPtr<IView> temp = MakeRow(pos, nextTop, TRUE);
        if (temp != NULL) {
            selectedView = temp;
        }

        mReferenceView->GetBottom(&nextTop);
        nextTop += mVerticalSpacing;

        pos += mNumColumns;
    }
    Int32 childCount;
    GetChildCount(&childCount);
    SetVisibleRangeHint(mFirstPosition, mFirstPosition + childCount - 1);
    return selectedView;
}

AutoPtr<IView> GridView::MakeRow(
    /* [in] */ Int32 startPos,
    /* [in] */ Int32 y,
    /* [in] */ Boolean flow)
{
    Int32 columnWidth = mColumnWidth;
    Int32 horizontalSpacing = mHorizontalSpacing;

    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);

    Int32 last;
    Int32 nextLeft;

    if (isLayoutRtl) {
        Int32 width;
        GetWidth(&width);
        nextLeft = width - ListPaddingRight() - columnWidth -
                ((mStretchMode == STRETCH_SPACING_UNIFORM) ? horizontalSpacing : 0);
    } else {
        nextLeft = ListPaddingLeft() +
                ((mStretchMode == STRETCH_SPACING_UNIFORM) ? horizontalSpacing : 0);
    }

    if (!mStackFromBottom) {
        last = Elastos::Core::Math::Min(startPos + mNumColumns, mItemCount);
    } else {
        last = startPos + 1;
        startPos = Elastos::Core::Math::Max(0, startPos - mNumColumns + 1);

        if (last - startPos < mNumColumns) {
            Int32 deltaLeft = (mNumColumns - (last - startPos)) * (columnWidth + horizontalSpacing);
            nextLeft += (isLayoutRtl ? -1 : +1) * deltaLeft;
        }
    }

    AutoPtr<IView> selectedView;

    Boolean hasFocus = ShouldShowSelector();
    Boolean inClick = TouchModeDrawsInPressedState();
    Int32 selectedPosition = mSelectedPosition;

    AutoPtr<IView> child;
    Int32 nextChildDir = isLayoutRtl ? -1 : +1;
    for (Int32 pos = startPos; pos < last; pos++) {
        Boolean selected = pos == selectedPosition;

        Int32 where = flow ? -1 : pos - startPos;
        child = MakeAndAddView(pos, y, flow, nextLeft, selected, where);

        nextLeft += nextChildDir * columnWidth;
        if (pos < last - 1) {
            nextLeft += nextChildDir * horizontalSpacing;
        }

        if (selected && (hasFocus || inClick)) {
            selectedView = child;
        }
    }

    mReferenceView = child;

    if (selectedView != NULL) {
        mReferenceViewInSelectedRow = mReferenceView;
    }

    return selectedView;
}

AutoPtr<IView> GridView::FillUp(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 nextBottom)
{
    AutoPtr<IView> selectedView;

    Int32 end = 0;
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        end = ListPaddingTop();
    }

    while (nextBottom > end && pos >= 0) {

        AutoPtr<IView> temp = MakeRow(pos, nextBottom, FALSE);
        if (temp != NULL) {
            selectedView = temp;
        }

        mReferenceView->GetTop(&nextBottom);
        nextBottom -= mVerticalSpacing;

        mFirstPosition = pos;

        pos -= mNumColumns;
    }

    if (mStackFromBottom) {
        mFirstPosition = Elastos::Core::Math::Max(0, pos + 1);
    }

    Int32 childCount;
    GetChildCount(&childCount);
    SetVisibleRangeHint(mFirstPosition, mFirstPosition + childCount - 1);
    return selectedView;
}

AutoPtr<IView> GridView::FillFromTop(
    /* [in] */ Int32 nextTop)
{
    mFirstPosition = Elastos::Core::Math::Min(mFirstPosition, mSelectedPosition);
    mFirstPosition = Elastos::Core::Math::Min(mFirstPosition, mItemCount - 1);
    if (mFirstPosition < 0) {
        mFirstPosition = 0;
    }
    mFirstPosition -= mFirstPosition % mNumColumns;
    return FillDown(mFirstPosition, nextTop);
}

AutoPtr<IView> GridView::FillFromBottom(
    /* [in] */ Int32 lastPosition,
    /* [in] */ Int32 nextBottom)
{
    lastPosition = Elastos::Core::Math::Max(lastPosition, mSelectedPosition);
    lastPosition = Elastos::Core::Math::Min(lastPosition, mItemCount - 1);

    Int32 invertedPosition = mItemCount - 1 - lastPosition;
    lastPosition = mItemCount - 1 - (invertedPosition - (invertedPosition % mNumColumns));

    return FillUp(lastPosition, nextBottom);
}

AutoPtr<IView> GridView::FillSelection(
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 selectedPosition = ReconcileSelectedPosition();
    Int32 numColumns = mNumColumns;
    Int32 verticalSpacing = mVerticalSpacing;

    Int32 rowStart;
    Int32 rowEnd = -1;

    if (!mStackFromBottom) {
        rowStart = selectedPosition - (selectedPosition % numColumns);
    } else {
        Int32 invertedSelection = mItemCount - 1 - selectedPosition;

        rowEnd = mItemCount - 1 - (invertedSelection - (invertedSelection % numColumns));
        rowStart = Elastos::Core::Math::Max(0, rowEnd - numColumns + 1);
    }

    Int32 fadingEdgeLength;
    GetVerticalFadingEdgeLength(&fadingEdgeLength);
    Int32 topSelectionPixel = GetTopSelectionPixel(childrenTop, fadingEdgeLength, rowStart);

    AutoPtr<IView> sel = MakeRow(mStackFromBottom ? rowEnd : rowStart, topSelectionPixel, TRUE);
    mFirstPosition = rowStart;

    AutoPtr<IView> referenceView = mReferenceView;

    Int32 bottom, top;
    if (!mStackFromBottom) {
        referenceView->GetBottom(&bottom);
        FillDown(rowStart + numColumns, bottom + verticalSpacing);
        PinToBottom(childrenBottom);

        referenceView->GetTop(&top);
        FillUp(rowStart - numColumns, top - verticalSpacing);
        AdjustViewsUpOrDown();
    } else {
        Int32 bottomSelectionPixel = GetBottomSelectionPixel(childrenBottom,
                fadingEdgeLength, numColumns, rowStart);
        referenceView->GetBottom(&bottom);
        Int32 offset = bottomSelectionPixel - bottom;
        OffsetChildrenTopAndBottom(offset);

        referenceView->GetTop(&top);
        FillUp(rowStart - 1, top - verticalSpacing);
        PinToTop(childrenTop);
        FillDown(rowEnd + numColumns, bottom + verticalSpacing);
        AdjustViewsUpOrDown();
    }

    return sel;
}

void GridView::PinToTop(
    /* [in] */ Int32 childrenTop)
{
    if (mFirstPosition == 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 top;
        child->GetTop(&top);
        Int32 offset = childrenTop - top;
        if (offset < 0) {
            OffsetChildrenTopAndBottom(offset);
        }
    }
}

void GridView::PinToBottom(
    /* [in] */ Int32 childrenBottom)
{
    Int32 count;
    GetChildCount(&count);
    if (mFirstPosition + count == mItemCount) {
        AutoPtr<IView> child;
        GetChildAt(count - 1, (IView**)&child);
        Int32 bottom;
        child->GetBottom(&bottom);
        Int32 offset = childrenBottom - bottom;
        if (offset > 0) {
            OffsetChildrenTopAndBottom(offset);
        }
    }
}

Int32 GridView::FindMotionRow(
    /* [in] */ Int32 y)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount > 0) {

        Int32 numColumns = mNumColumns;
        if (!mStackFromBottom) {
            for (Int32 i = 0; i < childCount; i += numColumns) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);
                Int32 bottom;
                child->GetBottom(&bottom);
                if (y <= bottom) {
                    return mFirstPosition + i;
                }
            }
        } else {
            for (Int32 i = childCount - 1; i >= 0; i -= numColumns) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);
                Int32 top;
                child->GetTop(&top);
                if (y >= top) {
                    return mFirstPosition + i;
                }
            }
        }
    }
    return IAdapterView::INVALID_POSITION;
}

ECode GridView::SmoothScrollToPosition(
    /* [in] */ Int32 position)
{
    return AbsListView::SmoothScrollToPosition(position);
}

ECode GridView::SmoothScrollByOffset(
    /* [in] */ Int32 offset)
{
    return AbsListView::SmoothScrollByOffset(offset);
}

Int32 GridView::GetGravity()
{
    return mGravity;
}

ECode GridView::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    *gravity = GetGravity();
    return NOERROR;
}

Int32 GridView::GetHorizontalSpacing()
{
    return mHorizontalSpacing;
}

ECode GridView::GetHorizontalSpacing(
    /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing)
    *spacing = GetHorizontalSpacing();
    return NOERROR;
}

Int32 GridView::GetRequestedHorizontalSpacing()
{
    return mRequestedHorizontalSpacing;
}

ECode GridView::GetRequestedHorizontalSpacing(
    /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing)
    *spacing = GetRequestedHorizontalSpacing();
    return NOERROR;
}

Int32 GridView::GetVerticalSpacing()
{
    return mVerticalSpacing;
}

ECode GridView::GetVerticalSpacing(
    /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing)
    *spacing = GetVerticalSpacing();
    return NOERROR;
}

Int32 GridView::GetColumnWidth()
{
    return mColumnWidth;
}

ECode GridView::GetColumnWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = GetColumnWidth();
    return NOERROR;
}

Int32 GridView::GetRequestedColumnWidth()
{
    return mRequestedColumnWidth;
}

ECode GridView::GetRequestedColumnWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = GetRequestedColumnWidth();
    return NOERROR;
}

Int32 GridView::GetNumColumns()
{
    return mNumColumns;
}

ECode GridView::GetNumColumns(
    /* [out] */ Int32* columns)
{
    VALIDATE_NOT_NULL(columns)
    *columns = GetNumColumns();
    return NOERROR;
}

ECode GridView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsListView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(GRIDVIEW_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode GridView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsListView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(GRIDVIEW_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);

    Int32 columnsCount = GetNumColumns();
    Int32 count;
    GetCount(&count);
    Int32 rowsCount = count / columnsCount;
    Int32 selectionMode = GetSelectionModeForAccessibility();
    AutoPtr<IAccessibilityNodeInfoCollectionInfo> collectionInfo;
    AccessibilityNodeInfoCollectionInfo::Obtain(
            rowsCount, columnsCount, FALSE, selectionMode,
            (IAccessibilityNodeInfoCollectionInfo**)&collectionInfo);
    info->SetCollectionInfo(collectionInfo);
    return NOERROR;
}

AutoPtr<IView> GridView::FillSpecific(
    /* [in] */ Int32 position,
    /* [in] */ Int32 top)
{
    Int32 numColumns = mNumColumns;

    Int32 motionRowStart;
    Int32 motionRowEnd = -1;

    if (!mStackFromBottom) {
        motionRowStart = position - (position % numColumns);
    } else {
        Int32 invertedSelection = mItemCount - 1 - position;

        motionRowEnd = mItemCount - 1 - (invertedSelection - (invertedSelection % numColumns));
        motionRowStart = Elastos::Core::Math::Max(0, motionRowEnd - numColumns + 1);
    }

    AutoPtr<IView> temp = MakeRow(mStackFromBottom ? motionRowEnd : motionRowStart, top, TRUE);

    mFirstPosition = motionRowStart;

    AutoPtr<IView> referenceView = mReferenceView;
    if (referenceView == NULL) {
        return NULL;
    }

    Int32 verticalSpacing = mVerticalSpacing;

    AutoPtr<IView> above;
    AutoPtr<IView> below;

    Int32 t, bottom;
    if (!mStackFromBottom) {

        referenceView->GetTop(&t);
        referenceView->GetBottom(&bottom);

        above = FillUp(motionRowStart - numColumns, t - verticalSpacing);
        AdjustViewsUpOrDown();
        below = FillDown(motionRowStart + numColumns, bottom + verticalSpacing);
        Int32 childCount;
        GetChildCount(&childCount);
        if (childCount > 0) {
            CorrectTooHigh(numColumns, verticalSpacing, childCount);
        }
    } else {
        referenceView->GetBottom(&bottom);
        referenceView->GetTop(&t);

        below = FillDown(motionRowEnd + numColumns, bottom + verticalSpacing);
        AdjustViewsUpOrDown();
        above = FillUp(motionRowStart - 1, t - verticalSpacing);
        Int32 childCount;
        GetChildCount(&childCount);
        if (childCount > 0) {
            CorrectTooLow(numColumns, verticalSpacing, childCount);
        }
    }

    if (temp != NULL) {
        return temp;
    } else if (above != NULL) {
        return above;
    } else {
        return below;
    }
}

void GridView::CorrectTooHigh(
    /* [in] */ Int32 numColumns,
    /* [in] */ Int32 verticalSpacing,
    /* [in] */ Int32 childCount)
{
    Int32 lastPosition = mFirstPosition + childCount - 1;
    if (lastPosition == mItemCount - 1 && childCount > 0) {
        AutoPtr<IView> lastChild;
        GetChildAt(childCount - 1, (IView**)&lastChild);

        Int32 lastBottom;
        lastChild->GetBottom(&lastBottom);
        Int32 end = (mBottom - mTop) - ListPaddingBottom();

        Int32 bottomOffset = end - lastBottom;

        AutoPtr<IView> firstChild;
        GetChildAt(0, (IView**)&firstChild);
        Int32 firstTop;
        firstChild->GetTop(&firstTop);

        if (bottomOffset > 0 && (mFirstPosition > 0 || firstTop < ListPaddingTop()))  {
            if (mFirstPosition == 0) {
                bottomOffset = Elastos::Core::Math::Min(bottomOffset, ListPaddingTop() - firstTop);
            }

            OffsetChildrenTopAndBottom(bottomOffset);
            if (mFirstPosition > 0) {
                Int32 top;
                firstChild->GetTop(&top);
                FillUp(mFirstPosition - (mStackFromBottom ? 1 : numColumns),
                        top - verticalSpacing);
                AdjustViewsUpOrDown();
            }
        }
    }
}

void GridView::CorrectTooLow(
    /* [in] */ Int32 numColumns,
    /* [in] */ Int32 verticalSpacing,
    /* [in] */ Int32 childCount)
{
    if (mFirstPosition == 0 && childCount > 0) {
        AutoPtr<IView> firstChild;
        GetChildAt(0, (IView**)&firstChild);

        Int32 firstTop;
        firstChild->GetTop(&firstTop);

        Int32 start = ListPaddingTop();

        Int32 end = (mBottom - mTop) - ListPaddingBottom();

        Int32 topOffset = firstTop - start;
        AutoPtr<IView> lastChild;
        GetChildAt(childCount - 1, (IView**)&lastChild);
        Int32 lastBottom;
        lastChild->GetBottom(&lastBottom);
        Int32 lastPosition = mFirstPosition + childCount - 1;

        if (topOffset > 0 && (lastPosition < mItemCount - 1 || lastBottom > end))  {
            if (lastPosition == mItemCount - 1 ) {
                topOffset = Elastos::Core::Math::Min(topOffset, lastBottom - end);
            }

            OffsetChildrenTopAndBottom(-topOffset);
            if (lastPosition < mItemCount - 1) {
                Int32 bottom;
                lastChild->GetBottom(&bottom);
                FillDown(lastPosition + (!mStackFromBottom ? 1 : numColumns),
                       bottom + verticalSpacing);
                AdjustViewsUpOrDown();
            }
        }
    }
}

AutoPtr<IView> GridView::FillFromSelection(
    /* [in] */ Int32 selectedTop,
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 fadingEdgeLength;
    GetVerticalFadingEdgeLength(&fadingEdgeLength);
    Int32 selectedPosition = mSelectedPosition;
    Int32 numColumns = mNumColumns;
    Int32 verticalSpacing = mVerticalSpacing;

    Int32 rowStart;
    Int32 rowEnd = -1;

    if (!mStackFromBottom) {
        rowStart = selectedPosition - (selectedPosition % numColumns);
    } else {
        Int32 invertedSelection = mItemCount - 1 - selectedPosition;

        rowEnd = mItemCount - 1 - (invertedSelection - (invertedSelection % numColumns));
        rowStart = Elastos::Core::Math::Max(0, rowEnd - numColumns + 1);
    }

    AutoPtr<IView> sel;
    AutoPtr<IView> referenceView;

    Int32 topSelectionPixel = GetTopSelectionPixel(childrenTop, fadingEdgeLength, rowStart);
    Int32 bottomSelectionPixel = GetBottomSelectionPixel(childrenBottom, fadingEdgeLength,
            numColumns, rowStart);

    sel = MakeRow(mStackFromBottom ? rowEnd : rowStart, selectedTop, TRUE);
    mFirstPosition = rowStart;

    referenceView = mReferenceView;
    AdjustForTopFadingEdge(referenceView, topSelectionPixel, bottomSelectionPixel);
    AdjustForBottomFadingEdge(referenceView, topSelectionPixel, bottomSelectionPixel);

    Int32 top, bottom;
    referenceView->GetTop(&top);
    referenceView->GetBottom(&bottom);

    if (!mStackFromBottom) {
        FillUp(rowStart - numColumns, top - verticalSpacing);
        AdjustViewsUpOrDown();
        FillDown(rowStart + numColumns, bottom + verticalSpacing);
    } else {
        FillDown(rowEnd + numColumns, bottom + verticalSpacing);
        AdjustViewsUpOrDown();
        FillUp(rowStart - 1, top - verticalSpacing);
    }
    return sel;
}

Int32 GridView::GetBottomSelectionPixel(
    /* [in] */ Int32 childrenBottom,
    /* [in] */ Int32 fadingEdgeLength,
    /* [in] */ Int32 numColumns,
    /* [in] */ Int32 rowStart)
{
    Int32 bottomSelectionPixel = childrenBottom;
    if (rowStart + numColumns - 1 < mItemCount - 1) {
        bottomSelectionPixel -= fadingEdgeLength;
    }
    return bottomSelectionPixel;
}

Int32 GridView::GetTopSelectionPixel(
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 fadingEdgeLength,
    /* [in] */ Int32 rowStart)
{
    Int32 topSelectionPixel = childrenTop;
    if (rowStart > 0) {
        topSelectionPixel += fadingEdgeLength;
    }
    return topSelectionPixel;
}

void GridView::AdjustForBottomFadingEdge(
    /* [in] */ IView* childInSelectedRow,
    /* [in] */ Int32 topSelectionPixel,
    /* [in] */ Int32 bottomSelectionPixel)
{
    Int32 bottom, top;
    childInSelectedRow->GetBottom(&bottom);
    childInSelectedRow->GetTop(&top);
    if (bottom > bottomSelectionPixel) {

        Int32 spaceAbove = top - topSelectionPixel;

        Int32 spaceBelow = bottom - bottomSelectionPixel;
        Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);

        OffsetChildrenTopAndBottom(-offset);
    }
}

void GridView::AdjustForTopFadingEdge(
    /* [in] */ IView* childInSelectedRow,
    /* [in] */ Int32 topSelectionPixel,
    /* [in] */ Int32 bottomSelectionPixel)
{
    Int32 top, bottom;
    childInSelectedRow->GetTop(&top);
    childInSelectedRow->GetBottom(&bottom);
    if (top < topSelectionPixel) {
        Int32 spaceAbove = topSelectionPixel - top;

        Int32 spaceBelow = bottomSelectionPixel - bottom;
        Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);

        OffsetChildrenTopAndBottom(offset);
    }
}

AutoPtr<IView> GridView::MoveSelection(
    /* [in] */ Int32 delta,
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 fadingEdgeLength;
    GetVerticalFadingEdgeLength(&fadingEdgeLength);
    Int32 selectedPosition = mSelectedPosition;
    Int32 numColumns = mNumColumns;
    Int32 verticalSpacing = mVerticalSpacing;

    Int32 oldRowStart;
    Int32 rowStart;
    Int32 rowEnd = -1;

    if (!mStackFromBottom) {
        oldRowStart = (selectedPosition - delta) - ((selectedPosition - delta) % numColumns);

        rowStart = selectedPosition - (selectedPosition % numColumns);
    } else {
        Int32 invertedSelection = mItemCount - 1 - selectedPosition;

        rowEnd = mItemCount - 1 - (invertedSelection - (invertedSelection % numColumns));
        rowStart = Elastos::Core::Math::Max(0, rowEnd - numColumns + 1);

        invertedSelection = mItemCount - 1 - (selectedPosition - delta);
        oldRowStart = mItemCount - 1 - (invertedSelection - (invertedSelection % numColumns));
        oldRowStart = Elastos::Core::Math::Max(0, oldRowStart - numColumns + 1);
    }

    Int32 rowDelta = rowStart - oldRowStart;

    Int32 topSelectionPixel = GetTopSelectionPixel(childrenTop, fadingEdgeLength, rowStart);
    Int32 bottomSelectionPixel = GetBottomSelectionPixel(childrenBottom, fadingEdgeLength,
            numColumns, rowStart);

    mFirstPosition = rowStart;

    AutoPtr<IView> sel;
    AutoPtr<IView> referenceView;

    if (rowDelta > 0) {
        Int32 oldBottom = 0;
        if (mReferenceViewInSelectedRow != NULL) {
            mReferenceViewInSelectedRow->GetBottom(&oldBottom);
        }

        sel = MakeRow(mStackFromBottom ? rowEnd : rowStart, oldBottom + verticalSpacing, TRUE);
        referenceView = mReferenceView;

        AdjustForBottomFadingEdge(referenceView, topSelectionPixel, bottomSelectionPixel);
    } else if (rowDelta < 0) {
        Int32 oldTop = 0;
        if (mReferenceViewInSelectedRow != NULL) {
            mReferenceViewInSelectedRow->GetTop(&oldTop);
        }

        sel = MakeRow(mStackFromBottom ? rowEnd : rowStart, oldTop - verticalSpacing, FALSE);
        referenceView = mReferenceView;

        AdjustForTopFadingEdge(referenceView, topSelectionPixel, bottomSelectionPixel);
    } else {
        Int32 oldTop = 0;
        if (mReferenceViewInSelectedRow != NULL) {
            mReferenceViewInSelectedRow->GetTop(&oldTop);
        }

        sel = MakeRow(mStackFromBottom ? rowEnd : rowStart, oldTop, TRUE);
        referenceView = mReferenceView;
    }

    Int32 bottom = 0, top = 0;
    referenceView->GetTop(&top);
    referenceView->GetBottom(&bottom);

    if (!mStackFromBottom) {
        FillUp(rowStart - numColumns, top - verticalSpacing);
        AdjustViewsUpOrDown();
        FillDown(rowStart + numColumns, bottom + verticalSpacing);
    } else {
        FillDown(rowEnd + numColumns, bottom + verticalSpacing);
        AdjustViewsUpOrDown();
        FillUp(rowStart - 1, top - verticalSpacing);
    }

    return sel;
}

Boolean GridView::DetermineColumns(
    /* [in] */ Int32 availableSpace)
{
    Int32 requestedHorizontalSpacing = mRequestedHorizontalSpacing;
    Int32 stretchMode = mStretchMode;
    Int32 requestedColumnWidth = mRequestedColumnWidth;
    Boolean didNotInitiallyFit = FALSE;

    if (mRequestedNumColumns == AUTO_FIT) {
        if (requestedColumnWidth > 0) {
            mNumColumns = (availableSpace + requestedHorizontalSpacing) /
                    (requestedColumnWidth + requestedHorizontalSpacing);
        } else {
            mNumColumns = 2;
        }
    } else {
        mNumColumns = mRequestedNumColumns;
    }

    if (mNumColumns <= 0) {
        mNumColumns = 1;
    }

    switch (stretchMode) {
    case NO_STRETCH:
        mColumnWidth = requestedColumnWidth;
        mHorizontalSpacing = requestedHorizontalSpacing;
        break;

    default:
        Int32 spaceLeftOver = availableSpace - (mNumColumns * requestedColumnWidth) -
                ((mNumColumns - 1) * requestedHorizontalSpacing);

        if(spaceLeftOver < 0) {
            didNotInitiallyFit = TRUE;
        }

        switch (stretchMode) {
        case STRETCH_COLUMN_WIDTH:
            mColumnWidth = requestedColumnWidth + spaceLeftOver / mNumColumns;
            mHorizontalSpacing = requestedHorizontalSpacing;
            break;

        case STRETCH_SPACING:
            mColumnWidth = requestedColumnWidth;
            if (mNumColumns > 1) {
                mHorizontalSpacing = requestedHorizontalSpacing +
                    spaceLeftOver / (mNumColumns - 1);
            } else {
                mHorizontalSpacing = requestedHorizontalSpacing + spaceLeftOver;
            }
            break;

        case STRETCH_SPACING_UNIFORM:
            mColumnWidth = requestedColumnWidth;
            if (mNumColumns > 1) {
                mHorizontalSpacing = requestedHorizontalSpacing +
                    spaceLeftOver / (mNumColumns + 1);
            } else {
                mHorizontalSpacing = requestedHorizontalSpacing + spaceLeftOver;
            }
            break;
        }

        break;
    }
    return didNotInitiallyFit;
}

ECode GridView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AbsListView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    if (widthMode == MeasureSpec::UNSPECIFIED) {
        if (mColumnWidth > 0) {
            widthSize = mColumnWidth + ListPaddingLeft() + ListPaddingRight();
        } else {
            widthSize = ListPaddingLeft() + ListPaddingRight();
        }
        Int32 verticalScrollbarWidth;
        GetVerticalScrollbarWidth(&verticalScrollbarWidth);
        widthSize += verticalScrollbarWidth;
    }

    Int32 childWidth = widthSize - ListPaddingLeft() - ListPaddingRight();
    DetermineColumns(childWidth);

    Int32 childHeight = 0;
    mItemCount = 0;
    if (mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->GetCount(&mItemCount);
    }

    Int32 count = mItemCount;
    if (count > 0) {
        AutoPtr<IView> child = ObtainView(0, mIsScrap);

        AutoPtr<IViewGroupLayoutParams> vglp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IAbsListViewLayoutParams* p;
        if (vglp != NULL) {
            p = IAbsListViewLayoutParams::Probe(vglp);
        }
        else {
            CAbsListViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                    IViewGroupLayoutParams::WRAP_CONTENT, 0, (IViewGroupLayoutParams**)&vglp);
            p = IAbsListViewLayoutParams::Probe(vglp);
            child->SetLayoutParams(vglp);
        }

        CAbsListViewLayoutParams* alvlp = (CAbsListViewLayoutParams*)p;
        IAdapter::Probe(mAdapter)->GetItemViewType(0, &alvlp->mViewType);
        alvlp->mForceAdd = TRUE;

        Int32 childHeightSpec = GetChildMeasureSpec(
            MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED), 0, alvlp->mHeight);
        Int32 childWidthSpec = GetChildMeasureSpec(
            MeasureSpec::MakeMeasureSpec(mColumnWidth, MeasureSpec::EXACTLY), 0, alvlp->mWidth);
        child->Measure(childWidthSpec, childHeightSpec);

        child->GetMeasuredHeight(&childHeight);

        if (mRecycler->ShouldRecycleViewType(alvlp->mViewType)) {
            mRecycler->AddScrapView(child, -1);
        }
    }

    if (heightMode == MeasureSpec::UNSPECIFIED) {
        Int32 verticalFadingEdgeLength;
        GetVerticalFadingEdgeLength(&verticalFadingEdgeLength);
        heightSize = ListPaddingTop() + ListPaddingBottom() + childHeight +
                verticalFadingEdgeLength * 2;
    }

    if (heightMode == MeasureSpec::AT_MOST) {
        Int32 ourSize =  ListPaddingTop() + ListPaddingBottom();

        Int32 numColumns = mNumColumns;
        for (Int32 i = 0; i < count; i += numColumns) {
            ourSize += childHeight;
            if (i + numColumns < count) {
                ourSize += mVerticalSpacing;
            }
            if (ourSize >= heightSize) {
                ourSize = heightSize;
                break;
            }
        }
        heightSize = ourSize;
    }

    SetMeasuredDimension(widthSize, heightSize);
    mWidthMeasureSpec = widthMeasureSpec;
    return NOERROR;
}

ECode GridView::AttachLayoutAnimationParameters(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count)
{
    AutoPtr<IAnimationParameters> temp;
    params->GetLayoutAnimationParameters((IAnimationParameters**)&temp);
    AutoPtr<IGridLayoutAnimationParameters> animationParams = IGridLayoutAnimationParameters::Probe(temp);

    if(animationParams == NULL) {
        animationParams = new GridLayoutAnimationController::GridLayoutAnimationParameters();
        params->SetLayoutAnimationParameters(IAnimationParameters::Probe(animationParams));
    }

    IAnimationParameters::Probe(animationParams)->SetCount(count);
    IAnimationParameters::Probe(animationParams)->SetIndex(index);
    animationParams->SetColumnsCount(mNumColumns);
    animationParams->SetRowsCount(count / mNumColumns);

    if(!mStackFromBottom) {
        animationParams->SetColumn(index % mNumColumns);
        animationParams->SetRow(index / mNumColumns);
    } else {
        Int32 invertedIndex = count - 1 - index;

        animationParams->SetColumn(mNumColumns - 1 - (invertedIndex & mNumColumns));
        Int32 rowCount = 0;
        animationParams->GetRowsCount(&rowCount);
        animationParams->SetRow(rowCount - 1 - invertedIndex / mNumColumns);
    }

    return NOERROR;
}

ECode GridView::LayoutChildren()
{
    Boolean blockLayoutRequests = mBlockLayoutRequests;
    if (!blockLayoutRequests) {
        mBlockLayoutRequests = TRUE;
    }

//try {
    AbsListView::LayoutChildren();

    Invalidate();

    if (mAdapter == NULL) {
        ResetList();
        InvokeOnItemScrollListener();
        if (!blockLayoutRequests) {
            mBlockLayoutRequests = FALSE;
        }
        return NOERROR;
    }

    Int32 childrenTop = ListPaddingTop();
    Int32 childrenBottom = mBottom - mTop - ListPaddingBottom();

    Int32 childCount;
    GetChildCount(&childCount);
    Int32 index;
    Int32 delta = 0;

    AutoPtr<IView> sel;
    AutoPtr<IView> oldSel = NULL;
    AutoPtr<IView> oldFirst = NULL;
    AutoPtr<IView> newSel = NULL;

    switch (mLayoutMode) {
    case LAYOUT_SET_SELECTION:
        index = mNextSelectedPosition - mFirstPosition;
        if (index >= 0 && index < childCount) {
            GetChildAt(index, (IView**)&newSel);
        }
        break;
    case LAYOUT_FORCE_TOP:
    case LAYOUT_FORCE_BOTTOM:
    case LAYOUT_SPECIFIC:
    case LAYOUT_SYNC:
        break;
    case LAYOUT_MOVE_SELECTION:
        if (mNextSelectedPosition >= 0) {
            delta = mNextSelectedPosition - mSelectedPosition;
        }
        break;
    default:
        index = mSelectedPosition - mFirstPosition;
        if (index >= 0 && index < childCount) {
            GetChildAt(index, (IView**)&oldSel);
        }

        GetChildAt(0, (IView**)&oldFirst);
    }

    Boolean dataChanged = mDataChanged;
    if (dataChanged) {
        HandleDataChanged();
    }

    if (mItemCount == 0) {
        ResetList();
        InvokeOnItemScrollListener();
        if (!blockLayoutRequests) {
            mBlockLayoutRequests = FALSE;
        }
        return NOERROR;
    }

    SetSelectedPositionInt(mNextSelectedPosition);

    AutoPtr<IAccessibilityNodeInfo> accessibilityFocusLayoutRestoreNode;
    AutoPtr<IView> accessibilityFocusLayoutRestoreView;
    Int32 accessibilityFocusPosition = INVALID_POSITION;

    // Remember which child, if any, had accessibility focus. This must
    // occur before recycling any views, since that will clear
    // accessibility focus.
    AutoPtr<IViewRootImpl> viewRootImpl;
    GetViewRootImpl((IViewRootImpl**)&viewRootImpl);

    if (viewRootImpl != NULL) {
        AutoPtr<IView> focusHost;
        viewRootImpl->GetAccessibilityFocusedHost((IView**)&focusHost);
        if (focusHost != NULL) {
            AutoPtr<IView> focusChild = GetAccessibilityFocusedChild(focusHost);
            if (focusChild != NULL) {
                Boolean hasTransientState;
                if (!dataChanged || (focusChild->HasTransientState(&hasTransientState), hasTransientState)
                        || mAdapterHasStableIds) {
                    // The views won't be changing, so try to maintain
                    // focus on the current host and virtual view.
                    accessibilityFocusLayoutRestoreView = focusHost;
                    viewRootImpl->GetAccessibilityFocusedVirtualView(
                        (IAccessibilityNodeInfo**)&accessibilityFocusLayoutRestoreNode);
                }

                // Try to maintain focus at the same position.
                GetPositionForView(focusChild, &accessibilityFocusPosition);
            }
        }
    }

    Int32 firstPosition = mFirstPosition;
    RecycleBin* recycleBin = mRecycler;

    if (dataChanged) {
        AutoPtr<IView> tempChild;
        for (Int32 i = 0; i < childCount; i++) {
            tempChild = NULL;
            GetChildAt(i, (IView**)&tempChild);
            recycleBin->AddScrapView(tempChild, firstPosition + 1);
        }
    } else {
        recycleBin->FillActiveViews(childCount, firstPosition);
    }

    DetachAllViewsFromParent();

    Int32 top = 0;
    switch (mLayoutMode) {
    case LAYOUT_SET_SELECTION:
        if (newSel != NULL) {
            newSel->GetTop(&top);
            sel = FillFromSelection(top, childrenTop, childrenBottom);
        } else {
            sel = FillSelection(childrenTop, childrenBottom);
        }
        break;
    case LAYOUT_FORCE_TOP:
        mFirstPosition = 0;
        sel = FillFromTop(childrenTop);
        AdjustViewsUpOrDown();
        break;
    case LAYOUT_FORCE_BOTTOM:
        sel = FillUp(mItemCount - 1, childrenBottom);
        AdjustViewsUpOrDown();
        break;
    case LAYOUT_SPECIFIC:
        sel = FillSpecific(mSelectedPosition, mSpecificTop);
        break;
    case LAYOUT_SYNC:
        sel = FillSpecific(mSyncPosition, mSpecificTop);
        break;
    case LAYOUT_MOVE_SELECTION:
        // Move the selection relative to its old position
        sel = MoveSelection(delta, childrenTop, childrenBottom);
        break;
    default:
        if (childCount == 0) {
            Boolean isInTouchMode;
            if (!mStackFromBottom) {
                SetSelectedPositionInt(mAdapter == NULL || (IsInTouchMode(&isInTouchMode), isInTouchMode) ?
                        IAdapterView::INVALID_POSITION : 0);
                sel = FillFromTop(childrenTop);
            } else {
                Int32 last = mItemCount - 1;
                SetSelectedPositionInt(mAdapter == NULL || (IsInTouchMode(&isInTouchMode), isInTouchMode) ?
                        IAdapterView::INVALID_POSITION : last);
                sel = FillFromBottom(last, childrenBottom);
            }
        } else {
            if (mSelectedPosition >= 0 && mSelectedPosition < mItemCount) {
                sel = FillSpecific(mSelectedPosition, oldSel == NULL ?
                        childrenTop : (oldSel->GetTop(&top), top));
            } else if (mFirstPosition < mItemCount)  {
                sel = FillSpecific(mFirstPosition, oldFirst == NULL ?
                        childrenTop : (oldFirst->GetTop(&top), top));
            } else {
                sel = FillSpecific(0, childrenTop);
            }
        }
        break;
    }

    recycleBin->ScrapActiveViews();

    if (sel != NULL) {
       PositionSelector(INVALID_POSITION, sel);
       sel->GetTop(&mSelectedTop);
    } else {
        Boolean inTouchMode = mTouchMode > TOUCH_MODE_DOWN
                && mTouchMode < TOUCH_MODE_SCROLL;
        if (inTouchMode) {
            // If the user's finger is down, select the motion position.
            AutoPtr<IView> child;
            GetChildAt(mMotionPosition - mFirstPosition, (IView**)&child);
            if (child != NULL) {
                PositionSelector(mMotionPosition, child);
            }
        } else if (mSelectedPosition != INVALID_POSITION) {
            // If we had previously positioned the selector somewhere,
            // put it back there. It might not match up with the data,
            // but it's transitioning out so it's not a big deal.
            AutoPtr<IView> child;
            GetChildAt(mSelectorPosition - mFirstPosition, (IView**)&child);
            if (child != NULL) {
                PositionSelector(mSelectorPosition, child);
            }
        } else {
            // Otherwise, clear selection.
            mSelectedTop = 0;
            mSelectorRect->SetEmpty();
        }
    }

    // Attempt to restore accessibility focus, if necessary.
    if (viewRootImpl != NULL) {
        AutoPtr<IView> newAccessibilityFocusedView;
        viewRootImpl->GetAccessibilityFocusedHost((IView**)&newAccessibilityFocusedView);
        if (newAccessibilityFocusedView == NULL) {
            Boolean isAttachedToWindow;
            if (accessibilityFocusLayoutRestoreView != NULL &&
                (accessibilityFocusLayoutRestoreView->IsAttachedToWindow(&isAttachedToWindow), isAttachedToWindow)) {
                AutoPtr<IAccessibilityNodeProvider> provider;
                accessibilityFocusLayoutRestoreView->GetAccessibilityNodeProvider(
                    (IAccessibilityNodeProvider**)&provider);
                if (accessibilityFocusLayoutRestoreNode != NULL && provider != NULL) {
                    Int64 sourceId;
                    accessibilityFocusLayoutRestoreNode->GetSourceNodeId(&sourceId);
                    Int32 virtualViewId = CAccessibilityNodeInfo::GetVirtualDescendantId(sourceId);
                    Boolean isFormAction;
                    provider->PerformAction(virtualViewId,
                            IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &isFormAction);
                } else {
                    Boolean isFocus;
                    accessibilityFocusLayoutRestoreView->RequestAccessibilityFocus(&isFocus);
                }
            } else if (accessibilityFocusPosition != INVALID_POSITION) {
                // Bound the position within the visible children.
                Int32 childCount;
                GetChildCount(&childCount);
                Int32 position = MathUtils::Constrain(
                        accessibilityFocusPosition - mFirstPosition, 0, childCount - 1);
                AutoPtr<IView> restoreView;
                GetChildAt(position, (IView**)&restoreView);
                if (restoreView != NULL) {
                    Boolean isFocus;
                    restoreView->RequestAccessibilityFocus(&isFocus);
                }
            }
        }
    }

    mLayoutMode = LAYOUT_NORMAL;
    mDataChanged = FALSE;
    mNeedSync = FALSE;
    SetNextSelectedPositionInt(mSelectedPosition);

    UpdateScrollIndicators();

    if (mItemCount > 0) {
        CheckSelectionChanged();
    }

    InvokeOnItemScrollListener();
//} finally {
    if (!blockLayoutRequests) {
        mBlockLayoutRequests = FALSE;
    }
//}

    return NOERROR;
}

AutoPtr<IView> GridView::MakeAndAddView(
    /* [in] */ Int32 position,
    /* [in] */ Int32 y,
    /* [in] */ Boolean flow,
    /* [in] */ Int32 childrenLeft,
    /* [in] */ Boolean selected,
    /* [in] */ Int32 where)
{
    AutoPtr<IView> child;

    if (!mDataChanged) {
        child = mRecycler->GetActiveView(position);
        if (child != NULL) {
            SetupChild(child, position, y, flow, childrenLeft, selected, TRUE, where);
            return child;
        }
    }

    child = ObtainView(position, mIsScrap);

    SetupChild(child, position, y, flow, childrenLeft, selected, (*mIsScrap)[0], where);

    return child;
}

void GridView::SetupChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 position,
    /* [in] */ Int32 y,
    /* [in] */ Boolean flow,
    /* [in] */ Int32 childrenLeft,
    /* [in] */ Boolean selected,
    /* [in] */ Boolean recycled,
    /* [in] */ Int32 where)
{
    Boolean isSelected = selected && ShouldShowSelector();

    Boolean sel;
    child->IsSelected(&sel);

    Boolean updateChildSelected = isSelected != sel;
    Int32 mode = mTouchMode;
    Boolean isPressed = mode > TOUCH_MODE_DOWN && mode < TOUCH_MODE_SCROLL &&
            mMotionPosition == position;

    Boolean press;
    child->IsPressed(&press);
    Boolean updateChildPressed = isPressed != press;

    Boolean layoutRequested;
    child->IsLayoutRequested(&layoutRequested);
    Boolean needToMeasure = !recycled || updateChildSelected || layoutRequested;

    // Respect layout params that are already in the view. Otherwise make
    // some up...
    AutoPtr<IViewGroupLayoutParams> vglp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    AutoPtr<IAbsListViewLayoutParams> p = IAbsListViewLayoutParams::Probe(vglp);
    if (p == NULL) {
        CAbsListViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, 0, (IAbsListViewLayoutParams**)&p);
    }
    IAdapter::Probe(mAdapter)->GetItemViewType(position, &((CAbsListViewLayoutParams*)p.Get())->mViewType);

    if (recycled && !((CAbsListViewLayoutParams*)p.Get())->mForceAdd) {
        AttachViewToParent(child, where, IViewGroupLayoutParams::Probe(p));
    } else {
        ((CAbsListViewLayoutParams*)p.Get())->mForceAdd = FALSE;
        AddViewInLayout(child, where, IViewGroupLayoutParams::Probe(p), TRUE);
    }

    if (updateChildSelected) {
        child->SetSelected(isSelected);
        if (isSelected) {
            Boolean isRequestFocus;
            View::RequestFocus(&isRequestFocus);
        }
    }

    if (updateChildPressed) {
        child->SetPressed(isPressed);
    }

    if (needToMeasure) {
        Int32 childHeightSpec = ViewGroup::GetChildMeasureSpec(
            MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED), 0, ((CAbsListViewLayoutParams*)p.Get())->mHeight);

        Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(
            MeasureSpec::MakeMeasureSpec(mColumnWidth, MeasureSpec::EXACTLY), 0, ((CAbsListViewLayoutParams*)p.Get())->mWidth);
        child->Measure(childWidthSpec, childHeightSpec);
    } else {
        CleanupLayoutState(child);
    }

    Int32 w;
    child->GetMeasuredWidth(&w);
    Int32 h;
    child->GetMeasuredHeight(&h);

    Int32 childLeft;
    Int32 childTop = flow ? y : y - h;

    switch (mGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
    case IGravity::LEFT:
        childLeft = childrenLeft;
        break;
    case IGravity::CENTER_HORIZONTAL:
        childLeft = childrenLeft + ((mColumnWidth - w) / 2);
        break;
    case IGravity::RIGHT:
        childLeft = childrenLeft + mColumnWidth - w;
        break;
    default:
        childLeft = childrenLeft;
        break;
    }

    if (needToMeasure) {
        Int32 childRight = childLeft + w;
        Int32 childBottom = childTop + h;
        child->Layout(childLeft, childTop, childRight, childBottom);
    } else {
        Int32 left, top;
        child->GetLeft(&left);
        child->GetTop(&top);
        child->OffsetLeftAndRight(childLeft - left);
        child->OffsetTopAndBottom(childTop - top);
    }

    if (mCachingStarted) {
        child->SetDrawingCacheEnabled(TRUE);
    }
}

Boolean GridView::SequenceScroll(
    /* [in] */ Int32 direction)
{
    return FALSE;
}

ECode GridView::SetSelection(
    /* [in] */ Int32 position)
{
    Boolean isInTouchMode;
    if (IsInTouchMode(&isInTouchMode), !isInTouchMode) {
        SetNextSelectedPositionInt(position);
    } else {
        mResurrectToPosition = position;
    }
    mLayoutMode = LAYOUT_SET_SELECTION;
    RequestLayout();

    return NOERROR;
}

ECode GridView::SetSelectionInt(
    /* [in] */ Int32 position)
{
    Int32 previousSelectedPosition = mNextSelectedPosition;

    SetNextSelectedPositionInt(position);
    LayoutChildren();

    Int32 next = mStackFromBottom ? mItemCount - 1  - mNextSelectedPosition :
        mNextSelectedPosition;
    Int32 previous = mStackFromBottom ? mItemCount - 1
            - previousSelectedPosition : previousSelectedPosition;

    Int32 nextRow = next / mNumColumns;
    Int32 previousRow = previous / mNumColumns;

    if (nextRow != previousRow) {
        AwakenScrollBars();
    }
    return NOERROR;
}

ECode GridView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CommonKey(keyCode, 1, event);
    return NOERROR;
}

ECode GridView::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CommonKey(keyCode, repeatCount, event);
    return NOERROR;
}

ECode GridView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CommonKey(keyCode, 1, event);
    return NOERROR;
}

Boolean GridView::CommonKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event)
{
    if (mAdapter == NULL) {
        return FALSE;
    }

    if (mDataChanged) {
        LayoutChildren();
    }

    Boolean handled = FALSE;
    Int32 action;
    event->GetAction(&action);

    if (action != IKeyEvent::ACTION_UP) {
        if (mSelectedPosition < 0) {
            switch (keyCode) {
                case IKeyEvent::KEYCODE_DPAD_UP:
                case IKeyEvent::KEYCODE_DPAD_DOWN:
                case IKeyEvent::KEYCODE_DPAD_LEFT:
                case IKeyEvent::KEYCODE_DPAD_RIGHT:
                case IKeyEvent::KEYCODE_DPAD_CENTER:
                case IKeyEvent::KEYCODE_SPACE:
                case IKeyEvent::KEYCODE_ENTER:
                    ResurrectSelection();
                    return TRUE;
            }
        }

        Boolean press;
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_LEFT:
                handled = ArrowScroll(IView::FOCUS_LEFT);
                break;

            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                handled = ArrowScroll(IView::FOCUS_RIGHT);
                break;

            case IKeyEvent::KEYCODE_DPAD_UP:

                event->IsAltPressed(&press);
                if (!press) {
                    handled = ArrowScroll(IView::FOCUS_UP);

                } else {
                    handled = FullScroll(IView::FOCUS_UP);
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_DOWN:
                event->IsAltPressed(&press);
                if (!press) {
                    handled = ArrowScroll(IView::FOCUS_DOWN);
                } else {
                    handled = FullScroll(IView::FOCUS_DOWN);
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_CENTER:
            case IKeyEvent::KEYCODE_ENTER: {
                Int32 count, childCount;
                event->GetRepeatCount(&count);
                if ((GetChildCount(&childCount), childCount) > 0 && count == 0) {
                    KeyPressed();
                }

                return TRUE;
            }

            case IKeyEvent::KEYCODE_SPACE:
                Boolean show = FALSE;
                if (mPopup == NULL || !(mPopup->IsShowing(&show), show)) {
                    event->IsShiftPressed(&press);
                    if (!press) {
                        handled = PageScroll(IView::FOCUS_DOWN);
                    } else {
                        handled = PageScroll(IView::FOCUS_UP);
                    }
                }
                break;
        }
    }

    if (!handled) {
        handled = SendToTextFilter(keyCode, count, event);
    }

    if (handled) {
        return TRUE;
    } else {
        switch (action) {
            case IKeyEvent::ACTION_DOWN:
            {
                Boolean result;
                AbsListView::OnKeyDown(keyCode, event, &result);
                return result;
            }
            case IKeyEvent::ACTION_UP:
            {
                Boolean result;
                AbsListView::OnKeyUp(keyCode, event, &result);
                return result;
            }
            case IKeyEvent::ACTION_MULTIPLE:
            {
                Boolean result;
                AbsListView::OnKeyMultiple(keyCode, count, event, &result);
                return result;
            }
            default:
                return FALSE;
        }
    }
    return FALSE;
}

Boolean GridView::PageScroll(
    /* [in] */ Int32 direction)
{
    Int32 nextPage = -1;

    Int32 childCount;
    GetChildCount(&childCount);
    if (direction == IView::FOCUS_UP) {
        nextPage = Elastos::Core::Math::Max(0, mSelectedPosition - childCount - 1);
    } else if (direction == IView::FOCUS_DOWN) {
        nextPage = Elastos::Core::Math::Min(mItemCount - 1, mSelectedPosition + childCount - 1);
    }

    if (nextPage >= 0) {
        SetSelectionInt(nextPage);
        InvokeOnItemScrollListener();
        AwakenScrollBars();
        return TRUE;
    }

    return FALSE;
}

Boolean GridView::FullScroll(
    /* [in] */ Int32 direction)
{
    Boolean moved = FALSE;
    if (direction == IView::FOCUS_UP) {
        mLayoutMode = LAYOUT_SET_SELECTION;
        SetSelectionInt(0);
        InvokeOnItemScrollListener();
        moved = TRUE;
    } else if (direction == IView::FOCUS_DOWN) {
        mLayoutMode = LAYOUT_SET_SELECTION;
        SetSelectionInt(mItemCount - 1);
        InvokeOnItemScrollListener();
        moved = TRUE;
    }

    if (moved) {
        AwakenScrollBars();
    }

    return moved;
}

Boolean GridView::ArrowScroll(
    /* [in] */ Int32 direction)
{
    Int32 selectedPosition = mSelectedPosition;
    Int32 numColumns = mNumColumns;

    Int32 startOfRowPos;
    Int32 endOfRowPos;

    Boolean moved = FALSE;

    if (!mStackFromBottom) {
        startOfRowPos = (selectedPosition / numColumns) * numColumns;
        endOfRowPos = Elastos::Core::Math::Min(startOfRowPos + numColumns - 1, mItemCount - 1);
    } else {
        Int32 invertedSelection = mItemCount - 1 - selectedPosition;
        endOfRowPos = mItemCount - 1 - (invertedSelection / numColumns) * numColumns;
        startOfRowPos = Elastos::Core::Math::Max(0, endOfRowPos - numColumns + 1);
    }

    switch (direction) {
        case IView::FOCUS_UP:
            if (startOfRowPos > 0) {
                mLayoutMode = LAYOUT_MOVE_SELECTION;
                SetSelectionInt(Elastos::Core::Math::Max(0, selectedPosition - numColumns));
                moved = TRUE;
            }
            break;
        case IView::FOCUS_DOWN:
            if (endOfRowPos < mItemCount - 1) {
                mLayoutMode = LAYOUT_MOVE_SELECTION;
                SetSelectionInt(Elastos::Core::Math::Min(selectedPosition + numColumns, mItemCount - 1));
                moved = TRUE;
            }
            break;
        case IView::FOCUS_LEFT:
            if (selectedPosition > startOfRowPos) {
                mLayoutMode = LAYOUT_MOVE_SELECTION;
                SetSelectionInt(Elastos::Core::Math::Max(0, selectedPosition - 1));
                moved = TRUE;
            }
            break;
        case IView::FOCUS_RIGHT:
            if (selectedPosition < endOfRowPos) {
                mLayoutMode = LAYOUT_MOVE_SELECTION;
                SetSelectionInt(Elastos::Core::Math::Min(selectedPosition + 1, mItemCount - 1));
                moved = TRUE;
            }
            break;
    }

    if (moved) {
        PlaySoundEffect(SoundEffectConstants::GetContantForFocusDirection(direction));
        InvokeOnItemScrollListener();
    }

    if (moved) {
        AwakenScrollBars();
    }

    return moved;
}

void GridView::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AbsListView::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);

    Int32 closestChildIndex = -1;
    if (gainFocus && previouslyFocusedRect != NULL) {
        previouslyFocusedRect->Offset(mScrollX, mScrollY);

        // figure out which item should be selected based on previously
        // focused rect
        AutoPtr<CRect> otherRect = (CRect*)mTempRect.Get();
        Int32 minDistance = Elastos::Core::Math::INT32_MAX_VALUE;
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            // only consider view's on appropriate edge of grid
            if (!IsCandidateSelection(i, direction)) {
                continue;
            }

            AutoPtr<IView> other;
            GetChildAt(i, (IView**)&other);
            other->GetDrawingRect(otherRect);
            OffsetDescendantRectToMyCoords(other, otherRect);
            Int32 distance;
            GetDistance(previouslyFocusedRect, otherRect, direction, &distance);

            if (distance < minDistance) {
                minDistance = distance;
                closestChildIndex = i;
            }
        }
    }

    if (closestChildIndex >= 0) {
        SetSelection(closestChildIndex + mFirstPosition);
    } else {
        RequestLayout();
    }
}

Boolean GridView::IsCandidateSelection(
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 direction)
{
    Int32 count;
    GetChildCount(&count);
    Int32 invertedIndex = count - 1 - childIndex;

    Int32 rowStart;
    Int32 rowEnd;

    if (!mStackFromBottom) {
        rowStart = childIndex - (childIndex % mNumColumns);
        rowEnd = Elastos::Core::Math::Max(rowStart + mNumColumns - 1, count);
    } else {
        rowEnd = count - 1 - (invertedIndex - (invertedIndex % mNumColumns));
        rowStart = Elastos::Core::Math::Max(0, rowEnd - mNumColumns + 1);
    }

    switch (direction) {
        case IView::FOCUS_RIGHT:
            return childIndex == rowStart;
        case IView::FOCUS_DOWN:
            return rowStart == 0;
        case IView::FOCUS_LEFT:
            return childIndex == rowEnd;
        case IView::FOCUS_UP:
            return rowEnd == count - 1;
        default:
            break;
            //throw new IllegalArgumentException("direction must be one of "
            //        + "{FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, FOCUS_RIGHT}.");
    }
    return FALSE;
}

ECode GridView::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mGravity != gravity) {
        mGravity = gravity;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode GridView::SetHorizontalSpacing(
    /* [in] */ Int32 horizontalSpacing)
{
    if (horizontalSpacing != mRequestedHorizontalSpacing) {
        mRequestedHorizontalSpacing = horizontalSpacing;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode GridView::SetVerticalSpacing(
    /* [in] */ Int32 verticalSpacing)
{
    if (verticalSpacing != mVerticalSpacing) {
        mVerticalSpacing = verticalSpacing;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode GridView::SetStretchMode(
    /* [in] */ Int32 stretchMode)
{
    if (stretchMode != mStretchMode) {
        mStretchMode = stretchMode;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

Int32 GridView::GetStretchMode()
{
    return mStretchMode;
}

ECode GridView::GetStretchMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = GetStretchMode();
    return NOERROR;
}


ECode GridView::SetColumnWidth(
    /* [in] */ Int32 columnWidth)
{
    if (columnWidth != mRequestedColumnWidth) {
        mRequestedColumnWidth = columnWidth;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode GridView::SetNumColumns(Int32 numColumns)
{
    if (numColumns != mRequestedNumColumns) {
        mRequestedNumColumns = numColumns;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode GridView::AdjustViewsUpOrDown()
{
    Int32 childCount;
    GetChildCount(&childCount);

    if (childCount > 0) {
        Int32 delta;
        AutoPtr<IView> child;

        if (!mStackFromBottom) {
            // Uh-oh -- we came up short. Slide all views up to make them
            // align with the top
            GetChildAt(0, (IView**)&child);
            Int32 top = 0;
            child->GetTop(&top);
            delta = top - ListPaddingTop();
            if (mFirstPosition != 0) {
                // It's OK to have some space above the first item if it is
                // part of the vertical spacing
                delta -= mVerticalSpacing;
            }
            if (delta < 0) {
                // We only are looking to see if we are too low, not too high
                delta = 0;
            }
        } else {
            // we are too high, slide all views down to align with bottom
            GetChildAt(childCount - 1, (IView**)&child);
            Int32 bottom, height;
            child->GetBottom(&bottom);
            GetHeight(&height);
            delta = bottom - (height - ListPaddingBottom());

            if (mFirstPosition + childCount < mItemCount) {
                // It's OK to have some space below the last item if it is
                // part of the vertical spacing
                delta += mVerticalSpacing;
            }

            if (delta > 0) {
                // We only are looking to see if we are too high, not too low
                delta = 0;
            }
        }

        if (delta != 0) {
            OffsetChildrenTopAndBottom(-delta);
        }
    }

    return NOERROR;
}

Int32 GridView::ComputeVerticalScrollExtent()
{
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        Int32 numColumns = mNumColumns;
        Int32 rowCount = (count + numColumns - 1) / numColumns;

        Int32 extent = rowCount * 100;

        AutoPtr<IView> view;
        GetChildAt(0, (IView**)&view);
        Int32 top;
        view->GetTop(&top);
        Int32 height;
        view->GetHeight(&height);
        if (height > 0) {
            extent += (top * 100) / height;
        }
        view = NULL;
        GetChildAt(count - 1, (IView**)&view);
        Int32 bottom;
        view->GetBottom(&bottom);
        view->GetHeight(&height);
        if (height > 0) {
            Int32 thisHeight;
            GetHeight(&thisHeight);
            extent -= ((bottom - thisHeight) * 100) / height;
        }

        return extent;
    }
    return 0;
}

Int32 GridView::ComputeVerticalScrollOffset()
{
    Int32 childCount;
    if (mFirstPosition >= 0 && (GetChildCount(&childCount), childCount) > 0) {
        AutoPtr<IView> view;
        GetChildAt(0, (IView**)&view);
        Int32 top;
        view->GetTop(&top);
        Int32 height;
        view->GetHeight(&height);
        if (height > 0) {
            Int32 numColumns = mNumColumns;
            Int32 whichRow = mFirstPosition / numColumns;
            Int32 rowCount = (mItemCount + numColumns - 1) / numColumns;
            Int32 thisHeight;
            GetHeight(&thisHeight);
            return Elastos::Core::Math::Max(whichRow * 100 - (top * 100) / height +
                    (Int32) ((Float) mScrollY / thisHeight * rowCount * 100), 0);
        }
    }
    return 0;
}

Int32 GridView::ComputeVerticalScrollRange()
{
    // TODO: Account for vertical spacing too
    Int32 numColumns = mNumColumns;
    Int32 rowCount = (mItemCount + numColumns - 1) / numColumns;
    Int32 result = Elastos::Core::Math::Max(rowCount * 100, 0);
    if (mScrollY != 0) {
        // Compensate for overscroll
        Int32 height;
        GetHeight(&height);
        result += Elastos::Core::Math::Abs((Int32) ((Float) mScrollY / height * rowCount * 100));
    }
    return result;
}

Int32 GridView::ListPaddingLeft()
{
    Int32 res;
    mListPadding->GetLeft(&res);
    return res;
}

Int32 GridView::ListPaddingTop()
{
    Int32 res;
    mListPadding->GetTop(&res);
    return res;
}

Int32 GridView::ListPaddingRight()
{
    Int32 res;
    mListPadding->GetRight(&res);
    return res;
}

Int32 GridView::ListPaddingBottom()
{
    Int32 res;
    mListPadding->GetBottom(&res);
    return res;
}

//@Override
ECode GridView::OnInitializeAccessibilityNodeInfoForItem(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsListView::OnInitializeAccessibilityNodeInfoForItem(view, position, info);

    Int32 count;
    GetCount(&count);
    Int32 columnsCount = GetNumColumns();
    Int32 rowsCount = count / columnsCount;

    Int32 row;
    Int32 column;
    if (!mStackFromBottom) {
        column = position % columnsCount;
        row = position / columnsCount;
    } else {
        Int32 invertedIndex = count - 1 - position;

        column = columnsCount - 1 - (invertedIndex % columnsCount);
        row = rowsCount - 1 - invertedIndex / columnsCount;
    }

    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 viewType;
    IAbsListViewLayoutParams::Probe(lp)->GetViewType(&viewType);
    Boolean isHeading = lp != NULL && viewType != ITEM_VIEW_TYPE_HEADER_OR_FOOTER;
    Boolean isSelected;
    IsItemChecked(position, &isSelected);

    AutoPtr<IAccessibilityNodeInfoCollectionItemInfo> itemInfo;
    AccessibilityNodeInfoCollectionItemInfo::Obtain(row, 1, column, 1, isHeading, isSelected,
        (IAccessibilityNodeInfoCollectionItemInfo**)&itemInfo);
    info->SetCollectionItemInfo(itemInfo);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

