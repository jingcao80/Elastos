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

#include "CSSLParameters.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLParameters, Object, ISSLParameters)

CAR_OBJECT_IMPL(CSSLParameters)

ECode CSSLParameters::GetCipherSuites(
    /* [out, callee] */ ArrayOf<String>** suites)
{
    VALIDATE_NOT_NULL(suites)
    *suites = NULL;
    if (mCipherSuites != NULL) {
        *suites = mCipherSuites->Clone();
        REFCOUNT_ADD(*suites)
    }

    return NOERROR;
}

ECode CSSLParameters::SetCipherSuites(
    /* [in] */ ArrayOf<String>* cipherSuites)
{
    mCipherSuites = NULL;
    if (cipherSuites != NULL) {
        mCipherSuites = cipherSuites->Clone();
    }
    return NOERROR;
}

ECode CSSLParameters::GetProtocols(
    /* [out, callee] */ ArrayOf<String>** protocols)
{
    VALIDATE_NOT_NULL(protocols)
    *protocols = NULL;

    if (mProtocols != NULL) {
        *protocols = mProtocols->Clone();
        REFCOUNT_ADD(*protocols);
    }
    return NOERROR;
}

ECode CSSLParameters::SetProtocols(
    /* [in] */ ArrayOf<String>* protocols)
{
    mProtocols = NULL == protocols ? NULL :
        protocols->Clone();
    return NOERROR;
}

ECode CSSLParameters::GetNeedClientAuth(
    /* [out] */ Boolean* auth)
{
    VALIDATE_NOT_NULL(auth)
    *auth = mNeedClientAuth;
    return NOERROR;
}

ECode CSSLParameters::SetNeedClientAuth(
    /* [in] */ Boolean needClientAuth)
{
    mNeedClientAuth = needClientAuth;
    mWantClientAuth = FALSE;
    return NOERROR;
}

ECode CSSLParameters::GetWantClientAuth(
    /* [out] */ Boolean* auth)
{
    VALIDATE_NOT_NULL(auth)
    *auth = mWantClientAuth;
    return NOERROR;
}

ECode CSSLParameters::SetWantClientAuth(
    /* [in] */ Boolean wantClientAuth)
{
    mWantClientAuth = wantClientAuth;
    mNeedClientAuth = FALSE;
    return NOERROR;
}

ECode CSSLParameters::constructor()
{
    return NOERROR;
}

ECode CSSLParameters::constructor(
    /* [in] */ ArrayOf<String>* cipherSuites)
{
    return SetCipherSuites(cipherSuites);
}

ECode CSSLParameters::constructor(
    /* [in] */ ArrayOf<String>* cipherSuites,
    /* [in] */ ArrayOf<String>* protocols)
{
    FAIL_RETURN(SetCipherSuites(cipherSuites))
    return SetProtocols(protocols);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

