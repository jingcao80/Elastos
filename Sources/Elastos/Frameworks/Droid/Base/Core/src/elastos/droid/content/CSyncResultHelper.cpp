
#include "elastos/droid/content/CSyncResultHelper.h"
#include "elastos/droid/content/CSyncResult.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CSyncResultHelper, Singleton, ISyncResultHelper)

CAR_SINGLETON_IMPL(CSyncResultHelper)

ECode CSyncResultHelper::GetALREADY_IN_PROGRESS(
    /* [out] */ ISyncResult** syncResult)
{
    VALIDATE_NOT_NULL(syncResult)
    *syncResult = CSyncResult::ALREADY_IN_PROGRESS;
    REFCOUNT_ADD(*syncResult)
    return NOERROR;
}

}
}
}
