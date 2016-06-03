
#ifndef __CMONKEYUTILSHELPER_H__
#define __CMONKEYUTILSHELPER_H__

#include "_CMonkeyUtilsHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyUtilsHelper)
{
public:
    CAR_SINGLETON_DECL()

    CARAPI ToCalendarTime(
        /* [in] */ Int64 time,
        /* [out] */ String *formatedTime);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYUTILSHELPER_H__
