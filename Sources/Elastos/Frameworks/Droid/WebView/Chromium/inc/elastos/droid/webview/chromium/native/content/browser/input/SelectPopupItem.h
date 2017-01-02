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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/DropdownItem.h"

using Elastos::Droid::Webkit::Webview::Chromium::Ui::DropdownItem;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Select popup item containing the label, the type and the enabled state
 * of an item belonging to a select popup dialog.
 */
class SelectPopupItem
    : public DropdownItem
{
public:
    SelectPopupItem(
        /* [in] */ const String& label,
        /* [in] */ Int32 type);

    //@Override
    CARAPI_(String) GetLabel();

    //@Override
    CARAPI_(String) GetSublabel();

    //@Override
    CARAPI_(Boolean) IsEnabled();

    //@Override
    CARAPI_(Boolean) IsGroupHeader();

    CARAPI_(Int32) GetType();

private:
    const String mLabel;
    const Int32 mType;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTPOPUPITEM_H__
