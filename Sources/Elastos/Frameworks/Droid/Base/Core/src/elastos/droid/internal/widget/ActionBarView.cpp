
#include "elastos/droid/app/CActionBarLayoutParams.h"
#include "elastos/droid/internal/transition/ActionBarTransition.h"
#include "elastos/droid/internal/widget/ActionBarView.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CActionMenuPresenter.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/widget/CProgressBar.h"
#include "elastos/droid/widget/CSpinner.h"
#include "elastos/core/Math.h"

using Elastos::Droid::App::CActionBarLayoutParams;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Internal::Transition::ActionBarTransition;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IBaseMenuPresenter;
using Elastos::Droid::Internal::Widget::EIID_IActionBarView;
using Elastos::Droid::Internal::Widget::EIID_IDecorToolbar;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::ICollapsibleActionView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CActionMenuPresenter;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::CProgressBar;
using Elastos::Droid::Widget::CSpinner;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//                      ActionBarView::SavedState
//=====================================================================
const AutoPtr<IParcelable> ActionBarView::SavedState::CREATOR = new ActionBarView::InnerParcelableCreator();

ActionBarView::SavedState::SavedState(
    /* [in] */ IParcelable* superState)
{
    // ==================before translated======================
    // super(superState);

    View::BaseSavedState::constructor(superState);
}

ECode ActionBarView::SavedState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // super.writeToParcel(out, flags);
    // out.writeInt(expandedMenuItemId);
    // out.writeInt(isOverflowOpen ? 1 : 0);

    assert(0);
    //View::BaseSavedState::WriteToParcel(out, flags);
    out->WriteInt32(expandedMenuItemId);
    out->WriteInt32(isOverflowOpen ? 1 : 0);
    return NOERROR;
}

ActionBarView::SavedState::SavedState(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // super(in);
    // expandedMenuItemId = in.readInt();
    // isOverflowOpen = in.readInt() != 0;

    assert(0);
    //View::BaseSavedState::constructor(superState);
    in->ReadInt32(&expandedMenuItemId);
    Int32 intTmp = 0;
    in->ReadInt32(&intTmp);
    isOverflowOpen = intTmp != 0;
}

//=====================================================================
//                 ActionBarView::InnerOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarView::InnerOnClickListener, Object, IViewOnClickListener)

ActionBarView::InnerOnClickListener::InnerOnClickListener(
    /* [in] */ ActionBarView* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarView::InnerOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // final MenuItemImpl item = mExpandedMenuPresenter.mCurrentExpandedItem;
    // if (item != null) {
    //     item.collapseActionView();
    // }

    AutoPtr<IMenuItemImpl> item = mOwner->mExpandedMenuPresenter->mCurrentExpandedItem;
    if (item) {
        Boolean res = FALSE;
        IMenuItem::Probe(item)->CollapseActionView(&res);
    }
    return NOERROR;
}

//=====================================================================
//                 ActionBarView::InnerOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarView::InnerOnClickListener1, Object, IViewOnClickListener)

ActionBarView::InnerOnClickListener1::InnerOnClickListener1(
    /* [in] */ ActionBarView* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarView::InnerOnClickListener1::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if (mMenuPrepared) {
    //     // Only invoke the window callback if the options menu has been initialized.
    //     mWindowCallback.onMenuItemSelected(Window.FEATURE_OPTIONS_PANEL, mLogoNavItem);
    // }

    if (mOwner->mWindowCallback) {
        Boolean res = FALSE;
        mOwner->mWindowCallback->OnMenuItemSelected(IWindow::FEATURE_OPTIONS_PANEL, IMenuItem::Probe(mOwner->mLogoNavItem), &res);
    }
    return NOERROR;
}

//=====================================================================
//                ActionBarView::InnerParcelableCreator
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarView::InnerParcelableCreator, Object, IParcelable)

ActionBarView::InnerParcelableCreator::InnerParcelableCreator()
{
}

ECode ActionBarView::InnerParcelableCreator::CreateFromParcel(
    /* [in] */ IParcel* in,
    /* [out] */ SavedState** result)
{
    VALIDATE_NOT_NULL(in);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SavedState(in);

    assert(0);
    *result = new SavedState(in);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::InnerParcelableCreator::NewArray(
    /* [in] */ Int32 size,
    /* [out] */ ArrayOf<SavedState*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SavedState[size];

    *result = ArrayOf<SavedState*>::Alloc(size);
    for (Int32 idx=0; idx<size; ++idx) {
        AutoPtr<SavedState> item;// = new SavedState();
        (*result)->Set(idx, item);
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::InnerParcelableCreator::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode ActionBarView::InnerParcelableCreator::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

//=====================================================================
//                       ActionBarView::HomeView
//=====================================================================
const Int64 ActionBarView::HomeView::DEFAULT_TRANSITION_DURATION;

ActionBarView::HomeView::HomeView()
    : mUpWidth(0)
    , mStartOffset(0)
    , mUpIndicatorRes(0)
{
}

ECode ActionBarView::HomeView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode ActionBarView::HomeView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(attrs);
    // ==================before translated======================
    // super(context, attrs);
    // LayoutTransition t = getLayoutTransition();
    // if (t != null) {
    //     // Set a lower duration than the default
    //     t.setDuration(DEFAULT_TRANSITION_DURATION);
    // }

    FrameLayout::constructor(context, attrs);
    AutoPtr<ILayoutTransition> t;
    GetLayoutTransition((ILayoutTransition**)&t);
    if (t != NULL) {
        // Set a lower duration than the default
        t->SetDuration(DEFAULT_TRANSITION_DURATION);
    }
    return NOERROR;
}

ECode ActionBarView::HomeView::SetShowUp(
    /* [in] */ Boolean isUp)
{
    // ==================before translated======================
    // mUpView.setVisibility(isUp ? VISIBLE : GONE);

    IView::Probe(mUpView)->SetVisibility(isUp ? VISIBLE : IView::GONE);
    return NOERROR;
}

ECode ActionBarView::HomeView::SetShowIcon(
    /* [in] */ Boolean showIcon)
{
    // ==================before translated======================
    // mIconView.setVisibility(showIcon ? VISIBLE : GONE);

    IView::Probe(mIconView)->SetVisibility(showIcon ? VISIBLE : IView::GONE);
    return NOERROR;
}

ECode ActionBarView::HomeView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    VALIDATE_NOT_NULL(icon);
    // ==================before translated======================
    // mIconView.setImageDrawable(icon);

    mIconView->SetImageDrawable(icon);
    return NOERROR;
}

ECode ActionBarView::HomeView::SetUpIndicator(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // mUpIndicator = d;
    // mUpIndicatorRes = 0;
    // updateUpIndicator();

    mUpIndicator = d;
    mUpIndicatorRes = 0;
    UpdateUpIndicator();
    return NOERROR;
}

ECode ActionBarView::HomeView::SetDefaultUpIndicator(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // mDefaultUpIndicator = d;
    // updateUpIndicator();

    mDefaultUpIndicator = d;
    UpdateUpIndicator();
    return NOERROR;
}

ECode ActionBarView::HomeView::SetUpIndicator(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // mUpIndicatorRes = resId;
    // mUpIndicator = null;
    // updateUpIndicator();

    mUpIndicatorRes = resId;
    mUpIndicator = NULL;
    UpdateUpIndicator();
    return NOERROR;
}

ECode ActionBarView::HomeView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    // ==================before translated======================
    // onPopulateAccessibilityEvent(event);
    // return true;

    OnPopulateAccessibilityEvent(event);
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarView::HomeView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // super.onPopulateAccessibilityEvent(event);
    // final CharSequence cdesc = getContentDescription();
    // if (!TextUtils.isEmpty(cdesc)) {
    //     event.getText().add(cdesc);
    // }

    FrameLayout::OnPopulateAccessibilityEvent(event);
    AutoPtr<ICharSequence> cdesc;
    GetContentDescription((ICharSequence**)&cdesc);
    if (!TextUtils::IsEmpty(cdesc)) {
        AutoPtr<IList> text;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&text);
        text->Add(TO_IINTERFACE(cdesc));
    }
    return NOERROR;
}

ECode ActionBarView::HomeView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Don't allow children to hover; we want this to be treated as a single component.
    // return onHoverEvent(event);

    OnHoverEvent(event, result);
    return NOERROR;
}

ECode ActionBarView::HomeView::GetStartOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUpView.getVisibility() == GONE ? mStartOffset : 0;

    Int32 visibility = 0;
    IView::Probe(mUpView)->GetVisibility(&visibility);
    *result = visibility == IView::GONE ? mStartOffset : 0;
    return NOERROR;
}

ECode ActionBarView::HomeView::GetUpWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUpWidth;

    *result = mUpWidth;
    return NOERROR;
}

ECode ActionBarView::GetContext(
    /* [out] */ IContext** result)
{
    return NOERROR;
}

ECode ActionBarView::SetSplitView(
    /* [in] */ IViewGroup* splitView)
{
    return NOERROR;
}

ECode ActionBarView::SetSplitWhenNarrow(
    /* [in] */ Boolean splitWhenNarrow)
{
    return NOERROR;
}

ECode ActionBarView::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode ActionBarView::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode ActionBarView::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode ActionBarView::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode ActionBarView::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode ActionBarView::DismissPopupMenus()
{
    return NOERROR;
}

ECode ActionBarView::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
    return NOERROR;
}

ECode ActionBarView::SaveHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    return NOERROR;
}

ECode ActionBarView::RestoreHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    return NOERROR;
}

void ActionBarView::HomeView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    // if (mUpIndicatorRes != 0) {
    //     // Reload for config change
    //     updateUpIndicator();
    // }

    FrameLayout::OnConfigurationChanged(newConfig);
    if (mUpIndicatorRes != 0) {
        // Reload for config change
        UpdateUpIndicator();
    }
}

ECode ActionBarView::HomeView::OnFinishInflate()
{
    // ==================before translated======================
    // mUpView = (ImageView) findViewById(com.android.internal.R.id.up);
    // mIconView = (ImageView) findViewById(com.android.internal.R.id.home);
    // mDefaultUpIndicator = mUpView.getDrawable();

    AutoPtr<IView> viewTmp;
    FindViewById(R::id::up, (IView**)&viewTmp);
    mUpView = IImageView::Probe(viewTmp);

    AutoPtr<IView> viewTmp1;
    FindViewById(R::id::home, (IView**)&viewTmp1);
    mIconView = IImageView::Probe(viewTmp1);
    mUpView->GetDrawable((IDrawable**)&mDefaultUpIndicator);
    return NOERROR;
}

void ActionBarView::HomeView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // measureChildWithMargins(mUpView, widthMeasureSpec, 0, heightMeasureSpec, 0);
    // final LayoutParams upLp = (LayoutParams) mUpView.getLayoutParams();
    // final int upMargins = upLp.leftMargin + upLp.rightMargin;
    // mUpWidth = mUpView.getMeasuredWidth();
    // mStartOffset = mUpWidth + upMargins;
    // int width = mUpView.getVisibility() == GONE ? 0 : mStartOffset;
    // int height = upLp.topMargin + mUpView.getMeasuredHeight() + upLp.bottomMargin;
    //
    // if (mIconView.getVisibility() != GONE) {
    //     measureChildWithMargins(mIconView, widthMeasureSpec, width, heightMeasureSpec, 0);
    //     final LayoutParams iconLp = (LayoutParams) mIconView.getLayoutParams();
    //     width += iconLp.leftMargin + mIconView.getMeasuredWidth() + iconLp.rightMargin;
    //     height = Math.max(height,
    //             iconLp.topMargin + mIconView.getMeasuredHeight() + iconLp.bottomMargin);
    // } else if (upMargins < 0) {
    //     // Remove the measurement effects of negative margins used for offsets
    //     width -= upMargins;
    // }
    //
    // final int widthMode = MeasureSpec.getMode(widthMeasureSpec);
    // final int heightMode = MeasureSpec.getMode(heightMeasureSpec);
    // final int widthSize = MeasureSpec.getSize(widthMeasureSpec);
    // final int heightSize = MeasureSpec.getSize(heightMeasureSpec);
    //
    // switch (widthMode) {
    //     case MeasureSpec.AT_MOST:
    //         width = Math.min(width, widthSize);
    //         break;
    //     case MeasureSpec.EXACTLY:
    //         width = widthSize;
    //         break;
    //     case MeasureSpec.UNSPECIFIED:
    //     default:
    //         break;
    // }
    // switch (heightMode) {
    //     case MeasureSpec.AT_MOST:
    //         height = Math.min(height, heightSize);
    //         break;
    //     case MeasureSpec.EXACTLY:
    //         height = heightSize;
    //         break;
    //     case MeasureSpec.UNSPECIFIED:
    //     default:
    //         break;
    // }
    // setMeasuredDimension(width, height);

    MeasureChildWithMargins(IView::Probe(mUpView), widthMeasureSpec, 0, heightMeasureSpec, 0);
    AutoPtr<IViewGroupLayoutParams> upLp;
    IView::Probe(mUpView)->GetLayoutParams((IViewGroupLayoutParams**)&upLp);

    Int32 leftMargin = 0, rightMargin = 0;
    IViewGroupMarginLayoutParams::Probe(upLp)->GetLeftMargin(&leftMargin);
    IViewGroupMarginLayoutParams::Probe(upLp)->GetRightMargin(&rightMargin);

    Int32 upMargins = leftMargin + rightMargin;
    IView::Probe(mUpView)->GetMeasuredWidth(&mUpWidth);
    mStartOffset = mUpWidth + upMargins;

    Int32 upviewVisibility = 0;
    IView::Probe(mUpView)->GetVisibility(&upviewVisibility);

    Int32 topMargin = 0, bottomMargin = 0;
    IViewGroupMarginLayoutParams::Probe(upLp)->GetTopMargin(&topMargin);

    Int32 measureHeight = 0;
    IView::Probe(mUpView)->GetMeasuredHeight(&measureHeight);

    Int32 width = upviewVisibility == IView::GONE ? 0 : mStartOffset;
    Int32 height = topMargin + measureHeight + bottomMargin;

    Int32 iconviewVisibility = 0;
    IView::Probe(mIconView)->GetVisibility(&iconviewVisibility);
    if (iconviewVisibility != IView::GONE) {
        MeasureChildWithMargins(IView::Probe(mIconView), widthMeasureSpec, width, heightMeasureSpec, 0);

        AutoPtr<IViewGroupLayoutParams> iconLp;
        IView::Probe(mIconView)->GetLayoutParams((IViewGroupLayoutParams**)&iconLp);

        Int32 measureWidth = 0, measureHeight = 0;
        IView::Probe(mIconView)->GetMeasuredWidth(&measureWidth);
        IView::Probe(mIconView)->GetMeasuredHeight(&measureHeight);
        IViewGroupMarginLayoutParams::Probe(iconLp)->GetLeftMargin(&leftMargin);
        IViewGroupMarginLayoutParams::Probe(iconLp)->GetRightMargin(&rightMargin);
        IViewGroupMarginLayoutParams::Probe(iconLp)->GetTopMargin(&topMargin);
        IViewGroupMarginLayoutParams::Probe(iconLp)->GetBottomMargin(&bottomMargin);
        width += leftMargin + measureWidth + rightMargin;

        height = Elastos::Core::Math::Max(height, topMargin + measureHeight + bottomMargin);
    }
    else if (upMargins < 0) {
        // Remove the measurement effects of negative margins used for offsets
        width -= upMargins;
    }

    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);

    switch (widthMode) {
        case View::MeasureSpec::AT_MOST:
            width = Elastos::Core::Math::Min(width, widthSize);
            break;
        case View::MeasureSpec::EXACTLY:
            width = widthSize;
            break;
        case View::MeasureSpec::UNSPECIFIED:
        default:
            break;
    }
    switch (heightMode) {
        case View::MeasureSpec::AT_MOST:
            height = Elastos::Core::Math::Min(height, heightSize);
            break;
        case View::MeasureSpec::EXACTLY:
            height = heightSize;
            break;
        case View::MeasureSpec::UNSPECIFIED:
        default:
            break;
    }
    SetMeasuredDimension(width, height);
}

ECode ActionBarView::HomeView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // ==================before translated======================
    // final int vCenter = (b - t) / 2;
    // final boolean isLayoutRtl = isLayoutRtl();
    // final int width = getWidth();
    // int upOffset = 0;
    // if (mUpView.getVisibility() != GONE) {
    //     final LayoutParams upLp = (LayoutParams) mUpView.getLayoutParams();
    //     final int upHeight = mUpView.getMeasuredHeight();
    //     final int upWidth = mUpView.getMeasuredWidth();
    //     upOffset = upLp.leftMargin + upWidth + upLp.rightMargin;
    //     final int upTop = vCenter - upHeight / 2;
    //     final int upBottom = upTop + upHeight;
    //     final int upRight;
    //     final int upLeft;
    //     if (isLayoutRtl) {
    //         upRight = width;
    //         upLeft = upRight - upWidth;
    //         r -= upOffset;
    //     } else {
    //         upRight = upWidth;
    //         upLeft = 0;
    //         l += upOffset;
    //     }
    //     mUpView.layout(upLeft, upTop, upRight, upBottom);
    // }
    //
    // final LayoutParams iconLp = (LayoutParams) mIconView.getLayoutParams();
    // final int iconHeight = mIconView.getMeasuredHeight();
    // final int iconWidth = mIconView.getMeasuredWidth();
    // final int hCenter = (r - l) / 2;
    // final int iconTop = Math.max(iconLp.topMargin, vCenter - iconHeight / 2);
    // final int iconBottom = iconTop + iconHeight;
    // final int iconLeft;
    // final int iconRight;
    // int marginStart = iconLp.getMarginStart();
    // final int delta = Math.max(marginStart, hCenter - iconWidth / 2);
    // if (isLayoutRtl) {
    //     iconRight = width - upOffset - delta;
    //     iconLeft = iconRight - iconWidth;
    // } else {
    //     iconLeft = upOffset + delta;
    //     iconRight = iconLeft + iconWidth;
    // }
    // mIconView.layout(iconLeft, iconTop, iconRight, iconBottom);

    Int32 vCenter = (b - t) / 2;
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);

    Int32 width = 0;
    GetWidth(&width);
    Int32 upOffset = 0;

    Int32 visibility = 0;
    IView::Probe(mUpView)->GetVisibility(&visibility);
    if (visibility != IView::GONE) {
        AutoPtr<IViewGroupLayoutParams> upLp;
        IView::Probe(mUpView)->GetLayoutParams((IViewGroupLayoutParams**)&upLp);

        Int32 upHeight = 0, upWidth = 0;
        IView::Probe(mUpView)->GetMeasuredWidth(&upWidth);
        IView::Probe(mUpView)->GetMeasuredHeight(&upHeight);

        Int32 leftMargin = 0, rightMargin = 0;
        IViewGroupMarginLayoutParams::Probe(upLp)->GetLeftMargin(&leftMargin);
        IViewGroupMarginLayoutParams::Probe(upLp)->GetRightMargin(&rightMargin);

        upOffset = leftMargin + upWidth + rightMargin;
        Int32 upTop = vCenter - upHeight / 2;
        Int32 upBottom = upTop + upHeight;
        Int32 upRight = 0;
        Int32 upLeft = 0;
        if (isLayoutRtl) {
            upRight = width;
            upLeft = upRight - upWidth;
            r -= upOffset;
        }
        else {
            upRight = upWidth;
            upLeft = 0;
            l += upOffset;
        }
        IView::Probe(mUpView)->Layout(upLeft, upTop, upRight, upBottom);
    }

    AutoPtr<IViewGroupLayoutParams> iconLp;
    IView::Probe(mIconView)->GetLayoutParams((IViewGroupLayoutParams**)&iconLp);

    Int32 iconWidth = 0, iconHeight = 0;
    IView::Probe(mIconView)->GetMeasuredWidth(&iconWidth);
    IView::Probe(mIconView)->GetMeasuredHeight(&iconHeight);

    Int32 hCenter = (r - l) / 2;
    Int32 topMargin = 0;
    IViewGroupMarginLayoutParams::Probe(iconLp)->GetTopMargin(&topMargin);
    Int32 iconTop = Elastos::Core::Math::Max(topMargin, vCenter - iconHeight / 2);
    Int32 iconBottom = iconTop + iconHeight;
    Int32 iconLeft;
    Int32 iconRight;
    Int32 marginStart = 0;
    IViewGroupMarginLayoutParams::Probe(iconLp)->GetMarginStart(&marginStart);
    Int32 delta = Elastos::Core::Math::Max(marginStart, hCenter - iconWidth / 2);
    if (isLayoutRtl) {
        iconRight = width - upOffset - delta;
        iconLeft = iconRight - iconWidth;
    }
    else {
        iconLeft = upOffset + delta;
        iconRight = iconLeft + iconWidth;
    }
    IView::Probe(mIconView)->Layout(iconLeft, iconTop, iconRight, iconBottom);
    return NOERROR;
}

void ActionBarView::HomeView::UpdateUpIndicator()
{
    // ==================before translated======================
    // if (mUpIndicator != null) {
    //     mUpView.setImageDrawable(mUpIndicator);
    // } else if (mUpIndicatorRes != 0) {
    //     mUpView.setImageDrawable(getContext().getDrawable(mUpIndicatorRes));
    // } else {
    //     mUpView.setImageDrawable(mDefaultUpIndicator);
    // }

    if (mUpIndicator != NULL) {
        mUpView->SetImageDrawable(mUpIndicator);
    }
    else if (mUpIndicatorRes != 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(mUpIndicatorRes, (IDrawable**)&drawable);
        mUpView->SetImageDrawable(drawable);
    }
    else {
        mUpView->SetImageDrawable(mDefaultUpIndicator);
    }
}

//=====================================================================
//            ActionBarView::ExpandedActionViewMenuPresenter
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarView::ExpandedActionViewMenuPresenter, Object, IMenuPresenter)

ActionBarView::ExpandedActionViewMenuPresenter::ExpandedActionViewMenuPresenter(
    /* [in] */ ActionBarView* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(menu);
    // ==================before translated======================
    // // Clear the expanded action view when menus change.
    // if (mMenu != null && mCurrentExpandedItem != null) {
    //     mMenu.collapseItemActionView(mCurrentExpandedItem);
    // }
    // mMenu = menu;

    if (mMenu && mCurrentExpandedItem) {
        Boolean res = FALSE;
        mMenu->CollapseItemActionView(mCurrentExpandedItem, &res);
    }
    mMenu = menu;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** result)
{
    VALIDATE_NOT_NULL(root);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;

    *result = NULL;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    // ==================before translated======================
    // // Make sure the expanded item we have is still there.
    // if (mCurrentExpandedItem != null) {
    //     boolean found = false;
    //
    //     if (mMenu != null) {
    //         final int count = mMenu.size();
    //         for (int i = 0; i < count; i++) {
    //             final MenuItem item = mMenu.getItem(i);
    //             if (item == mCurrentExpandedItem) {
    //                 found = true;
    //                 break;
    //             }
    //         }
    //     }
    //
    //     if (!found) {
    //         // The item we had expanded disappeared. Collapse.
    //         collapseItemActionView(mMenu, mCurrentExpandedItem);
    //     }
    // }

    if (mCurrentExpandedItem) {
        Boolean found = FALSE;

        if (mMenu) {
            Int32 count = 0;
            IMenu::Probe(mMenu)->GetSize(&count);
            for (Int32 i = 0; i < count; ++i) {
                AutoPtr<IMenuItem> item;
                IMenu::Probe(mMenu)->GetItem(i, (IMenuItem**)&item);
                if (TO_IINTERFACE(item) == TO_IINTERFACE(mCurrentExpandedItem)) {
                    found = TRUE;
                    break;
                }
            }
        }
        if (!found) {
            Boolean res = FALSE;
            CollapseItemActionView(mMenu, mCurrentExpandedItem, &res);
        }
    }
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::SetCallback(
    /* [in] */ IMenuPresenterCallback* cb)
{
    VALIDATE_NOT_NULL(cb);
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(subMenu);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    VALIDATE_NOT_NULL(menu);
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::FlagActionItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::ExpandItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(menu);
    VALIDATE_NOT_NULL(item);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ActionBarTransition.beginDelayedTransition(ActionBarView.this);
    //
    // mExpandedActionView = item.getActionView();
    // mExpandedHomeLayout.setIcon(mIcon.getConstantState().newDrawable(getResources()));
    // mCurrentExpandedItem = item;
    // if (mExpandedActionView.getParent() != ActionBarView.this) {
    //     addView(mExpandedActionView);
    // }
    // if (mExpandedHomeLayout.getParent() != mUpGoerFive) {
    //     mUpGoerFive.addView(mExpandedHomeLayout);
    // }
    // mHomeLayout.setVisibility(GONE);
    // if (mTitleLayout != null) mTitleLayout.setVisibility(GONE);
    // if (mTabScrollView != null) mTabScrollView.setVisibility(GONE);
    // if (mSpinner != null) mSpinner.setVisibility(GONE);
    // if (mCustomNavView != null) mCustomNavView.setVisibility(GONE);
    // setHomeButtonEnabled(false, false);
    // requestLayout();
    // item.setActionViewExpanded(true);
    //
    // if (mExpandedActionView instanceof CollapsibleActionView) {
    //     ((CollapsibleActionView) mExpandedActionView).onActionViewExpanded();
    // }
    //
    // return true;

    IMenuItem::Probe(item)->GetActionView((IView**)&(mOwner->mExpandedActionView));
    AutoPtr<IDrawableConstantState> state;
    mOwner->mIcon->GetConstantState((IDrawableConstantState**)&state);

    AutoPtr<IResources> resources;
    mOwner->GetResources((IResources**)&resources);
    AutoPtr<IDrawable> d;
    state->NewDrawable(resources, (IDrawable**)&d);
    mOwner->mExpandedHomeLayout->SetIcon(d);
    mCurrentExpandedItem = item;
    AutoPtr<IViewParent> eavParent;
    mOwner->mExpandedActionView->GetParent((IViewParent**)&eavParent);
    if (TO_IINTERFACE(eavParent) != TO_IINTERFACE(mOwner)) {
        mOwner->AddView(mOwner->mExpandedActionView);
    }
    AutoPtr<IViewParent> ehlParent;
    mOwner->mExpandedHomeLayout->GetParent((IViewParent**)&ehlParent);
    if (TO_IINTERFACE(ehlParent) != TO_IINTERFACE(mOwner)) {
        mOwner->AddView(mOwner->mExpandedHomeLayout);
    }
    mOwner->mHomeLayout->SetVisibility(IView::GONE);
    if (mOwner->mTitleLayout)
        IView::Probe(mOwner->mTitleLayout)->SetVisibility(IView::GONE);
    if (mOwner->mTabScrollView)
        IView::Probe(mOwner->mTabScrollView)->SetVisibility(IView::GONE);
    if (mOwner->mSpinner)
        IView::Probe(mOwner->mSpinner)->SetVisibility(IView::GONE);
    if (mOwner->mCustomNavView)
        IView::Probe(mOwner->mCustomNavView)->SetVisibility(IView::GONE);
    mOwner->RequestLayout();
    item->SetActionViewExpanded(TRUE);

    ICollapsibleActionView* viewTmp = ICollapsibleActionView::Probe(mOwner->mExpandedActionView);
    if (viewTmp) {
        viewTmp->OnActionViewExpanded();
    }
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::CollapseItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(menu);
    VALIDATE_NOT_NULL(item);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ActionBarTransition.beginDelayedTransition(ActionBarView.this);
    //
    // // Do this before detaching the actionview from the hierarchy, in case
    // // it needs to dismiss the soft keyboard, etc.
    // if (mExpandedActionView instanceof CollapsibleActionView) {
    //     ((CollapsibleActionView) mExpandedActionView).onActionViewCollapsed();
    // }
    //
    // removeView(mExpandedActionView);
    // mUpGoerFive.removeView(mExpandedHomeLayout);
    // mExpandedActionView = null;
    // if ((mDisplayOptions & ActionBar.DISPLAY_SHOW_HOME) != 0) {
    //     mHomeLayout.setVisibility(VISIBLE);
    // }
    // if ((mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //     if (mTitleLayout == null) {
    //         initTitle();
    //     } else {
    //         mTitleLayout.setVisibility(VISIBLE);
    //     }
    // }
    // if (mTabScrollView != null) mTabScrollView.setVisibility(VISIBLE);
    // if (mSpinner != null) mSpinner.setVisibility(VISIBLE);
    // if (mCustomNavView != null) mCustomNavView.setVisibility(VISIBLE);
    //
    // mExpandedHomeLayout.setIcon(null);
    // mCurrentExpandedItem = null;
    // setHomeButtonEnabled(mWasHomeEnabled); // Set by expandItemActionView above
    // requestLayout();
    // item.setActionViewExpanded(false);
    //
    // return true;

    assert(0);
    ActionBarTransition::BeginDelayedTransition(IViewGroup::Probe(this));
    ICollapsibleActionView* collapsibleActionView = ICollapsibleActionView::Probe(mOwner->mExpandedActionView);
    if (collapsibleActionView) {
        collapsibleActionView->OnActionViewCollapsed();
    }

    mOwner->RemoveView(mOwner->mExpandedActionView);
    mOwner->mUpGoerFive->RemoveView(mOwner->mExpandedHomeLayout);
    mOwner->mExpandedActionView = NULL;
    if ((mOwner->mDisplayOptions & IActionBar::DISPLAY_SHOW_HOME) != 0) {
        IView::Probe(mOwner->mHomeLayout)->SetVisibility(VISIBLE);
    }
    if ((mOwner->mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        if (mOwner->mTitleLayout == NULL) {
            mOwner->InitTitle();
        }
        else {
            IView::Probe(mOwner->mTitleLayout)->SetVisibility(VISIBLE);
        }
    }
    if (mOwner->mTabScrollView != NULL)
        IView::Probe(mOwner->mTabScrollView)->SetVisibility(VISIBLE);
    if (mOwner->mSpinner != NULL)
        IView::Probe(mOwner->mSpinner)->SetVisibility(VISIBLE);
    if (mOwner->mCustomNavView != NULL)
        IView::Probe(mOwner->mCustomNavView)->SetVisibility(VISIBLE);

    mOwner->mExpandedHomeLayout->SetIcon(NULL);
    mCurrentExpandedItem = NULL;
    mOwner->SetHomeButtonEnabled(mOwner->mWasHomeEnabled);
    mOwner->RequestLayout();
    item->SetActionViewExpanded(FALSE);
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0;

    *result = 0;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::OnSaveInstanceState(
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;

    *result = NULL;
    return NOERROR;
}

ECode ActionBarView::ExpandedActionViewMenuPresenter::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    VALIDATE_NOT_NULL(state);
    return NOERROR;
}

//=====================================================================
//                            ActionBarView
//=====================================================================
const Int32 ActionBarView::DISPLAY_DEFAULT;
const String ActionBarView::TAG("ActionBarView");
const Int32 ActionBarView::DISPLAY_RELAYOUT_MASK;
const Int32 ActionBarView::DEFAULT_CUSTOM_GRAVITY;

CAR_INTERFACE_IMPL_2(ActionBarView, AbsActionBarView, IDecorToolbar, IActionBarView)

ActionBarView::ActionBarView()
    : mNavigationMode(0)
    , mDisplayOptions(0)
    , mHomeDescriptionRes(0)
    , mItemPadding(0)
    , mTitleStyleRes(0)
    , mSubtitleStyleRes(0)
    , mProgressStyle(0)
    , mIndeterminateProgressStyle(0)
    , mUserTitle(FALSE)
    , mIncludeTabs(FALSE)
    , mIsCollapsible(FALSE)
    , mWasHomeEnabled(FALSE)
    , mMenuPrepared(FALSE)
    , mDefaultUpDescription(0)
{
}

ECode ActionBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(context, attrs);
    //
    // // Background is always provided by the container.
    // setBackgroundResource(0);
    //
    // TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.ActionBar,
    //         com.android.internal.R.attr.actionBarStyle, 0);
    //
    // mNavigationMode = a.getInt(R.styleable.ActionBar_navigationMode,
    //         ActionBar.NAVIGATION_MODE_STANDARD);
    // mTitle = a.getText(R.styleable.ActionBar_title);
    // mSubtitle = a.getText(R.styleable.ActionBar_subtitle);
    // mLogo = a.getDrawable(R.styleable.ActionBar_logo);
    // mIcon = a.getDrawable(R.styleable.ActionBar_icon);
    //
    // final LayoutInflater inflater = LayoutInflater.from(context);
    //
    // final int homeResId = a.getResourceId(
    //         com.android.internal.R.styleable.ActionBar_homeLayout,
    //         com.android.internal.R.layout.action_bar_home);
    //
    // mUpGoerFive = (ViewGroup) inflater.inflate(
    //         com.android.internal.R.layout.action_bar_up_container, this, false);
    // mHomeLayout = (HomeView) inflater.inflate(homeResId, mUpGoerFive, false);
    //
    // mExpandedHomeLayout = (HomeView) inflater.inflate(homeResId, mUpGoerFive, false);
    // mExpandedHomeLayout.setShowUp(true);
    // mExpandedHomeLayout.setOnClickListener(mExpandedActionViewUpListener);
    // mExpandedHomeLayout.setContentDescription(getResources().getText(
    //         mDefaultUpDescription));
    //
    // // This needs to highlight/be focusable on its own.
    // // TODO: Clean up the handoff between expanded/normal.
    // final Drawable upBackground = mUpGoerFive.getBackground();
    // if (upBackground != null) {
    //     mExpandedHomeLayout.setBackground(upBackground.getConstantState().newDrawable());
    // }
    // mExpandedHomeLayout.setEnabled(true);
    // mExpandedHomeLayout.setFocusable(true);
    //
    // mTitleStyleRes = a.getResourceId(R.styleable.ActionBar_titleTextStyle, 0);
    // mSubtitleStyleRes = a.getResourceId(R.styleable.ActionBar_subtitleTextStyle, 0);
    // mProgressStyle = a.getResourceId(R.styleable.ActionBar_progressBarStyle, 0);
    // mIndeterminateProgressStyle = a.getResourceId(
    //         R.styleable.ActionBar_indeterminateProgressStyle, 0);
    //
    // mProgressBarPadding = a.getDimensionPixelOffset(R.styleable.ActionBar_progressBarPadding, 0);
    // mItemPadding = a.getDimensionPixelOffset(R.styleable.ActionBar_itemPadding, 0);
    //
    // setDisplayOptions(a.getInt(R.styleable.ActionBar_displayOptions, DISPLAY_DEFAULT));
    //
    // final int customNavId = a.getResourceId(R.styleable.ActionBar_customNavigationLayout, 0);
    // if (customNavId != 0) {
    //     mCustomNavView = (View) inflater.inflate(customNavId, this, false);
    //     mNavigationMode = ActionBar.NAVIGATION_MODE_STANDARD;
    //     setDisplayOptions(mDisplayOptions | ActionBar.DISPLAY_SHOW_CUSTOM);
    // }
    //
    // mContentHeight = a.getLayoutDimension(R.styleable.ActionBar_height, 0);
    //
    // a.recycle();
    //
    // mLogoNavItem = new ActionMenuItem(context, 0, android.R.id.home, 0, 0, mTitle);
    //
    // mUpGoerFive.setOnClickListener(mUpClickListener);
    // mUpGoerFive.setClickable(true);
    // mUpGoerFive.setFocusable(true);
    //
    // if (getImportantForAccessibility() == View.IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
    //     setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_YES);
    // }

    assert(0);
    ViewGroup::constructor(context, attrs);
    SetBackgroundResource(0);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ActionBar),
        ArraySize(R::styleable::ActionBar));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, R::attr::actionBarStyle, 0, (ITypedArray**)&a);

    a->GetInt32(R::styleable::ActionBar_navigationMode, IActionBar::NAVIGATION_MODE_STANDARD, &mNavigationMode);
    a->GetText(R::styleable::ActionBar_title, (ICharSequence**)&mTitle);
    a->GetText(R::styleable::ActionBar_subtitle, (ICharSequence**)&mSubtitle);
    a->GetDrawable(R::styleable::ActionBar_logo, (IDrawable**)&mLogo);
    mIcon = NULL;
    a->GetDrawable(R::styleable::ActionBar_icon, (IDrawable**)&mIcon);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);

    Int32 homeResId = 0;
    a->GetResourceId(R::styleable::ActionBar_homeLayout, R::layout::action_bar_home, &homeResId);

    AutoPtr<IView> upView;
    inflater->Inflate(R::layout::action_bar_up_container, IViewGroup::Probe(this), FALSE, (IView**)&upView);
    mUpGoerFive = IViewGroup::Probe(upView);
    AutoPtr<IView> homev;
    inflater->Inflate(homeResId, mUpGoerFive, FALSE, (IView**)&homev);
    mHomeLayout = (HomeView*)homev.Get();
    AutoPtr<IView> expv;
    inflater->Inflate(homeResId, mUpGoerFive, FALSE, (IView**)&expv);
    mExpandedHomeLayout = (HomeView*)expv.Get();
    mExpandedHomeLayout->SetShowUp(TRUE);
    mExpandedHomeLayout->SetOnClickListener(mExpandedActionViewUpListener);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> csq;
    resources->GetText(mDefaultUpDescription, (ICharSequence**)&csq);
    mExpandedHomeLayout->SetContentDescription(csq);
    AutoPtr<IDrawable> upBackground;
    IView::Probe(mUpGoerFive)->GetBackground((IDrawable**)&upBackground);
    if (upBackground) {
        AutoPtr<IDrawableConstantState> upstate;
        upBackground->GetConstantState((IDrawableConstantState**)&upstate);
        AutoPtr<IDrawable> upd;
        upstate->NewDrawable((IDrawable**)&upd);
        mExpandedHomeLayout->SetBackground(upd);
    }
    mExpandedHomeLayout->SetEnabled(TRUE);
    mExpandedHomeLayout->SetFocusable(TRUE);

    a->GetResourceId(R::styleable::ActionBar_titleTextStyle, 0, &mTitleStyleRes);
    a->GetResourceId(R::styleable::ActionBar_subtitleTextStyle, 0, &mSubtitleStyleRes);
    a->GetResourceId(R::styleable::ActionBar_progressBarStyle, 0, &mProgressStyle);
    a->GetResourceId(R::styleable::ActionBar_indeterminateProgressStyle, 0, &mIndeterminateProgressStyle);

    a->GetDimensionPixelOffset(R::styleable::ActionBar_progressBarPadding, 0, &mProgressBarPadding);
    a->GetDimensionPixelOffset(R::styleable::ActionBar_itemPadding, 0, &mItemPadding);

    Int32 options = 0;
    a->GetInt32(R::styleable::ActionBar_displayOptions, DISPLAY_DEFAULT, &options);
    SetDisplayOptions(options);

    Int32 customNavId = 0;
    a->GetResourceId(R::styleable::ActionBar_customNavigationLayout, 0, &customNavId);
    if (customNavId != 0) {
        inflater->Inflate(customNavId, IViewGroup::Probe(this), FALSE, (IView**)&mCustomNavView);
        mNavigationMode = IActionBar::NAVIGATION_MODE_STANDARD;
        SetDisplayOptions(mDisplayOptions | IActionBar::DISPLAY_SHOW_CUSTOM);
    }

    a->GetLayoutDimension(R::styleable::ActionBar_height, 0, &mContentHeight);
    a->Recycle();

    //mLogoNavItem = new ActionMenuItem(context, 0, R::id::home, 0, 0, mTitle);
    mHomeLayout->SetOnClickListener(mUpClickListener);
    mHomeLayout->SetClickable(TRUE);
    mHomeLayout->SetFocusable(TRUE);

    Int32 accessibility = 0;
    GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode ActionBarView::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    // ==================before translated======================
    // mWindowCallback = cb;

    mWindowCallback = cb;
    return NOERROR;
}

ECode ActionBarView::OnDetachedFromWindow()
{
    // ==================before translated======================
    // super.onDetachedFromWindow();
    // removeCallbacks(mTabSelector);
    // if (mActionMenuPresenter != null) {
    //     mActionMenuPresenter.hideOverflowMenu();
    //     mActionMenuPresenter.hideSubMenus();
    // }

    AbsActionBarView::OnDetachedFromWindow();
    Boolean resTmp;
    RemoveCallbacks(mTabSelector, &resTmp);
    if (mActionMenuPresenter) {
        Boolean res = FALSE;
        mActionMenuPresenter->HideOverflowMenu(&res);
        mActionMenuPresenter->HideSubMenus(&res);
    }
    return NOERROR;
}

ECode ActionBarView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ActionBarView::InitProgress()
{
    // ==================before translated======================
    // mProgressView = new ProgressBar(mContext, null, 0, mProgressStyle);
    // mProgressView.setId(R.id.progress_horizontal);
    // mProgressView.setMax(10000);
    // mProgressView.setVisibility(GONE);
    // addView(mProgressView);

    CProgressBar::New(mContext, NULL, 0, mProgressStyle, (IProgressBar**)&mProgressView);
    IView::Probe(mProgressView)->SetId(R::id::progress_horizontal);
    mProgressView->SetMax(10000);
    IView::Probe(mProgressView)->SetVisibility(IView::GONE);
    AddView(IView::Probe(mProgressView));
    return NOERROR;
}

ECode ActionBarView::InitIndeterminateProgress()
{
    // ==================before translated======================
    // mIndeterminateProgressView = new ProgressBar(mContext, null, 0,
    //         mIndeterminateProgressStyle);
    // mIndeterminateProgressView.setId(R.id.progress_circular);
    // mIndeterminateProgressView.setVisibility(GONE);
    // addView(mIndeterminateProgressView);

    CProgressBar::New(mContext, NULL, 0, mIndeterminateProgressStyle, (IProgressBar**)&mIndeterminateProgressView);
    IView::Probe(mIndeterminateProgressView)->SetId(R::id::progress_circular);
    IView::Probe(mIndeterminateProgressView)->SetVisibility(IView::GONE);
    AddView(IView::Probe(mIndeterminateProgressView));
    return NOERROR;
}

ECode ActionBarView::SetSplitToolbar(
    /* [in] */ Boolean splitActionBar)
{
    // ==================before translated======================
    // if (mSplitActionBar != splitActionBar) {
    //     if (mMenuView != null) {
    //         final ViewGroup oldParent = (ViewGroup) mMenuView.getParent();
    //         if (oldParent != null) {
    //             oldParent.removeView(mMenuView);
    //         }
    //         if (splitActionBar) {
    //             if (mSplitView != null) {
    //                 mSplitView.addView(mMenuView);
    //             }
    //             mMenuView.getLayoutParams().width = LayoutParams.MATCH_PARENT;
    //         } else {
    //             addView(mMenuView);
    //             mMenuView.getLayoutParams().width = LayoutParams.WRAP_CONTENT;
    //         }
    //         mMenuView.requestLayout();
    //     }
    //     if (mSplitView != null) {
    //         mSplitView.setVisibility(splitActionBar ? VISIBLE : GONE);
    //     }
    //
    //     if (mActionMenuPresenter != null) {
    //         if (!splitActionBar) {
    //             mActionMenuPresenter.setExpandedActionViewsExclusive(
    //                     getResources().getBoolean(
    //                             com.android.internal.R.bool.action_bar_expanded_action_views_exclusive));
    //         } else {
    //             mActionMenuPresenter.setExpandedActionViewsExclusive(false);
    //             // Allow full screen width in split mode.
    //             mActionMenuPresenter.setWidthLimit(
    //                     getContext().getResources().getDisplayMetrics().widthPixels, true);
    //             // No limit to the item count; use whatever will fit.
    //             mActionMenuPresenter.setItemLimit(Integer.MAX_VALUE);
    //         }
    //     }
    //     super.setSplitToolbar(splitActionBar);
    // }

    assert(0);
    if (mSplitActionBar != splitActionBar) {
        if (mMenuView != NULL) {
            AutoPtr<IViewParent> viewParent;
            IView::Probe(mMenuView)->GetParent((IViewParent**)&viewParent);
            IViewGroup* oldParent = IViewGroup::Probe(viewParent);
            if (oldParent != NULL) {
                //car has no this func: oldParent->RemoveView(IView::Probe(mMenuView));
            }

            AutoPtr<IViewGroupLayoutParams> menuViewLayoutParams;
            IView::Probe(mMenuView)->GetLayoutParams((IViewGroupLayoutParams**)&menuViewLayoutParams);
            if (splitActionBar) {
                if (mSplitView != NULL) {
                    mSplitView->AddView(IView::Probe(mMenuView));
                }
                menuViewLayoutParams->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
            }
            else {
                AddView(IView::Probe(mMenuView));
                menuViewLayoutParams->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
            }
            IView::Probe(mMenuView)->RequestLayout();
        }
        if (mSplitView != NULL) {
            IView::Probe(mSplitView)->SetVisibility(splitActionBar ? VISIBLE : IView::GONE);
        }

        if (mActionMenuPresenter != NULL) {
            if (!splitActionBar) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                Boolean resTmp = FALSE;
                resources->GetBoolean(R::bool_::action_bar_expanded_action_views_exclusive, &resTmp);
                mActionMenuPresenter->SetExpandedActionViewsExclusive(resTmp);
            }
            else {
                mActionMenuPresenter->SetExpandedActionViewsExclusive(FALSE);

                AutoPtr<IContext> context;
                IView::Probe(this)->GetContext((IContext**)&context);
                AutoPtr<IResources> resources;
                context->GetResources((IResources**)&resources);
                AutoPtr<IDisplayMetrics> displayMetrics;
                resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
                Int32 widthPixel = 0;
                displayMetrics->GetWidthPixels(&widthPixel);
                mActionMenuPresenter->SetWidthLimit(widthPixel, TRUE);
                mActionMenuPresenter->SetItemLimit(Elastos::Core::Math::INT32_MAX_VALUE);
            }
        }
        AbsActionBarView::SetSplitToolbar(splitActionBar);
    }
    return NOERROR;
}

ECode ActionBarView::IsSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSplitActionBar;

    *result = mSplitActionBar;
    return NOERROR;
}

ECode ActionBarView::CanSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return true;

    *result = TRUE;
    return NOERROR;
}

ECode ActionBarView::HasEmbeddedTabs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIncludeTabs;

    *result = mIncludeTabs;
    return NOERROR;
}

ECode ActionBarView::SetEmbeddedTabView(
    /* [in] */ IScrollingTabContainerView* tabs)
{
    VALIDATE_NOT_NULL(tabs);
    // ==================before translated======================
    // if (mTabScrollView != null) {
    //     removeView(mTabScrollView);
    // }
    // mTabScrollView = tabs;
    // mIncludeTabs = tabs != null;
    // if (mIncludeTabs && mNavigationMode == ActionBar.NAVIGATION_MODE_TABS) {
    //     addView(mTabScrollView);
    //     ViewGroup.LayoutParams lp = mTabScrollView.getLayoutParams();
    //     lp.width = LayoutParams.WRAP_CONTENT;
    //     lp.height = LayoutParams.MATCH_PARENT;
    //     tabs.setAllowCollapse(true);
    // }

    if (mTabScrollView) {
        RemoveView(IView::Probe(mTabScrollView));
    }
    mTabScrollView = tabs;
    mIncludeTabs = tabs != NULL;
    if (mIncludeTabs && mNavigationMode == IActionBar::NAVIGATION_MODE_TABS) {
        AddView(IView::Probe(mTabScrollView));
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(mTabScrollView)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        lp->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
        lp->SetHeight(IViewGroupLayoutParams::MATCH_PARENT);
        tabs->SetAllowCollapse(TRUE);
    }
    return NOERROR;
}

ECode ActionBarView::SetMenuPrepared()
{
    // ==================before translated======================
    // mMenuPrepared = true;

    mMenuPrepared = TRUE;
    return NOERROR;
}

ECode ActionBarView::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
    VALIDATE_NOT_NULL(menu);
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // if (menu == mOptionsMenu) return;
    //
    // if (mOptionsMenu != null) {
    //     mOptionsMenu.removeMenuPresenter(mActionMenuPresenter);
    //     mOptionsMenu.removeMenuPresenter(mExpandedMenuPresenter);
    // }
    //
    // MenuBuilder builder = (MenuBuilder) menu;
    // mOptionsMenu = builder;
    // if (mMenuView != null) {
    //     final ViewGroup oldParent = (ViewGroup) mMenuView.getParent();
    //     if (oldParent != null) {
    //         oldParent.removeView(mMenuView);
    //     }
    // }
    // if (mActionMenuPresenter == null) {
    //     mActionMenuPresenter = new ActionMenuPresenter(mContext);
    //     mActionMenuPresenter.setCallback(cb);
    //     mActionMenuPresenter.setId(com.android.internal.R.id.action_menu_presenter);
    //     mExpandedMenuPresenter = new ExpandedActionViewMenuPresenter();
    // }
    //
    // ActionMenuView menuView;
    // final LayoutParams layoutParams = new LayoutParams(LayoutParams.WRAP_CONTENT,
    //         LayoutParams.MATCH_PARENT);
    // if (!mSplitActionBar) {
    //     mActionMenuPresenter.setExpandedActionViewsExclusive(
    //             getResources().getBoolean(
    //             com.android.internal.R.bool.action_bar_expanded_action_views_exclusive));
    //     configPresenters(builder);
    //     menuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //     final ViewGroup oldParent = (ViewGroup) menuView.getParent();
    //     if (oldParent != null && oldParent != this) {
    //         oldParent.removeView(menuView);
    //     }
    //     addView(menuView, layoutParams);
    // } else {
    //     mActionMenuPresenter.setExpandedActionViewsExclusive(false);
    //     // Allow full screen width in split mode.
    //     mActionMenuPresenter.setWidthLimit(
    //             getContext().getResources().getDisplayMetrics().widthPixels, true);
    //     // No limit to the item count; use whatever will fit.
    //     mActionMenuPresenter.setItemLimit(Integer.MAX_VALUE);
    //     // Span the whole width
    //     layoutParams.width = LayoutParams.MATCH_PARENT;
    //     layoutParams.height = LayoutParams.WRAP_CONTENT;
    //     configPresenters(builder);
    //     menuView = (ActionMenuView) mActionMenuPresenter.getMenuView(this);
    //     if (mSplitView != null) {
    //         final ViewGroup oldParent = (ViewGroup) menuView.getParent();
    //         if (oldParent != null && oldParent != mSplitView) {
    //             oldParent.removeView(menuView);
    //         }
    //         menuView.setVisibility(getAnimatedVisibility());
    //         mSplitView.addView(menuView, layoutParams);
    //     } else {
    //         // We'll add this later if we missed it this time.
    //         menuView.setLayoutParams(layoutParams);
    //     }
    // }
    // mMenuView = menuView;

    assert(0);
    if (TO_IINTERFACE(menu) == TO_IINTERFACE(mOptionsMenu))
        return NOERROR;

    if (mOptionsMenu) {
        mOptionsMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mActionMenuPresenter));
        mOptionsMenu->RemoveMenuPresenter(mExpandedMenuPresenter);
    }

    IMenuBuilder* builder = IMenuBuilder::Probe(menu);
    mOptionsMenu = builder;
    if (mMenuView) {
        AutoPtr<IViewParent> p;
        IView::Probe(mMenuView)->GetParent((IViewParent**)&p);
        IViewGroup* oldParent = IViewGroup::Probe(p);
        if (oldParent) {
            IViewManager* mg = IViewManager::Probe(oldParent);
            mg->RemoveView(IView::Probe(mMenuView));
        }
    }
    if (mActionMenuPresenter == NULL) {
        CActionMenuPresenter::New(mContext, (IActionMenuPresenter**)&mActionMenuPresenter);
        //IBaseMenuPresenter::Probe(mActionMenuPresenter)->SetCallback(cb);
        IView::Probe(mActionMenuPresenter)->SetId(R::id::action_menu_presenter);
        mExpandedMenuPresenter = new ExpandedActionViewMenuPresenter(this);
    }

    AutoPtr<IActionMenuView> menuView;
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
           IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&layoutParams);

    if (!mSplitActionBar) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Boolean b = FALSE;
        resources->GetBoolean(R::bool_::action_bar_expanded_action_views_exclusive, &b);
        mActionMenuPresenter->SetExpandedActionViewsExclusive(b);
        ConfigPresenters(builder);
        AutoPtr<IMenuView> v;
        IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&v);
        menuView = IActionMenuView::Probe(v);
        AutoPtr<IViewParent> p;
        IView::Probe(menuView)->GetParent((IViewParent**)&p);
        IViewGroup* oldParent = IViewGroup::Probe(p);
        if (oldParent && TO_IINTERFACE(oldParent) != TO_IINTERFACE(this)) {
            IViewManager* mg = IViewManager::Probe(oldParent);
            mg->RemoveView(IView::Probe(menuView));
        }
        AddView(IView::Probe(menuView), layoutParams);
    }
    else {
        mActionMenuPresenter->SetExpandedActionViewsExclusive(FALSE);
        // Allow full screen width in split mode.
        AutoPtr<IContext> context;
        IView::Probe(this)->GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IDisplayMetrics> metrics;
        resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        Int32 pixels = 0;
        metrics->GetWidthPixels(&pixels);
        mActionMenuPresenter->SetWidthLimit(pixels, TRUE);
        mActionMenuPresenter->SetItemLimit(Elastos::Core::Math::INT32_MAX_VALUE);
        layoutParams->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
        layoutParams->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
        ConfigPresenters(builder);
        AutoPtr<IMenuView> v;
        IMenuPresenter::Probe(mActionMenuPresenter)->GetMenuView(IViewGroup::Probe(this), (IMenuView**)&v);
        menuView = IActionMenuView::Probe(v);
        if (mSplitView) {
            AutoPtr<IViewParent> p;
            IView::Probe(menuView)->GetParent((IViewParent**)&p);
            IViewGroup* oldParent = IViewGroup::Probe(p);
            if (oldParent && oldParent != mSplitView) {
                IViewManager* mg = IViewManager::Probe(oldParent);
                mg->RemoveView(IView::Probe(menuView));
            }

            Int32 animatedVisibility = 0;
            GetAnimatedVisibility(&animatedVisibility);
            IView::Probe(menuView)->SetVisibility(animatedVisibility);
            mSplitView->AddView(IView::Probe(menuView), layoutParams);
        }
        else {
            // We'll add this later if we missed it this time.
            IView::Probe(menuView)->SetLayoutParams(layoutParams);
        }
    }
    mMenuView = menuView;
    return NOERROR;
}

ECode ActionBarView::HasExpandedActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mExpandedMenuPresenter != null &&
    //         mExpandedMenuPresenter.mCurrentExpandedItem != null;

    *result = mExpandedMenuPresenter != NULL && mExpandedMenuPresenter->mCurrentExpandedItem != NULL;
    return NOERROR;
}

ECode ActionBarView::CollapseActionView()
{
    // ==================before translated======================
    // final MenuItemImpl item = mExpandedMenuPresenter == null ? null :
    //         mExpandedMenuPresenter.mCurrentExpandedItem;
    // if (item != null) {
    //     item.collapseActionView();
    // }

    AutoPtr<IMenuItemImpl> item = mExpandedMenuPresenter == NULL ? NULL : mExpandedMenuPresenter->mCurrentExpandedItem;
    if (item) {
        Boolean res = FALSE;
        IMenuItem::Probe(item)->CollapseActionView(&res);
    }
    return NOERROR;
}

ECode ActionBarView::SetCustomView(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // final boolean showCustom = (mDisplayOptions & ActionBar.DISPLAY_SHOW_CUSTOM) != 0;
    // if (showCustom) {
    //     ActionBarTransition.beginDelayedTransition(this);
    // }
    // if (mCustomNavView != null && showCustom) {
    //     removeView(mCustomNavView);
    // }
    // mCustomNavView = view;
    // if (mCustomNavView != null && showCustom) {
    //     addView(mCustomNavView);
    // }

    Boolean showCustom = (mDisplayOptions & IActionBar::DISPLAY_SHOW_CUSTOM) != 0;
    if (showCustom) {
        ActionBarTransition::BeginDelayedTransition(this);
    }
    if (mCustomNavView != NULL && showCustom) {
        RemoveView(mCustomNavView);
    }
    mCustomNavView = view;
    if (mCustomNavView != NULL && showCustom) {
        AddView(mCustomNavView);
    }
    return NOERROR;
}

ECode ActionBarView::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTitle;

    *result = mTitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // mUserTitle = true;
    // setTitleImpl(title);

    mUserTitle = TRUE;
    SetTitleImpl(title);
    return NOERROR;
}

ECode ActionBarView::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // if (!mUserTitle) {
    //     setTitleImpl(title);
    // }

    if (!mUserTitle) {
        SetTitleImpl(title);
    }
    return NOERROR;
}

ECode ActionBarView::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSubtitle;

    *result = mSubtitle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    VALIDATE_NOT_NULL(subtitle);
    // ==================before translated======================
    // ActionBarTransition.beginDelayedTransition(this);
    // mSubtitle = subtitle;
    // if (mSubtitleView != null) {
    //     mSubtitleView.setText(subtitle);
    //     mSubtitleView.setVisibility(subtitle != null ? VISIBLE : GONE);
    //     final boolean visible = mExpandedActionView == null &&
    //             (mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0 &&
    //             (!TextUtils.isEmpty(mTitle) || !TextUtils.isEmpty(mSubtitle));
    //     mTitleLayout.setVisibility(visible ? VISIBLE : GONE);
    // }
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    ActionBarTransition::BeginDelayedTransition(this);
    mSubtitle = subtitle;
    if (mSubtitleView != NULL) {
        mSubtitleView->SetText(subtitle);
        IView::Probe(mSubtitleView)->SetVisibility(subtitle != NULL ? IView::VISIBLE : IView::GONE);
        Boolean visible = mExpandedActionView == NULL && (mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0 &&
            (!TextUtils::IsEmpty(mTitle) || !TextUtils::IsEmpty(mSubtitle));
        IView::Probe(mTitleLayout)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    }
    Boolean isEnabled = FALSE;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnabled);
    UpdateHomeAccessibility(isEnabled);
    return NOERROR;
}

ECode ActionBarView::SetHomeButtonEnabled(
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // setHomeButtonEnabled(enable, true);

    SetHomeButtonEnabled(enable, TRUE);
    return NOERROR;
}

ECode ActionBarView::SetDisplayOptions(
    /* [in] */ Int32 options)
{
    // ==================before translated======================
    // final int flagsChanged = mDisplayOptions == -1 ? -1 : options ^ mDisplayOptions;
    // mDisplayOptions = options;
    //
    // if ((flagsChanged & DISPLAY_RELAYOUT_MASK) != 0) {
    //     ActionBarTransition.beginDelayedTransition(this);
    //
    //     if ((flagsChanged & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //         final boolean setUp = (options & ActionBar.DISPLAY_HOME_AS_UP) != 0;
    //         mHomeLayout.setShowUp(setUp);
    //
    //         // Showing home as up implicitly enables interaction with it.
    //         // In honeycomb it was always enabled, so make this transition
    //         // a bit easier for developers in the common case.
    //         // (It would be silly to show it as up without responding to it.)
    //         if (setUp) {
    //             setHomeButtonEnabled(true);
    //         }
    //     }
    //
    //     if ((flagsChanged & ActionBar.DISPLAY_USE_LOGO) != 0) {
    //         final boolean logoVis = mLogo != null && (options & ActionBar.DISPLAY_USE_LOGO) != 0;
    //         mHomeLayout.setIcon(logoVis ? mLogo : mIcon);
    //     }
    //
    //     if ((flagsChanged & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //         if ((options & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //             initTitle();
    //         } else {
    //             mUpGoerFive.removeView(mTitleLayout);
    //         }
    //     }
    //
    //     final boolean showHome = (options & ActionBar.DISPLAY_SHOW_HOME) != 0;
    //     final boolean homeAsUp = (mDisplayOptions & ActionBar.DISPLAY_HOME_AS_UP) != 0;
    //     final boolean titleUp = !showHome && homeAsUp;
    //     mHomeLayout.setShowIcon(showHome);
    //
    //     final int homeVis = (showHome || titleUp) && mExpandedActionView == null ?
    //             VISIBLE : GONE;
    //     mHomeLayout.setVisibility(homeVis);
    //
    //     if ((flagsChanged & ActionBar.DISPLAY_SHOW_CUSTOM) != 0 && mCustomNavView != null) {
    //         if ((options & ActionBar.DISPLAY_SHOW_CUSTOM) != 0) {
    //             addView(mCustomNavView);
    //         } else {
    //             removeView(mCustomNavView);
    //         }
    //     }
    //
    //     if (mTitleLayout != null &&
    //             (flagsChanged & ActionBar.DISPLAY_TITLE_MULTIPLE_LINES) != 0) {
    //         if ((options & ActionBar.DISPLAY_TITLE_MULTIPLE_LINES) != 0) {
    //             mTitleView.setSingleLine(false);
    //             mTitleView.setMaxLines(2);
    //         } else {
    //             mTitleView.setMaxLines(1);
    //             mTitleView.setSingleLine(true);
    //         }
    //     }
    //
    //     requestLayout();
    // } else {
    //     invalidate();
    // }
    //
    // // Make sure the home button has an accurate content description for accessibility.
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    Int32 flagsChanged = mDisplayOptions == -1 ? -1 : options ^ mDisplayOptions;
    mDisplayOptions = options;

    if ((flagsChanged & DISPLAY_RELAYOUT_MASK) != 0) {
        ActionBarTransition::BeginDelayedTransition(this);

        if ((flagsChanged & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
            Boolean setUp = (options & IActionBar::DISPLAY_HOME_AS_UP) != 0;
            mHomeLayout->SetShowUp(setUp);
            if (setUp) {
                SetHomeButtonEnabled(TRUE);
            }
        }

        if ((flagsChanged & IActionBar::DISPLAY_USE_LOGO) != 0) {
            Boolean logoVis = mLogo != NULL && (options & IActionBar::DISPLAY_USE_LOGO) != 0;
            mHomeLayout->SetIcon(logoVis ? mLogo : mIcon);
        }

        if ((flagsChanged & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
            if ((options & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
                InitTitle();
            }
            else {
                AutoPtr<IViewManager> vm = IViewManager::Probe(mUpGoerFive);
                vm->RemoveView(IView::Probe(mTitleLayout));
            }
        }

        Boolean showHome = (options & IActionBar::DISPLAY_SHOW_HOME) != 0;
        Boolean homeAsUp = (mDisplayOptions & IActionBar::DISPLAY_HOME_AS_UP) != 0;
        Boolean titleUp = !showHome && homeAsUp;
        mHomeLayout->SetShowIcon(showHome);
        Int32 homeVis = (showHome || titleUp) && mExpandedActionView == NULL ? VISIBLE : IView::GONE;
        IView::Probe(mHomeLayout)->SetVisibility(homeVis);

        if ((flagsChanged & IActionBar::DISPLAY_SHOW_CUSTOM) != 0 && mCustomNavView) {
            if ((options & IActionBar::DISPLAY_SHOW_CUSTOM) != 0) {
                AddView(mCustomNavView);
            }
            else {
                RemoveView(mCustomNavView);
            }
        }

        if (mTitleLayout && (flagsChanged & IActionBar::DISPLAY_TITLE_MULTIPLE_LINES) != 0) {
            if ((options & IActionBar::DISPLAY_TITLE_MULTIPLE_LINES) != 0) {
                assert(0 && "TEXTVIEW NEED ADD INTERFACE");
                //TitleView->SetSingleLine(FALSE);
                mTitleView->SetMaxLines(2);
            }
            else {
                mTitleView->SetMaxLines(1);
                //mTitleView->SetSingleLine(TRUE);
            }
        }

        RequestLayout();
    }
    else {
        Invalidate();
    }

    // Make sure the home button has an accurate content description for accessibility.
    Boolean isEnabled;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnabled);
    UpdateHomeAccessibility(isEnabled);
    return NOERROR;
}

ECode ActionBarView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    VALIDATE_NOT_NULL(icon);
    // ==================before translated======================
    // mIcon = icon;
    // if (icon != null &&
    //         ((mDisplayOptions & ActionBar.DISPLAY_USE_LOGO) == 0 || mLogo == null)) {
    //     mHomeLayout.setIcon(icon);
    // }
    // if (mExpandedActionView != null) {
    //     mExpandedHomeLayout.setIcon(mIcon.getConstantState().newDrawable(getResources()));
    // }

    mIcon = icon;
    if (icon && ((mDisplayOptions & IActionBar::DISPLAY_USE_LOGO) == 0 || mLogo == NULL)) {
        mHomeLayout->SetIcon(icon);
    }
    if (mExpandedActionView) {
        AutoPtr<IDrawableConstantState> state;
        mIcon->GetConstantState((IDrawableConstantState**)&state);

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<IDrawable> drawable;
        state->NewDrawable(resources, (IDrawable**)&drawable);
        mExpandedHomeLayout->SetIcon(drawable);
    }
    return NOERROR;
}

ECode ActionBarView::SetIcon(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setIcon(resId != 0 ? mContext.getDrawable(resId) : null);

    AutoPtr<IDrawable> drawable;
    mContext->GetDrawable(resId, (IDrawable**)&drawable);
    SetIcon(resId != 0 ? drawable : NULL);
    return NOERROR;
}

ECode ActionBarView::HasIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIcon != null;

    *result = mIcon != NULL;
    return NOERROR;
}

ECode ActionBarView::SetLogo(
    /* [in] */ IDrawable* logo)
{
    VALIDATE_NOT_NULL(logo);
    // ==================before translated======================
    // mLogo = logo;
    // if (logo != null && (mDisplayOptions & ActionBar.DISPLAY_USE_LOGO) != 0) {
    //     mHomeLayout.setIcon(logo);
    // }

    mLogo = logo;
    if (logo && (mDisplayOptions & IActionBar::DISPLAY_USE_LOGO) != 0) {
        mHomeLayout->SetIcon(logo);
    }
    return NOERROR;
}

ECode ActionBarView::SetLogo(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setLogo(resId != 0 ? mContext.getDrawable(resId) : null);

    AutoPtr<IDrawable> drawable;
    mContext->GetDrawable(resId, (IDrawable**)&drawable);
    SetLogo(resId != 0 ? drawable : NULL);
    return NOERROR;
}

ECode ActionBarView::HasLogo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLogo != null;

    *result = mLogo != NULL;
    return NOERROR;
}

ECode ActionBarView::SetNavigationMode(
    /* [in] */ Int32 mode)
{
    // ==================before translated======================
    // final int oldMode = mNavigationMode;
    // if (mode != oldMode) {
    //     ActionBarTransition.beginDelayedTransition(this);
    //     switch (oldMode) {
    //     case ActionBar.NAVIGATION_MODE_LIST:
    //         if (mListNavLayout != null) {
    //             removeView(mListNavLayout);
    //         }
    //         break;
    //     case ActionBar.NAVIGATION_MODE_TABS:
    //         if (mTabScrollView != null && mIncludeTabs) {
    //             removeView(mTabScrollView);
    //         }
    //     }
    //
    //     switch (mode) {
    //     case ActionBar.NAVIGATION_MODE_LIST:
    //         if (mSpinner == null) {
    //             mSpinner = new Spinner(mContext, null,
    //                     com.android.internal.R.attr.actionDropDownStyle);
    //             mSpinner.setId(com.android.internal.R.id.action_bar_spinner);
    //             mListNavLayout = new LinearLayout(mContext, null,
    //                     com.android.internal.R.attr.actionBarTabBarStyle);
    //             LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
    //                     LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT);
    //             params.gravity = Gravity.CENTER;
    //             mListNavLayout.addView(mSpinner, params);
    //         }
    //         if (mSpinner.getAdapter() != mSpinnerAdapter) {
    //             mSpinner.setAdapter(mSpinnerAdapter);
    //         }
    //         mSpinner.setOnItemSelectedListener(mNavItemSelectedListener);
    //         addView(mListNavLayout);
    //         break;
    //     case ActionBar.NAVIGATION_MODE_TABS:
    //         if (mTabScrollView != null && mIncludeTabs) {
    //             addView(mTabScrollView);
    //         }
    //         break;
    //     }
    //     mNavigationMode = mode;
    //     requestLayout();
    // }

    Int32 oldMode = mNavigationMode;
    if (mode != oldMode) {
        ActionBarTransition::BeginDelayedTransition(this);
        switch (oldMode) {
            case IActionBar::NAVIGATION_MODE_LIST:
                if (mListNavLayout) {
                    RemoveView(IView::Probe(mListNavLayout));
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                if (mTabScrollView && mIncludeTabs) {
                    RemoveView(IView::Probe(mTabScrollView));
                }
                break;
        }

        switch (mode) {
            case IActionBar::NAVIGATION_MODE_LIST:
                {
                    if (!mSpinner) {
                        CSpinner::New(mContext, NULL, R::attr::actionDropDownStyle, (ISpinner**)&mSpinner);
                        IView::Probe(mSpinner)->SetId(R::id::action_bar_spinner);
                        CLinearLayout::New(mContext, NULL, R::attr::actionBarTabBarStyle, (ILinearLayout**)&mListNavLayout);
                        AutoPtr<ILinearLayoutLayoutParams> params;
                        CLinearLayoutLayoutParams::New(
                                IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT, (ILinearLayoutLayoutParams**)&params);
                        params->SetGravity(IGravity::CENTER);
                        IViewGroup::Probe(mListNavLayout)->AddView(IView::Probe(mSpinner), IViewGroupLayoutParams::Probe(params));
                    }

                    AutoPtr<ISpinnerAdapter> spinnerAdapter;
                    IAdapterView::Probe(mSpinner)->GetAdapter((IAdapter**)&spinnerAdapter);
                    if (spinnerAdapter != mSpinnerAdapter) {
                        IAdapterView::Probe(mSpinner)->SetAdapter(IAdapter::Probe(mSpinnerAdapter));
                    }
                    IAdapterView::Probe(mSpinner)->SetOnItemSelectedListener(mNavItemSelectedListener);
                    AddView(IView::Probe(mListNavLayout));
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                if (mTabScrollView && mIncludeTabs) {
                    AddView(IView::Probe(mTabScrollView));
                }
                break;
        }
        mNavigationMode = mode;
        RequestLayout();
    }
    return NOERROR;
}

ECode ActionBarView::SetDropdownParams(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IAdapterViewOnItemSelectedListener* l)
{
    VALIDATE_NOT_NULL(adapter);
    VALIDATE_NOT_NULL(l);
    // ==================before translated======================
    // mSpinnerAdapter = adapter;
    // mNavItemSelectedListener = l;
    // if (mSpinner != null) {
    //     mSpinner.setAdapter(adapter);
    //     mSpinner.setOnItemSelectedListener(l);
    // }

    mSpinnerAdapter = adapter;
    mNavItemSelectedListener = l;
    if (mSpinner != NULL) {
        IAdapterView::Probe(mSpinner)->SetAdapter(IAdapter::Probe(adapter));
        IAdapterView::Probe(mSpinner)->SetOnItemSelectedListener(l);
    }
    return NOERROR;
}

ECode ActionBarView::GetDropdownItemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSpinnerAdapter != null ? mSpinnerAdapter.getCount() : 0;

    Int32 count = 0;
    IAdapterView::Probe(mSpinnerAdapter)->GetCount(&count);
    *result = mSpinnerAdapter != NULL ? count : 0;
    return NOERROR;
}

ECode ActionBarView::SetDropdownSelectedPosition(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // mSpinner.setSelection(position);

    IAdapterView::Probe(mSpinner)->SetSelection(position);
    return NOERROR;
}

ECode ActionBarView::GetDropdownSelectedPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSpinner.getSelectedItemPosition();

    IAdapterView::Probe(mSpinner)->GetSelectedItemPosition(result);
    return NOERROR;
}

ECode ActionBarView::GetCustomView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCustomNavView;

    *result = mCustomNavView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::GetNavigationMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNavigationMode;

    *result = mNavigationMode;
    return NOERROR;
}

ECode ActionBarView::GetDisplayOptions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDisplayOptions;

    *result = mDisplayOptions;
    return NOERROR;
}

ECode ActionBarView::GetViewGroup(
    /* [out] */ IViewGroup** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return this;

    *result = IViewGroup::Probe(this);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::SetContextView(
    /* [in] */ IActionBarContextView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // mContextView = view;

    mContextView = view;
    return NOERROR;
}

ECode ActionBarView::SetCollapsible(
    /* [in] */ Boolean collapsible)
{
    // ==================before translated======================
    // mIsCollapsible = collapsible;

    mIsCollapsible = collapsible;
    return NOERROR;
}

ECode ActionBarView::IsTitleTruncated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mTitleView == null) {
    //     return false;
    // }
    //
    // final Layout titleLayout = mTitleView.getLayout();
    // if (titleLayout == null) {
    //     return false;
    // }
    //
    // final int lineCount = titleLayout.getLineCount();
    // for (int i = 0; i < lineCount; i++) {
    //     if (titleLayout.getEllipsisCount(i) > 0) {
    //         return true;
    //     }
    // }
    // return false;

    if (!mTitleView) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ILayout> titleLayout;
    mTitleView->GetLayout((ILayout**)&titleLayout);
    if (!titleLayout) {
        return FALSE;
    }

    Int32 lineCount = 0;
    titleLayout->GetLineCount(&lineCount);
    for (int i = 0; i < lineCount; i++) {
        Int32 count = 0;
        titleLayout->GetEllipsisCount(i, &count);
        if (count > 0) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(attrs);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new ActionBar.LayoutParams(getContext(), attrs);

    AutoPtr<IContext> context;
    IView::Probe(this)->GetContext((IContext**)&context);
    AutoPtr<IActionBarLayoutParams> lp;
    CActionBarLayoutParams::New(context, attrs, (IActionBarLayoutParams**)&lp);
    *result = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* lp,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(lp);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (lp == null) {
    //     lp = generateDefaultLayoutParams();
    // }
    // return lp;

    *result = lp;
    if (lp == NULL) {
        GenerateDefaultLayoutParams(result);
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::OnSaveInstanceState(
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Parcelable superState = super.onSaveInstanceState();
    // SavedState state = new SavedState(superState);
    //
    // if (mExpandedMenuPresenter != null && mExpandedMenuPresenter.mCurrentExpandedItem != null) {
    //     state.expandedMenuItemId = mExpandedMenuPresenter.mCurrentExpandedItem.getItemId();
    // }
    //
    // state.isOverflowOpen = isOverflowMenuShowing();
    //
    // return state;

    assert(0);
    AutoPtr<IParcelable> superState = View::OnSaveInstanceState();
    AutoPtr<SavedState> state = new SavedState(superState);
    if (mExpandedMenuPresenter != NULL && mExpandedMenuPresenter->mCurrentExpandedItem != NULL) {
        Int32 itemId = 0;
        IMenuItem::Probe(mExpandedMenuPresenter->mCurrentExpandedItem)->GetItemId(&itemId);
        state->expandedMenuItemId = itemId;
    }
    IAbsActionBarView::Probe(this)->IsOverflowMenuShowing(&state->isOverflowOpen);
    *result = state;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void ActionBarView::OnRestoreInstanceState(
    /* [in] */ IParcelable* p)
{
    // ==================before translated======================
    // SavedState state = (SavedState) p;
    //
    // super.onRestoreInstanceState(state.getSuperState());
    //
    // if (state.expandedMenuItemId != 0 &&
    //         mExpandedMenuPresenter != null && mOptionsMenu != null) {
    //     final MenuItem item = mOptionsMenu.findItem(state.expandedMenuItemId);
    //     if (item != null) {
    //         item.expandActionView();
    //     }
    // }
    //
    // if (state.isOverflowOpen) {
    //     postShowOverflowMenu();
    // }

    assert(0);
    SavedState* state = (SavedState*)p;
    View::OnRestoreInstanceState(state->GetSuperState());
    if (state->expandedMenuItemId != 0 && mExpandedMenuPresenter != NULL && mOptionsMenu != NULL) {
        AutoPtr<IMenuItem> item;
        IMenu::Probe(mOptionsMenu)->FindItem(state->expandedMenuItemId, (IMenuItem**)&item);
        if (item != NULL) {
            Boolean resTmp;
            item->ExpandActionView(&resTmp);
        }
    }
    if (state->isOverflowOpen) {
        PostShowOverflowMenu();
    }
}

ECode ActionBarView::SetNavigationIcon(
    /* [in] */ IDrawable* indicator)
{
    VALIDATE_NOT_NULL(indicator);
    // ==================before translated======================
    // mHomeLayout.setUpIndicator(indicator);

    mHomeLayout->SetUpIndicator(indicator);
    return NOERROR;
}

ECode ActionBarView::SetDefaultNavigationIcon(
    /* [in] */ IDrawable* icon)
{
    VALIDATE_NOT_NULL(icon);
    // ==================before translated======================
    // mHomeLayout.setDefaultUpIndicator(icon);

    mHomeLayout->SetDefaultUpIndicator(icon);
    return NOERROR;
}

ECode ActionBarView::SetNavigationIcon(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // mHomeLayout.setUpIndicator(resId);

    mHomeLayout->SetUpIndicator(resId);
    return NOERROR;
}

ECode ActionBarView::SetNavigationContentDescription(
    /* [in] */ ICharSequence* description)
{
    VALIDATE_NOT_NULL(description);
    // ==================before translated======================
    // mHomeDescription = description;
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    mHomeDescription = description;
    Boolean isEnabled = FALSE;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnabled);
    UpdateHomeAccessibility(isEnabled);
    return NOERROR;
}

ECode ActionBarView::SetNavigationContentDescription(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // mHomeDescriptionRes = resId;
    // mHomeDescription = resId != 0 ? getResources().getText(resId) : null;
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    mHomeDescriptionRes = resId;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);

    AutoPtr<ICharSequence> text;
    resources->GetText(resId, (ICharSequence**)&text);
    mHomeDescription = resId != 0 ? text : NULL;

    Boolean isEnabled = FALSE;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnabled);
    UpdateHomeAccessibility(isEnabled);
    return NOERROR;
}

ECode ActionBarView::SetDefaultNavigationContentDescription(
    /* [in] */ Int32 defaultNavigationContentDescription)
{
    // ==================before translated======================
    // if (mDefaultUpDescription == defaultNavigationContentDescription) {
    //     return;
    // }
    // mDefaultUpDescription = defaultNavigationContentDescription;
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    if (mDefaultUpDescription == defaultNavigationContentDescription) {
        return NOERROR;
    }
    mDefaultUpDescription = defaultNavigationContentDescription;
    Boolean isEnabled = FALSE;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnabled);
    UpdateHomeAccessibility(isEnabled);
    return NOERROR;
}

void ActionBarView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    //
    // mTitleView = null;
    // mSubtitleView = null;
    // if (mTitleLayout != null && mTitleLayout.getParent() == mUpGoerFive) {
    //     mUpGoerFive.removeView(mTitleLayout);
    // }
    // mTitleLayout = null;
    // if ((mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //     initTitle();
    // }
    //
    // if (mHomeDescriptionRes != 0) {
    //     setNavigationContentDescription(mHomeDescriptionRes);
    // }
    //
    // if (mTabScrollView != null && mIncludeTabs) {
    //     ViewGroup.LayoutParams lp = mTabScrollView.getLayoutParams();
    //     if (lp != null) {
    //         lp.width = LayoutParams.WRAP_CONTENT;
    //         lp.height = LayoutParams.MATCH_PARENT;
    //     }
    //     mTabScrollView.setAllowCollapse(true);
    // }

    assert(0);
    AbsActionBarView::OnConfigurationChanged(newConfig);
    mTitleView = NULL;
    mSubtitleView = NULL;

    if (mTitleLayout) {
        AutoPtr<IViewParent> p;
        IView::Probe(mTitleLayout)->GetParent((IViewParent**)&p);
        if(TO_IINTERFACE(p) == TO_IINTERFACE(mUpGoerFive)) {
            AutoPtr<IViewManager> vm = IViewManager::Probe(mUpGoerFive);
            vm->RemoveView(IView::Probe(mTitleLayout));
        }
    }

    mTitleLayout = NULL;
    if ((mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        InitTitle();
    }

    if (mHomeDescriptionRes != 0) {
        SetNavigationContentDescription(mHomeDescriptionRes);
    }

    if (mTabScrollView && mIncludeTabs) {
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(mTabScrollView)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        if (lp != NULL) {
            lp->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
            lp->SetHeight(IViewGroupLayoutParams::MATCH_PARENT);
        }
        mTabScrollView->SetAllowCollapse(TRUE);
    }
}

ECode ActionBarView::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    // ==================before translated======================
    // // Used by custom nav views if they don't supply layout params. Everything else
    // // added to an ActionBarView should have them already.
    // return new ActionBar.LayoutParams(DEFAULT_CUSTOM_GRAVITY);

    AutoPtr<IActionBarLayoutParams> ap;
    CActionBarLayoutParams::New(DEFAULT_CUSTOM_GRAVITY, (IActionBarLayoutParams**)&ap);
    *result = IViewGroupLayoutParams::Probe(ap);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarView::OnFinishInflate()
{
    // ==================before translated======================
    // super.onFinishInflate();
    //
    // mUpGoerFive.addView(mHomeLayout, 0);
    // addView(mUpGoerFive);
    //
    // if (mCustomNavView != null && (mDisplayOptions & ActionBar.DISPLAY_SHOW_CUSTOM) != 0) {
    //     final ViewParent parent = mCustomNavView.getParent();
    //     if (parent != this) {
    //         if (parent instanceof ViewGroup) {
    //             ((ViewGroup) parent).removeView(mCustomNavView);
    //         }
    //         addView(mCustomNavView);
    //     }
    // }

    AbsActionBarView::OnFinishInflate();
    mUpGoerFive->AddView(IView::Probe(mHomeLayout), 0);
    AddView(IView::Probe(mUpGoerFive));

    if (mCustomNavView && (mDisplayOptions & IActionBar::DISPLAY_SHOW_CUSTOM) != 0) {
        AutoPtr<IViewParent> parent;
        IView::Probe(mCustomNavView)->GetParent((IViewParent**)&parent);

        if (TO_IINTERFACE(parent) != TO_IINTERFACE(this)) {
            if (IViewGroup::Probe(parent)) {
                AutoPtr<IViewManager> vg = IViewManager::Probe(parent);
                vg->RemoveView(IView::Probe(mCustomNavView));
            }
            AddView(IView::Probe(mCustomNavView));
        }
    }
    return NOERROR;
}

void ActionBarView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // final int childCount = getChildCount();
    // if (mIsCollapsible) {
    //     int visibleChildren = 0;
    //     for (int i = 0; i < childCount; i++) {
    //         final View child = getChildAt(i);
    //         if (child.getVisibility() != GONE &&
    //                 !(child == mMenuView && mMenuView.getChildCount() == 0) &&
    //                 child != mUpGoerFive) {
    //             visibleChildren++;
    //         }
    //     }
    //
    //     final int upChildCount = mUpGoerFive.getChildCount();
    //     for (int i = 0; i < upChildCount; i++) {
    //         final View child = mUpGoerFive.getChildAt(i);
    //         if (child.getVisibility() != GONE) {
    //             visibleChildren++;
    //         }
    //     }
    //
    //     if (visibleChildren == 0) {
    //         // No size for an empty action bar when collapsable.
    //         setMeasuredDimension(0, 0);
    //         return;
    //     }
    // }
    //
    // int widthMode = MeasureSpec.getMode(widthMeasureSpec);
    // if (widthMode != MeasureSpec.EXACTLY) {
    //     throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
    //             "with android:layout_width=\"match_parent\" (or fill_parent)");
    // }
    //
    // int heightMode = MeasureSpec.getMode(heightMeasureSpec);
    // if (heightMode != MeasureSpec.AT_MOST) {
    //     throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
    //             "with android:layout_height=\"wrap_content\"");
    // }
    //
    // int contentWidth = MeasureSpec.getSize(widthMeasureSpec);
    //
    // int maxHeight = mContentHeight >= 0 ?
    //         mContentHeight : MeasureSpec.getSize(heightMeasureSpec);
    //
    // final int verticalPadding = getPaddingTop() + getPaddingBottom();
    // final int paddingLeft = getPaddingLeft();
    // final int paddingRight = getPaddingRight();
    // final int height = maxHeight - verticalPadding;
    // final int childSpecHeight = MeasureSpec.makeMeasureSpec(height, MeasureSpec.AT_MOST);
    // final int exactHeightSpec = MeasureSpec.makeMeasureSpec(height, MeasureSpec.EXACTLY);
    //
    // int availableWidth = contentWidth - paddingLeft - paddingRight;
    // int leftOfCenter = availableWidth / 2;
    // int rightOfCenter = leftOfCenter;
    //
    // final boolean showTitle = mTitleLayout != null && mTitleLayout.getVisibility() != GONE &&
    //         (mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0;
    //
    // HomeView homeLayout = mExpandedActionView != null ? mExpandedHomeLayout : mHomeLayout;
    //
    // final ViewGroup.LayoutParams homeLp = homeLayout.getLayoutParams();
    // int homeWidthSpec;
    // if (homeLp.width < 0) {
    //     homeWidthSpec = MeasureSpec.makeMeasureSpec(availableWidth, MeasureSpec.AT_MOST);
    // } else {
    //     homeWidthSpec = MeasureSpec.makeMeasureSpec(homeLp.width, MeasureSpec.EXACTLY);
    // }
    //
    // /*
    //  * This is a little weird.
    //  * We're only measuring the *home* affordance within the Up container here
    //  * on purpose, because we want to give the available space to all other views before
    //  * the title text. We'll remeasure the whole up container again later.
    //  * We need to measure this container so we know the right offset for the up affordance
    //  * no matter what.
    //  */
    // homeLayout.measure(homeWidthSpec, exactHeightSpec);
    //
    // int homeWidth = 0;
    // if ((homeLayout.getVisibility() != GONE && homeLayout.getParent() == mUpGoerFive)
    //         || showTitle) {
    //     homeWidth = homeLayout.getMeasuredWidth();
    //     final int homeOffsetWidth = homeWidth + homeLayout.getStartOffset();
    //     availableWidth = Math.max(0, availableWidth - homeOffsetWidth);
    //     leftOfCenter = Math.max(0, availableWidth - homeOffsetWidth);
    // }
    //
    // if (mMenuView != null && mMenuView.getParent() == this) {
    //     availableWidth = measureChildView(mMenuView, availableWidth, exactHeightSpec, 0);
    //     rightOfCenter = Math.max(0, rightOfCenter - mMenuView.getMeasuredWidth());
    // }
    //
    // if (mIndeterminateProgressView != null &&
    //         mIndeterminateProgressView.getVisibility() != GONE) {
    //     availableWidth = measureChildView(mIndeterminateProgressView, availableWidth,
    //             childSpecHeight, 0);
    //     rightOfCenter = Math.max(0,
    //             rightOfCenter - mIndeterminateProgressView.getMeasuredWidth());
    // }
    //
    // if (mExpandedActionView == null) {
    //     switch (mNavigationMode) {
    //         case ActionBar.NAVIGATION_MODE_LIST:
    //             if (mListNavLayout != null) {
    //                 final int itemPaddingSize = showTitle ? mItemPadding * 2 : mItemPadding;
    //                 availableWidth = Math.max(0, availableWidth - itemPaddingSize);
    //                 leftOfCenter = Math.max(0, leftOfCenter - itemPaddingSize);
    //                 mListNavLayout.measure(
    //                         MeasureSpec.makeMeasureSpec(availableWidth, MeasureSpec.AT_MOST),
    //                         MeasureSpec.makeMeasureSpec(height, MeasureSpec.EXACTLY));
    //                 final int listNavWidth = mListNavLayout.getMeasuredWidth();
    //                 availableWidth = Math.max(0, availableWidth - listNavWidth);
    //                 leftOfCenter = Math.max(0, leftOfCenter - listNavWidth);
    //             }
    //             break;
    //         case ActionBar.NAVIGATION_MODE_TABS:
    //             if (mTabScrollView != null) {
    //                 final int itemPaddingSize = showTitle ? mItemPadding * 2 : mItemPadding;
    //                 availableWidth = Math.max(0, availableWidth - itemPaddingSize);
    //                 leftOfCenter = Math.max(0, leftOfCenter - itemPaddingSize);
    //                 mTabScrollView.measure(
    //                         MeasureSpec.makeMeasureSpec(availableWidth, MeasureSpec.AT_MOST),
    //                         MeasureSpec.makeMeasureSpec(height, MeasureSpec.EXACTLY));
    //                 final int tabWidth = mTabScrollView.getMeasuredWidth();
    //                 availableWidth = Math.max(0, availableWidth - tabWidth);
    //                 leftOfCenter = Math.max(0, leftOfCenter - tabWidth);
    //             }
    //             break;
    //     }
    // }
    //
    // View customView = null;
    // if (mExpandedActionView != null) {
    //     customView = mExpandedActionView;
    // } else if ((mDisplayOptions & ActionBar.DISPLAY_SHOW_CUSTOM) != 0 &&
    //         mCustomNavView != null) {
    //     customView = mCustomNavView;
    // }
    //
    // if (customView != null) {
    //     final ViewGroup.LayoutParams lp = generateLayoutParams(customView.getLayoutParams());
    //     final ActionBar.LayoutParams ablp = lp instanceof ActionBar.LayoutParams ?
    //             (ActionBar.LayoutParams) lp : null;
    //
    //     int horizontalMargin = 0;
    //     int verticalMargin = 0;
    //     if (ablp != null) {
    //         horizontalMargin = ablp.leftMargin + ablp.rightMargin;
    //         verticalMargin = ablp.topMargin + ablp.bottomMargin;
    //     }
    //
    //     // If the action bar is wrapping to its content height, don't allow a custom
    //     // view to MATCH_PARENT.
    //     int customNavHeightMode;
    //     if (mContentHeight <= 0) {
    //         customNavHeightMode = MeasureSpec.AT_MOST;
    //     } else {
    //         customNavHeightMode = lp.height != LayoutParams.WRAP_CONTENT ?
    //                 MeasureSpec.EXACTLY : MeasureSpec.AT_MOST;
    //     }
    //     final int customNavHeight = Math.max(0,
    //             (lp.height >= 0 ? Math.min(lp.height, height) : height) - verticalMargin);
    //
    //     final int customNavWidthMode = lp.width != LayoutParams.WRAP_CONTENT ?
    //             MeasureSpec.EXACTLY : MeasureSpec.AT_MOST;
    //     int customNavWidth = Math.max(0,
    //             (lp.width >= 0 ? Math.min(lp.width, availableWidth) : availableWidth)
    //             - horizontalMargin);
    //     final int hgrav = (ablp != null ? ablp.gravity : DEFAULT_CUSTOM_GRAVITY) &
    //             Gravity.HORIZONTAL_GRAVITY_MASK;
    //
    //     // Centering a custom view is treated specially; we try to center within the whole
    //     // action bar rather than in the available space.
    //     if (hgrav == Gravity.CENTER_HORIZONTAL && lp.width == LayoutParams.MATCH_PARENT) {
    //         customNavWidth = Math.min(leftOfCenter, rightOfCenter) * 2;
    //     }
    //
    //     customView.measure(
    //             MeasureSpec.makeMeasureSpec(customNavWidth, customNavWidthMode),
    //             MeasureSpec.makeMeasureSpec(customNavHeight, customNavHeightMode));
    //     availableWidth -= horizontalMargin + customView.getMeasuredWidth();
    // }
    //
    // /*
    //  * Measure the whole up container now, allowing for the full home+title sections.
    //  * (This will re-measure the home view.)
    //  */
    // availableWidth = measureChildView(mUpGoerFive, availableWidth + homeWidth,
    //         MeasureSpec.makeMeasureSpec(mContentHeight, MeasureSpec.EXACTLY), 0);
    // if (mTitleLayout != null) {
    //     leftOfCenter = Math.max(0, leftOfCenter - mTitleLayout.getMeasuredWidth());
    // }
    //
    // if (mContentHeight <= 0) {
    //     int measuredHeight = 0;
    //     for (int i = 0; i < childCount; i++) {
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
    //
    // if (mContextView != null) {
    //     mContextView.setContentHeight(getMeasuredHeight());
    // }
    //
    // if (mProgressView != null && mProgressView.getVisibility() != GONE) {
    //     mProgressView.measure(MeasureSpec.makeMeasureSpec(
    //             contentWidth - mProgressBarPadding * 2, MeasureSpec.EXACTLY),
    //             MeasureSpec.makeMeasureSpec(getMeasuredHeight(), MeasureSpec.AT_MOST));
    // }

    assert(0);
    Int32 childCount = 0;
    GetChildCount(&childCount);
    if (mIsCollapsible) {
        Int32 visibleChildren = 0;
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            Int32 visible = 0;
            child->GetVisibility(&visible);
            Int32 count = 0;
            IViewGroup::Probe(mMenuView)->GetChildCount(&count);
            if (visible != IView::GONE && !(TO_IINTERFACE(child) == TO_IINTERFACE(mMenuView) && count == 0) && TO_IINTERFACE(child) != TO_IINTERFACE(mUpGoerFive)) {
                ++visibleChildren;
            }
        }

        Int32 upChildCount = 0;
        mUpGoerFive->GetChildCount(&upChildCount);
        for (Int32 i = 0; i < upChildCount; ++i) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 visible = 0;
            child->GetVisibility(&visible);
            if (visible != IView::GONE) {
                ++visibleChildren;
            }
        }

        if (visibleChildren == 0) {
            SetMeasuredDimension(0, 0);
            return ;
        }
    }

    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    if (widthMode != MeasureSpec::EXACTLY) {
        //throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
        //        "with android:layout_width=\"match_parent\" (or fill_parent)");
        return ;
    }

    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    if (heightMode != View::MeasureSpec::AT_MOST) {
        //throw new IllegalStateException(getClass().getSimpleName() + " can only be used " +
        //       "with android:layout_height=\"wrap_content\"");
        return ;
    }

    Int32 contentWidth = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 maxHeight = mContentHeight >= 0 ? mContentHeight : View::MeasureSpec::GetSize(heightMeasureSpec);

    Int32 paddingLeft, paddingRight, paddingTop, paddingBottom;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);

    Int32 verticalPadding = paddingTop + paddingBottom;
    Int32 height = maxHeight - verticalPadding;
    Int32 childSpecHeight = View::MeasureSpec::MakeMeasureSpec(height, MeasureSpec::AT_MOST);
    Int32 exactHeightSpec = View::MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY);

    Int32 availableWidth = contentWidth - paddingLeft - paddingRight;
    Int32 leftOfCenter = availableWidth / 2;
    Int32 rightOfCenter = leftOfCenter;

    Int32 visibility = 0;
    IView::Probe(mTitleLayout)->GetVisibility(&visibility);
    Boolean showTitle = mTitleLayout != NULL && visibility != IView::GONE && (mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0;

    AutoPtr<IActionBarViewHomeView> homeLayout = mExpandedActionView != NULL ? IActionBarViewHomeView::Probe(mExpandedHomeLayout) : IActionBarViewHomeView::Probe(mHomeLayout);

    AutoPtr<IViewGroupLayoutParams> homeLp;
    IView::Probe(homeLayout)->GetLayoutParams((IViewGroupLayoutParams**)&homeLp);
    Int32 homeWidthSpec = 0;
    Int32 homeLpWidth = 0;
    homeLp->GetWidth(&homeLpWidth);
    if (homeLpWidth < 0) {
        homeWidthSpec = View::MeasureSpec::MakeMeasureSpec(availableWidth, View::MeasureSpec::AT_MOST);
    }
    else {
        homeWidthSpec = View::MeasureSpec::MakeMeasureSpec(homeLpWidth, View::MeasureSpec::EXACTLY);
    }

    IView::Probe(homeLayout)->Measure(homeWidthSpec, exactHeightSpec);
    Int32 homeWidth = 0;
    Int32 homeVisibility = 0;
    IView::Probe(homeLayout)->GetVisibility(&homeVisibility);
    AutoPtr<IViewParent> viewParent;
    IView::Probe(homeLayout)->GetParent((IViewParent**)&viewParent);
    if ((homeVisibility != IView::GONE && TO_IINTERFACE(viewParent) == TO_IINTERFACE(mUpGoerFive)) || showTitle) {
        IView::Probe(homeLayout)->GetMeasuredWidth(&homeWidth);
        Int32 homeof = 0;
        homeLayout->GetStartOffset(&homeof);
        Int32 homeOffsetWidth = homeWidth + homeof;
        availableWidth = Elastos::Core::Math::Max(0, availableWidth - homeOffsetWidth);
        leftOfCenter = Elastos::Core::Math::Max(0, availableWidth - homeOffsetWidth);
    }

    AutoPtr<IViewParent> mp;
    IView::Probe(mMenuView)->GetParent((IViewParent**)&mp);
    if (mMenuView && TO_IINTERFACE(mp) == TO_IINTERFACE(this)) {
        availableWidth = MeasureChildView(IView::Probe(mMenuView), availableWidth, exactHeightSpec, 0);
        Int32 menuw = 0;
        IView::Probe(mMenuView)->GetMeasuredWidth(&menuw);
        rightOfCenter = Elastos::Core::Math::Max(0, rightOfCenter - menuw);
    }

    Int32 iv = 0;
    IView::Probe(mIndeterminateProgressView)->GetVisibility(&iv);
    if (mIndeterminateProgressView && iv != IView::GONE) {
        availableWidth = MeasureChildView(IView::Probe(mIndeterminateProgressView), availableWidth, childSpecHeight, 0);
        Int32 ipw = 0;
        IView::Probe(mIndeterminateProgressView)->GetMeasuredWidth(&ipw);
        rightOfCenter = Elastos::Core::Math::Max(0, rightOfCenter - ipw);
    }

    if (!mExpandedActionView) {
        switch (mNavigationMode) {
            case IActionBar::NAVIGATION_MODE_LIST:
                if (mListNavLayout) {
                    Int32 itemPaddingSize = showTitle ? mItemPadding * 2 : mItemPadding;
                    availableWidth = Elastos::Core::Math::Max(0, availableWidth - itemPaddingSize);
                    leftOfCenter = Elastos::Core::Math::Max(0, leftOfCenter - itemPaddingSize);
                    IView::Probe(mListNavLayout)->Measure(View::MeasureSpec::MakeMeasureSpec(availableWidth, View::MeasureSpec::AT_MOST),
                        View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY));
                    Int32 listNavWidth = 0;
                    IView::Probe(mListNavLayout)->GetMeasuredWidth(&listNavWidth);
                    availableWidth = Elastos::Core::Math::Max(0, availableWidth - listNavWidth);
                    leftOfCenter = Elastos::Core::Math::Max(0, leftOfCenter - listNavWidth);
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                if (mTabScrollView) {
                    Int32 itemPaddingSize = showTitle ? mItemPadding * 2 : mItemPadding;
                    availableWidth = Elastos::Core::Math::Max(0, availableWidth - itemPaddingSize);
                    leftOfCenter = Elastos::Core::Math::Max(0, leftOfCenter - itemPaddingSize);
                    IView::Probe(mTabScrollView)->Measure(View::MeasureSpec::MakeMeasureSpec(availableWidth, View::MeasureSpec::AT_MOST),
                        View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY));
                    Int32 tabWidth = 0;
                    IView::Probe(mTabScrollView)->GetMeasuredWidth(&tabWidth);
                    availableWidth = Elastos::Core::Math::Max(0, availableWidth - tabWidth);
                    leftOfCenter = Elastos::Core::Math::Max(0, leftOfCenter - tabWidth);
                }
                break;
        }
    }

    AutoPtr<IView> customView;
    if (mExpandedActionView) {
        customView = mExpandedActionView;
    }
    else if ((mDisplayOptions & IActionBar::DISPLAY_SHOW_CUSTOM) != 0 && mCustomNavView) {
        customView = mCustomNavView;
    }

    if (customView) {
        AutoPtr<IViewGroupLayoutParams> cv;
        customView->GetLayoutParams((IViewGroupLayoutParams**)&cv);
        AutoPtr<IViewGroupLayoutParams> lp;
        GenerateLayoutParams(cv, (IViewGroupLayoutParams**)&lp);
        AutoPtr<IActionBarLayoutParams> ablp;
        if (IActionBarLayoutParams::Probe(lp)) {
            ablp = IActionBarLayoutParams::Probe(lp);
        }
        else {
            ablp = NULL;
        }

        Int32 horizontalMargin = 0;
        Int32 verticalMargin = 0;
        if (ablp) {
            Int32 ablm = 0, abrm = 0, abtm = 0, abbm = 0;
            IViewGroupMarginLayoutParams::Probe(ablp)->GetLeftMargin(&ablm);
            IViewGroupMarginLayoutParams::Probe(ablp)->GetRightMargin(&abrm);
            IViewGroupMarginLayoutParams::Probe(ablp)->GetTopMargin(&abtm);
            IViewGroupMarginLayoutParams::Probe(ablp)->GetBottomMargin(&abbm);

            horizontalMargin = ablm + abrm;
            verticalMargin = abtm + abbm;
        }

        Int32 customNavHeightMode;
        if (mContentHeight <= 0) {
            customNavHeightMode = View::MeasureSpec::AT_MOST;
        }
        else {
            Int32 height = 0;
            customNavHeightMode = (lp->GetHeight(&height), height) != IViewGroupLayoutParams::WRAP_CONTENT ?
                View::MeasureSpec::EXACTLY : View::MeasureSpec::AT_MOST;
        }
        Int32 lh = 0, lw = 0, abg = 0;
        lp->GetHeight(&lh);
        lp->GetWidth(&lw);
        ablp->GetGravity(&abg);
        Int32 customNavHeight = Elastos::Core::Math::Max(0, (lh >= 0 ? Elastos::Core::Math::Min(lh, height) : height) - verticalMargin);
        Int32 customNavWidthMode = lw != IViewGroupLayoutParams::WRAP_CONTENT ? View::MeasureSpec::EXACTLY : View::MeasureSpec::AT_MOST;
        Int32 customNavWidth = Elastos::Core::Math::Max(0, (lw >= 0 ? Elastos::Core::Math::Min(lw, availableWidth) : availableWidth) - horizontalMargin);
        Int32 hgrav = (ablp ? abg : DEFAULT_CUSTOM_GRAVITY) & IGravity::HORIZONTAL_GRAVITY_MASK;
        if (hgrav == IGravity::CENTER_HORIZONTAL && lw == IViewGroupLayoutParams::MATCH_PARENT) {
            customNavWidth = Elastos::Core::Math::Min(leftOfCenter, rightOfCenter) * 2;
        }

        customView->Measure(View::MeasureSpec::MakeMeasureSpec(customNavWidth, customNavWidthMode),
            View::MeasureSpec::MakeMeasureSpec(customNavHeight, customNavHeightMode));
        Int32 cmw = 0;
        customView->GetMeasuredWidth(&cmw);
        availableWidth -= horizontalMargin + cmw;
    }

    availableWidth = MeasureChildView(IView::Probe(mUpGoerFive), availableWidth + homeWidth, View::MeasureSpec::MakeMeasureSpec(mContentHeight, View::MeasureSpec::EXACTLY), 0);
    if (mTitleLayout) {
        Int32 tw = 0;
        IView::Probe(mTitleLayout)->GetMeasuredWidth(&tw);
        leftOfCenter = Elastos::Core::Math::Max(0, leftOfCenter - tw);
    }

    if (mContentHeight <= 0) {
        Int32 measuredHeight = 0;
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            Int32 vh = 0;
            v->GetMeasuredHeight(&vh);
            Int32 paddedViewHeight = vh + verticalPadding;
            if (paddedViewHeight > measuredHeight) {
                measuredHeight = paddedViewHeight;
            }
        }
        SetMeasuredDimension(contentWidth, measuredHeight);
    }
    else {
        SetMeasuredDimension(contentWidth, maxHeight);
    }

    if (mContextView) {
        Int32 measureHeight = 0;
        GetMeasuredHeight(&measureHeight);
        IAbsActionBarView::Probe(mContextView)->SetContentHeight(measureHeight);
    }

    Int32 pv = 0;
    IView::Probe(mProgressView)->GetVisibility(&pv);
    if (mProgressView && pv != IView::GONE) {
        Int32 measureHeight = 0;
        GetMeasuredHeight(&measureHeight);
        IView::Probe(mProgressView)->Measure(View::MeasureSpec::MakeMeasureSpec(contentWidth - mProgressBarPadding * 2, View::MeasureSpec::EXACTLY),
            View::MeasureSpec::MakeMeasureSpec(measureHeight, View::MeasureSpec::AT_MOST));
    }
}

ECode ActionBarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // ==================before translated======================
    // final int contentHeight = b - t - getPaddingTop() - getPaddingBottom();
    //
    // if (contentHeight <= 0) {
    //     // Nothing to do if we can't see anything.
    //     return;
    // }
    //
    // final boolean isLayoutRtl = isLayoutRtl();
    // final int direction = isLayoutRtl ? 1 : -1;
    // int menuStart = isLayoutRtl ? getPaddingLeft() : r - l - getPaddingRight();
    // // In LTR mode, we start from left padding and go to the right; in RTL mode, we start
    // // from the padding right and go to the left (in reverse way)
    // int x = isLayoutRtl ? r - l - getPaddingRight() : getPaddingLeft();
    // final int y = getPaddingTop();
    //
    // HomeView homeLayout = mExpandedActionView != null ? mExpandedHomeLayout : mHomeLayout;
    // final boolean showTitle = mTitleLayout != null && mTitleLayout.getVisibility() != GONE &&
    //         (mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0;
    // int startOffset = 0;
    // if (homeLayout.getParent() == mUpGoerFive) {
    //     if (homeLayout.getVisibility() != GONE) {
    //         startOffset = homeLayout.getStartOffset();
    //     } else if (showTitle) {
    //         startOffset = homeLayout.getUpWidth();
    //     }
    // }
    //
    // // Position the up container based on where the edge of the home layout should go.
    // x += positionChild(mUpGoerFive,
    //         next(x, startOffset, isLayoutRtl), y, contentHeight, isLayoutRtl);
    // x = next(x, startOffset, isLayoutRtl);
    //
    // if (mExpandedActionView == null) {
    //     switch (mNavigationMode) {
    //         case ActionBar.NAVIGATION_MODE_STANDARD:
    //             break;
    //         case ActionBar.NAVIGATION_MODE_LIST:
    //             if (mListNavLayout != null) {
    //                 if (showTitle) {
    //                     x = next(x, mItemPadding, isLayoutRtl);
    //                 }
    //                 x += positionChild(mListNavLayout, x, y, contentHeight, isLayoutRtl);
    //                 x = next(x, mItemPadding, isLayoutRtl);
    //             }
    //             break;
    //         case ActionBar.NAVIGATION_MODE_TABS:
    //             if (mTabScrollView != null) {
    //                 if (showTitle) x = next(x, mItemPadding, isLayoutRtl);
    //                 x += positionChild(mTabScrollView, x, y, contentHeight, isLayoutRtl);
    //                 x = next(x, mItemPadding, isLayoutRtl);
    //             }
    //             break;
    //     }
    // }
    //
    // if (mMenuView != null && mMenuView.getParent() == this) {
    //     positionChild(mMenuView, menuStart, y, contentHeight, !isLayoutRtl);
    //     menuStart += direction * mMenuView.getMeasuredWidth();
    // }
    //
    // if (mIndeterminateProgressView != null &&
    //         mIndeterminateProgressView.getVisibility() != GONE) {
    //     positionChild(mIndeterminateProgressView, menuStart, y, contentHeight, !isLayoutRtl);
    //     menuStart += direction * mIndeterminateProgressView.getMeasuredWidth();
    // }
    //
    // View customView = null;
    // if (mExpandedActionView != null) {
    //     customView = mExpandedActionView;
    // } else if ((mDisplayOptions & ActionBar.DISPLAY_SHOW_CUSTOM) != 0 &&
    //         mCustomNavView != null) {
    //     customView = mCustomNavView;
    // }
    // if (customView != null) {
    //     final int layoutDirection = getLayoutDirection();
    //     ViewGroup.LayoutParams lp = customView.getLayoutParams();
    //     final ActionBar.LayoutParams ablp = lp instanceof ActionBar.LayoutParams ?
    //             (ActionBar.LayoutParams) lp : null;
    //     final int gravity = ablp != null ? ablp.gravity : DEFAULT_CUSTOM_GRAVITY;
    //     final int navWidth = customView.getMeasuredWidth();
    //
    //     int topMargin = 0;
    //     int bottomMargin = 0;
    //     if (ablp != null) {
    //         x = next(x, ablp.getMarginStart(), isLayoutRtl);
    //         menuStart += direction * ablp.getMarginEnd();
    //         topMargin = ablp.topMargin;
    //         bottomMargin = ablp.bottomMargin;
    //     }
    //
    //     int hgravity = gravity & Gravity.RELATIVE_HORIZONTAL_GRAVITY_MASK;
    //     // See if we actually have room to truly center; if not push against left or right.
    //     if (hgravity == Gravity.CENTER_HORIZONTAL) {
    //         final int centeredLeft = ((mRight - mLeft) - navWidth) / 2;
    //         if (isLayoutRtl) {
    //             final int centeredStart = centeredLeft + navWidth;
    //             final int centeredEnd = centeredLeft;
    //             if (centeredStart > x) {
    //                 hgravity = Gravity.RIGHT;
    //             } else if (centeredEnd < menuStart) {
    //                 hgravity = Gravity.LEFT;
    //             }
    //         } else {
    //             final int centeredStart = centeredLeft;
    //             final int centeredEnd = centeredLeft + navWidth;
    //             if (centeredStart < x) {
    //                 hgravity = Gravity.LEFT;
    //             } else if (centeredEnd > menuStart) {
    //                 hgravity = Gravity.RIGHT;
    //             }
    //         }
    //     } else if (gravity == Gravity.NO_GRAVITY) {
    //         hgravity = Gravity.START;
    //     }
    //
    //     int xpos = 0;
    //     switch (Gravity.getAbsoluteGravity(hgravity, layoutDirection)) {
    //         case Gravity.CENTER_HORIZONTAL:
    //             xpos = ((mRight - mLeft) - navWidth) / 2;
    //             break;
    //         case Gravity.LEFT:
    //             xpos = isLayoutRtl ? menuStart : x;
    //             break;
    //         case Gravity.RIGHT:
    //             xpos = isLayoutRtl ? x - navWidth : menuStart - navWidth;
    //             break;
    //     }
    //
    //     int vgravity = gravity & Gravity.VERTICAL_GRAVITY_MASK;
    //
    //     if (gravity == Gravity.NO_GRAVITY) {
    //         vgravity = Gravity.CENTER_VERTICAL;
    //     }
    //
    //     int ypos = 0;
    //     switch (vgravity) {
    //         case Gravity.CENTER_VERTICAL:
    //             final int paddedTop = getPaddingTop();
    //             final int paddedBottom = mBottom - mTop - getPaddingBottom();
    //             ypos = ((paddedBottom - paddedTop) - customView.getMeasuredHeight()) / 2;
    //             break;
    //         case Gravity.TOP:
    //             ypos = getPaddingTop() + topMargin;
    //             break;
    //         case Gravity.BOTTOM:
    //             ypos = getHeight() - getPaddingBottom() - customView.getMeasuredHeight()
    //                     - bottomMargin;
    //             break;
    //     }
    //     final int customWidth = customView.getMeasuredWidth();
    //     customView.layout(xpos, ypos, xpos + customWidth,
    //             ypos + customView.getMeasuredHeight());
    //     x = next(x, customWidth, isLayoutRtl);
    // }
    //
    // if (mProgressView != null) {
    //     mProgressView.bringToFront();
    //     final int halfProgressHeight = mProgressView.getMeasuredHeight() / 2;
    //     mProgressView.layout(mProgressBarPadding, -halfProgressHeight,
    //             mProgressBarPadding + mProgressView.getMeasuredWidth(), halfProgressHeight);
    // }

    assert(0);
    Int32 paddingTop, paddingBottom;
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);

    Int32 contentHeight = b - t - paddingTop - paddingBottom;
    if (contentHeight <= 0) {
        return NOERROR;
    }

    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);
    Int32 direction = isLayoutRtl ? 1 : -1;

    Int32 paddingLeft, paddingRight;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);

    Int32 menuStart = isLayoutRtl ? paddingLeft : r - l - paddingRight;
    Int32 x = isLayoutRtl ? r - l - paddingRight : paddingLeft;
    Int32 y = paddingTop;

    AutoPtr<HomeView> homeLayout = mExpandedActionView ? (HomeView*)mExpandedHomeLayout : (HomeView*)mHomeLayout;
    Int32 titleVisibility = 0;
    IView::Probe(mTitleLayout)->GetVisibility(&titleVisibility);
    Boolean showTitle = mTitleLayout != NULL && titleVisibility != IView::GONE &&
        (mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0;
    Int32 startOffset = 0;
    AutoPtr<IViewParent> homeViewParent;
    IView::Probe(homeLayout)->GetParent((IViewParent**)&homeViewParent);
    if (TO_IINTERFACE(homeViewParent)== TO_IINTERFACE(mUpGoerFive)) {
        Int32 homeVisibility = 0;
        IView::Probe(homeLayout)->GetVisibility(&homeVisibility);
        if (homeVisibility != IView::GONE) {
            homeLayout->GetStartOffset(&startOffset);
        }
        else if (showTitle) {
            homeLayout->GetUpWidth(&startOffset);
        }
    }

    x += PositionChild(IView::Probe(mUpGoerFive), Next(x, startOffset, isLayoutRtl), y, contentHeight, isLayoutRtl);
    x = Next(x, startOffset, isLayoutRtl);

    if (!mExpandedActionView) {
        switch (mNavigationMode) {
            case IActionBar::NAVIGATION_MODE_STANDARD:
                break;
            case IActionBar::NAVIGATION_MODE_LIST:
                if (mListNavLayout) {
                    if (showTitle) {
                        x = Next(x, mItemPadding, isLayoutRtl);
                    }
                    x += PositionChild(IView::Probe(mListNavLayout), x, y, contentHeight, isLayoutRtl);
                    x = Next(x, mItemPadding, isLayoutRtl);
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                if (mTabScrollView) {
                    if (showTitle) x = Next(x, mItemPadding, isLayoutRtl);
                    x += PositionChild(IView::Probe(mTabScrollView), x, y, contentHeight, isLayoutRtl);
                    x = Next(x, mItemPadding, isLayoutRtl);
                }
                break;
        }
    }

    if (mMenuView) {
        AutoPtr<IViewParent> p;
        IView::Probe(mMenuView)->GetParent((IViewParent**)&p);
        if (TO_IINTERFACE(p) == TO_IINTERFACE(this)) {
            PositionChild(IView::Probe(mMenuView), menuStart, y, contentHeight, !isLayoutRtl);
            Int32 w = 0;
            IView::Probe(mMenuView)->GetMeasuredWidth(&w);
            menuStart += direction * w;
        }
    }

    Int32 visibility = 0;
    IView::Probe(mIndeterminateProgressView)->GetVisibility(&visibility);
    if (mIndeterminateProgressView && visibility != IView::GONE) {
        PositionChild(IView::Probe(mIndeterminateProgressView), menuStart, y, contentHeight, !isLayoutRtl);
        Int32 w = 0;
        IView::Probe(mIndeterminateProgressView)->GetMeasuredWidth(&w);
        menuStart += direction * w;
    }

    AutoPtr<IView> customView;
    if (mExpandedActionView) {
        customView = mExpandedActionView;
    }
    else if ((mDisplayOptions & IActionBar::DISPLAY_SHOW_CUSTOM) != 0 && mCustomNavView) {
        customView = mCustomNavView;
    }

    if (customView) {
        Int32 layoutDirection = 0;
        GetLayoutDirection(&layoutDirection);
        AutoPtr<IViewGroupLayoutParams> lp;
        customView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IActionBarLayoutParams> ablp;
        Int32 gravity = DEFAULT_CUSTOM_GRAVITY;
        if (IActionBarLayoutParams::Probe(lp)) {
            ablp = IActionBarLayoutParams::Probe(lp);
            ablp->GetGravity(&gravity);
        }
        else {
            ablp = NULL;
        }

        Int32 navWidth = 0;
        customView->GetMeasuredWidth(&navWidth);
        Int32 topMargin = 0;
        Int32 bottomMargin = 0;
        if (ablp) {
            Int32 start = 0, end = 0;
            IViewGroupMarginLayoutParams::Probe(ablp)->GetMarginStart(&start);
            x = Next(x, start, isLayoutRtl);
            IViewGroupMarginLayoutParams::Probe(ablp)->GetMarginEnd(&end);
            menuStart += direction * end;
            IViewGroupMarginLayoutParams::Probe(ablp)->GetTopMargin(&topMargin);
            IViewGroupMarginLayoutParams::Probe(ablp)->GetBottomMargin(&bottomMargin);
        }

        Int32 hgravity = gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
        if (hgravity == IGravity::CENTER_HORIZONTAL) {
            Int32 centeredLeft = ((mRight - mLeft) - navWidth) / 2;
            if (isLayoutRtl) {
                Int32 centeredStart = centeredLeft + navWidth;
                Int32 centeredEnd = centeredLeft;
                if (centeredStart > x) {
                    hgravity = IGravity::RIGHT;
                }
                else if (centeredEnd < menuStart) {
                    hgravity = IGravity::LEFT;
                }
            }
            else {
                Int32 centeredStart = centeredLeft;
                Int32 centeredEnd = centeredLeft + navWidth;
                if (centeredStart < x) {
                    hgravity = IGravity::LEFT;
                }
                else if (centeredEnd > menuStart) {
                    hgravity = IGravity::RIGHT;
                }
            }
        }
        else if (gravity == IGravity::NO_GRAVITY) {
            hgravity = IGravity::START;
        }

        Int32 xpos = 0;
        switch (Gravity::GetAbsoluteGravity(hgravity, layoutDirection)) {
            case IGravity::CENTER_HORIZONTAL:
                xpos = ((mRight - mLeft) - navWidth) / 2;
                break;
            case IGravity::LEFT:
                xpos = isLayoutRtl ? menuStart : x;
                break;
            case IGravity::RIGHT:
                xpos = isLayoutRtl ? x - navWidth : menuStart - navWidth;
                break;
        }

        Int32 vgravity = gravity & IGravity::VERTICAL_GRAVITY_MASK;
        if (gravity == IGravity::NO_GRAVITY) {
            vgravity = IGravity::CENTER_VERTICAL;
        }

        Int32 ypos = 0;
        switch (vgravity) {
            case IGravity::CENTER_VERTICAL:
                {
                    Int32 paddedBottom = mBottom - mTop - paddingBottom;
                    Int32 h = 0;
                    customView->GetMeasuredHeight(&h);
                    ypos = ((paddedBottom - paddingTop) - h) / 2;
                }
                break;
            case IGravity::TOP:
                ypos = paddingTop + topMargin;
                break;
            case IGravity::BOTTOM:
                {
                    Int32 h = 0;
                    customView->GetMeasuredHeight(&h);
                    Int32 height = 0;
                    GetHeight(&height);
                    ypos = height - paddingBottom - h - bottomMargin;
                }
                break;
        }
        Int32 customWidth = 0;
        customView->GetMeasuredWidth(&customWidth);
        Int32 h = 0;
        customView->GetMeasuredHeight(&h);
        customView->Layout(xpos, ypos, xpos + customWidth, ypos + h);
        x = Next(x, customWidth, isLayoutRtl);
    }

    if (mProgressView) {
        IView::Probe(mProgressView)->BringToFront();
        Int32 ph = 0;
        IView::Probe(mProgressView)->GetMeasuredHeight(&ph);
        Int32 halfProgressHeight = ph / 2;
        Int32 pw = 0;
        IView::Probe(mProgressView)->GetMeasuredWidth(&pw);
        IView::Probe(mProgressView)->Layout(mProgressBarPadding, -halfProgressHeight, mProgressBarPadding + pw, halfProgressHeight);
    }
    return NOERROR;
}

void ActionBarView::ConfigPresenters(
    /* [in] */ IMenuBuilder* builder)
{
    // ==================before translated======================
    // if (builder != null) {
    //     builder.addMenuPresenter(mActionMenuPresenter, mPopupContext);
    //     builder.addMenuPresenter(mExpandedMenuPresenter, mPopupContext);
    // } else {
    //     mActionMenuPresenter.initForMenu(mPopupContext, null);
    //     mExpandedMenuPresenter.initForMenu(mPopupContext, null);
    //     mActionMenuPresenter.updateMenuView(true);
    //     mExpandedMenuPresenter.updateMenuView(true);
    // }

    assert(0);
    if (builder) {
        builder->AddMenuPresenter(IMenuPresenter::Probe(mActionMenuPresenter), mPopupContext);
        builder->AddMenuPresenter(IMenuPresenter::Probe(mExpandedMenuPresenter), mPopupContext);
    }
    else {
        IMenuPresenter::Probe(mActionMenuPresenter)->InitForMenu(mPopupContext, NULL);
        mExpandedMenuPresenter->InitForMenu(mPopupContext, NULL);
        IMenuPresenter::Probe(mActionMenuPresenter)->UpdateMenuView(TRUE);
        mExpandedMenuPresenter->UpdateMenuView(TRUE);
    }
}

void ActionBarView::SetTitleImpl(
    /* [in] */ ICharSequence* title)
{
    // ==================before translated======================
    // ActionBarTransition.beginDelayedTransition(this);
    // mTitle = title;
    // if (mTitleView != null) {
    //     mTitleView.setText(title);
    //     final boolean visible = mExpandedActionView == null &&
    //             (mDisplayOptions & ActionBar.DISPLAY_SHOW_TITLE) != 0 &&
    //             (!TextUtils.isEmpty(mTitle) || !TextUtils.isEmpty(mSubtitle));
    //     mTitleLayout.setVisibility(visible ? VISIBLE : GONE);
    // }
    // if (mLogoNavItem != null) {
    //     mLogoNavItem.setTitle(title);
    // }
    // updateHomeAccessibility(mUpGoerFive.isEnabled());

    assert(0);
    ActionBarTransition::BeginDelayedTransition(this);
    mTitle = title;
    if (mTitleView) {
        mTitleView->SetText(title);
        Boolean visible = mExpandedActionView == NULL && (mDisplayOptions & IActionBar::DISPLAY_SHOW_TITLE) != 0 &&
            (!TextUtils::IsEmpty(mTitle) || !TextUtils::IsEmpty(mSubtitle));
        IView::Probe(mTitleLayout)->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    }
    if (mLogoNavItem != NULL) {
        IMenuItem::Probe(mLogoNavItem)->SetTitle(title);
    }
    Boolean isEnalbed = FALSE;
    IView::Probe(mUpGoerFive)->IsEnabled(&isEnalbed);
    UpdateHomeAccessibility(isEnalbed);
}

void ActionBarView::SetHomeButtonEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ Boolean recordState)
{
    // ==================before translated======================
    // if (recordState) {
    //     mWasHomeEnabled = enable;
    // }
    //
    // if (mExpandedActionView != null) {
    //     // There's an action view currently showing and we want to keep the state
    //     // configured for the action view at the moment. If we needed to record the
    //     // new state for later we will have done so above.
    //     return;
    // }
    //
    // mUpGoerFive.setEnabled(enable);
    // mUpGoerFive.setFocusable(enable);
    // // Make sure the home button has an accurate content description for accessibility.
    // updateHomeAccessibility(enable);

    assert(0);
    if (recordState) {
        mWasHomeEnabled = enable;
    }

    if (mExpandedActionView) {
        return ;
    }

    IView::Probe(mUpGoerFive)->SetEnabled(enable);
    IView::Probe(mUpGoerFive)->SetFocusable(enable);
    UpdateHomeAccessibility(enable);
}

void ActionBarView::UpdateHomeAccessibility(
    /* [in] */ Boolean homeEnabled)
{
    // ==================before translated======================
    // if (!homeEnabled) {
    //     mUpGoerFive.setContentDescription(null);
    //     mUpGoerFive.setImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_NO);
    // } else {
    //     mUpGoerFive.setImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_AUTO);
    //     mUpGoerFive.setContentDescription(buildHomeContentDescription());
    // }

    if (!homeEnabled) {
        IView::Probe(mUpGoerFive)->SetContentDescription(NULL);
        IView::Probe(mUpGoerFive)->SetImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_NO);
    }
    else {
        IView::Probe(mUpGoerFive)->SetImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_AUTO);
        IView::Probe(mUpGoerFive)->SetContentDescription(BuildHomeContentDescription());
    }
}

AutoPtr<ICharSequence> ActionBarView::BuildHomeContentDescription()
{
    // ==================before translated======================
    // final CharSequence homeDesc;
    // if (mHomeDescription != null) {
    //     homeDesc = mHomeDescription;
    // } else {
    //     if ((mDisplayOptions & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //         homeDesc = mContext.getResources().getText(mDefaultUpDescription);
    //     } else {
    //         homeDesc = mContext.getResources().getText(R.string.action_bar_home_description);
    //     }
    // }
    //
    // final CharSequence title = getTitle();
    // final CharSequence subtitle = getSubtitle();
    // if (!TextUtils.isEmpty(title)) {
    //     final String result;
    //     if (!TextUtils.isEmpty(subtitle)) {
    //         result = getResources().getString(
    //                 R.string.action_bar_home_subtitle_description_format,
    //                 title, subtitle, homeDesc);
    //     } else {
    //         result = getResources().getString(R.string.action_bar_home_description_format,
    //                 title, homeDesc);
    //     }
    //     return result;
    // }
    // return homeDesc;

    AutoPtr<ICharSequence> homeDesc;
    if (mHomeDescription != NULL) {
        homeDesc = mHomeDescription;
    }
    else {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        if ((mDisplayOptions & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
            resources->GetText(mDefaultUpDescription, (ICharSequence**)&homeDesc);
        }
        else {
            resources->GetText(R::string::action_bar_home_description, (ICharSequence**)&homeDesc);
        }
    }

    AutoPtr<ICharSequence> title;
    GetTitle((ICharSequence**)&title);
    AutoPtr<ICharSequence> subtitle;
    GetSubtitle((ICharSequence**)&subtitle);
    if (!TextUtils::IsEmpty(title)) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String resultTmp;
        if (!TextUtils::IsEmpty(subtitle)) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(3);
            formatArgs->Set(0, title);
            formatArgs->Set(1, subtitle);
            formatArgs->Set(2, homeDesc);
            resources->GetString(R::string::action_bar_home_subtitle_description_format, formatArgs, &resultTmp);
        }
        else {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(2);
            formatArgs->Set(0, title);
            formatArgs->Set(1, homeDesc);
            resources->GetString(R::string::action_bar_home_description_format, formatArgs, &resultTmp);
        }

        AutoPtr<ICharSequence> result;
        CString::New(resultTmp, (ICharSequence**)&result);
        return result;
    }
    return homeDesc;
}

void ActionBarView::InitTitle()
{
    // ==================before translated======================
    // if (mTitleLayout == null) {
    //     LayoutInflater inflater = LayoutInflater.from(getContext());
    //     mTitleLayout = (LinearLayout) inflater.inflate(R.layout.action_bar_title_item,
    //             this, false);
    //     mTitleView = (TextView) mTitleLayout.findViewById(R.id.action_bar_title);
    //     mSubtitleView = (TextView) mTitleLayout.findViewById(R.id.action_bar_subtitle);
    //
    //     if (mTitleStyleRes != 0) {
    //         mTitleView.setTextAppearance(mContext, mTitleStyleRes);
    //     }
    //     if (mTitle != null) {
    //         mTitleView.setText(mTitle);
    //     }
    //
    //     if (mSubtitleStyleRes != 0) {
    //         mSubtitleView.setTextAppearance(mContext, mSubtitleStyleRes);
    //     }
    //     if (mSubtitle != null) {
    //         mSubtitleView.setText(mSubtitle);
    //         mSubtitleView.setVisibility(VISIBLE);
    //     }
    // }
    //
    // ActionBarTransition.beginDelayedTransition(this);
    // mUpGoerFive.addView(mTitleLayout);
    // if (mExpandedActionView != null ||
    //         (TextUtils.isEmpty(mTitle) && TextUtils.isEmpty(mSubtitle))) {
    //     // Don't show while in expanded mode or with empty text
    //     mTitleLayout.setVisibility(GONE);
    // } else {
    //     mTitleLayout.setVisibility(VISIBLE);
    // }

    assert(0);
    if (!mTitleLayout) {
        AutoPtr<IContext> context;
        IView::Probe(this)->GetContext((IContext**)&context);

        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(context, (ILayoutInflater**)&inflater);
        AutoPtr<IView> v;
        inflater->Inflate(R::layout::action_bar_title_item, IViewGroup::Probe(this), FALSE, (IView**)&v);
        mTitleLayout = ILinearLayout::Probe(v);
        AutoPtr<IView> title;
        IView::Probe(mTitleLayout)->FindViewById(R::id::action_bar_title, (IView**)&title);
        mTitleView = ITextView::Probe(title);
        AutoPtr<IView> subview;
        IView::Probe(mTitleLayout)->FindViewById(R::id::action_bar_subtitle, (IView**)&subview);
        mSubtitleView = ITextView::Probe(subview);

        if (mTitleStyleRes != 0) {
            mTitleView->SetTextAppearance(mContext, mTitleStyleRes);
        }
        if (mTitle) {
            mTitleView->SetText(mTitle);
        }

        if (mSubtitleStyleRes != 0) {
            mSubtitleView->SetTextAppearance(mContext, mSubtitleStyleRes);
        }
        if (mSubtitle) {
            mSubtitleView->SetText(mSubtitle);
            IView::Probe(mSubtitleView)->SetVisibility(IView::VISIBLE);
        }
    }

    ActionBarTransition::BeginDelayedTransition(this);
    IViewGroup::Probe(mUpGoerFive)->AddView(IView::Probe(mTitleLayout));
    if (mExpandedActionView || (TextUtils::IsEmpty(mTitle) && TextUtils::IsEmpty(mSubtitle))) {
        IView::Probe(mTitleLayout)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(mTitleLayout)->SetVisibility(VISIBLE);
    }
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


