
#include "elastos/droid/internal/view/menu/IconMenuItemView.h"
#include "elastos/droid/internal/view/menu/IconMenuView.h"
#include "elastos/droid/internal/view/menu/CIconMenuViewLayoutParams.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Text::Layout;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const Int32 IconMenuItemView::NO_ALPHA = 0xFF;
String IconMenuItemView::sPrependShortcutLabel;

#if 0
CAR_INTERFACE_IMPL_2(IconMenuItemView, TextView, IIconMenuItemView, IMenuItemView)
#else
CAR_INTERFACE_IMPL_2(IconMenuItemView, Elastos::Droid::View::View, IIconMenuItemView, IMenuItemView)
#endif

IconMenuItemView::IconMenuItemView()
    : mTextAppearance(0)
    , mDisabledAlpha(0.f)
    , mShortcutCaptionMode(FALSE)
{
    CRect::New((IRect**)&mPositionIconAvailable);
    CRect::New((IRect**)&mPositionIconOutput);
}

ECode IconMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    assert(0);
    // FAIL_RETURN(TextView::constructor(context, attrs));

    if (sPrependShortcutLabel.IsNull()) {
        /*
         * Views should only be constructed from the UI thread, so no
         * synchronization needed
         */
         AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        res->GetString(R::string::prepend_shortcut_label, &sPrependShortcutLabel);
    }

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::MenuView),
        ArraySize(R::styleable::MenuView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetFloat(R::styleable::MenuView_itemIconDisabledAlpha, 0.8f, &mDisabledAlpha);
    a->GetResourceId(R::styleable::MenuView_itemTextAppearance, -1, &mTextAppearance);
    mTextAppearanceContext = context;

    a->Recycle();

    return NOERROR;
}

ECode IconMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode IconMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

void IconMenuItemView::Initialize(
    /* [in] */ ICharSequence* title,
    /* [in] */ IDrawable* icon)
{
    SetClickable(TRUE);
    SetFocusable(TRUE);

    if (mTextAppearance != -1) {
        assert(0);
        // SetTextAppearance(mTextAppearanceContext, mTextAppearance);
    }

    SetTitle(title);
    SetIcon(icon);
}

ECode IconMenuItemView::Initialize(
    /* [in] */ IMenuItemImpl* _itemData,
    /* [in] */ Int32 menuType)
{
    mItemData = _itemData;
    IMenuItem* itemData = IMenuItem::Probe(_itemData);
    AutoPtr<ICharSequence> str;
    mItemData->GetTitleForItemView(this, (ICharSequence**)&str);
    AutoPtr<IDrawable> icon;
    itemData->GetIcon((IDrawable**)&icon);
    Initialize(str, icon);

    Boolean isVisible;
    itemData->IsVisible(&isVisible);
    SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
    Boolean isEnabled;
    itemData->IsEnabled(&isEnabled);
    SetEnabled(isEnabled);

    return NOERROR;
}

ECode IconMenuItemView::SetItemData(
    /* [in] */ IMenuItemImpl* data)
{
    mItemData = data;
    return NOERROR;
}

ECode IconMenuItemView::PerformClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Let the view's click listener have top priority (the More button relies on this)
    assert(0);
    // if (TextView::PerformClick(result), *result) {
    //     *result = TRUE;
    //     return NOERROR;
    // }

    Boolean res;
    if (mItemInvoker != NULL && (mItemInvoker->InvokeItem(mItemData, &res), res)) {
        PlaySoundEffect(SoundEffectConstants::CLICK);
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode IconMenuItemView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    if (mShortcutCaptionMode) {
        /*
         * Don't set the title directly since it will replace the
         * shortcut+title being shown. Instead, re-set the shortcut caption
         * mode so the new title is shown.
         */
        SetCaptionMode(TRUE);
    }
    else if (title != NULL) {
        assert(0);
        // SetText(title);
    }

    return NOERROR;
}

void IconMenuItemView::SetCaptionMode(
    /* [in] */ Boolean shortcut)
{
    /*
     * If there is no item model, don't do any of the below (for example,
     * the 'More' item doesn't have a model)
     */
    if (mItemData == NULL) {
        return;
    }
    Boolean res = FALSE;
    mItemData->ShouldShowShortcut(&res);
    mShortcutCaptionMode = shortcut && res;

    AutoPtr<ICharSequence> text;
    mItemData->GetTitleForItemView(this, (ICharSequence**)&text);

    if (mShortcutCaptionMode) {
        if (mShortcutCaption.IsNull()) {
            mItemData->GetShortcutLabel(&mShortcutCaption);
        }

        text = NULL;
        CString::New(mShortcutCaption, (ICharSequence**)&text);
    }

    assert(0);
    // SetText(text);
}

ECode IconMenuItemView::SetIcon(
        /* [in] */ IDrawable* icon)
{
    mIcon = icon;

    if (icon != NULL) {
        /* Set the bounds of the icon since setCompoundDrawables needs it. */
        Int32 width, height;
        icon->GetIntrinsicWidth(&width);
        icon->GetIntrinsicHeight(&height);
        icon->SetBounds(0, 0, width, height);

        assert(0);
        // // Set the compound drawables
        // SetCompoundDrawables(NULL, icon, NULL, NULL);

        // // When there is an icon, make sure the text is at the bottom
        // SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL);

        /*
         * Request a layout to reposition the icon. The positioning of icon
         * depends on this TextView's line bounds, which is only available
         * after a layout.
         */
        RequestLayout();
    }
    else {
        assert(0);
        // SetCompoundDrawables(NULL, NULL, NULL, NULL);

        // // When there is no icon, make sure the text is centered vertically
        // SetGravity(IGravity::CENTER_VERTICAL | IGravity::CENTER_HORIZONTAL);
    }

    return NOERROR;
}

ECode IconMenuItemView::SetItemInvoker(
        /* [in] */ IMenuBuilderItemInvoker* itemInvoker)
{
    mItemInvoker = itemInvoker;

    return NOERROR;
}

ECode IconMenuItemView::GetItemData(
    /* [out] */ IMenuItemImpl** itemData)
{
    VALIDATE_NOT_NULL(itemData)
    *itemData = mItemData;
    REFCOUNT_ADD(*itemData)
    return NOERROR;
}

//@override
ECode IconMenuItemView::SetVisibility(
    /* [in] */ Int32 v)
{
    assert(0);
    // TextView::SetVisibility(v);

    if (mIconMenuView != NULL) {
        // On visibility change, mark the IconMenuView to refresh itself eventually
        ((IconMenuView*)mIconMenuView.Get())->MarkStaleChildren();
    }

    return NOERROR;
}

void IconMenuItemView::SetIconMenuView(
    /* [in] */ IIconMenuView* iconMenuView)
{
    mIconMenuView = iconMenuView;;
}

//@override
ECode IconMenuItemView::DrawableStateChanged()
{
    assert(0);
    // FAIL_RETURN(TextView::DrawableStateChanged())

    if (mItemData != NULL && mIcon != NULL) {
        // When disabled, the not-focused state and the pressed state should
        // drop alpha on the icon
        Boolean isEnabled, isPressed, isFocused;
        IMenuItem::Probe(mItemData)->IsEnabled(&isEnabled);
        Boolean isInAlphaState = !isEnabled && ((IsPressed(&isPressed), isPressed)
            || !(IsFocused(&isFocused), isFocused));
        mIcon->SetAlpha(isInAlphaState ? (Int32)(mDisabledAlpha * NO_ALPHA) : NO_ALPHA);
    }

    return NOERROR;
}

//@override
ECode IconMenuItemView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    assert(0);
    // FAIL_RETURN(TextView::OnLayout(changed, left, top, right, bottom))

    PositionIcon();
}

ECode IconMenuItemView::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    assert(0);
    // FAIL_RETURN(TextView::OnTextChanged(text, start, before, after))
    // our layout params depend on the length of the text
    SetLayoutParams(IViewGroupLayoutParams::Probe(GetTextAppropriateLayoutParams()));
}

/**
 * @return layout params appropriate for this view.  If layout params already exist, it will
 *         augment them to be appropriate to the current text size.
 */
AutoPtr<IIconMenuViewLayoutParams> IconMenuItemView::GetTextAppropriateLayoutParams()
{
    AutoPtr<IIconMenuViewLayoutParams> lp;

    AutoPtr<IViewGroupLayoutParams> _lp;
    GetLayoutParams((IViewGroupLayoutParams**)&_lp);
    if (_lp == NULL) {
        // Default layout parameters
        CIconMenuViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::MATCH_PARENT, (IIconMenuViewLayoutParams**)&lp);
    }
    else {
        lp = IIconMenuViewLayoutParams::Probe(_lp);
    }

    assert(0);
    // AutoPtr<ICharSequence> text;
    // GetText((ICharSequence**)&text);
    // AutoPtr<ITextPaint> paint;
    // GetPaint((ITextPaint**)&paint);
    // Set the desired width of item
    // lp->SetDesiredWidth((Int32)Layout::GetDesiredWidth(text, paint));

    return lp;
}

void IconMenuItemView::PositionIcon()
{
    if (mIcon == NULL) {
        return;
    }

    // We reuse the output rectangle as a temp rect
    IRect* tmpRect = mPositionIconOutput;
    assert(0);
    // GetLineBounds(0, tmpRect);

    Int32 width, top;
    GetWidth(&width);
    tmpRect->GetTop(&top);
    mPositionIconAvailable->Set(0, 0, width, top);
    Int32 layoutDirection;
    GetLayoutDirection(&layoutDirection);

    Int32 iconWidth, iconHeight;
    mIcon->GetIntrinsicWidth(&iconWidth);
    mIcon->GetIntrinsicHeight(&iconHeight);
    Gravity::Apply(IGravity::CENTER_VERTICAL | IGravity::START,
        iconWidth, iconHeight, mPositionIconAvailable, mPositionIconOutput, layoutDirection);
    mIcon->SetBounds(mPositionIconOutput);
}

ECode IconMenuItemView::SetCheckable(
    /* [in] */ Boolean checkable)
{
    return NOERROR;
}

ECode IconMenuItemView::SetChecked(
    /* [in] */ Boolean checked)
{
    return NOERROR;
}

ECode IconMenuItemView::SetShortcut(
    /* [in] */ Boolean showShortcut,
    /* [in] */ Char32 shortcutKey)
{
    if (mShortcutCaptionMode) {
        /*
         * Shortcut has changed and we're showing it right now, need to
         * update (clear the old one first).
         */
        mShortcutCaption = NULL;
        SetCaptionMode(TRUE);
    }

    return NOERROR;
}

ECode IconMenuItemView::PrefersCondensedTitle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode IconMenuItemView::ShowsIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode IconMenuItemView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    assert(0);
    // return TextView::SetEnabled(enabled);
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
