#ifndef __ELASTOS_DROID_SERVER_CPERSISTENT_DATA_BLOCK_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CPERSISTENT_DATA_BLOCK_SERVICE_H__

#include "_Elastos_Droid_Server_CPersistentDataBlockService.h"
#include "elastos/droid/server/PersistentDataBlockService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CPersistentDataBlockService)
    , public PersistentDataBlockService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CPERSISTENT_DATA_BLOCK_SERVICE_H__
