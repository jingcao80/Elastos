
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITORHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITORHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsUsageMonitorHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsUsageMonitorHelper)
    , public Singleton
    , public ISmsUsageMonitorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI MergeShortCodeCategories(
        /* [in] */ Int32 type1,
        /* [in] */ Int32 type2,
        /* [in] */ Int32* result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITORHELPER_H__
