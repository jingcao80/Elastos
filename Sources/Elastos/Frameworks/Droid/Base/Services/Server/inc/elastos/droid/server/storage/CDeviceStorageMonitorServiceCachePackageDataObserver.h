#ifndef __ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICECACHEPACKAGEDATAOBSERVER_H__
#define __ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICECACHEPACKAGEDATAOBSERVER_H__

#include "_Elastos_Droid_Server_Storage_CDeviceStorageMonitorServiceCachePackageDataObserver.h"
#include "elastos/droid/server/storage/CDeviceStorageMonitorService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Storage {

CarClass(CDeviceStorageMonitorServiceCachePackageDataObserver)
    , public CDeviceStorageMonitorService::CachePackageDataObserver
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Storage
}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_STORAGE_CDEVICESTORAGEMONITORSERVICECACHEPACKAGEDATAOBSERVER_H__
