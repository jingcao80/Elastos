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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDCERTVERIFYRESULT_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDCERTVERIFYRESULT_H_

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
namespace Webview {
namespace Chromium {
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDCERTVERIFYRESULT_H_

