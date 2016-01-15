
#ifndef __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENTS_H__
#define __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENTS_H__

#include "_Elastos_Droid_App_Usage_CUsageEvents.h"
#include "elastos/droid/app/usage/UsageEvents.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

CarClass(CUsageEvents)
    , public UsageEvents
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CUSAGE_EVENTS_H__