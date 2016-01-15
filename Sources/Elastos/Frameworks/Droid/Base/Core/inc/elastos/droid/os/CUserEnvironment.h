#ifndef __ELASTOS_DROID_OS_CUSERENVIRONMENT_H__
#define __ELASTOS_DROID_OS_CUSERENVIRONMENT_H__

#include "_Elastos_Droid_Os_CUserEnvironment.h"
#include "elastos/droid/os/Environment.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CUserEnvironment)
    , public Environment::UserEnvironment
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUSERENVIRONMENT_H__
