
#include "elastos/droid/phone/CHfaService.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CHfaService::MyHfaLogicHfaLogicCallback, Object, IHfaLogicHfaLogicCallback)

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnSuccess()
{
    Logger::I(TAG, "onSuccess");
    return mHost->OnComplete();
}

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnError(
    /* [in] */ const String& error)
{
    StringBuilder sb;
    sb += "onError: ";
    sb += msg;
    Logger::I(TAG, sb.ToString());
    // We do not respond from this service. On success or failure
    // we do the same thing...finish.
    return mHost->OnComplete();
}

const String CHfaService::TAG("CHfaService");// = HfaService.class.getSimpleName();

CAR_INTERFACE_IMPL(CHfaService, Service, IHfaService)

CAR_OBJECT_IMPL(CHfaService)

ECode CHfaService::constructor()
{
    return Service::constructor();
}

ECode CHfaService::OnCreate()
{
    return Loger::I(TAG, "service started");
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

    CHfaLogic::New(this, c, otaResponseIntent, (IHfaLogic**)&mHfaLogic);
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