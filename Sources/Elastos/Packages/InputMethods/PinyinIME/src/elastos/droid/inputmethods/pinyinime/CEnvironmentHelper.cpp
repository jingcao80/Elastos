
#include "elastos/droid/ext/frameworkdef.h"
#include "CEnvironmentHelper.h"
#include "CEnvironment.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

AutoPtr<IEnvironment> CEnvironmentHelper::mInstance;

ECode CEnvironmentHelper::GetInstance(
    /* [out] */ IEnvironment** env)
{
    VALIDATE_NOT_NULL(env);
    if (NULL == mInstance) {
        CEnvironment::New((IEnvironment**)&mInstance);
    }
    *env = mInstance;
    REFCOUNT_ADD(*env);
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
