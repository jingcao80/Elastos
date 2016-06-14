
#ifndef __ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICE_H__
#define __ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICE_H__

#include "ContactUpdateService.h"
#include "_Elastos_Apps_Dialer_Contact_CContactUpdateService.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Contact {

CarClass(CContactUpdateService)
    , public ContactUpdateService
{
public:
    CAR_OBJECT_DECL()
};

} // Contact
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICE_H__
