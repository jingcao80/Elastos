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

#include "CookieHandler.h"

namespace Elastos {
namespace Net {

AutoPtr<ICookieHandler> CookieHandler::sSystemWideCookieHandler;

CAR_INTERFACE_IMPL(CookieHandler, Object, ICookieHandler)

CookieHandler::CookieHandler()
{
}

CookieHandler::~CookieHandler()
{
}

ECode CookieHandler::GetDefault(
    /* [out] */ ICookieHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = sSystemWideCookieHandler;
    REFCOUNT_ADD(*handler);

    return NOERROR;
}

ECode CookieHandler::SetDefault(
    /* [in] */ ICookieHandler* cHandler)
{
    sSystemWideCookieHandler = cHandler;
    return NOERROR;
}

ECode CookieHandler::Get(
    /* [in] */ IURI* uri,
    /* [in] */ IMap* requestHeaders,
    /* [out]*/ IMap** cookiesMap)
{
    assert(0 && "Sub class must overwrite this method.");
    return E_NOT_IMPLEMENTED;
}

ECode CookieHandler::Put(
    /* [in] */ IURI* uri,
    /* [in] */ IMap* requestHeaders)
{
    assert(0 && "Sub class must overwrite this method.");
    return E_NOT_IMPLEMENTED;
}

} // namespace Net
} // namespace Elastos
