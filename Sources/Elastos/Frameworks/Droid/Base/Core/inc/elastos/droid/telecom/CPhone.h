#ifndef __ELASTOS_DROID_TELECOM_CPHONE_H__
#define __ELASTOS_DROID_TELECOM_CPHONE_H__

#include "_Elastos_Droid_Telecom_CPhone.h"
#include "elastos/droid/telecom/Phone.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CPhone)
    , public Phone
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPHONE_H__