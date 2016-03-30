#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CPOWERMENUCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CPOWERMENUCONSTANTS_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CPowerMenuConstants.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CPowerMenuConstants)
    , public Singleton
    , public IPowerMenuConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAllActions(
        /* [out, callee] */ ArrayOf<String>** result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CPOWERMENUCONSTANTS_H__
