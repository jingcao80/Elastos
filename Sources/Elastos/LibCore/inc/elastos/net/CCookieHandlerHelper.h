#ifndef __ELASTOS_NET_CCOOKIEHANDLERHELPER_H__
#define __ELASTOS_NET_CCOOKIEHANDLERHELPER_H__

#include "_Elastos_Net_CCookieHandlerHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CCookieHandlerHelper)
    , public Singleton
    , public ICookieHandlerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefault(
        /* [out] */ ICookieHandler** handler);

    CARAPI SetDefault(
        /* [in] */ ICookieHandler* handler);

private:
   AutoPtr<ICookieHandler> mSystemWideCookieHandler;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CCOOKIEHANDLERHELPER_H__
