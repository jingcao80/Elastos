#include "org/apache/harmony/luni/internal/util/TimezoneGetter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

AutoPtr<ITimezoneGetter> TimezoneGetter::instance;
CAR_INTERFACE_IMPL(TimezoneGetter, Object, ITimezoneGetter)

AutoPtr<ITimezoneGetter> TimezoneGetter::GetInstance()
{
    return instance;
}

ECode TimezoneGetter::SetInstance(
    /* [in] */ ITimezoneGetter* getter)
{
    if (instance != NULL) {
        //throw new UnsupportedOperationException("TimezoneGetter instance already set");
        Logger::E("TimezoneGetter", "TimezoneGetter instance already set");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    instance = getter;
    return NOERROR;
}

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org
