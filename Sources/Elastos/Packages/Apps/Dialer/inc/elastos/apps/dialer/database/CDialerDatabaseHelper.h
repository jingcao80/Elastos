
#ifndef __ELASTOS_APPS_DIALER_DATABASE_CDIALERDATABASEHELPER_H__
#define __ELASTOS_APPS_DIALER_DATABASE_CDIALERDATABASEHELPER_H__

#include "elastos/apps/dialer/database/DialerDatabaseHelper.h"
#include "_Elastos_Apps_Dialer_Database_CDialerDatabaseHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Database {

CarClass(CDialerDatabaseHelper)
    , public DialerDatabaseHelper
{
public:
    CAR_OBJECT_DECL();
};

} // Database
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DATABASE_CDIALERDATABASEHELPER_H__
