#include "MonkeySourceRandomHelper.h"
#include "CMonkeySourceRandom.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

void MonkeySourceRandomHelper::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret)
{
    if(!ret)
        return;
    CMonkeySourceRandom::GetKeyName(keycode, ret);
}

void MonkeySourceRandomHelper:: GetKeyCode(
    /* [in] */ const String& keyName,
    /* [out] */ Int32 *keycode)
{
    if(!keycode)
        return;
    CMonkeySourceRandom::GetKeyCode(keyName, keycode);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos