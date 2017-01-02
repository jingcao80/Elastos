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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUDIALOGHELPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUDIALOGHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * Helper for menus that appear as Dialogs (context and submenus).
 *
 * @hide
 */
class MenuDialogHelper
    : public Object
    , public IMenuDialogHelper
    , public IDialogInterfaceOnKeyListener
    , public IDialogInterfaceOnClickListener
    , public IDialogInterfaceOnDismissListener
    , public IMenuPresenterCallback
{
private:
    class Listener
        : public Object
        , public IDialogInterfaceOnKeyListener
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnDismissListener
        , public IMenuPresenterCallback
    {
    public:
        Listener(
            /* [in] */ MenuDialogHelper* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* flag);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IWeakReference> mWeakOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IMenuBuilder* menu);

    /**
     * Shows menu as a dialog.
     *
     * @param windowToken Optional token to assign to the window.
     */
    CARAPI Show(
        /* [in] */ IBinder* windowToken);

    CARAPI OnKey(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* flag);

    CARAPI SetPresenterCallback(
        /* [in] */ IMenuPresenterCallback* cb);

    /**
     * Dismisses the menu's dialog.
     *
     * @see Dialog#dismiss()
     */
    CARAPI Dismiss();

    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    CARAPI OnOpenSubMenu(
        /* [in] */ IMenuBuilder* subMenu,
        /* [out] */ Boolean* result);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

protected:
    CARAPI Init(
        /* [in] */ IMenuBuilder* menu);

private:
    IMenuBuilder* mMenu;
    // In order to use convenient
    AutoPtr<IDialog> mDialog;
    // AutoPtr<IAlertDialog> mDialog;
    AutoPtr<IListMenuPresenter> mPresenter;
    AutoPtr<IMenuPresenterCallback> mPresenterCallback;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUDIALOGHELPER_H__
