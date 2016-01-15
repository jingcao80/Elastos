
#ifndef __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_
#define __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_

#include "elastos/core/Object.h"

using Elastos::Core::IThrowable;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

/**
 * A set of utility methods to help produce consistent
 * {@link Object#equals equals} and {@link Object#hashCode hashCode} methods.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class LangUtils
{
public:
    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ Int32 hashcode);

    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ Boolean b);

    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ IObject* obj);

    static CARAPI_(Boolean) Equals(
        /* [in] */ IObject* obj1,
        /* [in] */ IObject* obj2);

    static CARAPI_(Boolean) Equals(
        /* [in] */ ArrayOf<IObject*>* a1,
        /* [in] */ ArrayOf<IObject*>* a2);

private:
    LangUtils() {}

public:
    static const Int32 HASH_SEED = 17;
    static const Int32 HASH_OFFSET = 37;
};

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_
