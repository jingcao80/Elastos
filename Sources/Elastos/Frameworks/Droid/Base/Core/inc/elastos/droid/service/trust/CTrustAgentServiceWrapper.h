#ifndef __ELASTOS_DROID_SERVICE_TRUST_CTRUSTAGENTSERVICEWRAPPER_H__
#define __ELASTOS_DROID_SERVICE_TRUST_CTRUSTAGENTSERVICEWRAPPER_H__

#include "_Elastos_Droid_Service_Trust_CTrustAgentServiceWrapper.h"
#include "elastos/droid/service/trust/TrustAgentService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Trust {

CarClass(CTrustAgentServiceWrapper)
    , public TrustAgentService::TrustAgentServiceWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Trust
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_TRUST_CTRUSTAGENTSERVICEWRAPPER_H__