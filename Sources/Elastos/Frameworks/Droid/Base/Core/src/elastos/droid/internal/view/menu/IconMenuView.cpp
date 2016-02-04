
#include "Elastos.Droid.Content.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/view/menu/IconMenuView.h"
#include "elastos/droid/internal/view/menu/CIconMenuItemView.h"
#include "elastos/droid/internal/view/menu/MenuBuilder.h"
#include "elastos/droid/internal/view/menu/CIconMenuViewLayoutParams.h"
#include "elastos/droid/internal/view/menu/CIconMenuViewSavedState.h"
#include "elastos/droid/internal/view/menu/MenuItemImpl.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL(IconMenuView::OnClickListener, Object, IViewOnClickListener)

IconMenuView::OnClickListener::OnClickListener(
    /* [in] */ IconMenuView* host)
    : mHost(host)
{}

ECode IconMenuView::OnClickListener::OnClick(
    /* [in] */ IView* v)
{
    // Switches the menu to expanded mode. Requires support from
    // the menu's active callback.
    mHost->mMenu->ChangeMenuMode();
    return NOERROR;
}

CAR_INTERFACE_IMPL(IconMenuView::SavedState, View::BaseSavedState, IIconMenuViewSavedState)
IconMenuView::SavedState::SavedState()
    : mFocusedPosition(0)
{}

ECode IconMenuView::SavedState::constructor()
{
    return NOERROR;
}

ECode IconMenuView::SavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 focusedPosition)
{
    View::BaseSavedState::constructor(superState);
    mFocusedPosition = focusedPosition;
    return NOERROR;
}

ECode IconMenuView::SavedState::SetFocusedPosition(
    /* [in] */ Int32 focusedPosition)
{
    mFocusedPosition = focusedPosition;
    return NOERROR;
}

ECode IconMenuView::SavedState::GetFocusedPosition(
    /* [out] */ Int32* focusedPosition)
{
    *focusedPosition = mFocusedPosition;
    return NOERROR;
}

ECode IconMenuView::SavedState::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    View::BaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mFocusedPosition);
    return NOERROR;
}

ECode IconMenuView::SavedState::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    View::BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mFocusedPosition);
    return NOERROR;
}

CAR_INTERFACE_IMPL(IconMenuView::LayoutParams, MarginLayoutParams, IIconMenuViewLayoutParams)

IconMenuView::LayoutParams::LayoutParams()
    : mLeft(0)
    , mTop(0)
    , mRight(0)
    , mBottom(0)
    , mDesiredWidth(0)
    , mMaxNumItemsOnRow(0)
{}

ECode IconMenuView::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    return MarginLayoutParams::constructor(c, attrs);
}

ECode IconMenuView::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return MarginLayoutParams::constructor(width, height);
}

ECode IconMenuView::LayoutParams::SetLeft(
    /* [in] */ Int32 left)
{
    mLeft = left;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetLeft(
    /* [out] */ Int32* left)
{
    *left = mLeft;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::SetTop(
    /* [in] */ Int32 top)
{
    mTop = top;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetTop(
    /* [out] */ Int32* top)
{
    *top = mTop;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::SetRight(
    /* [in] */ Int32 right)
{
    mRight = right;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetRight(
    /* [out] */ Int32* right)
{
    *right = mRight;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::SetBottom(
    /* [in] */ Int32 bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetBottom(
    /* [out] */ Int32* bottom)
{
    *bottom = mBottom;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::SetDesiredWidth(
    /* [in] */ Int32 desiredWidth)
{
    mDesiredWidth = desiredWidth;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetDesiredWidth(
    /* [out] */ Int32* desiredWidth)
{
    *desiredWidth = mDesiredWidth;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::SetMaxNumItemsOnRow(
    /* [in] */ Int32 maxNumItemsOnRow)
{
    mMaxNumItemsOnRow = maxNumItemsOnRow;
    return NOERROR;
}

ECode IconMenuView::LayoutParams::GetMaxNumItemsOnRow(
    /* [out] */ Int32* maxNumItemsOnRow)
{
    *maxNumItemsOnRow = mMaxNumItemsOnRow;
    return NOERROR;
}

const Int32 IconMenuView::ITEM_CAPTION_CYCLE_DELAY;

CAR_INTERFACE_IMPL_4(IconMenuView, ViewGroup, IIconMenuView,
    IMenuBuilderItemInvoker, IMenuView, IRunnable)

IconMenuView::IconMenuView()
    : mRowHeight(0)
    , mMaxRows(0)
    , mMaxItems(0)
    , mMaxItemsPerRow(0)
    , mNumActualItemsShown(0)
    , mHorizontalDividerHeight(0)
    , mVerticalDividerWidth(0)
    , mAnimations(0)
    , mHasStaleChildren(FALSE)
    , mMenuBeingLongpressed(FALSE)
    , mLastChildrenCaptionMode(FALSE)
    , mLayoutNumRows(0)
{}

ECode IconMenuView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::IconMenuView),
        ArraySize(R::styleable::IconMenuView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);

    a->GetDimensionPixelSize(R::styleable::IconMenuView_rowHeight, 64, &mRowHeight);
    a->GetInt32(R::styleable::IconMenuView_maxRows, 2, &mMaxRows);
    a->GetInt32(R::styleable::IconMenuView_maxItems, 6, &mMaxItems);
    a->GetInt32(R::styleable::IconMenuView_maxItemsPerRow, 3, &mMaxItemsPerRow);
    a->GetDrawable(R::styleable::IconMenuView_moreIcon, (IDrawable**)&mMoreIcon);
    a->Recycle();

    a = NULL;
    attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::MenuView),
            ArraySize(R::styleable::MenuView));
    context->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);

    a->GetDrawable(R::styleable::MenuView_itemBackground, (IDrawable**)&mItemBackground);
    a->GetDrawable(R::styleable::MenuView_horizontalDivider, (IDrawable**)&mHorizontalDivider);
    a->GetDrawable(R::styleable::MenuView_verticalDivider, (IDrawable**)&mVerticalDivider);
    a->GetResourceId(R::styleable::MenuView_windowAnimationStyle, 0, &mAnimations);
    a->Recycle();

    if (mHorizontalDivider != NULL) {
        mHorizontalDivider->GetIntrinsicHeight(&mHorizontalDividerHeight);
        // Make sure to have some height for the divider
        if (mHorizontalDividerHeight == -1) mHorizontalDividerHeight = 1;
    }

    if (mVerticalDivider != NULL) {
        mVerticalDivider->GetIntrinsicWidth(&mVerticalDividerWidth);
        // Make sure to have some width for the divider
        if (mVerticalDividerWidth == -1) mVerticalDividerWidth = 1;
    }

    mLayout = ArrayOf<Int32>::Alloc(mMaxRows);

    // This view will be drawing the dividers
    SetWillNotDraw(FALSE);

    // This is so we'll receive the MENU key in touch mode
    SetFocusableInTouchMode(TRUE);
    // This is so our children can still be arrow-key focused
    SetDescendantFocusability(FOCUS_AFTER_DESCENDANTS);

    return NOERROR;
}

Int32 IconMenuView::GetMaxItems()
{
    return mMaxItems;
}

void IconMenuView::LayoutItems(
    /* [in] */ Int32 width)
{
    Int32 numItems;
    ViewGroup::GetChildCount(&numItems);
    if (numItems == 0) {
        mLayoutNumRows = 0;
        return;
    }

    // Start with the least possible number of rows
    Int32 curNumRows = Elastos::Core::Math::Min(
        (Int32)Elastos::Core::Math::Ceil(numItems / (Float)mMaxItemsPerRow), mMaxRows);

    /*
     * Increase the number of rows until we find a configuration that fits
     * all of the items' titles. Worst case, we use mMaxRows.
     */
    for (; curNumRows <= mMaxRows; curNumRows++) {
        LayoutItemsUsingGravity(curNumRows, numItems);

        if (curNumRows >= numItems) {
            // Can't have more rows than items
            break;
        }

        if (DoItemsFit()) {
            // All the items fit, so this is a good configuration
            break;
        }
    }
}

void IconMenuView::LayoutItemsUsingGravity(
    /* [in] */ Int32 numRows,
    /* [in] */ Int32 numItems)
{
    Int32 numBaseItemsPerRow = numItems / numRows;
    Int32 numLeftoverItems = numItems % numRows;
    /**
     * The bottom rows will each get a leftover item. Rows (indexed at 0)
     * that are >= this get a leftover item. Note: if there are 0 leftover
     * items, no rows will get them since this value will be greater than
     * the last row.
     */
    Int32 rowsThatGetALeftoverItem = numRows - numLeftoverItems;

    for (Int32 i = 0; i < numRows; i++) {
        (*mLayout)[i] = numBaseItemsPerRow;

        // Fill the bottom rows with a leftover item each
        if (i >= rowsThatGetALeftoverItem) {
            (*mLayout)[i]++;
        }
    }

    mLayoutNumRows = numRows;
}

Boolean IconMenuView::DoItemsFit()
{
    Int32 itemPos = 0;

    Int32 numRows = mLayoutNumRows;
    for (Int32 row = 0; row < numRows; row++) {
        Int32 numItemsOnRow = (*mLayout)[row];

        /*
         * If there is only one item on this row, increasing the
         * number of rows won't help.
         */
        if (numItemsOnRow == 1) {
            itemPos++;
            continue;
        }

        for (Int32 itemsOnRowCounter = numItemsOnRow; itemsOnRowCounter > 0;
                itemsOnRowCounter--) {
            AutoPtr<IView> child;
            ViewGroup::GetChildAt(itemPos++, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> _lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
            AutoPtr<IIconMenuViewLayoutParams> lp = IIconMenuViewLayoutParams::Probe(_lp);
            Int32 maxNumItemsOnRow;
            lp->GetMaxNumItemsOnRow(&maxNumItemsOnRow);
            if (maxNumItemsOnRow < numItemsOnRow) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

AutoPtr<IDrawable> IconMenuView::GetItemBackgroundDrawable()
{
    AutoPtr<IDrawableConstantState> state;
    mItemBackground->GetConstantState((IDrawableConstantState**)&state);
    assert(state != NULL);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IDrawable> drawable;
    state->NewDrawable(res, (IDrawable**)&drawable);
    return drawable;
}

AutoPtr<IIconMenuItemView> IconMenuView::CreateMoreItemView()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::icon_menu_item_layout, NULL, (IView**)&view);
    AutoPtr<IIconMenuItemView> itemView = IIconMenuItemView::Probe(view);

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<ICharSequence> text;
    r->GetText(R::string::more_item_label, (ICharSequence**)&text);
    ((IconMenuItemView*)itemView.Get())->Initialize(text, mMoreIcon);

    // Set up a click listener on the view since there will be no invocation sequence
    // due to the lack of a MenuItemData this view
    AutoPtr<IViewOnClickListener> onClick = new OnClickListener(this);
    IView::Probe(itemView)->SetOnClickListener(onClick);

    return itemView;
}

ECode IconMenuView::Initialize(
    /* [in] */ IMenuBuilder* menu)
{
    mMenu = menu;
    return NOERROR;
}

void IconMenuView::PositionChildren(
    /* [in] */ Int32 menuWidth,
    /* [in] */ Int32 menuHeight)
{
    // Clear the containers for the positions where the dividers should be drawn
    if (mHorizontalDivider != NULL) mHorizontalDividerRects.Clear();
    if (mVerticalDivider != NULL) mVerticalDividerRects.Clear();

    // Get the minimum number of rows needed
    const Int32 numRows = mLayoutNumRows;
    const Int32 numRowsMinus1 = numRows - 1;
    const ArrayOf<Int32>* numItemsForRow = mLayout;

    // The item position across all rows
    Int32 itemPos = 0;
    AutoPtr<IView> child;
    AutoPtr<IIconMenuViewLayoutParams> lp;

    // Use float for this to get precise positions (uniform item widths
    // instead of last one taking any slack), and then convert to ints at last opportunity
    Float itemLeft;
    Float itemTop = 0;
    // Since each row can have a different number of items, this will be computed per row
    Float itemWidth;
    // Subtract the space needed for the horizontal dividers
    Float itemHeight = (menuHeight - mHorizontalDividerHeight * (numRows - 1)) / (Float)numRows;

    for (Int32 row = 0; row < numRows; row++) {
        // Start at the left
        itemLeft = 0;

        // Subtract the space needed for the vertical dividers, and divide by the number of items
        itemWidth = (menuWidth - mVerticalDividerWidth * ((*numItemsForRow)[row] - 1))
                / (Float)(*numItemsForRow)[row];

        for (Int32 itemPosOnRow = 0; itemPosOnRow < (*numItemsForRow)[row]; itemPosOnRow++) {
            // Tell the child to be exactly this size
            child = NULL;
            GetChildAt(itemPos, (IView**)&child);
            child->Measure(MeasureSpec::MakeMeasureSpec((Int32)itemWidth, MeasureSpec::EXACTLY),
                    MeasureSpec::MakeMeasureSpec((Int32)itemHeight, MeasureSpec::EXACTLY));

            // Remember the child's position for layout
            AutoPtr<IViewGroupLayoutParams> _lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
            lp = IIconMenuViewLayoutParams::Probe(_lp);

            lp->SetLeft((Int32)itemLeft);
            lp->SetRight((Int32)(itemLeft + itemWidth));
            lp->SetTop((Int32)itemTop);
            lp->SetBottom((Int32)(itemTop + itemHeight));

            // Increment by item width
            itemLeft += itemWidth;
            itemPos++;

            // Add a vertical divider to draw
            if (mVerticalDivider != NULL) {
                AutoPtr<IRect> rect;
                CRect::New((Int32)itemLeft,
                        (Int32)itemTop, (Int32)(itemLeft + mVerticalDividerWidth),
                        (Int32)(itemTop + itemHeight), (IRect**)&rect);
                mVerticalDividerRects.PushBack(rect);
            }

            // Increment by divider width (even if we're not computing
            // dividers, since we need to leave room for them when
            // calculating item positions)
            itemLeft += mVerticalDividerWidth;
        }

        // Last child on each row should extend to very right edge
        if (lp != NULL) {
            lp->SetRight(menuWidth);
        }

        itemTop += itemHeight;

        // Add a horizontal divider to draw
        if ((mHorizontalDivider != NULL) && (row < numRowsMinus1)) {
            AutoPtr<IRect> rect;
            CRect::New(0, (Int32)itemTop, menuWidth,
                    (Int32)(itemTop + mHorizontalDividerHeight), (IRect**)&rect);
            mHorizontalDividerRects.PushBack(rect);

            itemTop += mHorizontalDividerHeight;
        }
    }
}

void IconMenuView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 measuredWidth = ResolveSize(Elastos::Core::Math::INT32_MAX_VALUE, widthMeasureSpec);
    CalculateItemFittingMetadata(measuredWidth);
    LayoutItems(measuredWidth);

    // Get the desired height of the icon menu view (last row of items does
    // not have a divider below)
    const Int32 layoutNumRows = mLayoutNumRows;
    const Int32 desiredHeight = (mRowHeight + mHorizontalDividerHeight) *
            layoutNumRows - mHorizontalDividerHeight;

    // Maximum possible width and desired height
    ViewGroup::SetMeasuredDimension(measuredWidth,
            ResolveSize(desiredHeight, heightMeasureSpec));

    // Position the children
    if (layoutNumRows > 0) {
        Int32 w, h;
        GetMeasuredWidth(&w);
        GetMeasuredHeight(&h);
        PositionChildren(w, h);
    }
}

ECode IconMenuView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AutoPtr<IView> child;
    Int32 childCount;
    ViewGroup::GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        child = NULL;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        AutoPtr<IIconMenuViewLayoutParams> lp = IIconMenuViewLayoutParams::Probe(_lp);

        Int32 left, right, top, bottom;
        lp->GetLeft(&left);
        lp->GetRight(&right);
        lp->GetTop(&top);
        lp->GetBottom(&bottom);

        // Layout children according to positions set during the measure
        child->Layout(left, top, right, bottom);
    }
    return NOERROR;
}

void IconMenuView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> drawable = mHorizontalDivider;
    if (drawable != NULL) {
        // If we have a horizontal divider to draw, draw it at the remembered positions
        List<AutoPtr<IRect> >::ReverseIterator riter = mHorizontalDividerRects.RBegin();
        for (; riter != mHorizontalDividerRects.REnd(); ++riter) {
            IRect* rect = *riter;
            drawable->SetBounds(rect);
            drawable->Draw(canvas);
        }
    }

    drawable = mVerticalDivider;
    if (drawable != NULL) {
        // If we have a vertical divider to draw, draw it at the remembered positions
        List<AutoPtr<IRect> >::ReverseIterator riter = mVerticalDividerRects.RBegin();
        for (; riter != mVerticalDividerRects.REnd(); ++riter) {
            IRect* rect = *riter;
            drawable->SetBounds(rect);
            drawable->Draw(canvas);
        }
    }
}

ECode IconMenuView::InvokeItem(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* state)
{
    return mMenu->PerformItemAction(IMenuItem::Probe(item), 0, state);
}

ECode IconMenuView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIconMenuViewLayoutParams> lp;
    FAIL_RETURN(CIconMenuViewLayoutParams::New(context, attrs, (IIconMenuViewLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

Boolean IconMenuView::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    // Override to allow type-checking of LayoutParams.
    return IIconMenuViewLayoutParams::Probe(p) != NULL;
}

void IconMenuView::MarkStaleChildren()
{
    if (!mHasStaleChildren) {
        mHasStaleChildren = TRUE;
        RequestLayout();
    }
}

Int32 IconMenuView::GetNumActualItemsShown()
{
    return mNumActualItemsShown;
}

void IconMenuView::SetNumActualItemsShown(
    /* [in] */ Int32 count)
{
    mNumActualItemsShown = count;
}

ECode IconMenuView::GetWindowAnimations(
    /* [out] */ Int32* animations)
{
    VALIDATE_NOT_NULL(animations)
    *animations = mAnimations;
    return NOERROR;
}

ECode IconMenuView::GetLayout(
    /* [out, callee] */ ArrayOf<Int32>** layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = mLayout;
    REFCOUNT_ADD(*layout);
    return NOERROR;
}

ECode IconMenuView::GetLayoutNumRows(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    *num = mLayoutNumRows;
    return NOERROR;
}

ECode IconMenuView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    if (keyCode == IKeyEvent::KEYCODE_MENU) {
        Int32 action, repeatCount;
        event->GetAction(&action);
        event->GetRepeatCount(&repeatCount);
        if (action == IKeyEvent::ACTION_DOWN && repeatCount == 0) {
            Boolean res;
            RemoveCallbacks(this, &res);
            PostDelayed(this, CViewConfiguration::GetLongPressTimeout(), &res);
        }
        else if (action == IKeyEvent::ACTION_UP) {
            if (mMenuBeingLongpressed) {
                // It was in cycle mode, so reset it (will also remove us
                // from being called back)
                SetCycleShortcutCaptionMode(FALSE);
                *result = TRUE;
                return NOERROR;
            }
            else {
                // Just remove us from being called back
                Boolean res;
                RemoveCallbacks(this, &res);
                // Fall through to normal processing too
            }
        }
    }

    return ViewGroup::DispatchKeyEvent(event, result);
}

ECode IconMenuView::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();

    Boolean res;
    RequestFocus(&res);
    return NOERROR;
}

ECode IconMenuView::OnDetachedFromWindow()
{
    SetCycleShortcutCaptionMode(FALSE);
    return ViewGroup::OnDetachedFromWindow();
}

ECode IconMenuView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    if (!hasWindowFocus) {
        SetCycleShortcutCaptionMode(FALSE);
    }

    return ViewGroup::OnWindowFocusChanged(hasWindowFocus);
}

void IconMenuView::SetCycleShortcutCaptionMode(
    /* [in] */ Boolean cycleShortcutAndNormal)
{
    if (!cycleShortcutAndNormal) {
        /*
         * We're setting back to title, so remove any callbacks for setting
         * to shortcut
         */
        Boolean res;
        RemoveCallbacks(this, &res);
        SetChildrenCaptionMode(FALSE);
        mMenuBeingLongpressed = FALSE;

    } else {
        // Set it the first time (the cycle will be started in run()).
        SetChildrenCaptionMode(TRUE);
    }
}

ECode IconMenuView::Run()
{
    if (mMenuBeingLongpressed) {

        // Cycle to other caption mode on the children
        SetChildrenCaptionMode(!mLastChildrenCaptionMode);
    }
    else {
        // Switch ourselves to continuously cycle the items captions
        mMenuBeingLongpressed = TRUE;
        SetCycleShortcutCaptionMode(TRUE);
    }

    // We should run again soon to cycle to the other caption mode
    Boolean res;
    PostDelayed(this, ITEM_CAPTION_CYCLE_DELAY, &res);

    return NOERROR;
}

void IconMenuView::SetChildrenCaptionMode(
    /* [in] */ Boolean shortcut)
{
    // Set the last caption mode pushed to children
    mLastChildrenCaptionMode = shortcut;

    Int32 childCount;
    ViewGroup::GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        ((IconMenuItemView*)IIconMenuItemView::Probe(view))->SetCaptionMode(shortcut);
    }
}

void IconMenuView::CalculateItemFittingMetadata(
    /* [in] */ Int32 width)
{
    Int32 maxNumItemsPerRow = mMaxItemsPerRow;
    Int32 numItems;
    ViewGroup::GetChildCount(&numItems);
    for (Int32 i = 0; i < numItems; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> _lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&_lp);
        AutoPtr<IIconMenuViewLayoutParams> lp = IIconMenuViewLayoutParams::Probe(_lp);

        // Start with 1, since that case does not get covered in the loop below
        lp->SetMaxNumItemsOnRow(1);
        for (Int32 curNumItemsPerRow = maxNumItemsPerRow; curNumItemsPerRow > 0;
                curNumItemsPerRow--) {
            // Check whether this item can fit into a row containing curNumItemsPerRow
            Int32 desiredWidth;
            lp->GetDesiredWidth(&desiredWidth);
            if (desiredWidth < width / curNumItemsPerRow) {
                // It can, mark this value as the most dense row it can fit into
                lp->SetMaxNumItemsOnRow(curNumItemsPerRow);
                break;
            }
        }
    }
}

AutoPtr<IParcelable> IconMenuView::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = ViewGroup::OnSaveInstanceState();

    AutoPtr<IView> focusedView;
    GetFocusedChild((IView**)&focusedView);
    AutoPtr<IParcelable> rst;
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        if (view == focusedView) {
            CIconMenuViewSavedState::New(superState, i, (IParcelable**)&rst);
            return rst;
        }
    }

    CIconMenuViewSavedState::New(superState, -1, (IParcelable**)&rst);
    return rst;

}

void IconMenuView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<IIconMenuViewSavedState> ss = IIconMenuViewSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    IAbsSavedState::Probe(ss)->GetSuperState((IParcelable**)&superState);

    Int32 focusedPosition;
    ss->GetFocusedPosition(&focusedPosition);
    Int32 childCount;
    GetChildCount(&childCount);
    if (focusedPosition >= childCount) {
        return;
    }

    AutoPtr<IView> v;
    GetChildAt(focusedPosition, (IView**)&v);
    if (v != NULL) {
        Boolean rst;
        v->RequestFocus(&rst);
    }
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
