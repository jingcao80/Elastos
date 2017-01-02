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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupItem.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/PopupItemType.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

SelectPopupItem::SelectPopupItem(
    /* [in] */ const String& label,
    /* [in] */ Int32 type)
    : mLabel(label)
    , mType(type)
{
}

//@Override
String SelectPopupItem::GetLabel()
{
    return mLabel;
}

//@Override
String SelectPopupItem::GetSublabel()
{
    return String(NULL);
}

//@Override
Boolean SelectPopupItem::IsEnabled()
{
    return mType == PopupItemType::ENABLED || mType == PopupItemType::GROUP;
}

//@Override
Boolean SelectPopupItem::IsGroupHeader()
{
    return mType == PopupItemType::GROUP;
}

Int32 SelectPopupItem::GetType()
{
    return mType;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
