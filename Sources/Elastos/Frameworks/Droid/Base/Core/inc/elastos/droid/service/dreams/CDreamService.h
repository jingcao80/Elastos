
#ifndef __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICE_H__
#define __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICE_H__

#include "_Elastos_Droid_Service_Dreams_CDreamService.h"
#include "elastos/droid/service/dreams/DreamService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CarClass(CDreamService)
    , public DreamService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_DREAMS_CDREAMSERVICE_H__
