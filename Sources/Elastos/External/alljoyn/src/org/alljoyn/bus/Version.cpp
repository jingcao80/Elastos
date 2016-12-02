
#include "org/alljoyn/bus/Version.h"
#include <alljoyn/version.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

String Version::Get()
{
    return String(ajn::GetVersion());
}

String Version::GetBuildInfo()
{
    return String(ajn::GetBuildInfo());
}

Int32 Version::GetNumeric()
{
    return ajn::GetNumericVersion();
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
