
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/SettingInjectorService.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CMessageHelper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IService;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String SettingInjectorService::TAG("SettingInjectorService");

CAR_INTERFACE_IMPL(SettingInjectorService, Service, ISettingInjectorService)

SettingInjectorService::SettingInjectorService(
    /* [in] */ const String& name)
    : mName(name)
{
}

ECode SettingInjectorService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = NULL;
    return NOERROR;
}

ECode SettingInjectorService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    return Service::OnStart(intent, startId);
}

ECode SettingInjectorService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    OnHandleIntent(intent);
    FAIL_RETURN(Service::StopSelf(startId))
    *result = IService::START_NOT_STICKY;
    return NOERROR;
}

ECode SettingInjectorService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    Boolean enabled = FALSE;
    ECode ec = OnGetEnabled(&enabled);
    if (FAILED(ec)) {
        SendStatus(intent, TRUE);
        return E_RUNTIME_EXCEPTION;
    }
    SendStatus(intent, enabled);
    return NOERROR;
}

ECode SettingInjectorService::SendStatus(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IMessage> message = CMessage::Obtain();
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutBoolean(ISettingInjectorService::ENABLED_KEY, enabled);
    message->SetData(bundle);

    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        String str1, str2;
        IObject::Probe(intent)->ToString(&str1);
        IObject::Probe(message)->ToString(&str2);
        Logger::D(TAG, "%s: received %s, enabled=%s, sending message: %s",
            mName.string(), str1.string(), enabled ? "TRUE" : "FALSE", str2.string());
    }

    AutoPtr<IParcelable> par;
    intent->GetParcelableExtra(ISettingInjectorService::MESSENGER_KEY, (IParcelable**)&par);
    AutoPtr<IMessenger> messenger = IMessenger::Probe(par);
    ECode ec = messenger->Send(message);
    if (FAILED(ec)) {
        StringBuilder sb(mName);
        sb += ": sending dynamic status failed";
        Logger::E(TAG, sb.ToString());
    }
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
