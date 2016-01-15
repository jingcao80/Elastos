
#ifndef __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__
#define __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__

#include "_Elastos_Droid_App_Usage_CUsageEvent.h"
#include "elastos/droid/app/usage/UsageEvents.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

CarClass(CUsageEvent)
    , public UsageEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__
