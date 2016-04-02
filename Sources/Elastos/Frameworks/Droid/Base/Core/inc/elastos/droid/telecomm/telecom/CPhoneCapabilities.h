#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONECAPABILITIES_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONECAPABILITIES_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhoneCapabilities.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Defines capabilities a phone call can support, such as conference calling and video telephony.
 * Also defines properties of a phone call, such as whether it is using VoLTE technology.
 * @hide
 */
CarClass(CPhoneCapabilities)
    , public Singleton
    , public IPhoneCapabilities
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [in] */ Int32 capabilities,
        /* [out] */ String* result);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPHONECAPABILITIES_H__