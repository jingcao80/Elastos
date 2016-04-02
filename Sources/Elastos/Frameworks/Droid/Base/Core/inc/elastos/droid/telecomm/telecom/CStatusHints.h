#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CSTATUSHINTS_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CSTATUSHINTS_H__

#include "_Elastos_Droid_Telecomm_Telecom_CStatusHints.h"
#include "elastos/droid/telecomm/telecom/StatusHints.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CStatusHints)
    , public StatusHints
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CSTATUSHINTS_H__