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

#ifndef __ELASTOS_DROID_NET_PROXY_H__
#define __ELASTOS_DROID_NET_PROXY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;

using Elastos::Net::IProxySelector;
using Elastos::Utility::Regex::IPattern;

using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A convenience class for accessing the user and default proxy
 * settings.
 */
class Proxy
{
public:
    /**
     * Return the proxy object to be used for the URL given as parameter.
     * @param ctx A Context used to get the settings for the proxy host.
     * @param url A URL to be accessed. Used to evaluate exclusion list.
     * @return Proxy (java.net) object containing the host name. If the
     *         user did not set a hostname it returns the default host.
     *         A null value means that no host is to be used.
     * {@hide}
     */
    static CARAPI GetProxy(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& url,
        /* [out] */ Elastos::Net::IProxy** result);

    /**
     * Return the proxy host set by the user.
     * @param ctx A Context used to get the settings for the proxy host.
     * @return String containing the host name. If the user did not set a host
     *         name it returns the default host. A null value means that no
     *         host is to be used.
     * @deprecated Use standard java vm proxy values to find the host, port
     *         and exclusion list.  This call ignores the exclusion list.
     */
    static CARAPI GetHost(
        /* [in] */ IContext* ctx,
        /* [out] */ String* result);

    /**
     * Return the proxy port set by the user.
     * @param ctx A Context used to get the settings for the proxy port.
     * @return The port number to use or -1 if no proxy is to be used.
     * @deprecated Use standard java vm proxy values to find the host, port
     *         and exclusion list.  This call ignores the exclusion list.
     */
    static CARAPI GetPort(
        /* [in] */ IContext* ctx,
        /* [out] */ Int32* result);

    /**
     * Return the default proxy host specified by the carrier.
     * @return String containing the host name or null if there is no proxy for
     * this carrier.
     * @deprecated Use standard java vm proxy values to find the host, port and
     *         exclusion list.  This call ignores the exclusion list and no
     *         longer reports only mobile-data apn-based proxy values.
     */
    static CARAPI GetDefaultHost(
        /* [out] */ String* result);

    /**
     * Return the default proxy port specified by the carrier.
     * @return The port number to be used with the proxy host or -1 if there is
     * no proxy for this carrier.
     * @deprecated Use standard java vm proxy values to find the host, port and
     *         exclusion list.  This call ignores the exclusion list and no
     *         longer reports only mobile-data apn-based proxy values.
     */
    static CARAPI GetDefaultPort(
        /* [out] */ Int32* result);

    /**
     * Returns the preferred proxy to be used by clients. This is a wrapper
     * around {@link android.net.Proxy#getHost()}.
     *
     * @param context the context which will be passed to
     * {@link android.net.Proxy#getHost()}
     * @param url the target URL for the request
     * @note Calling this method requires permission
     * android.permission.ACCESS_NETWORK_STATE
     * @return The preferred proxy to be used by clients, or null if there
     * is no proxy.
     * {@hide}
     */
    static CARAPI GetPreferredHttpHost(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [out] */ IHttpHost** result);

    /**
     * Validate syntax of hostname, port and exclusion list entries
     * {@hide}
     */
    static CARAPI Validate(
        /* [in] */ const String& hostname,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI SetHttpProxySystemProperty(
        /* [in] */ IProxyInfo* p);

    /** @hide */
    static CARAPI SetHttpProxySystemProperty(
        /* [in] */ const String& host,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList,
        /* [in] */ IUri* pacFileUrl);

private:
    static CARAPI_(AutoPtr<IPattern>) getHOSTNAME_PATTERN();

    static CARAPI_(AutoPtr<IPattern>) getEXCLLIST_PATTERN();

    static CARAPI_(AutoPtr<IProxySelector>) getDEFAULT_PROXY_SELECTOR();

    static CARAPI_(Boolean) IsLocalHost(
        /* [in] */ const String& host);

    // Hostname / IP REGEX validation
    // Matches blank input, ips, and domain names
    static const String NAME_IP_REGEX;

    static const String EXCL_REGEX;

    static const String EXCLLIST_REGEXP;

    // Set to true to enable extra debugging.
    static const Boolean DEBUG;

    static const String TAG;

    static /* const */ AutoPtr<IProxySelector> sDefaultProxySelector;

    static AutoPtr<IConnectivityManager> sConnectivityManager;

    static const String HOSTNAME_REGEXP;

    static /* const */ AutoPtr<IPattern> HOSTNAME_PATTERN;

    static /* const */ AutoPtr<IPattern> EXCLLIST_PATTERN;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_PROXY_H__
