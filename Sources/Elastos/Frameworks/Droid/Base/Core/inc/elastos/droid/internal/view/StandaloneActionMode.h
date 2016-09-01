#ifndef __ELASTOS_DROID_INTERNAL_VIEW_STANDALONEACTIONMODE_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_STANDALONEACTIONMODE_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/view/ActionMode.h"

using Elastos::Droid::View::ActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::Widget::IActionBarContextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class StandaloneActionMode
    : public ActionMode
{
private:
    class MenuBuilderCallback
        : public Object
        , public IMenuBuilderCallback
    {
    public:
        MenuBuilderCallback(
            /* [in] */ StandaloneActionMode* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* state);

        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

    private:
        StandaloneActionMode* mOwner;
    };

public:
    StandaloneActionMode();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IActionBarContextView* view,
        /* [in] */ IActionModeCallback* callback,
        /* [in] */ Boolean isFocusable);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);

    CARAPI SetTitle(
        /* [in] */ Int32 resId);

    CARAPI SetSubtitle(
        /* [in] */ Int32 resId);

    CARAPI SetTitleOptionalHint(
        /* [in] */ Boolean titleOptional);

    CARAPI IsTitleOptional(
        /* [out] */ Boolean* isTitleOptional);

    CARAPI SetCustomView(
        /* [in] */ IView* view);

    CARAPI Invalidate();

    CARAPI Finish();

    CARAPI GetMenu(
        /* [out] */ IMenu** menu);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** subtitle);

    CARAPI GetCustomView(
        /* [out] */ IView** view);

    CARAPI GetMenuInflater(
        /* [out] */ IMenuInflater** menuInflater);

    CARAPI OnMenuItemSelected(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* state);

    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    CARAPI_(Boolean) OnSubMenuSelected(
        /* [in] */ ISubMenuBuilder* subMenu);

    CARAPI OnCloseSubMenu(
        /* [in] */ ISubMenuBuilder* menu);

    CARAPI OnMenuModeChange(
        /* [in] */ IMenuBuilder* menu);

    CARAPI_(Boolean) IsUiFocusable();

private:
    IContext* mContext;
    AutoPtr<IActionBarContextView> mContextView;
    AutoPtr<IActionModeCallback> mCallback;
    AutoPtr<IWeakReference> mCustomView;
    Boolean mFinished;
    Boolean mFocusable;

    AutoPtr<IMenuBuilder> mMenu;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_STANDALONEACTIONMODE_H__
