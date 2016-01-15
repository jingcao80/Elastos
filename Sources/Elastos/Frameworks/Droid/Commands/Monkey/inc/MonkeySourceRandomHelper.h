#ifndef __MONKEYSOURCERANDOMHELPER_H__
#define __MONKEYSOURCERANDOMHELPER_H__
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeySourceRandomHelper{

public:
    void GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret);

    void GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);
    };

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYSOURCERANDOMHELPER_H__