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

#ifndef __ELASTOS_NET_CCOOKIEMANAGER_H__
#define __ELASTOS_NET_CCOOKIEMANAGER_H__

#include "_Elastos_Net_CCookieManager.h"
#include "CookieHandler.h"
#include <elastos/utility/etl/List.h>

using Elastos::Core::ICharSequence;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Net {

CarClass(CCookieManager)
    , public CookieHandler
    , public ICookieManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCookieManager();

    virtual ~CCookieManager();

    /**
     * Constructs a new cookie manager.
     *
     * The invocation of this constructor is the same as the invocation of
     * CookieManager(null, null).
     *
     */
    CARAPI constructor();

    /**
     * Constructs a new cookie manager using a specified cookie store and a
     * cookie policy.
     *
     * @param store
     *            a CookieStore to be used by cookie manager. The manager will
     *            use a default one if the arg is null.
     * @param cookiePolicy
     *            a CookiePolicy to be used by cookie manager
     *            ACCEPT_ORIGINAL_SERVER will be used if the arg is null.
     */
    CARAPI constructor(
        /* [in] */ ICookieStore* store,
        /* [in] */ ICookiePolicy* cookiePolicy);

    /**
     * Sets the cookie policy of this cookie manager.
     *
     * ACCEPT_ORIGINAL_SERVER is the default policy for CookieManager.
     *
     * @param cookiePolicy
     *            the cookie policy. if null, the original policy will not be
     *            changed.
     */
    CARAPI SetCookiePolicy(
        /* [in] */ ICookiePolicy* cookiePolicy);

    /**
     * Gets current cookie store.
     *
     * @return the cookie store currently used by cookie manager.
     */
    CARAPI GetCookieStore(
        /* [out] */ ICookieStore** cookieStore);

    CARAPI Get(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* requestHeaders,
        /* [out] */ IMap** cookiesMap);

    /**
     * Sets cookies according to uri and responseHeaders
     *
     * @param uri
     *            the specified uri
     * @param responseHeaders
     *            a list of request headers
     * @throws IOException
     *             if some error of I/O operation occurs
     */
    CARAPI Put(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* responseHeaders);

    /**
     * Returns a cookie-safe path by truncating everything after the last "/".
     * When request path like "/foo/bar.html" yields a cookie, that cookie's
     * default path is "/foo/".
     */
    static CARAPI_(String) PathToCookiePath(
        /* [in] */ const String& path);

private:
    static CARAPI CookiesToHeaders(
        /* [in] */ List< AutoPtr<IHttpCookie> >& cookies,
        /* [out] */ IMap** cookiesMap);

    static CARAPI_(AutoPtr<List< AutoPtr<IHttpCookie> > >) ParseCookie(
        /* [in] */ IMap* responseHeaders);

private:
    AutoPtr<ICookieStore> mStore;

    AutoPtr<ICookiePolicy> mPolicy;

    static const String VERSION_ZERO_HEADER;

    static const String VERSION_ONE_HEADER;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CCOOKIEMANAGER_H__
