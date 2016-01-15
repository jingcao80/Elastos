
#ifndef __ELASTOS_DROID_APP_CINSTRUMENTATION_H__
#define __ELASTOS_DROID_APP_CINSTRUMENTATION_H__

#include "_Elastos_Droid_App_CInstrumentation.h"
#include "elastos/droid/app/Instrumentation.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CInstrumentation)
    , public Instrumentation
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CINSTRUMENTATION_H__
