
#ifndef __ELASTOS_DROID_APP_CLOCAL_ACTIVITY_RECORD_H__
#define __ELASTOS_DROID_APP_CLOCAL_ACTIVITY_RECORD_H__

#include "_Elastos_Droid_App_CLocalActivityRecord.h"
#include "elastos/droid/app/LocalActivityManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CLocalActivityRecord)
    , public LocalActivityManager::LocalActivityRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLOCAL_ACTIVITY_RECORD_H__