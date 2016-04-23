#ifndef __ELASTOS_DROID_SERVICE_VOICE_ALWAYSONHOTWORDDETECTOR_H__
#define __ELASTOS_DROID_SERVICE_VOICE_ALWAYSONHOTWORDDETECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Hardware::Soundtrigger::IIRecognitionStatusCallback;
using Elastos::Droid::Hardware::Soundtrigger::IKeyphraseEnrollmentInfo;
using Elastos::Droid::Hardware::Soundtrigger::IKeyphraseMetadata;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTrigger;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTriggerKeyphraseRecognitionEvent;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTriggerKeyphraseRecognitionExtra;
using Elastos::Droid::Internal::App::IIVoiceInteractionManagerService;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::IInteger32;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

/**
 * A class that lets a VoiceInteractionService implementation interact with
 * always-on keyphrase detection APIs.
 */
class AlwaysOnHotwordDetector
    : public Object
    , public IAlwaysOnHotwordDetector
{
public:
     /**
     * Additional payload for {@link Callback#onDetected}.
     */
    class EventPayload
        : public Object
        , public IAlwaysOnHotwordDetectorEventPayload

    {
    public:
        CAR_INTERFACE_DECL()

        EventPayload();

        CARAPI constructor(
            /* [in] */ Boolean triggerAvailable,
            /* [in] */ Boolean captureAvailable,
            /* [in] */ IAudioFormat* audioFormat,
            /* [in] */ Int32 captureSession,
            /* [in] */ ArrayOf<Byte>* data);

        /**
         * Gets the format of the audio obtained using {@link #getTriggerAudio()}.
         * May be null if there's no audio present.
         */
        // @Nullable
        CARAPI GetCaptureAudioFormat(
            /* [out] */ IAudioFormat** af);

        /**
         * Gets the raw audio that triggered the keyphrase.
         * This may be null if the trigger audio isn't available.
         * If non-null, the format of the audio can be obtained by calling
         * {@link #getCaptureAudioFormat()}.
         *
         * @see AlwaysOnHotwordDetector#RECOGNITION_FLAG_CAPTURE_TRIGGER_AUDIO
         */
        // @Nullable
        CARAPI GetTriggerAudio(
            /* [out, callee] */ ArrayOf<Byte>** ta);

        /**
         * Gets the session ID to start a capture from the DSP.
         * This may be null if streaming capture isn't possible.
         * If non-null, the format of the audio that can be captured can be
         * obtained using {@link #getCaptureAudioFormat()}.
         *
         * TODO: Candidate for Public API when the API to start capture with a session ID
         * is made public.
         *
         * TODO: Add this to {@link #getCaptureAudioFormat()}:
         * "Gets the format of the audio obtained using {@link #getTriggerAudio()}
         * or {@link #getCaptureSession()}. May be null if no audio can be obtained
         * for either the trigger or a streaming session."
         *
         * TODO: Should this return a known invalid value instead?
         *
         * @hide
         */
        // @Nullable
        CARAPI GetCaptureSession(
            /* [out] */ IInteger32** cs);

    private:
        Boolean mTriggerAvailable;
        // Indicates if {@code captureSession} can be used to continue capturing more audio
        // from the DSP hardware.
        Boolean mCaptureAvailable;
        // The session to use when attempting to capture more audio from the DSP hardware.
        Int32 mCaptureSession;
        AutoPtr<IAudioFormat> mAudioFormat;
        // Raw data associated with the event.
        // This is the audio that triggered the keyphrase if {@code isTriggerAudio} is true.
        AutoPtr<ArrayOf<Byte> > mData;
    };

    /**
     * Callbacks for always-on hotword detection.
     */
    class Callback
        : public Object
        , public IAlwaysOnHotwordDetectorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback();

        /**
         * Called when the hotword availability changes.
         * This indicates a change in the availability of recognition for the given keyphrase.
         * It's called at least once with the initial availability.<p/>
         *
         * Availability implies whether the hardware on this system is capable of listening for
         * the given keyphrase or not. <p/>
         *
         * @see AlwaysOnHotwordDetector#STATE_HARDWARE_UNAVAILABLE
         * @see AlwaysOnHotwordDetector#STATE_KEYPHRASE_UNSUPPORTED
         * @see AlwaysOnHotwordDetector#STATE_KEYPHRASE_UNENROLLED
         * @see AlwaysOnHotwordDetector#STATE_KEYPHRASE_ENROLLED
         */
        virtual CARAPI OnAvailabilityChanged(
            /* [in] */ Int32 status) = 0;

        /**
         * Called when the keyphrase is spoken.
         * This implicitly stops listening for the keyphrase once it's detected.
         * Clients should start a recognition again once they are done handling this
         * detection.
         *
         * @param eventPayload Payload data for the detection event.
         *        This may contain the trigger audio, if requested when calling
         *        {@link AlwaysOnHotwordDetector#startRecognition(int)}.
         */
        virtual CARAPI OnDetected(
            /*@NonNull*/ /* [in] */ IAlwaysOnHotwordDetectorEventPayload* eventPayload) = 0;

        /**
         * Called when the detection fails due to an error.
         */
        virtual CARAPI OnError() = 0;

        /**
         * Called when the recognition is paused temporarily for some reason.
         * This is an informational callback, and the clients shouldn't be doing anything here
         * except showing an indication on their UI if they have to.
         */
        virtual CARAPI OnRecognitionPaused() = 0;

        /**
         * Called when the recognition is resumed after it was temporarily paused.
         * This is an informational callback, and the clients shouldn't be doing anything here
         * except showing an indication on their UI if they have to.
         */
        virtual CARAPI OnRecognitionResumed() = 0;
    };

    /** @hide */
    class SoundTriggerListener
        : public Object
        , public IIRecognitionStatusCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        SoundTriggerListener();

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        // @Override
        CARAPI OnDetected(
            /* [in] */ ISoundTriggerKeyphraseRecognitionEvent* event);

        // @Override
        CARAPI OnError(
            /* [in] */ Int32 status);

        // @Override
        CARAPI OnRecognitionPaused();

        // @Override
        CARAPI OnRecognitionResumed();

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        AutoPtr<IHandler> mHandler;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AlwaysOnHotwordDetector::MyHandler")

        MyHandler(
            /* [in] */ AlwaysOnHotwordDetector* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AlwaysOnHotwordDetector* mHost;
    };

    class RefreshAvailabiltyTask
        : public AsyncTask
    {
    public:
        RefreshAvailabiltyTask(
            /* [in] */ AlwaysOnHotwordDetector* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        /**
         * @return The initial availability without checking the enrollment status.
         */
        CARAPI InternalGetInitialAvailability(
            /* [out] */ Int32* result);

        /**
         * @return The corresponding {@link KeyphraseSoundModel} or null if none is found.
         */
        CARAPI InternalGetIsEnrolled(
            /* [in] */ Int32 keyphraseId,
            /* [in] */ ILocale* locale,
            /* [out] */ Boolean* result);

    private:
        AlwaysOnHotwordDetector* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AlwaysOnHotwordDetector();

    /**
     * @param text The keyphrase text to get the detector for.
     * @param locale The java locale for the detector.
     * @param callback A non-null Callback for receiving the recognition events.
     * @param voiceInteractionService The current voice interaction service.
     * @param modelManagementService A service that allows management of sound models.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& text,
        /* [in] */ ILocale* locale,
        /* [in] */ IAlwaysOnHotwordDetectorCallback* callback,
        /* [in] */ IKeyphraseEnrollmentInfo* keyphraseEnrollmentInfo,
        /* [in] */ IIVoiceInteractionService* voiceInteractionService,
        /* [in] */ IIVoiceInteractionManagerService* modelManagementService);

    /**
     * Gets the recognition modes supported by the associated keyphrase.
     *
     * @see #RECOGNITION_MODE_USER_IDENTIFICATION
     * @see #RECOGNITION_MODE_VOICE_TRIGGER
     *
     * @throws UnsupportedOperationException if the keyphrase itself isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    /*@RecognitionModes*/
    CARAPI GetSupportedRecognitionModes(
        /* [out] */ Int32* srm);

    /**
     * Starts recognition for the associated keyphrase.
     *
     * @see #RECOGNITION_FLAG_CAPTURE_TRIGGER_AUDIO
     * @see #RECOGNITION_FLAG_ALLOW_MULTIPLE_TRIGGERS
     *
     * @param recognitionFlags The flags to control the recognition properties.
     * @return Indicates whether the call succeeded or not.
     * @throws UnsupportedOperationException if the recognition isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    CARAPI StartRecognition(/*@RecognitionFlags*/
    /* [in] */ Int32 recognitionFlags,
    /* [out] */ Boolean* result);

    /**
     * Stops recognition for the associated keyphrase.
     *
     * @return Indicates whether the call succeeded or not.
     * @throws UnsupportedOperationException if the recognition isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    CARAPI StopRecognition(
        /* [out] */ Boolean* result);

    /**
     * Creates an intent to start the enrollment for the associated keyphrase.
     * This intent must be invoked using {@link Activity#startActivityForResult(Intent, int)}.
     * Starting re-enrollment is only valid if the keyphrase is un-enrolled,
     * i.e. {@link #STATE_KEYPHRASE_UNENROLLED},
     * otherwise {@link #createReEnrollIntent()} should be preferred.
     *
     * @return An {@link Intent} to start enrollment for the given keyphrase.
     * @throws UnsupportedOperationException if managing they keyphrase isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    CARAPI CreateEnrollIntent(
        /* [out] */ IIntent** intent);

    /**
     * Creates an intent to start the un-enrollment for the associated keyphrase.
     * This intent must be invoked using {@link Activity#startActivityForResult(Intent, int)}.
     * Starting re-enrollment is only valid if the keyphrase is already enrolled,
     * i.e. {@link #STATE_KEYPHRASE_ENROLLED}, otherwise invoking this may result in an error.
     *
     * @return An {@link Intent} to start un-enrollment for the given keyphrase.
     * @throws UnsupportedOperationException if managing they keyphrase isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    CARAPI CreateUnEnrollIntent(
        /* [out] */ IIntent** intent);

    /**
     * Creates an intent to start the re-enrollment for the associated keyphrase.
     * This intent must be invoked using {@link Activity#startActivityForResult(Intent, int)}.
     * Starting re-enrollment is only valid if the keyphrase is already enrolled,
     * i.e. {@link #STATE_KEYPHRASE_ENROLLED}, otherwise invoking this may result in an error.
     *
     * @return An {@link Intent} to start re-enrollment for the given keyphrase.
     * @throws UnsupportedOperationException if managing they keyphrase isn't supported.
     *         Callers should only call this method after a supported state callback on
     *         {@link Callback#onAvailabilityChanged(int)} to avoid this exception.
     * @throws IllegalStateException if the detector is in an invalid state.
     *         This may happen if another detector has been instantiated or the
     *         {@link VoiceInteractionService} hosting this detector has been shut down.
     */
    CARAPI CreateReEnrollIntent(
        /* [out] */ IIntent** intent);

protected:
    /**
     * Invalidates this hotword detector so that any future calls to this result
     * in an IllegalStateException.
     *
     * @hide
     */
    CARAPI Invalidate();

    /**
     * Reloads the sound models from the service.
     *
     * @hide
     */
    CARAPI OnSoundModelsChanged();

private:
    CARAPI GetSupportedRecognitionModesLocked(
        /* [out] */ Int32* srml);

    CARAPI GetManageIntentLocked(
        /* [in] */ Int32 action,
        /* [out] */ IIntent** intent);

    CARAPI StartRecognitionLocked(
        /* [in] */ Int32 recognitionFlags,
        /* [out] */ Int32* srl);

    CARAPI StopRecognitionLocked(
        /* [out] */ Int32* srl);

    CARAPI NotifyStateChangedLocked();

private:
    /**
     * Indicates that this hotword detector is no longer valid for any recognition
     * and should not be used anymore.
     */
    const static Int32 STATE_INVALID = -3;

    /**
     * Indicates that the detector isn't ready currently.
     */
    const static Int32 STATE_NOT_READY = 0;

    const static String TAG;
    const static Boolean DBG;

    const static Int32 STATUS_ERROR = ISoundTrigger::STATUS_ERROR;
    const static Int32 STATUS_OK = ISoundTrigger::STATUS_OK;

    const static Int32 MSG_AVAILABILITY_CHANGED = 1;
    const static Int32 MSG_HOTWORD_DETECTED = 2;
    const static Int32 MSG_DETECTION_ERROR = 3;
    const static Int32 MSG_DETECTION_PAUSE = 4;
    const static Int32 MSG_DETECTION_RESUME = 5;

    String mText;
    AutoPtr<ILocale> mLocale;
    /**
     * The metadata of the Keyphrase, derived from the enrollment application.
     * This may be null if this keyphrase isn't supported by the enrollment application.
     */
    AutoPtr<IKeyphraseMetadata> mKeyphraseMetadata;
    AutoPtr<IKeyphraseEnrollmentInfo> mKeyphraseEnrollmentInfo;
    AutoPtr<IIVoiceInteractionService> mVoiceInteractionService;
    AutoPtr<IIVoiceInteractionManagerService> mModelManagementService;
    AutoPtr<SoundTriggerListener> mInternalCallback;
    AutoPtr<Callback> mExternalCallback;
    Object mLock;
    AutoPtr<IHandler> mHandler;

    Int32 mAvailability;
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_ALWAYSONHOTWORDDETECTOR_H__
