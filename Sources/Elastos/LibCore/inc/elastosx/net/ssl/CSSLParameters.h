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

#ifndef __ELASTOSX_NET_SSL_CSSLPARAMETERS_H__
#define __ELASTOSX_NET_SSL_CSSLPARAMETERS_H__

#include "_Elastosx_Net_Ssl_CSSLParameters.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * SSL handshake parameters that include protocols, cipher suites, and
 * client authentication requirements.
 * @since 1.6
 */
CarClass(CSSLParameters)
    , public Object
    , public ISSLParameters
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI SetCipherSuites(
        /* [in] */ ArrayOf<String>* cipherSuites);

    CARAPI GetProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols);

    CARAPI SetProtocols(
        /* [in] */ ArrayOf<String>* protocols);

    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* auth);

    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean needClientAuth);

    CARAPI GetWantClientAuth(
        /* [out] */ Boolean* auth);

    CARAPI SetWantClientAuth(
        /* [in] */ Boolean wantClientAuth);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<String>* cipherSuites);

    CARAPI constructor(
        /* [in] */ ArrayOf<String>* cipherSuites,
        /* [in] */ ArrayOf<String>* protocols);

private:
    AutoPtr<ArrayOf<String> > mCipherSuites;
    AutoPtr<ArrayOf<String> > mProtocols;
    Boolean mNeedClientAuth;
    Boolean mWantClientAuth;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_CSSLPARAMETERS_H__
