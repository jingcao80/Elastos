#ifndef __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CCONTACTUPDATEUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CCONTACTUPDATEUTILS_H__

#include "_Elastos_Apps_Contacts_Common_Database_CContactUpdateUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

/**
 * Static methods to update contact information.
 */
CarClass(CContactUpdateUtils)
    , public singleton
    , public IContactUpdateUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetSuperPrimary(
        /* [in] */ IContext* context,
        /* [in] */ Int64 dataId);
};

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CCONTACTUPDATEUTILS_H__
