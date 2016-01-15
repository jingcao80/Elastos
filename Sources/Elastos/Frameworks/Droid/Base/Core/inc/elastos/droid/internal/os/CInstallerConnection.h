#ifndef __ELASTOS_DROID_INTERNAL_OS_CINSTALLERCONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_OS_CINSTALLERCONNECTION_H__

#include "_Elastos_Droid_Internal_Os_CInstallerConnection.h"
#include "elastos/droid/internal/os/InstallerConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CInstallerConnection) , public InstallerConnection
{
public:
    CAR_OBJECT_DECL()
};

} // Os
} // Internal
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CINSTALLERCONNECTION_H__
