
#include "CServiceOne.h"
#include "CGetSystemTimeService.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

static const String TAG("ServiceDemo::CServiceOne");

CAR_OBJECT_IMPL(CServiceOne)

ECode CServiceOne::constructor()
{
    return NOERROR;
}

ECode CServiceOne::OnCreate()
{
    Logger::I(TAG, "OnCreate");
    return NOERROR;
}

ECode CServiceOne::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Logger::I(TAG, "OnStart: %s", TO_CSTR(intent));
    return NOERROR;
}

ECode CServiceOne::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    Logger::I(TAG, "OnBind: %s", TO_CSTR(intent));
    return CGetSystemTimeService::New(binder);
}

ECode CServiceOne::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    Logger::I(TAG, "OnUnbind: %s", TO_CSTR(intent));
    *succeeded = TRUE;
    return NOERROR;
}



} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos
