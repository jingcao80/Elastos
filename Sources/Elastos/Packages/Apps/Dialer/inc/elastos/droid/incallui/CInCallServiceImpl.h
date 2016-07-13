
#ifndef __ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__
#define __ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__

#include "Elastos.Droid.Telecomm.h"
#include "_Elastos_Droid_InCallUI_CInCallServiceImpl.h"
#include <elastos/droid/telecomm/telecom/InCallService.h>

using Elastos::Droid::Telecomm::Telecom::InCallService;
using Elastos::Droid::Telecomm::Telecom::IPhone;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Used to receive updates about calls from the Telecomm component.  This service is bound to
 * Telecomm while there exist calls which potentially require UI. This includes ringing (incoming),
 * dialing (outgoing), and active calls. When the last call is disconnected, Telecomm will unbind to
 * the service triggering InCallActivity (via CallList) to finish soon after.
 */
CarClass(CInCallServiceImpl)
    , public InCallService
{
public:
    CAR_OBJECT_DECL();

    // @Override
    CARAPI OnPhoneCreated(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI OnPhoneDestroyed(
        /* [in] */ IPhone* phone);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__
