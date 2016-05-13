#ifndef __ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICEHELPER_H__
#define __ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICEHELPER_H__

#include "_Elastos_Apps_Dialer_Contact_CContactUpdateServiceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Contact {

CarClass(CContactUpdateServiceHelper)
    , public Singleton
    , public IContactUpdateServiceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Creates an intent that sets the selected data item as super primary (default) */
    CARAPI CreateSetSuperPrimaryIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int64 dataId,
        /* [out] */ IIntent** intent);

};

} // Contact
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CONTACT_CCONTACTUPDATESERVICEHELPER_H__
