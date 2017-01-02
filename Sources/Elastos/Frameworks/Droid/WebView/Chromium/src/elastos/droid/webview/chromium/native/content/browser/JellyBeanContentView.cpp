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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/JellyBeanContentView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                         JellyBeanContentView
//=====================================================================
JellyBeanContentView::JellyBeanContentView(
    /* [in] */ IContext* context,
    /* [in] */ ContentViewCore* cvc)
    : ContentView(context, cvc)
{
}

Boolean JellyBeanContentView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (mContentViewCore->SupportsAccessibilityAction(action)) {
        return mContentViewCore->PerformAccessibilityAction(action, arguments);
    }

    Boolean res;
    ContentView::PerformAccessibilityAction(action, arguments, &res);
    return res;
}

AutoPtr<IAccessibilityNodeProvider> JellyBeanContentView::GetAccessibilityNodeProvider()
{
    AutoPtr<IAccessibilityNodeProvider> provider = mContentViewCore->GetAccessibilityNodeProvider();
    if (provider != NULL) {
        return provider;
    } else {
        ContentView::GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        return provider;
    }
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
