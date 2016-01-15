
#ifndef __ELASTOS_DROID_NET_PROXYINFO_H__
#define __ELASTOS_DROID_NET_PROXYINFO_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetSocketAddress;
using Elastos::Utility::IList;

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
class ProxyInfo
    : public Object
    , public IParcelable
    , public IProxyInfo
{
public:
    CAR_INTERFACE_DECL()

    ProxyInfo();

    /**
     * Constructs a {@link ProxyInfo} object that points at a Direct proxy
     * on the specified host and port.
     */
    static CARAPI BuildDirectProxy(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ IProxyInfo** result);

    /**
     * Constructs a {@link ProxyInfo} object that points at a Direct proxy
     * on the specified host and port.
     *
     * The proxy will not be used to access any host in exclusion list, exclList.
     *
     * @param exclList Hosts to exclude using the proxy on connections for.  These
     *                 hosts can use wildcards such as *.example.com.
     */
    static CARAPI BuildDirectProxy(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IList* exclList,
        /* [out] */ IProxyInfo** result);

    /**
     * Construct a {@link ProxyInfo} that will download and run the PAC script
     * at the specified URL.
     */
    static CARAPI BuildPacProxy(
        /* [in] */ IUri* pacUri,
        /* [out] */ IProxyInfo** result);

    /**
     * Create a ProxyProperties that points at a HTTP Proxy.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& exclList);

    /**
     * Create a ProxyProperties that points at a PAC URL.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IUri* pacFileUrl);

    /**
     * Create a ProxyProperties that points at a PAC URL.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& pacFileUrl);

    /**
     * Only used in PacManager after Local Proxy is bound.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IUri* pacFileUrl,
        /* [in] */ Int32 localProxyPort);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IProxyInfo* source);

    /**
     * @hide
     */
    CARAPI GetSocketAddress(
        /* [out] */ IInetSocketAddress** result);

    /**
     * Returns the URL of the current PAC script or null if there is
     * no PAC script.
     */
    CARAPI GetPacFileUrl(
        /* [out] */ IUri** result);

    /**
     * When configured to use a Direct Proxy this returns the host
     * of the proxy.
     */
    CARAPI GetHost(
        /* [out] */ String* result);

    /**
     * When configured to use a Direct Proxy this returns the port
     * of the proxy
     */
    CARAPI GetPort(
        /* [out] */ Int32* result);

    /**
     * When configured to use a Direct Proxy this returns the list
     * of hosts for which the proxy is ignored.
     */
    CARAPI GetExclusionList(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * comma separated
     * @hide
     */
    CARAPI GetExclusionListAsString(
        /* [out] */ String* result);

    /**
     * @hide
     */
    CARAPI IsValid(
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    CARAPI MakeProxy(
        /* [out] */ Elastos::Net::IProxy** result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    /*
     * generate hashcode based on significant fields
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& exclList,
        /* [in] */ ArrayOf<String>* parsedExclList);

    // comma separated
    CARAPI SetExclusionList(
        /* [in] */ const String& exclusionList);

private:
    String mHost;

    Int32 mPort;

    String mExclusionList;

    AutoPtr<ArrayOf<String> > mParsedExclusionList;

    AutoPtr<IUri> mPacFileUrl;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_PROXYINFO_H__
