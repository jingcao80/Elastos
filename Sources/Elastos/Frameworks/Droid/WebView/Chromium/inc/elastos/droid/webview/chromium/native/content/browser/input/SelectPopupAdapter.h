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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPADAPTER_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ArrayAdapter;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Select popup item adapter for SelectPopupDialog, used so we can disable
 * OPTION_GROUP items.
 */
class SelectPopupAdapter
    : public ArrayAdapter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new SelectPopupItem adapter for the select popup alert dialog list.
     * @param context        Application context.
     * @param layoutResource Layout resource used for the alert dialog list.
     * @param items          SelectPopupItem array list.
     */
    SelectPopupAdapter(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layoutResource,
        /* [in] */ IList* items);

    //@Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    //@Override
    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* result);

private:
    // Holds the items of the select popup alert dialog list.
    AutoPtr<IList> mItems;

    // True if all items have type PopupItemType.ENABLED.
    Boolean mAreAllItemsEnabled;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPADAPTER_H__
