
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDdsSchedulerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/* {@hide} */
CarClass(CDdsSchedulerHelper)
    , public Singleton
    , public IDdsSchedulerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IDdsScheduler** result);

    CARAPI Init();
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULERHELPER_H__
