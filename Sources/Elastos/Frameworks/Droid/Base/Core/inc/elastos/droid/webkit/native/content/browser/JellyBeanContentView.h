// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_JELLYBEANCONTENTVIEW_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_JELLYBEANCONTENTVIEW_H_

#include "elastos/droid/webkit/native/content/browser/ContentView.h"

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

/**
  * A version of {@link ContentView} that supports JellyBean features.
  */
class JellyBeanContentView : public ContentView
{
public:
    JellyBeanContentView(
        /* [in] */ IContext* context,
        /* [in] */ ContentViewCore* cvc);

    // @Override
    CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    // @Override
    CARAPI_(AutoPtr<IAccessibilityNodeProvider>) GetAccessibilityNodeProvider();
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_JELLYBEANCONTENTVIEW_H_
