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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_REMOTEANDROIDKEYSTORE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_REMOTEANDROIDKEYSTORE_H_

#include <Elastos.Droid.Webkit.Native.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosKeyStore.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosPrivateKey.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.net;
// import android.os.RemoteException;
// import android.util.Log;

using Elastos::Droid::Webkit::Webview::Chromium::Net::ElastosKeyStore;
using Elastos::Droid::Webkit::Webview::Chromium::Net::ElastosPrivateKey;
using Elastos::Droid::Webkit::Webview::Chromium::Net::IRemoteElastosKeyStore;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * Provides a remoted implementation of AndroidKeyStore where all calls are forwarded via
  * binder to an external process.
  */
class RemoteElastosKeyStore
    : public ElastosKeyStore
{
private:
    class RemotePrivateKey
        : public ElastosPrivateKey
    {
    public:
        RemotePrivateKey(
            /* [in] */ Int32 handle,
            /* [in] */ RemoteElastosKeyStore* store);

        virtual CARAPI_(Int32) GetHandle();

        // @Override
        CARAPI_(AutoPtr<ElastosKeyStore>) GetKeyStore();

    public:
        // Reference to the key on a remote store.
        const Int32 mHandle;
        // Key store handling this key.
        const AutoPtr<RemoteElastosKeyStore> mStore;
    };

public:
    RemoteElastosKeyStore(
        /* [in] */ IRemoteElastosKeyStore* manager);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetRSAKeyModulus(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetDSAKeyParamQ(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetECKeyOrder(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) RawSignDigestWithPrivateKey(
        /* [in] */ ElastosPrivateKey* key,
        /* [in] */ ArrayOf<Byte>* message);

    // @Override
    CARAPI_(Int32) GetPrivateKeyType(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetPrivateKeyEncodedBytes(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(Int64) GetOpenSSLHandleForPrivateKey(
        /* [in] */ ElastosPrivateKey* privateKey);

    virtual CARAPI_(AutoPtr<ElastosPrivateKey>) CreateKey(
        /* [in] */ const String& alias);

    // @Override
    CARAPI ReleaseKey(
        /* [in] */ ElastosPrivateKey* key);

private:
    static const String TAG;
    const AutoPtr<IRemoteElastosKeyStore> mRemoteManager;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_REMOTEANDROIDKEYSTORE_H_

