#ifndef __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSIONSERVICE_H__
#define __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSIONSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::App::IIVoiceInteractionManagerService;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

/**
 * An active voice interaction session, initiated by a {@link VoiceInteractionService}.
 */
class VoiceInteractionSessionService
    : public Elastos::Droid::App::Service
    , public IVoiceInteractionSessionService
{
private:
    class MInterface
        : public Object
        , public IIVoiceInteractionSessionService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MInterface(
            /* [in] */ VoiceInteractionSessionService* host);

        CARAPI NewSession(
            /* [in] */ IBinder* token,
            /* [in] */ IBundle* args);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        VoiceInteractionSessionService* mHost;
    };

    class MHandlerCallerCallback
        : public Object
        , public IHandlerCallerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MHandlerCallerCallback(
            /* [in] */ VoiceInteractionSessionService* host);

        // @Override
        CARAPI ExecuteMessage(
            /* [in] */ IMessage* msg);

    private:
        VoiceInteractionSessionService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VoiceInteractionSessionService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    virtual CARAPI OnNewSession(
        /* [in] */ IBundle* args,
        /* [out] */ IVoiceInteractionSession** session) = 0;

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

protected:
    CARAPI DoNewSession(
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* args);

protected:
    const static Int32 MSG_NEW_SESSION = 1;

    AutoPtr<IIVoiceInteractionManagerService> mSystemService;
    AutoPtr<IVoiceInteractionSession> mSession;

    AutoPtr<IIVoiceInteractionSessionService> mInterface;

    AutoPtr<IHandlerCaller> mHandlerCaller;

    AutoPtr<IHandlerCallerCallback> mHandlerCallerCallback;
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSIONSERVICE_H__
