
#include "CMonkeyUtilsHelper.h"
#include "MonkeyUtils.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyUtilsHelper::ToCalendarTime(
    /* [in] */ Int64 time,
    /* [out] */ String *formatedTime)
{
    VALIDATE_NOT_NULL(formatedTime);
    MonkeyUtils::ToCalendarTime(time, formatedTime);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
