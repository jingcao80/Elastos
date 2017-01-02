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

#ifndef __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_

#include "_Org_Apache_Http_Conn_SSL_CBrowserCompatHostnameVerifier.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * The HostnameVerifier that works the same way as Curl and Firefox.
 * <p/>
 * The hostname must match either the first CN, or any of the subject-alts.
 * A wildcard can occur in the CN, and in any of the subject-alts.
 * <p/>
 * The only difference between BROWSER_COMPATIBLE and STRICT is that a wildcard
 * (such as "*.foo.com") with BROWSER_COMPATIBLE matches all subdomains,
 * including "a.b.foo.com".
 *
 * @author Julius Davies
 */
CarClass(CBrowserCompatHostnameVerifier)
    , public AbstractVerifier
{
public:
    CAR_OBJECT_DECL()

    using AbstractVerifier::Verify;

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<String>* cns,
        /* [in] */ ArrayOf<String>* subjectAlts);

    CARAPI ToString(
        /* [out] */ String* str);

};

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_
