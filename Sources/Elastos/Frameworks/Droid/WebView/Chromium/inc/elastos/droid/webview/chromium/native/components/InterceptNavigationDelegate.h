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

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_INTERCEPTNAVIGATIONDELEGATE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_INTERCEPTNAVIGATIONDELEGATE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/components/NavigationParams.h"

// package org.chromium.components.navigation_interception;
// import org.chromium.base.CalledByNative;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

class InterceptNavigationDelegate
    : public Object
{
public:
    /**
      * This method is called for every top-level navigation within the associated WebContents.
      * The method allows the embedder to ignore navigations. This is used on Android to 'convert'
      * certain navigations to Intents to 3rd party applications.
      *
      * @param navigationParams parameters describing the navigation.
      * @return true if the navigation should be ignored.
      */
    // @CalledByNative
    virtual CARAPI_(Boolean) ShouldIgnoreNavigation(
        /* [in] */ NavigationParams* navigationParams) = 0;

    static CARAPI_(void*) ElaInterceptNavigationDelegateCallback_Init();

private:
    static CARAPI_(Boolean) ShouldIgnoreNavigation(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* navigationParams);

};

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_INTERCEPTNAVIGATIONDELEGATE_H_

