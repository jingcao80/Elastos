#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDPROXY_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CIccCardProxy.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/telephony/uicc/IccCardProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CIccCardProxy)
    , public IccCardProxy
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDPROXY_H__
