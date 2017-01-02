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

#include "org/conscrypt/OpenSSLCipherContext.h"
#include "org/conscrypt/NativeCrypto.h"

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLCipherContext, Object, IOpenSSLCipherContext)

OpenSSLCipherContext::OpenSSLCipherContext()
    : mContext(0)
{}

OpenSSLCipherContext::~OpenSSLCipherContext()
{
    // try {
    NativeCrypto::EVP_CIPHER_CTX_cleanup(mContext);
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLCipherContext::constructor(
    /* [in] */ Int64 ctx)
{
    if (ctx == 0) {
        // throw new NullPointerException("ctx == 0");
        return E_NULL_POINTER_EXCEPTION;
    }

    mContext = ctx;
    return NOERROR;
}

ECode OpenSSLCipherContext::GetContext(
    /* [out] */  Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContext;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
