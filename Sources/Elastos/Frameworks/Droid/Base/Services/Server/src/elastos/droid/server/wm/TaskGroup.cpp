
#include "wm/TaskGroup.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

ECode TaskGroup::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("id=") + StringUtils::ToString(mTaskId) + String("tokens=")/* + mTokens */;
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
