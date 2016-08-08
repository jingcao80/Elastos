
#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CLOWSTORAGEMONITOR_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CLOWSTORAGEMONITOR_H__

#include "_Elastos_Droid_Providers_Telephony_CLowStorageMonitor.h"
#include "elastos/droid/providers/telephony/MmsSmsDatabaseHelper.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CLowStorageMonitor)
    , public MmsSmsDatabaseHelper::LowStorageMonitor
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CLOWSTORAGEMONITOR_H__
