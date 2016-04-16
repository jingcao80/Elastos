// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_MEDIADRMCREDENTIALMANAGER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_MEDIADRMCREDENTIALMANAGER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * A wrapper of the android MediaDrmCredentialManager
  */
// @JNINamespace("content")
class MediaDrmCredentialManager
    : public Object
{
private:
    /**
      * Callback interface for getting notified from credential reset.
      */
    class MediaDrmCredentialManagerCallback
        : public Object
    {
    public:
        /**
          * This method will be called when credential reset attempt is done.
          * @param succeeded Whether or not it succeeded.
          */
        // @CalledByNative("MediaDrmCredentialManagerCallback")
        virtual CARAPI OnCredentialResetFinished(
            /* [in] */ Boolean succeeded) = 0;

        static CARAPI_(void) OnCredentialResetFinished(
            /* [in] */ IInterface* obj,
            /* [in] */ Boolean succeeded);

    };

public:
    /**
      * Attempts to reset the DRM credentials.
      * @param callback It notifies whether or not it succeeded.
      */
    static CARAPI ResetCredentials(
        /* [in] */ MediaDrmCredentialManagerCallback* callback);

    static CARAPI_(void*) ElaMediaDrmCredentialManagerCallback_Init();

private:
    static CARAPI NativeResetCredentials(
        /* [in] */ MediaDrmCredentialManagerCallback* callback);
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_MEDIADRMCREDENTIALMANAGER_H_
