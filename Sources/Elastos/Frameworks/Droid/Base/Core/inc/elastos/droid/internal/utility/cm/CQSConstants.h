#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Utility_Cm_CQSConstants.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CQSConstants)
    , public Singleton
    , public IQSConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

public:
    static AutoPtr<IArrayList> TILES_DEFAULT;
    static AutoPtr<IArrayList> TILES_AVAILABLE;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__
