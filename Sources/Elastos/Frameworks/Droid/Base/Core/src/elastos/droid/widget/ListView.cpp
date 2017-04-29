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
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionItemInfo.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/view/FocusFinder.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/widget/CArrayAdapter.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/widget/CHeaderViewListAdapter.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::EIID_IApplicationInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionItemInfo;
using Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoCollectionItemInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoCollectionInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewGroupLayoutParams;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::FocusFinder;
using Elastos::Droid::View::ViewRootImpl;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CAbsListViewLayoutParams;
using Elastos::Droid::Widget::CHeaderViewListAdapter;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

#ifndef TO_CRECT
#define TO_CRECT(obj) ((CRect*)obj.Get())
#endif

const Int32 ListView::NO_POSITION = -1;
const Float ListView::MAX_SCROLL_FACTOR = 0.33f;
const Int32 ListView::MIN_SCROLL_PREVIEW_PIXELS = 2;
const String ListView::LISTVIEW_NAME("ListView");

//==============================================================================
//            ListView::FixedViewInfo
//==============================================================================

CAR_INTERFACE_IMPL(ListView::FixedViewInfo, Object, IFixedViewInfo);

ListView::FixedViewInfo::FixedViewInfo()
    : mIsSelectable(FALSE)
{
}

ECode ListView::FixedViewInfo::SetView(
    /* [in] */ IView* view)
{
    mView = view;
    return NOERROR;
}

ECode ListView::FixedViewInfo::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode ListView::FixedViewInfo::SetData(
    /* [in] */ IInterface* data)
{
    mData = data;
    return NOERROR;
}

ECode ListView::FixedViewInfo::GetData(
    /* [out] */ IInterface** data)
{
    VALIDATE_NOT_NULL(data);
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode ListView::FixedViewInfo::SetSelectable(
    /* [in] */ Boolean selectable)
{
    mIsSelectable = selectable;
    return NOERROR;
}

ECode ListView::FixedViewInfo::GetSelectable(
    /* [out] */ Boolean* selectable)
{
    VALIDATE_NOT_NULL(selectable);
    *selectable = mIsSelectable;
    return NOERROR;
}

//==============================================================================
//            ListView::FocusSelector
//==============================================================================

ListView::FocusSelector::FocusSelector(
    /* [in] */ ListView* host)
    : mHost(host)
    , mPosition(0)
    , mPositionTop(0)
{
}

AutoPtr<ListView::FocusSelector> ListView::FocusSelector::Setup(
    /* [in] */ Int32 position,
    /* [in] */ Int32 top)
{
    mPosition = position;
    mPositionTop = top;
    return this;
}

ECode ListView::FocusSelector::Run()
{
    mHost->SetSelectionFromTop(mPosition, mPositionTop);
    return NOERROR;
}

//==============================================================================
//            ListView::ArrowScrollFocusResult
//==============================================================================
ListView::ArrowScrollFocusResult::ArrowScrollFocusResult()
    : mSelectedPosition(0)
    , mAmountToScroll(0)
{}

Int32 ListView::ArrowScrollFocusResult::GetSelectedPosition()
{
    return mSelectedPosition;
}

Int32 ListView::ArrowScrollFocusResult::GetAmountToScroll()
{
    return mAmountToScroll;
}

void ListView::ArrowScrollFocusResult::Populate(
    /* [in] */ Int32 selectedPosition,
    /* [in] */ Int32 amountToScroll)
{
    mSelectedPosition = selectedPosition;
    mAmountToScroll = amountToScroll;
}

//==============================================================================
//            ListView
//==============================================================================

CAR_INTERFACE_IMPL(ListView, AbsListView, IListView)

ListView::ListView()
    : mDividerHeight(0)
    , mIsCacheColorOpaque(FALSE)
    , mDividerIsOpaque(FALSE)
    , mHeaderDividersEnabled(FALSE)
    , mFooterDividersEnabled(FALSE)
    , mAreAllItemsSelectable(TRUE)
    , mItemsCanFocus(FALSE)
{
    CArrayList::New((IArrayList**)&mHeaderViewInfos);
    CArrayList::New((IArrayList**)&mFooterViewInfos);
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
    mArrowScrollFocusResult = new ArrowScrollFocusResult();
}

ListView::~ListView()
{
    Int32 size;
    mHeaderViewInfos->GetSize(&size);
    if (size > 0) {
        mHeaderViewInfos->Clear();
    }
    mFooterViewInfos->GetSize(&size);
    if (size > 0) {
        mFooterViewInfos->Clear();
    }
}

ECode ListView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::listViewStyle);
}

ECode ListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AbsListView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ListView);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    AutoPtr< ArrayOf<ICharSequence*> > entries;
    a->GetTextArray(R::styleable::ListView_entries, (ArrayOf<ICharSequence*>**)&entries);
    if (entries != NULL) {
        Int32 count = entries->GetLength();
        AutoPtr< ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(count);
        for (Int32 i = 0; i < count; ++i) {
            objects->Set(i, (*entries)[i]);
        }
        AutoPtr<IAdapter> a;
        FAIL_RETURN(CArrayAdapter::New(context, R::layout::simple_list_item_1, objects, (IAdapter**)&a));
        SetAdapter(a);
    }

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::ListView_divider, (IDrawable**)&d);

    if (d != NULL) {
        // If a divider is specified use its intrinsic height for divider height
        SetDivider(d);
    }

    AutoPtr<IDrawable> osHeader;
    a->GetDrawable(R::styleable::ListView_overScrollHeader, (IDrawable**)&osHeader);
    if (osHeader != NULL) {
        SetOverscrollHeader(osHeader);
    }

    AutoPtr<IDrawable> osFooter;
    a->GetDrawable(R::styleable::ListView_overScrollFooter, (IDrawable**)&osFooter);
    if (osFooter != NULL) {
        SetOverscrollFooter(osFooter);
    }

    Int32 dividerHeight;
    a->GetDimensionPixelSize(R::styleable::ListView_dividerHeight, 0, &dividerHeight);
    if (dividerHeight != 0) {
        SetDividerHeight(dividerHeight);
    }

    a->GetBoolean(R::styleable::ListView_headerDividersEnabled, TRUE, &mHeaderDividersEnabled);
    a->GetBoolean(R::styleable::ListView_footerDividersEnabled, TRUE, &mFooterDividersEnabled);

    a->Recycle();

    return NOERROR;
}

ECode ListView::GetMaxScrollAmount(
    /* [out] */ Int32* amount)
{
    VALIDATE_NOT_NULL(amount);
    *amount = GetMaxScrollAmount();
    return NOERROR;
}

Int32 ListView::GetMaxScrollAmount()
{
    return (Int32)(MAX_SCROLL_FACTOR * (mBottom - mTop));
}

void ListView::AdjustViewsUpOrDown()
{
    Int32 childCount;
    GetChildCount(&childCount);
    Int32 delta;

    if (childCount > 0) {
        AutoPtr<IView> child;
        if (!mStackFromBottom) {
            GetChildAt(0, (IView**)&child);
            Int32 top;
            child->GetTop(&top);

            Int32 topTmp = 0;
            mListPadding->GetTop(&topTmp);
            delta = top - topTmp;
            if (mFirstPosition != 0) {
                delta -= mDividerHeight;
            }
            if (delta < 0) {
                delta = 0;
            }
        }
        else {
            GetChildAt(childCount - 1, (IView**)&child);
            Int32 bottom;
            child->GetBottom(&bottom);
            Int32 height;
            GetHeight(&height);

            Int32 bottomTmp = 0;
            mListPadding->GetBottom(&bottomTmp);
            delta = bottom - (height - bottomTmp);

            if (mFirstPosition + childCount < mItemCount) {
                delta += mDividerHeight;
            }

            if (delta > 0) {
                delta = 0;
            }
        }
        if (delta != 0) {
            OffsetChildrenTopAndBottom(-delta);
        }
    }
}

ECode ListView::AddHeaderView(
    /* [in] */ IView* v,
    /* [in] */ IInterface* data,
    /* [in] */ Boolean isSelectable)
{
    AutoPtr<FixedViewInfo> info = new FixedViewInfo();
    info->mView = v;
    info->mData = data;
    info->mIsSelectable = isSelectable;
    mHeaderViewInfos->Add((IFixedViewInfo*)info.Get());
    mAreAllItemsSelectable &= isSelectable;

    // Wrap the adapter if it wasn't already wrapped.
    if (mAdapter != NULL) {
        if (IHeaderViewListAdapter::Probe(mAdapter) == NULL) {
            AutoPtr<IListAdapter> adapter;
            CHeaderViewListAdapter::New(mHeaderViewInfos, mFooterViewInfos,
                    mAdapter, (IListAdapter**)&adapter);
            mAdapter = adapter;
        }

        // In the case of re-adding a header view, or adding one later on,
        // we need to notify the observer.
        if (mDataSetObserver != NULL) {
            mDataSetObserver->OnChanged();
        }
    }

    return NOERROR;
}

ECode ListView::AddHeaderView(
    /* [in] */ IView* v)
{
    return AddHeaderView(v, NULL, TRUE);
}

Int32 ListView::GetHeaderViewsCount()
{
    Int32 size;
    mHeaderViewInfos->GetSize(&size);
    return size;
}

ECode ListView::RemoveHeaderView(
    /* [in] */ IView* v,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 size;
    mHeaderViewInfos->GetSize(&size);
    if (size > 0) {
        Boolean result = FALSE;
        Boolean isRemoved;
        if (mAdapter != NULL
                && (IHeaderViewListAdapter::Probe(mAdapter)->RemoveHeader(v, &isRemoved), isRemoved)) {
            if (mDataSetObserver != NULL) {
                mDataSetObserver->OnChanged();
            }
            result = TRUE;
        }
        RemoveFixedViewInfo(v, mHeaderViewInfos);
        *res = result;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

void ListView::RemoveFixedViewInfo(
    /* [in] */ IView* v,
    /* [in] */ IArrayList* where)
{
    Int32 len;
    where->GetSize(&len);
    for (Int32 i = 0; i < len; ++i) {
        AutoPtr<IInterface> obj;
        where->Get(i, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        AutoPtr<IView> view;
        info->GetView((IView**)&view);
        if (view.Get() == v) {
            where->Remove(i);
            break;
        }
    }
}

ECode ListView::AddFooterView(
    /* [in] */ IView* v,
    /* [in] */ IInterface* data,
    /* [in] */ Boolean isSelectable)
{
    AutoPtr<FixedViewInfo> info = new FixedViewInfo();
    info->mView = v;
    info->mData = data;
    info->mIsSelectable = isSelectable;
    mFooterViewInfos->Add((IFixedViewInfo*)info.Get());
    mAreAllItemsSelectable &= isSelectable;

    // Wrap the adapter if it wasn't already wrapped.
    if (mAdapter != NULL) {
        if (IHeaderViewListAdapter::Probe(mAdapter) == NULL) {
            AutoPtr<IListAdapter> adapter;
            CHeaderViewListAdapter::New(mHeaderViewInfos, mFooterViewInfos,
                    mAdapter, (IListAdapter**)&adapter);
            mAdapter = adapter;
        }

        // In the case of re-adding a footer view, or adding one later on,
        // we need to notify the observer.
        if (mDataSetObserver != NULL) {
            mDataSetObserver->OnChanged();
        }
    }

    return NOERROR;
}

ECode ListView::AddFooterView(
    /* [in] */ IView* v)
{
    return AddFooterView(v, NULL, TRUE);
}

Int32 ListView::GetFooterViewsCount()
{
    Int32 size;
    mFooterViewInfos->GetSize(&size);
    return size;
}

ECode ListView::RemoveFooterView(
    /* [in] */ IView* v,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 size;
    mFooterViewInfos->GetSize(&size);
    if (size > 0) {
        Boolean result = FALSE;
        Boolean isRemoved;
        if (mAdapter != NULL
                && (IHeaderViewListAdapter::Probe(mAdapter)->RemoveFooter(v, &isRemoved), isRemoved)) {
            if (mDataSetObserver != NULL) {
                mDataSetObserver->OnChanged();
            }
            result = TRUE;
        }
        RemoveFixedViewInfo(v, mFooterViewInfos);
        *res = result;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode ListView::GetAdapter(
    /* [out] */ IAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    *adapter = IAdapter::Probe(mAdapter);
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

ECode ListView::SetRemoteViewsAdapter(
    /* [in] */ IIntent* intent)
{
    return AbsListView::SetRemoteViewsAdapter(intent);
}

ECode ListView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    if (mAdapter != NULL && mDataSetObserver != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(mDataSetObserver);
    }
    ResetList();
    mRecycler->Clear();

    Int32 size, size2;
    mHeaderViewInfos->GetSize(&size);
    mFooterViewInfos->GetSize(&size2);
    if (size > 0 || size2 > 0) {
        mAdapter = NULL;
        CHeaderViewListAdapter::New(mHeaderViewInfos, mFooterViewInfos,
                IListAdapter::Probe(adapter), (IListAdapter**)&mAdapter);
    }
    else {
        mAdapter = IListAdapter::Probe(adapter);
    }

    mOldSelectedPosition = IAdapterView::INVALID_POSITION;
    mOldSelectedRowId = IAdapterView::INVALID_ROW_ID;

    AbsListView::SetAdapter(adapter);

    if (mAdapter != NULL) {
        mAdapter->AreAllItemsEnabled(&mAreAllItemsSelectable);
        mOldItemCount = mItemCount;
        AutoPtr<IAdapter> _mAdapter = IAdapter::Probe(mAdapter);
        _mAdapter->GetCount(&mItemCount);
        CheckFocus();

        mDataSetObserver = new AdapterDataSetObserver(this);
        _mAdapter->RegisterDataSetObserver(mDataSetObserver);

        Int32 count;
        _mAdapter->GetViewTypeCount(&count);
        mRecycler->SetViewTypeCount(count);

        Int32 position;
        if (mStackFromBottom) {
            position = LookForSelectablePosition(mItemCount - 1, FALSE);
        }
        else {
            position = LookForSelectablePosition(0, TRUE);
        }

        SetSelectedPositionInt(position);
        SetNextSelectedPositionInt(position);

        if (mItemCount == 0) {
            CheckSelectionChanged();
        }
    }
    else {
        mAreAllItemsSelectable = TRUE;
        CheckFocus();
        CheckSelectionChanged();
    }
    RequestLayout();

    return NOERROR;
}

void ListView::ResetList()
{
    ClearRecycledState(mHeaderViewInfos);
    ClearRecycledState(mFooterViewInfos);

    AbsListView::ResetList();

    mLayoutMode = AbsListView::LAYOUT_NORMAL;
}

void ListView::ClearRecycledState(
    /* [in] */ IArrayList* infos)
{
    if (infos != NULL) {
        Int32 count;
        infos->GetSize(&count);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            infos->Get(i, (IInterface**)&obj);
            AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
            AutoPtr<IView> child;
            info->GetView((IView**)&child);
            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            AutoPtr<IAbsListViewLayoutParams> p = IAbsListViewLayoutParams::Probe(params);
            if (p != NULL) {
                p->SetRecycledHeaderFooter(FALSE);
            }
        }
    }
}

Boolean ListView::ShowingTopFadingEdge()
{
    Int32 topTmp = 0;
    mListPadding->GetTop(&topTmp);
    Int32 listTop = mScrollY + topTmp;
    Int32 top;
    AutoPtr<IView> view;
    GetChildAt(0, (IView**)&view);
    view->GetTop(&top);
    return (mFirstPosition > 0) || (top > listTop);
}

Boolean ListView::ShowingBottomFadingEdge()
{
    Int32 childCount;
    GetChildCount(&childCount);
    Int32 bottomOfBottomChild;
    AutoPtr<IView> view;
    GetChildAt(childCount - 1, (IView**)&view);
    view->GetBottom(&bottomOfBottomChild);
    Int32 lastVisiblePosition = mFirstPosition + childCount - 1;
    Int32 height;
    GetHeight(&height);

    Int32 bottomTmp = 0;
    mListPadding->GetBottom(&bottomTmp);
    Int32 listBottom = mScrollY + height - bottomTmp;

    return (lastVisiblePosition < mItemCount - 1) || (bottomOfBottomChild < listBottom);
}

ECode ListView::RequestChildRectangleOnScreen(
    /* [in] */ IView* childView,
    /* [in] */ IRect* r,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 rectTopWithinChild;
    AutoPtr<CRect> rect = (CRect*)r;
    r->GetTop(&rectTopWithinChild);
    Int32 left, top, scrollX, scrollY;
    childView->GetLeft(&left);
    childView->GetTop(&top);
    childView->GetScrollX(&scrollX);
    childView->GetScrollY(&scrollY);
    r->Offset(left, top);
    r->Offset(-scrollX, -scrollY);

    Int32 height;
    GetHeight(&height);
    Int32 listUnfadedTop;
    GetScrollY(&listUnfadedTop);
    Int32 listUnfadedBottom = listUnfadedTop + height;
    Int32 fadingEdge;
    GetVerticalFadingEdgeLength(&fadingEdge);

    if (ShowingBottomFadingEdge()) {
        if ((mSelectedPosition > 0) || (rectTopWithinChild > fadingEdge)) {
            listUnfadedTop += fadingEdge;
        }
    }

    Int32 childCount;
    GetChildCount(&childCount);
    AutoPtr<IView> child;
    GetChildAt(childCount - 1, (IView**)&child);
    Int32 bottomOfBottomChild;
    child->GetBottom(&bottomOfBottomChild);

    if (ShowingBottomFadingEdge()) {
        if ((mSelectedPosition < mItemCount -1)
            || (rect->mBottom < (bottomOfBottomChild - fadingEdge))) {
            listUnfadedBottom -= fadingEdge;
        }
    }

    Int32 scrollYDelta = 0;
    Int32 rectGetHeight;
    rect->GetHeight(&rectGetHeight);
    if (rect->mBottom > listUnfadedBottom && rect->mTop > listUnfadedTop) {
        if (rectGetHeight > height) {
            scrollYDelta += (rect->mTop - listUnfadedTop);
        }
        else {
            scrollYDelta += (rect->mBottom - listUnfadedBottom);
        }
        Int32 distanceToBottom = bottomOfBottomChild - listUnfadedBottom;
        scrollYDelta = Elastos::Core::Math::Min(scrollYDelta, distanceToBottom);
    }
    else if (rect->mTop < listUnfadedTop && rect->mBottom < listUnfadedBottom) {
        if (rectGetHeight > height) {
            scrollYDelta -= (listUnfadedBottom - rect->mBottom);
        }
        else {
            scrollYDelta -= (listUnfadedTop - rect->mTop);
        }

        child = NULL;
        GetChildAt(0, (IView**)&child);
        Int32 scrollTop;
        child->GetTop(&scrollTop);
        Int32 deltaToTop = scrollTop - listUnfadedTop;
        scrollYDelta = Elastos::Core::Math::Max(scrollYDelta, deltaToTop);
    }

    Boolean scroll = scrollYDelta != 0;
    if (scroll) {
        ScrollListItemsBy(-scrollYDelta);
        PositionSelector(IAdapterView::INVALID_POSITION, childView);
        childView->GetTop(&mSelectedTop);
        Invalidate();
    }
    *res = scroll;
    return NOERROR;
}


ECode ListView::FillGap(
    /* [in] */ Boolean down)
{
    Int32 count;
    GetChildCount(&count);
    if (down) {
        Int32 paddingTop = 0;
        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            GetListPaddingTop(&paddingTop);
        }
        Int32 startOffSet;
        if (count > 0) {
            AutoPtr<IView> child;
            GetChildAt(count - 1, (IView**)&child);
            child->GetBottom(&startOffSet);
            startOffSet += mDividerHeight;
        }
        else {
            startOffSet = paddingTop;
        }
        FillDown(mFirstPosition + count, startOffSet);
        Int32 childCount;
        GetChildCount(&childCount);
        CorrectTooHigh(childCount);
    }
    else {
        Int32 paddingBottom = 0;
        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            GetListPaddingBottom(&paddingBottom);
        }
        Int32 startOffSet;
        if (count > 0) {
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            child->GetTop(&startOffSet);
            startOffSet -= mDividerHeight;
        }
        else {
            Int32 height;
            GetHeight(&height);
            startOffSet = height - paddingBottom;
        }
        FillUp(mFirstPosition - 1, startOffSet);
        Int32 childCount;
        GetChildCount(&childCount);
        CorrectTooLow(childCount);
    }
    return NOERROR;
}

AutoPtr<IView> ListView::FillDown(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 nextTop)
{
    AutoPtr<IView> selectedView;

    Int32 end = mBottom - mTop;
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        Int32 bottom;
        mListPadding->GetBottom(&bottom);
        end -= bottom;
    }

    while (nextTop < end && pos < mItemCount) {
        // is this the selected item?
        Boolean selected = pos == mSelectedPosition;
        Int32 left = 0;
        mListPadding->GetLeft(&left);
        AutoPtr<IView> child = MakeAndAddView(pos, nextTop, TRUE, left, selected);

        child->GetBottom(&nextTop);
        nextTop += mDividerHeight;
        if (selected) {
            selectedView = child;
        }
        pos++;
    }

    Int32 childCount;
    GetChildCount(&childCount);
    SetVisibleRangeHint(mFirstPosition, mFirstPosition + childCount - 1);
    return selectedView;
}

AutoPtr<IView> ListView::FillUp(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 nextBottom)
{
    AutoPtr<IView> selectedView ;

    Int32 end = 0;
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        Int32 top;
        mListPadding->GetTop(&top);
        end = top;
    }

    while (nextBottom > end && pos >= 0) {
        // is this the selected item?
        Boolean selected = pos == mSelectedPosition;
        Int32 left;
        mListPadding->GetLeft(&left);
        AutoPtr<IView> child = MakeAndAddView(pos, nextBottom, FALSE, left, selected);
        child->GetTop(&nextBottom);
        nextBottom -= mDividerHeight;
        if (selected) {
            selectedView = child;
        }
        pos--;
    }

    mFirstPosition = pos + 1;
    Int32 childCount;
    GetChildCount(&childCount);
    SetVisibleRangeHint(mFirstPosition, mFirstPosition + childCount - 1);
    return selectedView;
}

AutoPtr<IView> ListView::FillFromTop(
    /* [in] */ Int32 nextTop)
{
    mFirstPosition = Elastos::Core::Math::Min(mFirstPosition, mSelectedPosition);
    mFirstPosition = Elastos::Core::Math::Min(mFirstPosition, mItemCount - 1);
    if (mFirstPosition < 0) {
        mFirstPosition = 0;
    }
    return FillDown(mFirstPosition, nextTop);
}

AutoPtr<IView> ListView::FillFromMiddle(
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 height = childrenBottom - childrenTop;

    Int32 position = ReconcileSelectedPosition();

    Int32 left;
    mListPadding->GetLeft(&left);
    AutoPtr<IView> sel = MakeAndAddView(position, childrenTop, TRUE, left, TRUE);
    mFirstPosition = position;

    Int32 selHeight;
    sel->GetMeasuredHeight(&selHeight);
    if (selHeight <= height) {
        sel->OffsetTopAndBottom((height - selHeight) / 2);
    }

    FillAboveAndBelow(sel, position);

    Int32 childCount;
    GetChildCount(&childCount);
    if (!mStackFromBottom) {
        CorrectTooHigh(childCount);
    }
    else {
        CorrectTooLow(childCount);
    }

    return sel;
}

void ListView::FillAboveAndBelow(
    /* [in] */ IView* sel,
    /* [in] */ Int32 position)
{
    Int32 dividerHeight = mDividerHeight;
    Int32 top, bottom;
    sel->GetTop(&top);
    sel->GetBottom(&bottom);
    if (!mStackFromBottom) {
        FillUp(position - 1, top - dividerHeight);
        AdjustViewsUpOrDown();
        FillDown(position + 1, bottom + dividerHeight);
    }
    else {
        FillDown(position + 1, bottom + dividerHeight);
        AdjustViewsUpOrDown();
        FillUp(position - 1, top - dividerHeight);
    }
}

AutoPtr<IView> ListView::FillFromSelection(
    /* [in] */ Int32 selectedTop,
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 fadingEdgeLength;
    GetVerticalFadingEdgeLength(&fadingEdgeLength);
    Int32 selectedPosition = mSelectedPosition;

    AutoPtr<IView> sel;

    Int32 topSelectionPixel = GetTopSelectionPixel(childrenTop,
            fadingEdgeLength, selectedPosition);
    Int32 bottomSelectionPixel = GetBottomSelectionPixel(childrenBottom,
            fadingEdgeLength, selectedPosition);

    Int32 left;
    mListPadding->GetLeft(&left);
    sel = MakeAndAddView(selectedPosition, selectedTop, TRUE, left, TRUE);

    // Some of the newly selected item extends below the bottom of the list
    Int32 top, bottom;
    sel->GetTop(&top);
    sel->GetBottom(&bottom);
    if (bottom > bottomSelectionPixel) {
        // Find space available above the selection into which we can scroll
        // upwards
        Int32 spaceAbove = top - topSelectionPixel;

        // Find space required to bring the bottom of the selected item
        // fully into view
        Int32 spaceBelow = bottom - bottomSelectionPixel;
        Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);

        // Now offset the selected item to get it into view
        sel->OffsetTopAndBottom(-offset);
    }
    else if (top < topSelectionPixel) {
        // Find space required to bring the top of the selected item fully
        // into view
        Int32 spaceAbove = topSelectionPixel - top;

        // Find space available below the selection into which we can scroll
        // downwards
        Int32 spaceBelow = bottomSelectionPixel - bottom;
        Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);

        // Offset the selected item to get it into view
        sel->OffsetTopAndBottom(offset);
    }

    // Fill in views above and below
    FillAboveAndBelow(sel, selectedPosition);

    Int32 childCount;
    GetChildCount(&childCount);
    if (!mStackFromBottom) {
        CorrectTooHigh(childCount);
    }
    else {
        CorrectTooLow(childCount);
    }
    return sel;
}

Int32 ListView::GetBottomSelectionPixel(
    /* [in] */ Int32 childrenBottom,
    /* [in] */ Int32 fadingEdgeLength,
    /* [in] */ Int32 selectedPosition)
{
    Int32 bottomSelectionPixel = childrenBottom;
    if (selectedPosition != mItemCount - 1) {
        bottomSelectionPixel -= fadingEdgeLength;
    }
    return bottomSelectionPixel;
}

Int32 ListView::GetTopSelectionPixel(
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 fadingEdgeLength,
    /* [in] */ Int32 selectedPosition)
{
    Int32 topSelectionPixel = childrenTop;
    if (selectedPosition > 0) {
        topSelectionPixel += fadingEdgeLength;
    }
    return topSelectionPixel;
}

ECode ListView::SmoothScrollToPosition(
    /* [in] */ Int32 position)
{
    return AbsListView::SmoothScrollToPosition(position);
}

ECode ListView::SmoothScrollByOffset(
    /* [in] */ Int32 offset)
{
    return AbsListView::SmoothScrollByOffset(offset);
}

AutoPtr<IView> ListView::MoveSelection(
    /* [in] */ IView* oldSel,
    /* [in] */ IView* newSel,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 childrenTop,
    /* [in] */ Int32 childrenBottom)
{
    Int32 fadingEdgeLength;
    GetVerticalFadingEdgeLength(&fadingEdgeLength);
    Int32 selectedPosition = mSelectedPosition;

    AutoPtr<IView> sel;

    Int32 topSelectionPixel = GetTopSelectionPixel(childrenTop,
            fadingEdgeLength, selectedPosition);
    Int32 bottomSelectionPixel = GetBottomSelectionPixel(childrenTop,
            fadingEdgeLength, selectedPosition);

    Int32 left;
    mListPadding->GetLeft(&left);
    if (delta > 0) {
        /*
         * Case 1: Scrolling down.
         */

        /*
         *     Before           After
         *    |       |        |       |
         *    +-------+        +-------+
         *    |   A   |        |   A   |
         *    |   1   |   =>   +-------+
         *    +-------+        |   B   |
         *    |   B   |        |   2   |
         *    +-------+        +-------+
         *    |       |        |       |
         *
         *    Try to keep the top of the previously selected item where it was.
         *    oldSel = A
         *    sel = B
         */

        // Put oldSel (A) where it belongs
        Int32 oldTop;
        oldSel->GetTop(&oldTop);
        oldSel = MakeAndAddView(selectedPosition - 1, oldTop, TRUE, left, FALSE);

        Int32 dividerHeight = mDividerHeight;

        // Now put the new selection (B) below that
        Int32 oldBottom;
        oldSel->GetBottom(&oldBottom);
        sel = MakeAndAddView(selectedPosition, oldBottom + dividerHeight, TRUE,
                left, TRUE);

        // Some of the newly selected item extends below the bottom of the list
        Int32 selTop, selBottom;
        sel->GetTop(&selTop);
        sel->GetBottom(&selBottom);
        if (selBottom > bottomSelectionPixel) {

            // Find space available above the selection into which we can scroll upwards
            Int32 spaceAbove = selTop - topSelectionPixel;

            // Find space required to bring the bottom of the selected item fully into view
            Int32 spaceBelow = selBottom - bottomSelectionPixel;

            // Don't scroll more than half the height of the list
            Int32 halfVerticalSpace = (childrenBottom - childrenTop) / 2;
            Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);
            offset = Elastos::Core::Math::Min(offset, halfVerticalSpace);

            // We placed oldSel, so offset that item
            oldSel->OffsetTopAndBottom(-offset);
            // Now offset the selected item to get it into view
            sel->OffsetTopAndBottom(-offset);
        }

        // Fill in views above and below
        if (!mStackFromBottom) {
            FillUp(mSelectedPosition - 2, selTop - dividerHeight);
            AdjustViewsUpOrDown();
            FillDown(mSelectedPosition + 1, selBottom + dividerHeight);
        }
        else {
            FillDown(mSelectedPosition + 1, selBottom + dividerHeight);
            AdjustViewsUpOrDown();
            FillUp(mSelectedPosition - 2, selTop - dividerHeight);
        }
    }
    else if (delta < 0) {
        /*
         * Case 2: Scrolling up.
         */

        /*
         *     Before           After
         *    |       |        |       |
         *    +-------+        +-------+
         *    |   A   |        |   A   |
         *    +-------+   =>   |   1   |
         *    |   B   |        +-------+
         *    |   2   |        |   B   |
         *    +-------+        +-------+
         *    |       |        |       |
         *
         *    Try to keep the top of the item about to become selected where it was.
         *    newSel = A
         *    olSel = B
         */

        if (newSel != NULL) {
            // Try to position the top of newSel (A) where it was before it was selected
            Int32 newTop;
            newSel->GetTop(&newTop);
            sel = MakeAndAddView(selectedPosition, newTop, TRUE, left, TRUE);
        }
        else {
            // If (A) was not on screen and so did not have a view, position
            // it above the oldSel (B)
            Int32 oldTop;
            oldSel->GetTop(&oldTop);
            sel = MakeAndAddView(selectedPosition, oldTop, FALSE, left, TRUE);
        }

        // Some of the newly selected item extends above the top of the list
        Int32 selTop, selBottom;
        sel->GetTop(&selTop);
        sel->GetBottom(&selBottom);
        if (selTop < topSelectionPixel) {
            // Find space required to bring the top of the selected item fully into view
            Int32 spaceAbove = topSelectionPixel - selTop;

            // Find space available below the selection into which we can scroll downwards
            Int32 spaceBelow = bottomSelectionPixel - selBottom;

            // Don't scroll more than half the height of the list
            Int32 halfVerticalSpace = (childrenBottom - childrenTop) / 2;
            Int32 offset = Elastos::Core::Math::Min(spaceAbove, spaceBelow);
            offset = Elastos::Core::Math::Min(offset, halfVerticalSpace);

            // Offset the selected item to get it into view
            sel->OffsetTopAndBottom(offset);
        }

        // Fill in views above and below
        FillAboveAndBelow(sel, selectedPosition);
    }
    else {
        Int32 oldTop;
        oldSel->GetTop(&oldTop);

        /*
         * Case 3: Staying still
         */
        sel = MakeAndAddView(selectedPosition, oldTop, TRUE, left, TRUE);

        // We're staying still...
        if (oldTop < childrenTop) {
            // ... but the top of the old selection was off screen.
            // (This can happen if the data changes size out from under us)
            Int32 newBottom;
            sel->GetBottom(&newBottom);
            if (newBottom < childrenTop + 20) {
                // Not enough visible -- bring it onscreen
                Int32 selTop;
                sel->GetTop(&selTop);
                sel->OffsetTopAndBottom(childrenTop - selTop);
            }
        }

        // Fill in views above and below
        FillAboveAndBelow(sel, selectedPosition);
    }
    return sel;
}

ECode ListView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount > 0) {
        AutoPtr<IView> focusedChild;
        GetFocusedChild((IView**)&focusedChild);
        if (focusedChild != NULL) {
            Int32 index;
            IndexOfChild(focusedChild, &index);
            Int32 childPosition = mFirstPosition + index;
            Int32 childBottom;
            focusedChild->GetBottom(&childBottom);
            Int32 offset = Elastos::Core::Math::Max(0, childBottom - (h - mPaddingTop));
            Int32 top;
            focusedChild->GetTop(&top);
            top -= offset;
            if (mFocusSelector == NULL) {
                mFocusSelector = new FocusSelector(this);
            }
            Boolean res;
            Post(mFocusSelector->Setup(childPosition, top), &res);
        }
    }
    AbsListView::OnSizeChanged(w, h, oldw, oldh);
    return NOERROR;
}

ECode ListView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AbsListView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);

    Int32 childWidth, childHeight, childState;
    childWidth = childHeight = childState = 0;
    if (mAdapter == NULL) {
        mItemCount = 0;
    }
    else {
        IAdapter::Probe(mAdapter)->GetCount(&mItemCount);
    }

    if (mItemCount > 0 && (widthMode == View::MeasureSpec::UNSPECIFIED ||
        heightMode == View::MeasureSpec::UNSPECIFIED)) {

        AutoPtr<IView> child = ObtainView(0, mIsScrap);
        MeasureScrapChild(child, 0, widthMeasureSpec);
        child->GetMeasuredWidth(&childWidth);
        child->GetMeasuredHeight(&childHeight);
        Int32 measuredState;
        child->GetMeasuredState(&measuredState);
        childState = CombineMeasuredStates(childState, measuredState);

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<CAbsListViewLayoutParams> p =
            (CAbsListViewLayoutParams*)IAbsListViewLayoutParams::Probe(params);
        if (RecycleOnMeasure() && mRecycler->ShouldRecycleViewType(p->mViewType)) {
            mRecycler->AddScrapView(child, 0);
        }
    }

    Int32 leftTmp = 0, topTmp = 0, rightTmp = 0, bottomTmp = 0;
    mListPadding->GetLeft(&leftTmp);
    mListPadding->GetTop(&topTmp);
    mListPadding->GetRight(&rightTmp);
    mListPadding->GetBottom(&bottomTmp);

    if (widthMode == View::MeasureSpec::UNSPECIFIED) {
        Int32 width;
        GetVerticalScrollbarWidth(&width);
        widthSize = leftTmp + rightTmp + childWidth + width;
    }
    else {
        widthSize |= (childState & IView::MEASURED_STATE_MASK);
    }

    if (heightMode == View::MeasureSpec::UNSPECIFIED) {
        Int32 length;
        GetVerticalFadingEdgeLength(&length);
        heightSize = topTmp + bottomTmp + childHeight + length * 2;
    }

    if (heightMode == View::MeasureSpec::AT_MOST) {
        heightSize = MeasureHeightOfChildren(widthMeasureSpec, 0, NO_POSITION, heightSize, -1);
    }

    SetMeasuredDimension(widthSize, heightSize);
    mWidthMeasureSpec = widthMeasureSpec;
    return NOERROR;
}

void ListView::MeasureScrapChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 position,
    /* [in] */ Int32 widthMeasureSpec)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<IAbsListViewLayoutParams> p;
    if(lp != NULL) {
        p = IAbsListViewLayoutParams::Probe(lp);
    }

    if (p == NULL || lp == NULL){
        lp = NULL;
        GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&lp);
        p = IAbsListViewLayoutParams::Probe(lp);
        child->SetLayoutParams(lp);
    }

    Int32 viewType;
    IAdapter::Probe(mAdapter)->GetItemViewType(position, &viewType);
    p->SetViewType(viewType);
    p->SetForceAdd(TRUE);

    Int32 pWidth;
    IViewGroupLayoutParams::Probe(p)->GetWidth(&pWidth);
    Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(widthMeasureSpec,
            TO_CRECT(mListPadding)->mLeft + TO_CRECT(mListPadding)->mRight, pWidth);

    Int32 lpHeight;
    IViewGroupLayoutParams::Probe(p)->GetHeight(&lpHeight);
    Int32 childHeightSpec;
    if (lpHeight > 0) {
        childHeightSpec = View::MeasureSpec::MakeMeasureSpec(lpHeight, View::MeasureSpec::EXACTLY);
    }
    else {
        childHeightSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    }
    child->Measure(childWidthSpec, childHeightSpec);
}

Boolean ListView::RecycleOnMeasure()
{
    return TRUE;
}

Int32 ListView::MeasureHeightOfChildren(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 startPosition,
    /* [in] */ Int32 endPosition,
    /* [in] */ Int32 maxHeight,
    /* [in] */ Int32 disallowPartialChildPosition)
{
    AutoPtr<IListAdapter> adapter = mAdapter;
    if (adapter == NULL) {
        return TO_CRECT(mListPadding)->mTop + TO_CRECT(mListPadding)->mBottom;
    }
    Int32 returnedHeight = TO_CRECT(mListPadding)->mTop + TO_CRECT(mListPadding)->mBottom;
    Int32 dividerHeight;
    if ((mDividerHeight > 0) && mDivider != NULL) {
        dividerHeight = mDividerHeight;
    }
    else {
        dividerHeight = 0;
    }
    Int32 prevHeightWithoutPartialChild = 0;
    AutoPtr<IView> child;
    Int32 count;
    IAdapter::Probe(adapter)->GetCount(&count);
    endPosition = (endPosition == NO_POSITION) ? count - 1 : endPosition;
    Boolean recyle = RecycleOnMeasure();

    for (Int32 i = startPosition; i <= endPosition; ++i) {
        child = ObtainView(i, mIsScrap);
        MeasureScrapChild(child, i, widthMeasureSpec);

        if (i > 0) {
            returnedHeight += dividerHeight;
        }
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        if (recyle && mRecycler->ShouldRecycleViewType(((CAbsListViewLayoutParams*)lp.Get())->mViewType)) {
            mRecycler->AddScrapView(child, -1);
        }

        Int32 getChildHeight;
        child->GetMeasuredHeight(&getChildHeight);
        returnedHeight += getChildHeight;

        if (returnedHeight >= maxHeight) {
            return (disallowPartialChildPosition >= 0)
                && (i > disallowPartialChildPosition)
                && (prevHeightWithoutPartialChild > 0)
                && (returnedHeight != maxHeight) ? prevHeightWithoutPartialChild :maxHeight;
        }
        if ((disallowPartialChildPosition >= 0) && (i >= disallowPartialChildPosition)) {
            prevHeightWithoutPartialChild = returnedHeight;
        }
    }
    return returnedHeight;
}

Int32 ListView::FindMotionRow(
    /* [in] */ Int32 y)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount > 0) {
        if (!mStackFromBottom) {
            for(Int32 i = 0; i < childCount; i++) {
                AutoPtr<IView> v;
                GetChildAt(i, (IView**)&v);
                Int32 bottom;
                v->GetBottom(&bottom);
                if (y <= bottom) {
                    return mFirstPosition + i;
                }
            }
        }
        else {
            for(Int32 i = childCount - 1; i >= 0; i--) {
                AutoPtr<IView> v;
                GetChildAt(i, (IView**)&v);
                Int32 top;
                v->GetTop(&top);
                if (y >= top) {
                    return mFirstPosition + i;
                }
            }
        }
    }
    return IAdapterView::INVALID_POSITION;
}

AutoPtr<IView> ListView::FillSpecific(
    /* [in] */ Int32 position,
    /* [in] */ Int32 top)
{
    Boolean tempIsSelected = position == mSelectedPosition;
    Int32 left;
    mListPadding->GetLeft(&left);
    AutoPtr<IView> temp = MakeAndAddView(position, top, TRUE, left, tempIsSelected);
    // Possibly changed again in fillUp if we add rows above this one.
    mFirstPosition = position;

    AutoPtr<IView> above;
    AutoPtr<IView> below;

    Int32 dividerHeight = mDividerHeight;
    Int32 tempTop, tempBottom;
    temp->GetTop(&tempTop);
    temp->GetBottom(&tempBottom);
    if (!mStackFromBottom) {
        above = FillUp(position - 1, tempTop - dividerHeight);
        // This will correct for the top of the first view not touching the top of the list
        AdjustViewsUpOrDown();
        below = FillDown(position + 1, tempBottom + dividerHeight);
        Int32 childCount;
        GetChildCount(&childCount);
        if (childCount > 0) {
            CorrectTooHigh(childCount);
        }
    }
    else {
        below = FillDown(position + 1, tempBottom + dividerHeight);
        // This will correct for the bottom of the last view not touching the bottom of the list
        AdjustViewsUpOrDown();
        above = FillUp(position - 1, tempTop - dividerHeight);
        Int32 childCount;
        GetChildCount(&childCount);
        if (childCount > 0) {
            CorrectTooLow(childCount);
        }
    }

    if (tempIsSelected) {
        return temp;
    }
    else if (above != NULL) {
        return above;
    }
    else {
        return below;
    }
}

void ListView::CorrectTooHigh(
    /* [in] */ Int32 childCount)
{
    Int32 lastPosition = mFirstPosition + childCount - 1;
    if (lastPosition == mItemCount - 1 && childCount > 0) {
        AutoPtr<IView> lastChild;
        GetChildAt(childCount - 1, (IView**)&lastChild);
        Int32 lastBottom;
        lastChild->GetBottom(&lastBottom);
        Int32 end = (mBottom - mTop) - TO_CRECT(mListPadding)->mBottom;
        Int32 bottomOffSet = end - lastBottom;
        AutoPtr<IView> firstChild;
        GetChildAt(0, (IView**)&firstChild);
        Int32 firstTop;
        firstChild->GetTop(&firstTop);

        if (bottomOffSet > 0 && (mFirstPosition > 0 || firstTop < TO_CRECT(mListPadding)->mTop)) {
            if (mFirstPosition == 0) {
                bottomOffSet = Elastos::Core::Math::Min(bottomOffSet, TO_CRECT(mListPadding)->mTop - firstTop);
            }

            OffsetChildrenTopAndBottom(bottomOffSet);
            Int32 firstChildTop;
            firstChild->GetTop(&firstChildTop);
            if (mFirstPosition > 0) {
                FillUp(mFirstPosition - 1, firstChildTop - mDividerHeight);
                AdjustViewsUpOrDown();
            }
        }
    }
}

void ListView::CorrectTooLow(
    /* [in] */ Int32 childCount)
{
    if (mFirstPosition == 0 && childCount > 0) {
        AutoPtr<IView> firstChild;
        GetChildAt(0, (IView**)&firstChild);
        Int32 firstTop;
        firstChild->GetTop(&firstTop);
        Int32 end = (mBottom - mTop) - TO_CRECT(mListPadding)->mBottom;
        Int32 topOffSet = firstTop - TO_CRECT(mListPadding)->mTop;
        AutoPtr<IView> lastChild;
        GetChildAt(childCount - 1, (IView**)&lastChild);
        Int32 lastBottom;
        lastChild->GetBottom(&lastBottom);
        Int32 lastPosition = mFirstPosition + childCount - 1;

        if (topOffSet > 0) {
            if (lastPosition < mItemCount - 1 || lastBottom > end) {
                if (lastPosition == mItemCount - 1) {
                    topOffSet = Elastos::Core::Math::Min(topOffSet, lastBottom - end);
                }
                // Move everything up
                OffsetChildrenTopAndBottom(-topOffSet);
                if (lastPosition < mItemCount - 1) {
                    Int32 lastChildBottom;
                    lastChild->GetBottom(&lastChildBottom);
                    FillDown(lastPosition + 1, lastChildBottom + mDividerHeight);
                    AdjustViewsUpOrDown();
                }
            }
            else if (lastPosition == mItemCount - 1) {
                AdjustViewsUpOrDown();
            }
        }
    }
}

ECode ListView::LayoutChildren()
{
    Boolean blockLayoutRequests = mBlockLayoutRequests;
    if (blockLayoutRequests) {
        return NOERROR;
    }
    mBlockLayoutRequests = TRUE;

    //try {
    AbsListView::LayoutChildren();

    Invalidate();

    if (mAdapter == NULL) {
        ResetList();
        InvokeOnItemScrollListener();

        // finally
        if (!blockLayoutRequests) {
            mBlockLayoutRequests = FALSE;
        }

        return NOERROR;
    }

    const Int32 childrenTop = TO_CRECT(mListPadding)->mTop;
    const Int32 childrenBottom = mBottom - mTop - TO_CRECT(mListPadding)->mBottom;
    Int32 childCount;
    GetChildCount(&childCount);

    Int32 index = 0, delta = 0;

    AutoPtr<IView> sel, oldSel, oldFirst, newSel;

    // Remember stuff we will need down below
    switch(mLayoutMode) {
        case AbsListView::LAYOUT_SET_SELECTION:
            index = mNextSelectedPosition - mFirstPosition;
            if (index >= 0 && index < childCount) {
                GetChildAt(index, (IView**)&newSel);
            }
            break;
        case AbsListView::LAYOUT_FORCE_TOP:
        case AbsListView::LAYOUT_FORCE_BOTTOM:
        case AbsListView::LAYOUT_SPECIFIC:
        case AbsListView::LAYOUT_SYNC:
            break;
        case AbsListView::LAYOUT_MOVE_SELECTION:
        default:
            // Remember the previously selected view
            index = mSelectedPosition - mFirstPosition;
            if (index >= 0 && index < childCount) {
                GetChildAt(index, (IView**)&oldSel);
            }

            // Remember the previous first child
            GetChildAt(0, (IView**)&oldFirst);
            if (mNextSelectedPosition >= 0) {
                delta = mNextSelectedPosition - mSelectedPosition;
            }

            // Caution: newSel might be null
            GetChildAt(index + delta, (IView**)&newSel);
    }

    Boolean dataChanged = mDataChanged;
    if (dataChanged) {
        HandleDataChanged();
    }

    // Handle the empty set by removing all views that are visible
    // and calling it a day
    Int32 adaCount;
    IAdapter::Probe(mAdapter)->GetCount(&adaCount);
    if (mItemCount == 0) {
        ResetList();
        InvokeOnItemScrollListener();

        // finally
        if (!blockLayoutRequests) {
            mBlockLayoutRequests = FALSE;
        }

        return NOERROR;
    }
    else if (mItemCount != adaCount) {
        Slogger::E("ListView", "IllegalStateException: The content of the adapter has changed but " \
            "ListView did not receive a notification. Make sure the content of " \
            "your adapter is not modified from a background thread, but only " \
            "from the UI thread. Make sure your adapter calls notifyDataSetChanged() " \
            "when its content changes. [in ListView(0x%08x), with Adapter(%p)]", mID, mAdapter.Get());

        // finally
        if (!blockLayoutRequests) {
            mBlockLayoutRequests = FALSE;
        }

        return E_ILLEGAL_STATE_EXCEPTION;
    }
    SetSelectedPositionInt(mNextSelectedPosition);

    AutoPtr<IAccessibilityNodeInfo> accessibilityFocusLayoutRestoreNode;
    AutoPtr<IView> accessibilityFocusLayoutRestoreView;
    Int32 accessibilityFocusPosition = IAdapterView::INVALID_POSITION;

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
                Boolean res;
                if (!dataChanged || IsDirectChildHeaderOrFooter(focusChild)
                        || (focusChild->HasTransientState(&res), res)
                        || mAdapterHasStableIds) {
                    // The views won't be changing, so try to maintain
                    // focus on the current host and virtual view.
                    accessibilityFocusLayoutRestoreView = focusHost;
                    viewRootImpl->GetAccessibilityFocusedVirtualView((IAccessibilityNodeInfo**)&accessibilityFocusLayoutRestoreNode);
                }

                // If all else fails, maintain focus at the same
                // position.
                GetPositionForView(focusChild, &accessibilityFocusPosition);
            }
        }
    }

    AutoPtr<IView> focusLayoutRestoreDirectChild;
    AutoPtr<IView> focusLayoutRestoreView;

    // Take focus back to us temporarily to avoid the eventual call to
    // clear focus when removing the focused child below from messing
    // things up when ViewAncestor assigns focus back to someone else.
    AutoPtr<IView> focusedChild;
    GetFocusedChild((IView**)&focusedChild);
    if (focusedChild != NULL) {
        // TODO: in some cases focusedChild.getParent() == null

        // We can remember the focused view to restore after re-layout
        // if the data hasn't changed, or if the focused position is a
        // header or footer.
        if (!dataChanged || IsDirectChildHeaderOrFooter(focusedChild)) {
            focusLayoutRestoreDirectChild = focusedChild;
            FindFocus((IView**)&focusLayoutRestoreView);
            if (focusLayoutRestoreView != NULL) {
                focusLayoutRestoreView->OnStartTemporaryDetach();
            }
        }
        Boolean res;
        RequestFocus(&res);
    }

    // Pull all children into the RecycleBin.
    // These views will be reused if possible
    const Int32 firstPosition = mFirstPosition;
    AutoPtr<AbsListView::RecycleBin> recycleBin = mRecycler;
    if (dataChanged) {
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> childView;
            GetChildAt(i, (IView**)&childView);
            recycleBin->AddScrapView(childView, firstPosition + i);
        }
    }
    else {
        recycleBin->FillActiveViews(childCount, firstPosition);
    }

    // Clear out old views
    DetachAllViewsFromParent();
    recycleBin->RemoveSkippedScrap();

    switch (mLayoutMode) {
        case AbsListView::LAYOUT_SET_SELECTION:
            if (newSel != NULL) {
                Int32 newSelTop;
                newSel->GetTop(&newSelTop);
                sel = FillFromSelection(newSelTop, childrenTop, childrenBottom);
            }
            else {
                sel = FillFromMiddle(childrenTop, childrenBottom);
            }
            break;
        case AbsListView::LAYOUT_SYNC:
            sel = FillSpecific(mSyncPosition, mSpecificTop);
            break;
        case AbsListView::LAYOUT_FORCE_BOTTOM:
            sel = FillUp(mItemCount - 1, childrenBottom);
            AdjustViewsUpOrDown();
            break;
        case AbsListView::LAYOUT_FORCE_TOP:
            mFirstPosition = 0;
            sel = FillFromTop(childrenTop);
            AdjustViewsUpOrDown();
            break;
        case AbsListView::LAYOUT_SPECIFIC:
            sel = FillSpecific(ReconcileSelectedPosition(), mSpecificTop);
            break;
        case AbsListView::LAYOUT_MOVE_SELECTION:
            sel = MoveSelection(oldSel, newSel, delta, childrenTop, childrenBottom);
            break;
        default:
            if (childCount == 0) {
                if (!mStackFromBottom) {
                    Int32 position = LookForSelectablePosition(0, TRUE);
                    SetSelectedPositionInt(position);
                    sel = FillFromTop(childrenTop);
                }
                else {
                    Int32 position = LookForSelectablePosition(mItemCount - 1, FALSE);
                    SetSelectedPositionInt(position);
                    sel = FillUp(mItemCount - 1, childrenBottom);
                }
            }
            else {
                Int32 top = childrenTop;
                if (mSelectedPosition >= 0 && mSelectedPosition < mItemCount) {
                    if (oldSel != NULL) {
                        oldSel->GetTop(&top);
                    }
                    sel = FillSpecific(mSelectedPosition, top);
                }
                else if (mFirstPosition < mItemCount) {
                    if (oldFirst != NULL) {
                        oldFirst->GetTop(&top);
                    }
                    sel = FillSpecific(mFirstPosition, top);
                }
                else {
                    sel = FillSpecific(0, childrenTop);
                }
            }
            break;
    }

    // Flush any cached views that did not get reused above
    mRecycler->ScrapActiveViews();

    if (sel != NULL) {
        // The current selected item should get focus if items are
        // focusable.
        Boolean IsHasFocus;
        sel->HasFocus(&IsHasFocus);
        Boolean res;

        if (mItemsCanFocus && (HasFocus(&res), res) && IsHasFocus) {
            Boolean fLRVRequestFocus = FALSE, selRequestFocus = FALSE;
            if (focusLayoutRestoreView != NULL) focusLayoutRestoreView->RequestFocus(&fLRVRequestFocus);
            sel->RequestFocus(&selRequestFocus);

            Boolean focusWasTaken = ((sel == focusLayoutRestoreDirectChild)
                    && fLRVRequestFocus) || selRequestFocus;
            if (!focusWasTaken) {
                AutoPtr<IView> focused;
                GetFocusedChild((IView**)&focused);
                if (focused != NULL) {
                    focused->ClearFocus();
                }
                PositionSelector(IAdapterView::INVALID_POSITION, sel);
            }
            else {
                sel->SetSelected(FALSE);
                mSelectorRect->SetEmpty();
            }
        }
        else {
            PositionSelector(IAdapterView::INVALID_POSITION, sel);
        }
        sel->GetTop(&mSelectedTop);
    }
    else {
        const Boolean inTouchMode = mTouchMode == AbsListView::TOUCH_MODE_TAP
                || mTouchMode == AbsListView::TOUCH_MODE_DONE_WAITING;
        if (inTouchMode) {
            // If the user's finger is down, select the motion position.
            AutoPtr<IView> child;
            GetChildAt(mMotionPosition - mFirstPosition, (IView**)&child);
            if (child != NULL) {
                PositionSelector(mMotionPosition, child);
            }
        }
        else if (mSelectorPosition != IAdapterView::INVALID_POSITION) {
            // If we had previously positioned the selector somewhere,
            // put it back there. It might not match up with the data,
            // but it's transitioning out so it's not a big deal.
            AutoPtr<IView> child;
            GetChildAt(mSelectorPosition - mFirstPosition, (IView**)&child);
            if (child != NULL) {
                PositionSelector(mSelectorPosition, child);
            }
        }
        else {
            // Otherwise, clear selection.
            mSelectedTop = 0;
            mSelectorRect->SetEmpty();
        }

        // Even if there is not selected position, we may need to
        // restore focus (i.e. something focusable in touch mode).

        Boolean res;
        if ((HasFocus(&res), res) && focusLayoutRestoreView != NULL) {
            focusLayoutRestoreView->RequestFocus(&res);
        }
    }

    // Attempt to restore accessibility focus, if necessary.
    if (viewRootImpl != NULL) {
        AutoPtr<IView> newAccessibilityFocusedView;
        viewRootImpl->GetAccessibilityFocusedHost((IView**)&newAccessibilityFocusedView);
        Boolean res;
        if (newAccessibilityFocusedView == NULL) {
            if (accessibilityFocusLayoutRestoreView != NULL
                    && (accessibilityFocusLayoutRestoreView->IsAttachedToWindow(&res), res)) {
                AutoPtr<IAccessibilityNodeProvider> provider;
                accessibilityFocusLayoutRestoreView->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (accessibilityFocusLayoutRestoreNode != NULL && provider != NULL) {
                    Int64 id;
                    accessibilityFocusLayoutRestoreNode->GetSourceNodeId(&id);
                    Int32 virtualViewId = CAccessibilityNodeInfo::GetVirtualDescendantId(id);
                    provider->PerformAction(virtualViewId,
                            IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &res);
                }
                else {
                    accessibilityFocusLayoutRestoreView->RequestAccessibilityFocus(&res);
                }
            }
            else if (accessibilityFocusPosition != IAdapterView::INVALID_POSITION) {
                using Elastos::Core::Math;
                Int32 childCount;
                GetChildCount(&childCount);
                Int32 position = Math::Constrain((accessibilityFocusPosition - mFirstPosition), 0, (childCount - 1));
                AutoPtr<IView> restoreView;
                GetChildAt(position, (IView**)&restoreView);
                if (restoreView != NULL) {
                    restoreView->RequestAccessibilityFocus(&res);
                }
            }
        }
    }

    // Tell focus view we are done mucking with it, if it is still in
    // our view hierarchy.
    if (focusLayoutRestoreView != NULL) {
        AutoPtr<IBinder> winToken;
        focusLayoutRestoreView->GetWindowToken((IBinder**)&winToken);
        if (winToken != NULL) {
            focusLayoutRestoreView->OnFinishTemporaryDetach();
        }
    }
    mLayoutMode = AbsListView::LAYOUT_NORMAL;
    mDataChanged = FALSE;
    if (mPositionScrollAfterLayout != NULL) {
        Boolean res;
        Post(mPositionScrollAfterLayout, &res);
        mPositionScrollAfterLayout = NULL;
    }
    mNeedSync = FALSE;
    SetNextSelectedPositionInt(mSelectedPosition);

    UpdateScrollIndicators();

    if (mItemCount > 0) {
        CheckSelectionChanged();
    }

    InvokeOnItemScrollListener();
    //}  finally{
    if (!blockLayoutRequests) {
        mBlockLayoutRequests = FALSE;
    }
    //}
    return NOERROR;
}

Boolean ListView::IsDirectChildHeaderOrFooter(
    /* [in] */ IView* child)
{
    AutoPtr<IArrayList> headers = mHeaderViewInfos;
    Int32 numHeaders;
    headers->GetSize(&numHeaders);
    for (Int32 i = 0; i < numHeaders; i++) {
        AutoPtr<IInterface> obj;
        headers->Get(i, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        AutoPtr<IView> view;
        info->GetView((IView**)&view);
        if (child == view) {
            return TRUE;
        }
    }

    AutoPtr<IArrayList> footers = mFooterViewInfos;
    Int32 numFooters;
    footers->GetSize(&numFooters);
    for (Int32 i = 0; i < numFooters; i++) {
        AutoPtr<IInterface> obj;
        footers->Get(i, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        AutoPtr<IView> view;
        info->GetView((IView**)&view);
        if (child == view) {
            return TRUE;
        }
    }

    return FALSE;
}

AutoPtr<IView> ListView::MakeAndAddView(
    /* [in] */ Int32 position,
    /* [in] */ Int32 y,
    /* [in] */ Boolean flow,
    /* [in] */ Int32 childrenLeft,
    /* [in] */ Boolean selected)
{
    AutoPtr<IView> child;

    if (!mDataChanged) {
        // Try to use an existing view for this position
        child = mRecycler->GetActiveView(position);
        if (child != NULL) {
            // Found it -- we're using an existing child
            // This just needs to be positioned
            SetupChild(child, position, y, flow, childrenLeft, selected, TRUE);

            return child;
        }
    }

    // Make a new view for this position, or convert an unused view if possible
    child = ObtainView(position, mIsScrap);

    // This needs to be positioned and measured
    SetupChild(child, position, y, flow, childrenLeft, selected, (*mIsScrap)[0]);

    return child;
}

void ListView::SetupChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 position,
    /* [in] */ Int32 y,
    /* [in] */ Boolean flowDown,
    /* [in] */ Int32 childrenLeft,
    /* [in] */ Boolean selected,
    /* [in] */ Boolean recycled)
{
    // Trace.traceBegin(Trace.TRACE_TAG_VIEW, "setupListItem");
    Boolean isSelected = selected && ShouldShowSelector();
    Boolean res;
    Boolean updateChildSelected = isSelected != (child->IsSelected(&res), res);
    Int32 mode = mTouchMode;
    Boolean isPressed = mode > AbsListView::TOUCH_MODE_DOWN && mode < AbsListView::TOUCH_MODE_SCROLL &&
        mMotionPosition == position;
    Boolean updateChildPressed = isPressed != (child->IsPressed(&res), res);
    Boolean needToMeasure = !recycled || updateChildSelected || (child->IsLayoutRequested(&res), res);

    // Respect layout params that are already in the view. Otherwise make some up...
    // noinspection unchecked
    AutoPtr<IViewGroupLayoutParams> p;
    child->GetLayoutParams((IViewGroupLayoutParams**)&p);
    if (p == NULL) {
        GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&p);
    }
    CAbsListViewLayoutParams* cp = (CAbsListViewLayoutParams*)IAbsListViewLayoutParams::Probe(p);
    assert(cp != NULL);
    IAdapter::Probe(mAdapter)->GetItemViewType(position, &(cp->mViewType));

    if ((recycled && !cp->mForceAdd) || (cp->mRecycledHeaderFooter &&
        cp->mViewType == IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER)) {
        AttachViewToParent(child, flowDown ? -1 : 0, p);
    }
    else {
        cp->mForceAdd = FALSE;
        if (cp->mViewType == IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
            cp->mRecycledHeaderFooter = TRUE;
        }
        AddViewInLayout(child, flowDown ? -1 : 0, p, TRUE);
    }

    if (updateChildSelected) {
        child->SetSelected(isSelected);
    }

    if (updateChildPressed) {
        child->SetPressed(isPressed);
    }

    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE && mCheckStates != NULL) {
        AutoPtr<ICheckable> checkable = ICheckable::Probe(child);
        if (checkable != NULL) {
            Boolean value;
            mCheckStates->Get(position, &value);
            checkable->SetChecked(value);
        }
        else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IApplicationInfo> info;
            context->GetApplicationInfo((IApplicationInfo**)&info);
            Int32 version;
            info->GetTargetSdkVersion(&version);
            if (version >= Build::VERSION_CODES::HONEYCOMB) {
                Boolean value;
                mCheckStates->Get(position, &value);
                child->SetActivated(value);
            }
        }
    }

    if (needToMeasure) {
        Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(mWidthMeasureSpec,
                TO_CRECT(mListPadding)->mLeft + TO_CRECT(mListPadding)->mRight, cp->mWidth);
        Int32 childHeightSpec = 0;
        if (cp->mHeight > 0) {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(cp->mHeight,
                View::MeasureSpec::EXACTLY);
        }
        else {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(0,
                View::MeasureSpec::UNSPECIFIED);
        }
        child->Measure(childWidthSpec, childHeightSpec);
    }
    else {
        CleanupLayoutState(child);
    }

    Int32 w;
    child->GetMeasuredWidth(&w);
    Int32 h;
    child->GetMeasuredHeight(&h);
    Int32 childTop = flowDown ? y : y - h;

    if (needToMeasure) {
        Int32 childRight = childrenLeft + w;
        Int32 childBottom = childTop + h;
        child->Layout(childrenLeft, childTop, childRight, childBottom);
    }
    else {
        Int32 l, t;
        child->GetLeft(&l);
        child->GetTop(&t);
        child->OffsetLeftAndRight(childrenLeft - l);
        child->OffsetTopAndBottom(childTop - t);
    }

    if (mCachingStarted && (child->IsDrawingCacheEnabled(&res), !res)) {
        child->SetDrawingCacheEnabled(TRUE);
    }

    if (recycled) {
        AutoPtr<IViewGroupLayoutParams> layoutParams;
        child->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
        CAbsListViewLayoutParams* params =
            (CAbsListViewLayoutParams*)IAbsListViewLayoutParams::Probe(layoutParams);
        if (params->mScrappedFromPosition != position) {
            child->JumpDrawablesToCurrentState();
        }
    }

    // Trace.traceEnd(Trace.TRACE_TAG_VIEW);
}

Boolean ListView::CanAnimate()
{
    return AbsListView::CanAnimate() && mItemCount > 0;
}

ECode ListView::SetSelection(
    /* [in] */ Int32 position)
{
    return SetSelectionFromTop(position, 0);
}

ECode ListView::SetSelectionInt(
    /* [in] */ Int32 position)
{
    SetNextSelectedPositionInt(position);

    Boolean awakeScrollbars = FALSE;
    if (mSelectedPosition >= 0) {
        if (position == mSelectedPosition - 1) {
            awakeScrollbars = TRUE;
        }
        else if (position == mSelectedPosition + 1) {
            awakeScrollbars = TRUE;
        }
    }

    if (mPositionScroller != NULL) {
        mPositionScroller->Stop();
    }

    LayoutChildren();

    if (awakeScrollbars) {
        AwakenScrollBars();
    }
    return NOERROR;
}

Int32 ListView::LookForSelectablePosition(
    /* [in] */ Int32 position,
    /* [in] */ Boolean lookDown)
{
    AutoPtr<IListAdapter> adapter = mAdapter;
    Boolean res;
    if (adapter == NULL || (IsInTouchMode(&res), res)) {
        return IAdapterView::INVALID_POSITION;
    }

    Int32 count;
    IAdapter::Probe(adapter)->GetCount(&count);
    if (!mAreAllItemsSelectable) {
        Boolean enabled;
        adapter->IsEnabled(position, &enabled);
        if (lookDown) {
            position = Elastos::Core::Math::Max(0, position);
            while(position < count && !enabled) {
                position++;
            }
        }
        else {
            position = Elastos::Core::Math::Min(position, count - 1);
            while(position >= 0 && !enabled) {
                position--;
            }
        }

        if (position < 0 || position >= count) {
            return IAdapterView::INVALID_POSITION;
        }
        return position;
    }
    else {
        if (position < 0 || position >= count) {
            return IAdapterView::INVALID_POSITION;
        }
        return position;
    }
}

Int32 ListView::LookForSelectablePositionAfter(
    /* [in] */ Int32 current,
    /* [in] */ Int32 position,
    /* [in] */ Boolean lookDown)
{
    AutoPtr<IListAdapter> adapter = mAdapter;
    Boolean res;
    if (adapter == NULL || (IsInTouchMode(&res), res)) {
        return IAdapterView::INVALID_POSITION;
    }

    // First check after the starting position in the specified direction.
    const Int32 after = LookForSelectablePosition(position, lookDown);
    if (after != IAdapterView::INVALID_POSITION) {
        return after;
    }

    // Then check between the starting position and the current position.
    Int32 count;
    IAdapter::Probe(adapter)->GetCount(&count);
    current = MathUtils::Constrain(current, -1, count - 1);
    Boolean enabled;
    adapter->IsEnabled(position, &enabled);
    if (lookDown) {
        position = Elastos::Core::Math::Min(position - 1, count - 1);
        while ((position > current) && !enabled) {
            position--;
        }
        if (position <= current) {
            return IAdapterView::INVALID_POSITION;
        }
    }
    else {
        position = Elastos::Core::Math::Max(0, position + 1);
        while ((position < current) && !enabled) {
            position++;
        }
        if (position >= current) {
            return IAdapterView::INVALID_POSITION;
        }
    }

    return position;
}

ECode ListView::SetSelectionAfterHeaderView()
{
    Int32 count;
    mHeaderViewInfos->GetSize(&count);
    if (count > 0) {
        mNextSelectedPosition = 0;
        return NOERROR;
    }
    if (mAdapter != NULL) {
        SetSelection(count);
    }
    else {
        mNextSelectedPosition = count;
        mLayoutMode = AbsListView::LAYOUT_SET_SELECTION;
    }
    return NOERROR;
}

ECode ListView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean handled;
    AbsListView::DispatchKeyEvent(event, &handled);
    if (!handled) {
        AutoPtr<IView> focused;
        GetFocusedChild((IView**)&focused);
        Int32 keyEvent;
        event->GetAction(&keyEvent);
        if (focused != NULL && keyEvent == IKeyEvent::ACTION_DOWN) {
            Int32 keyCode;
            event->GetKeyCode(&keyCode);
            OnKeyDown(keyCode, event, &handled);
        }
    }
    *res = handled;
    return NOERROR;
}

ECode ListView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CommonKey(keyCode, 1, event);
    return NOERROR;
}

ECode ListView::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CommonKey(keyCode, repeatCount, event);
    return NOERROR;
}

ECode ListView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CommonKey(keyCode, 1, event);
    return NOERROR;
}

Boolean ListView::CommonKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event)
{
    Boolean res;
    if (mAdapter == NULL || (IsAttachedToWindow(&res), !res)) {
        return FALSE;
    }

    if (mDataChanged) {
        LayoutChildren();
    }

    Boolean handled = FALSE;
    Int32 action;
    event->GetAction(&action);

    if (action != IKeyEvent::ACTION_UP) {
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_UP:
                Boolean noModifiersUp, hasModifiersUp;
                event->HasNoModifiers(&noModifiersUp);
                event->HasModifiers(IKeyEvent::META_ALT_ON ,&hasModifiersUp);
                if (noModifiersUp) {
                    handled = ResurrectSelectionIfNeeded();
                    if (!handled) {
                        while(count-- > 0) {
                            if (ArrowScroll(IView::FOCUS_UP)) {
                                handled = TRUE;
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
                else if (hasModifiersUp) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_UP);
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_DOWN:
                Boolean noModifiersDown, hasModifiersDown;
                event->HasNoModifiers(&noModifiersDown);
                event->HasModifiers(IKeyEvent::META_ALT_ON ,&hasModifiersDown);
                if (noModifiersDown) {
                    handled = ResurrectSelectionIfNeeded();
                    if (!handled) {
                        while (count-- > 0) {
                            if (ArrowScroll(IView::FOCUS_DOWN)) {
                                handled = TRUE;
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
                else if (hasModifiersDown) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_DOWN);
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_LEFT:
                Boolean noModifiersLeft;
                event->HasNoModifiers(&noModifiersLeft);
                if (noModifiersLeft) {
                    ASSERT_SUCCEEDED(HandleHorizontalFocusWithinListItem(IView::FOCUS_LEFT, &handled));
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                Boolean noModifiersRight;
                event->HasNoModifiers(&noModifiersRight);
                if (noModifiersRight) {
                    ASSERT_SUCCEEDED(HandleHorizontalFocusWithinListItem(IView::FOCUS_RIGHT, &handled));
                }
                break;

            case IKeyEvent::KEYCODE_DPAD_CENTER:
            case IKeyEvent::KEYCODE_ENTER:
                Boolean noModifiersEnter;
                event->HasNoModifiers(&noModifiersEnter);
                if (noModifiersEnter) {
                    handled = ResurrectSelectionIfNeeded();
                    Int32 repeatCount;
                    event->GetRepeatCount(&repeatCount);
                    Int32 childCount;
                    GetChildCount(&childCount);
                    if (!handled && repeatCount == 0 && childCount > 0) {
                        KeyPressed();
                        handled = TRUE;
                    }
                }
                break;

            case IKeyEvent::KEYCODE_SPACE:
                Boolean isShowing;
                if (mPopup == NULL || (mPopup->IsShowing(&isShowing), !isShowing)) {
                    Boolean noModifiersPageSpace, hasModifiersPageSpace;
                    event->HasNoModifiers(&noModifiersPageSpace);
                    event->HasModifiers(IKeyEvent::META_SHIFT_ON ,&hasModifiersPageSpace);
                    if (noModifiersPageSpace) {
                        handled = ResurrectSelectionIfNeeded() || PageScroll(IView::FOCUS_DOWN);
                    }
                    else if (hasModifiersPageSpace) {
                        handled = ResurrectSelectionIfNeeded() || PageScroll(IView::FOCUS_UP);
                    }
                    handled = TRUE;
                }
                break;

            case IKeyEvent::KEYCODE_PAGE_UP:
                Boolean noModifiersPageUp, hasModifiersPageUp;
                event->HasNoModifiers(&noModifiersPageUp);
                event->HasModifiers(IKeyEvent::META_ALT_ON ,&hasModifiersPageUp);
                if (noModifiersPageUp) {
                    handled = ResurrectSelectionIfNeeded() || PageScroll(IView::FOCUS_UP);
                }
                else if (hasModifiersPageUp) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_UP);
                }
                break;

            case IKeyEvent::KEYCODE_PAGE_DOWN:
                Boolean noModifiersPageDown, hasModifiersPageDown;
                event->HasNoModifiers(&noModifiersPageDown);
                event->HasModifiers(IKeyEvent::META_ALT_ON ,&hasModifiersPageDown);
                if (noModifiersPageDown) {
                    handled = ResurrectSelectionIfNeeded() || PageScroll(IView::FOCUS_DOWN);
                }
                else if (hasModifiersPageDown) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_DOWN);
                }
                break;

            case IKeyEvent::KEYCODE_MOVE_HOME:
                Boolean noModifiersMoveHome;
                event->HasNoModifiers(&noModifiersMoveHome);
                if (noModifiersMoveHome) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_UP);
                }
                break;

            case IKeyEvent::KEYCODE_MOVE_END:
                Boolean noModifiersMoveEnd;
                event->HasNoModifiers(&noModifiersMoveEnd);
                if (noModifiersMoveEnd) {
                    handled = ResurrectSelectionIfNeeded() || FullScroll(IView::FOCUS_DOWN);
                }
                break;

            case IKeyEvent::KEYCODE_TAB:
                // XXX Sometimes it is useful to be able to TAB through the items in
                //     a ListView sequentially.  Unfortunately this can create an
                //     asymmetry in TAB navigation order unless the list selection
                //     always reverts to the top or bottom when receiving TAB focus from
                //     another widget.  Leaving this behavior disabled for now but
                //     perhaps it should be configurable (and more comprehensive).
                if (FALSE) {
                    Boolean noModifiersTab, hasModifiersTab;
                    event->HasNoModifiers(&noModifiersTab);
                    event->HasModifiers(IKeyEvent::META_SHIFT_ON ,&hasModifiersTab);
                    if (noModifiersTab) {
                        handled = ResurrectSelectionIfNeeded() || ArrowScroll(IView::FOCUS_DOWN);
                    }
                    else if (hasModifiersTab) {
                        handled = ResurrectSelectionIfNeeded() || ArrowScroll(IView::FOCUS_UP);
                    }
                }
            break;
            }
    }

    if (handled) {
        return TRUE;
    }

    if (SendToTextFilter(keyCode, count, event)) {
        return TRUE;
    }

    switch (action) {
        case IKeyEvent::ACTION_DOWN:
            AbsListView::OnKeyDown(keyCode, event, &res);
            return res;

        case IKeyEvent::ACTION_UP:
            AbsListView::OnKeyUp(keyCode, event, &res);
            return res;

        case IKeyEvent::ACTION_MULTIPLE:
            AbsListView::OnKeyMultiple(keyCode, count, event, &res);
            return res;

        default: // shouldn't happen
            return FALSE;
    }
}

Boolean ListView::PageScroll(
    /* [in] */ Int32 direction)
{
    Int32 nextPage;
    Boolean down;

    Int32 childCount;
    GetChildCount(&childCount);
    if (direction == IView::FOCUS_UP) {
        nextPage = Elastos::Core::Math::Max(0, mSelectedPosition - childCount - 1);
        down = FALSE;
    }
    else if (direction == IView::FOCUS_DOWN) {
        nextPage = Elastos::Core::Math::Min(mItemCount - 1, mSelectedPosition + childCount - 1);
        down = TRUE;
    }
    else {
        return FALSE;
    }

    if (nextPage >= 0) {
        const Int32 position = LookForSelectablePositionAfter(mSelectedPosition, nextPage, down);
        if (position >= 0) {
            mLayoutMode = AbsListView::LAYOUT_SPECIFIC;
            Int32 length;
            GetVerticalFadingEdgeLength(&length);
            mSpecificTop = mPaddingTop + length;

            Int32 childCount;
            GetChildCount(&childCount);
            if (down && (position > (mItemCount - childCount))) {
                mLayoutMode = AbsListView::LAYOUT_FORCE_BOTTOM;
            }

            if (!down && (position < childCount)) {
                mLayoutMode = AbsListView::LAYOUT_FORCE_TOP;
            }

            SetSelectionInt(position);
            InvokeOnItemScrollListener();
            if (!AwakenScrollBars()) {
                Invalidate();
            }
            return TRUE;
        }
    }
    return FALSE;
}

Boolean ListView::FullScroll(
    /* [in] */ Int32 direction)
{
    Boolean moved = FALSE;
    if (direction == IView::FOCUS_UP) {
        if (mSelectedPosition != 0) {
            const Int32 position = LookForSelectablePositionAfter(mSelectedPosition, 0, TRUE);
            if (position >= 0) {
                mLayoutMode = AbsListView::LAYOUT_FORCE_TOP;
                SetSelectionInt(position);
                InvokeOnItemScrollListener();
            }
            moved = TRUE;
        }
    }
    else if (direction == IView::FOCUS_DOWN) {
        const Int32 lastItem = mItemCount - 1;
        if (mSelectedPosition < lastItem) {
            const Int32 position = LookForSelectablePositionAfter(mSelectedPosition, lastItem, FALSE);
            if (position >= 0) {
                mLayoutMode = AbsListView::LAYOUT_FORCE_BOTTOM;
                SetSelectionInt(position);
                InvokeOnItemScrollListener();
            }
            moved = TRUE;
        }
    }
    if (moved && !AwakenScrollBars()) {
        AwakenScrollBars();
        Invalidate();
    }
    return moved;
}

ECode ListView::HandleHorizontalFocusWithinListItem(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = FALSE;

    if (direction != IView::FOCUS_LEFT && direction != IView::FOCUS_RIGHT) {
        Slogger::E("ListView", "direction must be one of {View.FOCUS_LEFT, View.FOCUS_RIGHT}");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 numChildren;
    GetChildCount(&numChildren);
    if (mItemsCanFocus && numChildren > 0 && mSelectedPosition != IAdapterView::INVALID_POSITION) {
        AutoPtr<IView> selectedView;
        GetSelectedView((IView**)&selectedView);
        Boolean hasFocus;
        if (selectedView != NULL
                && (selectedView->HasFocus(&hasFocus), hasFocus)
                && IViewGroup::Probe(selectedView) != NULL) {
            AutoPtr<IView> currentFocus;
            selectedView->FindFocus((IView**)&currentFocus);
            AutoPtr<IViewGroup> theView = IViewGroup::Probe(selectedView);
            AutoPtr<IView> nextFocus;
            FocusFinder::GetInstance()->FindNextFocus(
                    theView, currentFocus, direction, (IView**)&nextFocus);
            if (nextFocus != NULL) {
                currentFocus->GetFocusedRect(mTempRect);
                OffsetDescendantRectToMyCoords(currentFocus, mTempRect);
                OffsetRectIntoDescendantCoords(nextFocus, mTempRect);
                Boolean res = FALSE;
                nextFocus->RequestFocus(direction, mTempRect, &res);
                if (res) {
                    *rst = TRUE;
                    return NOERROR;
                }
            }

            AutoPtr<IView> globalNextFocus;
            FocusFinder::GetInstance()->FindNextFocus(
                    theView, currentFocus, direction, (IView**)&globalNextFocus);
            if (globalNextFocus != NULL) {
                *rst = IsViewAncestorOf(globalNextFocus, this);
                return NOERROR;
            }
        }

    }
    *rst = FALSE;
    return NOERROR;
}

Boolean ListView::ArrowScroll(
    /* [in] */ Int32 direction)
{
    //try{
    mInLayout = TRUE;
    Boolean handled = ArrowScrollImpl(direction);
    if (handled) {
        PlaySoundEffect(SoundEffectConstants::GetContantForFocusDirection(direction));
    }
    mInLayout = FALSE;
    return handled;
    //} finally {
    //    mInLayout = FALSE;
    //}
}

Int32 ListView::NextSelectedPositionForDirection(
    /* [in] */ IView* selectedView,
    /* [in] */ Int32 selectedPos,
    /* [in] */ Int32 direction)
{
    Int32 nextSelected;

    if (direction == IView::FOCUS_DOWN) {
        Int32 height;
        GetHeight(&height);
        const Int32 listBottom = height - TO_CRECT(mListPadding)->mBottom;
        Int32 bottom;
        selectedView->GetBottom(&bottom);
        if (selectedView != NULL && bottom <= listBottom) {
            nextSelected = selectedPos != IAdapterView::INVALID_POSITION && selectedPos >= mFirstPosition ?
                    selectedPos + 1 : mFirstPosition;
        }
        else {
            return IAdapterView::INVALID_POSITION;
        }
    }
    else {
        const Int32 listTop = TO_CRECT(mListPadding)->mTop;
        Int32 top;
        selectedView->GetTop(&top);
        if (selectedView != NULL && top >= listTop) {
            Int32 childCount;
            GetChildCount(&childCount);
            const Int32 lastPos = mFirstPosition + childCount - 1;
            nextSelected = selectedPos != IAdapterView::INVALID_POSITION && selectedPos <= lastPos ?
                    selectedPos - 1 : lastPos;
        }
        else {
            return IAdapterView::INVALID_POSITION;
        }
    }

    Int32 count;
    IAdapter::Probe(mAdapter)->GetCount(&count);
    if (nextSelected < 0 || nextSelected >= count) {
        return IAdapterView::INVALID_POSITION;
    }
    return LookForSelectablePosition(nextSelected, direction == IView::FOCUS_DOWN);
}

Boolean ListView::ArrowScrollImpl(
    /* [in] */ Int32 direction)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount < 0) {
        return FALSE;
    }
    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    Int32 selectedPos = mSelectedPosition;

    Int32 nextSelectedPosition = NextSelectedPositionForDirection(selectedView, selectedPos, direction);
    Int32 amountToScroll = AmountToScroll(direction, nextSelectedPosition);

    AutoPtr<ListView::ArrowScrollFocusResult> focusResult = mItemsCanFocus ?
        ArrowScrollFocused(direction) : NULL;
    if (focusResult != NULL) {
        nextSelectedPosition = focusResult->GetSelectedPosition();
        amountToScroll = focusResult->GetAmountToScroll();
    }

    Boolean needToRedraw = focusResult != NULL;
    if (nextSelectedPosition != IAdapterView::INVALID_POSITION) {
        HandleNewSelectionChange(selectedView, direction, nextSelectedPosition, needToRedraw);
        SetSelectedPositionInt(nextSelectedPosition);
        SetNextSelectedPositionInt(nextSelectedPosition);
        selectedView = NULL;
        GetSelectedView((IView**)&selectedView);
        selectedPos = nextSelectedPosition;
        if (mItemsCanFocus && !needToRedraw) {
            AutoPtr<IView> focused;
            GetFocusedChild((IView**)&focused);
            if (focused != NULL) {
                focused->ClearFocus();
            }
        }

        needToRedraw = TRUE;
        CheckSelectionChanged();
    }

    if (amountToScroll > 0) {
        ScrollListItemsBy((direction == IView::FOCUS_UP) ? amountToScroll : -amountToScroll);
        needToRedraw = TRUE;
    }

    Boolean hasFocus;
    selectedView->HasFocus(&hasFocus);
    if (mItemsCanFocus && (focusResult == NULL)
        && selectedView != NULL && hasFocus) {
        AutoPtr<IView> focused;
        selectedView->FindFocus((IView**)&focused);
        if (!IsViewAncestorOf(focused, this) || DistanceToView(focused) > 0) {
            focused->ClearFocus();
        }
    }

    if (nextSelectedPosition == IAdapterView::INVALID_POSITION && selectedView != NULL
            && !IsViewAncestorOf(selectedView, this)) {
        selectedView = NULL;
        HideSelector();
        mResurrectToPosition = IAdapterView::INVALID_POSITION;
    }

    if (needToRedraw) {
        if (selectedView != NULL) {
            PositionSelectorLikeFocus(selectedPos, selectedView);
            selectedView->GetTop(&mSelectedTop);
        }
        if (!AwakenScrollBars()) {
            Invalidate();
        }

        InvokeOnItemScrollListener();
        return TRUE;
    }

    return FALSE;
}

ECode ListView::HandleNewSelectionChange(
    /* [in] */ IView* selectedView,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 newSelectedPosition,
    /* [in] */ Boolean newFocusAssigned)
{
    if (newSelectedPosition == IAdapterView::INVALID_POSITION) {
        Slogger::E("ListView", "newSelectedPosition needs to be valid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IView> topView;
    AutoPtr<IView> bottomView;
    Int32 topViewIndex = 0;
    Int32 bottomViewIndex = 0;
    Boolean topSelected = FALSE;
    Int32 selectedIndex = mSelectedPosition - mFirstPosition;
    Int32 nextSelectedIndex = newSelectedPosition - mFirstPosition;
    if (direction == IView::FOCUS_UP) {
        topViewIndex = nextSelectedIndex;
        bottomViewIndex = selectedIndex;
        GetChildAt(topViewIndex, (IView**)&topView);
        bottomView = selectedView;
        topSelected = TRUE;
    }
    else {
        topViewIndex = selectedIndex;
        bottomViewIndex = nextSelectedIndex;
        topView = selectedView;
        GetChildAt(bottomViewIndex, (IView**)&bottomView);
    }

    Int32 numChildren;
    GetChildCount(&numChildren);
    if (topView != NULL) {
        topView->SetSelected(!newFocusAssigned && topSelected);
        MeasureAndAdjustDown(topView, topViewIndex, numChildren);
    }

    if (bottomView != NULL) {
        bottomView->SetSelected(!newFocusAssigned && !topSelected);
        MeasureAndAdjustDown(bottomView, bottomViewIndex, numChildren);
    }
    return NOERROR;
}

void ListView::MeasureAndAdjustDown(
    /* [in] */ IView* child,
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 numChildren)
{
    Int32 oldHeight, meaHeight;
    child->GetHeight(&oldHeight);
    MeasureItem(child);
    child->GetMeasuredHeight(&meaHeight);
    if (meaHeight != oldHeight) {
        RelayoutMeasuredItem(child);
        Int32 heightDelta;
        child->GetMeasuredHeight(&heightDelta);
        heightDelta -= oldHeight;
        for(Int32 i = childIndex + 1; i < numChildren; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->OffsetTopAndBottom(heightDelta);
        }
    }
}

void ListView::MeasureItem(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> p;
    child->GetLayoutParams((IViewGroupLayoutParams**)&p);
    if (p == NULL) {
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&p);
    }
    Int32 width;
    p->GetWidth(&width);
    Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(mWidthMeasureSpec,
            TO_CRECT(mListPadding)->mLeft + TO_CRECT(mListPadding)->mRight, width);
    Int32 height;
    p->GetHeight(&height);
    Int32 childHeightSpec;
    if (height > 0) {
        childHeightSpec = View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY);
    }
    else {
        childHeightSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    }
    child->Measure(childWidthSpec, childHeightSpec);
}

void ListView::RelayoutMeasuredItem(
    /* [in] */ IView* child)
{
    Int32 w;
    child->GetMeasuredWidth(&w);
    Int32 h;
    child->GetMeasuredHeight(&h);
    Int32 childLetf = TO_CRECT(mListPadding)->mLeft;
    Int32 childRight = childLetf + w;
    Int32 childTop;
    child->GetTop(&childTop);
    Int32 childBottom = childTop + h;
    child->Layout(childLetf, childTop, childRight, childBottom);
}

Int32 ListView::GetArrowScrollPreviewLength()
{
    Int32 length;
    GetVerticalFadingEdgeLength(&length);
    return Elastos::Core::Math::Max(MIN_SCROLL_PREVIEW_PIXELS, length);
}

Int32 ListView::AmountToScroll(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 nextSelectedPosition)
{
    Int32 height;
    GetHeight(&height);
    Int32 listBottom = height - TO_CRECT(mListPadding)->mBottom;
    Int32 listTop = TO_CRECT(mListPadding)->mTop;

    Int32 numChildren;
    GetChildCount(&numChildren);

    if (direction == IView::FOCUS_DOWN) {
        Int32 indexToMakeVisible = numChildren - 1;
        if (nextSelectedPosition != IAdapterView::INVALID_POSITION) {
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }

        while (numChildren <= indexToMakeVisible) {
            // Child to view is not attached yet.
            AutoPtr<IView> childView;
            GetChildAt(numChildren - 1, (IView**)&childView);
            AddViewBelow(childView, mFirstPosition + numChildren - 1);
            numChildren++;
        }

        Int32 positionToMakeVisible = mFirstPosition + indexToMakeVisible;
        AutoPtr<IView> viewToMakeVisible;
        GetChildAt(indexToMakeVisible, (IView**)&viewToMakeVisible);

        Int32 goalBottom = listBottom;
        if (positionToMakeVisible < mItemCount - 1) {
            goalBottom -= GetArrowScrollPreviewLength();
        }
        Int32 visibleTop, visibleBottom;
        viewToMakeVisible->GetTop(&visibleTop);
        viewToMakeVisible->GetBottom(&visibleBottom);
        if (visibleTop <= goalBottom) {
            return 0;
        }

        Int32 amount;
        GetMaxScrollAmount(&amount);
        if (nextSelectedPosition != IAdapterView::INVALID_POSITION
            &&(goalBottom - visibleTop) >= amount) {
            return 0;
        }

        Int32 amountToScroll = visibleBottom - goalBottom;

        if ((mFirstPosition) == mItemCount) {
            AutoPtr<IView> childView;
            GetChildAt(numChildren - 1, (IView**)&childView);
            Int32 max;
            childView->GetBottom(&max);
            max -= listBottom;
            amountToScroll = Elastos::Core::Math::Min(amountToScroll, max);
        }
        return Elastos::Core::Math::Min(amountToScroll, GetMaxScrollAmount());
    }
    else {
        Int32 indexToMakeVisible = 0;
        if (nextSelectedPosition != IAdapterView::INVALID_POSITION) {
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }

        while (indexToMakeVisible < 0) {
            // Child to view is not attached yet.
            AutoPtr<IView> childView;
            GetChildAt(0, (IView**)&childView);
            AddViewAbove(childView, mFirstPosition);
            mFirstPosition--;
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }

        Int32 positionToMakeVisible = mFirstPosition + indexToMakeVisible;
        AutoPtr<IView> viewToMakeVisible;
        GetChildAt(indexToMakeVisible, (IView**)&viewToMakeVisible);
        Int32 goalTop = listTop;
        if (positionToMakeVisible > 0) {
            goalTop += GetArrowScrollPreviewLength();
        }
        Int32 visibleTop, visibleBottom;
        viewToMakeVisible->GetTop(&visibleTop);
        viewToMakeVisible->GetBottom(&visibleBottom);
        if (visibleTop >= goalTop) {
            return 0;
        }

        if (nextSelectedPosition != IAdapterView::INVALID_POSITION
            && (visibleBottom - goalTop) >= GetMaxScrollAmount()) {
            return 0;
        }

        Int32 amountToScroll = goalTop - visibleTop;
        if (mFirstPosition == 0) {
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            Int32 maxTop = 0;
            child->GetTop(&maxTop);
            Int32 max = listTop - maxTop;
            amountToScroll = Elastos::Core::Math::Min(amountToScroll, max);
        }
        return Elastos::Core::Math::Min(amountToScroll, GetMaxScrollAmount());
    }
}

Int32 ListView::LookForSelectablePositionOnScreen(
    /* [in] */ Int32 direction)
{
    Int32 firstPosition = mFirstPosition;
    if (direction == IView::FOCUS_DOWN) {
        Int32 startPos = (mSelectedPosition != IAdapterView::INVALID_POSITION)
                ? mSelectedPosition + 1 : firstPosition;
        Int32 count = 0;
        IAdapter::Probe(mAdapter)->GetCount(&count);
        if (startPos > count) {
            return IAdapterView::INVALID_POSITION;
        }
        if (startPos < firstPosition) {
            startPos = firstPosition;
        }

        Int32 lastVisiblePosition;
        GetLastVisiblePosition(&lastVisiblePosition);
        AutoPtr<IAdapter> temp;
        GetAdapter((IAdapter**)&temp);
        AutoPtr<IListAdapter> adapter = IListAdapter::Probe(temp);
        for (Int32 pos = startPos; pos <= lastVisiblePosition; pos++) {
            Boolean res = FALSE;
            adapter->IsEnabled(pos, &res);
            AutoPtr<IView> child;
            GetChildAt(pos - firstPosition, (IView**)&child);
            Int32 visible;
            child->GetVisibility(&visible);
            if (res && visible == IView::VISIBLE) {
                return pos;
            }
        }
    }
    else {
        Int32 childCount;
        GetChildCount(&childCount);
        Int32 last = firstPosition + childCount - 1;
        Int32 startPos = (mSelectedPosition != IAdapterView::INVALID_POSITION)
                ? mSelectedPosition - 1 : firstPosition + childCount - 1;
        Int32 count = 0;
        IAdapter::Probe(mAdapter)->GetCount(&count);
        if (startPos < 0 || startPos >= count) {
            return IAdapterView::INVALID_POSITION;
        }
        if (startPos > last) {
            startPos = last;
        }

        AutoPtr<IAdapter> temp;
        GetAdapter((IAdapter**)&temp);
        AutoPtr<IListAdapter> adapter = IListAdapter::Probe(temp);
        for(Int32 pos = startPos; pos <= firstPosition; pos--) {
            Boolean res;
            adapter->IsEnabled(pos, &res);
            AutoPtr<IView> child;
            GetChildAt(pos - firstPosition, (IView**)&child);
            Int32 visible;
            child->GetVisibility(&visible);
            if (res && visible == IView::VISIBLE) {
                return pos;
            }
        }
    }
    return IAdapterView::INVALID_POSITION;
}

AutoPtr<ListView::ArrowScrollFocusResult> ListView::ArrowScrollFocused(
    /* [in] */ Int32 direction)
{
    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    AutoPtr<IView> newFocus;
    Boolean selectedViewHasFocus = FALSE;
    selectedView->HasFocus(&selectedViewHasFocus);
    if (selectedView != NULL && selectedViewHasFocus) {
        AutoPtr<IView> oldFocus;
        selectedView->FindFocus((IView**)&oldFocus);
        FocusFinder::GetInstance()->FindNextFocus(this,
                oldFocus, direction, (IView**)&newFocus);
    }
    else {
        if (direction == IView::FOCUS_DOWN) {
            Boolean topFadingEdgeShowing = (mFirstPosition > 0);
            Int32 listTop = TO_CRECT(mListPadding)->mTop +
                    (topFadingEdgeShowing ? GetArrowScrollPreviewLength() : 0);
            Int32 selectedViewTop = 0;
            selectedView->GetTop(&selectedViewTop);
            Int32 ySearchPoint = (selectedView != NULL && selectedViewTop > listTop) ?
                    selectedViewTop : listTop;
            mTempRect->Set(0, ySearchPoint, 0, ySearchPoint);
        }
        else {
            Int32 childCount;
            GetChildCount(&childCount);
            Boolean bottomFadingEdgeShowing = (mFirstPosition + childCount - 1) < mItemCount;
            Int32 height;
            GetHeight(&height);
            Int32 listBottom = height - TO_CRECT(mListPadding)->mBottom -
                    (bottomFadingEdgeShowing ? GetArrowScrollPreviewLength() : 0);
            Int32 selectedViewBottom = 0;
            selectedView->GetBottom(&selectedViewBottom);
            Int32 ySearchPoint = (selectedView != NULL && selectedViewBottom < listBottom) ?
                    selectedViewBottom : listBottom;
            mTempRect->Set(0, ySearchPoint, 0, ySearchPoint);
        }
        FocusFinder::GetInstance()->FindNextFocusFromRect(this,
                mTempRect, direction, (IView**)&newFocus);
    }

    if (newFocus != NULL) {
        Int32 positionOfNewFocus;
        PositionOfNewFocus(newFocus, &positionOfNewFocus);
        if (mSelectedPosition != IAdapterView::INVALID_POSITION &&
            positionOfNewFocus != mSelectedPosition) {
            Int32 selectablePosition = LookForSelectablePositionOnScreen(direction);
            if (selectablePosition != IAdapterView::INVALID_POSITION &&
                ((direction == IView::FOCUS_DOWN && selectablePosition < positionOfNewFocus) ||
                (direction == IView::FOCUS_UP && selectablePosition > positionOfNewFocus))) {
                return NULL;
            }
        }

        Int32 focusScroll = AmountToScrollToNewFocus(direction, newFocus, positionOfNewFocus);
        Int32 maxScrollAmount;
        GetMaxScrollAmount(&maxScrollAmount);
        if (focusScroll < maxScrollAmount) {
            Boolean res = FALSE;
            newFocus->RequestFocus(direction, &res);
            mArrowScrollFocusResult->Populate(positionOfNewFocus, focusScroll);
            return mArrowScrollFocusResult;
        }
        else if (DistanceToView(newFocus) < maxScrollAmount) {
            Boolean res = FALSE;
            newFocus->RequestFocus(direction, &res);
            mArrowScrollFocusResult->Populate(positionOfNewFocus, maxScrollAmount);
            return mArrowScrollFocusResult;
        }
    }
    return NULL;
}

ECode ListView::PositionOfNewFocus(
    /* [in] */ IView* newFocus,
    /* [out] */ Int32* focus)
{
    VALIDATE_NOT_NULL(focus);
    *focus = 0;

    Int32 numChildren;
    GetChildCount(&numChildren);
    for (Int32 i = 0; i < numChildren; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (IsViewAncestorOf(newFocus, child)) {
            *focus = mFirstPosition + i;
            return NOERROR;
        }
    }

    Slogger::E("ListView", "newFocus is not a child of any of the children of the list!");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

Boolean ListView::IsViewAncestorOf(
    /* [in] */ IView* child,
    /* [in] */ IView* parent)
{
    if (child == parent) {
        return TRUE;
    }
    AutoPtr<IViewParent> theParent;
    child->GetParent((IViewParent**)&theParent);
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(theParent);
    AutoPtr<IView> viewParent = IView::Probe(theParent);
    return (viewGroup != NULL && IsViewAncestorOf(viewParent, parent));
}

Int32 ListView::AmountToScrollToNewFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IView* newFocus,
    /* [in] */ Int32 positionOfNewFocus)
{
    Int32 amountToScroll = 0;
    newFocus->GetDrawingRect(mTempRect);
    OffsetDescendantRectToMyCoords(newFocus, mTempRect);
    if (direction == IView::FOCUS_UP) {
        Int32 top = 0;
        mTempRect->GetTop(&top);
        if (top < TO_CRECT(mListPadding)->mTop) {
            amountToScroll = TO_CRECT(mListPadding)->mTop - top;
            if (positionOfNewFocus > 0) {
                amountToScroll += GetArrowScrollPreviewLength();
            }
        }
    }
    else {
        Int32 height;
        GetHeight(&height);
        Int32 listBottom= height - TO_CRECT(mListPadding)->mBottom;
        Int32 bottom = 0;
        mTempRect->GetBottom(&bottom);
        if (bottom > listBottom) {
            amountToScroll = bottom - listBottom;
            if (positionOfNewFocus < mItemCount - 1) {
                amountToScroll += GetArrowScrollPreviewLength();
            }
        }
    }
    return amountToScroll;
}

Int32 ListView::DistanceToView(
    /* [in] */ IView* descendant)
{
    Int32 distance = 0;
    descendant->GetDrawingRect(mTempRect);
    OffsetDescendantRectToMyCoords(descendant, mTempRect);
    Int32 listBottom = mBottom - mTop - TO_CRECT(mListPadding)->mBottom;
    Int32 top = 0, bottom = 0;
    mTempRect->GetTop(&top);
    mTempRect->GetBottom(&bottom);
    if (bottom < TO_CRECT(mListPadding)->mTop) {
        distance = TO_CRECT(mListPadding)->mTop - bottom;
    }
    else if (top > listBottom) {
        distance = top - listBottom;
    }
    return distance;
}

void ListView::ScrollListItemsBy(
    /* [in] */ Int32 amount)
{
    OffsetChildrenTopAndBottom(amount);
    Int32 height;
    GetHeight(&height);
    Int32 listBottom = height - TO_CRECT(mListPadding)->mBottom;
    Int32 listTop = TO_CRECT(mListPadding)->mTop;

    if (amount < 0) {
        Int32 numChildren;
        GetChildCount(&numChildren);
        AutoPtr<IView> last;
        GetChildAt(numChildren - 1, (IView**)&last);
        Int32 bottom;
        while((last->GetBottom(&bottom), bottom) < listBottom) {
            Int32 lastVisiblePosition = mFirstPosition + numChildren - 1;
            if (lastVisiblePosition < mItemCount - 1) {
                last = AddViewAbove(last, lastVisiblePosition);
                numChildren++;
            }
            else {
                break;
            }
        }

        if ((last->GetBottom(&bottom), bottom) < listBottom) {
            OffsetChildrenTopAndBottom(listBottom - (last->GetBottom(&bottom), bottom));
        }
        AutoPtr<IView> first;
        GetChildAt(0, (IView**)&first);
        while((first->GetBottom(&bottom), bottom) < listTop) {
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            first->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
            AutoPtr<CAbsListViewLayoutParams> lp =
                (CAbsListViewLayoutParams*)IAbsListViewLayoutParams::Probe(layoutParams);
            if (mRecycler->ShouldRecycleViewType(lp->mViewType)) {
                mRecycler->AddScrapView(first, mFirstPosition);
            }

            DetachViewFromParent(first);
            first = NULL;
            GetChildAt(0, (IView**)&first);
            mFirstPosition++;
        }
    }
    else {
        AutoPtr<IView> first;
        GetChildAt(0, (IView**)&first);
        Int32 top;
        while(((first->GetTop(&top), top) > listTop) && (mFirstPosition > 0)) {
            first = AddViewAbove(first, mFirstPosition);
            mFirstPosition--;
        }

        if ((first->GetTop(&top), top) > listTop) {
            OffsetChildrenTopAndBottom(listTop - (first->GetTop(&top), top));
        }

        Int32 childCount;
        GetChildCount(&childCount);
        Int32 lastIndex = childCount - 1;
        AutoPtr<IView> last;
        GetChildAt(lastIndex, (IView**)&last);

        while((last->GetTop(&top), top) > listBottom) {
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            last->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
            AutoPtr<CAbsListViewLayoutParams> lp =
                (CAbsListViewLayoutParams*)IAbsListViewLayoutParams::Probe(layoutParams);
            if (mRecycler->ShouldRecycleViewType(lp->mViewType)) {
                mRecycler->AddScrapView(last, mFirstPosition + lastIndex);
            }

            DetachViewFromParent(last);
            last = NULL;
            GetChildAt(--lastIndex, (IView**)&last);
        }

    }
}

AutoPtr<IView> ListView::AddViewAbove(
    /* [in] */ IView* theView,
    /* [in] */ Int32 position)
{
    Int32 abovePosition = position - 1;
    AutoPtr<IView> view = ObtainView(abovePosition, mIsScrap);
    Int32 edgeOfNewChild = 0;
    theView->GetTop(&edgeOfNewChild);
    edgeOfNewChild -= mDividerHeight;
    SetupChild(view, abovePosition, edgeOfNewChild, FALSE, TO_CRECT(mListPadding)->mLeft,
            FALSE, (*mIsScrap)[0]);
    return view;
}

AutoPtr<IView> ListView::AddViewBelow(
    /* [in] */ IView* theView,
    /* [in] */ Int32 position)
{
    Int32 belowPosition = position + 1;
    AutoPtr<IView> view = ObtainView(belowPosition, mIsScrap);
    Int32 edgeOfNewChild = 0;
    theView->GetTop(&edgeOfNewChild);
    edgeOfNewChild += mDividerHeight;
    SetupChild(view, belowPosition, edgeOfNewChild, TRUE, TO_CRECT(mListPadding)->mLeft,
            FALSE, (*mIsScrap)[0]);
    return view;
}

ECode ListView::SetItemsCanFocus(
    /* [in] */ Boolean itemsCanFocus)
{
    mItemsCanFocus = itemsCanFocus;
    if (!itemsCanFocus) {
        SetDescendantFocusability(ViewGroup::FOCUS_BLOCK_DESCENDANTS);
    }
    return NOERROR;
}

ECode ListView::GetItemsCanFocus(
    /* [out] */ Boolean* canFocus)
{
    VALIDATE_NOT_NULL(canFocus);
    *canFocus = mItemsCanFocus;
    return NOERROR;
}

ECode ListView::IsOpaque(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean isOpaque;
    Boolean retValue = (mCachingActive && mIsCacheColorOpaque && mDividerIsOpaque &&
            HasOpaqueScrollbars()) || (AbsListView::IsOpaque(&isOpaque), isOpaque);
    if (retValue) {
        Int32 listTop = TO_CRECT(mListPadding) != NULL ? TO_CRECT(mListPadding)->mTop : mPaddingTop;
        AutoPtr<IView> first;
        GetChildAt(0, (IView**)&first);
        Int32 firstTop = 0;
        if (first == NULL || (first->GetTop(&firstTop), firstTop) > listTop) {
            *res = FALSE;
            return NOERROR;
        }

        Int32 height;
        GetHeight(&height);
        Int32 listBottom = height - (TO_CRECT(mListPadding) != NULL ?
                TO_CRECT(mListPadding)->mBottom : mPaddingBottom);
        Int32 childCount;
        GetChildCount(&childCount);
        AutoPtr<IView> last;
        GetChildAt(childCount - 1, (IView**)&last);
        Int32 lastBottom = 0;
        last->GetBottom(&lastBottom);
        if (last == NULL || lastBottom < listBottom) {
            *res = FALSE;
            return NOERROR;
        }
    }
    *res = retValue;
    return NOERROR;
}

ECode ListView::SetCacheColorHint(
    /* [in] */ Int32 color)
{
    Boolean opaque = (((UInt32)color) >> 24) == 0XFF;
    mIsCacheColorOpaque = opaque;
    if (opaque) {
        if (mDividerPaint == NULL) {
            FAIL_RETURN(CPaint::New((IPaint**)&mDividerPaint));
        }
        mDividerPaint->SetColor(color);
    }
    AbsListView::SetCacheColorHint(color);
    return NOERROR;
}

void ListView::DrawOverscrollHeader(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IDrawable* drawable,
    /* [in] */ IRect* bounds)
{
    Int32 height = 0;
    drawable->GetMinimumHeight(&height);
    Int32 saveCabvas = 0;
    Boolean clipCanvas = FALSE;
    canvas->Save(&saveCabvas);
    canvas->ClipRect(bounds, &clipCanvas);

    AutoPtr<CRect> rect = (CRect*)IRect::Probe(bounds);
    Int32 span = rect->mTop - rect->mBottom;
    if (span < height) {
        rect->mTop = rect->mBottom - height;
    }

    drawable->SetBounds(bounds);
    drawable->Draw(canvas);

    canvas->Restore();
}

void ListView::DrawOverscrollFooter(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IDrawable* drawable,
    /* [in] */ IRect* bounds)
{
    Int32 height = 0;
    drawable->GetMinimumHeight(&height);
    Int32 saveCabvas = 0;
    Boolean clipCanvas = FALSE;
    canvas->Save(&saveCabvas);
    canvas->ClipRect(bounds, &clipCanvas);

    AutoPtr<CRect> rect = (CRect*)IRect::Probe(bounds);
    Int32 span = rect->mBottom - rect->mTop;
    if (span < height) {
        rect->mBottom = rect->mTop + height;
    }

    drawable->SetBounds(bounds);
    drawable->Draw(canvas);

    canvas->Restore();
}

ECode ListView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mCachingStarted) {
        mCachingActive = TRUE;
    }

    Int32 dividerHeight = mDividerHeight;
    AutoPtr<IDrawable> overscrollHeader = mOverScrollHeader;
    AutoPtr<IDrawable> overscrollFooter = mOverScrollFooter;
    Boolean drawOverscrollHeader = overscrollHeader != NULL;
    Boolean drawOverscrollFooter = overscrollFooter != NULL;
    Boolean drawDividers = dividerHeight > 0 && mDivider != NULL;

    if (drawDividers || drawOverscrollHeader || drawOverscrollFooter) {
        AutoPtr<CRect> bounds = TO_CRECT(mTempRect);
        bounds->mLeft = mPaddingLeft;
        bounds->mRight = mRight - mLeft -mPaddingRight;

        Int32 count;
        GetChildCount(&count);
        Int32 headerCount;
        mHeaderViewInfos->GetSize(&headerCount);
        Int32 itemCount = mItemCount;
        Int32 footerCount;
        mFooterViewInfos->GetSize(&footerCount);
        Int32 footerLimit = itemCount - footerCount;
        Boolean headerDividers = mHeaderDividersEnabled;
        Boolean footerDividers = mFooterDividersEnabled;
        Int32 first = mFirstPosition;
        AutoPtr<IListAdapter> adapter = mAdapter;
        Boolean res, res1;
        IsOpaque(&res);
        AbsListView::IsOpaque(&res1);
        Boolean fillForMissingDividers = res && !res1;

        if (fillForMissingDividers && mDividerPaint == NULL && mIsCacheColorOpaque) {
            CPaint::New((IPaint**)&mDividerPaint);
            Int32 hint;
            GetCacheColorHint(&hint);
            mDividerPaint->SetColor(hint);
        }
        AutoPtr<IPaint> paint = mDividerPaint;
        Int32 effectivePaddingTop = 0;
        Int32 effectivePaddingBottom = 0;
        if ((mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK) {
            effectivePaddingTop = TO_CRECT(mListPadding)->mTop;
            effectivePaddingBottom = TO_CRECT(mListPadding)->mBottom;
        }
        Int32 listBottom = mBottom - mTop - effectivePaddingBottom +mScrollY;
        if (!mStackFromBottom) {
            Int32 bottom = 0;
            Int32 scrollY = mScrollY;
            if (count > 0 && scrollY < 0) {
                if (drawOverscrollHeader) {
                    bounds->mBottom = 0;
                    bounds->mTop = scrollY;
                    DrawOverscrollHeader(canvas, overscrollHeader, bounds);
                }
                else if (drawDividers) {
                    bounds->mBottom = 0;
                    bounds->mTop = -dividerHeight;
                    DrawDivider(canvas, bounds, -1);
                }
            }

            for(Int32 i = 0; i < count; i++) {
                const Int32 itemIndex = (first + i);
                const Boolean isHeader = (itemIndex < headerCount);
                const Boolean isFooter = (itemIndex >= footerLimit);

                if ((headerDividers || !isHeader) && (footerDividers || !isFooter)) {
                    AutoPtr<IView> child;
                    GetChildAt(i, (IView**)&child);
                    child->GetBottom(&bottom);
                    const Boolean isLastItem = (i == (count - 1));

                    if (drawDividers && (bottom < listBottom)
                        && !(drawOverscrollFooter && isLastItem)) {
                        const Int32 nextIndex = (itemIndex + 1);
                        // Draw dividers between enabled items, headers
                        // and/or footers when enabled and requested, and
                        // after the last enabled item.
                        Boolean enabled = FALSE;
                        Boolean addEnabled = FALSE;
                        adapter->IsEnabled(itemIndex, &enabled);
                        adapter->IsEnabled(nextIndex, &addEnabled);

                        if (enabled && (headerDividers || (!isHeader
                                && (nextIndex >= headerCount)))
                                && (isLastItem || (addEnabled && (footerDividers
                                || (!isFooter && (nextIndex < footerLimit)))))) {
                            bounds->mTop = bottom;
                            bounds->mBottom = bottom + dividerHeight;
                            DrawDivider(canvas, bounds, i);
                        }
                        else if (fillForMissingDividers) {
                            bounds->mTop = bottom;
                            bounds->mBottom = bottom + dividerHeight;
                            canvas->DrawRect(bounds, paint);
                        }
                    }
                }
            }

            Int32 overFooterBottom = mBottom + mScrollY;
            if (drawOverscrollFooter && first + count == itemCount &&
                overFooterBottom > bottom) {
                bounds->mTop = bottom;
                bounds->mBottom = overFooterBottom;
                DrawOverscrollFooter(canvas, overscrollFooter, bounds);
            }
        }
        else {
            Int32 top = 0;
            Int32 scrollY = mScrollY;
            if (count > 0 && drawOverscrollHeader) {
                bounds->mTop = scrollY;
                AutoPtr<IView> view;
                GetChildAt(0, (IView**)&view);
                view->GetTop(&(bounds->mBottom));
                DrawOverscrollHeader(canvas, overscrollHeader, bounds);
            }

            Int32 start = drawOverscrollHeader ? 1 : 0;
            for(Int32 i = start; i < count; i++) {
                const Int32 itemIndex = (first + i);
                const Boolean isHeader = (itemIndex < headerCount);
                const Boolean isFooter = (itemIndex >= footerLimit);
                if ((headerDividers || !isHeader) && (footerDividers || !isFooter)) {
                    AutoPtr<IView> child;
                    GetChildAt(i, (IView**)&child);
                    child->GetTop(&top);

                    Boolean enabled = FALSE;
                    Boolean addEnabled = FALSE;
                    if (drawDividers && (top > effectivePaddingTop)) {
                        const Boolean isFirstItem = (i == start);
                        const Int32 previousIndex = (itemIndex - 1);
                        // Draw dividers between enabled items, headers
                        // and/or footers when enabled and requested, and
                        // before the first enabled item.
                        adapter->IsEnabled(itemIndex, &enabled);
                        adapter->IsEnabled(previousIndex, &addEnabled);
                        if (enabled && (headerDividers || (!isHeader
                                && (previousIndex >= headerCount)))
                                && (isFirstItem || (addEnabled && (footerDividers || (!isFooter
                                && (previousIndex < footerLimit)))))) {
                            bounds->mTop = top - dividerHeight;
                            bounds->mBottom = top;
                            DrawDivider(canvas, bounds, i - 1);
                        }
                        else if (fillForMissingDividers) {
                            bounds->mTop = top - dividerHeight;
                            bounds->mBottom = top;
                            canvas->DrawRect(bounds, paint);
                        }
                    }
                }
            }

            if (count > 0 && scrollY > 0) {
                if (drawOverscrollFooter) {
                    Int32 absListBottom = mBottom;
                    bounds->mTop = absListBottom;
                    bounds->mBottom = absListBottom + scrollY;
                    DrawOverscrollFooter(canvas, overscrollFooter, bounds);
                }
                else if (drawDividers) {
                    bounds->mTop = listBottom;
                    bounds->mBottom = listBottom + dividerHeight;
                    DrawDivider(canvas, bounds, -1);
                }
            }
        }
    }

    return AbsListView::DispatchDraw(canvas);
}

Boolean ListView::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    Boolean more = AbsListView::DrawChild(canvas, child, drawingTime);
    View* view = VIEW_PROBE(child);
    if (mCachingActive && view->mCachingFailed) {
        mCachingActive = FALSE;
    }
    return more;
}

void ListView::DrawDivider(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ Int32 childIndex)
{
    AutoPtr<IDrawable> divider = mDivider;
    divider->SetBounds(bounds);
    divider->Draw(canvas);
}

ECode ListView::GetDivider(
    /* [out] */ IDrawable** divider)
{
    VALIDATE_NOT_NULL(divider);
    *divider = mDivider;
    REFCOUNT_ADD(*divider);
    return NOERROR;
}

ECode ListView::SetDivider(
    /* [in] */ IDrawable* divider)
{
    if (divider != NULL) {
        divider->GetIntrinsicHeight(&mDividerHeight);
    }
    else {
        mDividerHeight = 0;
    }

    mDivider = divider;
    Int32 opacity = 0;
    if(divider) {
        divider->GetOpacity(&opacity);
        mDividerIsOpaque = opacity == IPixelFormat::OPAQUE;
    }
    else {
        mDividerIsOpaque = TRUE;
    }
    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode ListView::GetDividerHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mDividerHeight;
    return NOERROR;
}

ECode ListView::SetDividerHeight(
    /* [in] */ Int32 height)
{
    mDividerHeight = height;
    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode ListView::SetHeaderDividersEnabled(
    /* [in] */ Boolean headerDividersEnabled)
{
    mHeaderDividersEnabled = headerDividersEnabled;
    Invalidate();
    return NOERROR;
}

ECode ListView::AreHeaderDividersEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mHeaderDividersEnabled;
    return NOERROR;
}

ECode ListView::SetFooterDividersEnabled(
    /* [in] */ Boolean footerDividersEnabled)
{
    mFooterDividersEnabled = footerDividersEnabled;
    Invalidate();
    return NOERROR;
}

ECode ListView::AreFooterDividersEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mFooterDividersEnabled;
    return NOERROR;
}

ECode ListView::SetOverscrollHeader(
    /* [in] */ IDrawable* header)
{
    mOverScrollHeader = header;
    if (mScrollY < 0) {
        Invalidate();
    }
    return NOERROR;
}

ECode ListView::GetOverscrollHeader(
    /* [out] */ IDrawable** overScrollHeader)
{
    VALIDATE_NOT_NULL(overScrollHeader);
    *overScrollHeader = mOverScrollHeader;
    REFCOUNT_ADD(*overScrollHeader)
    return NOERROR;
}

ECode ListView::SetOverscrollFooter(
    /* [in] */ IDrawable* footer)
{
    mOverScrollFooter = footer;
    Invalidate();
    return NOERROR;
}

ECode ListView::GetOverscrollFooter(
    /* [out] */ IDrawable** overScrollFooter)
{
    VALIDATE_NOT_NULL(overScrollFooter)
    *overScrollFooter = mOverScrollFooter;
    REFCOUNT_ADD(*overScrollFooter)
    return NOERROR;
}

void ListView::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AbsListView::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);
    AutoPtr<IListAdapter> adapter = mAdapter;
    Int32 closetChildIndex = -1;
    Int32 closestChildTop = 0;
    if (adapter != NULL && gainFocus && previouslyFocusedRect != NULL) {
        previouslyFocusedRect->Offset(mScrollX, mScrollY);
        Int32 count = 0;
        IAdapter::Probe(adapter)->GetCount(&count);
        Int32 numChildren;
        GetChildCount(&numChildren);
        if (count < numChildren + mFirstPosition) {
            mLayoutMode = AbsListView::LAYOUT_NORMAL;
            LayoutChildren();
        }

        AutoPtr<CRect> otherRect = TO_CRECT(mTempRect);
        Int32 minDistance = Elastos::Core::Math::INT32_MAX_VALUE;
        Int32 childCount;
        GetChildCount(&childCount);
        Int32 firstPosition = mFirstPosition;

        for(Int32 i = 0; i < childCount; i++) {
            Boolean res = FALSE;
            adapter->IsEnabled(firstPosition + i, &res);
            if (res) {
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
                closetChildIndex = i;
                other->GetTop(&closestChildTop);
            }
        }
    }

    if (closetChildIndex >= 0) {
        SetSelectionFromTop(closetChildIndex + mFirstPosition, closestChildTop);
    }
    else {
        RequestLayout();
    }
}

ECode ListView::OnFinishInflate()
{
    AbsListView::OnFinishInflate();
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        for(Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AddHeaderView(child);
        }
        RemoveAllViews();
    }
    return NOERROR;
}

AutoPtr<IView> ListView::FindViewTraversal(
    /* [in] */ Int32 id)
{
    AutoPtr<IView> v = NULL;
    v = AbsListView::FindViewTraversal(id);
    if (v == NULL) {
        v = FindViewInHeadersOrFooters(mHeaderViewInfos, id);
        if (v != NULL) {
            return v;
        }
        v = FindViewInHeadersOrFooters(mFooterViewInfos, id);
        if (v != NULL) {
            return v;
        }
    }
    return v;
}

AutoPtr<IView> ListView::FindViewInHeadersOrFooters(
    /* [in] */ IArrayList* where,
    /* [in] */ Int32 id)
{
    if (where != NULL) {
        Int32 len;
        where->GetSize(&len);
        AutoPtr<IView> v;

        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            where->Get(i, (IInterface**)&obj);
            v = NULL;
            IFixedViewInfo::Probe(obj)->GetView((IView**)&v);

            Boolean res = FALSE;
            v->IsRootNamespace(&res);
            if (!res) {
                AutoPtr<IView> subV;
                v->FindViewById(id, (IView**)&subV);

                if (subV != NULL) {
                    return subV;
                }
            }
        }
    }

    return NULL;
}

AutoPtr<IView> ListView::FindViewWithTagTraversal(
    /* [in] */ IInterface* tag)
{
    AutoPtr<IView> v = NULL;
    v = AbsListView::FindViewWithTagTraversal(tag);
    if (v == NULL) {
        v = FindViewWithTagInHeadersOrFooters(mHeaderViewInfos, tag);
        if (v != NULL) return v;

        v = FindViewWithTagInHeadersOrFooters(mFooterViewInfos, tag);
        if (v != NULL) return v;
    }
    return v;
}

AutoPtr<IView> ListView::FindViewWithTagInHeadersOrFooters(
    /* [in] */ IArrayList* where,
    /* [in] */ IInterface* tag)
{
    if (where != NULL) {
        Int32 len;
        where->GetSize(&len);
        AutoPtr<IView> v;

        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            where->Get(i, (IInterface**)&obj);
            v = NULL;
            IFixedViewInfo::Probe(obj)->GetView((IView**)&v);

            Boolean res = FALSE;
            v->IsRootNamespace(&res);
            if (!res) {
                AutoPtr<IView> subV;
                v->FindViewWithTag(tag, (IView**)&subV);

                if (subV != NULL) {
                    return subV;
                }
            }
        }
    }

    return NULL;
}

AutoPtr<IView> ListView::FindViewByPredicateTraversal(
    /* [in] */ IPredicate* predicate,
    /* [in] */ IView* childToSkip)
{
    AutoPtr<IView> v = NULL;
    v = AbsListView::FindViewByPredicateTraversal(predicate, childToSkip);
    if (v == NULL) {
        v = FindViewByPredicateInHeadersOrFooters(mHeaderViewInfos, predicate, childToSkip);
        if (v != NULL) return v;

        v = FindViewByPredicateInHeadersOrFooters(mFooterViewInfos, predicate, childToSkip);
        if (v != NULL) return v;
    }
    return v;
}

AutoPtr<IView> ListView::FindViewByPredicateInHeadersOrFooters(
    /* [in] */ IArrayList* where,
    /* [in] */ IPredicate* predicate,
    /* [in] */ IView* childToSkip)
{
    if (where != NULL) {
        Int32 len;
        where->GetSize(&len);
        AutoPtr<IView> v;

        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            where->Get(i, (IInterface**)&obj);
            v = NULL;
            IFixedViewInfo::Probe(obj)->GetView((IView**)&v);

            Boolean res = FALSE;
            v->IsRootNamespace(&res);
            if (v.Get() != childToSkip && !res) {
                AutoPtr<IView> subV;
                v->FindViewByPredicate(predicate, (IView**)&subV);

                if (subV != NULL) {
                    return subV;
                }
            }
        }
    }

   return NULL;
}

ECode ListView::GetCheckItemIds(
    /* [out, callee] */ ArrayOf<Int64>** itemIds)
{
    VALIDATE_NOT_NULL(itemIds);
    itemIds = NULL;

    Boolean res = FALSE;
    AutoPtr<ArrayOf<Int64> > array;
    IAdapter::Probe(mAdapter)->HasStableIds(&res);
    if (mAdapter != NULL && res) {
        GetCheckedItemIds((ArrayOf<Int64>**)&array);
        *itemIds = array;
        REFCOUNT_ADD(*itemIds);
        return NOERROR;
    }

    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE && mCheckStates != NULL && mAdapter != NULL) {
        AutoPtr<ISparseBooleanArray> states = mCheckStates;
        Int32 count;
        states->GetSize(&count);
        AutoPtr<ArrayOf<Int64> > ids = ArrayOf<Int64>::Alloc(count);
        AutoPtr<IListAdapter> adapter = mAdapter;

        Int32 checkedCount = 0;
        for (Int32 i = 0; i < count; i++) {
            Boolean value;
            states->ValueAt(i, &value);
            if (value) {
                Int32 key;
                states->KeyAt(i, &key);
                IAdapter::Probe(adapter)->GetItemId(key, &((*ids)[checkedCount++]));
            }
        }

        if (checkedCount == count) {
            *itemIds = ids;
            REFCOUNT_ADD(*itemIds);
            return NOERROR;
        }
        else {
            AutoPtr<ArrayOf<Int64> > result = ArrayOf<Int64>::Alloc(checkedCount);
            result->Copy(0, ids, 0, checkedCount);
            *itemIds = result;
            REFCOUNT_ADD(*itemIds);
            return NOERROR;
        }
    }
    return NOERROR;
}

Int32 ListView::GetHeightForPosition(
    /* [in] */ Int32 position)
{
    const Int32 height = AbsListView::GetHeightForPosition(position);
    if (ShouldAdjustHeightForDivider(position)) {
        return height + mDividerHeight;
    }
    return height;
}

Boolean ListView::ShouldAdjustHeightForDivider(
    /* [in] */ Int32 itemIndex)
{
    const Int32 dividerHeight = mDividerHeight;
    const AutoPtr<IDrawable> overscrollHeader = mOverScrollHeader;
    const AutoPtr<IDrawable> overscrollFooter = mOverScrollFooter;
    const Boolean drawOverscrollHeader = overscrollHeader != NULL;
    const Boolean drawOverscrollFooter = overscrollFooter != NULL;
    const Boolean drawDividers = dividerHeight > 0 && mDivider != NULL;

    if (drawDividers) {
        Boolean res1, res2;
        IsOpaque(&res1);
        AbsListView::IsOpaque(&res2);

        const Boolean fillForMissingDividers = res1 && !res2;
        const Int32 itemCount = mItemCount;
        Int32 headerCount;
        mHeaderViewInfos->GetSize(&headerCount);
        Int32 footerCount;
        mFooterViewInfos->GetSize(&footerCount);
        const Int32 footerLimit = (itemCount - footerCount);
        const Boolean isHeader = (itemIndex < headerCount);
        const Boolean isFooter = (itemIndex >= footerLimit);
        const Boolean headerDividers = mHeaderDividersEnabled;
        const Boolean footerDividers = mFooterDividersEnabled;
        if ((headerDividers || !isHeader) && (footerDividers || !isFooter)) {
            const AutoPtr<IListAdapter> adapter = mAdapter;
            if (!mStackFromBottom) {
                const Boolean isLastItem = (itemIndex == (itemCount - 1));
                if (!drawOverscrollFooter || !isLastItem) {
                    const Int32 nextIndex = itemIndex + 1;
                    // Draw dividers between enabled items, headers
                    // and/or footers when enabled and requested, and
                    // after the last enabled item.
                    Boolean enabled1, enabled2;
                    adapter->IsEnabled(itemIndex, &enabled1);
                    adapter->IsEnabled(nextIndex, &enabled2);
                    if (enabled1 && (headerDividers || (!isHeader
                            && (nextIndex >= headerCount))) && (isLastItem
                            || (enabled2 && (footerDividers || (!isFooter
                            && (nextIndex < footerLimit)))))) {
                        return TRUE;
                    }
                    else if (fillForMissingDividers) {
                        return TRUE;
                    }
                }
            }
            else {
                const Int32 start = drawOverscrollHeader ? 1 : 0;
                const Boolean isFirstItem = (itemIndex == start);
                if (!isFirstItem) {
                    const Int32 previousIndex = (itemIndex - 1);
                    // Draw dividers between enabled items, headers
                    // and/or footers when enabled and requested, and
                    // before the first enabled item.
                    Boolean enabled1, enabled2;
                    adapter->IsEnabled(itemIndex, &enabled1);
                    adapter->IsEnabled(previousIndex, &enabled2);
                    if (enabled1 && (headerDividers || (!isHeader
                            && (previousIndex >= headerCount))) && (isFirstItem ||
                            (enabled2 && (footerDividers || (!isFooter
                            && (previousIndex < footerLimit)))))) {
                        return TRUE;
                    }
                    else if (fillForMissingDividers) {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}

ECode ListView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsListView::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert(LISTVIEW_NAME));
    return NOERROR;
}

ECode ListView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsListView::OnInitializeAccessibilityNodeInfo(info);
    info->SetClassName(CoreUtils::Convert(LISTVIEW_NAME));

    Int32 rowsCount;
    GetCount(&rowsCount);
    const Int32 selectionMode = GetSelectionModeForAccessibility();

    AutoPtr<IAccessibilityNodeInfoCollectionInfo> collectionInfo;
    AccessibilityNodeInfoCollectionInfo::Obtain(rowsCount, 1, FALSE,
            selectionMode, (IAccessibilityNodeInfoCollectionInfo**)&collectionInfo);
    info->SetCollectionInfo(collectionInfo);

    return NOERROR;
}

ECode ListView::OnInitializeAccessibilityNodeInfoForItem(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsListView::OnInitializeAccessibilityNodeInfoForItem(view, position, info);

    AutoPtr<IViewGroupLayoutParams> params;
    view->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<IAbsListViewLayoutParams> lp = IAbsListViewLayoutParams::Probe(params);

    Boolean isHeading = FALSE;
    if (lp != NULL) {
        Int32 viewType;
        lp->GetViewType(&viewType);
        isHeading = viewType != IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER;
    }
    Boolean isSelected;
    IsItemChecked(position, &isSelected);
    AutoPtr<IAccessibilityNodeInfoCollectionItemInfo> itemInfo;

    AccessibilityNodeInfoCollectionItemInfo::Obtain(position, 1, 0, 1, isHeading,
            isSelected, (IAccessibilityNodeInfoCollectionItemInfo**)&itemInfo);
    info->SetCollectionItemInfo(itemInfo);
    return NOERROR;
}

}// amespace Widget
}// namespace Droid
}// namespace Elastos
