
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITOR_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsUsageMonitor.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsUsageMonitor), public SmsUsageMonitor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSUSAGEMONITOR_H__
