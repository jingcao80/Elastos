// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_ANDROIDCERTVERIFYRESULT_H_
#define _ELASTOS_DROID_WEBKIT_NET_ANDROIDCERTVERIFYRESULT_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import java.security.cert.CertificateEncodingException;
// import java.security.cert.X509Certificate;
// import java.util.ArrayList;
// import java.util.Collections;
// import java.util.List;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

/**
  * The result of a certification verification.
  */
// @JNINamespace("net::android")
class ElastosCertVerifyResult
    : public Object
{
public:
    ElastosCertVerifyResult(
        /* [in] */ Int32 status,
        /* [in] */ Boolean isIssuedByKnownRoot,
        /* [in] */ IList* certificateChain);

    ElastosCertVerifyResult(
        /* [in] */ Int32 status);

    // @CalledByNative
    virtual CARAPI_(Int32) GetStatus();

    // @CalledByNative
    virtual CARAPI_(Boolean) IsIssuedByKnownRoot();

    // @CalledByNative
    virtual CARAPI_(AutoPtr< ArrayOf< AutoPtr< ArrayOf<Byte> > > >) GetCertificateChainEncoded();

    static CARAPI_(void*) ElaElastosCertVerifyResultCallback_Init();

private:
    static CARAPI_(Int32) GetStatus(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) IsIssuedByKnownRoot(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > >) GetCertificateChainEncoded(
        /* [in] */ IInterface* obj);

private:
    /**
      * The verification status. One of the values in CertVerifyStatusAndroid.
      */
    const Int32 mStatus;
    /**
      * True if the root CA in the chain is in the system store.
      */
    const Boolean mIsIssuedByKnownRoot;
    /**
      * The properly ordered certificate chain used for verification.
      */
    AutoPtr<IList> mCertificateChain;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_ANDROIDCERTVERIFYRESULT_H_

