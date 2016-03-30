#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_POWERMENUCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_POWERMENUCONSTANTS_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class PowerMenuConstants
{
public:
    static CARAPI GetAllActions(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    static AutoPtr<ArrayOf<String> > ALL_ACTIONS;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_POWERMENUCONSTANTS_H__
