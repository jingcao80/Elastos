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

#ifndef __ORG_APACHE_HTTP_UTILITY_ENTITYUTILS_H_
#define __ORG_APACHE_HTTP_UTILITY_ENTITYUTILS_H_

#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpEntity;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

/**
 * Static helpers for dealing with {@link HttpEntity entities}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 569637 $
 *
 * @since 4.0
 */
class EntityUtils
{
public:
    static CARAPI ToByteArray(
        /* [in] */ IHttpEntity* entity,
        /* [out] */ ArrayOf<Byte>** bytes);

    static CARAPI GetContentCharSet(
        /* [in] */ IHttpEntity* entity,
        /* [out] */ String* str);

    static CARAPI ToString(
        /* [in] */ IHttpEntity* entity,
        /* [in] */ const String& defaultCharset,
        /* [out] */ String* str);

    static CARAPI ToString(
        /* [in] */ IHttpEntity* entity,
        /* [out] */ String* str);

private:
    EntityUtils() {}
};

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_UTILITY_ENTITYUTILS_H_
