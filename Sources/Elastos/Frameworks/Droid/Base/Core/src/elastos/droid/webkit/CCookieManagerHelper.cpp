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

#include "elastos/droid/webkit/CCookieManagerHelper.h"
#include "elastos/droid/webkit/CookieManager.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CCookieManagerHelper, Object, ICookieManagerHelper);

CAR_SINGLETON_IMPL(CCookieManagerHelper);

ECode CCookieManagerHelper::GetInstance(
    /* [out] */ ICookieManager** cookieManager)
{
    VALIDATE_NOT_NULL(cookieManager);
    AutoPtr<ICookieManager> _cookieManager = CookieManager::GetInstance();
    *cookieManager = _cookieManager.Get();
    REFCOUNT_ADD(*cookieManager);
    return NOERROR;
}

ECode CCookieManagerHelper::AllowFileSchemeCookies(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CookieManager::AllowFileSchemeCookies();
    return NOERROR;
}

ECode CCookieManagerHelper::SetAcceptFileSchemeCookies(
    /* [in] */ Boolean accept)
{
    CookieManager::SetAcceptFileSchemeCookies(accept);
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
