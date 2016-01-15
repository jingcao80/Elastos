
#ifndef __ELASTOS_DROID_SERVICE_PERSISTENTDATA_CPERSISTENTDATABLOCKMANAGER_H__
#define __ELASTOS_DROID_SERVICE_PERSISTENTDATA_CPERSISTENTDATABLOCKMANAGER_H__

#include "_Elastos_Droid_Service_Persistentdata_CPersistentDataBlockManager.h"
#include "elastos/droid/service/persistentdata/PersistentDataBlockManager.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Persistentdata {

CarClass(CPersistentDataBlockManager)
    , public PersistentDataBlockManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Persistentdata
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_PERSISTENTDATA_CPERSISTENTDATABLOCKMANAGER_H__
