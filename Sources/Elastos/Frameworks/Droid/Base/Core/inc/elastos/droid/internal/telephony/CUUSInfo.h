
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CUUSINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CUUSINFO_H__

#include "_Elastos_Droid_Internal_Telephony_CUUSInfo.h"
#include "elastos/droid/internal/telephony/UUSInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CUUSInfo)
    , public UUSInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CUUSINFO_H__
