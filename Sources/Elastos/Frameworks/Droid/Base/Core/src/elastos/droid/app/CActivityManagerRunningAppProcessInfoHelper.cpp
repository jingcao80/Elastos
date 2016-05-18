#include "elastos/droid/app/CActivityManagerRunningAppProcessInfoHelper.h"
#include "elastos/droid/app/CActivityManagerRunningAppProcessInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CActivityManagerRunningAppProcessInfoHelper)

CAR_INTERFACE_IMPL(CActivityManagerRunningAppProcessInfoHelper, \
    Singleton, IActivityManagerRunningAppProcessInfoHelper)

CARAPI CActivityManagerRunningAppProcessInfoHelper::ProcStateToImportance(
    /* [in] */ Int32 procState,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CActivityManagerRunningAppProcessInfo::ProcStateToImportance(procState);
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos

