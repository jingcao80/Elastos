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

#include "org/conscrypt/COpenSSLECPointContextHelper.h"
#include "org/conscrypt/OpenSSLECPointContext.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECPointContextHelper)

CAR_INTERFACE_IMPL(COpenSSLECPointContextHelper, Singleton, IOpenSSLECPointContext)

ECode COpenSSLECPointContextHelper::GetInstance(
    /* [in] */ Int32 curveType,
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ IECPoint* javaPoint,
    /* [out] */ IOpenSSLECPointContext** result)
{
    return OpenSSLECPointContext::GetInstance(
            curveType, group, javaPoint, result);
}

} // namespace Conscrypt
} // namespace Org
