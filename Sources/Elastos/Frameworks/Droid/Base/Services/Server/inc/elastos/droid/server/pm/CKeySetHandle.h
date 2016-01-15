
#ifndef __ELASTOS_DROID_SERVER_PM_CKEYSETHANDLE_H__
#define __ELASTOS_DROID_SERVER_PM_CKEYSETHANDLE_H__

#include "_Elastos_Droid_Server_Pm_CKeySetHandle.h"
#include "elastos/droid/os/Binder.h"

using Elastos::Droid::Os::Binder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CKeySetHandle), public Binder
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CKEYSETHANDLE_H__
