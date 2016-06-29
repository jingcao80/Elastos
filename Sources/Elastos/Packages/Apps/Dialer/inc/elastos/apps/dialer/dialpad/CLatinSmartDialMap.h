
#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__

#include "_Elastos_Apps_Dialer_Dialpad_CLatinSmartDialMap.h"
#include <elastos/core/Object.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CarClass(CLatinSmartDialMap)
    , public Object
    , public ILatinSmartDialMap
    , public ISmartDialMap
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI IsValidDialpadAlphabeticChar(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    CARAPI IsValidDialpadNumericChar(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    CARAPI IsValidDialpadCharacter(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    /*
     * The switch statement in this function was generated using the python code:
     * from unidecode import unidecode
     * for i in range(192, 564):
     *     char = unichr(i)
     *     decoded = unidecode(char)
     *     # Unicode characters that decompose into multiple characters i.e.
     *     #  into ss are not supported for now
     *     if (len(decoded) == 1 and decoded.isalpha()):
     *         print "case '" + char + "': return '" + unidecode(char) +  "';"
     *
     * This gives us a way to map characters containing accents/diacritics to their
     * alphabetic equivalents. The unidecode library can be found at:
     * http://pypi.python.org/pypi/Unidecode/0.04.1
     *
     * Also remaps all upper case latin characters to their lower case equivalents.
     */
    CARAPI NormalizeCharacter(
        /* [in] */ Char32 ch,
        /* [out] */ Char32* result);

    CARAPI GetDialpadIndex(
        /* [in] */ Char32 ch,
        /* [out] */ Byte* result);

    CARAPI GetDialpadNumericCharacter(
        /* [in] */ Char32 ch,
        /* [out] */ Char32* result);

private:
    static const Char32 LATIN_LETTERS_TO_DIGITS[];
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__
