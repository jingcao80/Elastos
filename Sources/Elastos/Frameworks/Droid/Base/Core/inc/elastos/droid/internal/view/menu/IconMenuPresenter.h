
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUPRESENTER_H__

#include "elastos/droid/internal/view/menu/BaseMenuPresenter.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * MenuPresenter for the classic "six-pack" icon menu.
 */
class IconMenuPresenter
    : public BaseMenuPresenter
    , public IIconMenuPresenter
{
protected:
    class SubMenuPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        SubMenuPresenterCallback(
            /* [in] */ IconMenuPresenter* host);

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        // /@Override
        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

    private:
        IconMenuPresenter*  mHost;
    };

public:
    ~IconMenuPresenter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    //@Override
    CARAPI InitForMenu(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu);

    //@Override
    CARAPI BindItemView(
        /* [in] */ IMenuItemImpl* item,
        /* [in] */ IMenuItemView* itemView);

    //@Override
    CARAPI ShouldIncludeItem(
        /* [in] */ Int32 childIndex,
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnSubMenuSelected(
        /* [in] */ ISubMenuBuilder* subMenu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI UpdateMenuView(
        /* [in] */ Boolean cleared);

    CARAPI GetNumActualItemsShown(
        /* [out] */ Int32* num);

    CARAPI SaveHierarchyState(
        /* [in] */ IBundle* outState);

    CARAPI RestoreHierarchyState(
        /* [in] */ IBundle* inState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** parcel);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

protected:
    IconMenuPresenter();

    CARAPI Init(
        /* [in] */ IContext* ctx);

    //@Override
    virtual CARAPI_(void) AddItemView(
        /* [in] */ IView* itemView,
        /* [in] */ Int32 childIndex);

    //@Override
    virtual CARAPI_(Boolean) FilterLeftoverView(
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 childIndex);

protected:
    Int32 mOpenSubMenuId;
    AutoPtr<SubMenuPresenterCallback> mSubMenuPresenterCallback;
    AutoPtr<IMenuDialogHelper> mOpenSubMenu;

private:
    AutoPtr<IIconMenuItemView> mMoreView;
    Int32 mMaxItems;

    static String VIEWS_TAG;
    static String OPEN_SUBMENU_KEY;

};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUPRESENTER_H__
