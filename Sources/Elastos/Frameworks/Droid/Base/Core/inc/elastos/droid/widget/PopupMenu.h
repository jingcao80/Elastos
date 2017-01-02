//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WIDGET_POPUPMENU_H__
#define __ELASTOS_DROID_WIDGET_POPUPMENU_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/widget/ListPopupWindow.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::View::Menu::IMenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC PopupMenu
    : public Object
    , public IPopupMenu
{
private:
    class ECO_LOCAL MyCallback
        : public Object
        , public IMenuBuilderCallback
        , public IMenuPresenterCallback
    {
    public:
        MyCallback(
            /* [in] */ PopupMenu* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* state);

        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* handle);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

    private:
        PopupMenu* mHost;
    };

    class ECO_LOCAL MyForwardingListener
        : public ListPopupWindow::ForwardingListener
    {
    public:
        MyForwardingListener(
            /* [in] */ IView* view,
            /* [in] */ PopupMenu* host);

        ~MyForwardingListener();

        // @Override
        virtual CARAPI GetPopup(
            /* [out] */ IListPopupWindow** result);

    protected:
        // @Override
        virtual CARAPI_(Boolean) OnForwardingStarted();

        // @Override
        virtual CARAPI_(Boolean) OnForwardingStopped();

    private:
        PopupMenu* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PopupMenu();

    ~PopupMenu();

    /**
     * Construct a new PopupMenu.
     *
     * @param context Context for the PopupMenu.
     * @param anchor Anchor view for this popup. The popup will appear below the anchor if there
     *               is room, or above it if there is not.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* anchor);

    /**
     * Construct a new PopupMenu.
     *
     * @param context Context for the PopupMenu.
     * @param anchor Anchor view for this popup. The popup will appear below the anchor if there
     *               is room, or above it if there is not.
     * @param gravity The {@link Gravity} value for aligning the popup with its anchor
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* anchor,
        /* [in] */ Int32 gravity);

    /**
     * Returns an {@link OnTouchListener} that can be added to the anchor view
     * to implement drag-to-open behavior.
     * <p>
     * When the listener is set on a view, touching that view and dragging
     * outside of its bounds will open the popup window. Lifting will select the
     * currently touched list item.
     * <p>
     * Example usage:
     * <pre>
     * PopupMenu myPopup = new PopupMenu(context, myAnchor);
     * myAnchor.setOnTouchListener(myPopup.getDragToOpenListener());
     * </pre>
     *
     * @return a touch listener that controls drag-to-open behavior
     */
    CARAPI GetDragToOpenListener(
        /* [out] */ IViewOnTouchListener** listener);

    /**
     * @return the {@link Menu} associated with this popup. Populate the returned Menu with
     * items before calling {@link #show()}.
     *
     * @see #show()
     * @see #getMenuInflater()
     */
    CARAPI GetMenu(
        /* [out] */ IMenu** menu);

    /**
     * @return a {@link MenuInflater} that can be used to inflate menu items from XML into the
     * menu returned by {@link #getMenu()}.
     *
     * @see #getMenu()
     */
    CARAPI GetMenuInflater(
        /* [out] */ IMenuInflater** flater);

    /**
     * Inflate a menu resource into this PopupMenu. This is equivalent to calling
     * popupMenu.getMenuInflater().inflate(menuRes, popupMenu.getMenu()).
     * @param menuRes Menu resource to inflate
     */
    CARAPI Inflate(
        /* [in] */ Int32 menuRes);

    /**
     * Show the menu popup anchored to the view specified during construction.
     * @see #dismiss()
     */
    CARAPI Show();

    /**
     * Dismiss the menu popup.
     * @see #show()
     */
    CARAPI Dismiss();

    /**
     * Set a listener that will be notified when the user selects an item from the menu.
     *
     * @param listener Listener to notify
     */
    CARAPI SetOnMenuItemClickListener(
        /* [in] */ IPopupMenuOnMenuItemClickListener* listener);

    /**
     * Set a listener that will be notified when this menu is dismissed.
     *
     * @param listener Listener to notify
     */
    CARAPI SetOnDismissListener(
        /* [in] */ IPopupMenuOnDismissListener* listener);

    /**
     * @hide
     */
    CARAPI OnMenuItemSelected(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* rst);

    /**
     * @hide
     */
    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    /**
     * @hide
     */
    CARAPI OnOpenSubMenu(
        /* [in] */ IMenuBuilder* subMenu,
        /* [out] */ Boolean* rst);

    /**
     * @hide
     */
    CARAPI OnCloseSubMenu(
        /* [in] */ ISubMenuBuilder* menu);

    /**
     * @hide
     */
    CARAPI OnMenuModeChange(
        /* [in] */ IMenuBuilder* menu);

private:
    IContext* mContext;
    AutoPtr<IMenuBuilder> mMenu;
    AutoPtr<IView> mAnchor;
    AutoPtr<IMenuPopupHelper> mPopup;
    AutoPtr<IPopupMenuOnMenuItemClickListener> mMenuItemClickListener;
    AutoPtr<IPopupMenuOnDismissListener> mDismissListener;
    AutoPtr<IViewOnTouchListener> mDragListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_POPUPMENU_H__
