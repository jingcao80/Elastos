#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CINCALLADAPTER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CINCALLADAPTER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CInCallAdapter.h"
#include "elastos/droid/telecomm/telecom/InCallAdapter.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CInCallAdapter)
    , public InCallAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CINCALLADAPTER_H__