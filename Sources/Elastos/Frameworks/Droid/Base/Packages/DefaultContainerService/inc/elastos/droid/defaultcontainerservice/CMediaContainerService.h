#ifndef __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CMEDIACONTAINERSERVICE_H__
#define __ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CMEDIACONTAINERSERVICE_H__

#include "_Elastos_Droid_DefaultContainerService_CMediaContainerService.h"
#include "DefaultContainerService.h"
#include "elastos/coredef.h"

namespace Elastos {
namespace Droid {
namespace DefaultContainerService {

CarClass(CMediaContainerService), public DefaultContainerService::MediaContainerService
{
public:
    CAR_OBJECT_DECL()
};


} //namespace DefaultContainerService
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_DEFAULTCONTAINERSERVICE_CMEDIACONTAINERSERVICE_H__