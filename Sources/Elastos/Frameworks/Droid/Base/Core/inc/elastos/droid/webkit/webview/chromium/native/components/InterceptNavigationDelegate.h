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

