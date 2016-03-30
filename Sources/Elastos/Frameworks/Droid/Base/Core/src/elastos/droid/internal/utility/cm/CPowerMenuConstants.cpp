#include "elastos/droid/internal/utility/cm/PowerMenuConstants.h"
#include "elastos/droid/internal/utility/cm/CPowerMenuConstants.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CPowerMenuConstants, Singleton, IPowerMenuConstants)

CAR_SINGLETON_IMPL(CPowerMenuConstants)

ECode CPowerMenuConstants::GetAllActions(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return PowerMenuConstants::GetAllActions(result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
