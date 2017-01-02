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

#include "elastos/droid/widget/TabWidget.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/widget/CImageView.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              TabWidget::TabListener
//==============================================================================

CAR_INTERFACE_IMPL_2(TabWidget::TabListener, Object, IViewOnClickListener, IViewOnFocusChangeListener)

TabWidget::TabListener::TabListener(
    /* [in] */ Int32 tabIndex,
    /* [in] */ TabWidget* owner)
    : mTabIndex(tabIndex)
    , mHost(owner)
{}

ECode TabWidget::TabListener::OnClick(
     /* [in] */ IView* v)
{
    return mHost->mSelectionChangedListener->OnTabSelectionChanged(mTabIndex, TRUE);
}

ECode TabWidget::TabListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    return mHost->OnFocusChange(v, hasFocus);
}


//==============================================================================
//              TabWidget
//==============================================================================
CAR_INTERFACE_IMPL(TabWidget, LinearLayout, ITabWidget)

TabWidget::TabWidget()
    : mSelectedTab(-1)
    , mDrawBottomStrips(TRUE)
    , mStripMoved(FALSE)
    , mImposedTabsHeight(-1)
{
    CRect::New((IRect**)&mBounds);
}

ECode TabWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr< ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TabWidget);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
        attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    Boolean res;
    a->GetBoolean(R::styleable::TabWidget_tabStripEnabled, TRUE, &res);
    SetStripEnabled(res);

    AutoPtr<IDrawable> dr;
    a->GetDrawable(R::styleable::TabWidget_tabStripLeft, (IDrawable**)&dr);
    SetLeftStripDrawable(dr);
    dr = NULL;
    a->GetDrawable(R::styleable::TabWidget_tabStripRight, (IDrawable**)&dr);
    SetRightStripDrawable(dr);

    a->Recycle();
    return NOERROR;
}

ECode TabWidget::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    mStripMoved = TRUE;
    return LinearLayout::OnSizeChanged(w, h, oldw, oldh);
}

Int32 TabWidget::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    if (mSelectedTab == -1) {
        return i;
    } else {
        // Always draw the selected tab last, so that drop shadows are drawn
        // in the correct z-order.
        if (i == childCount - 1) {
            return mSelectedTab;
        }
        else if (i >= mSelectedTab) {
            return i + 1;
        }
        else {
            return i;
        }
    }
}

void TabWidget::InitTabWidget()
{
    SetChildrenDrawingOrderEnabled(TRUE);

    IContext* context = mContext;

    AutoPtr<IApplicationInfo> ai;
    context->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 version;
    ai->GetTargetSdkVersion(&version);
    if (version <= Build::VERSION_CODES::DONUT) {
        // Donut apps get old color scheme
        if (mLeftStrip == NULL) {
            context->GetDrawable(R::drawable::tab_bottom_left_v4, (IDrawable**)&mLeftStrip);
        }
        if (mRightStrip == NULL) {
            context->GetDrawable(R::drawable::tab_bottom_right_v4, (IDrawable**)&mRightStrip);
        }
    }
    else {
        // Use modern color scheme for Eclair and beyond
        if (mLeftStrip == NULL) {
            context->GetDrawable(R::drawable::tab_bottom_left, (IDrawable**)&mLeftStrip);
        }
        if (mRightStrip == NULL) {
            context->GetDrawable(R::drawable::tab_bottom_right, (IDrawable**)&mRightStrip);
        }
    }

    // Deal with focus, as we don't want the focus to go by default
    // to a tab other than the current tab
    SetFocusable(TRUE);
    AutoPtr<TabListener> listener = new TabListener(0, this);
    SetOnFocusChangeListener(listener);
}

void TabWidget::MeasureChildBeforeLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 totalWidth,
    /* [in] */ Int32 heightMeasureSpec,
    /* [in] */ Int32 totalHeight)
{
    Boolean enabled;
    if ((IsMeasureWithLargestChildEnabled(&enabled), enabled)
        && mImposedTabsHeight >= 0) {
        widthMeasureSpec = MeasureSpec::MakeMeasureSpec(
            totalWidth + (*mImposedTabWidths)[childIndex], MeasureSpec::EXACTLY);
        heightMeasureSpec = MeasureSpec::MakeMeasureSpec(mImposedTabsHeight,
            MeasureSpec::EXACTLY);
    }

    LinearLayout::MeasureChildBeforeLayout(child, childIndex,
        widthMeasureSpec, totalWidth, heightMeasureSpec, totalHeight);
}

void TabWidget::MeasureHorizontal(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (MeasureSpec::GetMode(widthMeasureSpec) == MeasureSpec::UNSPECIFIED) {
        LinearLayout::MeasureHorizontal(widthMeasureSpec, heightMeasureSpec);
        return;
    }

    // First, measure with no constraint
    Int32 unspecifiedWidth = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
    mImposedTabsHeight = -1;
    LinearLayout::MeasureHorizontal(unspecifiedWidth, heightMeasureSpec);

    Int32 measureWidth;
    GetMeasuredWidth(&measureWidth);
    Int32 extraWidth = measureWidth - MeasureSpec::GetSize(widthMeasureSpec);
    if (extraWidth > 0) {
        Int32 count;
        GetChildCount(&count);

        Int32 childCount = 0;
        AutoPtr<IView> child;
        for (Int32 i = 0; i < count; i++) {
            child = NULL;
            GetChildAt(i, (IView**)&child);
            Int32 visible;
            child->GetVisibility(&visible);

            if (visible == IView::GONE) continue;
            childCount++;
        }

        if (childCount > 0) {
            if (mImposedTabWidths == NULL || mImposedTabWidths->GetLength() != count) {
                mImposedTabWidths = ArrayOf<Int32>::Alloc(count);
            }
            Int32 visibility;
            AutoPtr<IView> child;
            for (Int32 i = 0; i < count; i++) {
                child = NULL;
                GetChildAt(i, (IView**)&child);
                child->GetVisibility(&visibility);
                if (visibility == IView::GONE) continue;
                Int32 childWidth, childHeight;
                child->GetMeasuredWidth(&childWidth);
                child->GetMeasuredHeight(&childHeight);
                Int32 delta = extraWidth / childCount;
                Int32 newWidth = Elastos::Core::Math::Max(0, childWidth - delta);
                (*mImposedTabWidths)[i] = newWidth;
                // Make sure the extra width is evenly distributed, no Int32 division remainder
                extraWidth -= childWidth - newWidth; // delta may have been clamped
                childCount--;
                mImposedTabsHeight = Elastos::Core::Math::Max(mImposedTabsHeight, childHeight);
            }
        }
    }

    // Measure again, this time with imposed tab widths and respecting initial spec request
    LinearLayout::MeasureHorizontal(widthMeasureSpec, heightMeasureSpec);
}

/**
 * Returns the tab indicator view at the given index.
 *
 * @param index the zero-based index of the tab indicator view to return
 * @return the tab indicator view at the given index
 */
ECode TabWidget::GetChildTabViewAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v)
    return GetChildAt(index, v);
}

/**
 * Returns the number of tab indicator views.
 * @return the number of tab indicator views.
 */
ECode TabWidget::GetTabCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return GetChildCount(count);
}

/**
 * Sets the drawable to use as a divider between the tab indicators.
 * @param drawable the divider drawable
 */
ECode TabWidget::SetDividerDrawable(
    /* [in] */ IDrawable* drawable)
{
    return LinearLayout::SetDividerDrawable(drawable);
}

/**
 * Sets the drawable to use as a divider between the tab indicators.
 * @param resId the resource identifier of the drawable to use as a
 * divider.
 */
ECode TabWidget::SetDividerDrawable(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> dr;
    mContext->GetDrawable(resId, (IDrawable**)&dr);

    return SetDividerDrawable(dr);
}

/**
 * Sets the drawable to use as the left part of the strip below the
 * tab indicators.
 * @param drawable the left strip drawable
 */
ECode TabWidget::SetLeftStripDrawable(
    /* [in] */ IDrawable* drawable)
{
    mLeftStrip = drawable;
    RequestLayout();
    Invalidate();

    return NOERROR;
}

/**
 * Sets the drawable to use as the left part of the strip below the
 * tab indicators.
 * @param resId the resource identifier of the drawable to use as the
 * left strip drawable
 */
ECode TabWidget::SetLeftStripDrawable(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> dr;
    mContext->GetDrawable(resId, (IDrawable**)&dr);

    return SetLeftStripDrawable(dr);
}

/**
 * Sets the drawable to use as the right part of the strip below the
 * tab indicators.
 * @param drawable the right strip drawable
 */
ECode TabWidget::SetRightStripDrawable(
    /* [in] */ IDrawable* drawable)
{
    mRightStrip = drawable;
    RequestLayout();
    Invalidate();

    return NOERROR;
}

/**
 * Sets the drawable to use as the right part of the strip below the
 * tab indicators.
 * @param resId the resource identifier of the drawable to use as the
 * right strip drawable
 */
ECode TabWidget::SetRightStripDrawable(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> dr;
    mContext->GetDrawable(resId, (IDrawable**)&dr);

    return SetRightStripDrawable(dr);
}

/**
 * Controls whether the bottom strips on the tab indicators are drawn or
 * not.  The default is to draw them.  If the user specifies a custom
 * view for the tab indicators, then the TabHost class calls this method
 * to disable drawing of the bottom strips.
 * @param stripEnabled TRUE if the bottom strips should be drawn.
 */
ECode TabWidget::SetStripEnabled(
    /* [in] */ Boolean stripEnabled)
{
    mDrawBottomStrips = stripEnabled;
    Invalidate();

    return NOERROR;
}

/**
 * Indicates whether the bottom strips on the tab indicators are drawn
 * or not.
 */
ECode TabWidget::IsStripEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mDrawBottomStrips;
    return NOERROR;
}

ECode TabWidget::ChildDrawableStateChanged(
    /* [in] */ IView* child)
{
    if (mSelectedTab != -1) {
        Int32 count;
        AutoPtr<IView> v;
        if ((GetTabCount(&count), count) > 0
                && child == (GetChildTabViewAt(mSelectedTab, (IView**)&v),  v)) {
            // To make sure that the bottom strip is redrawn
            Invalidate();
        }
    }
    return LinearLayout::ChildDrawableStateChanged(child);
}

ECode TabWidget::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::DispatchDraw(canvas);

    // Do nothing if there are no tabs.
    Int32 count;
    if ((GetTabCount(&count), count) == 0) return NOERROR;

    // If the user specified a custom view for the tab indicators, then
    // do not draw the bottom strips.
    if (!mDrawBottomStrips) {
        // Skip drawing the bottom strips.
        return NOERROR;
    }

    AutoPtr<IView> selectedChild;
    GetChildTabViewAt(mSelectedTab, (IView**)&selectedChild);

    IDrawable* leftStrip = mLeftStrip;
    IDrawable* rightStrip = mRightStrip;

    AutoPtr< ArrayOf<Int32> > drawableState;
    selectedChild->GetDrawableState((ArrayOf<Int32>**)&drawableState);

    Boolean res;
    leftStrip->SetState(drawableState, &res);
    rightStrip->SetState(drawableState, &res);

    if (mStripMoved) {
        IRect* bounds = mBounds;
        Int32 l, r;
        selectedChild->GetLeft(&l);
        selectedChild->GetRight(&r);
        bounds->SetLeft(l);
        bounds->SetRight(r);

        Int32 myHeight;
        GetHeight(&myHeight);

        Int32 w, h;
        leftStrip->GetIntrinsicWidth(&w);
        leftStrip->GetIntrinsicHeight(&h);
        leftStrip->SetBounds(Elastos::Core::Math::Min(0, l - w),
                myHeight - h, l, myHeight);

        rightStrip->GetIntrinsicWidth(&w);
        rightStrip->GetIntrinsicHeight(&h);
        Int32 width;
        GetWidth(&width);
        rightStrip->SetBounds(r, myHeight - h,
            Elastos::Core::Math::Max(width, r + w), myHeight);
        mStripMoved = FALSE;
    }

    leftStrip->Draw(canvas);
    rightStrip->Draw(canvas);
    return NOERROR;
}

/**
 * Sets the current tab.
 * This method is used to bring a tab to the front of the Widget,
 * and is used to post to the rest of the UI that a different tab
 * has been brought to the foreground.
 *
 * Note, this is separate from the traditional "focus" that is
 * employed from the view logic.
 *
 * For instance, if we have a list in a tabbed view, a user may be
 * navigating up and down the list, moving the UI focus (orange
 * highlighting) through the list items.  The cursor movement does
 * not effect the "selected" tab though, because what is being
 * scrolled through is all on the same tab.  The selected tab only
 * changes when we navigate between tabs (moving from the list view
 * to the next tabbed view, in this example).
 *
 * To move both the focus AND the selected tab at once, please use
 * {@link #setCurrentTab}. Normally, the view logic takes care of
 * adjusting the focus, so unless you're circumventing the UI,
 * you'll probably just focus your interest here.
 *
 *  @param index The tab that you want to indicate as the selected
 *  tab (tab brought to the front of the widget)
 *
 *  @see #focusCurrentTab
 */
ECode TabWidget::SetCurrentTab(
    /* [in] */ Int32 index)
{
    Int32 count;
    if (index < 0 || index >= (GetTabCount(&count), count) || index == mSelectedTab) {
        return NOERROR;
    }

    if (mSelectedTab != -1) {
        AutoPtr<IView> v;
        GetChildTabViewAt(mSelectedTab, (IView**)&v);
        v->SetSelected(FALSE);
    }

    mSelectedTab = index;
    AutoPtr<IView> tabView;
    GetChildTabViewAt(mSelectedTab, (IView**)&tabView);
    tabView->SetSelected(TRUE);
    mStripMoved = TRUE;
    Boolean isShow;
    if (IsShown(&isShow), isShow) {
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    }

    return NOERROR;
}

ECode TabWidget::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    OnPopulateAccessibilityEvent(event);
    // Dispatch only to the selected tab.
    if (mSelectedTab != -1) {
        AutoPtr<IView> tabView;
        GetChildTabViewAt(mSelectedTab, (IView**)&tabView);
        Int32 visible;
        if (tabView != NULL && (tabView->GetVisibility(&visible), visible) == IView::VISIBLE) {
            return tabView->DispatchPopulateAccessibilityEvent(event, res);
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode TabWidget::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CTabWidget"), (ICharSequence**)&seq);
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    Int32 count;
    GetTabCount(&count);
    IAccessibilityRecord::Probe(event)->SetItemCount(count);
    IAccessibilityRecord::Probe(event)->SetCurrentItemIndex(mSelectedTab);
    return NOERROR;
}

ECode TabWidget::SendAccessibilityEventUnchecked(
    /* [in] */ IAccessibilityEvent* event)
{
    // this class fires events only when tabs are focused or selected
    Int32 type;
    event->GetEventType(&type);
    Boolean isFocus;
    if (type == IAccessibilityEvent::TYPE_VIEW_FOCUSED && (IsFocused(&isFocus), isFocus)) {
        IAccessibilityRecord::Probe(event)->Recycle();
        return NOERROR;
    }
    return LinearLayout::SendAccessibilityEventUnchecked(event);
}

ECode TabWidget::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    LinearLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> clsName;
    CString::New(String("CTabWidget"), (ICharSequence**)&clsName);
    info->SetClassName(clsName);
    return NOERROR;
}

/**
 * Sets the current tab and focuses the UI on it.
 * This method makes sure that the focused tab matches the selected
 * tab, normally at {@link #setCurrentTab}.  Normally this would not
 * be an issue if we go through the UI, since the UI is responsible
 * for calling TabWidget.onFocusChanged(), but in the case where we
 * are selecting the tab programmatically, we'll need to make sure
 * focus keeps up.
 *
 *  @param index The tab that you want focused (highlighted in orange)
 *  and selected (tab brought to the front of the widget)
 *
 *  @see #setCurrentTab
 */
ECode TabWidget::FocusCurrentTab(
    /* [in] */ Int32 index)
{
    Int32 oldTab = mSelectedTab;

    // set the tab
    SetCurrentTab(index);

    // change the focus if applicable.
    if (oldTab != index) {
        AutoPtr<IView> tabView;
        GetChildTabViewAt(index, (IView**)&tabView);
        Boolean res;
        tabView->RequestFocus(&res);
    }

    return NOERROR;
}

ECode TabWidget::SetEnabled(
    /* [in] */ Boolean enabled)
{
    LinearLayout::SetEnabled(enabled);
    Int32 count;
    GetTabCount(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> tabView;
        GetChildTabViewAt(i, (IView**)&tabView);
        tabView->SetEnabled(enabled);
    }

    return NOERROR;
}

ECode TabWidget::AddView(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    if (lp == NULL) {
        AutoPtr<ILinearLayoutLayoutParams> lp;
        CLinearLayoutLayoutParams::New(0, IViewGroupLayoutParams::MATCH_PARENT,
                1.0f, (ILinearLayoutLayoutParams**)&lp);
        IViewGroupMarginLayoutParams::Probe(lp)->SetMargins(0, 0, 0, 0);
        child->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }

    // Ensure you can navigate to the tab with the keyboard, and you can touch it
    child->SetFocusable(TRUE);
    child->SetClickable(TRUE);

    LinearLayout::AddView(child);

    // TODO: detect this via geometry with a tabwidget listener rather
    // than potentially interfere with the view's listener
    Int32 count;
    GetTabCount(&count);
    AutoPtr<TabListener> listener = new TabListener(count - 1, this);
    child->SetOnClickListener(listener);
    child->SetOnFocusChangeListener(listener);

    return NOERROR;
}

ECode TabWidget::RemoveAllViews()
{
    LinearLayout::RemoveAllViews();
    mSelectedTab = -1;
    return NOERROR;
}

/**
 * Provides a way for {@link TabHost} to be notified that the user clicked on a tab indicator.
 */
ECode TabWidget::SetTabSelectionListener(
    /* [in] */ ITabWidgetOnTabSelectionChanged* listener)
{
    mSelectionChangedListener = listener;
    return NOERROR;
}

ECode TabWidget::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    Int32 count;
    if (v == (IView*)this && hasFocus
        && (GetTabCount(&count), count) > 0 && ( mSelectedTab!= -1) ) {
        AutoPtr<IView> tabView;
        GetChildTabViewAt(mSelectedTab, (IView**)&tabView);
        Boolean res;
        return tabView->RequestFocus(&res);
    }

    if (hasFocus) {
        Int32 i = 0;
        Int32 numTabs;
        GetTabCount(&numTabs);
        while (i < numTabs) {
            AutoPtr<IView> tabView;
            GetChildTabViewAt(mSelectedTab, (IView**)&tabView);
            if (v == tabView) {
                SetCurrentTab(i);
                mSelectionChangedListener->OnTabSelectionChanged(i, FALSE);
                Boolean isShow;
                if (IsShown(&isShow), isShow) {
                    // a tab is focused so send an event to announce the tab widget state
                    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_FOCUSED);
                }

                break;
            }
            i++;
        }
    }

    return NOERROR;
}


} // namespace Widget
} // namespace Droid
} // namespace Elastos
