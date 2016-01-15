
#ifndef __ELASTOS_NET_PROXYSELECTORIMPL_H__
#define __ELASTOS_NET_PROXYSELECTORIMPL_H__

#include "ProxySelector.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Net {

class ProxySelectorImpl
    : public ProxySelector
{
public:

    //@Override
    CARAPI ConnectFailed(
        /* [in] */ IURI* uri,
        /* [in] */ ISocketAddress* sa,
        /* [in] */ ECode ec);

    //@Override
    CARAPI Select(
        /* [in] */ IURI* uri,
        /* [out] */ IList** container);

private:
    CARAPI SelectOneProxy(
        /* [in] */ IURI* uri,
        /* [out] */ IProxy** proxy);

    /**
     * Returns the proxy identified by the {@code hostKey} system property, or
     * null.
     */
    CARAPI LookupProxy(
        /* [in] */ const String& hostKey,
        /* [in] */ const String& portKey,
        /* [in] */ ProxyType type,
        /* [in] */Int32 defaultPort,
        /* [out] */ IProxy** proxy);

    CARAPI GetSystemPropertyInt(
        /* [in] */ const String& key,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * result);

    /**
     * Returns true if the {@code nonProxyHosts} system property pattern exists
     * and matches {@code host}.
     */
    CARAPI_(Boolean) IsNonProxyHost(
        /* [in] */ const String& host,
        /* [in] */ const String& nonProxyHosts);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_PROXYSELECTORIMPL_H__
