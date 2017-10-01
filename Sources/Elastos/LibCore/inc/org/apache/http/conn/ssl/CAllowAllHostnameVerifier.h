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

#ifndef __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_

#include "_Org_Apache_Http_Conn_SSL_CAllowAllHostnameVerifier.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * The ALLOW_ALL HostnameVerifier essentially turns hostname verification
 * off. This implementation is a no-op, and never throws the SSLException.
 *
 * @author Julius Davies
 */
CarClass(CAllowAllHostnameVerifier)
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

#endif // __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_
