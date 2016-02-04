#ifndef __ELASTOS_DROID_SERVER_CPERSISTERNT_DATA_BLOCK_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CPERSISTERNT_DATA_BLOCK_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_CPersistentDataBlockBinderService.h"
#include "elastos/droid/server/PersistentDataBlockService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CPersistentDataBlockBinderService)
    , public PersistentDataBlockService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CPERSISTERNT_DATA_BLOCK_BINDER_SERVICE_H__
