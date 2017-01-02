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

#include "org/conscrypt/CSSLParametersImplHelper.h"
#include "org/conscrypt/SSLParametersImpl.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CSSLParametersImplHelper)

CAR_INTERFACE_IMPL(CSSLParametersImplHelper, Singleton, ISSLParametersImpl)

ECode CSSLParametersImplHelper::GetDefaultX509TrustManager(
    /* [out] */ IX509TrustManager** result)
{
    return SSLParametersImpl::GetDefaultX509TrustManager(result);
}

ECode CSSLParametersImplHelper::GetClientKeyType(
    /* [in] */ Byte keyType,
    /* [out] */ String* result)
{
    return SSLParametersImpl::GetClientKeyType(keyType, result);
}

} // namespace Conscrypt
} // namespace Org
