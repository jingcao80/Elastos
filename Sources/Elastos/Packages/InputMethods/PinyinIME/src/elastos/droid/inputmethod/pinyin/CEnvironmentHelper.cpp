
#include "elastos/droid/ext/frameworkdef.h"
#include "CEnvironmentHelper.h"
#include "CEnvironment.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

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

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
