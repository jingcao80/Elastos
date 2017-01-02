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

#ifndef __ELASTOS_NET_COOKIESTOREIMPL_H__
#define __ELASTOS_NET_COOKIESTOREIMPL_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Net::IURI;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

DEFINE_OBJECT_HASH_FUNC_FOR(IURI)

namespace Elastos {
namespace Net {

class CookieStoreImpl
    : public Object
    , public ICookieStore
{
public:
    CAR_INTERFACE_DECL()

    CookieStoreImpl();
    ~CookieStoreImpl();

    CARAPI Add(
        /* [in] */ IURI* uri,
        /* [in] */ IHttpCookie* cookie);

    CARAPI Get(
        /* [in] */ IURI* uri,
        /* [out] */ IList** httpCookies);

    CARAPI GetCookies(
        /* [out] */ IList** httpCookies);

    CARAPI GetURIs(
        /* [out] */ IList** URIs);

    CARAPI Remove(
        /* [in] */ IURI* uri,
        /* [in] */ IHttpCookie* cookie,
        /* [out] */ Boolean* succeeded);

    CARAPI RemoveAll(
        /* [out] */ Boolean* succeeded);

private:
    CARAPI_(AutoPtr<IURI>) GetCookiesUri(
        /* [in] */ IURI* uri);

private:
    /** this map may have null keys! */
    AutoPtr<IMap> mMap;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_COOKIESTOREIMPL_H__
