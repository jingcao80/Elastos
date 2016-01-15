#ifndef __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICE_H__
#define __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICE_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Soundtrigger::IKeyphraseEnrollmentInfo;
using Elastos::Droid::Internal::App::IIVoiceInteractionManagerService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Service::Voice::IAlwaysOnHotwordDetector;
using Elastos::Droid::Service::Voice::IAlwaysOnHotwordDetectorCallback;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

class VoiceInteractionService
    : public Elastos::Droid::App::Service
    , public IVoiceInteractionService
{
private:
    class MInterface
        : public Object
        , public IIVoiceInteractionService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MInterface(
            /* [in] */ VoiceInteractionService* host);

        // @Override
        CARAPI Ready();

        // @Override
        CARAPI Shutdown();

        // @Override
        CARAPI SoundModelsChanged();

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        VoiceInteractionService* mHost;
    };

protected:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ VoiceInteractionService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        VoiceInteractionService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VoiceInteractionService();

    CARAPI constructor();

    /**
     * Check whether the given service component is the currently active
     * VoiceInteractionService.
     */
    static CARAPI IsActiveService(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* service,
        /* [out] */ Boolean* isActiveService);

    /**
     * Initiate the execution of a new {@link android.service.voice.VoiceInteractionSession}.
     * @param args Arbitrary arguments that will be propagated to the session.
     */
    CARAPI StartSession(
        /* [in] */ IBundle* args);

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    /**
     * Called during service initialization to tell you when the system is ready
     * to receive interaction from it. You should generally do initialization here
     * rather than in {@link #onCreate()}. Methods such as {@link #startSession(Bundle)} and
     * {@link #createAlwaysOnHotwordDetector(String, Locale, android.service.voice.AlwaysOnHotwordDetector.Callback)}
     * will not be operational until this point.
     */
    CARAPI OnReady();

    /**
     * Called during service de-initialization to tell you when the system is shutting the
     * service down.
     * At this point this service may no longer be the active {@link VoiceInteractionService}.
     */
    CARAPI OnShutdown();

    /**
     * Creates an {@link AlwaysOnHotwordDetector} for the given keyphrase and locale.
     * This instance must be retained and used by the client.
     * Calling this a second time invalidates the previously created hotword detector
     * which can no longer be used to manage recognition.
     *
     * @param keyphrase The keyphrase that's being used, for example "Hello Android".
     * @param locale The locale for which the enrollment needs to be performed.
     * @param callback The callback to notify of detection events.
     * @return An always-on hotword detector for the given keyphrase and locale.
     */
    CARAPI CreateAlwaysOnHotwordDetector(
        /* [in] */ const String& keyphrase,
        /* [in] */ ILocale* locale,
        /* [in] */ IAlwaysOnHotwordDetectorCallback* callback,
        /* [out] */ IAlwaysOnHotwordDetector** aohd);

protected:
    /**
     * @return Details of keyphrases available for enrollment.
     * @hide
     */
    // @VisibleForTesting
    CARAPI GetKeyphraseEnrollmentInfo(
        /* [out] */ IKeyphraseEnrollmentInfo** kei);

private:
    CARAPI OnShutdownInternal();

    CARAPI OnSoundModelsChangedInternal();

    CARAPI SafelyShutdownHotwordDetector();

protected:
    AutoPtr<IIVoiceInteractionService> mInterface;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<IIVoiceInteractionManagerService> mSystemService;

    const static Int32 MSG_READY = 1;
    const static Int32 MSG_SHUTDOWN = 2;
    const static Int32 MSG_SOUND_MODELS_CHANGED = 3;

private:
    Object mLock;

    AutoPtr<IKeyphraseEnrollmentInfo> mKeyphraseEnrollmentInfo;

    AutoPtr<IAlwaysOnHotwordDetector> mHotwordDetector;
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICE_H__
