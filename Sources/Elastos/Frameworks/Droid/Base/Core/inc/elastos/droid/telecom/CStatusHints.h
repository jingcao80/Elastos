#ifndef __ELASTOS_DROID_TELECOM_CSTATUSHINTS_H__
#define __ELASTOS_DROID_TELECOM_CSTATUSHINTS_H__

#include "_Elastos_Droid_Telecom_CStatusHints.h"
#include "elastos/droid/telecom/StatusHints.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CStatusHints)
    , public StatusHints
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CSTATUSHINTS_H__