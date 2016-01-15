#include "CBrickReceiver.h"
#include <elastos/utility/logging/Slogger.h>
//#include "elastos/droid/os/SystemService.h"

using Elastos::Utility::Logging::Slogger;
//using Elastos::Droid::Os::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {

ECode CBrickReceiver::OnReceive(
    /* [in] */ IContext *context,
    /* [in] */ IIntent *intent)
{
    Slogger::W(String("BrickReceiver"), String("!!! BRICKING DEVICE !!!") );
    //SystemService::Start(String("brick") );
    return NOERROR;
}

} // Server
} // Droid
} // Elastos