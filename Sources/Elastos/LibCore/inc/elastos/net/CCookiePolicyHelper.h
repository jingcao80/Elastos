
#ifndef __ELASTOS_NET_CCOOKIEPOLICYHELPER_H__
#define __ELASTOS_NET_CCOOKIEPOLICYHELPER_H__

#include "_Elastos_Net_CCookiePolicyHelper.h"
#include "Singleton.h"

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
