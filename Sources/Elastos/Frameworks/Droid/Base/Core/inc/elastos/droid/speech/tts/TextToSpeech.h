//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SPEECH_TTS_TextToSpeech_H__
#define __ELASTOS_DROID_SPEECH_TTS_TextToSpeech_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"

#include "elastos/droid/speech/tts/UtteranceProgressListener.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/List.h>
#include "Elastos.Droid.Speech.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Media.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Etl::Set;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Media::IAudioAttributes;
//using Elastos::Droid::Speech::Tts::CTtsEngines;
using Elastos::Utility::IList;
using Elastos::IO::IFile;
using Elastos::Utility::ISet;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 *
 * Synthesizes speech from text for immediate playback or to create a sound file.
 * <p>A TextToSpeech instance can only be used to synthesize text once it has completed its
 * initialization. Implement the {@link TextToSpeech.OnInitListener} to be
 * notified of the completion of the initialization.<br>
 * When you are done using the TextToSpeech instance, call the {@link #shutdown()} method
 * to release the native resources used by the TextToSpeech engine.
 *
 */
class TextToSpeech
    : public Object
    , public ITextToSpeech
{
public:
    /**
     * Information about an installed text-to-speech engine.
     *
     * @see TextToSpeech#getEngines
     */
    //static
    class TextToSpeechEngineInfo
        : public Object
        , public ITextToSpeechEngineInfo
    {
    public:
        CAR_INTERFACE_DECL()

        TextToSpeechEngineInfo();

        CARAPI ToString(
            /* [out] */ String* ret);
    public:
        /**
         * Engine package name..
         */
        String name;
        /**
         * Localized label for the engine.
         */
        String label;
        /**
         * Icon for the engine.
         */
        Int32 icon;
        /**
         * Whether this engine is a part of the system
         * image.
         *
         * @hide
         */
        Boolean system;
        /**
         * The priority the engine declares for the the intent filter
         * {@code android.intent.action.TTS_SERVICE}
         *
         * @hide
         */
        Int32 priority;
    };

private:
    /**
     * Java:
     * private interface Action<R> {
     *    R run(ITextToSpeechService service) throws RemoteException;
     * }
     */
    class TextToSpeechActionR
        : public Object
    {
    public:
        virtual CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service) = 0;
    };

    class TextToSpeechActionRShutdown
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRShutdown(
            /* [in] */ TextToSpeech* tts);
    private:
        TextToSpeech* mTts;
    };

    class TextToSpeechActionRSpeak
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRSpeak(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ const String& text,
            /* [in] */ Int32 queueMode,
            /* [in] */ IHashMap* params);
    private:
        TextToSpeech* mTts;
        String mText;
        Int32 mQueueMode;
        AutoPtr<IHashMap> mParams;
    };

    class TextToSpeechActionRPlayEarcon
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRPlayEarcon(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ const String& earcon,
            /* [in] */ Int32 queueMode,
            /* [in] */ IHashMap* params);
    private:
        TextToSpeech* mTts;
        String mEarcon;
        Int32 mQueueMode;
        AutoPtr<IHashMap> mParams;
    };

    class TextToSpeechActionRPlaySilence
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRPlaySilence(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ Int64 durationInMs,
            /* [in] */ Int32 queueMode,
            /* [in] */ IHashMap* params);
    private:
        TextToSpeech* mTts;
        Int64 mDurationInMs;
        Int32 mQueueMode;
        AutoPtr<IHashMap> mParams;
    };

    class TextToSpeechActionRGetFeatures
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRGetFeatures(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ ILocale* locale);
    private:
        TextToSpeech* mTts;
        AutoPtr<ILocale> mLocale;
    };

    class TextToSpeechActionRIsSpeaking
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRIsSpeaking(
            /* [in] */ TextToSpeech* tts);
    private:
        TextToSpeech* mTts;
    };

    class TextToSpeechActionRStop
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRStop(
            /* [in] */ TextToSpeech* tts);
    private:
        TextToSpeech* mTts;
    };

    class TextToSpeechActionRSetLanguage
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRSetLanguage(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ ILocale* locale);
    private:
        TextToSpeech* mTts;
        AutoPtr<ILocale> mLocale;
    };

    class TextToSpeechActionRGetLanguage
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRGetLanguage(
            /* [in] */ TextToSpeech* tts);
    private:
        TextToSpeech* mTts;
    };

    class TextToSpeechActionRIsLanguageAvailable
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRIsLanguageAvailable(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ ILocale* locale);
    private:
        TextToSpeech* mTts;
        AutoPtr<ILocale> mLocale;
    };

    class TextToSpeechActionRSynthesizeToFile
        : public TextToSpeechActionR
    {
    public:
        CARAPI_(Handle32) Run(
            /* [in] */ IITextToSpeechService* service);
    public:
        TextToSpeechActionRSynthesizeToFile(
            /* [in] */ TextToSpeech* tts,
            /* [in] */ const String& text,
            /* [in] */ IHashMap* params,
            /* [in] */ const String& filename);
    private:
        TextToSpeech* mTts;
        String mText;
        AutoPtr<IHashMap> mParams;
        String mFilename;
    };

private:
    class TextToSpeechConnection
        : public Object
        , public IServiceConnection
    {
    private:
        class TextToSpeechConnectionCallback
            : public Object
            , public IITextToSpeechCallback
        {
        public:
            CAR_INTERFACE_DECL()

            TextToSpeechConnectionCallback();

            virtual ~TextToSpeechConnectionCallback();

            CARAPI constructor();

            CARAPI constructor(
                /* [in] */ TextToSpeech* tts);
        public:
            //@Override
            CARAPI OnStop(
                /* [in] */ const String& utteranceId);

            //@Override
            CARAPI OnSuccess(
                /* [in] */ const String& utteranceId);

            //@Override
            CARAPI OnError(
                /* [in] */ const String& utteranceId,
                /* [in] */ Int32 errorCode);

            //@Override
            CARAPI OnStart(
                /* [in] */ const String& utteranceId);

        private:
            TextToSpeech* mTts;

        };

    public:
        CAR_INTERFACE_DECL()

        TextToSpeechConnection();

        virtual ~TextToSpeechConnection();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ TextToSpeech* tts);

    public:
        //@Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        //public
        CARAPI_(AutoPtr</*IIBinder*/IInterface>) GetCallerIdentity();

        /**
         * Clear connection related fields and cancel mOnServiceConnectedAsyncTask if set.
         *
         * @return true if we cancel mOnSetupConnectionAsyncTask in progress.
         */
        CARAPI_(Boolean) ClearServiceConnection();

        //@Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        CARAPI_(void) Disconnect();

        CARAPI_(Boolean) IsEstablished();

        //<R>
        CARAPI_(Handle32) RunAction(
            /* [in] */ TextToSpeechActionR* action,
            /* [in] */ Handle32 errorResult,
            /* [in] */ const String& method,
            /* [in] */ Boolean reconnect,
            /* [in] */ Boolean onlyEstablishedConnection);
    private:
        TextToSpeech* mTts;
        AutoPtr<IITextToSpeechService> mService;
        //private final ITextToSpeechCallback.Stub mCallback = new ITextToSpeechCallback.Stub() {...}
        AutoPtr<IITextToSpeechCallback> mCallback;      // = new TextToSpeechConnectionCallback(mTts);

        //SetupConnectionAsyncTask mOnSetupConnectionAsyncTask;

        Boolean mEstablished;
        Boolean onlyEstablishedConnection;
    };

    friend class TextToSpeechConnection;

public:
    CAR_INTERFACE_DECL()

    TextToSpeech();

    virtual ~TextToSpeech();

    CARAPI constructor();

    /**
     * The constructor for the TextToSpeech class, using the default TTS engine.
     * This will also initialize the associated TextToSpeech engine if it isn't already running.
     *
     * @param context
     *            The context this instance is running in.
     * @param listener
     *            The {@link TextToSpeech.OnInitListener} that will be called when the
     *            TextToSpeech engine has initialized.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITextToSpeechOnInitListener* listener);

    /**
     * The constructor for the TextToSpeech class, using the given TTS engine.
     * This will also initialize the associated TextToSpeech engine if it isn't already running.
     *
     * @param context
     *            The context this instance is running in.
     * @param listener
     *            The {@link TextToSpeech.OnInitListener} that will be called when the
     *            TextToSpeech engine has initialized.
     * @param engine Package name of the TTS engine to use.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITextToSpeechOnInitListener* listener,
        /* [in] */ const String& engine);

    /**
     * Used by the framework to instantiate TextToSpeech objects with a supplied
     * package name, instead of using {@link android.content.Context#getPackageName()}
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITextToSpeechOnInitListener* listener,
        /* [in] */ const String& engine,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean useFallback);
public:
    /**
     * Releases the resources used by the TextToSpeech engine.
     * It is good practice for instance to call this method in the onDestroy() method of an Activity
     * so the TextToSpeech engine can be cleanly stopped.
     */
    CARAPI Shutdown();

    /**
     * Adds a mapping between a string of text and a sound resource in a
     * package. After a call to this method, subsequent calls to
     * {@link #speak(String, int, HashMap)} will play the specified sound resource
     * if it is available, or synthesize the text it is missing.
     *
     * @param text
     *            The string of text. Example: <code>"south_south_east"</code>
     *
     * @param packagename
     *            Pass the packagename of the application that contains the
     *            resource. If the resource is in your own application (this is
     *            the most common case), then put the packagename of your
     *            application here.<br/>
     *            Example: <b>"com.google.marvin.compass"</b><br/>
     *            The packagename can be found in the AndroidManifest.xml of
     *            your application.
     *            <p>
     *            <code>&lt;manifest xmlns:android=&quot;...&quot;
     *      package=&quot;<b>com.google.marvin.compass</b>&quot;&gt;</code>
     *            </p>
     *
     * @param resourceId
     *            Example: <code>R.raw.south_south_east</code>
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddSpeech(
        /* [in] */ const String& text,
        /* [in] */ const String& packagename,
        /* [in] */ Int32 resourceId,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a CharSequence (may be spanned with TtsSpans) of text
     * and a sound resource in a package. After a call to this method, subsequent calls to
     * {@link #speak(String, int, HashMap)} will play the specified sound resource
     * if it is available, or synthesize the text it is missing.
     *
     * @param text
     *            The string of text. Example: <code>"south_south_east"</code>
     *
     * @param packagename
     *            Pass the packagename of the application that contains the
     *            resource. If the resource is in your own application (this is
     *            the most common case), then put the packagename of your
     *            application here.<br/>
     *            Example: <b>"com.google.marvin.compass"</b><br/>
     *            The packagename can be found in the AndroidManifest.xml of
     *            your application.
     *            <p>
     *            <code>&lt;manifest xmlns:android=&quot;...&quot;
     *      package=&quot;<b>com.google.marvin.compass</b>&quot;&gt;</code>
     *            </p>
     *
     * @param resourceId
     *            Example: <code>R.raw.south_south_east</code>
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddSpeech(
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& packagename,
        /* [in] */ Int32 resourceId,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a string of text and a sound file. Using this, it
     * is possible to add custom pronounciations for a string of text.
     * After a call to this method, subsequent calls to {@link #speak(String, int, HashMap)}
     * will play the specified sound resource if it is available, or synthesize the text it is
     * missing.
     *
     * @param text
     *            The string of text. Example: <code>"south_south_east"</code>
     * @param filename
     *            The full path to the sound file (for example:
     *            "/sdcard/mysounds/hello.wav")
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddSpeech(
        /* [in] */ const String& text,
        /* [in] */ const String& filename,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a CharSequence (may be spanned with TtsSpans and a sound file.
     * Using this, it is possible to add custom pronounciations for a string of text.
     * After a call to this method, subsequent calls to {@link #speak(String, int, HashMap)}
     * will play the specified sound resource if it is available, or synthesize the text it is
     * missing.
     *
     * @param text
     *            The string of text. Example: <code>"south_south_east"</code>
     * @param file
     *            File object pointing to the sound file.
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddSpeech(
        /* [in] */ ICharSequence* text,
        /* [in] */ IFile* file,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a string of text and a sound resource in a
     * package. Use this to add custom earcons.
     *
     * @see #playEarcon(String, int, HashMap)
     *
     * @param earcon The name of the earcon.
     *            Example: <code>"[tick]"</code><br/>
     *
     * @param packagename
     *            the package name of the application that contains the
     *            resource. This can for instance be the package name of your own application.
     *            Example: <b>"com.google.marvin.compass"</b><br/>
     *            The package name can be found in the AndroidManifest.xml of
     *            the application containing the resource.
     *            <p>
     *            <code>&lt;manifest xmlns:android=&quot;...&quot;
     *      package=&quot;<b>com.google.marvin.compass</b>&quot;&gt;</code>
     *            </p>
     *
     * @param resourceId
     *            Example: <code>R.raw.tick_snd</code>
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddEarcon(
        /* [in] */ const String& earcon,
        /* [in] */ const String& packagename,
        /* [in] */ Int32 resourceId,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a string of text and a sound file.
     * Use this to add custom earcons.
     *
     * @see #playEarcon(String, int, HashMap)
     *
     * @param earcon
     *            The name of the earcon.
     *            Example: <code>"[tick]"</code>
     * @param filename
     *            The full path to the sound file (for example:
     *            "/sdcard/mysounds/tick.wav")
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddEarcon(
        /* [in] */ const String& earcon,
        /* [in] */ const String& filename,
        /* [out] */ Int32 *ret);

    /**
     * Adds a mapping between a string of text and a sound file.
     * Use this to add custom earcons.
     *
     * @see #playEarcon(String, int, HashMap)
     *
     * @param earcon
     *            The name of the earcon.
     *            Example: <code>"[tick]"</code>
     * @param file
     *            File object pointing to the sound file.
     *
     * @return Code indicating success or failure. See {@link #ERROR} and {@link #SUCCESS}.
     */
    CARAPI AddEarcon(
        /* [in] */ const String& earcon,
        /* [in] */ IFile* file,
        /* [out] */ Int32* ret);


    /**
     * Speaks the text using the specified queuing strategy and speech parameters, the text may
     * be spanned with TtsSpans.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}) and using the
     * {@link Engine#KEY_PARAM_UTTERANCE_ID} parameter.
     *
     * @param text The string of text to be spoken. No longer than
     *            {@link #getMaxSpeechInputLength()} characters.
     * @param queueMode The queuing strategy to use, {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_STREAM},
     *            {@link Engine#KEY_PARAM_VOLUME},
     *            {@link Engine#KEY_PARAM_PAN}.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     * @param utteranceId An unique identifier for this request.
     *
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the speak operation.
     */
    CARAPI Speak(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 queueMode,
        /* [in] */ IBundle* params,
        /* [in] */ const String& utteranceId,
        /* [in] */ Int32* ret);

    /**
     * Speaks the string using the specified queuing strategy and speech
     * parameters.
     *
     * @param text The string of text to be spoken.
     * @param queueMode The queuing strategy to use, {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_STREAM},
     *            {@link Engine#KEY_PARAM_UTTERANCE_ID},
     *            {@link Engine#KEY_PARAM_VOLUME},
     *            {@link Engine#KEY_PARAM_PAN}.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI Speak(
        /* [in] */ const String& text,
        /* [in] */ Int32 queueMode,
        /* [in] */ IHashMap* params,
        /* [in] */ Int32* ret);

    /**
     * Plays the earcon using the specified queueing mode and parameters.
     * The earcon must already have been added with {@link #addEarcon(String, String)} or
     * {@link #addEarcon(String, String, int)}.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}) and using the
     * {@link Engine#KEY_PARAM_UTTERANCE_ID} parameter.
     *
     * @param earcon The earcon that should be played
     * @param queueMode {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_STREAM},
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     *
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the playEarcon operation.
     */
    CARAPI PlayEarcon(
        /* [in] */ const String& earcon,
        /* [in] */ Int32 queueMode,
        /* [in] */ IBundle* params,
        /* [in] */ const String& utteranceld,
        /* [out] */ Int32* ret);

    /**
     * Plays the earcon using the specified queueing mode and parameters.
     * The earcon must already have been added with {@link #addEarcon(String, String)} or
     * {@link #addEarcon(String, String, int)}.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}) and using the
     * {@link Engine#KEY_PARAM_UTTERANCE_ID} parameter.
     *
     * @param earcon The earcon that should be played
     * @param queueMode {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_STREAM},
     *            {@link Engine#KEY_PARAM_UTTERANCE_ID}.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     *
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the playEarcon operation.
     * @deprecated As of API level 21, replaced by
     *         {@link #playEarcon(String, int, Bundle, String)}.
     */
    CARAPI PlayEarcon(
        /* [in] */ const String& earcon,
        /* [in] */ Int32 queueMode,
        /* [in] */ IHashMap* params,
        /* [out] */ Int32* ret);

    /**
     * Plays silence for the specified amount of time using the specified
     * queue mode.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}) and using the
     * {@link Engine#KEY_PARAM_UTTERANCE_ID} parameter.
     *
     * @param durationInMs The duration of the silence.
     * @param queueMode {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param utteranceId An unique identifier for this request.
     *
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the playSilentUtterance operation.
     */
    CARAPI PlaySilentUtterance(
        /* [in] */ Int64 durationInMs,
        /* [in] */ Int32 queueMode,
        /* [in] */ const String& utteranceId,
        /* [out] */ Int32* ret);

    /**
     * Plays silence for the specified amount of time using the specified
     * queue mode.
     *
     * @param durationInMs The duration of the silence.
     * @param queueMode {@link #QUEUE_ADD} or {@link #QUEUE_FLUSH}.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_UTTERANCE_ID}.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI PlaySilence(
        /* [in] */ Int64 durationInMs,
        /* [in] */ Int32 queueMode,
        /* [in] */ IHashMap* params,
        /* [out] */ Int32* ret);

    /**
     * Queries the engine for the set of features it supports for a given locale.
     * Features can either be framework defined, e.g.
     * {@link TextToSpeech.Engine#KEY_FEATURE_NETWORK_SYNTHESIS} or engine specific.
     * Engine specific keys must be prefixed by the name of the engine they
     * are intended for. These keys can be used as parameters to
     * {@link TextToSpeech#speak(String, int, java.util.HashMap)} and
     * {@link TextToSpeech#synthesizeToFile(String, java.util.HashMap, String)}.
     *
     * Features are boolean flags, and their values in the synthesis parameters
     * must be behave as per {@link Boolean#parseBoolean(String)}.
     *
     * @param locale The locale to query features for.
     */
    CARAPI GetFeatures(
        /* [in] */ ILocale* locale,
        /* [out] */ ISet** ret);

    /**
     * Checks whether the TTS engine is busy speaking. Note that a speech item is
     * considered complete once it's audio data has been sent to the audio mixer, or
     * written to a file. There might be a finite lag between this point, and when
     * the audio hardware completes playback.
     *
     * @return {@code true} if the TTS engine is speaking.
     */
    CARAPI IsSpeaking(
        /* [out] */ Boolean* ret);

    /**
     * Interrupts the current utterance (whether played or rendered to file) and discards other
     * utterances in the queue.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI Stop(
        /* [out] */ Int32* ret);

    /**
     * Sets the speech rate.
     *
     * This has no effect on any pre-recorded speech.
     *
     * @param speechRate Speech rate. {@code 1.0} is the normal speech rate,
     *            lower values slow down the speech ({@code 0.5} is half the normal speech rate),
     *            greater values accelerate it ({@code 2.0} is twice the normal speech rate).
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI SetSpeechRate(
        /* [in] */ Float speechRate,
        /* [out] */ Int32* ret);

    /**
     * Sets the speech pitch for the TextToSpeech engine.
     *
     * This has no effect on any pre-recorded speech.
     *
     * @param pitch Speech pitch. {@code 1.0} is the normal pitch,
     *            lower values lower the tone of the synthesized voice,
     *            greater values increase it.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI SetPitch(
        /* [in] */ Float pitch,
        /* [out] */ Int32* ret);

    /**
     * Sets the audio attributes to be used when speaking text or playing
     * back a file.
     *
     * @param audioAttributes Valid AudioAttributes instance.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    CARAPI SetAudioAttributes(
        /* [in] */ IAudioAttributes* audioAttributes,
        /* [out] */ Int32* ret);

    /**
     * @return the engine currently in use by this TextToSpeech instance.
     * @hide
     */
    CARAPI GetCurrentEngine(
        /* [out] */ String* ret);

    /**
     * Returns a Locale instance describing the language currently being used as the default
     * Text-to-speech language.
     *
     * The locale object returned by this method is NOT a valid one. It has identical form to the
     * one in {@link #getLanguage()}. Please refer to {@link #getLanguage()} for more information.
     *
     * @return language, country (if any) and variant (if any) used by the client stored in a
     *     Locale instance, or {@code null} on error.
     * @deprecated As of API level 21, use <code>getDefaultVoice().getLocale()</code> ({@link
     *   #getDefaultVoice()})
     */
    // @Deprecated
    CARAPI GetDefaultLanguage(
        /* [out] */ ILocale** language);

    /**
     * Sets the text-to-speech language.
     * The TTS engine will try to use the closest match to the specified
     * language as represented by the Locale, but there is no guarantee that the exact same Locale
     * will be used. Use {@link #isLanguageAvailable(Locale)} to check the level of support
     * before choosing the language to use for the next utterances.
     *
     * @param loc The locale describing the language to be used.
     *
     * @return Code indicating the support status for the locale. See {@link #LANG_AVAILABLE},
     *         {@link #LANG_COUNTRY_AVAILABLE}, {@link #LANG_COUNTRY_VAR_AVAILABLE},
     *         {@link #LANG_MISSING_DATA} and {@link #LANG_NOT_SUPPORTED}.
     */
    CARAPI_(Int32) SetLanguage(
        /* [in] */ ILocale* loc,
        /* [out] */ Int32* ret);

    /**
     * Returns a Locale instance describing the language currently being used by the TextToSpeech
     * engine.
     *
     * @return language, country (if any) and variant (if any) used by the engine stored in a Locale
     *     instance, or {@code null} on error.
     */
    CARAPI GetLanguage(
        /* [out] */ ILocale** language);

    /**
     * Query the engine about the set of available languages.
     */
    CARAPI GetAvailableLanguages(
        /* [out] */ ISet** languages);

    /**
     * Query the engine about the set of available voices.
     *
     * Each TTS Engine can expose multiple voices for each locale, each with a different set of
     * features.
     *
     * @see #setVoice(Voice)
     * @see Voice
     */
    CARAPI GetVoices(
        /* [out] */ ISet** voices);

    /**
     * Sets the text-to-speech voice.
     *
     * @param voice One of objects returned by {@link #getVoices()}.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     *
     * @see #getVoices
     * @see Voice
     */
    CARAPI SetVoice(
        /* [in] */ IVoice* voice,
        /* [out] */ Int32* ret);

    /**
     * Returns a Voice instance describing the voice currently being used for synthesis
     * requests sent to the TextToSpeech engine.
     *
     * @return Voice instance used by the client, or {@code null} if not set or on error.
     *
     * @see #getVoices
     * @see #setVoice
     * @see Voice
     */
    CARAPI GetVoice(
        /* [out] */ IVoice** voice);

    /**
     * Returns a Voice instance that's the default voice for the default Text-to-speech language.
     * @return The default voice instance for the default language, or {@code null} if not set or
     *     on error.
     */
    CARAPI GetDefaultVoice(
        /* [out] */ IVoice** voice);

    /**
     * Checks if the specified language as represented by the Locale is available and supported.
     *
     * @param loc The Locale describing the language to be used.
     *
     * @return Code indicating the support status for the locale. See {@link #LANG_AVAILABLE},
     *         {@link #LANG_COUNTRY_AVAILABLE}, {@link #LANG_COUNTRY_VAR_AVAILABLE},
     *         {@link #LANG_MISSING_DATA} and {@link #LANG_NOT_SUPPORTED}.
     */
    CARAPI_(Int32) IsLanguageAvailable(
        /* [in] */ ILocale* loc,
        /* [out] */ Int32* ret);

    /**
     * Synthesizes the given text to a file using the specified parameters.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}).
     *
     * @param text The text that should be synthesized. No longer than
     *            {@link #getMaxSpeechInputLength()} characters.
     * @param params Parameters for the request. Can be null.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     * @param file File to write the generated audio data to.
     * @param utteranceId An unique identifier for this request.
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the synthesizeToFile operation.
     */
    CARAPI SynthesizeToFile(
        /* [in] */ ICharSequence* text,
        /* [in] */ IBundle* params,
        /* [in] */ IFile* filename,
        /* [in] */ const String& utteranceld,
        /* [out] */ Int32* ret);

    /**
     * Synthesizes the given text to a file using the specified parameters.
     * This method is asynchronous, i.e. the method just adds the request to the queue of TTS
     * requests and then returns. The synthesis might not have finished (or even started!) at the
     * time when this method returns. In order to reliably detect errors during synthesis,
     * we recommend setting an utterance progress listener (see
     * {@link #setOnUtteranceProgressListener}) and using the
     * {@link Engine#KEY_PARAM_UTTERANCE_ID} parameter.
     *
     * @param text The text that should be synthesized. No longer than
     *            {@link #getMaxSpeechInputLength()} characters.
     * @param params Parameters for the request. Can be null.
     *            Supported parameter names:
     *            {@link Engine#KEY_PARAM_UTTERANCE_ID}.
     *            Engine specific parameters may be passed in but the parameter keys
     *            must be prefixed by the name of the engine they are intended for. For example
     *            the keys "com.svox.pico_foo" and "com.svox.pico:bar" will be passed to the
     *            engine named "com.svox.pico" if it is being used.
     * @param filename Absolute file filename to write the generated audio data to.It should be
     *            something like "/sdcard/myappsounds/mysound.wav".
     *
     * @return {@link #ERROR} or {@link #SUCCESS} of <b>queuing</b> the synthesizeToFile operation.
     * @deprecated As of API level 21, replaced by
     *         {@link #synthesizeToFile(CharSequence, Bundle, File, String)}.
     * @Deprecated
     */
    CARAPI SynthesizeToFile(
        /* [in] */ const String& text,
        /* [in] */ IHashMap* params,
        /* [in] */ const String& filename,
        /* [out] */ Int32* ret);

    /**
     * Sets the listener that will be notified when synthesis of an utterance completes.
     *
     * @param listener The listener to use.
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     *
     * @deprecated Use {@link #setOnUtteranceProgressListener(UtteranceProgressListener)}
     *        instead.
     */
    //@Deprecated
    CARAPI SetOnUtteranceCompletedListener(
        /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener,
        /* [out] */ Int32* ret);

    /**
     * Sets the listener that will be notified of various events related to the
     * synthesis of a given utterance.
     *
     * See {@link UtteranceProgressListener} and
     * {@link TextToSpeech.Engine#KEY_PARAM_UTTERANCE_ID}.
     *
     * @param listener the listener to use.
     * @return {@link #ERROR} or {@link #SUCCESS}
     */
    CARAPI SetOnUtteranceProgressListener(
        /* [in] */ IUtteranceProgressListener* listener,
        /* [out] */ Int32* ret);

    /**
     * Sets the TTS engine to use.
     *
     * @deprecated This doesn't inform callers when the TTS engine has been
     *        initialized. {@link #TextToSpeech(Context, OnInitListener, String)}
     *        can be used with the appropriate engine name. Also, there is no
     *        guarantee that the engine specified will be loaded. If it isn't
     *        installed or disabled, the user / system wide defaults will apply.
     *
     * @param enginePackageName The package name for the synthesis engine (e.g. "com.svox.pico")
     *
     * @return {@link #ERROR} or {@link #SUCCESS}.
     */
    //@Deprecated
    CARAPI SetEngineByPackageName(
        /* [in] */ const String& enginePackageName,
        /* [out] */ Int32* ret);

    /**
     * Gets the package name of the default speech synthesis engine.
     *
     * @return Package name of the TTS engine that the user has chosen
     *        as their default.
     */
    CARAPI GetDefaultEngine(
        /* [out] */ String* engine);

    /**
     * Checks whether the user's settings should override settings requested
     * by the calling application. As of the Ice cream sandwich release,
     * user settings never forcibly override the app's settings.
     */
    CARAPI AreDefaultsEnforced(
        /* [out] */ Boolean* enforced);

    /**
     * Gets a list of all installed TTS engines.
     *
     * @return A list of engine info objects. The list can be empty, but never {@code null}.
     */
    CARAPI GetEngines(
        /* [out] */ IList** ret);

    /**
     * Limit of length of input string passed to speak and synthesizeToFile.
     *
     * @see #speak
     * @see #synthesizeToFile
     */
    static CARAPI_(Int32) GetMaxSpeechInputLength();

private:
    //private <R>
    CARAPI_(Handle32) RunActionNoReconnect(
        /* [in] */ TextToSpeechActionR* action,
        /* [in] */ Handle32 errorResult,
        /* [in] */ const String& method,
        /* [in] */ Boolean onlyEstablishedConnection);

    //private <R>
    CARAPI_(Handle32) RunAction(
        /* [in] */ TextToSpeechActionR* action,
        /* [in] */ Handle32 errorResult,
        /* [in] */ const String& method);

    //private <R>
    CARAPI_(Handle32) RunAction(
        /* [in] */ TextToSpeechActionR* action,
        /* [in] */ Handle32 errorResult,
        /* [in] */ const String& method,
        /* [in] */ Boolean reconnect,
        /* [in] */ Boolean onlyEstablishedConnection);

    CARAPI_(Int32) InitTts();

    CARAPI_(Boolean) ConnectToEngine(
        /* [in] */ const String& engine);

    CARAPI_(void) DispatchOnInit(
        /* [in] */ Int32 result);

    CARAPI_(AutoPtr</*IIBinder*/IInterface>) GetCallerIdentity();

private:
    CARAPI_(AutoPtr<IUri>) MakeResourceUri(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resourceId);

private:
    CARAPI_(AutoPtr<IBundle>) ConvertParamsHashMaptoBundle(
        /* [in] */ IMap* params);

    CARAPI_(void) CopyStringParam(
        /* [in] */ IBundle* bundle,
        /* [in] */ IMap* params,
        /* [in] */ const String& key);

    CARAPI_(void) CopyIntParam(
        /* [in] */ IBundle* bundle,
        /* [in] */ IMap* params,
        /* [in] */ const String& key);

    CARAPI_(void) CopyFloatParam(
        /* [in] */ IBundle* bundle,
        /* [in] */ IMap* params,
        /* [in] */ const String& key);
public:
    static const Int32 QUEUE_DESTROY;       // = 2;
private:
    static const String TAG;                // = "TextToSpeech";

private:
    AutoPtr<IContext> mContext;
    AutoPtr<TextToSpeechConnection> mServiceConnection;
    AutoPtr<ITextToSpeechOnInitListener> mInitListener;
    // Written from an unspecified application thread, read from
    // a binder thread.
    /*volatile*/ AutoPtr<IUtteranceProgressListener> mUtteranceProgressListener;
    //private final Object mStartLock = new Object();
    Object mStartLock;

    String mRequestedEngine;
    // Whether to initialize this TTS object with the default engine,
    // if the requested engine is not available. Valid only if mRequestedEngine
    // is not null. Used only for testing, though potentially useful API wise
    // too.
    Boolean mUseFallback;
    Map<String, AutoPtr<IUri> > mEarcons;
    Map<String, AutoPtr<IUri> > mUtterances;
    AutoPtr<IBundle> mParams;
    AutoPtr<ITtsEngines> mEnginesHelper;
    String mPackageName;
    /*volatile*/ String mCurrentEngine;

};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_TextToSpeech_H__
