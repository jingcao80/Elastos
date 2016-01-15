#ifndef __MONKEYUTILS_H__
#define __MONKEYUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::IDate;
using Elastos::Text::ISimpleDateFormat;
namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyUtils {
public:
    static void ToCalendarTime(
            /* [in] */ Int64 time,
            /* [out] */ String *formatedTime);

    static AutoPtr<IDate> InitStatics();

private:
    static Object mLockLock;
    MonkeyUtils();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYUTILS_H__