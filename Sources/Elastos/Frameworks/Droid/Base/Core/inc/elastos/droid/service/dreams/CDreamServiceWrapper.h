
#ifndef __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICEWRAPPER_H__
#define __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICEWRAPPER_H__

#include "_Elastos_Droid_Service_Dreams_CDreamServiceWrapper.h"
#include "elastos/droid/service/dreams/DreamService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CarClass(CDreamServiceWrapper)
    , public DreamService::DreamServiceWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICEWRAPPER_H__
