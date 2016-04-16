// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_JAVASCRIPTINTERFACE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_JAVASCRIPTINTERFACE_H_

#include "elastos/droid/ext/frameworkext.h"

// package org.chromium.content.browser;
// import java.lang.annotation.ElementType;
// import java.lang.annotation.Retention;
// import java.lang.annotation.RetentionPolicy;
// import java.lang.annotation.Target;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Marks a method as being able to be exposed to JavaScript.  This is used for safety purposes so
  * that only explicitly marked methods get exposed instead of every method in a class.
  * @see {@link ContentViewCore#addJavascriptInterface(Class, String, boolean)}
  */
// @SuppressWarnings("javadoc")
// @Retention(RetentionPolicy.RUNTIME)
// @Target({ElementType.METHOD})
class JavascriptInterface
    : public Object
{
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_JAVASCRIPTINTERFACE_H_
