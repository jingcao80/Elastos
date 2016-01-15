#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_CNAT_464_XLATH__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_CNAT_464_XLATH__

#include "_Elastos_Droid_Server_Connectivity_CNat464Xlat.h"
#include "elastos/droid/server/connectivity/Nat464Xlat.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

CarClass(CNat464Xlat)
    , public Nat464Xlat
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Connectivity
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_CNAT_464_XLATH__