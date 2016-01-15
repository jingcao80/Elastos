// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_ANDROIDPRIVATEKEY_H_
#define _ELASTOS_DROID_WEBKIT_NET_ANDROIDPRIVATEKEY_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/ElastosKeyStore.h"
#include <elastos/core/Object.h>

// package org.chromium.net;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Webkit::Net::ElastosKeyStore;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_ANDROIDPRIVATEKEY_H_

