
#ifndef __ELASTOS_DROID_APP_CJOB_SCHEDULER_IMPL_H__
#define __ELASTOS_DROID_APP_CJOB_SCHEDULER_IMPL_H__

#include "_Elastos_Droid_App_CJobSchedulerImpl.h"
#include "elastos/droid/app/JobSchedulerImpl.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CJobSchedulerImpl)
    , public JobSchedulerImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CJOB_SCHEDULER_IMPL_H__