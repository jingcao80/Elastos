
#include "elastos/droid/phone/CHfaService.h"
#include "elastos/droid/phone/HfaLogic.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CHfaService::MyHfaLogicHfaLogicCallback, Object, IHfaLogicHfaLogicCallback)

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnSuccess()
{
    Logger::I(TAG, "onSuccess");
    mHost->OnComplete();
    return NOERROR;
}

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnError(
    /* [in] */ const String& error)
{
    StringBuilder sb;
    sb += "onError: ";
    sb += error;
    Logger::I(TAG, sb.ToString());
    // We do not respond from this service. On success or failure
    // we do the same thing...finish.
    mHost->OnComplete();
    return NOERROR;
}

const String CHfaService::TAG("CHfaService");// = HfaService.class.getSimpleName();

CAR_OBJECT_IMPL(CHfaService)

ECode CHfaService::constructor()
{
    return Service::constructor();
}

ECode CHfaService::OnCreate()
{
    return Logger::I(TAG, "service started");
}

ECode CHfaService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT, (IParcelable**)&parcelable);
    AutoPtr<IPendingIntent> otaResponseIntent = IPendingIntent::Probe(parcelable);

    AutoPtr<IHfaLogicHfaLogicCallback> c = new MyHfaLogicHfaLogicCallback(this);

    mHfaLogic = new HfaLogic(this, c, otaResponseIntent);
    mHfaLogic->Start();

    *result = START_REDELIVER_INTENT;
    return NOERROR;
}

ECode CHfaService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = NULL;
    return NOERROR;
}

void CHfaService::OnComplete()
{
    StopSelf();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos