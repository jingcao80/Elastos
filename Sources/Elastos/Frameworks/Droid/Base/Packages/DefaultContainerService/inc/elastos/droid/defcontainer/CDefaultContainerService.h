#ifndef __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CDEFAULTCONTAINERSERVICE_H__
#define __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CDEFAULTCONTAINERSERVICE_H__

#include "_Elastos_Droid_DefContainer_CDefaultContainerService.h"
#include "DefaultContainerService.h"
#include "elastos/coredef.h"

namespace Elastos {
namespace Droid {
namespace DefContainer {

CarClass(CDefaultContainerService)
    , public DefaultContainerService
{
public:
    CAR_OBJECT_DECL()
};

} //namespace DefContainer
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CDEFAULTCONTAINERSERVICE_H__