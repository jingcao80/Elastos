
#ifndef __ORG_APACHE_HTTP_CLIENT_UTILS_URIUTILS_H__
#define __ORG_APACHE_HTTP_CLIENT_UTILS_URIUTILS_H__

#include "Elastos.CoreLibrary.Apache.h"

using Elastos::Net::IURI;
using Org::Apache::Http::IHttpHost;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Utils {

/**
 * A collection of utilities for {@link URI URIs}, to workaround
 * bugs within the class or for ease-of-use features.
 */
class URIUtils
{
private:
    /**
     * This class should not be instantiated.
     */
    URIUtils() {}

public:
    /**
     * Constructs a {@link URI} using all the parameters. This should be
     * used instead of
     * {@link URI#URI(String, String, String, int, String, String, String)}
     * or any of the other URI multi-argument URI constructors.
     *
     * See <a
     * href="https://issues.apache.org/jira/browse/HTTPCLIENT-730">HTTPCLIENT-730</a>
     * for more information.
     *
     * @param scheme
     *            Scheme name
     * @param host
     *            Host name
     * @param port
     *            Port number
     * @param path
     *            Path
     * @param query
     *            Query
     * @param fragment
     *            Fragment
     *
     * @throws URISyntaxException
     *             If both a scheme and a path are given but the path is
     *             relative, if the URI string constructed from the given
     *             components violates RFC&nbsp;2396, or if the authority
     *             component of the string is present but cannot be parsed
     *             as a server-based authority
     */
    static CARAPI CreateURI(
        /* [in] */ const String& scheme,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& path,
        /* [in] */ const String& query,
        /* [in] */ const String& fragment,
        /* [out] */ IURI** uri);

    /**
     * A convenience method for creating a new {@link URI} whose scheme, host
     * and port are taken from the target host, but whose path, query and
     * fragment are taken from the existing URI. The fragment is only used if
     * dropFragment is false.
     *
     * @param uri
     *            Contains the path, query and fragment to use.
     * @param target
     *            Contains the scheme, host and port to use.
     * @param dropFragment
     *            True if the fragment should not be copied.
     *
     * @throws URISyntaxException
     *             If the resulting URI is invalid.
     */
    static CARAPI RewriteURI(
        /* [in] */ IURI* inUri,
        /* [in] */ IHttpHost* target,
        /* [in] */ Boolean dropFragment,
        /* [out] */ IURI** uri);

    /**
     * A convenience method for
     * {@link URIUtils#rewriteURI(URI, HttpHost, boolean)} that always keeps the
     * fragment.
     */
    static CARAPI RewriteURI(
        /* [in] */ IURI* inUri,
        /* [in] */ IHttpHost* target,
        /* [out] */ IURI** uri);

    /**
     * Resolves a URI reference against a base URI. Work-around for bug in
     * java.net.URI (<http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=4708535>)
     *
     * @param baseURI the base URI
     * @param reference the URI reference
     * @return the resulting URI
     */
    static CARAPI Resolve(
        /* [in] */ IURI* baseURI,
        /* [in] */ const String& reference,
        /* [out] */ IURI** uri);

    /**
     * Resolves a URI reference against a base URI. Work-around for bug in
     * java.net.URI (<http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=4708535>)
     *
     * @param baseURI the base URI
     * @param reference the URI reference
     * @return the resulting URI
     */
    static CARAPI Resolve(
        /* [in] */ IURI* baseURI,
        /* [in] */ IURI* reference,
        /* [out] */ IURI** uri);
};

} // namespace Utils
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_UTILS_URIUTILS_H__
