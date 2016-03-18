#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_PHONECAPABILITIES_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_PHONECAPABILITIES_H__

#include "Elastos.Droid.Telecomm.h"
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
class PhoneCapabilities
    : public Singleton
    , public IPhoneCapabilities
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI ToString(
        /* [in] */ Int32 capabilities,
        /* [out] */ String* result);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_PHONECAPABILITIES_H__