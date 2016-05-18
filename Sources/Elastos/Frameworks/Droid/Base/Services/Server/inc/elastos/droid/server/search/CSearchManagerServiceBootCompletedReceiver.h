#ifndef  __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEBOOTCOMPLETEDRECEIVER_H__
#define  __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEBOOTCOMPLETEDRECEIVER_H__

#include "Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Search_CSearchManagerServiceBootCompletedReceiver.h"
#include "elastos/droid/server/search/CSearchManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CarClass(CSearchManagerServiceBootCompletedReceiver)
    , public CSearchManagerService::BootCompletedReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Server
} // namespace Search

#endif // __ELASTOS_DROID_SERVER_SEARCH_CSEARCHMANAGERSERVICEBOOTCOMPLETEDRECEIVER_H__