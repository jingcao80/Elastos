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

#ifndef __ELASTOS_NET_CCOOKIEPOLICYHELPER_H__
#define __ELASTOS_NET_CCOOKIEPOLICYHELPER_H__

#include "_Elastos_Net_CCookiePolicyHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CCookiePolicyHelper)
    , public Singleton
    , public ICookiePolicyHelper
{
private:
    class CookiePolicyAcceptAll
        : public Object
        , public ICookiePolicy
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ShouldAccept(
            /* [in] */ IURI* uri,
            /* [in] */ IHttpCookie* cookie,
            /* [out] */ Boolean* result);
    };

    class CookiePolicyAcceptNone
        : public Object
        , public ICookiePolicy
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ShouldAccept(
            /* [in] */ IURI* uri,
            /* [in] */ IHttpCookie* cookie,
            /* [out] */ Boolean* result);
    };

    class CookiePolicyAcceptOriginalServer
        : public Object
        , public ICookiePolicy
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ShouldAccept(
            /* [in] */ IURI* uri,
            /* [in] */ IHttpCookie* cookie,
            /* [out] */ Boolean* result);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCookiePolicy(
        /* [in] */ CookiePolicyKind kind,
        /* [out] */ ICookiePolicy** policy);

    static CARAPI_(AutoPtr<ICookiePolicy>) GetCookiePolicy(
        /* [in] */ CookiePolicyKind kind);

private:
    static AutoPtr<ICookiePolicy> ACCEPT_ALL;
    static AutoPtr<ICookiePolicy> ACCEPT_NONE;
    static AutoPtr<ICookiePolicy> ACCEPT_ORIGINAL_SERVER;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CCOOKIEPOLICYHELPER_H__
