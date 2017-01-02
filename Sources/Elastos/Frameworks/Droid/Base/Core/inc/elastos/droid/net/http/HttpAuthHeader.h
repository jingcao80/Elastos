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

#ifndef __ELASTOS_DROID_NET_HTTP_HTTPAUTHHEADER_H__
#define __ELASTOS_DROID_NET_HTTP_HTTPAUTHHEADER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * HttpAuthHeader: a class to store HTTP authentication-header parameters.
 * For more information, see: RFC 2617: HTTP Authentication.
 *
 * {@hide}
 */
class HttpAuthHeader
    : public Object
    , public IHttpAuthHeader
{
public:
    CAR_INTERFACE_DECL()

    HttpAuthHeader();

    /**
     * Creates a new HTTP-authentication header object from the
     * input header string.
     * The header string is assumed to contain parameters of at
     * most one authentication-scheme (ensured by the caller).
     */
    CARAPI constructor(
        /* [in] */ const String& header);

    /**
     * @return True iff this is a proxy authentication header.
     */
    CARAPI IsProxy(
        /* [out] */ Boolean* result);

    /**
     * Marks this header as a proxy authentication header.
     */
    CARAPI SetProxy();

    /**
     * @return The username string.
     */
    CARAPI GetUsername(
        /* [out] */ String* result);

    /**
     * Sets the username string.
     */
    CARAPI SetUsername(
        /* [in] */ const String& username);

    /**
     * @return The password string.
     */
    CARAPI GetPassword(
        /* [out] */ String* result);

    /**
     * Sets the password string.
     */
    CARAPI SetPassword(
        /* [in] */ const String& password);

    /**
     * @return True iff this is the  BASIC-authentication request.
     */
    CARAPI IsBasic(
        /* [out] */ Boolean* result);

    /**
     * @return True iff this is the DIGEST-authentication request.
     */
    CARAPI IsDigest(
        /* [out] */ Boolean* result);

    /**
     * @return The authentication scheme requested. We currently
     * support two schemes:
     * HttpAuthHeader.BASIC  - basic, and
     * HttpAuthHeader.DIGEST - digest (algorithm=MD5, QOP="auth").
     */
    CARAPI GetScheme(
        /* [out] */ Int32* result);

    /**
     * @return True if indicating that the previous request from
     * the client was rejected because the nonce value was stale.
     */
    CARAPI GetStale(
        /* [out] */ Boolean* result);

    /**
     * @return The realm value or null if there is none.
     */
    CARAPI GetRealm(
        /* [out] */ String* result);

    /**
     * @return The nonce value or null if there is none.
     */
    CARAPI GetNonce(
        /* [out] */ String* result);

    /**
     * @return The opaque value or null if there is none.
     */
    CARAPI GetOpaque(
        /* [out] */ String* result);

    /**
     * @return The QOP ("quality-of_protection") value or null if
     * there is none. The QOP value is always lower-case.
     */
    CARAPI GetQop(
        /* [out] */ String* result);

    /**
     * @return The name of the algorithm used or null if there is
     * none. By default, MD5 is used.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    /**
     * @return True iff the authentication scheme requested by the
     * server is supported; currently supported schemes:
     * BASIC,
     * DIGEST (only algorithm="md5", no qop or qop="auth).
     */
    CARAPI IsSupportedScheme(
        /* [out] */ Boolean* result);

private:
    /**
     * Parses the header scheme name and then scheme parameters if
     * the scheme is supported.
     */
    CARAPI ParseHeader(
        /* [in] */ const String& header);

    /**
     * Parses the authentication scheme name. If we have a Digest
     * scheme, sets the algorithm value to the default of MD5.
     * @return The authentication scheme parameters string to be
     * parsed later (if the scheme is supported) or null if failed
     * to parse the scheme (the header value is null?).
     */
    CARAPI_(String) ParseScheme(
        /* [in] */ const String& header);

    /**
     * Parses a comma-separated list of authentification scheme
     * parameters.
     */
    CARAPI ParseParameters(
        /* [in] */ const String& parameters);

    /**
     * Parses a single authentication scheme parameter. The parameter
     * string is expected to follow the format: PARAMETER=VALUE.
     */
    CARAPI ParseParameter(
        /* [in] */ const String& parameter);

    /**
     * If the token is a known parameter name, parses and initializes
     * the token value.
     */
    CARAPI ParseParameter(
        /* [in] */ const String& token,
        /* [in] */ const String& value);

    /**
     * Parses and initializes the 'stale' paramer value. Any value
     * different from case-insensitive "true" is considered "false".
     */
    CARAPI ParseStale(
        /* [in] */ const String& value);

    /**
     * Trims double-quotes around a parameter value if there are any.
     * @return The string value without the outermost pair of double-
     * quotes or null if the original value is null.
     */
    static CARAPI_(String) TrimDoubleQuotesIfAny(
        /* [in] */ const String& value);

private:
    static const String REALM_TOKEN;

    static const String NONCE_TOKEN;

    static const String STALE_TOKEN;

    static const String OPAQUE_TOKEN;

    static const String QOP_TOKEN;

    static const String ALGORITHM_TOKEN;

    /**
     * An authentication scheme. We currently support two different schemes:
     * HttpAuthHeader.BASIC  - basic, and
     * HttpAuthHeader.DIGEST - digest (algorithm=MD5, QOP="auth").
     */
    Int32 mScheme;

    /**
     * A flag, indicating that the previous request from the client was
     * rejected because the nonce value was stale. If stale is TRUE
     * (case-insensitive), the client may wish to simply retry the request
     * with a new encrypted response, without reprompting the user for a
     * new username and password.
     */
    Boolean mStale;

    /**
     * A string to be displayed to users so they know which username and
     * password to use.
     */
    String mRealm;

    /**
     * A server-specified data string which should be uniquely generated
     * each time a 401 response is made.
     */
    String mNonce;

    /**
     * A string of data, specified by the server, which should be returned
     *  by the client unchanged in the Authorization header of subsequent
     * requests with URIs in the same protection space.
     */
    String mOpaque;

    /**
     * This directive is optional, but is made so only for backward
     * compatibility with RFC 2069 [6]; it SHOULD be used by all
     * implementations compliant with this version of the Digest scheme.
     * If present, it is a quoted string of one or more tokens indicating
     * the "quality of protection" values supported by the server.  The
     * value "auth" indicates authentication; the value "auth-int"
     * indicates authentication with integrity protection.
     */
    String mQop;

    /**
     * A string indicating a pair of algorithms used to produce the digest
     * and a checksum. If this is not present it is assumed to be "MD5".
     */
    String mAlgorithm;

    /**
     * Is this authentication request a proxy authentication request?
     */
    Boolean mIsProxy;

    /**
     * Username string we get from the user.
     */
    String mUsername;

    /**
     * Password string we get from the user.
     */
    String mPassword;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_HTTPAUTHHEADER_H__
