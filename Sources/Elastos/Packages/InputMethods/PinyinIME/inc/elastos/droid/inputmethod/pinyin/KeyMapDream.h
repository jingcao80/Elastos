
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to map the symbols on Dream's hardware keyboard to corresponding
 * Chinese full-width symbols.
 */
class KeyMapDream
{
public:
    static CARAPI_(Char32) GetChineseLabel(
        /* [in] */ Int32 keyCode);

private:
    // Number of shift bits to store full-width symbols
    static const Int32 SHIFT_FWCH = 8;
    static const Int32 mKeyMap[];
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__
