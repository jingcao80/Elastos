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
