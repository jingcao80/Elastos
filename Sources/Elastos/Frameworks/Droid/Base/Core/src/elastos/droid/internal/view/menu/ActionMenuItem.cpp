
#include "elastos/droid/internal/view/menu/ActionMenuItem.h"

using Elastos::Core::CString;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::EIID_IMenuItem;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const Int32 ActionMenuItem::NO_ICON = 0;
const Int32 ActionMenuItem::CHECKABLE      = 0x00000001;
const Int32 ActionMenuItem::CHECKED        = 0x00000002;
const Int32 ActionMenuItem::EXCLUSIVE      = 0x00000004;
const Int32 ActionMenuItem::HIDDEN         = 0x00000008;
const Int32 ActionMenuItem::ENABLED        = 0x00000010;

CAR_INTERFACE_IMPL_2(ActionMenuItem, Object, IActionMenuItem, IMenuItem)

ActionMenuItem::ActionMenuItem()
    : mId(0)
    , mGroup(0)
    , mCategoryOrder(0)
    , mOrdering(0)
    , mShortcutNumericChar(0)
    , mShortcutAlphabeticChar(0)
    , mIconResId(NO_ICON)
    , mFlags(ENABLED)
{
}

ECode ActionMenuItem::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 group,
    /* [in] */ Int32 id,
    /* [in] */ Int32 categoryOrder,
    /* [in] */ Int32 ordering,
    /* [in] */ ICharSequence* title)
{
    mContext = context;
    mId = id;
    mGroup = group;
    mCategoryOrder = categoryOrder;
    mOrdering = ordering;
    mTitle = title;
    return NOERROR;
}

ECode ActionMenuItem::GetAlphabeticShortcut(
    /* [out] */ Char32* shortcut)
{
    VALIDATE_NOT_NULL(shortcut)
    *shortcut = mShortcutAlphabeticChar;
    return NOERROR;
}

ECode ActionMenuItem::GetGroupId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mGroup;
    return NOERROR;
}

ECode ActionMenuItem::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mIconDrawable;
    REFCOUNT_ADD(*icon);

    return NOERROR;
}

ECode ActionMenuItem::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent);

    return NOERROR;
}

ECode ActionMenuItem::GetItemId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode ActionMenuItem::GetMenuInfo(
    /* [out] */ IContextMenuInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    return NOERROR;
}

ECode ActionMenuItem::GetNumericShortcut(
    /* [out] */ Char32* shortcut)
{
    VALIDATE_NOT_NULL(shortcut)
    *shortcut = mShortcutNumericChar;
    return NOERROR;
}

ECode ActionMenuItem::GetOrder(
    /* [out] */ Int32* ordering)
{
    VALIDATE_NOT_NULL(ordering)
    *ordering = mOrdering;
    return NOERROR;
}

ECode ActionMenuItem::GetSubMenu(
    /* [out] */ ISubMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = NULL;
    return NOERROR;
}

ECode ActionMenuItem::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = mTitle;
    REFCOUNT_ADD(*title);

    return NOERROR;
}

ECode ActionMenuItem::GetTitleCondensed(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = mTitleCondensed != NULL ? mTitleCondensed : mTitle;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode ActionMenuItem::HasSubMenu(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = FALSE;
    return NOERROR;
}

ECode ActionMenuItem::IsCheckable(
    /* [out] */ Boolean* checkable)
{
    VALIDATE_NOT_NULL(checkable)
    *checkable = (mFlags & CHECKABLE) != 0;
    return NOERROR;
}

ECode ActionMenuItem::IsChecked(
    /* [out] */ Boolean* checked)
{
    VALIDATE_NOT_NULL(checked)
    *checked = (mFlags & CHECKED) != 0;
    return NOERROR;
}

ECode ActionMenuItem::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = (mFlags & ENABLED) != 0;
    return NOERROR;
}

ECode ActionMenuItem::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible)
    *visible = (mFlags & HIDDEN) == 0;
    return NOERROR;
}

ECode ActionMenuItem::SetAlphabeticShortcut(
    /* [in] */ Char32 alphaChar)
{
    mShortcutAlphabeticChar = alphaChar;

    return NOERROR;
}

ECode ActionMenuItem::SetCheckable(
    /* [in] */ Boolean checkable)
{
    mFlags = (mFlags & ~CHECKABLE) | (checkable ? CHECKABLE : 0);
    return NOERROR;
}

ECode ActionMenuItem::SetExclusiveCheckable(
    /* [in] */ Boolean exclusive)
{
    mFlags = (mFlags & ~EXCLUSIVE) | (exclusive ? EXCLUSIVE : 0);
    return NOERROR;
}

ECode ActionMenuItem::SetChecked(
    /* [in] */ Boolean checked)
{
    mFlags = (mFlags & ~CHECKED) | (checked ? CHECKED : 0);
    return NOERROR;
}

ECode ActionMenuItem::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mFlags = (mFlags & ~ENABLED) | (enabled ? ENABLED : 0);
    return NOERROR;
}

ECode ActionMenuItem::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIconDrawable = icon;
    mIconResId = NO_ICON;
    return NOERROR;
}

ECode ActionMenuItem::SetIcon(
    /* [in] */ Int32 iconRes)
{
    mIconResId = iconRes;
    mIconDrawable = NULL;
    mContext->GetDrawable(iconRes, (IDrawable**)&mIconDrawable);
    return NOERROR;
}

ECode ActionMenuItem::SetIntent(
    /* [in] */ IIntent* intent)
{
    mIntent = intent;
    return NOERROR;
}

ECode ActionMenuItem::SetNumericShortcut(
    /* [in] */ Char32 numericChar)
{
    mShortcutNumericChar = numericChar;
    return NOERROR;
}

ECode ActionMenuItem::SetOnMenuItemClickListener(
    /* [in] */ IOnMenuItemClickListener* menuItemClickListener)
{
    mClickListener = menuItemClickListener;
    return NOERROR;
}

ECode ActionMenuItem::SetShortcut(
    /* [in] */ Char32 numericChar,
    /* [in] */ Char32 alphaChar)
{
    mShortcutNumericChar = numericChar;
    mShortcutAlphabeticChar = alphaChar;

    return NOERROR;
}

ECode ActionMenuItem::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode ActionMenuItem::SetTitle(
    /* [in] */ Int32 title)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    assert(res != NULL);

    String str;
    res->GetString(title, &str);
    mTitle = NULL;
    CString::New(str, (ICharSequence**)&mTitle);
    return NOERROR;
}

ECode ActionMenuItem::SetTitleCondensed(
    /* [in] */ ICharSequence* title)
{
    mTitleCondensed = title;
    return NOERROR;
}

ECode ActionMenuItem::SetVisible(
    /* [in] */ Boolean visible)
{
    mFlags = (mFlags & HIDDEN) | (visible ? 0 : HIDDEN);
    return NOERROR;
}

ECode ActionMenuItem::Invoke(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean tmp = FALSE;
    if (mClickListener != NULL && (mClickListener->OnMenuItemClick((IMenuItem*)this->Probe(EIID_IMenuItem), &tmp), tmp)) {
        *res = TRUE;
        return NOERROR;
    }

    if (mIntent != NULL) {
        mContext->StartActivity(mIntent);
        *res = TRUE;
        return NOERROR;
    }

    *res = FALSE;
    return NOERROR;
}

ECode ActionMenuItem::SetShowAsAction(
    /* [in] */ Int32 show)
{
    // Do nothing. ActionMenuItems always show as action buttons.
    return NOERROR;
}

ECode ActionMenuItem::SetActionView(
    /* [in] */ IView* actionView)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ActionMenuItem::GetActionView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    return NOERROR;;
}

ECode ActionMenuItem::SetActionView(
    /* [in] */ Int32 resId)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ActionMenuItem::GetActionProvider(
    /* [out] */ IActionProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = NULL;
    return NOERROR;
}

ECode ActionMenuItem::SetActionProvider(
    /* [in] */ IActionProvider* actionProvider)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ActionMenuItem::SetShowAsActionFlags(
    /* [in] */ Int32 actionEnum)
{
    SetShowAsAction(actionEnum);
    return NOERROR;
}

ECode ActionMenuItem::ExpandActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuItem::CollapseActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuItem::IsActionViewExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuItem::SetOnActionExpandListener(
    /* [in] */ IOnActionExpandListener* listener)
{
    // No need to save the listener; ActionMenuItem does not support collapsing items.
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
