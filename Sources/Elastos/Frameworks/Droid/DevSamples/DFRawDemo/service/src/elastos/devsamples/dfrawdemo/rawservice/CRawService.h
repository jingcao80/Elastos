
#ifndef __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CRAWSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CRAWSERVICE_H__

#include "CService.h"
#include "_Elastos_DevSamples_DFRawDemo_RawService_CRawService.h"

namespace Elastos {
namespace DevSamples {
namespace DFRawDemo {
namespace RawService {

CarClass(CRawService)
    , public CService::RawService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace RawService
} // namespace DFRawDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CRAWSERVICE_H__
