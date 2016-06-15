
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_EXPANDEDMENUVIEW_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_EXPANDEDMENUVIEW_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ListView.h"

using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * The expanded menu view is a list-like menu with all of the available menu items.  It is opened
 * by the user clicking no the 'More' button on the icon menu view.
 */
class ExpandedMenuView
    : public ListView
    , public IExpandedMenuView
    , public IMenuBuilderItemInvoker
    , public IMenuView
{
private:
    class OnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnItemClickListener(
            /* [in] */ ExpandedMenuView* owner);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        ExpandedMenuView* mOwner;
    };

public:
    ExpandedMenuView();

    CAR_INTERFACE_DECL()

    /**
     * Instantiates the ExpandedMenuView that is linked with the provided MenuBuilder.
     * @param menu The model for the menu which this MenuView will display
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Initialize(
        /* [in] */ IMenuBuilder* menu);

    CARAPI InvokeItem(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI GetWindowAnimations(
        /* [out] */ Int32* animations);

protected:
    //@override
    CARAPI OnDetachedFromWindow();

private:
    AutoPtr<IMenuBuilder> mMenu;

    /** Default animations for this menu */
    Int32 mAnimations;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_EXPANDEDMENUVIEW_H__
