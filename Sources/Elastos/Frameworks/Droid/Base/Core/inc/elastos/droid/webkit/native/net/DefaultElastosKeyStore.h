// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_DEFAULTANDROIDKEYSTORE_H_
#define _ELASTOS_DROID_WEBKIT_NET_DEFAULTANDROIDKEYSTORE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/ElastosKeyStore.h"
#include "elastos/droid/webkit/native/net/ElastosPrivateKey.h"

// package org.chromium.net;
// import android.util.Log;
// import java.lang.reflect.Method;
// import java.security.NoSuchAlgorithmException;
// import java.security.PrivateKey;
// import java.security.Signature;
// import java.security.interfaces.DSAKey;
// import java.security.interfaces.DSAParams;
// import java.security.interfaces.DSAPrivateKey;
// import java.security.interfaces.ECKey;
// import java.security.interfaces.ECPrivateKey;
// import java.security.interfaces.RSAKey;
// import java.security.interfaces.RSAPrivateKey;
// import java.security.spec.ECParameterSpec;

using Elastos::Droid::Webkit::Net::ElastosKeyStore;
using Elastos::Droid::Webkit::Net::ElastosPrivateKey;
using Elastos::Security::IPrivateKey;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

/**
  * Simple implementation of the ElastosKeyStore for use with an in-process Java KeyStore.
  */
class DefaultElastosKeyStore
    : public ElastosKeyStore
{
private:
    class DefaultElastosPrivateKey
        : public ElastosPrivateKey
    {
    public:
        DefaultElastosPrivateKey(
            /* [in] */ IPrivateKey* key,
            /* [in] */ DefaultElastosKeyStore* store);

        virtual CARAPI_(AutoPtr<IPrivateKey>) GetJavaKey();

        // @Override
        CARAPI_(AutoPtr<ElastosKeyStore>) GetKeyStore();

    public:
        // The actual Java key being wrapped.
        const AutoPtr<IPrivateKey> mKey;
        // Key store handling this key.
        const AutoPtr<DefaultElastosKeyStore> mStore;
    };

public:
    virtual CARAPI_(AutoPtr<ElastosPrivateKey>) CreateKey(
        /* [in] */ IPrivateKey* javaKey);

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
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetPrivateKeyEncodedBytes(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(AutoPtr< ArrayOf<Byte> >) RawSignDigestWithPrivateKey(
        /* [in] */ ElastosPrivateKey* key,
        /* [in] */ ArrayOf<Byte>* message);

    // @Override
    CARAPI_(Int32) GetPrivateKeyType(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI_(Int64) GetOpenSSLHandleForPrivateKey(
        /* [in] */ ElastosPrivateKey* key);

    // @Override
    CARAPI ReleaseKey(
        /* [in] */ ElastosPrivateKey* key);

private:
    static const String TAG;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_DEFAULTANDROIDKEYSTORE_H_

