
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/view/menu/ListMenuItemView.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"

using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const String ListMenuItemView::TAG("ListMenuItemView");

CAR_INTERFACE_IMPL_2(ListMenuItemView, LinearLayout, IListMenuItemView, IMenuItemView)

ListMenuItemView::ListMenuItemView()
    : mTextAppearance(0)
    , mTextAppearanceContext(NULL)
    , mPreserveIconSpacing(FALSE)
    , mMenuType(0)
    , mForceShowIcon(FALSE)
{}

ECode ListMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::MenuView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetDrawable(R::styleable::MenuView_itemBackground, (IDrawable**)&mBackground);
    a->GetResourceId(R::styleable::MenuView_itemTextAppearance, -1, &mTextAppearance);
    a->GetBoolean(R::styleable::MenuView_preserveIconSpacing, FALSE, &mPreserveIconSpacing);
    mTextAppearanceContext = context;

    a->Recycle();
    return NOERROR;
}

ECode ListMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ListMenuItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ListMenuItemView::OnFinishInflate()
{
    FAIL_RETURN(LinearLayout::OnFinishInflate())

    SetBackgroundDrawable(mBackground);

    AutoPtr<IView> view;
    FindViewById(R::id::title, (IView**)&view);
    mTitleView = ITextView::Probe(view);
    if (mTextAppearance != -1) {
        mTitleView->SetTextAppearance(mTextAppearanceContext, mTextAppearance);
    }

    view = NULL;
    FindViewById(R::id::shortcut, (IView**)&view);
    mShortcutView = ITextView::Probe(view);
    return NOERROR;
}

ECode ListMenuItemView::Initialize(
    /* [in] */ IMenuItemImpl* _itemData,
    /* [in] */ Int32 menuType)
{
    mItemData = _itemData;
    IMenuItem* itemData = IMenuItem::Probe(_itemData);
    mMenuType = menuType;

    Boolean visible = FALSE;
    itemData->IsVisible(&visible);
    SetVisibility(visible ? IView::VISIBLE : IView::GONE);

    AutoPtr<ICharSequence> title;
    mItemData->GetTitleForItemView(this, (ICharSequence**)&title);
    SetTitle(title);
    Boolean isCheckable = FALSE;
    itemData->IsCheckable(&isCheckable);
    SetCheckable(isCheckable);
    Boolean mssc = FALSE;
    mItemData->ShouldShowShortcut(&mssc);
    Char32 mgsc;
    mItemData->GetShortcut(&mgsc);
    SetShortcut(mssc, mgsc);
    AutoPtr<IDrawable> icon;
    itemData->GetIcon((IDrawable**)&icon);
    SetIcon(icon);
    Boolean isEnabled = FALSE;
    itemData->IsEnabled(&isEnabled);
    SetEnabled(isEnabled);

    return NOERROR;
}

ECode ListMenuItemView::SetForceShowIcon(
    /* [in] */ Boolean forceShow)
{
    mPreserveIconSpacing = mForceShowIcon = forceShow;
    return NOERROR;
}

ECode ListMenuItemView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    Int32 visibility;
    IView::Probe(mTitleView)->GetVisibility(&visibility);
    if (title != NULL) {
        mTitleView->SetText(title);

        if (visibility != IView::VISIBLE) {
            IView::Probe(mTitleView)->SetVisibility(IView::VISIBLE);
        }
    }
    else {
        if (visibility != IView::GONE) {
            IView::Probe(mTitleView)->SetVisibility(IView::GONE);
        }
    }

    return NOERROR;
}

ECode ListMenuItemView::GetItemData(
    /* [out] */ IMenuItemImpl** itemData)
{
    VALIDATE_NOT_NULL(itemData)
    *itemData = mItemData;
    REFCOUNT_ADD(*itemData)
    return NOERROR;
}

ECode ListMenuItemView::SetCheckable(
    /* [in] */ Boolean checkable)
{
    if (!checkable && mRadioButton == NULL && mCheckBox == NULL) {
        return NOERROR;
    }

    // Depending on whether its exclusive check or not, the checkbox or
    // radio button will be the one in use (and the other will be otherCompoundButton)
    AutoPtr<ICompoundButton> compoundButton;
    AutoPtr<ICompoundButton> otherCompoundButton;

    Boolean check = FALSE;
    mItemData->IsExclusiveCheckable(&check);
    if (check) {
        if (mRadioButton == NULL) {
            InsertRadioButton();
        }
        compoundButton = ICompoundButton::Probe(mRadioButton);
        otherCompoundButton = ICompoundButton::Probe(mCheckBox);
    }
    else {
        if (mCheckBox == NULL) {
            InsertCheckBox();
        }
        compoundButton = ICompoundButton::Probe(mCheckBox);
        otherCompoundButton = ICompoundButton::Probe(mRadioButton);
    }

    if (checkable) {
        IMenuItem::Probe(mItemData)->IsChecked(&check);
        ICheckable::Probe(compoundButton)->SetChecked(check);

        Int32 newVisibility = checkable ? IView::VISIBLE : IView::GONE;

        Int32 tmpVisibility;
        IView::Probe(compoundButton)->GetVisibility(&tmpVisibility);
        if (tmpVisibility != newVisibility) {
            IView::Probe(compoundButton)->SetVisibility(newVisibility);
        }

        // Make sure the other compound button isn't visible
        if (otherCompoundButton != NULL) {
            IView::Probe(otherCompoundButton)->GetVisibility(&tmpVisibility);
            if (tmpVisibility != IView::GONE) {
                IView::Probe(otherCompoundButton)->SetVisibility(IView::GONE);
            }
        }
    }
    else {
        if (mCheckBox != NULL) {
            IView::Probe(mCheckBox)->SetVisibility(IView::GONE);
        }

        if (mRadioButton != NULL) {
            IView::Probe(mRadioButton)->SetVisibility(IView::GONE);
        }
    }

    return NOERROR;
}

ECode ListMenuItemView::SetChecked(
    /* [in] */ Boolean checked)
{
    AutoPtr<ICompoundButton> compoundButton;

    Boolean isExclusiveCheckable = FALSE;
    mItemData->IsExclusiveCheckable(&isExclusiveCheckable);
    if (isExclusiveCheckable) {
        if (mRadioButton == NULL) {
            InsertRadioButton();
        }
        compoundButton = ICompoundButton::Probe(mRadioButton);
    }
    else {
        if (mCheckBox == NULL) {
            InsertCheckBox();
        }
        compoundButton = ICompoundButton::Probe(mCheckBox);
    }

    ICheckable::Probe(compoundButton)->SetChecked(checked);

    return NOERROR;
}

ECode ListMenuItemView::SetShortcut(
    /* [in] */ Boolean showShortcut,
    /* [in] */ Char32 shortcutKey)
{
    Boolean res = FALSE;
    mItemData->ShouldShowShortcut(&res);
    Int32 newVisibility = (showShortcut && res) ? IView::VISIBLE : IView::GONE;

    if (newVisibility == IView::VISIBLE) {
        String label;
        mItemData->GetShortcutLabel(&label);
        AutoPtr<ICharSequence> cs;
        CString::New(label, (ICharSequence**)&cs);
        mShortcutView->SetText(cs);
    }

    Int32 visibility;
    IView::Probe(mShortcutView)->GetVisibility(&visibility);
    if (visibility != newVisibility) {
        IView::Probe(mShortcutView)->SetVisibility(newVisibility);
    }

    return NOERROR;
}

ECode ListMenuItemView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    Boolean show = FALSE;
    mItemData->ShouldShowIcon(&show);
    Boolean showIcon = show || mForceShowIcon;
    if (!showIcon && !mPreserveIconSpacing) {
        return NOERROR;
    }

    if (mIconView == NULL && icon == NULL && !mPreserveIconSpacing) {
        return NOERROR;
    }

    if (mIconView == NULL) {
        InsertIconView();
    }

    if (icon != NULL || mPreserveIconSpacing) {
        mIconView->SetImageDrawable(showIcon ? icon : NULL);

        Int32 visibility;
        IView::Probe(mIconView)->GetVisibility(&visibility);
        if (visibility != IView::VISIBLE) {
            IView::Probe(mIconView)->SetVisibility(IView::VISIBLE);
        }
    }
    else {
        IView::Probe(mIconView)->SetVisibility(IView::GONE);
    }

    return NOERROR;
}

ECode ListMenuItemView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mIconView != NULL && mPreserveIconSpacing) {
        // Enforce minimum icon spacing
        AutoPtr<IViewGroupLayoutParams> lp;
        GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IViewGroupLayoutParams> iconLp;
        IView::Probe(mIconView)->GetLayoutParams((IViewGroupLayoutParams**)&iconLp);

        Int32 height = 0, width = 0;
        lp->GetHeight(&height);
        iconLp->GetWidth(&width);
        if (height > 0 && width <= 0) {
            iconLp->SetWidth(height);
        }
    }

    LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

void ListMenuItemView::InsertIconView()
{
    AutoPtr<ILayoutInflater> inflater = GetInflater();
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::list_menu_item_icon,
            this, FALSE, (IView**)&view);
    mIconView = IImageView::Probe(view);
    AddView(view, 0);
}

void ListMenuItemView::InsertRadioButton()
{
    AutoPtr<ILayoutInflater> inflater = GetInflater();
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::list_menu_item_radio,
            this, FALSE, (IView**)&view);
    mRadioButton = IRadioButton::Probe(view);
    AddView(view);
}

void ListMenuItemView::InsertCheckBox()
{
    AutoPtr<ILayoutInflater> inflater = GetInflater();
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::list_menu_item_checkbox,
            this, FALSE, (IView**)&view);
    mCheckBox = ICheckBox::Probe(view);
    AddView(view);
}

ECode ListMenuItemView::PrefersCondensedTitle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ListMenuItemView::ShowsIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mForceShowIcon;
    return NOERROR;
}

AutoPtr<ILayoutInflater> ListMenuItemView::GetInflater()
{
    if (mInflater == NULL) {
        LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
    }

    return mInflater;
}

ECode ListMenuItemView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FAIL_RETURN(LinearLayout::OnInitializeAccessibilityNodeInfo(info))

    Boolean hasSubMenu;
    if (mItemData != NULL && (IMenuItem::Probe(mItemData)->HasSubMenu(&hasSubMenu), hasSubMenu)) {
        info->SetCanOpenPopup(TRUE);
    }
    return NOERROR;
}

ECode ListMenuItemView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return LinearLayout::SetEnabled(enabled);
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
