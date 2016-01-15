
#ifndef __ELASTOS_DROID_APP_JOB_CJOBINFO_BUILDER_H__
#define __ELASTOS_DROID_APP_JOB_CJOBINFO_BUILDER_H__

#include "_Elastos_Droid_App_Job_CJobInfoBuilder.h"
#include "elastos/droid/app/job/CJobInfo.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

CarClass(CJobInfoBuilder)
    , public JobInfoBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_JOB_CJOBINFO_BUILDER_H__