
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDctControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CarClass(CDctControllerHelper)
    , public Singleton
    , public IDctControllerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IDctController** result);

    CARAPI MakeDctController(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones,
        /* [in] */ ILooper* looper,
        /* [out] */ IDctController** result);
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLERHELPER_H__
