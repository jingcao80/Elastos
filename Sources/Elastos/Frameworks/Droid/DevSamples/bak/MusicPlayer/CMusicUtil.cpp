#include "CMusicUtil.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;

namespace MusicPlayer {

HashMap<Int32, AutoPtr<IServiceConnection> > CMusicUtil::sConnectionMap;

CMusicUtil :: CMusicUtil()
{}

ECode CMusicUtil::BindToService(
    /* [in] */ IContext* context,
    /* [in] */ IServiceConnection* conn)
{
    Logger::E("CMusicService", "BindToService");
//    AutoPtr<ServiceConnectionWrapper> connWrap = new ServiceConnectionWrapper(conn);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(context, String("MusicPlayer.CMusicService"));

    AutoPtr<IComponentName> componentName;
    // Use StartService to Make use MusicService will not exist untill call StopService
    ECode ec = context->StartService(intent, (IComponentName**)&componentName);
    if (FAILED(ec) || componentName == NULL) {
        Logger::E("CMusicUtil", "StartService return ERROR\n");
        return ec;
    }

    intent = NULL;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(context, String("MusicPlayer.CMusicService"));
    Boolean result;
    ec = context->BindService(intent, conn, IContext::BIND_AUTO_CREATE, &result);
    if (result) {
        sConnectionMap[(Int32)context] = conn;
        return NOERROR;
    }

    Logger::E("CMusicService", "BindService return FALSE");

    return ec;
}

ECode CMusicUtil::UnbindFromService(
    /* [in] */ IContext* context)
{
    HashMap<Int32, AutoPtr<IServiceConnection> >::Iterator it =
            sConnectionMap.Find((Int32)context);
    if (it != sConnectionMap.End()) {
        AutoPtr<IServiceConnection> conn = it->mSecond;
        context->UnbindService(conn);
        sConnectionMap.Erase(it);
    }

    //  sPlayController->RemoveOnTickListener(context);

    Int32 count = sConnectionMap.GetSize();
    Logger::D("CMusicUtil", "UnbindFromService() count=%d", count);

#if 0
    if(count <= 0) {
      delete sService;
      sService = NULL;
    }
#endif

    return NOERROR;
}

} // namespace MusicPlayer

