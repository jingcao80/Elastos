
#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_DIGESTSCHEME_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_DIGESTSCHEME_H__

#include "org/apache/http/impl/auth/RFC2617Scheme.h"

using Elastos::Security::IMessageDigest;
using Elastos::Utility::IMap;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Auth::ICredentials;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

/**
 * <p>
 * Digest authentication scheme as defined in RFC 2617.
 * Both MD5 (default) and MD5-sess are supported.
 * Currently only qop=auth or no qop is supported. qop=auth-int
 * is unsupported. If auth and auth-int are provided, auth is
 * used.
 * </p>
 * <p>
 * Credential charset is configured via the
 * {@link org.apache.http.auth.params.AuthPNames#CREDENTIAL_CHARSET
 *        credential charset} parameter.
 * Since the digest username is included as clear text in the generated
 * Authentication header, the charset of the username must be compatible
 * with the
 * {@link org.apache.http.params.CoreProtocolPNames#HTTP_ELEMENT_CHARSET
 *        http element charset}.
 * </p>
 *
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @author Rodney Waldhoff
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author Ortwin Glueck
 * @author Sean C. Sullivan
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class DigestScheme : public RFC2617Scheme
{
public:
    DigestScheme();

    /**
     * Processes the Digest challenge.
     *
     * @param header the challenge header
     *
     * @throws MalformedChallengeException is thrown if the authentication challenge
     * is malformed
     */
    CARAPI ProcessChallenge(
        /* [in] */ IHeader* header);

    /**
     * Tests if the Basic authentication process has been completed.
     *
     * @return <tt>true</tt> if Basic authorization has been processed,
     *   <tt>false</tt> otherwise.
     */
    CARAPI IsComplete(
        /* [out] */ Boolean* isComplete);

    /**
     * Returns textual designation of the basic authentication scheme.
     *
     * @return <code>basic</code>
     */
    CARAPI GetSchemeName(
        /* [out] */ String* name);

    CARAPI_(void) OverrideParamter(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Returns <tt>false</tt>. Basic authentication scheme is request based.
     *
     * @return <tt>false</tt>.
     */
    CARAPI IsConnectionBased(
        /* [out] */ Boolean* connectionBased);

    /**
     * Returns a basic <tt>Authorization</tt> header value for the given
     * {@link Credentials} and charset.
     *
     * @param credentials The credentials to encode.
     * @param charset The charset to use for encoding the credentials
     *
     * @return a basic authorization header
     */
    CARAPI Authenticate(
        /* [in] */ ICredentials* credentials,
        /* [in] */ IHttpRequest* request,
        /* [out] */ IHeader** header);

    /**
     * Creates a random cnonce value based on the current time.
     *
     * @return The cnonce value as String.
     * @throws UnsupportedDigestAlgorithmException if MD5 algorithm is not supported.
     */
    static CARAPI_(String) CreateCnonce();

private:
    CARAPI_(String) GetCnonce();

    static CARAPI CreateMessageDigest(
        /* [in] */ const String& digAlg,
        /* [out] */ IMessageDigest** digest);

    /**
     * Creates an MD5 response digest.
     *
     * @return The created digest as string. This will be the response tag's
     *         value in the Authentication HTTP header.
     * @throws AuthenticationException when MD5 is an unsupported algorithm
     */
    CARAPI CreateDigest(
        /* [in] */ ICredentials* credentials,
        /* [out] */ String* digest);

    /**
     * Creates digest-response header as defined in RFC2617.
     *
     * @param credentials User credentials
     * @param digest The response tag's value as String.
     *
     * @return The digest-response as String.
     */
    CARAPI CreateDigestHeader(
        /* [in] */ ICredentials* credentials,
        /* [in] */ const String& digest,
        /* [out] */ IHeader** header);

    CARAPI_(String) GetQopVariantString();

    /**
     * Encodes the 128 bit (16 bytes) MD5 digest into a 32 characters long
     * <CODE>String</CODE> according to RFC 2617.
     *
     * @param binaryData array containing the digest
     * @return encoded MD5, or <CODE>null</CODE> if encoding failed
     */
    static CARAPI_(String) Encode(
        /* [in] */ ArrayOf<Byte>* binaryData);

private:
    /**
     * Hexa values used when creating 32 character long digest in HTTP DigestScheme
     * in case of authentication.
     *
     * @see #encode(byte[])
     */
    static const AutoPtr< ArrayOf<Char32> > HEXADECIMAL;

    static const String NC; //nonce-count is always 1
    static const Int32 QOP_MISSING = 0;
    static const Int32 QOP_AUTH_INT = 1;
    static const Int32 QOP_AUTH = 2;

    /** Whether the digest authentication process is complete */
    Boolean mComplete;

    //TODO: supply a real nonce-count, currently a server will interprete a repeated request as a replay

    Int32 mQopVariant;
    String mCnonce;
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_DIGESTSCHEME_H__
