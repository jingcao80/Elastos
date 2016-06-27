#ifndef  __ELASTOS_DROID_PHONE_CHFASERVICE_H__
#define  __ELASTOS_DROID_PHONE_CHFASERVICE_H__

#include "_Elastos_Droid_TeleService_Phone_CHfaService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Service.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Service for performing HfaActivation without any UI.
 */
CarClass(CHfaService)
    , public Elastos::Droid::App::Service
{
private:
    class MyHfaLogicHfaLogicCallback
        : public Object
        , public IHfaLogicHfaLogicCallback
    {
    public:
        TO_STRING_IMPL("CHfaService::MyHfaLogicHfaLogicCallback")

        CAR_INTERFACE_DECL()

        MyHfaLogicHfaLogicCallback(
            /* [in] */ CHfaService* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnSuccess();

        //@Override
        CARAPI OnError(
            /* [in] */ const String& error);

    private:
        CHfaService* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    CARAPI_(void) OnComplete();

private:
    static const String TAG;

    AutoPtr<IHfaLogic> mHfaLogic;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CHFASERVICE_H__