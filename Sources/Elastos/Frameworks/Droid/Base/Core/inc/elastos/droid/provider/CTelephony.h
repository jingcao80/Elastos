#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONY_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONY_H__

#include "_Elastos_Droid_Provider_CTelephony.h"
#include "elastos/droid/provider/Telephony.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephony)
    , public Telephony
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONY_H__
