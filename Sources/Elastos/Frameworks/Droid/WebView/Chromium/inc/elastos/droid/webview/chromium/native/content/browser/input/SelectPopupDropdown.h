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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopup.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;

using Elastos::Utility::IList;

// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.content.browser.RenderCoordinates;
// import org.chromium.ui.DropdownAdapter;
// import org.chromium.ui.DropdownItem;
// import org.chromium.ui.DropdownPopupWindow;

// import java.util.List;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

class DropdownPopupWindow;

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Webview::Chromium::Ui::DropdownPopupWindow;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Input {

/**
 * Handles the dropdown popup for the <select> HTML tag support.
 */
class SelectPopupDropdown
    : public SelectPopup
{
private:
    class InnerAdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAdapterViewOnItemClickListener(
            /* [in] */ SelectPopupDropdown* owner);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        SelectPopupDropdown* mOwner;
    };

    class InnerPopupWindowOnDismissListener
        : public Object
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerPopupWindowOnDismissListener(
            /* [in] */ SelectPopupDropdown* owner);

        CARAPI OnDismiss();

    private:
        SelectPopupDropdown* mOwner;
    };

public:
    SelectPopupDropdown(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ IList* items,
        /* [in] */ IRect* bounds,
        /* [in] */ ArrayOf<Int32>* selected);

    //@Override
    CARAPI_(void) Show();

    //@Override
    CARAPI_(void) Hide();

private:
    AutoPtr<ContentViewCore> mContentViewCore;
    AutoPtr<IContext> mContext;

    AutoPtr<DropdownPopupWindow> mDropdownPopupWindow;
    Int32 mInitialSelection;
    Boolean mAlreadySelectedItems;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPDROPDOWN_H__
