#ifndef __ELASTOS_DROID_SPEECH_TTS_TextToSpeechService_H__
#define __ELASTOS_DROID_SPEECH_TTS_TextToSpeechService_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"

#include "elastos/droid/app/Service.h"
#include "elastos/droid/speech/tts/AudioPlaybackHandler.h"
//#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/speech/tts/SynthesisRequest.h"
#include "elastos/droid/speech/tts/AbstractSynthesisCallback.h"
#include "elastos/droid/speech/tts/EventLoggerV1.h"
#include "elastos/droid/speech/tts/AudioPlaybackQueueItem.h"
//#include "elastos/droid/os/RemoteCallbackList.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::Os::IRemoteCallbackList;
//using Elastos::Droid::Os::RemoteCallbackList;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Droid::Speech::Tts::IITextToSpeechService;
using Elastos::Droid::App::Service;
using Elastos::Net::IURI;
//using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Set;
using Elastos::Utility::IList;
using Elastos::IO::IFileOutputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Abstract base class for TTS engine implementations. The following methods
 * need to be implemented:
 * <ul>
 * <li>{@link #onIsLanguageAvailable}</li>
 * <li>{@link #onLoadLanguage}</li>
 * <li>{@link #onGetLanguage}</li>
 * <li>{@link #onSynthesizeText}</li>
 * <li>{@link #onStop}</li>
 * </ul>
 * The first three deal primarily with language management, and are used to
 * query the engine for it's support for a given language and indicate to it
 * that requests in a given language are imminent.
 *
 * {@link #onSynthesizeText} is central to the engine implementation. The
 * implementation should synthesize text as per the request parameters and
 * return synthesized data via the supplied callback. This class and its helpers
 * will then consume that data, which might mean queuing it for playback or writing
 * it to a file or similar. All calls to this method will be on a single thread,
 * which will be different from the main thread of the service. Synthesis must be
 * synchronous which means the engine must NOT hold on to the callback or call any
 * methods on it after the method returns.
 *
 * {@link #onStop} tells the engine that it should stop
 * all ongoing synthesis, if any. Any pending data from the current synthesis
 * will be discarded.
 *
 * {@link #onGetLanguage} is not required as of JELLYBEAN_MR2 (API 18) and later, it is only
 * called on earlier versions of Android.
 *
 * API Level 20 adds support for Voice objects. Voices are an abstraction that allow the TTS
 * service to expose multiple backends for a single locale. Each one of them can have a different
 * features set. In order to fully take advantage of voices, an engine should implement
 * the following methods:
 * <ul>
 * <li>{@link #onGetVoices()}</li>
 * <li>{@link #onIsValidVoiceName(String)}</li>
 * <li>{@link #onLoadVoice(String)}</li>
 * <li>{@link #onGetDefaultVoiceNameFor(String, String, String)}</li>
 * </ul>
 * The first three methods are siblings of the {@link #onGetLanguage},
 * {@link #onIsLanguageAvailable} and {@link #onLoadLanguage} methods. The last one,
 * {@link #onGetDefaultVoiceNameFor(String, String, String)} is a link between locale and voice
 * based methods. Since API level 21 {@link TextToSpeech#setLanguage} is implemented by
 * calling {@link TextToSpeech#setVoice} with the voice returned by
 * {@link #onGetDefaultVoiceNameFor(String, String, String)}.
 *
 * If the client uses a voice instead of a locale, {@link SynthesisRequest} will contain the
 * requested voice name.
 *
 * The default implementations of Voice-related methods implement them using the
 * pre-existing locale-based implementation.
 */
//public abstract class
class TextToSpeechService
    : public Elastos::Droid::App::Service
    , public ITextToSpeechService
{
private:
    /**
     * Synthesizer thread. This thread is used to run {@link SynthHandler}.
     */
    //private class
    class SynthThread
        : public Object
//        , public HandlerThread
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        SynthThread();

        virtual ~SynthThread();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ TextToSpeechService* ttss);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

        CARAPI_(void) OnLooperPrepared();

        CARAPI_(void) BroadcastTtsQueueProcessingCompleted();

    private:
        Boolean mFirstIdle;
        AutoPtr<TextToSpeechService> mTtss;
    };

private:
    /**
     * An item in the synth thread queue.
     */
    //private abstract class
    class SpeechItem
        : public Object
    {
    public:
        CAR_INTERFACE_DECL()

        SpeechItem();

        virtual ~SpeechItem();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IInterface* caller,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ TextToSpeechService* ttss);

        AutoPtr<IInterface> GetCallerIdentity();

        CARAPI GetCallerUid(
            /* [out] */ Int32* ret);

        CARAPI GetCallerPid(
            /* [out] */ Int32* ret);

        /**
         * Checker whether the item is valid. If this method returns false, the item should not
         * be played.
         */
        virtual Boolean IsValid() = 0;

        /**
         * Plays the speech item. Blocks until playback is finished.
         * Must not be called more than once.
         *
         * Only called on the synthesis thread.
         */
        //public
        CARAPI Play();

        virtual void PlayImpl() = 0;

        /**
         * Stops the speech item.
         * Must not be called more than once.
         *
         * Can be called on multiple threads,  but not on the synthesis thread.
         */
        //public
        CARAPI Stop();

        virtual void StopImpl() = 0;

        //protected synchronized
        CARAPI_(Boolean) IsStopped();

    protected:
        //private final
        AutoPtr<IInterface> mCallerIdentity;

        //private final
        Int32 mCallerUid;
        //private final
        Int32 mCallerPid;
        //private
        Boolean mStarted;
        //private
        Boolean mStopped;
        Object mLock;
        AutoPtr<TextToSpeechService> mTtss;
    };

public:
    /**
     * An item in the synth thread queue that process utterance (and call back to client about
     * progress).
     */
    class UtteranceSpeechItem
        : public SpeechItem
        , public IUtteranceProgressDispatcher
    {
    public:
        CAR_INTERFACE_DECL()

        UtteranceSpeechItem();

        UtteranceSpeechItem(
            /* [in] */ IInterface* caller,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid);

        CARAPI DispatchOnSuccess();

        CARAPI DispatchOnStop();

        CARAPI DispatchOnStart();

        CARAPI DispatchOnError(
            /* [in] */ Int32 errorCode);

        String GetUtteranceId();

        String GetStringParam(
            /* [in] */ IBundle* params,
            /* [in] */ const String& key,
            /* [in] */ const String& defaultValue);

        Int32 GetIntParam(
            /* [in] */ IBundle* params,
            /* [in] */ const String& key,
            /* [in] */ Int32 defaultValue);

        Float GetFloatParam(
            /* [in] */ IBundle* params,
            /* [in] */ const String& key,
            /* [in] */ Float defaultValue);
    };

public:
    /**
     * UtteranceSpeechItem for V1 API speech items. V1 API speech items keep
     * synthesis parameters in a single Bundle passed as parameter. This class
     * allow subclasses to access them conveniently.
     */
    class SpeechItemV1
        : public UtteranceSpeechItem
    {
    public:
        SpeechItemV1(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ IBundle* params,
            /* [in] */ const String& utteranceId,
            /* [in] */ TextToSpeechService* ttss);

        Boolean HasLanguage();

        Int32 GetSpeechRate();

        Int32 GetPitch();

        String GetUtteranceId();

        AudioOutputParams* GetAudioParams();

    protected:
        IBundle* mParams;
        String mUtteranceId;
    };

public:

    class SynthesisSpeechItemV1
        : public SpeechItemV1
    {
    public:
        SynthesisSpeechItemV1(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ IBundle* params,
            /* [in] */ const String& utteranceId,
            /* [in] */ TextToSpeechService* ttss);

        ICharSequence* GetText();

        Boolean HasLanguage();

        Boolean IsValid();

        void PlayImpl();

        AbstractSynthesisCallback* CreateSynthesisCallback();

        void SetRequestParams(
            /* [in] */ ISynthesisRequest* request);

        void StopImpl();

        String GetCountry();

        String GetVariant();

        String GetLanguage();

        String GetVoiceName();

        Int32 GetSpeechRate();

        Int32 GetPitch();

    private:
        // Never null.
        AutoPtr<ICharSequence> mText;
        AutoPtr<ISynthesisRequest> mSynthesisRequest;
        AutoPtr<ArrayOf<String> > mDefaultLocale;
        // Non null after synthesis has started, and all accesses
        // guarded by 'this'.
        AutoPtr<AbstractSynthesisCallback> mSynthesisCallback;
        AutoPtr<EventLoggerV1> mEventLogger;
        Int32 mCallerUid;
    };

private:

    class SynthesisToFileOutputStreamSpeechItemV1
        : public SynthesisSpeechItemV1
    {
    public:
        SynthesisToFileOutputStreamSpeechItemV1(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ IBundle* params,
            /* [in] */ const String& utteranceId,
            /* [in] */ IFileOutputStream* fileOutputStream,
            /* [in] */ TextToSpeechService* ttss);

        AbstractSynthesisCallback* createSynthesisCallback();

        void PlayImpl();

    private:
        AutoPtr<IFileOutputStream> mFileOutputStream;
    };

private:
    class AudioSpeechItemV1
        : public SpeechItemV1
    {
    private:
        AutoPtr<AudioPlaybackQueueItem> mItem;

    public:
        AudioSpeechItemV1(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int64 callerUid,
            /* [in] */ Int64 callerPid,
            /* [in] */ IBundle* params,
            /* [in] */ const String& utteranceId,
            /* [in] */ IURI* uri,
            /* [in] */ TextToSpeechService* ttss);

        Boolean IsValid();

        void PlayImpl();

        void StopImpl();

        String GetUtteranceId();

        AudioOutputParams* GetAudioParams();
    };

private:
    class SilenceSpeechItem
        : public UtteranceSpeechItem
    {
    private:
        Int64 mDuration;
        String mUtteranceId;

    public:
        SilenceSpeechItem(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& utteranceId,
            /* [in] */ Int64 duration,
            /* [in] */ TextToSpeechService* ttss);

        Boolean IsValid();

        void PlayImpl();

        void StopImpl();

        String GetUtteranceId();
    };

private:

    /**
     * Call {@link TextToSpeechService#onLoadLanguage} on synth thread.
     */
     class LoadLanguageItem
        : public SpeechItem
    {
    private:
        String mLanguage;
        String mCountry;
        String mVariant;

    public:
        LoadLanguageItem(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& language,
            /* [in] */ const String& country,
            /* [in] */ const String& variant);

        Boolean isValid();

        void playImpl();

        void stopImpl();
    };

private:
    /**
     * Call {@link TextToSpeechService#onLoadLanguage} on synth thread.
     */
    class LoadVoiceItem
        : public SpeechItem
    {
    private:
        String mVoiceName;

    public:
        LoadVoiceItem(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& voiceName);

        Boolean isValid();

        void playImpl();

        void stopImpl();
    };

private:
    //private class
    class SynthHandler
        : public Object
    {
    private:
        class RunnableSynthHandlerEnqueueSpeechItem
            : public Runnable
        {
        public:
            CARAPI Run();
        public:
            RunnableSynthHandlerEnqueueSpeechItem(
                /* [in] */ SynthHandler* sh,
                /* [in] */ SpeechItem* si);
        private:
            AutoPtr<SynthHandler> mSh;
            AutoPtr<SpeechItem> mSi;
        };

    public:
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    public:
        //public
        SynthHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ TextToSpeechService* ttss);

    public:
        //public
        CARAPI_(Boolean) IsSpeaking();

        //public
        CARAPI_(void) Quit();

        /**
         * Adds a speech item to the queue.
         *
         * Called on a service binder thread.
         */
        //public
        CARAPI_(Int32) EnqueueSpeechItem(
            /* [in] */ Int32 queueMode,
            /* [in] */ SpeechItem* speechItem);

        /**
         * Stops all speech output and removes any utterances still in the queue for
         * the calling app.
         *
         * Called on a service binder thread.
         */
        //public
        CARAPI_(Int32) StopForApp(
            /* [in] */ IInterface* callerIdentity);

        //public
        CARAPI_(Int32) StopAll();

    private:
        //private synchronized
        CARAPI_(AutoPtr<SpeechItem>) GetCurrentSpeechItem();

        //private synchronized
        CARAPI_(AutoPtr<SpeechItem>) SetCurrentSpeechItem(
            /* [in] */ SpeechItem* speechItem);

        //private synchronized
        CARAPI_(AutoPtr<SpeechItem>) MaybeRemoveCurrentSpeechItem(
            /* [in] */ IInterface* callerIdentity);

    private:
        //private
        AutoPtr<SpeechItem> mCurrentSpeechItem;// = NULL;
        Object mLock;
        AutoPtr<TextToSpeechService> mTtss;
    };

private:
    class TextToSpeechServiceStub
        : public Object
        , public IITextToSpeechService
    {
    public:
        CAR_INTERFACE_DECL()

        TextToSpeechServiceStub();

        virtual ~TextToSpeechServiceStub();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ TextToSpeechService* ttss);

    public:
        //@Override
        //public int
        CARAPI Speak(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ const String & text,
            /* [in] */ Int32 queueMode,
            /* [in] */ IBundle* params,
            /* [out] */ Int32* result);

        //@Override
        //public int
        CARAPI SynthesizeToFile(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ const String & text,
            /* [in] */ const String & filename,
            /* [in] */ IBundle* params,
            /* [out] */ Int32* result);

        //@Override
        //public int
        CARAPI PlayAudio(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ IUri* audioUri,
            /* [in] */ Int32 queueMode,
            /* [in] */ IBundle* params,
            /* [out] */ Int32* result);

        //@Override
        //public int
        CARAPI PlaySilence(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ Int64 duration,
            /* [in] */ Int32 queueMode,
            /* [in] */ IBundle* params,
            /* [out] */ Int32* result);

        //@Override
        //public boolean
        CARAPI IsSpeaking(
            /* [out] */ Boolean* result);

        //@Override
        //public int
        CARAPI Stop(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [out] */ Int32* result);

        //@Override
        //public String []
        CARAPI GetLanguage(
            /* [out] */ ArrayOf<String>** result);

        /*
         * If defaults are enforced, then no language is "available" except
         * perhaps the default language selected by the user.
         */
        //@Override
        //public int
        CARAPI IsLanguageAvailable(
            /* [in] */ const String & lang,
            /* [in] */ const String & country,
            /* [in] */ const String & variant,
            /* [out] */ Int32* result);

        //@Override
        //public String[]
        CARAPI GetFeaturesForLanguage(
            /* [in] */ const String & lang,
            /* [in] */ const String & country,
            /* [in] */ const String & variant,
            /* [out] */ ArrayOf<String>** result);

        /*
         * There is no point loading a non default language if defaults
         * are enforced.
         */
        //@Override
        //public int
        CARAPI LoadLanguage(
            /* [in] */ const String & lang,
            /* [in] */ const String & country,
            /* [in] */ const String & variant,
            /* [out] */ Int32* result);

        //@Override
        //public void
        CARAPI SetCallback(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ IITextToSpeechCallback* cb);

    public:
        TextToSpeechServiceStub(
            /* [in] */ TextToSpeechService* ttss);

    private:
        //private
        String Intern(
            /* [in] */ const String& in);

        //private
        Boolean CheckNonNull(
            /* [in] */ ArrayOf<IInterface*>* args);
    private:
        AutoPtr<TextToSpeechService> mTtss;
    };

private:
    //private class
    class CallbackMap
        : public Object
//        , public IRemoteCallbackList
//        , public RemoteCallbackList
    {
    public:
        //public void
        void SetCallback(
            /* [in] */ /*IIBinder*/IBinder* caller,
            /* [in] */ IITextToSpeechCallback* cb);

        //public void
        void DispatchOnDone(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ const String& utteranceId);

        //public void
        void DispatchOnStart(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ const String& utteranceId);

        //public void
        void DispatchOnError(
            /* [in] */ IInterface* callerIdentity,
            /* [in] */ const String& utteranceId,
            /* [in] */ Int32 errorCode);

        //@Override
        //public void
        void OnCallbackDied(
            /* [in] */ IITextToSpeechCallback* callback,
            /* [in] */ IInterface* cookie);

        //@Override
        //public
        void Kill();
    public:
        CallbackMap(
            /* [in] */ TextToSpeechService* ttss);
    private:
        //private
        AutoPtr<IITextToSpeechCallback> GetCallbackFor(
            /* [in] */ IInterface* caller);

    private:
        //private final
        HashMap< AutoPtr</*IIBinder*/IBinder>, AutoPtr<IITextToSpeechCallback> > mCallerToCallback;
        Object mCallerToCallbackLock;
        AutoPtr<TextToSpeechService> mTtss;
    };

public:
    CAR_INTERFACE_DECL()

    TextToSpeechService();

    virtual ~TextToSpeechService();

    CARAPI constructor();

public:
    //@Override
    //public
    CARAPI OnCreate();

    //@Override
    //public
    CARAPI OnDestroy();

public:
    //@Override
    //public
    AutoPtr</*IIBinder*/IBinder> OnBind(
        /* [in] */ IIntent* intent);

protected:
    /**
     * Queries the service for a set of supported voices.
     *
     * Can be called on multiple threads.
     *
     * The default implementation tries to enumerate all available locales, pass them to
     * {@link #onIsLanguageAvailable(String, String, String)} and create Voice instances (using
     * the locale's BCP-47 language tag as the voice name) for the ones that are supported.
     * Note, that this implementation is suitable only for engines that don't have multiple voices
     * for a single locale. Also, this implementation won't work with Locales not listed in the
     * set returned by the {@link Locale#getAvailableLocales()} method.
     *
     * @return A list of voices supported.
     */
    CARAPI OnGetVoices(
        /* [out] */ IList** voices);

    /**
     * Return a name of the default voice for a given locale.
     *
     * This method provides a mapping between locales and available voices. This method is
     * used in {@link TextToSpeech#setLanguage}, which calls this method and then calls
     * {@link TextToSpeech#setVoice} with the voice returned by this method.
     *
     * Also, it's used by {@link TextToSpeech#getDefaultVoice()} to find a default voice for
     * the default locale.
     *
     * @param lang ISO-3 language code.
     * @param country ISO-3 country code. May be empty or null.
     * @param variant Language variant. May be empty or null.

     * @return A name of the default voice for a given locale.
     */
    CARAPI OnGetDefaultVoiceNameFor(
        /* [in]  */ String lang,
        /* [in]  */ String country,
        /* [in]  */ String variant,
        /* [out] */ String* name);

    /**
     * Notifies the engine that it should load a speech synthesis voice. There is no guarantee
     * that this method is always called before the voice is used for synthesis. It is merely
     * a hint to the engine that it will probably get some synthesis requests for this voice
     * at some point in the future.
     *
     * Will be called only on synthesis thread.
     *
     * The default implementation creates a Locale from the voice name (by interpreting the name as
     * a BCP-47 tag for the locale), and passes it to
     * {@link #onLoadLanguage(String, String, String)}.
     *
     * @param voiceName Name of the voice.
     * @return {@link TextToSpeech#ERROR} or {@link TextToSpeech#SUCCESS}.
     */
    CARAPI OnLoadVoice(
        /* [in]  */ String voiceName,
        /* [out] */ Int32* voice);

    /**
     * Checks whether the engine supports a voice with a given name.
     *
     * Can be called on multiple threads.
     *
     * The default implementation treats the voice name as a language tag, creating a Locale from
     * the voice name, and passes it to {@link #onIsLanguageAvailable(String, String, String)}.
     *
     * @param voiceName Name of the voice.
     * @return {@link TextToSpeech#ERROR} or {@link TextToSpeech#SUCCESS}.
     */
    CARAPI OnIsValidVoiceName(
        /* [in]  */ String voiceName,
        /* [out] */ Int32* ret);

    /**
     * Checks whether the engine supports a given language.
     *
     * Can be called on multiple threads.
     *
     * Its return values HAVE to be consistent with onLoadLanguage.
     *
     * @param lang ISO-3 language code.
     * @param country ISO-3 country code. May be empty or null.
     * @param variant Language variant. May be empty or null.
     * @return Code indicating the support status for the locale.
     *         One of {@link TextToSpeech#LANG_AVAILABLE},
     *         {@link TextToSpeech#LANG_COUNTRY_AVAILABLE},
     *         {@link TextToSpeech#LANG_COUNTRY_VAR_AVAILABLE},
     *         {@link TextToSpeech#LANG_MISSING_DATA}
     *         {@link TextToSpeech#LANG_NOT_SUPPORTED}.
     */
    //protected abstract
    virtual CARAPI_(Int32) OnIsLanguageAvailable(
        /* [in] */ const String& lang,
        /* [in] */ const String& country,
        /* [in] */ const String& variant) = 0;

    /**
     * Returns the language, country and variant currently being used by the TTS engine.
     *
     * This method will be called only on Android 4.2 and before (API <= 17). In later versions
     * this method is not called by the Android TTS framework.
     *
     * Can be called on multiple threads.
     *
     * @return A 3-element array, containing language (ISO 3-letter code),
     *         country (ISO 3-letter code) and variant used by the engine.
     *         The country and variant may be {@code ""}. If country is empty, then variant must
     *         be empty too.
     * @see Locale#getISO3Language()
     * @see Locale#getISO3Country()
     * @see Locale#getVariant()
     */
    //protected abstract
    virtual CARAPI_(AutoPtr< ArrayOf<String> >) OnGetLanguage() = 0;

    /**
     * Notifies the engine that it should load a speech synthesis language. There is no guarantee
     * that this method is always called before the language is used for synthesis. It is merely
     * a hint to the engine that it will probably get some synthesis requests for this language
     * at some point in the future.
     *
     * Can be called on multiple threads.
     * In <= Android 4.2 (<= API 17) can be called on main and service binder threads.
     * In > Android 4.2 (> API 17) can be called on main and synthesis threads.
     *
     * @param lang ISO-3 language code.
     * @param country ISO-3 country code. May be empty or null.
     * @param variant Language variant. May be empty or null.
     * @return Code indicating the support status for the locale.
     *         One of {@link TextToSpeech#LANG_AVAILABLE},
     *         {@link TextToSpeech#LANG_COUNTRY_AVAILABLE},
     *         {@link TextToSpeech#LANG_COUNTRY_VAR_AVAILABLE},
     *         {@link TextToSpeech#LANG_MISSING_DATA}
     *         {@link TextToSpeech#LANG_NOT_SUPPORTED}.
     */
    //protected abstract
    virtual CARAPI_(Int32) OnLoadLanguage(
        /* [in] */ const String& lang,
        /* [in] */ const String& country,
        /* [in] */ const String& variant) = 0;

    /**
     * Notifies the service that it should stop any in-progress speech synthesis.
     * This method can be called even if no speech synthesis is currently in progress.
     *
     * Can be called on multiple threads, but not on the synthesis thread.
     */
    //protected abstract
    virtual CARAPI_(void) OnStop() = 0;

    /**
     * Tells the service to synthesize speech from the given text. This method
     * should block until the synthesis is finished. Used for requests from V1
     * clients ({@link android.speech.tts.TextToSpeech}). Called on the synthesis
     * thread.
     *
     * @param request The synthesis request.
     * @param callback The callback that the engine must use to make data
     *            available for playback or for writing to a file.
     */
    //protected abstract
    virtual CARAPI_(void) OnSynthesizeText(
        /* [in] */ ISynthesisRequest* request,
        /* [in] */ ISynthesisCallback* callback) = 0;

    /**
     * Queries the service for a set of features supported for a given language.
     *
     * Can be called on multiple threads.
     *
     * @param lang ISO-3 language code.
     * @param country ISO-3 country code. May be empty or null.
     * @param variant Language variant. May be empty or null.
     * @return A list of features supported for the given language.
     */
    //protected
    CARAPI_(AutoPtr< Set<String> >) OnGetFeaturesForLanguage(
        /* [in] */ const String& lang,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);

private:
    //private
    CARAPI_(Int32) GetExpectedLanguageAvailableStatus(
        /* [in] */ ILocale* locale);

    //private
    CARAPI_(Int32) GetDefaultSpeechRate();

    //private
    CARAPI_(AutoPtr< ArrayOf<String> >) GetSettingsLocale();

    //private
    CARAPI_(Int32) GetSecureSettingInt(
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue);

private:
    //private
    static const Boolean DBG;           // = FALSE;
    //private
    static const String TAG;            // = "TextToSpeechService";

    //private
    static const Int32 MAX_SPEECH_ITEM_CHAR_LENGTH;     // = 4000;
    //private
    static const String SYNTH_THREAD_NAME;              // = "SynthThread";

    //private
    AutoPtr<SynthHandler> mSynthHandler;
    // A thread and it's associated handler for playing back any audio
    // associated with this TTS engine. Will handle all requests except synthesis
    // to file requests, which occur on the synthesis thread.
    //private
    AutoPtr<AudioPlaybackHandler> mAudioPlaybackHandler;
    //private
    AutoPtr<ITtsEngines> mEngineHelper;

    //private
    AutoPtr<CallbackMap> mCallbacks;
    //private
    String mPackageName;

    /**
     * Binder returned from {@code #onBind(Intent)}. The methods in this class can be
     * called called from several different threads.
     */
    // NOTE: All calls that are passed in a calling app are interned so that
    // they can be used as message objects (which are tested for equality using ==).
    //private final
    AutoPtr<IITextToSpeechService> mBinder;             // = (IITextToSpeechService*)(new TextToSpeechServiceStub());

};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_TextToSpeechService_H__
