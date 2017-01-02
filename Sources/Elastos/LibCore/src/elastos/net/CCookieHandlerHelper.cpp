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

#include "CCookieHandlerHelper.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CCookieHandlerHelper, Singleton, ICookieHandlerHelper)

CAR_SINGLETON_IMPL(CCookieHandlerHelper)

ECode CCookieHandlerHelper::GetDefault(
    /* [out] */ ICookieHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mSystemWideCookieHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CCookieHandlerHelper::SetDefault(
    /* [in] */ ICookieHandler* handler)
{
    mSystemWideCookieHandler = handler;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
