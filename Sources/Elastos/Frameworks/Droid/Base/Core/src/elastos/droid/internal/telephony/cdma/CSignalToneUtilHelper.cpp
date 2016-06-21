#include "elastos/droid/internal/telephony/cdma/CSignalToneUtilHelper.h"
#include "elastos/droid/internal/telephony/cdma/SignalToneUtil.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CSignalToneUtilHelper, Singleton, ISignalToneUtilHelper)
CAR_SINGLETON_IMPL(CSignalToneUtilHelper)

ECode CSignalToneUtilHelper::GetAudioToneFromSignalInfo(
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SignalToneUtil::GetAudioToneFromSignalInfo(signalType, alertPitch, signal);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
