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

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDPRIVATEKEY_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDPRIVATEKEY_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosKeyStore.h"
#include <elastos/core/Object.h>

// package org.chromium.net;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Webkit::Webview::Chromium::Net::ElastosKeyStore;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * Abstract private key that bundles the PrivateKey and AndroidKeyStore that it belongs to.
  */
// @JNINamespace("net::android")
class ElastosPrivateKey
    : public Object
{
public:
    /** @return AndroidKeyStore that handles this key. */
    // @CalledByNative
    virtual CARAPI_(AutoPtr<ElastosKeyStore>) GetKeyStore() = 0;

    static CARAPI_(void*) ElaElastosPrivateKeyCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) GetKeyStore(
        /* [in] */ IInterface* obj);

};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDPRIVATEKEY_H_

