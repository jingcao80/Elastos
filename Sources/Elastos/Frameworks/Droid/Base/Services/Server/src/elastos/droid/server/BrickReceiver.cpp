
#include <Elastos.Droid.Os.h>
#include "elastos/droid/server/BrickReceiver.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::ISystemService;
using Elastos::Droid::Os::CSystemService;

namespace Elastos {
namespace Droid {
namespace Server {

ECode BrickReceiver::OnReceive(
    /* [in] */ IContext *context,
    /* [in] */ IIntent *intent)
{
    Slogger::W("BrickReceiver", "!!! BRICKING DEVICE !!!");
    AutoPtr<Elastos::Droid::Os::ISystemService> systemService;
    CSystemService::AcquireSingleton((Elastos::Droid::Os::ISystemService**)&systemService);
    return systemService->Start(String("brick"));
}

ECode BrickReceiver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "BrickReceiver";
    return NOERROR;
}

} // Server
} // Droid
} // Elastos