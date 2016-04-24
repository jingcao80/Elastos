
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOUNDMANAGERHELPER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOUNDMANAGERHELPER_H__

#include <elastos/core/Singleton.h>
#include "_Elastos_Droid_Inputmethods_PinyinIME_CSoundManagerHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Inputmethods::PinyinIME::ISoundManager;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CarClass(CSoundManagerHelper)
    , public Singleton
    , public ISoundManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ ISoundManager** sm);

private:
    static AutoPtr<ISoundManager> mInstance;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOUNDMANAGERHELPER_H__
