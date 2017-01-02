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

#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_FRC2617SCHEME_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_FRC2617SCHEME_H__

#include "org/apache/http/impl/auth/AuthSchemeBase.h"

using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

/**
 * Abstract authentication scheme class that lays foundation for all
 * RFC 2617 compliant authetication schemes and provides capabilities common
 * to all authentication schemes defined in RFC 2617.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
*/
class RFC2617Scheme : public AuthSchemeBase
{
public:
    RFC2617Scheme();

    /**
     * Returns authentication parameter with the given name, if available.
     *
     * @param name The name of the parameter to be returned
     *
     * @return the parameter with the given name
     */
    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ String* param);

    /**
     * Returns authentication realm. The realm may not be null.
     *
     * @return the authentication realm
     */
    CARAPI GetRealm(
        /* [out] */ String* realm);

protected:
    CARAPI ParseChallenge(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len);

    /**
     * Returns authentication parameters map. Keys in the map are lower-cased.
     *
     * @return the map of authentication parameters
     */
    CARAPI_(AutoPtr<IMap>) GetParameters();

private:
    /**
     * Authentication parameter map.
     */
    AutoPtr<IMap> mParams;
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__
