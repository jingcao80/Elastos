#ifndef __ELASTOS_DROID_SPEECH_SREC_Recognizer_H__
#define __ELASTOS_DROID_SPEECH_SREC_Recognizer_H__

#include <elastos/io/InputStream.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Speech.h"

using Elastos::Utility::ILocale;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

/**
 * Simple, synchronous speech recognizer, using the Nuance SREC package.
 * Usages proceeds as follows:
 *
 * <ul>
 * <li>Create a <code>Recognizer</code>.
 * <li>Create a <code>Recognizer.Grammar</code>.
 * <li>Setup the <code>Recognizer.Grammar</code>.
 * <li>Reset the <code>Recognizer.Grammar</code> slots, if needed.
 * <li>Fill the <code>Recognizer.Grammar</code> slots, if needed.
 * <li>Compile the <code>Recognizer.Grammar</code>, if needed.
 * <li>Save the filled <code>Recognizer.Grammar</code>, if needed.
 * <li>Start the <code>Recognizer</code>.
 * <li>Loop over <code>advance</code> and <code>putAudio</code> until recognition complete.
 * <li>Fetch and process results, or notify of failure.
 * <li>Stop the <code>Recognizer</code>.
 * <li>Destroy the <code>Recognizer</code>.
 * </ul>
 *
 * <p>Below is example code</p>
 *
 * <pre class="prettyprint">
 *
 * // create and start audio input
 * InputStream audio = new MicrophoneInputStream(11025, 11025*5);
 * // create a Recognizer
 * String cdir = Recognizer.getConfigDir(null);
 * Recognizer recognizer = new Recognizer(cdir + "/baseline11k.par");
 * // create and load a Grammar
 * Recognizer.Grammar grammar = recognizer.new Grammar(cdir + "/grammars/VoiceDialer.g2g");
 * // setup the Grammar to work with the Recognizer
 * grammar.setupRecognizer();
 * // fill the Grammar slots with names and save, if required
 * grammar.resetAllSlots();
 * for (String name : names) grammar.addWordToSlot("@Names", name, null, 1, "V=1");
 * grammar.compile();
 * grammar.save(".../foo.g2g");
 * // start the Recognizer
 * recognizer.start();
 * // loop over Recognizer events
 * while (true) {
 *     switch (recognizer.advance()) {
 *     case Recognizer.EVENT_INCOMPLETE:
 *     case Recognizer.EVENT_STARTED:
 *     case Recognizer.EVENT_START_OF_VOICING:
 *     case Recognizer.EVENT_END_OF_VOICING:
 *         // let the Recognizer continue to run
 *         continue;
 *     case Recognizer.EVENT_RECOGNITION_RESULT:
 *         // success, so fetch results here!
 *         for (int i = 0; i < recognizer.getResultCount(); i++) {
 *             String result = recognizer.getResult(i, Recognizer.KEY_LITERAL);
 *         }
 *         break;
 *     case Recognizer.EVENT_NEED_MORE_AUDIO:
 *         // put more audio in the Recognizer
 *         recognizer.putAudio(audio);
 *         continue;
 *     default:
 *         notifyFailure();
 *         break;
 *     }
 *     break;
 * }
 * // stop the Recognizer
 * recognizer.stop();
 * // destroy the Recognizer
 * recognizer.destroy();
 * // stop the audio device
 * audio.close();
 *
 * </pre>
 */
class Recognizer
    : public Object
    , public IRecognizer
{
public:
    /**
     * Represents a grammar loaded into the Recognizer.
     */
    class RecognizerGrammar
        : public Object
        , public IRecognizerGrammar
    {
    public:
        friend class Recognizer;

        CAR_INTERFACE_DECL()

        RecognizerGrammar();

        virtual ~RecognizerGrammar();

        CARAPI constructor();

        /**
         * Create a <code>Grammar</code> instance.
         * @param g2gFileName pathname of g2g file.
         */
        CARAPI constructor(
            /* [in] */ const String& g2gFileName,
            /* [in] */ IRecognizer* r);

        /**
         * Reset all slots.
         */
        CARAPI ResetAllSlots();

        /**
         * Add a word to a slot.
         *
         * @param slot slot name.
         * @param word word to insert.
         * @param pron pronunciation, or null to derive from word.
         * @param weight weight to give the word.  One is normal, 50 is low.
         * @param tag semantic meaning tag string.
         */
        CARAPI AddWordToSlot(
            /* [in] */ const String& slot,
            /* [in] */ const String& word,
            /* [in] */ const String& pron,
            /* [in] */ Int32 weight,
            /* [in] */ const String& tag);

        /**
         * Compile all slots.
         */
        CARAPI Compile();

        /**
         * Setup <code>Grammar</code> with <code>Recognizer</code>.
         */
        CARAPI SetupRecognizer();

        /**
         * Save <code>Grammar</code> to g2g file.
         *
         * @param g2gFileName
         * @throws IOException
         */
        CARAPI Save(
            /* [in] */ const String& g2gFileName);

        /**
         * Release resources associated with this <code>Grammar</code>.
         */
        CARAPI Destroy();

    protected:
        /**
         * Clean up resources.
         */
        CARAPI_(void) Finalize();

    private:
        Int32 mGrammar;
        Recognizer* mR;
    };
    friend class RecognizerGrammar;

public:
    CAR_INTERFACE_DECL()

    Recognizer();

    virtual ~Recognizer();

    CARAPI constructor();

    /**
     * Create an instance of a SREC speech recognizer.
     *
     * @param configFile pathname of the baseline*.par configuration file,
     * which in turn contains references to dictionaries, speech models,
     * and other data needed to configure and operate the recognizer.
     * A separate config file is needed for each audio sample rate.
     * Two files, baseline11k.par and baseline8k.par, which correspond to
     * 11025 and 8000 hz, are present in the directory indicated by
     * {@link #getConfigDir}.
     * @throws IOException
     */
    CARAPI constructor(
        /* [in] */ const String& configFile);

    /**
     * Get the pathname of the SREC configuration directory corresponding to the
     * language indicated by the Locale.
     * This directory contains dictionaries, speech models,
     * configuration files, and other data needed by the Recognizer.
     * @param locale <code>Locale</code> corresponding to the desired language,
     * or null for default, currently <code>Locale.US</code>.
     * @return Pathname of the configuration directory.
     */
    static CARAPI GetConfigDir(
        /* [in] */ ILocale* locale,
        /* [out] */ String* ret);


    /**
     * Start recognition
     */
    CARAPI Start();

    /**
     * Process some audio and return the current status.
     * @return recognition event, one of:
     * <ul>
     * <li><code>EVENT_INVALID</code>
     * <li><code>EVENT_NO_MATCH</code>
     * <li><code>EVENT_INCOMPLETE</code>
     * <li><code>EVENT_STARTED</code>
     * <li><code>EVENT_STOPPED</code>
     * <li><code>EVENT_START_OF_VOICING</code>
     * <li><code>EVENT_END_OF_VOICING</code>
     * <li><code>EVENT_SPOKE_TOO_SOON</code>
     * <li><code>EVENT_RECOGNITION_RESULT</code>
     * <li><code>EVENT_START_OF_UTTERANCE_TIMEOUT</code>
     * <li><code>EVENT_RECOGNITION_TIMEOUT</code>
     * <li><code>EVENT_NEED_MORE_AUDIO</code>
     * <li><code>EVENT_MAX_SPEECH</code>
     * </ul>
     */
    CARAPI Advance(
        /* [out] */ Int32* ret);

    /**
     * Put audio samples into the <code>Recognizer</code>.
     * @param buf holds the audio samples.
     * @param offset offset of the first sample.
     * @param length number of bytes containing samples.
     * @param isLast indicates no more audio data, normally false.
     * @return number of bytes accepted.
     */
    CARAPI PutAudio(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean isLast,
        /* [out] */ Int32* ret);

    /**
     * Read audio samples from an <code>InputStream</code> and put them in the
     * <code>Recognizer</code>.
     * @param audio <code>InputStream</code> containing PCM audio samples.
     */
    CARAPI PutAudio(
        /* [in] */ IInputStream* audio);

    /**
     * Get the number of recognition results.  Must be called after
     * <code>EVENT_RECOGNITION_RESULT</code> is returned by
     * <code>advance</code>, but before <code>stop</code>.
     *
     * @return number of results in nbest list.
     */
    CARAPI GetResultCount(
        /* [out] */ Int32* ret);

    /**
     * Get a set of keys for the result.  Must be called after
     * <code>EVENT_RECOGNITION_RESULT</code> is returned by
     * <code>advance</code>, but before <code>stop</code>.
     *
     * @param index index of result.
     * @return array of keys.
     */
    CARAPI GetResultKeys(
        /* [in] */ Int32 index,
        /* [out] */ ArrayOf<String>** ret);

    /**
     * Get a result value.  Must be called after
     * <code>EVENT_RECOGNITION_RESULT</code> is returned by
     * <code>advance</code>, but before <code>stop</code>.
     *
     * @param index index of the result.
     * @param key key of the result.  This is typically one of
     * <code>KEY_CONFIDENCE</code>, <code>KEY_LITERAL</code>, or
     * <code>KEY_MEANING</code>, but the user can also define their own keys
     * in a grxml file, or in the <code>tag</code> slot of
     * <code>Grammar.addWordToSlot</code>.
     * @return the result.
     */
    CARAPI GetResult(
        /* [in] */ Int32 index,
        /* [in] */ const String& key,
        /* [out] */ String* ret);

    /**
     * Stop the <code>Recognizer</code>.
     */
    CARAPI Stop();

    /**
     * Reset the acoustic state vectorto it's default value.
     *
     * @hide
     */
    CARAPI ResetAcousticState();

    /**
     * Set the acoustic state vector.
     * @param state String containing the acoustic state vector.
     *
     * @hide
     */
    CARAPI SetAcousticState(
        /* [in] */ const String& state);

    /**
     * Get the acoustic state vector.
     * @return String containing the acoustic state vector.
     *
     * @hide
     */
    CARAPI GetAcousticState(
        /* [out] */ String* ret);

    /**
     * Clean up resources.
     */
    CARAPI Destroy();

    CARAPI GetVocabulary(
        /* [out] */ Int32* ret);

public:
    /**
     * Produce a displayable string from an <code>advance</code> event.
     * @param event
     * @return String representing the event.
     */
    static CARAPI EventToString(
        /* [in] */ Int32 event,
        /* [out] */ String* ret);

protected:
    /**
     * Clean up resources.
     */
    CARAPI_(void) Finalize();// throws Throwable

    /* an example session captured, for reference
    void doall() {
        if (PMemInit ( )
           || lhs_audioinOpen ( WAVE_MAPPER, SREC_TEST_DEFAULT_AUDIO_FREQUENCY, &audio_in_handle )
           || srec_test_init_application_data ( &applicationData, argc, argv )
           || SR_SessionCreate ( "/system/usr/srec/config/en.us/baseline11k.par" )
           || SR_RecognizerCreate ( &applicationData.recognizer )
           || SR_RecognizerSetup ( applicationData.recognizer)
           || ESR_SessionGetLCHAR ( L("cmdline.vocabulary"), filename, &flen )
           || SR_VocabularyLoad ( filename, &applicationData.vocabulary )
           || SR_VocabularyGetLanguage ( applicationData.vocabulary, &applicationData.locale )
           || (applicationData.nametag = NULL)
           || SR_NametagsCreate ( &applicationData.nametags )
           || (LSTRCPY ( applicationData.grammars [0].grammar_path, "/system/usr/srec/config/en.us/grammars/VoiceDialer.g2g" ), 0)
           || (LSTRCPY ( applicationData.grammars [0].grammarID, "BothTags" ), 0)
           || (LSTRCPY ( applicationData.grammars [0].ruleName, "trash" ), 0)
           || (applicationData.grammars [0].is_ve_grammar = ESR_FALSE, 0)
           || SR_GrammarLoad (applicationData.grammars [0].grammar_path, &applicationData.grammars [applicationData.grammarCount].grammar )
           || SR_GrammarSetupVocabulary ( applicationData.grammars [0].grammar, applicationData.vocabulary )
           || SR_GrammarSetupRecognizer( applicationData.grammars [0].grammar, applicationData.recognizer )
           || SR_GrammarSetDispatchFunction ( applicationData.grammars [0].grammar, L("myDSMCallback"), NULL, myDSMCallback )
           || (applicationData.grammarCount++, 0)
           || SR_RecognizerActivateRule ( applicationData.recognizer, applicationData.grammars [0].grammar,
                           applicationData.grammars [0].ruleName, 1 )
           || (applicationData.active_grammar_num = 0, 0)
           || lhs_audioinStart ( audio_in_handle )
           || SR_RecognizerStart ( applicationData.recognizer )
           || strl ( applicationData.grammars [0].grammar, &applicationData, audio_in_handle, &recognition_count )
           || SR_RecognizerStop ( applicationData.recognizer )
           || lhs_audioinStop ( audio_in_handle )
           || SR_RecognizerDeactivateRule ( applicationData.recognizer, applicationData.grammars [0].grammar, applicationData.grammars [0].ruleName )
           || (applicationData.active_grammar_num = -1, 0)
           || SR_GrammarDestroy ( applicationData.grammars [0].grammar )
           || (applicationData.grammarCount--, 0)
           || SR_NametagsDestroy ( applicationData.nametags )
           || (applicationData.nametags = NULL, 0)
           || SR_VocabularyDestroy ( applicationData.vocabulary )
           || (applicationData.vocabulary = NULL)
           || SR_RecognizerUnsetup ( applicationData.recognizer) // releases acoustic models
           || SR_RecognizerDestroy ( applicationData.recognizer )
           || (applicationData.recognizer = NULL)
           || SR_SessionDestroy ( )
           || srec_test_shutdown_application_data ( &applicationData )
           || lhs_audioinClose ( &audio_in_handle )
           || PMemShutdown ( )
    }
    */

private:
    //
    // PMem native methods
    //
    //natice
    static CARAPI_(void) PMemInit();
    //natice
    static CARAPI_(void) PMemShutdown();

    //
    // SR_Session native methods
    //
    //native
    static CARAPI_(void) SR_SessionCreate(
        /* [in] */ const String& filename);
    //native
    static CARAPI_(void) SR_SessionDestroy();

    //
    // SR_Recognizer native methods
    //
private:
    //
    // SR_Recognizer methods
    //
    //native
    static CARAPI_(void) SR_RecognizerStart(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_RecognizerStop(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Int64) SR_RecognizerCreate();
    //native
    static CARAPI_(void) SR_RecognizerDestroy(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_RecognizerSetup(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_RecognizerUnsetup(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSetup(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(String) SR_RecognizerGetParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key);
    //native
    static CARAPI_(Int32) SR_RecognizerGetSize_tParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key);
    //native
    static CARAPI_(Boolean) SR_RecognizerGetBoolParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key);
    //native
    static CARAPI_(void) SR_RecognizerSetParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key,
        /* [in] */ const String& value);
    //native
    static CARAPI_(void) SR_RecognizerSetSize_tParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key,
        /* [in] */ Int32 value);
    //native
    static CARAPI_(void) SR_RecognizerSetBoolParameter(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& key,
        /* [in] */ Boolean value);
    //native
    static CARAPI_(void) SR_RecognizerSetupRule(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 grammar,
        /* [in] */ const String& ruleName);
    //native
    static CARAPI_(Boolean) SR_RecognizerHasSetupRules(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_RecognizerActivateRule(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 grammar,
        /* [in] */ const String& ruleName,
        /* [in] */ Int32 weight);
    //native
    static CARAPI_(void) SR_RecognizerDeactivateRule(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 grammar,
        /* [in] */ const String& ruleName);
    //native
    static CARAPI_(void) SR_RecognizerDeactivateAllRules(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsActiveRule(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 grammar,
        /* [in] */ const String& ruleName);
    //native
    static CARAPI_(Boolean) SR_RecognizerCheckGrammarConsistency(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 grammar);
    //native
    static CARAPI_(Int32) SR_RecognizerPutAudio(
        /* [in] */ Int64 recognizer,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean isLast);
    //native
    static CARAPI_(Int32) SR_RecognizerAdvance(
        /* [in] */ Int64 recognizer);
    // private static native void SR_RecognizerLoadUtterance(int recognizer,
    //         const LCHAR* filename);
    // private static native void SR_RecognizerLoadWaveFile(int recognizer,
    //         const LCHAR* filename);
    // private static native void SR_RecognizerSetLockFunction(int recognizer,
    //         SR_RecognizerLockFunction function, void* data);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalClipping(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalDCOffset(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalNoisy(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalTooQuiet(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalTooFewSamples(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Boolean) SR_RecognizerIsSignalTooManySamples(
        /* [in] */ Int64 recognizer);
    // private static native void SR_Recognizer_Change_Sample_Rate (size_t new_sample_rate);

    //
    // SR_AcousticState native methods
    //
    //native
    static CARAPI_(void) SR_AcousticStateReset(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_AcousticStateSet(
        /* [in] */ Int64 recognizer,
        /* [in] */ const String& state);
    //native
    static CARAPI_(String) SR_AcousticStateGet(
        /* [in] */ Int64 recognizer);

    //
    // SR_Grammar native methods
    //
    //native
    static CARAPI_(void) SR_GrammarCompile(
        /* [in] */ Int32 grammar);
    //native
    static CARAPI_(void) SR_GrammarAddWordToSlot(
        /* [in] */ Int32 grammar,
        /* [in] */ const String& slot,
        /* [in] */ const String& word,
        /* [in] */ const String& pronunciation,
        /* [in] */ Int32 weight,
        /* [in] */ const String& tag);
    //native
    static CARAPI_(void) SR_GrammarResetAllSlots(
        /* [in] */ Int32 grammar);
    // private static native void SR_GrammarAddNametagToSlot(int grammar, String slot,
    // const struct SR_Nametag_t* nametag, int weight, String tag);
    //native
    static CARAPI_(void) SR_GrammarSetupVocabulary(
        /* [in] */ Int32 grammar,
        /* [in] */ Int32 vocabulary);
    // private static native void SR_GrammarSetupModels(int grammar, SR_AcousticModels* models);
    //native
    static CARAPI_(void) SR_GrammarSetupRecognizer(
        /* [in] */ Int32 grammar,
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(void) SR_GrammarUnsetupRecognizer(
        /* [in] */ Int32 grammar);
    // private static native void SR_GrammarGetModels(int grammar,SR_AcousticModels** models);
    //native
    static CARAPI_(Int32) SR_GrammarCreate();
    //native
    static CARAPI_(void) SR_GrammarDestroy(
        /* [in] */ Int32 grammar);
    //native
    static CARAPI_(Int32) SR_GrammarLoad(
        /* [in] */ const String& filename);
    //native
    static CARAPI_(void) SR_GrammarSave(
        /* [in] */ Int32 grammar,
        /* [in] */ const String& filename);
    // private static native void SR_GrammarSetDispatchFunction(int grammar,
    //         const LCHAR* name, void* userData, SR_GrammarDispatchFunction function);
    // private static native void SR_GrammarSetParameter(int grammar, const
    //         LCHAR* key, void* value);
    // private static native void SR_GrammarSetSize_tParameter(int grammar,
    //         const LCHAR* key, size_t value);
    // private static native void SR_GrammarGetParameter(int grammar, const
    //         LCHAR* key, void** value);
    // private static native void SR_GrammarGetSize_tParameter(int grammar,
    //         const LCHAR* key, size_t* value);
    // private static native void SR_GrammarCheckParse(int grammar, const LCHAR*
    //         transcription, SR_SemanticResult** result, size_t* resultCount);
    //native
    static CARAPI_(void) SR_GrammarAllowOnly(
        /* [in] */ Int32 grammar,
        /* [in] */ const String& transcription);
    //native
    static CARAPI_(void) SR_GrammarAllowAll(
        /* [in] */ Int32 grammar);

    //
    // SR_Vocabulary native methods
    //
    // private static native int SR_VocabularyCreate();
    //native
    static CARAPI_(Int32) SR_VocabularyLoad();
    // private static native void SR_VocabularySave(SR_Vocabulary* self,
    //         const LCHAR* filename);
    // private static native void SR_VocabularyAddWord(SR_Vocabulary* self,
    //         const LCHAR* word);
    // private static native void SR_VocabularyGetLanguage(SR_Vocabulary* self,
    //         ESR_Locale* locale);
    //native
    static CARAPI_(void) SR_VocabularyDestroy(
        /* [in] */ Int32 vocabulary);
    //native
    static CARAPI_(String) SR_VocabularyGetPronunciation(
        /* [in] */ Int32 vocabulary,
        /* [in] */ const String& word);

    //
    // SR_RecognizerResult native methods
    //
    //native
    static CARAPI_(AutoPtr< ArrayOf<Byte> >) SR_RecognizerResultGetWaveform(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Int32) SR_RecognizerResultGetSize(
        /* [in] */ Int64 recognizer);
    //native
    static CARAPI_(Int32) SR_RecognizerResultGetKeyCount(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 nbest);
    //native
    static CARAPI_(AutoPtr< ArrayOf<String> >) SR_RecognizerResultGetKeyList(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 nbest);
    //native
    static CARAPI_(String) SR_RecognizerResultGetValue(
        /* [in] */ Int64 recognizer,
        /* [in] */ Int32 nbest,
        /* [in] */ const String& key);
    // private static native void SR_RecognizerResultGetLocale(int recognizer, ESR_Locale* locale);

private:
    static String TAG;          // = "Recognizer";

    // handle to SR_Vocabulary object
    Int32 mVocabulary;

    // handle to SR_Recognizer object
    Int32 mRecognizer;

    // Grammar currently associated with Recognizer via SR_GrammarSetupRecognizer
    AutoPtr<RecognizerGrammar> mActiveGrammar;

    // audio buffer for putAudio(InputStream)
    AutoPtr<ArrayOf<Byte> > mPutAudioBuffer;

};

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_Recognizer_H__
