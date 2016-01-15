
#ifndef __ELASTOS_DROID_UTILITY_CSIZEF_HELPER_H__
#define __ELASTOS_DROID_UTILITY_CSIZEF_HELPER_H__

#include "_Elastos_Droid_Utility_CSizeFHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSizeFHelper)
    , public Singleton
    , public ISizeFHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Parses the specified string as a size value.
     * <p>
     * The ASCII characters {@code \}{@code u002a} ('*') and
     * {@code \}{@code u0078} ('x') are recognized as separators between
     * the width and height.</p>
     * <p>
     * For any {@code SizeF s}: {@code SizeF.parseSizeF(s.toString()).equals(s)}.
     * However, the method also handles sizes expressed in the
     * following forms:</p>
     * <p>
     * "<i>width</i>{@code x}<i>height</i>" or
     * "<i>width</i>{@code *}<i>height</i>" {@code => new SizeF(width, height)},
     * where <i>width</i> and <i>height</i> are string integers potentially
     * containing a sign, such as "-10", "+7" or "5".</p>
     *
     * <pre>{@code
     * SizeF.parseSizeF("3*+6").equals(new SizeF(3, 6)) == true
     * SizeF.parseSizeF("-3x-6").equals(new SizeF(-3, -6)) == true
     * SizeF.parseSizeF("4 by 3") => throws NumberFormatException
     * }</pre>
     *
     * @param string the string representation of a size value.
     * @return the size value represented by {@code string}.
     *
     * @throws NumberFormatException if {@code string} cannot be parsed
     * as a size value.
     * @throws NullPointerException if {@code string} was {@code null}
     */
    CARAPI ParseSizeF(
        /* [in] */ const String& string,
        /* [out] */ ISizeF** outration);
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CSIZEF_HELPER_H__
