
#ifndef __ELASTOS_DROID_INTERNAL_OS_CRUNTIMEINIT_H__
#define __ELASTOS_DROID_INTERNAL_OS_CRUNTIMEINIT_H__

#include "_Elastos_Droid_Internal_Os_CRuntimeInit.h"
#include "RuntimeInit.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CRuntimeInit)
    , public RuntimeInit
    , public IRuntimeInit
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CRUNTIMEINIT_H__
