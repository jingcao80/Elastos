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

#include "OpenSSLNativeReference.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLNativeReference::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLNativeReference, Object, IOpenSSLNativeReference);

OpenSSLNativeReference::OpenSSLNativeReference()
    : mContext(0)
{
}

ECode OpenSSLNativeReference::constructor(
    /* [in] */ Int64 ctx)
{
    if (ctx == 0) {
        //throw new NullPointerException("ctx == 0");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    mContext = ctx;
    return NOERROR;
}

ECode OpenSSLNativeReference::GetNativeContext(
    /* [out] */ Int64* ctxRef)
{
    VALIDATE_NOT_NULL(ctxRef);
    *ctxRef = mContext;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
