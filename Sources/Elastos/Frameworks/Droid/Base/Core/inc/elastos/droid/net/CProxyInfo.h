
#ifndef __ELASTOS_DROID_NET_CPROXYINFO_H__
#define __ELASTOS_DROID_NET_CPROXYINFO_H__

#include "_Elastos_Droid_Net_CProxyInfo.h"
#include "elastos/droid/net/ProxyInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes a proxy configuration.
 *
 * Proxy configurations are already integrated within the Apache HTTP stack.
 * So {@link URLConnection} and {@link HttpClient} will use them automatically.
 *
 * Other HTTP stacks will need to obtain the proxy info from
 * {@link Proxy#PROXY_CHANGE_ACTION} broadcast as the extra {@link Proxy#EXTRA_PROXY_INFO}.
 */
CarClass(CProxyInfo)
    , public ProxyInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CPROXYINFO_H__
