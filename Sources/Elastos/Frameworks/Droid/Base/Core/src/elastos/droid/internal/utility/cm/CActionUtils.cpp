#include "elastos/droid/internal/utility/cm/ActionUtils.h"
#include "elastos/droid/internal/utility/cm/CActionUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CActionUtils, Singleton, IActionUtils)

CAR_SINGLETON_IMPL(CActionUtils)

ECode CActionUtils::KillForegroundApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    return ActionUtils::KillForegroundApp(context, userId, result);
}

ECode CActionUtils::SwitchToLastApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    return ActionUtils::SwitchToLastApp(context, userId, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
