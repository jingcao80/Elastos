
#include "elastos/droid/incallui/CallerInfoAsyncQuery.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

void CallerInfoAsyncQuery::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ CallerInfo* info,
    /* [in] */ IOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie)
{
    Logger::E("CallerInfoAsyncQuery", "TODO CallerInfoAsyncQuery::StartQuery");
    //assert(0);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
