
#include "elastos/droid/server/accounts/CTestFeaturesSession.h"
#include "elastos/droid/server/accounts/CRemoveAccountSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenLabelSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenSession.h"
#include "elastos/droid/server/accounts/CAddAcountSession.h"
#include "elastos/droid/server/search/CSearchManagerServiceBootCompletedReceiver.h"
#include "elastos/droid/server/search/CSearchManagerServiceGlobalSearchProviderObserver.h"
#include "elastos/droid/server/search/CSearchManagerServiceUserReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

CAR_OBJECT_IMPL(CSearchManagerServiceBootCompletedReceiver)
CAR_OBJECT_IMPL(CSearchManagerServiceGlobalSearchProviderObserver)
CAR_OBJECT_IMPL(CSearchManagerServiceUserReceiver)

} // namespace Search
} // namespace Server
} // namespace Droid
} // namespace Elastos
