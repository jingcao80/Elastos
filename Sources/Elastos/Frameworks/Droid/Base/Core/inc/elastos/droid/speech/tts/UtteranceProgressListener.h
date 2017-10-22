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

#ifndef __ELASTOS_DROID_SPEECH_TTS_UtteranceProgressListener_H__
#define __ELASTOS_DROID_SPEECH_TTS_UtteranceProgressListener_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Speech.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Listener for events relating to the progress of an utterance through
 * the synthesis queue. Each utterance is associated with a call to
 * {@link TextToSpeech#speak} or {@link TextToSpeech#synthesizeToFile} with an
 * associated utterance identifier, as per {@link TextToSpeech.Engine#KEY_PARAM_UTTERANCE_ID}.
 *
 * The callbacks specified in this method can be called from multiple threads.
 */
//public abstract class
class UtteranceProgressListener
    : public Object
    , public IUtteranceProgressListener
{
private:
    class UtteranceProgressListenerStand
        : public Object
        , public IUtteranceProgressListener
    {
    public:
        CAR_INTERFACE_DECL()

        UtteranceProgressListenerStand();

        virtual ~UtteranceProgressListenerStand();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener);
    public:
        //@Override
        //public synchronized void
        CARAPI OnDone(
            /* [in] */ const String& utteranceId);

        /**
         * Called when an error has occurred during processing. This can be called
         * at any point in the synthesis process. Note that there might be calls
         * to {@link #onStart(String)} for specified utteranceId but there will never
         * be a call to both {@link #onDone(String)} and {@link #onError(String)} for
         * the same utterance.
         *
         * @param utteranceId the utterance ID of the utterance.
         * @deprecated Use {@link #onError(String,int)} instead
         */
        //@Override
        //public void
        CARAPI OnError(
            /* [in] */ const String& utteranceId);
        /**
         * Called when an error has occurred during processing. This can be called
         * at any point in the synthesis process. Note that there might be calls
         * to {@link #onStart(String)} for specified utteranceId but there will never
         * be a call to both {@link #onDone(String)} and {@link #onError(String,int)} for
         * the same utterance. The default implementation calls {@link #onError(String)}.
         *
         * @param utteranceId the utterance ID of the utterance.
         * @param errorCode one of the ERROR_* codes from {@link TextToSpeech}
         */
        CARAPI OnError(
            /* [in] */ const String& utteranceId,
            /* [in] */ Int32 errorCode);

        //@Override
        //public void
        CARAPI OnStart(
            /* [in] */ const String& utteranceId);

    private:
        AutoPtr<ITextToSpeechOnUtteranceCompletedListener> mListener;
        Object mMutex;
    };

public:
    CAR_INTERFACE_DECL()

    UtteranceProgressListener();

    ~UtteranceProgressListener();

    CARAPI constructor();

    /**
     * Called when an utterance "starts" as perceived by the caller. This will
     * be soon before audio is played back in the case of a {@link TextToSpeech#speak}
     * or before the first bytes of a file are written to storage in the case
     * of {@link TextToSpeech#synthesizeToFile}.
     *
     * @param utteranceId the utterance ID of the utterance.
     */
    //public abstract
    virtual CARAPI OnStart(
        /* [in] */ const String& utteranceId) = 0;

    /**
     * Called when an utterance has successfully completed processing.
     * All audio will have been played back by this point for audible output, and all
     * output will have been written to disk for file synthesis requests.
     *
     * This request is guaranteed to be called after {@link #onStart(String)}.
     *
     * @param utteranceId the utterance ID of the utterance.
     */
    //public abstract
    virtual CARAPI OnDone(
        /* [in] */ const String& utteranceId) = 0;

    /**
     * Called when an error has occurred during processing. This can be called
     * at any point in the synthesis process. Note that there might be calls
     * to {@link #onStart(String)} for specified utteranceId but there will never
     * be a call to both {@link #onDone(String)} and {@link #onError(String)} for
     * the same utterance.
     *
     * @param utteranceId the utterance ID of the utterance.
     */
    //public abstract
    virtual CARAPI OnError(
        /* [in] */ const String& utteranceId) = 0;

    /**
     * Wraps an old deprecated OnUtteranceCompletedListener with a shiny new
     * progress listener.
     *
     * @hide
     */
    static AutoPtr<IUtteranceProgressListener> From(
        /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener);
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_UtteranceProgressListener_H__
