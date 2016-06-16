#ifndef  __ELASTOS_DROID_PHONE_CHFASERVICE_H__
#define  __ELASTOS_DROID_PHONE_CHFASERVICE_H__

#include "_Elastos_Droid_Phone_CHfaService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Service for performing HfaActivation without any UI.
 */
CarClass(CHfaService)
    , public Service
    , public IHfaService
{
private:
    class MyHfaLogicHfaLogicCallback
        : public Object
        , public IHfaLogicHfaLogicCallback
    {
    public:
        TO_STRING_IMPL("CHfaService::MyHfaLogicHfaLogicCallback")

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
    CAR_INTERFACE_DECL()

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
    static const String TAG = HfaService.class.getSimpleName();

    AutoPtr<IHfaLogic> mHfaLogic;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CHFASERVICE_H__