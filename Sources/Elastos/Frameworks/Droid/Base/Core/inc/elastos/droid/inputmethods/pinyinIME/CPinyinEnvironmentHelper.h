
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENTHELPER_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENTHELPER_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CPinyinEnvironmentHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CarClass(CPinyinEnvironmentHelper)
    , public Singleton
    , public IPinyinEnvironmentHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetInstance(
        /* [out] */ IPinyinEnvironment** env);

private:
    /**
     * The configurations are managed in a singleton.
     */
    static AutoPtr<IPinyinEnvironment> mInstance;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENTHELPER_H__
