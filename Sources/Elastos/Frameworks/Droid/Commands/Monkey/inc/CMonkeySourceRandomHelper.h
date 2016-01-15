
#ifndef __CMONKEYSOURCERANDOMHELPER_H__
#define __CMONKEYSOURCERANDOMHELPER_H__

#include "_CMonkeySourceRandomHelper.h"
#include "MonkeySourceRandomHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandomHelper), public MonkeySourceRandomHelper
{
public:
    CARAPI GetKeyName(
        /* [in] */ Int32 keycode,
        /* [out] */ String *ret);

    CARAPI GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOMHELPER_H__
