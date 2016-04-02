#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHELPER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHELPER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhoneAccountHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CPhoneAccountHelper)
    , public Singleton
    , public IPhoneAccountHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Builder(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ ICharSequence* label,
        /* [out] */ IPhoneAccountBuilder** res);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHELPER_H__