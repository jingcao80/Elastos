
#ifndef _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_
#define _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_

#include "_Elastos_Droid_View_CSoundEffectConstantsHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSoundEffectConstantsHelper)
    , public Singleton
{
public:
    CAR_SINGLETON_DECL()

    CARAPI GetContantForFocusDirection(
        /* [in] */ Int32 direction,
        /* [out] */ Int32* result);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_

