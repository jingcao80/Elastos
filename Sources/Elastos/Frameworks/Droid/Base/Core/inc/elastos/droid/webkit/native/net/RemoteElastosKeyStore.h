// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_REMOTEANDROIDKEYSTORE_H_
#define _ELASTOS_DROID_WEBKIT_NET_REMOTEANDROIDKEYSTORE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/ElastosKeyStore.h"
#include "elastos/droid/webkit/native/net/ElastosPrivateKey.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.net;
// import android.os.RemoteException;
// import android.util.Log;

using Elastos::Droid::Webkit::Net::ElastosKeyStore;
using Elastos::Droid::Webkit::Net::ElastosPrivateKey;
using Elastos::Droid::Webkit::Net::IRemoteElastosKeyStore;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_REMOTEANDROIDKEYSTORE_H_

