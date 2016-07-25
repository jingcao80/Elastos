#ifndef __ELASTOS_DROID_TELECOM_CINCALLADAPTER_H__
#define __ELASTOS_DROID_TELECOM_CINCALLADAPTER_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telecom_CInCallAdapter.h"
#include "elastos/droid/telecom/InCallAdapter.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CInCallAdapter)
    , public InCallAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CINCALLADAPTER_H__
