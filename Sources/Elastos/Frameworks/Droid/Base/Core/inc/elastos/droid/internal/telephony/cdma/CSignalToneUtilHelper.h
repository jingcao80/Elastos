#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CSIGNALTONEUTILHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CSIGNALTONEUTILHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CSignalToneUtilHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CSignalToneUtilHelper)
    , public Singleton
    , public ISignalToneUtilHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetAudioToneFromSignalInfo(
        /* [in] */ Int32 signalType,
        /* [in] */ Int32 alertPitch,
        /* [in] */ Int32 signal,
        /* [out] */ Int32* result);
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CSIGNALTONEUTILHELPER_H__
