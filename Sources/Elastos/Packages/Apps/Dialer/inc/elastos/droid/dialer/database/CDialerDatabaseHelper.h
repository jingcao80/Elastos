
#ifndef __ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPER_H__
#define __ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPER_H__

#include "elastos/apps/dialer/database/DialerDatabaseHelper.h"
#include "_Elastos_Droid_Dialer_Database_CDialerDatabaseHelper.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DATABASE_CDIALERDATABASEHELPER_H__
