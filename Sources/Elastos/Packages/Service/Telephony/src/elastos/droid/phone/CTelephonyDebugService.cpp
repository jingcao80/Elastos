
#include "elastos/droid/phone/CTelephonyDebugService.h"

namespace Elastos {
namespace Droid {
namespace Phone {

String CTelephonyDebugService::TAG("TelephonyDebugService");

CAR_INTERFACE_IMPL(CTelephonyDebugService, Service, ITelephonyDebugService)

CAR_OBJECT_IMPL(CTelephonyDebugService)

CTelephonyDebugService::CTelephonyDebugService()
{
    Logger::D(TAG, "TelephonyDebugService()");
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
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    return mDebugService->Dump(fd, pw, args);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos