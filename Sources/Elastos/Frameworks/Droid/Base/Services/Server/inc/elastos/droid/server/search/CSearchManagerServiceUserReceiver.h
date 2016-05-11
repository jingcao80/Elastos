#ifndef  __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEUSERRECEIVER_H__
#define  __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEUSERRECEIVER_H__

#include "Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Search_CSearchManagerServiceUserReceiver.h"
#include "elastos/droid/server/search/CSearchManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CarClass(CSearchManagerServiceUserReceiver)
    , public CSearchManagerService::UserReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Elastos
} // namespace Droid
} // namespace Server
} // namespace Search

#endif // __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEUSERRECEIVER_H__