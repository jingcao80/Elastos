
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/PopupMenu.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuPopupHelper.h"
#include "elastos/droid/view/CMenuInflater.h"

using Elastos::Droid::Internal::View::Menu::CMenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::EIID_IPopupMenu;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//          PopupMenu::MyCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(PopupMenu::MyCallback, Object, IMenuBuilderCallback, IMenuPresenterCallback)

ECode PopupMenu::MyCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    return mHost->OnMenuItemSelected(menu, item, state);
}

ECode PopupMenu::MyCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return mHost->OnMenuModeChange(menu);
}

ECode PopupMenu::MyCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* handle)
{
    VALIDATE_NOT_NULL(handle);
    return mHost->OnOpenSubMenu(subMenu, handle);
}

ECode PopupMenu::MyCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    return mHost->OnCloseMenu(menu, allMenusAreClosing);
}

//==============================================================================
//          PopupMenu::MyForwardingListener
//==============================================================================

PopupMenu::MyForwardingListener::MyForwardingListener(
    /* [in] */ IView* view,
    /* [in] */ PopupMenu* host)
    : mHost(host)
{
    ListPopupWindow::ForwardingListener::constructor(view);
}

PopupMenu::MyForwardingListener::~MyForwardingListener()
{}

ECode PopupMenu::MyForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** result)
{
    VALIDATE_NOT_NULL(result);
    // This will be null until show() is called.
    return mHost->mPopup->GetPopup(result);
}

Boolean PopupMenu::MyForwardingListener::OnForwardingStarted()
{
    mHost->Show();
    return TRUE;
}

Boolean PopupMenu::MyForwardingListener::OnForwardingStopped()
{
    mHost->Dismiss();
    return TRUE;
}

//==============================================================================
//          PopupMenu
//==============================================================================

CAR_INTERFACE_IMPL(PopupMenu, Object, IPopupMenu);

PopupMenu::PopupMenu()
    : mContext(NULL)
{}

PopupMenu::~PopupMenu()
{}

ECode PopupMenu::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* anchor)
{
    return constructor(context, anchor, IGravity::NO_GRAVITY);
}

ECode PopupMenu::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* anchor,
    /* [in] */ Int32 gravity)
{
    mContext = context;
    CMenuBuilder::New(context, (IMenuBuilder**)&mMenu);
    AutoPtr<MyCallback> callback = new MyCallback(this);
    mMenu->SetCallback(callback);
    mAnchor = anchor;
    CMenuPopupHelper::New(context, mMenu, anchor, (IMenuPopupHelper**)&mPopup);
    mPopup->SetGravity(gravity);
    IMenuPresenter::Probe(mPopup)->SetCallback(callback);
    return NOERROR;
}

ECode PopupMenu::GetDragToOpenListener(
    /* [out] */ IViewOnTouchListener** listener)
{
    VALIDATE_NOT_NULL(listener);

    if (mDragListener == NULL) {
        mDragListener = new MyForwardingListener(mAnchor, this);
    }

    *listener = mDragListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode PopupMenu::GetMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu);
    *menu = IMenu::Probe(mMenu);
    REFCOUNT_ADD(*menu);
    return NOERROR;
}

ECode PopupMenu::GetMenuInflater(
    /* [out] */ IMenuInflater** inflate)
{
    VALIDATE_NOT_NULL(inflate);
    return CMenuInflater::New(mContext, inflate);
}

ECode PopupMenu::Inflate(
    /* [in] */ Int32 menuRes)
{
    AutoPtr<IMenuInflater> flater;
    GetMenuInflater((IMenuInflater**)&flater);
    AutoPtr<IMenu> menu = IMenu::Probe(mMenu);
    flater->Inflate(menuRes, menu);
    return NOERROR;
}

ECode PopupMenu::Show()
{
    ASSERT_SUCCEEDED(mPopup->Show());
    return NOERROR;
}

ECode PopupMenu::Dismiss()
{
    mPopup->Dismiss();
    return NOERROR;
}

ECode PopupMenu::SetOnMenuItemClickListener(
    /* [in] */ IPopupMenuOnMenuItemClickListener* listener)
{
    mMenuItemClickListener = listener;
    return NOERROR;
}

ECode PopupMenu::SetOnDismissListener(
    /* [in] */ IPopupMenuOnDismissListener* listener)
{
    mDismissListener = listener;
    return NOERROR;
}

ECode PopupMenu::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);

    if (mMenuItemClickListener != NULL) {
        Boolean result;
        mMenuItemClickListener->OnMenuItemClick(item, &result);
        *rst = result;
        return NOERROR;
    }
    *rst = FALSE;
    return NOERROR;
}

ECode PopupMenu::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (mDismissListener != NULL) {
        mDismissListener->OnDismiss(this);
    }
    return NOERROR;
}

ECode PopupMenu::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = FALSE;

    if (subMenu == NULL) {
        return NOERROR;
    }

    Boolean hasVisible = FALSE;
    IMenu::Probe(subMenu)->HasVisibleItems(&hasVisible);
    if (!hasVisible) {
        *rst = TRUE;
        return NOERROR;
    }

    AutoPtr<IMenuPopupHelper> helper;
    CMenuPopupHelper::New(mContext, subMenu, mAnchor, (IMenuPopupHelper**)&helper);
    ASSERT_SUCCEEDED(helper->Show());
    *rst = TRUE;
    return NOERROR;
}

ECode PopupMenu::OnCloseSubMenu(
    /* [in] */ ISubMenuBuilder* menu)
{
    return NOERROR;
}

ECode PopupMenu::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
