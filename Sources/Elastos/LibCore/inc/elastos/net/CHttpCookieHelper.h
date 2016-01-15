#ifndef __ELASTOS_NET_CHTTPCOOKIEHELPER_H__
#define __ELASTOS_NET_CHTTPCOOKIEHELPER_H__

#include "_Elastos_Net_CHttpCookieHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Utility::IList;

namespace Elastos {
namespace Net {

CarClass(CHttpCookieHelper)
    , public Singleton
    , public IHttpCookieHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsDomainMatches(
        /* [in] */ const String& domainPattern,
        /* [in] */ const String& host,
        /* [out] */ Boolean* match);

    CARAPI Parse(
        /* [in] */ const String& header,
        /* [out] */ IList** cookies);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CHTTPCOOKIEHELPER_H__
