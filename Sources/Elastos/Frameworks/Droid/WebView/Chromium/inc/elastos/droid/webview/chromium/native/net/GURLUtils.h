// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_GURLUTILS_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_GURLUTILS_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * Class to access the GURL library from java.
  */
// @JNINamespace("net")
class GURLUtils
    : public Object
{
public:
    /**
      * Get the origin of an url: Ex getOrigin("http://www.example.com:8080/index.html?bar=foo")
      * would return "http://www.example.com:8080". It will return an empty string for an
      * invalid url.
      *
      * @return The origin of the url
      */
    static CARAPI_(String) GetOrigin(
        /* [in] */ const String& url);

    /**
      * Get the scheme of the url (e.g. http, https, file). The returned string
      * contains everything before the "://".
      *
      * @return The scheme of the url.
      */
    static CARAPI_(String) GetScheme(
        /* [in] */ const String& url);

private:
    static CARAPI_(String) NativeGetOrigin(
        /* [in] */ const String& url);

    static CARAPI_(String) NativeGetScheme(
        /* [in] */ const String& url);
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_GURLUTILS_H_

