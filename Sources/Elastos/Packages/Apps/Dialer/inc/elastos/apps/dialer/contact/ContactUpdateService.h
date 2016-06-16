#ifndef __ELASTOS_APPS_DIALER_CONTACT_CONTACTUPDATESERVICE_H__
#define __ELASTOS_APPS_DIALER_CONTACT_CONTACTUPDATESERVICE_H__

#include "elastos/droid/app/IntentService.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Contact {

/**
 * Service for updating primary number on a contact.
 */
class ContactUpdateService
    , public IntentService
    , public IContactUpdateService
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    /** Creates an intent that sets the selected data item as super primary (default) */
    static CARAPI_(AutoPtr<IIntent>) CreateSetSuperPrimaryIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int64 dataId);

protected:
    // @Override
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

};

} // Contact
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CONTACT_CONTACTUPDATESERVICE_H__
