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

// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONCLIENT_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONCLIENT_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/NavigationHistory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Provides functionality needed to query and page history and the ability to access
  * items in the history.
  */
class NavigationClient
{
public:
    /**
      * Get a directed copy of the navigation history of the view.
      * @param isForward Whether the returned history should be entries after the current entry.
      * @param itemLimit The limit on the number of items included in the history.
      * @return A directed navigation for the page.
      */
    virtual CARAPI_(AutoPtr<NavigationHistory>) GetDirectedNavigationHistory(
        /* [in] */ Boolean isForward,
        /* [in] */ Int32 itemLimit) = 0;

    /**
      * Navigates to the specified index in the navigation entry for this page.
      * @param index The navigation index to navigate to.
      */
    virtual CARAPI GoToNavigationIndex(
        /* [in] */ Int32 index) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONCLIENT_H_
