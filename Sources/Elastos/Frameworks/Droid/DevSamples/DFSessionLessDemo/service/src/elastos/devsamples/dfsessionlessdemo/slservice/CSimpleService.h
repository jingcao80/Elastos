
#ifndef __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSIMPLESERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSIMPLESERVICE_H__

#include "CService.h"
#include "_Elastos_DevSamples_DFSessionLessDemo_SLService_CSimpleService.h"

namespace Elastos {
namespace DevSamples {
namespace DFSessionLessDemo {
namespace SLService {

CarClass(CSimpleService)
    , public CService::SimpleService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace SLService
} // namespace DFSessionLessDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSIMPLESERVICE_H__
