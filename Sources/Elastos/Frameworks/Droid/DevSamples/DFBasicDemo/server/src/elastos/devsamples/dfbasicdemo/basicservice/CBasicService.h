
#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__

#include "CService.h"
#include "_Elastos_DevSamples_DFBasicDemo_BasicService_CBasicService.h"

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicService {

CarClass(CBasicService)
    , public CService::BasicService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace BasicService
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__
