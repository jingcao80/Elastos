
#include "elastos/droid/internal/widget/ActionBarContextView.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
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

using Elastos::Droid::Internal::Widget::EIID_IActionBarContextView;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::CViewGroupMarginLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::CActionMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

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
//                         ActionBarContextView
//=====================================================================
const String ActionBarContextView::TAG("ActionBarContextView");
const Int32 ActionBarContextView::ANIMATE_IDLE;
const Int32 ActionBarContextView::ANIMATE_IN;
const Int32 ActionBarContextView::ANIMATE_OUT;

CAR_INTERFACE_IMPL_2(ActionBarContextView, AbsActionBarView, IAnimatorListener, IActionBarContextView)

ActionBarContextView::ActionBarContextView()
    : mTitleStyleRes(0)
    , mSubtitleStyleRes(0)
    , mTitleOptional(FALSE)
    , mAnimateInOnLayout(FALSE)
    , mAnimationMode(0)
{
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, com.android.internal.R.attr.actionModeStyle);

    return constructor(context, attrs, R::attr::actionModeStyle);
}

ECode ActionBarContextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

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
    // ==================before translated======================
    // super(context, attrs, defStyleAttr, defStyleRes);
    //
    // final TypedArray a = context.obtainStyledAttributes(
    //         attrs, R.styleable.ActionMode, defStyleAttr, defStyleRes);
    // setBackground(a.getDrawable(
    //         com.android.internal.R.styleable.ActionMode_background));
    // mTitleStyleRes = a.getResourceId(
    //         com.android.internal.R.styleable.ActionMode_titleTextStyle, 0);
    // mSubtitleStyleRes = a.getResourceId(
    //         com.android.internal.R.styleable.ActionMode_subtitleTextStyle, 0);
    //
    // mContentHeight = a.getLayoutDimension(
    //         com.android.internal.R.styleable.ActionMode_height, 0);
    //
    // mSplitBackground = a.getDrawable(
    //         com.android.internal.R.styleable.ActionMode_backgroundSplit);
    //
    // mCloseItemLayout = a.getResourceId(
    //         com.android.internal.R.styleable.ActionMode_closeItemLayout,
    //         R.layout.action_mode_close_item);
    //
    // a.recycle();

    AbsActionBarView::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr<ITypedArray> a;
    AutoPtr< ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(const_cast<Int32*>(R::styleable::ActionMode),
        ArraySize(R::styleable::ActionMode));
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
    // ==================before translated======================
    // super.onDetachedFromWindow();
    // if (mActionMenuPresenter != null) {
    //     mActionMenuPresenter.hideOverflowMenu();
    //     mActionMenuPresenter.hideSubMenus();
    // }

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
    // ==================before translated======================
    // if (mSplitActionBar != split) {
    //     if (mActionMenuPresenter != null) {
    //         // Mode is already active; move everything over and adjust the menu itself.
    //         final LayoutParams layoutParams = new LayoutParams(LayoutParams.WRAP_CONTENT,
    //                 LayoutParams.MATCH_PARENT);
    //         if (!split) {
    //             mMenuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //             mMenuView.setBackground(null);
    //             final ViewGroup oldParent = (ViewGroup) mMenuView.getParent();
    //             if (oldParent != null) oldParent.removeView(mMenuView);
    //             addView(mMenuView, layoutParams);
    //         } else {
    //             // Allow full screen width in split mode.
    //             mActionMenuPresenter.setWidthLimit(
    //                     getContext().getResources().getDisplayMetrics().widthPixels, true);
    //             // No limit to the item count; use whatever will fit.
    //             mActionMenuPresenter.setItemLimit(Integer.MAX_VALUE);
    //             // Span the whole width
    //             layoutParams.width = LayoutParams.MATCH_PARENT;
    //             layoutParams.height = mContentHeight;
    //             mMenuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //             mMenuView.setBackground(mSplitBackground);
    //             final ViewGroup oldParent = (ViewGroup) mMenuView.getParent();
    //             if (oldParent != null) oldParent.removeView(mMenuView);
    //             mSplitView.addView(mMenuView, layoutParams);
    //         }
    //     }
    //     super.setSplitToolbar(split);
    // }

    if (mSplitActionBar != split) {
        if (mActionMenuPresenter != NULL) {
            // Mode is already active; move everything over and adjust the menu itself.
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&layoutParams);
            if (!split) {
                AutoPtr<IMenuView> view;
                IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&view);
                mMenuView = IActionMenuView::Probe(view);
                IView::Probe(mMenuView)->SetBackground(NULL);
                AutoPtr<IViewParent> oldParent;;
                IView::Probe(mMenuView)->GetParent((IViewParent**)&oldParent);
                IViewManager* vm = IViewManager::Probe(oldParent);
                if (vm != NULL)
                    vm->RemoveView(IView::Probe(mMenuView));
                AddView(IView::Probe(mMenuView), layoutParams);
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
                IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&view);
                mMenuView = IActionMenuView::Probe(view);
                IView::Probe(mMenuView)->SetBackground(mSplitBackground);
                AutoPtr<IViewParent> oldParent;
                IView::Probe(mMenuView)->GetParent((IViewParent**)&oldParent);
                IViewManager* vm = IViewManager::Probe(oldParent);
                if (vm != NULL)
                    vm->RemoveView(IView::Probe(mMenuView));
                mSplitView->AddView(IView::Probe(mMenuView), layoutParams);
            }
        }
        AbsActionBarView::SetSplitToolbar(split);
    }
    return NOERROR;
}

ECode ActionBarContextView::SetContentHeight(
    /* [in] */ Int32 height)
{
    // ==================before translated======================
    // mContentHeight = height;

    mContentHeight = height;
    return NOERROR;
}

ECode ActionBarContextView::SetCustomView(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // if (mCustomView != null) {
    //     removeView(mCustomView);
    // }
    // mCustomView = view;
    // if (mTitleLayout != null) {
    //     removeView(mTitleLayout);
    //     mTitleLayout = null;
    // }
    // if (view != null) {
    //     addView(view);
    // }
    // requestLayout();

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
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // mTitle = title;
    // initTitle();

    mTitle = title;
    InitTitle();
    return NOERROR;
}

ECode ActionBarContextView::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    VALIDATE_NOT_NULL(subtitle);
    // ==================before translated======================
    // mSubtitle = subtitle;
    // initTitle();

    mSubtitle = subtitle;
    InitTitle();
    return NOERROR;
}

ECode ActionBarContextView::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTitle;

    *result = mTitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContextView::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSubtitle;

    *result = mSubtitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContextView::InitForMode(
    /* [in] */ IActionMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    // ==================before translated======================
    // if (mClose == null) {
    //     LayoutInflater inflater = LayoutInflater.from(mContext);
    //     mClose = inflater.inflate(mCloseItemLayout, this, false);
    //     addView(mClose);
    // } else if (mClose.getParent() == null) {
    //     addView(mClose);
    // }
    //
    // View closeButton = mClose.findViewById(R.id.action_mode_close_button);
    // closeButton.setOnClickListener(new OnClickListener() {
    //     public void onClick(View v) {
    //         mode.finish();
    //     }
    // });
    //
    // final MenuBuilder menu = (MenuBuilder) mode.getMenu();
    // if (mActionMenuPresenter != null) {
    //     mActionMenuPresenter.dismissPopupMenus();
    // }
    // mActionMenuPresenter = new ActionMenuPresenter(mContext);
    // mActionMenuPresenter.setReserveOverflow(true);
    //
    // final LayoutParams layoutParams = new LayoutParams(LayoutParams.WRAP_CONTENT,
    //         LayoutParams.MATCH_PARENT);
    // if (!mSplitActionBar) {
    //     menu.addMenuPresenter(mActionMenuPresenter, mPopupContext);
    //     mMenuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //     mMenuView.setBackgroundDrawable(null);
    //     addView(mMenuView, layoutParams);
    // } else {
    //     // Allow full screen width in split mode.
    //     mActionMenuPresenter.setWidthLimit(
    //             getContext().getResources().getDisplayMetrics().widthPixels, true);
    //     // No limit to the item count; use whatever will fit.
    //     mActionMenuPresenter.setItemLimit(Integer.MAX_VALUE);
    //     // Span the whole width
    //     layoutParams.width = LayoutParams.MATCH_PARENT;
    //     layoutParams.height = mContentHeight;
    //     menu.addMenuPresenter(mActionMenuPresenter, mPopupContext);
    //     mMenuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //     mMenuView.setBackgroundDrawable(mSplitBackground);
    //     mSplitView.addView(mMenuView, layoutParams);
    // }
    //
    // mAnimateInOnLayout = true;

    AutoPtr<IViewParent> vp;
    if (mClose == NULL) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        inflater->Inflate(mCloseItemLayout, IViewGroup::Probe(this), FALSE, (IView**)&mClose);
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
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&layoutParams);
    if (!mSplitActionBar) {
        menu->AddMenuPresenter(IMenuPresenter::Probe(mActionMenuPresenter), mPopupContext);
        AutoPtr<IMenuView> view;
        IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&view);
        mMenuView = IActionMenuView::Probe(view);
        IView::Probe(mMenuView)->SetBackgroundDrawable(NULL);
        AddView(IView::Probe(mMenuView), layoutParams);
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
        menu->AddMenuPresenter(IMenuPresenter::Probe(mActionMenuPresenter), mPopupContext);
        AutoPtr<IMenuView> view;
        IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&view);
        mMenuView = IActionMenuView::Probe(view);
        IView::Probe(mMenuView)->SetBackgroundDrawable(mSplitBackground);
        mSplitView->AddView(IView::Probe(mMenuView), layoutParams);
    }

    mAnimateInOnLayout = TRUE;
    return NOERROR;
}

ECode ActionBarContextView::CloseMode()
{
    // ==================before translated======================
    // if (mAnimationMode == ANIMATE_OUT) {
    //     // Called again during close; just finish what we were doing.
    //     return;
    // }
    // if (mClose == null) {
    //     killMode();
    //     return;
    // }
    //
    // finishAnimation();
    // mAnimationMode = ANIMATE_OUT;
    // mCurrentAnimation = makeOutAnimation();
    // mCurrentAnimation.start();

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
    // ==================before translated======================
    // finishAnimation();
    // removeAllViews();
    // if (mSplitView != null) {
    //     mSplitView.removeView(mMenuView);
    // }
    // mCustomView = null;
    // mMenuView = null;
    // mAnimateInOnLayout = false;

    FinishAnimation();
    RemoveAllViews();
    if (IViewManager::Probe(mSplitView) != NULL) {
        IViewManager::Probe(mSplitView)->RemoveView(IView::Probe(mMenuView));
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
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.showOverflowMenu();
    // }
    // return false;

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
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.hideOverflowMenu();
    // }
    // return false;

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
    // ==================before translated======================
    // if (mActionMenuPresenter != null) {
    //     return mActionMenuPresenter.isOverflowMenuShowing();
    // }
    // return false;

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
    // ==================before translated======================
    // return new MarginLayoutParams(getContext(), attrs);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewGroupMarginLayoutParams> lp;
    CViewGroupMarginLayoutParams::New(context, attrs, (IViewGroupMarginLayoutParams**)&lp);
    *result = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarContextView::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    return NOERROR;
}

ECode ActionBarContextView::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // if (mAnimationMode == ANIMATE_OUT) {
    //     killMode();
    // }
    // mAnimationMode = ANIMATE_IDLE;

    if (mAnimationMode == ANIMATE_OUT) {
        KillMode();
    }
    mAnimationMode = ANIMATE_IDLE;
    return NOERROR;
}

ECode ActionBarContextView::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode ActionBarContextView::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode ActionBarContextView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ActionBarContextView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // if (event.getEventType() == AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED) {
    //     // Action mode started
    //     event.setSource(this);
    //     event.setClassName(getClass().getName());
    //     event.setPackageName(getContext().getPackageName());
    //     event.setContentDescription(mTitle);
    // } else {
    //     super.onInitializeAccessibilityEvent(event);
    // }

    assert(event != NULL);
    Int32 type = 0;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        // Action mode started
        IAccessibilityRecord::Probe(event)->SetSource(IView::Probe(this));
        AutoPtr<ICharSequence> name;
        CString::New(String("CActionBarContextView"), (ICharSequence**)&name);
        IAccessibilityRecord::Probe(event)->SetClassName(name);

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String pkgName;
        context->GetPackageName(&pkgName);
        name->Release();
        CString::New(pkgName, (ICharSequence**)&name);
        event->SetPackageName(name);
        IAccessibilityRecord::Probe(event)->SetContentDescription(mTitle);
    }
    else {
        AbsActionBarView::OnInitializeAccessibilityEvent(event);
    }
    return NOERROR;
}

ECode ActionBarContextView::SetTitleOptional(
    /* [in] */ Boolean titleOptional)
{
    // ==================before translated======================
    // if (titleOptional != mTitleOptional) {
    //     requestLayout();
    // }
    // mTitleOptional = titleOptional;

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
    // ==================before translated======================
    // return mTitleOptional;

    *result = mTitleOptional;
    return NOERROR;
}

ECode ActionBarContextView::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Used by custom views if they don't supply layout params. Everything else
    // // added to an ActionBarContextView should have them already.
    // return new MarginLayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);

    CViewGroupLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::WRAP_CONTENT, result);
    return NOERROR;
}

void ActionBarContextView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // final int widthMode = MeasureSpec.getMode(widthMeasureSpec);
    // if (widthMode != MeasureSpec.EXACTLY) {
    //     throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
    //             "with android:layout_width=\"match_parent\" (or fill_parent)");
    // }
    //
    // final int heightMode = MeasureSpec.getMode(heightMeasureSpec);
    // if (heightMode == MeasureSpec.UNSPECIFIED) {
    //     throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
    //             "with android:layout_height=\"wrap_content\"");
    // }
    //
    // final int contentWidth = MeasureSpec.getSize(widthMeasureSpec);
    //
    // int maxHeight = mContentHeight > 0 ?
    //         mContentHeight : MeasureSpec.getSize(heightMeasureSpec);
    //
    // final int verticalPadding = getPaddingTop() + getPaddingBottom();
    // int availableWidth = contentWidth - getPaddingLeft() - getPaddingRight();
    // final int height = maxHeight - verticalPadding;
    // final int childSpecHeight = MeasureSpec.makeMeasureSpec(height, MeasureSpec.AT_MOST);
    //
    // if (mClose != null) {
    //     availableWidth = measureChildView(mClose, availableWidth, childSpecHeight, 0);
    //     MarginLayoutParams lp = (MarginLayoutParams) mClose.getLayoutParams();
    //     availableWidth -= lp.leftMargin + lp.rightMargin;
    // }
    //
    // if (mMenuView != null && mMenuView.getParent() == this) {
    //     availableWidth = measureChildView(mMenuView, availableWidth,
    //             childSpecHeight, 0);
    // }
    //
    // if (mTitleLayout != null && mCustomView == null) {
    //     if (mTitleOptional) {
    //         final int titleWidthSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
    //         mTitleLayout.measure(titleWidthSpec, childSpecHeight);
    //         final int titleWidth = mTitleLayout.getMeasuredWidth();
    //         final boolean titleFits = titleWidth <= availableWidth;
    //         if (titleFits) {
    //             availableWidth -= titleWidth;
    //         }
    //         mTitleLayout.setVisibility(titleFits ? VISIBLE : GONE);
    //     } else {
    //         availableWidth = measureChildView(mTitleLayout, availableWidth, childSpecHeight, 0);
    //     }
    // }
    //
    // if (mCustomView != null) {
    //     ViewGroup.LayoutParams lp = mCustomView.getLayoutParams();
    //     final int customWidthMode = lp.width != LayoutParams.WRAP_CONTENT ?
    //             MeasureSpec.EXACTLY : MeasureSpec.AT_MOST;
    //     final int customWidth = lp.width >= 0 ?
    //             Math.min(lp.width, availableWidth) : availableWidth;
    //     final int customHeightMode = lp.height != LayoutParams.WRAP_CONTENT ?
    //             MeasureSpec.EXACTLY : MeasureSpec.AT_MOST;
    //     final int customHeight = lp.height >= 0 ?
    //             Math.min(lp.height, height) : height;
    //     mCustomView.measure(MeasureSpec.makeMeasureSpec(customWidth, customWidthMode),
    //             MeasureSpec.makeMeasureSpec(customHeight, customHeightMode));
    // }
    //
    // if (mContentHeight <= 0) {
    //     int measuredHeight = 0;
    //     final int count = getChildCount();
    //     for (int i = 0; i < count; i++) {
    //         View v = getChildAt(i);
    //         int paddedViewHeight = v.getMeasuredHeight() + verticalPadding;
    //         if (paddedViewHeight > measuredHeight) {
    //             measuredHeight = paddedViewHeight;
    //         }
    //     }
    //     setMeasuredDimension(contentWidth, measuredHeight);
    // } else {
    //     setMeasuredDimension(contentWidth, maxHeight);
    // }

    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    if (widthMode != View::MeasureSpec::EXACTLY) {
        /*throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
                "with android:layout_width=\"match_parent\" (or fill_parent)");*/
        return ;
    }

    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    if (heightMode == View::MeasureSpec::UNSPECIFIED) {
        /*throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
                "with android:layout_height=\"wrap_content\"");*/
        return ;
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

    AutoPtr<IViewParent> vp;
    IView::Probe(mMenuView)->GetParent((IViewParent**)&vp);
    if (mMenuView != NULL && (TO_IINTERFACE(vp)== TO_IINTERFACE(this))) {
        availableWidth = MeasureChildView(IView::Probe(mMenuView), availableWidth, childSpecHeight, 0);
    }

    if (mTitleLayout != NULL && mCustomView == NULL) {
        if (mTitleOptional) {
            Int32 titleWidthSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
            IView::Probe(mTitleLayout)->Measure(titleWidthSpec, childSpecHeight);
            Int32 titleWidth;
            IView::Probe(mTitleLayout)->GetMeasuredWidth(&titleWidth);

            Boolean titleFits = titleWidth <= availableWidth;
            if (titleFits) {
                availableWidth -= titleWidth;
            }
            IView::Probe(mTitleLayout)->SetVisibility(titleFits ? IView::VISIBLE : IView::GONE);
        }
        else {
            availableWidth = MeasureChildView(IView::Probe(mTitleLayout), availableWidth, childSpecHeight, 0);
        }
    }

    if (mCustomView != NULL) {
        AutoPtr<IViewGroupLayoutParams> lp;
        mCustomView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        Int32 w;
        lp->GetWidth(&w);
        Int32 customWidthMode = w != IViewGroupLayoutParams::WRAP_CONTENT ? MeasureSpec::EXACTLY : MeasureSpec::AT_MOST;
        Int32 customWidth = w >= 0 ? Elastos::Core::Math::Min(w, availableWidth) : availableWidth;
        Int32 h;
        lp->GetHeight(&h);
        Int32 customHeightMode = h != IViewGroupLayoutParams::WRAP_CONTENT ? View::MeasureSpec::EXACTLY : View::MeasureSpec::AT_MOST;
        Int32 customHeight = h >= 0 ? Elastos::Core::Math::Min(h, height) : height;
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
}

ECode ActionBarContextView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // ==================before translated======================
    // final boolean isLayoutRtl = isLayoutRtl();
    // int x = isLayoutRtl ? r - l - getPaddingRight() : getPaddingLeft();
    // final int y = getPaddingTop();
    // final int contentHeight = b - t - getPaddingTop() - getPaddingBottom();
    //
    // if (mClose != null && mClose.getVisibility() != GONE) {
    //     MarginLayoutParams lp = (MarginLayoutParams) mClose.getLayoutParams();
    //     final int startMargin = (isLayoutRtl ? lp.rightMargin : lp.leftMargin);
    //     final int endMargin = (isLayoutRtl ? lp.leftMargin : lp.rightMargin);
    //     x = next(x, startMargin, isLayoutRtl);
    //     x += positionChild(mClose, x, y, contentHeight, isLayoutRtl);
    //     x = next(x, endMargin, isLayoutRtl);
    //
    //     if (mAnimateInOnLayout) {
    //         mAnimationMode = ANIMATE_IN;
    //         mCurrentAnimation = makeInAnimation();
    //         mCurrentAnimation.start();
    //         mAnimateInOnLayout = false;
    //     }
    // }
    //
    // if (mTitleLayout != null && mCustomView == null && mTitleLayout.getVisibility() != GONE) {
    //     x += positionChild(mTitleLayout, x, y, contentHeight, isLayoutRtl);
    // }
    //
    // if (mCustomView != null) {
    //     x += positionChild(mCustomView, x, y, contentHeight, isLayoutRtl);
    // }
    //
    // x = isLayoutRtl ? getPaddingLeft() : r - l - getPaddingRight();
    //
    // if (mMenuView != null) {
    //     x += positionChild(mMenuView, x, y, contentHeight, !isLayoutRtl);
    // }

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
    mClose->GetVisibility(&visible);
    if (mClose != NULL && visible != IView::GONE) {
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

    IView::Probe(mTitleLayout)->GetVisibility(&visible);
    if (mTitleLayout != NULL && mCustomView == NULL && visible != IView::GONE) {
        x += PositionChild(IView::Probe(mTitleLayout), x, y, contentHeight, isLayoutRtl);
    }

    if (mCustomView != NULL) {
        x += PositionChild(IView::Probe(mCustomView), x, y, contentHeight, isLayoutRtl);
    }

    x = isLayoutRtl ? paddingLeft : r - l - paddingRight;
    if (mMenuView != NULL) {
        x += PositionChild(IView::Probe(mMenuView), x, y, contentHeight, !isLayoutRtl);
    }
    return NOERROR;
}

void ActionBarContextView::InitTitle()
{
    // ==================before translated======================
    // if (mTitleLayout == null) {
    //     LayoutInflater inflater = LayoutInflater.from(getContext());
    //     inflater.inflate(R.layout.action_bar_title_item, this);
    //     mTitleLayout = (LinearLayout) getChildAt(getChildCount() - 1);
    //     mTitleView = (TextView) mTitleLayout.findViewById(R.id.action_bar_title);
    //     mSubtitleView = (TextView) mTitleLayout.findViewById(R.id.action_bar_subtitle);
    //     if (mTitleStyleRes != 0) {
    //         mTitleView.setTextAppearance(mContext, mTitleStyleRes);
    //     }
    //     if (mSubtitleStyleRes != 0) {
    //         mSubtitleView.setTextAppearance(mContext, mSubtitleStyleRes);
    //     }
    // }
    //
    // mTitleView.setText(mTitle);
    // mSubtitleView.setText(mSubtitle);
    //
    // final boolean hasTitle = !TextUtils.isEmpty(mTitle);
    // final boolean hasSubtitle = !TextUtils.isEmpty(mSubtitle);
    // mSubtitleView.setVisibility(hasSubtitle ? VISIBLE : GONE);
    // mTitleLayout.setVisibility(hasTitle || hasSubtitle ? VISIBLE : GONE);
    // if (mTitleLayout.getParent() == null) {
    //     addView(mTitleLayout);
    // }

    if (mTitleLayout == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(context, (ILayoutInflater**)&inflater);
        assert(inflater != NULL);
        AutoPtr<IView> view;
        inflater->Inflate(R::layout::action_bar_title_item, IViewGroup::Probe(this), (IView**)&view);

        Int32 count = 0;
        GetChildCount(&count);
        AutoPtr<IView> viewTmp;
        GetChildAt(count - 1, (IView**)&viewTmp);
        mTitleLayout = ILinearLayout::Probe(viewTmp);
        viewTmp = NULL;
        IView::Probe(mTitleLayout)->FindViewById(R::id::action_bar_title, (IView**)&viewTmp);
        mTitleView = ITextView::Probe(viewTmp);
        viewTmp = NULL;
        IView::Probe(mTitleLayout)->FindViewById(R::id::action_bar_subtitle, (IView**)&viewTmp);
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
    IView::Probe(mTitleLayout)->SetVisibility(hasTitle || hasSubtitle ? IView::VISIBLE : IView::GONE);

    AutoPtr<IViewParent> vp;
    IView::Probe(mTitleLayout)->GetParent((IViewParent**)&vp);
    if (vp == NULL) {
        AddView(IView::Probe(mTitleLayout));
    }
}

void ActionBarContextView::FinishAnimation()
{
    // ==================before translated======================
    // final Animator a = mCurrentAnimation;
    // if (a != null) {
    //     mCurrentAnimation = null;
    //     a.end();
    // }

    AutoPtr<IAnimator> a = mCurrentAnimation;
    if (a != NULL) {
        mCurrentAnimation = NULL;
        a->End();
    }
}

AutoPtr<IAnimator> ActionBarContextView::MakeInAnimation()
{
    // ==================before translated======================
    // mClose.setTranslationX(-mClose.getWidth() -
    //         ((MarginLayoutParams) mClose.getLayoutParams()).leftMargin);
    // ObjectAnimator buttonAnimator = ObjectAnimator.ofFloat(mClose, "translationX", 0);
    // buttonAnimator.setDuration(200);
    // buttonAnimator.addListener(this);
    // buttonAnimator.setInterpolator(new DecelerateInterpolator());
    //
    // AnimatorSet set = new AnimatorSet();
    // AnimatorSet.Builder b = set.play(buttonAnimator);
    //
    // if (mMenuView != null) {
    //     final int count = mMenuView.getChildCount();
    //     if (count > 0) {
    //         for (int i = count - 1, j = 0; i >= 0; i--, j++) {
    //             View child = mMenuView.getChildAt(i);
    //             child.setScaleY(0);
    //             ObjectAnimator a = ObjectAnimator.ofFloat(child, "scaleY", 0, 1);
    //             a.setDuration(300);
    //             b.with(a);
    //         }
    //     }
    // }
    //
    // return set;

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
    IAnimator::Probe(buttonAnimator)->AddListener(IAnimatorListener::Probe(this));

    AutoPtr<IDecelerateInterpolator> decele;
    CDecelerateInterpolator::New((IDecelerateInterpolator**)&decele);
    IAnimator::Probe(buttonAnimator)->SetInterpolator(ITimeInterpolator::Probe(decele));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IAnimatorSetBuilder> b;
    animatorSet->Play(IAnimator::Probe(buttonAnimator), (IAnimatorSetBuilder**)&b);

    if (mMenuView != NULL) {
        Int32 count = 0;
        IViewGroup::Probe(mMenuView)->GetChildCount(&count);
        if (count > 0) {
            for (Int32 i = count - 1, j = 0; i >= 0; --i, ++j) {
                AutoPtr<IView> child;
                IViewGroup::Probe(mMenuView)->GetChildAt(i, (IView**)&child);
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

    AutoPtr<IAnimator> result = IAnimator::Probe(animatorSet);
    return result;
}


AutoPtr<IAnimator> ActionBarContextView::MakeOutAnimation()
{
    // ==================before translated======================
    // ObjectAnimator buttonAnimator = ObjectAnimator.ofFloat(mClose, "translationX",
    //         -mClose.getWidth() - ((MarginLayoutParams) mClose.getLayoutParams()).leftMargin);
    // buttonAnimator.setDuration(200);
    // buttonAnimator.addListener(this);
    // buttonAnimator.setInterpolator(new DecelerateInterpolator());
    //
    // AnimatorSet set = new AnimatorSet();
    // AnimatorSet.Builder b = set.play(buttonAnimator);
    //
    // if (mMenuView != null) {
    //     final int count = mMenuView.getChildCount();
    //     if (count > 0) {
    //         for (int i = 0; i < 0; i++) {
    //             View child = mMenuView.getChildAt(i);
    //             child.setScaleY(0);
    //             ObjectAnimator a = ObjectAnimator.ofFloat(child, "scaleY", 0);
    //             a.setDuration(300);
    //             b.with(a);
    //         }
    //     }
    // }
    //
    // return set;

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
    IAnimator::Probe(buttonAnimator)->AddListener(IAnimatorListener::Probe(this));
    AutoPtr<IDecelerateInterpolator> decele;
    CDecelerateInterpolator::New((IDecelerateInterpolator**)&decele);
    IAnimator::Probe(buttonAnimator)->SetInterpolator(ITimeInterpolator::Probe(decele));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IAnimatorSetBuilder> b;
    animatorSet->Play(IAnimator::Probe(buttonAnimator), (IAnimatorSetBuilder**)&b);

    if (mMenuView != NULL) {
        Int32 count;
        IViewGroup::Probe(mMenuView)->GetChildCount(&count);
        if (count > 0) {
            for (Int32 i = 0; i < count; ++i) {
                AutoPtr<IView> child;
                IViewGroup::Probe(mMenuView)->GetChildAt(i, (IView**)&child);
                child->SetScaleY(0);

                AutoPtr<ArrayOf<Float> > tmpValues = ArrayOf<Float>::Alloc(1);
                (*tmpValues)[0] = 0.0f;
                AutoPtr<IObjectAnimator> a = CObjectAnimator::OfFloat(child, String("scaleY"), tmpValues);
                IValueAnimator::Probe(a)->SetDuration(300);
                b->With(IAnimator::Probe(a));
            }
        }
    }

    AutoPtr<IAnimator> result = IAnimator::Probe(animatorSet);
    return result;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


