
#include "elastos/droid/widget/ActionMenuView.h"
#include "elastos/droid/widget/CActionMenuPresenter.h"
#include "elastos/droid/widget/CActionMenuViewLayoutParams.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IActionMenuItemView;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuView;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderItemInvoker;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Widget {

static Int32 BitCount(Int64 v) {
    // Combines techniques from several sources
    if (v < 0)
        v = -v;
    v -=  (v >> 1) & 0x5555555555555555L;
    v = (v & 0x3333333333333333L) + ((v >> 2) & 0x3333333333333333L);
    Int32 i =  ((Int32)(v >> 32)) + (Int32) v;
    if (i < 0)
        i = -i;
    i = (i & 0x0F0F0F0F) + ((i >> 4) & 0x0F0F0F0F);
    i += i >> 8;
    i += i >> 16;
    return i  & 0x0000007F;
}

String ActionMenuView::TAG("ActionMenuView");
const Int32 ActionMenuView::MIN_CELL_SIZE = 56; // dips
const Int32 ActionMenuView::GENERATED_ITEM_PADDING = 4; // dips

CAR_INTERFACE_IMPL(ActionMenuView::ActionMenuViewLayoutParams, LinearLayout::LinearLayoutLayoutParams, IActionMenuViewLayoutParams)

ActionMenuView::ActionMenuViewLayoutParams::ActionMenuViewLayoutParams()
    : mIsOverflowButton(0)
    , mCellsUsed(0)
    , mExtraPixels(0)
    , mExpandable(0)
    , mPreventEdgeOffset(0)
    , mExpanded(0)
{}

ECode ActionMenuView::ActionMenuViewLayoutParams::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::LinearLayoutLayoutParams::constructor(ctx, attrs);
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* other)
{
    LinearLayout::LinearLayoutLayoutParams::constructor(other);
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::constructor(
    /* [in] */ IActionMenuViewLayoutParams* actLp)
{
    IViewGroupMarginLayoutParams* temp = IViewGroupMarginLayoutParams::Probe(actLp);
    LinearLayout::LinearLayoutLayoutParams::constructor(temp);
    actLp->GetIsOverflowButton(&mIsOverflowButton);
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    LinearLayout::LinearLayoutLayoutParams::constructor(width, height);
    mIsOverflowButton = FALSE;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean isOverflowButton)
{
    LinearLayout::LinearLayoutLayoutParams::constructor(width, height);
    mIsOverflowButton = isOverflowButton;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetIsOverflowButton(
    /* [in] */ Boolean isOverflow)
{
    mIsOverflowButton = isOverflow;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetIsOverflowButton(
    /* [out] */ Boolean* isOverflow)
{
    VALIDATE_NOT_NULL(isOverflow)
    *isOverflow = mIsOverflowButton;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetCellsUsed(
    /* [in] */ Int32 cellsUsed)
{
    mCellsUsed = cellsUsed;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetCellsUsed(
    /* [out] */ Int32* cellsUsed)
{
    VALIDATE_NOT_NULL(cellsUsed)
    *cellsUsed = mCellsUsed;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetExtraPixels(
    /* [in] */ Int32 extraPixels)
{
    mExtraPixels = extraPixels;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetExtraPixels(
    /* [out] */ Int32* extraPixels)
{
    VALIDATE_NOT_NULL(extraPixels)
    *extraPixels = mExtraPixels;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetExpandable(
    /* [in] */ Boolean expandable)
{
    mExpandable = expandable;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetExpandable(
    /* [out] */ Boolean* expandable)
{
    VALIDATE_NOT_NULL(expandable)
    *expandable = mExpandable;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetPreventEdgeOffset(
    /* [in] */ Boolean preventEdgeOffset)
{
    mPreventEdgeOffset = preventEdgeOffset;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetPreventEdgeOffset(
    /* [out] */ Boolean* preventEdgeOffset)
{
    VALIDATE_NOT_NULL(preventEdgeOffset)
    *preventEdgeOffset = mPreventEdgeOffset;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::SetExpanded(
    /* [in] */ Boolean expanded)
{
    mExpanded = expanded;
    return NOERROR;
}

ECode ActionMenuView::ActionMenuViewLayoutParams::GetExpanded(
    /* [out] */ Boolean* expanded)
{
    VALIDATE_NOT_NULL(expanded)
    *expanded = mExpanded;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuView::MenuBuilderCallback, Object, IActionMenuViewLayoutParams)

ActionMenuView::MenuBuilderCallback::MenuBuilderCallback(
    /* [in] */ ActionMenuView* host)
    : mHost(host)
{}

ECode ActionMenuView::MenuBuilderCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)
    return mHost->mOnMenuItemClickListener != NULL &&
            mHost->mOnMenuItemClickListener->OnMenuItemClick(item, state);
}

/**
 * Called when the mode of the menu changes (for example, from icon to expanded).
 *
 * @param menu the menu that has changed modes
 */
ECode ActionMenuView::MenuBuilderCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    if (mHost->mMenuBuilderCallback != NULL) {
        mHost->mMenuBuilderCallback->OnMenuModeChange(menu);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuView::ActionMenuPresenterCallback, Object, IMenuPresenterCallback)

ECode ActionMenuView::ActionMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    return NOERROR;
}

ECode ActionMenuView::ActionMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL_3(ActionMenuView, LinearLayout, IActionMenuView, IMenuView, IMenuBuilderItemInvoker)

ActionMenuView::ActionMenuView()
    : mReserveOverflow(FALSE)
    , mFormatItems(FALSE)
    , mFormatItemsWidth(0)
    , mMinCellSize(0)
    , mGeneratedItemPadding(0)
    , mMeasuredExtraWidth(0)
    , mMaxItemHeight(0)
{
}

ECode ActionMenuView::constructor(
    /* [in] */ IContext* context)
{
    InitActionMenu(context, NULL);
    return NOERROR;
}

ECode ActionMenuView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    InitActionMenu(context, attrs);
    return NOERROR;
}

ECode ActionMenuView::SetPopupTheme(
    /* [in] */ Int32 resId)
{
    if (mPopupTheme != resId) {
        mPopupTheme = resId;
        if (resId == 0) {
            mPopupContext = mContext;
        } else {
            CContextThemeWrapper::New(mContext, resId, (IContext**)&mPopupContext);
        }
    }
    return NOERROR;
}

/**
 * @return resource identifier of the theme used to inflate popup menus, or
 *         0 if menus are inflated against the action menu view theme
 * @see #setPopupTheme(int)
 */
ECode ActionMenuView::GetPopupTheme(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme)
    *theme = mPopupTheme;
    return NOERROR;
}

ECode ActionMenuView::SetPresenter(
    /* [in] */ IActionMenuPresenter* presenter)
{
    mPresenter = NULL;
    if (presenter != NULL) {
        IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(presenter);
        wrs->GetWeakReference((IWeakReference**)&mPresenter);
    }
    presenter->SetMenuView(this);
    return NOERROR;
}

ECode ActionMenuView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    LinearLayout::OnConfigurationChanged(newConfig);
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL)
        IMenuPresenter::Probe(presenter)->UpdateMenuView(FALSE);

    Boolean rst = FALSE;
    if (presenter != NULL && (presenter->IsOverflowMenuShowing(&rst), rst)) {
        presenter->HideOverflowMenu(&rst);
        presenter->ShowOverflowMenu(&rst);
    }
    return NOERROR;
}

ECode ActionMenuView::SetOnMenuItemClickListener(
    /* [in] */ IActionMenuViewOnMenuItemClickListener* listener)
{
    mOnMenuItemClickListener = listener;
    return NOERROR;
}

ECode ActionMenuView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // If we've been given an exact size to match, apply special formatting during layout.
    const Boolean wasFormatted = mFormatItems;
    mFormatItems = MeasureSpec::GetMode(widthMeasureSpec) == MeasureSpec::EXACTLY;

    if (wasFormatted != mFormatItems) {
        mFormatItemsWidth = 0; // Reset this when switching modes
    }

    // Special formatting can change whether items can fit as action buttons.
    // Kick the menu and update presenters when this changes.
    const Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    if (mFormatItems && mMenu != NULL && widthSize != mFormatItemsWidth) {
        mFormatItemsWidth = widthSize;
        mMenu->OnItemsChanged(TRUE);
    }

    Int32 childCount;
    GetChildCount(&childCount);
    if (mFormatItems && childCount> 0) {
        OnMeasureExactFormat(widthMeasureSpec, heightMeasureSpec);
    }
    else {
        // Previous measurement at exact format may have set margins - reset them.
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            AutoPtr<IViewGroupLayoutParams> temp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
            AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
            lp->SetLeftMargin(0);
            lp->SetRightMargin(0);
        }

        LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
    return NOERROR;
}

void ActionMenuView::OnMeasureExactFormat(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // We already know the width mode is EXACTLY if we're here.
    const Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    const Int32 widthPadding = mPaddingLeft + mPaddingRight;
    const Int32 heightPadding = mPaddingTop + mPaddingBottom;

    const Int32 itemHeightSpec = heightMode == MeasureSpec::EXACTLY
            ? MeasureSpec::MakeMeasureSpec(heightSize - heightPadding, MeasureSpec::EXACTLY)
            : MeasureSpec::MakeMeasureSpec(
                Elastos::Core::Math::Min(mMaxItemHeight, heightSize - heightPadding), MeasureSpec::AT_MOST);

    widthSize -= widthPadding;

    // Divide the view into cells.
    const Int32 cellCount = widthSize / mMinCellSize;
    const Int32 cellSizeRemaining = widthSize % mMinCellSize;

    if (cellCount == 0) {
        // Give up, nothing fits.
        SetMeasuredDimension(widthSize, 0);
        return;
    }

    const Int32 cellSize = mMinCellSize + cellSizeRemaining / cellCount;

    Int32 cellsRemaining = cellCount;
    Int32 maxChildHeight = 0;
    Int32 maxCellsUsed = 0;
    Int32 expandableItemCount = 0;
    Int32 visibleItemCount = 0;
    Boolean hasOverflow = FALSE;

    // This is used as a bitfield to locate the smallest items present. Assumes childCount < 64.
    Int64 smallestItemsAt = 0;

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 visiable = 0;
        if ((child->GetVisibility(&visiable), visiable) == IView::GONE) continue;

        const Boolean isGeneratedItem = IActionMenuItemView::Probe(child) != NULL;
        visibleItemCount++;

        if (isGeneratedItem) {
            // Reset padding for generated menu item views; it may change below
            // and views are recycled.
            child->SetPadding(mGeneratedItemPadding, 0, mGeneratedItemPadding, 0);
        }

        AutoPtr<IViewGroupLayoutParams> tmp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&tmp);
        AutoPtr<IActionMenuViewLayoutParams> lp = IActionMenuViewLayoutParams::Probe(tmp);
        AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(tmp);

        lp->SetExpanded(FALSE);
        lp->SetExtraPixels(0);
        lp->SetCellsUsed(0);
        lp->SetExpandable(FALSE);
        mlp->SetLeftMargin(0);
        mlp->SetRightMargin(0);

        Boolean hasText;
        IActionMenuItemView::Probe(child)->HasText(&hasText);

        lp->SetPreventEdgeOffset(isGeneratedItem && hasText);
        // Overflow always gets 1 cell. No more, no less.
        Boolean isOverflow;
        lp->GetIsOverflowButton(&isOverflow);
        const Int32 cellsAvailable = isOverflow ? 1 : cellsRemaining;

        const Int32 cellsUsed = MeasureChildForCells(child, cellSize, cellsAvailable,
                itemHeightSpec, heightPadding);

        maxCellsUsed = Elastos::Core::Math::Max(maxCellsUsed, cellsUsed);
        Boolean expandable;
        lp->GetExpandable(&expandable);
        if (expandable) expandableItemCount++;
        if (isOverflow) hasOverflow = TRUE;

        cellsRemaining -= cellsUsed;

        Int32 measureHeight = 0;
        child->GetMeasuredHeight(&measureHeight);
        maxChildHeight = Elastos::Core::Math::Max(maxChildHeight, measureHeight);
        if (cellsUsed == 1) smallestItemsAt |= (1 << i);
    }

    // When we have overflow and a single expanded (text) item, we want to try centering it
    // visually in the available space even though overflow consumes some of it.
    const Boolean centerSingleExpandedItem = hasOverflow && visibleItemCount == 2;

    // Divide space for remaining cells if we have items that can expand.
    // Try distributing whole leftover cells to smaller items first.

    Boolean needsExpansion = FALSE;
    while (expandableItemCount > 0 && cellsRemaining > 0) {
        Int32 minCells = Elastos::Core::Math::INT32_MAX_VALUE;
        Int64 minCellsAt = 0; // Bit locations are indices of relevant child views
        Int32 minCellsItemCount = 0;
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);

            // Don't try to expand items that shouldn't.
            Boolean expandable;
            alp->GetExpandable(&expandable);
            if (!expandable) continue;

            // Mark indices of children that can receive an extra cell.
            Int32 cellsUsed;
            alp->GetCellsUsed(&cellsUsed);
            if (cellsUsed < minCells) {
                minCells = cellsUsed;
                minCellsAt = 1 << i;
                minCellsItemCount = 1;
            } else if (cellsUsed == minCells) {
                minCellsAt |= 1 << i;
                minCellsItemCount++;
            }
        }

        // Items that get expanded will always be in the set of smallest items when we're done.
        smallestItemsAt |= minCellsAt;

        if (minCellsItemCount > cellsRemaining) break; // Couldn't expand anything evenly. Stop.

        // We have enough cells, all minimum size items will be incremented.
        minCells++;

        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);

            if ((minCellsAt & (1 << i)) == 0) {
                // If this item is already at our small item count, mark it for later.
                Int32 cellsUsed;
                alp->GetCellsUsed(&cellsUsed);
                if (cellsUsed == minCells) smallestItemsAt |= 1 << i;
                continue;
            }

            Boolean preventEdgeOffset;
            alp->GetPreventEdgeOffset(&preventEdgeOffset);
            if (centerSingleExpandedItem && preventEdgeOffset && cellsRemaining == 1) {
                // Add padding to this item such that it centers.
                child->SetPadding(mGeneratedItemPadding + cellSize, 0, mGeneratedItemPadding, 0);
            }

            Int32 cellsUsed;
            alp->GetCellsUsed(&cellsUsed);
            alp->SetCellsUsed(++cellsUsed);
            alp->SetExpanded(TRUE);
            cellsRemaining--;
        }

        needsExpansion = TRUE;
    }

    // Divide any space left that wouldn't divide along cell boundaries
    // evenly among the smallest items

    const Boolean singleItem = !hasOverflow && visibleItemCount == 1;
    if (cellsRemaining > 0 && smallestItemsAt != 0 &&
            (cellsRemaining < visibleItemCount - 1 || singleItem || maxCellsUsed > 1)) {
        Float expandCount = (Int32)BitCount(smallestItemsAt);

        if (!singleItem) {
            // The items at the far edges may only expand by half in order to pin to either side.
            if ((smallestItemsAt & 1) != 0) {
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);

                Boolean preventEdgeOffset;
                alp->GetPreventEdgeOffset(&preventEdgeOffset);
                if (!preventEdgeOffset) expandCount -= 0.5f;
            }

            if ((smallestItemsAt & (1 << (childCount - 1))) != 0) {
                AutoPtr<IView> preChild;
                GetChildAt(childCount - 1, (IView**)&preChild);
                AutoPtr<IViewGroupLayoutParams> preLp;
                preChild->GetLayoutParams((IViewGroupLayoutParams**)&preLp);
                AutoPtr<IActionMenuViewLayoutParams> preAlp = IActionMenuViewLayoutParams::Probe(preLp);

                Boolean preventEdgeOffset;
                preAlp->GetPreventEdgeOffset(&preventEdgeOffset);
                if (!preventEdgeOffset) expandCount -= 0.5f;
            }
        }

        const Int32 extraPixels = expandCount > 0 ?
                (Int32) (cellsRemaining * cellSize / expandCount) : 0;

        for (Int32 i = 0; i < childCount; i++) {
            if ((smallestItemsAt & (1 << i)) == 0) continue;
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);
            AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(lp);
            Boolean isOverflow;
            if (IActionMenuItemView::Probe(child) != NULL) {
                // If this is one of our views, expand and measure at the larger size.
                alp->SetExtraPixels(extraPixels);
                alp->SetExpanded(TRUE);
                Boolean preventEdgeOffset;
                alp->GetPreventEdgeOffset(&preventEdgeOffset);
                if (i == 0 && !preventEdgeOffset) {
                    // First item getsAutoPtr<IView> child = GetChildAt(i); part of its new padding pushed out of sight.
                    // The last item will get this implicitly from layout.
                    mlp->SetLeftMargin(-extraPixels / 2);
                }

                needsExpansion = TRUE;

            } else if (alp->GetIsOverflowButton(&isOverflow), isOverflow) {
                alp->SetExtraPixels(extraPixels);
                alp->SetExpanded(TRUE);
                mlp->SetRightMargin(-extraPixels / 2);
                needsExpansion = TRUE;
            } else {
                // If we don't know what it is, give it some margins instead
                // and let it center within its space. We still want to pin
                // against the edges.
                if (i != 0) {
                    mlp->SetLeftMargin(extraPixels / 2);
                }
                if (i != childCount - 1) {
                    mlp->SetRightMargin(extraPixels / 2);
                }
            }
        }

        cellsRemaining = 0;
    }

    // Remeasure any items that have had extra space allocated to them.
    if (needsExpansion) {
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);

            Boolean expanded;
            alp->GetExpanded(&expanded);
            if (!expanded) continue;

            Int32 useds;
            alp->GetCellsUsed(&useds);
            Int32 extraPixels;
            alp->GetExtraPixels(&extraPixels);
            const Int32 width = useds * cellSize + extraPixels;
            child->Measure(MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY),
                    itemHeightSpec);
        }
    }

    if (heightMode != MeasureSpec::EXACTLY) {
        heightSize = maxChildHeight;
    }

    SetMeasuredDimension(widthSize, heightSize);
    mMeasuredExtraWidth = cellsRemaining * cellSize;
}

Int32 ActionMenuView::MeasureChildForCells(
    /* [in] */ IView* child,
    /* [in] */ Int32 cellSize,
    /* [in] */ Int32 cellsRemaining,
    /* [in] */ Int32 parentHeightMeasureSpec,
    /* [in] */ Int32 parentHeightPadding)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);

    const Int32 childHeightSize = MeasureSpec::GetSize(parentHeightMeasureSpec) -
            parentHeightPadding;
    const Int32 childHeightMode = MeasureSpec::GetMode(parentHeightMeasureSpec);
    const Int32 childHeightSpec = MeasureSpec::MakeMeasureSpec(childHeightSize, childHeightMode);

    AutoPtr<IActionMenuItemView> itemView = IActionMenuItemView::Probe(child) != NULL ?
            IActionMenuItemView::Probe(child) : NULL;

    Boolean hasText = FALSE;
    if(itemView != NULL)
        itemView->HasText(&hasText);

    Int32 cellsUsed = 0;
    if (cellsRemaining > 0 && (!hasText || cellsRemaining >= 2)) {
        const Int32 childWidthSpec = MeasureSpec::MakeMeasureSpec(
                cellSize * cellsRemaining, MeasureSpec::AT_MOST);
        child->Measure(childWidthSpec, childHeightSpec);

        Int32 measuredWidth = 0;
        child->GetMeasuredWidth(&measuredWidth);
        cellsUsed = measuredWidth / cellSize;
        if (measuredWidth % cellSize != 0) cellsUsed++;
        if (hasText && cellsUsed < 2) cellsUsed = 2;
    }

    Boolean overflow;
    const Boolean expandable = !(alp->GetIsOverflowButton(&overflow), overflow) && hasText;
    alp->SetExpandable(expandable);

    alp->SetCellsUsed(cellsUsed);
    const Int32 targetWidth = cellsUsed * cellSize;
    child->Measure(MeasureSpec::MakeMeasureSpec(targetWidth, MeasureSpec::EXACTLY),
            childHeightSpec);
    return cellsUsed;
}

ECode ActionMenuView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (!mFormatItems) {
        return LinearLayout::OnLayout(changed, left, top, right, bottom);
    }

    Int32 childCount;
    GetChildCount(&childCount);
    const Int32 midVertical = (top + bottom) / 2;
    Int32 dividerWidth;
    GetDividerWidth(&dividerWidth);
    Int32 overflowWidth = 0;
    Int32 nonOverflowWidth = 0;
    Int32 nonOverflowCount = 0;
    Int32 widthRemaining = right - left - mPaddingRight - mPaddingLeft;
    Boolean hasOverflow = FALSE;
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);

        Int32 visiable = 0;
        if ((v->GetVisibility(&visiable), visiable) == IView::GONE) {
            continue;
        }

        AutoPtr<IViewGroupLayoutParams> lp;
        v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);
        AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(lp);

        Boolean isOverflow;
        alp->GetIsOverflowButton(&isOverflow);
        if (isOverflow) {
            v->GetMeasuredWidth(&overflowWidth);
            if (HasDividerBeforeChildAt(i)) {
                overflowWidth += dividerWidth;
            }

            Int32 height = 0;
            v->GetMeasuredHeight(&height);
            Int32 r;
            Int32 l;
            if (isLayoutRtl) {
                Int32 leftMargin;
                mlp->GetLeftMargin(&leftMargin);
                l = mPaddingLeft + leftMargin;
                r = l + overflowWidth;
            } else {
                Int32 rightMargin;
                mlp->GetRightMargin(&rightMargin);
                Int32 w;
                GetWidth(&w);
                r = w - mPaddingRight - rightMargin;
                l = r - overflowWidth;
            }

            Int32 t = midVertical - (height / 2);
            Int32 b = t + height;
            v->Layout(l, t, r, b);

            widthRemaining -= overflowWidth;
            hasOverflow = TRUE;
        } else {
            Int32 mW = 0;
            v->GetMeasuredWidth(&mW);
            Int32 leftMargin, rightMargin;
            mlp->GetLeftMargin(&leftMargin);
            mlp->GetRightMargin(&rightMargin);
            const Int32 size = mW + leftMargin + rightMargin;
            nonOverflowWidth += size;
            widthRemaining -= size;
            if (HasDividerBeforeChildAt(i)) {
                nonOverflowWidth += dividerWidth;
            }
            nonOverflowCount++;
        }
    }

    if (childCount == 1 && !hasOverflow) {
        // Center a single child
        AutoPtr<IView> v;
        GetChildAt(0, (IView**)&v);
        Int32 width = 0;
        v->GetMeasuredWidth(&width);

        Int32 height = 0;
        v->GetMeasuredHeight(&height);

        const Int32 midHorizontal = (right - left) / 2;
        const Int32 l = midHorizontal - width / 2;
        const Int32 t = midVertical - height / 2;
        return v->Layout(l, t, l + width, t + height);
    }

    const Int32 spacerCount = nonOverflowCount - (hasOverflow ? 0 : 1);
    const Int32 spacerSize = Elastos::Core::Math::Max(0, spacerCount > 0 ? widthRemaining / spacerCount : 0);

    if (isLayoutRtl) {
        Int32 w;
        GetWidth(&w);
        Int32 startRight = w - mPaddingRight;
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            AutoPtr<IViewGroupLayoutParams> lp;
            v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IActionMenuViewLayoutParams> alp = IActionMenuViewLayoutParams::Probe(lp);
            AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(lp);

            Int32 visiable = 0;
            Boolean isOverflow;
            alp->GetIsOverflowButton(&isOverflow);
            if ((v->GetVisibility(&visiable), visiable) == IView::GONE || isOverflow) {
                continue;
            }

            Int32 rightMargin;
            mlp->GetRightMargin(&rightMargin);
            startRight -= rightMargin;
            Int32 width = 0;
            v->GetMeasuredWidth(&width);

            Int32 height = 0;
            v->GetMeasuredHeight(&height);

            Int32 t = midVertical - height / 2;
            v->Layout(startRight - width, t, startRight, t + height);
            Int32 leftMargin;
            mlp->GetRightMargin(&leftMargin);
            startRight -= width + leftMargin + spacerSize;
        }
    } else {
        Int32 startLeft = mPaddingLeft;
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            AutoPtr<IViewGroupLayoutParams> tmp;
            v->GetLayoutParams((IViewGroupLayoutParams**)&tmp);
            AutoPtr<IActionMenuViewLayoutParams> lp = IActionMenuViewLayoutParams::Probe(tmp);
            AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(tmp);

            Int32 visiable = 0;
            Boolean isOverflow;
            lp->GetIsOverflowButton(&isOverflow);
            if ((v->GetVisibility(&visiable), visiable) == IView::GONE || isOverflow) {
                continue;
            }

            Int32 leftMargin;
            mlp->GetRightMargin(&leftMargin);
            startLeft += leftMargin;
            Int32 width = 0;
            v->GetMeasuredWidth(&width);

            Int32 height = 0;
            v->GetMeasuredHeight(&height);

            Int32 t = midVertical - height / 2;
            v->Layout(startLeft, t, startLeft + width, t + height);
            Int32 rightMargin;
            mlp->GetRightMargin(&rightMargin);
            startLeft += width + rightMargin + spacerSize;
        }
    }
    return NOERROR;
}

ECode ActionMenuView::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    Boolean rst;
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL)
        presenter->DismissPopupMenus(&rst);
    return NOERROR;
}

ECode ActionMenuView::IsOverflowReserved(
    /* [out */ Boolean* reserveOverflow)
{
    *reserveOverflow = mReserveOverflow;
    return NOERROR;
}

ECode ActionMenuView::SetOverflowReserved(
    /* [in] */ Boolean reserveOverflow)
{
    mReserveOverflow = reserveOverflow;
    return NOERROR;
}

ECode ActionMenuView::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** lp)
{
    VALIDATE_NOT_NULL(lp)

    AutoPtr<IActionMenuViewLayoutParams> amvlp;
    CActionMenuViewLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT, (IActionMenuViewLayoutParams**)&amvlp);
    ILinearLayoutLayoutParams::Probe(amvlp)->SetGravity(IGravity::CENTER_VERTICAL);
    *lp = IViewGroupLayoutParams::Probe(amvlp);
    REFCOUNT_ADD(*lp);
    return NOERROR;
}

ECode ActionMenuView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IActionMenuViewLayoutParams> lp;
    FAIL_RETURN(CActionMenuViewLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IActionMenuViewLayoutParams**)&lp));

    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> ActionMenuView::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    if (p != NULL) {
        AutoPtr<IActionMenuViewLayoutParams> result;
        if(IActionMenuViewLayoutParams::Probe(p) != NULL) {
            CActionMenuViewLayoutParams::New(IActionMenuViewLayoutParams::Probe(p), (IActionMenuViewLayoutParams**)&result);
        }
        else {
            CActionMenuViewLayoutParams::New(p, (IActionMenuViewLayoutParams**)&result);
        }

        AutoPtr<ILinearLayoutLayoutParams> llp = ILinearLayoutLayoutParams::Probe(result);

        Int32 gravity;
        llp->GetGravity(&gravity);
        if (gravity <= IGravity::NO_GRAVITY) {
            llp->SetGravity(IGravity::CENTER_VERTICAL);
        }
        AutoPtr<IViewGroupLayoutParams> vlp = IViewGroupLayoutParams::Probe(result);
        return vlp;
    }

    AutoPtr<IViewGroupLayoutParams> ret;
    GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&ret);
    return ret;
}

Boolean ActionMenuView::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return p != NULL && IActionMenuViewLayoutParams::Probe(p) != NULL;
}

ECode ActionMenuView::GenerateOverflowButtonLayoutParams(
    /* [out] */ IActionMenuViewLayoutParams** amlp)
{
    VALIDATE_NOT_NULL(amlp)
    AutoPtr<IViewGroupLayoutParams> lp;
    GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&lp);
    *amlp = IActionMenuViewLayoutParams::Probe(lp);
    REFCOUNT_ADD(*amlp)
    return NOERROR;
}

ECode ActionMenuView::InvokeItem(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mMenu->PerformItemAction(IMenuItem::Probe(item), 0, result);
}

ECode ActionMenuView::GetWindowAnimations(
    /* [out] */ Int32* animations)
{
    VALIDATE_NOT_NULL(animations);

    *animations = 0;
    return NOERROR;
}

ECode ActionMenuView::Initialize(
    /* [in] */ IMenuBuilder* menu)
{
    mMenu = menu;
    return NOERROR;
}

ECode ActionMenuView::GetMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    if (mMenu == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CMenuBuilder::New(context, (IMenuBuilder**)&mMenu);
        AutoPtr<MenuBuilderCallback> cb = new MenuBuilderCallback(this);
        mMenu->SetCallback(cb);
        AutoPtr<IActionMenuPresenter> tmp;
        CActionMenuPresenter::New(context, (IActionMenuPresenter**)&tmp);
        mPresenter = NULL;
        IWeakReferenceSource::Probe(tmp)->GetWeakReference((IWeakReference**)&mPresenter);
        tmp->SetReserveOverflow(TRUE);

        AutoPtr<IMenuPresenterCallback> acb = new ActionMenuPresenterCallback();
        IMenuPresenter::Probe(tmp)->SetCallback(mActionMenuPresenterCallback != NULL
                ? mActionMenuPresenterCallback : acb);
        mMenu->AddMenuPresenter(IMenuPresenter::Probe(tmp), mPopupContext);
        tmp->SetMenuView(this);
    }

    *menu = IMenu::Probe(mMenu);
    REFCOUNT_ADD(*menu)
    return NOERROR;
}

    /**
     * Must be called before the first call to getMenu()
     * @hide
     */
ECode ActionMenuView::SetMenuCallbacks(
    /* [in] */ IMenuPresenterCallback* pcb,
    /* [in] */ IMenuBuilderCallback* mcb)
{
    mActionMenuPresenterCallback = pcb;
    mMenuBuilderCallback = mcb;
    return NOERROR;
}

    /**
     * Returns the current menu or null if one has not yet been configured.
     * @hide Internal use only for action bar integration
     */
ECode ActionMenuView::PeekMenu(
    /* [out] */ IMenuBuilder** build)
{
    VALIDATE_NOT_NULL(build)
    *build = mMenu;
    REFCOUNT_ADD(*build)
    return NOERROR;
}

    /**
     * Show the overflow items from the associated menu.
     *
     * @return true if the menu was able to be shown, false otherwise
     */
ECode ActionMenuView::ShowOverflowMenu(
    /* [out] */ Boolean* showOverflowMenu)
{
    VALIDATE_NOT_NULL(showOverflowMenu)
    *showOverflowMenu = FALSE;
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        presenter->ShowOverflowMenu(showOverflowMenu);
    }
    return NOERROR;
}

    /**
     * Hide the overflow items from the associated menu.
     *
     * @return true if the menu was able to be hidden, false otherwise
     */
ECode ActionMenuView::HideOverflowMenu(
    /* [out] */ Boolean* hideOverflowMenu)
{
    VALIDATE_NOT_NULL(hideOverflowMenu)
    *hideOverflowMenu = FALSE;
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        presenter->HideOverflowMenu(hideOverflowMenu);
    }
    return NOERROR;
}

    /**
     * Check whether the overflow menu is currently showing. This may not reflect
     * a pending show operation in progress.
     *
     * @return true if the overflow menu is currently showing
     */
ECode ActionMenuView::IsOverflowMenuShowing(
    /* [out] */ Boolean* isOverflowMenuShowing)
{
    VALIDATE_NOT_NULL(isOverflowMenuShowing)
    *isOverflowMenuShowing = FALSE;
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        presenter->IsOverflowMenuShowing(isOverflowMenuShowing);
    }
    return NOERROR;
}

    /** @hide */
ECode ActionMenuView::IsOverflowMenuShowPending(
    /* [out] */ Boolean* isOverflowMenuShowPending)
{
    VALIDATE_NOT_NULL(isOverflowMenuShowPending)
    *isOverflowMenuShowPending = FALSE;
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        presenter->IsOverflowMenuShowPending(isOverflowMenuShowPending);
    }
    return NOERROR;
}


    /**
     * Dismiss any popups associated with this menu view.
     */
ECode ActionMenuView::DismissPopupMenus()
{
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        Boolean result;
        return presenter->DismissPopupMenus(&result);
    }
    return NOERROR;
}
    /** @hide */
ECode ActionMenuView::SetExpandedActionViewsExclusive(
    /* [in] */ Boolean exclusive)
{
    AutoPtr<IActionMenuPresenter> presenter = GetPresenter();
    if (presenter != NULL) {
        return presenter->SetExpandedActionViewsExclusive(exclusive);
    }
    return NOERROR;
}

Boolean ActionMenuView::HasDividerBeforeChildAt(
    /* [in] */ Int32 childIndex)
{
    if (childIndex == 0) {
        return FALSE;
    }

    AutoPtr<IView> childBefore;
    GetChildAt(childIndex - 1, (IView**)&childBefore);
    AutoPtr<IView> child;
    GetChildAt(childIndex, (IView**)&child);
    Boolean result = FALSE;
    Int32 childCount;
    GetChildCount(&childCount);
    if (childIndex < childCount && IActionMenuChildView::Probe(childBefore) != NULL) {
        Boolean need = FALSE;
        IActionMenuChildView::Probe(childBefore)->NeedsDividerAfter(&need);
        result |= need;
    }
    if (childIndex > 0 && IActionMenuChildView::Probe(child) != NULL) {
        Boolean need = FALSE;
        IActionMenuChildView::Probe(child)->NeedsDividerBefore(&need);
        result |= need;
    }

    return result;
}

ECode ActionMenuView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

void ActionMenuView::InitActionMenu(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    SetBaselineAligned(FALSE);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    assert(metrics != NULL);

    Float density = 0.f;
    metrics->GetDensity(&density);

    mMinCellSize = (Int32) (MIN_CELL_SIZE * density);
    mGeneratedItemPadding = (Int32) (GENERATED_ITEM_PADDING * density);
    mPopupContext = context;
    mPopupTheme = 0;
}

AutoPtr<IActionMenuPresenter> ActionMenuView::GetPresenter()
{
    AutoPtr<IActionMenuPresenter> presenter;
    if (mPresenter != NULL) {
        AutoPtr<IInterface> obj;
        mPresenter->Resolve(EIID_IInterface, (IInterface**)&obj);
        presenter = IActionMenuPresenter::Probe(obj);
    }
    return presenter;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
