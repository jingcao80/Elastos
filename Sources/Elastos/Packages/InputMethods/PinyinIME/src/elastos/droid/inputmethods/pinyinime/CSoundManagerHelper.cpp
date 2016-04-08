
#include "elastos/droid/ext/frameworkdef.h"
#include "CSoundManagerHelper.h"
#include "CSoundManager.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

AutoPtr<ISoundManager> CSoundManagerHelper::mInstance;
CAR_INTERFACE_IMPL(CSoundManagerHelper, Singleton, ISoundManagerHelper)
CAR_SINGLETON_IMPL(CSoundManagerHelper)

ECode CSoundManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ ISoundManager** sm)
{
    VALIDATE_NOT_NULL(sm);
    if (NULL == mInstance) {
        if (NULL != context) {
            CSoundManager::New(context, (ISoundManager**)&mInstance);
        }
    }

    *sm = mInstance;
    REFCOUNT_ADD(*sm);
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
