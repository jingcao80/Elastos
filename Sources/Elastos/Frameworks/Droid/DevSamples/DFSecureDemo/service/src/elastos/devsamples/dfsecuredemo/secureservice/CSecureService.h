
#ifndef __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSECURESERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSECURESERVICE_H__

#include "CService.h"
#include "_Elastos_DevSamples_DFSecureDemo_SecureService_CSecureService.h"

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace SecureService {

CarClass(CSecureService)
    , public CService::SecureService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace SecureService
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSECURESERVICE_H__
