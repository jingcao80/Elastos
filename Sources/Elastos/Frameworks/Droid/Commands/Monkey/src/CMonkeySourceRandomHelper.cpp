
#include "CMonkeySourceRandomHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeySourceRandomHelper::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret)
{
    VALIDATE_NOT_NULL(ret);
    MonkeySourceRandomHelper::GetKeyName(keycode, ret);
    return NOERROR;
}

ECode CMonkeySourceRandomHelper::GetKeyCode(
    /* [in] */ const String& keyName,
    /* [out] */ Int32 *keycode)
{
    VALIDATE_NOT_NULL(keycode);
    MonkeySourceRandomHelper::GetKeyCode(keyName, keycode);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
