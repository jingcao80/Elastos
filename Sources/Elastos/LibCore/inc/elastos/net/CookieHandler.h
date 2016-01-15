
#ifndef __ELASTOS_NET_COOKIEHANDLER_H__
#define __ELASTOS_NET_COOKIEHANDLER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Utility::IMap;
using Elastos::Net::ICookieHandler;

namespace Elastos {
namespace Net {

class CookieHandler
    : public Object
    , public ICookieHandler
{
public:
    CAR_INTERFACE_DECL()

    CookieHandler();

    virtual ~CookieHandler();

    static CARAPI GetDefault(
        /* [out] */ ICookieHandler** handler);

    static CARAPI SetDefault(
        /* [in] */ ICookieHandler* cHandler);

    CARAPI Get(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* requestHeaders,
        /* [out]*/ IMap** cookiesMap);

    CARAPI Put(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* requestHeaders);

private:
    static AutoPtr<ICookieHandler> sSystemWideCookieHandler;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_COOKIEHANDLER_H__
