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

#include "elastos/droid/internal/widget/ActionBarContextView.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CViewGroupMarginLayoutParams.h"
#include "elastos/droid/widget/CActionMenuPresenter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/core/Math.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::Widget::EIID_IActionBarContextView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::CViewGroupMarginLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::CActionMenuPresenter;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String ActionBarContextView::TAG("ActionBarContextView");
const Int32 ActionBarContextView::ANIMATE_IDLE = 0;
const Int32 ActionBarContextView::ANIMATE_IN = 1;
const Int32 ActionBarContextView::ANIMATE_OUT = 2;

//=====================================================================
//             ActionBarContextView::InnerCloseButtonListener
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarContextView::InnerCloseButtonListener, Object, IViewOnClickListener)

ActionBarContextView::InnerCloseButtonListener::InnerCloseButtonListener(
    /* [in] */ IActionMode* mode)
    : mMode(mode)
{
}

ECode ActionBarContextView::InnerCloseButtonListener::OnClick(
    /* [in] */ IView* v)
{
    mMode->Finish();
    return NOERROR;
}

//=====================================================================
//                         ActionBarContextView::AnimatorListener
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarContextView::AnimatorListener, Object, IAnimatorListener)

ActionBarContextView::AnimatorListener::AnimatorListener(
    /* [in] */ ActionBarContextView* host)
    : mHost(host)
{
}

ECode ActionBarContextView::AnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode ActionBarContextView::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mAnimationMode == ActionBarContextView::ANIMATE_OUT) {
        mHost->KillMode();
    }
    mHost->mAnimationMode = ANIMATE_IDLE;
    return NOERROR;
}

ECode ActionBarContextView::AnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode ActionBarContextView::AnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

//=====================================================================
//                         ActionBarContextView
//=====================================================================

CAR_INTERFACE_IMPL(ActionBarContextView, AbsActionBarView, IActionBarContextView)

ActionBarContextView::ActionBarContextView()
    : mTitleStyleRes(0)
    , mSubtitleStyleRes(0)
    , mTitleOptional(FALSE)
    , mCloseItemLayout(0)
    , mAnimateInOnLayout(FALSE)
    , mAnimationMode(0)
{
}

ActionBarContextView::~ActionBarContextView()
{
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::actionModeStyle);
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(attrs);

    AbsActionBarView::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr<ITypedArray> a;
    AutoPtr< ArrayOf<Int32> > values = TO_ATTRS_ARRAYOF(R::styleable::ActionMode);
    context->ObtainStyledAttributes(attrs, values, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IDrawable> dr;
    a->GetDrawable(R::styleable::ActionMode_background, (IDrawable**)&dr);
    SetBackground(dr);
    a->GetResourceId(R::styleable::ActionMode_titleTextStyle, 0, &mTitleStyleRes);
    a->GetResourceId(R::styleable::ActionMode_subtitleTextStyle, 0, &mSubtitleStyleRes);
    a->GetLayoutDimension(R::styleable::ActionMode_height, 0, &mContentHeight);
    a->GetDrawable(R::styleable::ActionMode_backgroundSplit, (IDrawable**)&mSplitBackground);
    a->GetResourceId(R::styleable::ActionMode_closeItemLayout, R::layout::action_mode_close_item, &mCloseItemLayout);
    a->Recycle();
    return NOERROR;
}

ECode ActionBarContextView::OnDetachedFromWindow()
{
    AbsActionBarView::OnDetachedFromWindow();
    if (mActionMenuPresenter != NULL) {
        Boolean res = FALSE;
        mActionMenuPresenter->HideOverflowMenu(&res);
        mActionMenuPresenter->HideSubMenus(&res);
    }
    return NOERROR;
}

ECode ActionBarContextView::SetSplitToolbar(
    /* [in] */ Boolean split)
{
    if (mSplitActionBar != split) {
        if (mActionMenuPresenter != NULL) {
            // Mode is already active; move everything over and adjust the menu itself.
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            CViewGroupLayoutParams::New(
                IViewGroupLayoutParams::WRAP_CONTENT,
                IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&layoutParams);
            if (!split) {
                AutoPtr<IMenuView> view;
                IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(this, (IMenuView**)&view);
                mMenuView = IActionMenuView::Probe(view);
                IView* v = IView::Probe(mMenuView);
                v->SetBackground(NULL);
                AutoPtr<IViewParent> oldParent;;
                v->GetParent((IViewParent**)&oldParent);
                IViewManager* vm = IViewManager::Probe(oldParent);
                if (vm != NULL) {
                    vm->RemoveView(v);
                }
                AddView(v, layoutParams);
            }
            else {
                AutoPtr<IContext> context;
                GetContext((IContext**)&context);
                AutoPtr<IResources> rs;
                context->GetResources((IResources**)&rs);

                AutoPtr<IDisplayMetrics> dm;
                rs->GetDisplayMetrics((IDisplayMetrics**)&dm);
                Int32 widthPixels;
                dm->GetWidthPixels(&widthPixels);
                // Allow full screen width in split mode.

                mActionMenuPresenter->SetWidthLimit(widthPixels, TRUE);
                // No limit to the item count; use whatever will fit.
                mActionMenuPresenter->SetItemLimit(Elastos::Core::Math::INT32_MAX_VALUE);
                // Span the whole width
                layoutParams->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
                layoutParams->SetHeight(mContentHeight);
                AutoPtr<IMenuView> view;
                IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(this, (IMenuView**)&view);
                mMenuView = IActionMenuView::Probe(view);
                IView* v = IView::Probe(mMenuView);
                v->SetBackground(mSplitBackground);
                AutoPtr<IViewParent> oldParent;
                v->GetParent((IViewParent**)&oldParent);
                IViewManager* vm = IViewManager::Probe(oldParent);
                if (vm != NULL) {
                    vm->RemoveView(v);
                }
                mSplitView->AddView(v, layoutParams);
            }
        }
        AbsActionBarView::SetSplitToolbar(split);
    }
    return NOERROR;
}

ECode ActionBarContextView::SetContentHeight(
    /* [in] */ Int32 height)
{
    mContentHeight = height;
    return NOERROR;
}

ECode ActionBarContextView::SetCustomView(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view)

    if (mCustomView != NULL) {
        RemoveView(mCustomView);
    }
    mCustomView = view;
    if (mTitleLayout != NULL) {
        RemoveView(IView::Probe(mTitleLayout));
        mTitleLayout = NULL;
    }
    if (view != NULL) {
        AddView(view);
    }
    RequestLayout();
    return NOERROR;
}

ECode ActionBarContextView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title)

    mTitle = title;
    InitTitle();
    return NOERROR;
}

ECode ActionBarContextView::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    VALIDATE_NOT_NULL(subtitle)

    mSubtitle = subtitle;
    InitTitle();
    return NOERROR;
}

ECode ActionBarContextView::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContextView::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSubtitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContextView::InitForMode(
    /* [in] */ IActionMode* mode)
{
    VALIDATE_NOT_NULL(mode)
    AutoPtr<IViewParent> vp;
    if (mClose == NULL) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        inflater->Inflate(mCloseItemLayout, this, FALSE, (IView**)&mClose);
        AddView(mClose);
    }
    else if ((mClose->GetParent((IViewParent**)&vp), vp) == NULL) {
        AddView(mClose);
    }

    AutoPtr<IView> closeButton;
    mClose->FindViewById(R::id::action_mode_close_button, (IView**)&closeButton);
    AutoPtr<IViewOnClickListener> l = new InnerCloseButtonListener(mode);
    closeButton->SetOnClickListener(l);

    AutoPtr<IMenu> mb;
    mode->GetMenu((IMenu**)&mb);
    IMenuBuilder* menu = IMenuBuilder::Probe(mb);
    if (mActionMenuPresenter != NULL) {
        Boolean res = FALSE;
        mActionMenuPresenter->DismissPopupMenus(&res);
    }
    CActionMenuPresenter::New(mContext, (IActionMenuPresenter**)&mActionMenuPresenter);
    mActionMenuPresenter->SetReserveOverflow(TRUE);

    AutoPtr<IViewGroupLayoutParams> layoutParams;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            (IViewGroupLayoutParams**)&layoutParams);

    IMenuPresenter* actionMenuPresenter = IMenuPresenter::Probe(mActionMenuPresenter);
    if (!mSplitActionBar) {
        menu->AddMenuPresenter(actionMenuPresenter, mPopupContext);
        AutoPtr<IMenuView> view;
        actionMenuPresenter->GetMenuView(this, (IMenuView**)&view);
        mMenuView = IActionMenuView::Probe(view);
        IView* menuView = IView::Probe(mMenuView);
        menuView->SetBackgroundDrawable(NULL);
        AddView(menuView, layoutParams);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> rs;
        context->GetResources((IResources**)&rs);

        AutoPtr<IDisplayMetrics> dm;
        rs->GetDisplayMetrics((IDisplayMetrics**)&dm);

        // Allow full screen width in split mode.
        Int32 widthPixel = 0;
        dm->GetWidthPixels(&widthPixel);
        mActionMenuPresenter->SetWidthLimit(widthPixel, TRUE);
        // No limit to the item count; use whatever will fit.
        mActionMenuPresenter->SetItemLimit(Elastos::Core::Math::INT32_MAX_VALUE);
        // Span the whole width
        layoutParams->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
        layoutParams->SetHeight(mContentHeight);
        menu->AddMenuPresenter(actionMenuPresenter, mPopupContext);
        AutoPtr<IMenuView> view;
        actionMenuPresenter->GetMenuView(this, (IMenuView**)&view);
        mMenuView = IActionMenuView::Probe(view);
        IView* menuView = IView::Probe(mMenuView);
        menuView->SetBackgroundDrawable(mSplitBackground);
        mSplitView->AddView(menuView, layoutParams);
    }

    mAnimateInOnLayout = TRUE;
    return NOERROR;
}

ECode ActionBarContextView::CloseMode()
{
    if (mAnimationMode == ANIMATE_OUT) {
        // Called again during close; just finish what we were doing.
        return NOERROR;
    }
    if (mClose == NULL) {
        KillMode();
        return NOERROR;
    }

    FinishAnimation();

    mAnimationMode = ANIMATE_OUT;
    mCurrentAnimation = MakeOutAnimation();
    mCurrentAnimation->Start();
    return NOERROR;
}

ECode ActionBarContextView::KillMode()
{
    FinishAnimation();
    RemoveAllViews();
    IViewManager* vm = IViewManager::Probe(mSplitView);
    if (vm != NULL) {
        vm->RemoveView(IView::Probe(mMenuView));
    }
    mCustomView = NULL;
    mMenuView = NULL;
    mAnimateInOnLayout = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->ShowOverflowMenu(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->HideOverflowMenu(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mActionMenuPresenter != NULL) {
        return mActionMenuPresenter->IsOverflowMenuShowing(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(attrs);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    return CViewGroupMarginLayoutParams::New(context, attrs, result);
}

ECode ActionBarContextView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    Int32 type = 0;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        IAccessibilityRecord* _event = IAccessibilityRecord::Probe(event);
        // Action mode started
        _event->SetSource(IView::Probe(this));
        AutoPtr<ICharSequence> name;
        CString::New(String("CActionBarContextView"), (ICharSequence**)&name);
        _event->SetClassName(name);

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String pkgName;
        context->GetPackageName(&pkgName);
        name = NULL;
        CString::New(pkgName, (ICharSequence**)&name);
        event->SetPackageName(name);
        _event->SetContentDescription(mTitle);
    }
    else {
        AbsActionBarView::OnInitializeAccessibilityEvent(event);
    }
    return NOERROR;
}

ECode ActionBarContextView::SetTitleOptional(
    /* [in] */ Boolean titleOptional)
{
    if (titleOptional != mTitleOptional) {
        RequestLayout();
    }
    mTitleOptional = titleOptional;
    return NOERROR;
}

ECode ActionBarContextView::IsTitleOptional(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTitleOptional;
    return NOERROR;
}

ECode ActionBarContextView::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    // Used by custom views if they don't supply layout params. Everything else
    // added to an ActionBarContextView should have them already.

    return CViewGroupMarginLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT, result);
}

ECode ActionBarContextView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    if (widthMode != View::MeasureSpec::EXACTLY) {
        ALOGE("IllegalStateException %s  can only be used with "
            "android:layout_width=\"match_parent\" (or fill_parent)", TO_CSTR(this));
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    if (heightMode == View::MeasureSpec::UNSPECIFIED) {
        ALOGE("IllegalStateException %s  can only be used with "
            "android:layout_height=\"wrap_content\"", TO_CSTR(this));
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 contentWidth = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 maxHeight = mContentHeight > 0 ? mContentHeight : View::MeasureSpec::GetSize(heightMeasureSpec);

    Int32 paddingTop = 0, paddingBottom = 0;
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    Int32 verticalPadding = paddingTop + paddingBottom;

    Int32 paddingLeft = 0, paddingRight = 0;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    Int32 availableWidth = contentWidth - paddingLeft - paddingRight;
    Int32 height = maxHeight - verticalPadding;
    Int32 childSpecHeight = View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::AT_MOST);

    if (mClose != NULL) {
        availableWidth = MeasureChildView(mClose, availableWidth, childSpecHeight, 0);
        AutoPtr<IViewGroupLayoutParams> vglp;
        mClose->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(vglp);

        Int32 leftMargin = 0, rightMargin = 0;
        lp->GetLeftMargin(&leftMargin);
        lp->GetRightMargin(&rightMargin);
        availableWidth -= leftMargin + rightMargin;
    }

    if (mMenuView != NULL) {
        IView* menuView = IView::Probe(mMenuView);
        AutoPtr<IViewParent> vp;
        menuView->GetParent((IViewParent**)&vp);
        if (vp.Get() == (IViewParent*)this) {
            availableWidth = MeasureChildView(menuView, availableWidth, childSpecHeight, 0);
        }
    }

    if (mTitleLayout != NULL && mCustomView == NULL) {
        IView* titleLayout = IView::Probe(mTitleLayout);
        if (mTitleOptional) {
            Int32 titleWidthSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
            titleLayout->Measure(titleWidthSpec, childSpecHeight);
            Int32 titleWidth;
            titleLayout->GetMeasuredWidth(&titleWidth);

            Boolean titleFits = titleWidth <= availableWidth;
            if (titleFits) {
                availableWidth -= titleWidth;
            }
            titleLayout->SetVisibility(titleFits ? IView::VISIBLE : IView::GONE);
        }
        else {
            availableWidth = MeasureChildView(titleLayout, availableWidth, childSpecHeight, 0);
        }
    }

    using Elastos::Core::Math;

    if (mCustomView != NULL) {
        AutoPtr<IViewGroupLayoutParams> lp;
        mCustomView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        Int32 w;
        lp->GetWidth(&w);
        Int32 customWidthMode = w != IViewGroupLayoutParams::WRAP_CONTENT ? MeasureSpec::EXACTLY : MeasureSpec::AT_MOST;
        Int32 customWidth = w >= 0 ? Math::Min(w, availableWidth) : availableWidth;
        Int32 h;
        lp->GetHeight(&h);
        Int32 customHeightMode = h != IViewGroupLayoutParams::WRAP_CONTENT ? View::MeasureSpec::EXACTLY : View::MeasureSpec::AT_MOST;
        Int32 customHeight = h >= 0 ? Math::Min(h, height) : height;
        mCustomView->Measure(View::MeasureSpec::MakeMeasureSpec(customWidth, customWidthMode),
            View::MeasureSpec::MakeMeasureSpec(customHeight, customHeightMode));
    }

    if (mContentHeight <= 0) {
        Int32 measuredHeight = 0;
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            Int32 paddedViewHeight;
            v->GetMeasuredHeight(&paddedViewHeight);
            paddedViewHeight += verticalPadding;

            if (paddedViewHeight > measuredHeight) {
                measuredHeight = paddedViewHeight;
            }
        }
        SetMeasuredDimension(contentWidth, measuredHeight);
    }
    else {
        SetMeasuredDimension(contentWidth, maxHeight);
    }
    return NOERROR;
}

ECode ActionBarContextView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);
    Int32 paddingLeft = 0, paddingRight = 0, paddingTop = 0, paddingBottom = 0;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    Int32 x = isLayoutRtl ? r - l - paddingRight : paddingLeft;
    Int32 y = paddingTop;
    Int32 contentHeight = b - t - paddingTop - paddingBottom;

    Int32 visible = 0;
    if (mClose != NULL && (mClose->GetVisibility(&visible), visible != IView::GONE)) {
        AutoPtr<IViewGroupLayoutParams> vglp;
        mClose->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(vglp);

        Int32 leftMargin = 0;
        Int32 rightMargin = 0;
        lp->GetLeftMargin(&leftMargin);
        lp->GetRightMargin(&rightMargin);
        Int32 startMargin = isLayoutRtl ? rightMargin : leftMargin;
        Int32 endMargin = isLayoutRtl ? leftMargin : rightMargin;
        x = Next(x, startMargin, isLayoutRtl);
        x += PositionChild(mClose, x, y, contentHeight, isLayoutRtl);
        x = Next(x, endMargin, isLayoutRtl);

        if (mAnimateInOnLayout) {
            mAnimationMode = ANIMATE_IN;
            mCurrentAnimation = MakeInAnimation();
            mCurrentAnimation->Start();
            mAnimateInOnLayout = FALSE;
        }
    }

    if (mTitleLayout != NULL && mCustomView == NULL) {
        IView* titleLayout = IView::Probe(mTitleLayout);
        titleLayout->GetVisibility(&visible);
        if (visible != IView::GONE) {
            x += PositionChild(titleLayout, x, y, contentHeight, isLayoutRtl);
        }
    }

    if (mCustomView != NULL) {
        x += PositionChild(IView::Probe(mCustomView), x, y, contentHeight, isLayoutRtl);
    }

    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    x = isLayoutRtl ? paddingLeft : r - l - paddingRight;

    if (mMenuView != NULL) {
        x += PositionChild(IView::Probe(mMenuView), x, y, contentHeight, !isLayoutRtl);
    }
    return NOERROR;
}

void ActionBarContextView::InitTitle()
{
    if (mTitleLayout == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(context, (ILayoutInflater**)&inflater);
        assert(inflater != NULL);
        AutoPtr<IView> view;
        inflater->Inflate(R::layout::action_bar_title_item, this, (IView**)&view);

        Int32 count = 0;
        GetChildCount(&count);
        AutoPtr<IView> viewTmp;
        GetChildAt(count - 1, (IView**)&viewTmp);
        mTitleLayout = ILinearLayout::Probe(viewTmp);
        IView* titleLayout = IView::Probe(mTitleLayout);
        viewTmp = NULL;
        titleLayout->FindViewById(R::id::action_bar_title, (IView**)&viewTmp);
        mTitleView = ITextView::Probe(viewTmp);
        viewTmp = NULL;
        titleLayout->FindViewById(R::id::action_bar_subtitle, (IView**)&viewTmp);
        mSubtitleView = ITextView::Probe(viewTmp);
        if (mTitleStyleRes != 0) {
            mTitleView->SetTextAppearance(mContext, mTitleStyleRes);
        }
        if (mSubtitleStyleRes != 0) {
            mSubtitleView->SetTextAppearance(mContext, mSubtitleStyleRes);
        }
    }

    mTitleView->SetText(mTitle);
    mSubtitleView->SetText(mSubtitle);

    Boolean hasTitle = !TextUtils::IsEmpty(mTitle);
    Boolean hasSubtitle = !TextUtils::IsEmpty(mSubtitle);
    IView::Probe(mSubtitleView)->SetVisibility(hasSubtitle ? IView::VISIBLE : IView::GONE);
    IView* tl = IView::Probe(mTitleLayout);
    tl->SetVisibility(hasTitle || hasSubtitle ? IView::VISIBLE : IView::GONE);

    AutoPtr<IViewParent> vp;
    tl->GetParent((IViewParent**)&vp);
    if (vp == NULL) {
        AddView(tl);
    }
}

void ActionBarContextView::FinishAnimation()
{
    AutoPtr<IAnimator> a = mCurrentAnimation;
    if (a != NULL) {
        mCurrentAnimation = NULL;
        a->End();
    }
}

AutoPtr<IAnimator> ActionBarContextView::MakeInAnimation()
{
    assert(mClose != NULL);
    Int32 width = 0;
    mClose->GetWidth(&width);

    AutoPtr<IViewGroupLayoutParams> vglp;
    mClose->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    IViewGroupMarginLayoutParams* layoutParams = IViewGroupMarginLayoutParams::Probe(vglp);

    Int32 leftMargin;
    layoutParams->GetLeftMargin(&leftMargin);
    mClose->SetTranslationX(-width - leftMargin);
    AutoPtr<ArrayOf<Float> > tmpValues = ArrayOf<Float>::Alloc(1);
    (*tmpValues)[0] = 0.0f;
    AutoPtr<IObjectAnimator> buttonAnimator = CObjectAnimator::OfFloat(mClose, String("translationX"), tmpValues);
    IValueAnimator::Probe(buttonAnimator)->SetDuration(200);
    IAnimator* animator = IAnimator::Probe(buttonAnimator);
    AutoPtr<IAnimatorListener> al = new AnimatorListener(this);
    animator->AddListener(al);

    AutoPtr<IDecelerateInterpolator> decele;
    CDecelerateInterpolator::New((IDecelerateInterpolator**)&decele);
    animator->SetInterpolator(ITimeInterpolator::Probe(decele));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IAnimatorSetBuilder> b;
    animatorSet->Play(animator, (IAnimatorSetBuilder**)&b);
    if (mMenuView != NULL) {
        IViewGroup* vg = IViewGroup::Probe(mMenuView);
        Int32 count = 0;
        vg->GetChildCount(&count);
        if (count > 0) {
            for (Int32 i = count - 1, j = 0; i >= 0; --i, ++j) {
                AutoPtr<IView> child;
                vg->GetChildAt(i, (IView**)&child);
                child->SetScaleY(0);

                AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
                (*values)[0] = 0.0f;
                (*values)[1] = 1.0f;
                AutoPtr<IObjectAnimator> a = CObjectAnimator::OfFloat(child, String("scaleY"), values);
                IValueAnimator::Probe(a)->SetDuration(300);
                b->With(IAnimator::Probe(a));
            }
        }
    }

    return IAnimator::Probe(animatorSet);
}

AutoPtr<IAnimator> ActionBarContextView::MakeOutAnimation()
{
    Int32 width = 0;
    mClose->GetWidth(&width);
    AutoPtr<IViewGroupLayoutParams> vglp;
    mClose->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    IViewGroupMarginLayoutParams* layoutParams = IViewGroupMarginLayoutParams::Probe(vglp);
    Int32 leftMargin;
    layoutParams->GetLeftMargin(&leftMargin);
    AutoPtr<ArrayOf<Float> > tmpValues = ArrayOf<Float>::Alloc(1);
    (*tmpValues)[0] = -width - leftMargin;
    AutoPtr<IObjectAnimator> buttonAnimator = CObjectAnimator::OfFloat(mClose, String("translationX"), tmpValues);
    IValueAnimator::Probe(buttonAnimator)->SetDuration(200);
    IAnimator* animator = IAnimator::Probe(buttonAnimator);
    AutoPtr<IAnimatorListener> al = new AnimatorListener(this);
    animator->AddListener(al);
    AutoPtr<IDecelerateInterpolator> decele;
    CDecelerateInterpolator::New((IDecelerateInterpolator**)&decele);
    animator->SetInterpolator(ITimeInterpolator::Probe(decele));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IAnimatorSetBuilder> b;
    animatorSet->Play(animator, (IAnimatorSetBuilder**)&b);

    if (mMenuView != NULL) {
        IViewGroup* vg = IViewGroup::Probe(mMenuView);
        Int32 count;
        vg->GetChildCount(&count);
        if (count > 0) {
            for (Int32 i = 0; i < count; ++i) {
                AutoPtr<IView> child;
                vg->GetChildAt(i, (IView**)&child);
                child->SetScaleY(0);

                AutoPtr<ArrayOf<Float> > tmpValues = ArrayOf<Float>::Alloc(1);
                (*tmpValues)[0] = 0.0f;
                AutoPtr<IObjectAnimator> a = CObjectAnimator::OfFloat(child, String("scaleY"), tmpValues);
                IValueAnimator::Probe(a)->SetDuration(300);
                b->With(IAnimator::Probe(a));
            }
        }
    }

    return IAnimator::Probe(animatorSet);
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
