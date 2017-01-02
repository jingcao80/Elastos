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

#ifndef __ORG_APACHE_HTTP_CONN_SSL_ABSTRACTVERIFIER_H__
#define __ORG_APACHE_HTTP_CONN_SSL_ABSTRACTVERIFIER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICharSequence;
using Elastos::Security::Cert::IX509Certificate;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::IHostnameVerifier;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * Abstract base class for all standard {@link X509HostnameVerifier}
 * implementations.
 *
 * @author Julius Davies
 */
class AbstractVerifier
    : public Object
    , public IAbstractVerifier
    , public IX509HostnameVerifier
    , public IHostnameVerifier
{
public:
    AbstractVerifier() {}

    virtual ~AbstractVerifier() {}

    CAR_INTERFACE_DECL()

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ISSLSocket* ssl);

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ISSLSession* session,
        /* [out] */ Boolean* result);

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ IX509Certificate* cert);

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<String>* cns,
        /* [in] */ ArrayOf<String>* subjectAlts,
        /* [in] */ Boolean strictWithSubDomains);

    using IX509HostnameVerifier::Verify;

    static CARAPI_(Boolean) AcceptableCountryWildcard(
        /* [in] */ const String& cn);

    static CARAPI_(AutoPtr< ArrayOf<String> >) GetCNs(
        /* [in] */ IX509Certificate* cert);


    /**
     * Extracts the array of SubjectAlt DNS names from an X509Certificate.
     * Returns null if there aren't any.
     * <p/>
     * Note:  Java doesn't appear able to extract international characters
     * from the SubjectAlts.  It can only extract international characters
     * from the CN field.
     * <p/>
     * (Or maybe the version of OpenSSL I'm using to test isn't storing the
     * international characters correctly in the SubjectAlts?).
     *
     * @param cert X509Certificate
     * @return Array of SubjectALT DNS names stored in the certificate.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) GetDNSSubjectAlts(
        /* [in] */ IX509Certificate* cert);

    /**
     * Counts the number of dots "." in a string.
     * @param s  string to count dots from
     * @return  number of dots
     */
    static CARAPI_(Int32) CountDots(
        /* [in] */ const String& s);

private:
    /**
     * This contains a list of 2nd-level domains that aren't allowed to
     * have wildcards when combined with country-codes.
     * For example: [*.co.uk].
     * <p/>
     * The [*.co.uk] problem is an interesting one.  Should we just hope
     * that CA's would never foolishly allow such a certificate to happen?
     * Looks like we're the only implementation guarding against this.
     * Firefox, Curl, Sun Java 1.4, 5, 6 don't bother with this check.
     */
    static const AutoPtr< ArrayOf<ICharSequence*> > BAD_COUNTRY_2LDS;

};

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SSL_ABSTRACTVERIFIER_H__
