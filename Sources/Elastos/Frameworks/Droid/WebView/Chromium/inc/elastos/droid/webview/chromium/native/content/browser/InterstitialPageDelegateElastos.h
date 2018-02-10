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
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INTERSTITIALPAGEDELEGATEANDROID_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INTERSTITIALPAGEDELEGATEANDROID_H_

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
  * Allows the specification and handling of Interstitial pages in java.
  */
// @JNINamespace("content")
class InterstitialPageDelegateElastos
    : public Object
{
public:
    /**
      * Constructs an interstitial with the given HTML content.
      *
      * @param htmlContent The HTML content for the interstitial.
      */
    InterstitialPageDelegateElastos(
        /* [in] */ const String& htmlContent);

    /**
      * @return The pointer to the underlying native counterpart.
      */
    virtual CARAPI_(HANDLE) GetNative();

    static CARAPI_(void*) ElaInterstitialPageDelegateElastosCallback_Init();

protected:
    /**
      * Called when "proceed" is triggered on the interstitial.
      */
    // @CalledByNative
    virtual CARAPI OnProceed();

    /**
      * Called when "dont' proceed" is triggered on the interstitial.
      */
    // @CalledByNative
    virtual CARAPI OnDontProceed();

    /**
      * Called when a command has been received from the interstitial.
      *
      * @param command The command that was received.
      */
    // @CalledByNative
    virtual CARAPI CommandReceived(
        /* [in] */ const String& command);

    /**
      * Notifies the native interstitial to proceed.
      */
    virtual CARAPI Proceed();

    /**
      * Notifies the native interstitial to not proceed.
      */
    virtual CARAPI DontProceed();

private:
    static CARAPI_(void) OnProceed(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnDontProceed(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) CommandReceived(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& command);

    static CARAPI_(void) OnNativeDestroyed(
        /* [in] */ IInterface* obj);

    // @CalledByNative
    CARAPI OnNativeDestroyed();

    CARAPI_(HANDLE) NativeInit(
        /* [in] */ const String& htmlContent);

    CARAPI NativeProceed(
        /* [in] */ HANDLE nativeInterstitialPageDelegateElastos);

    CARAPI NativeDontProceed(
        /* [in] */ HANDLE nativeInterstitialPageDelegateElastos);

private:
    HANDLE mNativePtr;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INTERSTITIALPAGEDELEGATEANDROID_H_
