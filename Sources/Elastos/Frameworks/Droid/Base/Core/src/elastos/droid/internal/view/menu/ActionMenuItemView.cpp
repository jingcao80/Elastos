
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/view/menu/ActionMenuItemView.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/Toast.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Droid::Widget::EIID_IActionMenuChildView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL_2(ActionMenuItemView::MyListener, Object, IViewOnClickListener, IViewOnLongClickListener)

ActionMenuItemView::MyListener::MyListener(
    /* [in] */ ActionMenuItemView* owner)
    : mOwner(owner)
{}

ECode ActionMenuItemView::MyListener::OnClick(
    /* [in] */ IView* v)
{
    return mOwner->OnClick(v);
}

ECode ActionMenuItemView::MyListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mOwner->OnLongClick(v, result);
}

ActionMenuItemView::ActionMenuItemForwardingListener::ActionMenuItemForwardingListener(
    /* [in] */ ActionMenuItemView* owner)
    : mOwner(owner)
{
    ListPopupWindow::ForwardingListener::constructor(owner);
}

ECode ActionMenuItemView::ActionMenuItemForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** window)
{
    VALIDATE_NOT_NULL(window)
    if (mOwner->mPopupCallback != NULL) {
        return mOwner->mPopupCallback->GetPopup(window);
    }
    *window = NULL;
    return NOERROR;
}

Boolean ActionMenuItemView::ActionMenuItemForwardingListener::OnForwardingStarted()
{
    Boolean res;
    // Call the invoker, then check if the expected popup is showing.
    if (mOwner->mItemInvoker != NULL &&
        (mOwner->mItemInvoker->InvokeItem(mOwner->mItemData, &res), res)) {
        AutoPtr<IListPopupWindow> popup;
        GetPopup((IListPopupWindow**)&popup);
        return popup != NULL && (popup->IsShowing(&res), res);
    }
    return FALSE;
}

Boolean ActionMenuItemView::ActionMenuItemForwardingListener::OnForwardingStopped()
{
    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    if (popup != NULL) {
        popup->Dismiss();
        return TRUE;
    }
    return FALSE;
}

String ActionMenuItemView::TAG("ActionMenuItemView");
const Int32 ActionMenuItemView::MAX_ICON_SIZE = 32; // dp

CAR_INTERFACE_IMPL_5(ActionMenuItemView, TextView, IActionMenuItemView, IMenuItemView,
    IViewOnClickListener, IViewOnLongClickListener, IActionMenuChildView)

ActionMenuItemView::ActionMenuItemView()
    : mAllowTextWithIcon(FALSE)
    , mExpandedFormat(FALSE)
    , mMinWidth(0)
    , mSavedPaddingLeft(0)
    , mMaxIconSize(0)
{
}

ECode ActionMenuItemView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ActionMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ActionMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ActionMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(TextView::constructor(context, attrs, defStyleAttr, defStyleRes))

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_allowActionMenuItemTextWithIcon, &mAllowTextWithIcon);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ActionMenuItemView);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetDimensionPixelSize(R::styleable::ActionMenuItemView_minWidth, 0, &mMinWidth);
    a->Recycle();

    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density = 0.f;
    metrics->GetDensity(&density);
    mMaxIconSize = (Int32) (MAX_ICON_SIZE * density + 0.5f);

    AutoPtr<MyListener> listener = new MyListener(this);
    SetOnClickListener(listener);
    SetOnLongClickListener(listener);

    mSavedPaddingLeft = -1;
    return NOERROR;
}

void ActionMenuItemView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    TextView::OnConfigurationChanged(newConfig);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_allowActionMenuItemTextWithIcon, &mAllowTextWithIcon);
    UpdateTextButtonVisibility();
}

ECode ActionMenuItemView::SetPadding(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    mSavedPaddingLeft = l;
    return TextView::SetPadding(l, t, r, b);
}

ECode ActionMenuItemView::GetItemData(
    /* [out] */ IMenuItemImpl** itemData)
{
    assert(itemData != NULL);
    *itemData = mItemData;
    REFCOUNT_ADD(*itemData);

    return NOERROR;
}

ECode ActionMenuItemView::Initialize(
    /* [in] */ IMenuItemImpl* _itemData,
    /* [in] */ Int32 menuType)
{
    mItemData = _itemData;
    IMenuItem* itemData = IMenuItem::Probe(_itemData);

    AutoPtr<IDrawable> icon;
    itemData->GetIcon((IDrawable**)&icon);
    SetIcon(icon);

    AutoPtr<ICharSequence> title;
    _itemData->GetTitleForItemView(this, (ICharSequence**)&title);
    SetTitle(title); // Title only takes effect if there is no icon

    Int32 id = 0;
    itemData->GetItemId(&id);
    SetId(id);

    Boolean visible = FALSE;
    itemData->IsVisible(&visible);
    SetVisibility(visible ? IView::VISIBLE : IView::GONE);

    Boolean enabled = FALSE;
    itemData->IsEnabled(&enabled);
    SetEnabled(enabled);

    Boolean hasSubMenu;
    if (itemData->HasSubMenu(&hasSubMenu), hasSubMenu) {
        if (mForwardingListener == NULL) {
            mForwardingListener = new ActionMenuItemForwardingListener(this);
        }
    }

    return NOERROR;
}

ECode ActionMenuItemView::OnTouchEvent(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean hasSubMenu;
    IMenuItem::Probe(mItemData)->HasSubMenu(&hasSubMenu);
    Boolean res;
    if (hasSubMenu && mForwardingListener != NULL
        && (mForwardingListener->OnTouch(this, e, &res), res)) {
        *result = TRUE;
        return NOERROR;
    }
    return TextView::OnTouchEvent(e, result);
}

ECode ActionMenuItemView::OnClick(
    /* [in] */ IView* v)
{
    if (mItemInvoker != NULL) {
        Boolean state = FALSE;
        return mItemInvoker->InvokeItem(mItemData, &state);
    }

    return NOERROR;
}

ECode ActionMenuItemView::SetItemInvoker(
    /* [in] */ IMenuBuilderItemInvoker* invoker)
{
    mItemInvoker = invoker;
    return NOERROR;
}

ECode ActionMenuItemView::SetPopupCallback(
    /* [in] */ IPopupCallback* popupCallback)
{
    mPopupCallback = popupCallback;
    return NOERROR;
}

ECode ActionMenuItemView::PrefersCondensedTitle(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = TRUE;
    return NOERROR;
}

ECode ActionMenuItemView::SetCheckable(
    /* [in] */ Boolean checkable)
{
    // TODO Support checkable action items
    return NOERROR;
}

ECode ActionMenuItemView::SetChecked(
    /* [in] */ Boolean checked)
{
    // TODO Support checkable action items
    return NOERROR;
}

ECode ActionMenuItemView::SetExpandedFormat(
    /* [in] */ Boolean expandedFormat)
{
    if (mExpandedFormat != expandedFormat) {
        mExpandedFormat = expandedFormat;
        if (mItemData != NULL) {
            return mItemData->ActionFormatChanged();
        }
    }

    return NOERROR;
}

void ActionMenuItemView::UpdateTextButtonVisibility()
{
    Boolean visible = !TextUtils::IsEmpty(mTitle);

    Boolean tmp = FALSE;
    visible &= mIcon == NULL ||
            ((mItemData->ShowsTextAsAction(&tmp), tmp) && (mAllowTextWithIcon || mExpandedFormat));

    SetText(visible ? mTitle : NULL);
}

ECode ActionMenuItemView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    if (icon != NULL) {
        Int32 width = 0;
        icon->GetIntrinsicWidth(&width);
        Int32 height = 0;
        icon->GetIntrinsicHeight(&height);

        if (width > mMaxIconSize) {
            Float scale = (Float) mMaxIconSize / width;
            width = mMaxIconSize;
            height *= scale;
        }
        if (height > mMaxIconSize) {
            Float scale = (Float) mMaxIconSize / height;
            height = mMaxIconSize;
            width *= scale;
        }

        icon->SetBounds(0, 0, width, height);
    }

    SetCompoundDrawables(icon, NULL, NULL, NULL);

    UpdateTextButtonVisibility();

    return NOERROR;
}

ECode ActionMenuItemView::HasText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    *result = !TextUtils::IsEmpty(text);
    return NOERROR;
}

ECode ActionMenuItemView::SetShortcut(
    /* [in] */ Boolean showShortcut,
    /* [in] */ Char32 shortcutKey)
{
    // Action buttons don't show text for shortcut keys.
    return NOERROR;
}

ECode ActionMenuItemView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;

    SetContentDescription(mTitle);
    UpdateTextButtonVisibility();

    return NOERROR;
}

ECode ActionMenuItemView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    OnPopulateAccessibilityEvent(event);
    *result = TRUE;
    return NOERROR;
}

ECode ActionMenuItemView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FAIL_RETURN(TextView::OnPopulateAccessibilityEvent(event))
    AutoPtr<ICharSequence> cdesc;
    GetContentDescription((ICharSequence**)&cdesc);
    if (!TextUtils::IsEmpty(cdesc)) {
        AutoPtr<IList> texts;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&texts);
        assert(texts != NULL);
        texts->Add(cdesc);
    }

    return NOERROR;
}

ECode ActionMenuItemView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Don't allow children to hover; we want this to be treated as a single component.
    return OnHoverEvent(event, result);
}

ECode ActionMenuItemView::ShowsIcon(
    /* [out] */ Boolean* shows)
{
    VALIDATE_NOT_NULL(shows)
    *shows = TRUE;
    return NOERROR;
}

ECode ActionMenuItemView::NeedsDividerBefore(
    /* [out] */ Boolean* need)
{
    VALIDATE_NOT_NULL(need)
    AutoPtr<IDrawable> icon;
    Boolean hasNext;
    *need = (HasText(&hasNext), hasNext)
        && (IMenuItem::Probe(mItemData)->GetIcon((IDrawable**)&icon), icon) == NULL;
    return NOERROR;
}

ECode ActionMenuItemView::NeedsDividerAfter(
    /* [out] */ Boolean* need)
{
    VALIDATE_NOT_NULL(need)
    return HasText(need);
}

ECode ActionMenuItemView::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean hasNext;
    if (HasText(&hasNext), hasNext) {
        // Don't show the cheat sheet for items that already show text.
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > screenPos;
    GetLocationOnScreen((ArrayOf<Int32>**)&screenPos);
    AutoPtr<IRect> displayFrame;
    CRect::New((IRect**)&displayFrame);
    GetWindowVisibleDisplayFrame(displayFrame);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);
    Int32 midy = (*screenPos)[1] + height / 2;
    Int32 referenceX = (*screenPos)[0] + width / 2;

    Int32 direct;
    v->GetLayoutDirection(&direct);
    if (direct == IView::LAYOUT_DIRECTION_LTR) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> metrics;
        res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        Int32 screenWidth = 0;
        metrics->GetWidthPixels(&screenWidth);
        referenceX = screenWidth - referenceX; // mirror
    }

    AutoPtr<ICharSequence> title;
    IMenuItem::Probe(mItemData)->GetTitle((ICharSequence**)&title);

    AutoPtr<IToast> cheatSheet;
    Toast::MakeText(context, title, IToast::LENGTH_SHORT, (IToast**)&cheatSheet);

    Int32 fHeight = 0;
    if (midy < (displayFrame->GetHeight(&fHeight), fHeight)) {
        // Show along the top; follow action buttons
        cheatSheet->SetGravity(IGravity::TOP | IGravity::END, referenceX, height);
    }
    else {
        // Show along the bottom center
        cheatSheet->SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL, 0, height);
    }
    cheatSheet->Show();

    *result = TRUE;
    return NOERROR;
}

void ActionMenuItemView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Boolean textVisible;
    HasText(&textVisible);
    if (textVisible && mSavedPaddingLeft >= 0) {
        Int32 t, r, b;
        GetPaddingTop(&t);
        GetPaddingRight(&r);
        GetPaddingBottom(&b);
        TextView::SetPadding(mSavedPaddingLeft, t, r, b);
    }

    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    const Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    const Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 oldMeasuredWidth;
    GetMeasuredWidth(&oldMeasuredWidth);
    const Int32 targetWidth = widthMode == MeasureSpec::AT_MOST ? Elastos::Core::Math::Min(widthSize, mMinWidth)
            : mMinWidth;

    if (widthMode != MeasureSpec::EXACTLY && mMinWidth > 0 && oldMeasuredWidth < targetWidth) {
        // Remeasure at exactly the minimum width.
        TextView::OnMeasure(MeasureSpec::MakeMeasureSpec(targetWidth, MeasureSpec::EXACTLY),
                heightMeasureSpec);
    }

    if (!textVisible && mIcon != NULL) {
        // TextView won't center compound drawables in both dimensions without
        // a little coercion. Pad in to center the icon after we've measured.
        Int32 w;
        GetMeasuredWidth(&w);

        AutoPtr<IRect> bounds;
        mIcon->GetBounds((IRect**)&bounds);
        assert(bounds != NULL);
        Int32 dw = 0;
        bounds->GetWidth(&dw);

        Int32 t, r, b;
        GetPaddingTop(&t);
        GetPaddingRight(&r);
        GetPaddingBottom(&b);
        TextView::SetPadding((w - dw) / 2, t, r, b);
    }
}

ECode ActionMenuItemView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return TextView::SetEnabled(enabled);
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
