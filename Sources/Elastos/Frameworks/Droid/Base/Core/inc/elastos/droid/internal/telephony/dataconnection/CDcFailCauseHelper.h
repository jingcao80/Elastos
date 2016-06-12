
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcFailCauseHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Returned as the reason for a connection failure as defined
 * by RIL_DataCallFailCause in ril.h and some local errors.
 */
CarClass(CDcFailCauseHelper)
    , public Singleton
    , public IDcFailCauseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromInt32(
        /* [in] */ Int32 errorCode,
        /* [out] */ IDcFailCause** result);
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSEHELPER_H__
