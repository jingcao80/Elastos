
#include "elastos/droid/phone/CTelephonyDebugService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

String CTelephonyDebugService::TAG("TelephonyDebugService");

CAR_OBJECT_IMPL(CTelephonyDebugService)

CTelephonyDebugService::CTelephonyDebugService()
{
    Logger::D(TAG, "TelephonyDebugService()");

    assert(0);
    //mDebugService = new DebugService();
}

ECode CTelephonyDebugService::constructor()
{
    return Service::constructor();
}

ECode CTelephonyDebugService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = NULL;
    return NOERROR;
}

ECode CTelephonyDebugService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
    //mDebugService->Dump(fd, pw, args);
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos