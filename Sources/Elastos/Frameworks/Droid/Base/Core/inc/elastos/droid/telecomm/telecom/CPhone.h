#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONE_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhone.h"
#include "elastos/droid/telecomm/telecom/Phone.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CPhone)
    , public Phone
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPHONE_H__