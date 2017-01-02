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

#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__

#include "_Elastos_Droid_Dialer_Dialpad_CLatinSmartDialMap.h"
#include <elastos/core/Object.h>

namespace Elastos{
namespace Droid{
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CLATINSMARTDIALMAP_H__
