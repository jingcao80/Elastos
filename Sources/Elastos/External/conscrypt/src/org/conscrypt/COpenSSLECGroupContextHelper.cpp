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

#include "org/conscrypt/COpenSSLECGroupContextHelper.h"
#include "org/conscrypt/COpenSSLECGroupContext.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECGroupContextHelper)

CAR_INTERFACE_IMPL(COpenSSLECGroupContextHelper, Singleton, IOpenSSLECGroupContext)

ECode COpenSSLECGroupContextHelper::GetCurveByName(
    /* [in] */ const String& curveName,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetCurveByName(curveName, result);
}

ECode COpenSSLECGroupContextHelper::GetInstance(
    /* [in] */ Int32 type,
    /* [in] */ IBigInteger* p,
    /* [in] */ IBigInteger* a,
    /* [in] */ IBigInteger* b,
    /* [in] */ IBigInteger* x,
    /* [in] */ IBigInteger* y,
    /* [in] */ IBigInteger* n,
    /* [in] */ IBigInteger* h,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetInstance(
            type, p, a, b, x, y, n, h, result);
}

ECode COpenSSLECGroupContextHelper::GetInstance(
    /* [in] */ IECParameterSpec* params,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetInstance(params, result);
}

} // namespace Conscrypt
} // namespace Org
