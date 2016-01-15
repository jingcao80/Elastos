
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
