#ifndef __ELASTOS_DROID_SPEECH_TTS_FileSynthesisCallback_H__
#define __ELASTOS_DROID_SPEECH_TTS_FileSynthesisCallback_H__

#include "elastos/droid/speech/tts/AbstractSynthesisCallback.h"
#include <Elastos.CoreLibrary.h>
//
using Elastos::IO::IFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Speech synthesis request that writes the audio to a WAV file.
 */
class FileSynthesisCallback
    : public AbstractSynthesisCallback      // abstract class
{
public:
    CAR_INTERFACE_DECL();

    FileSynthesisCallback();

    virtual ~FileSynthesisCallback();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFileChannel* fileChannel,
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ Boolean clientIsUsingV2);

    CARAPI Stop();

    CARAPI GetMaxBufferSize(
        /* [out] */ Int32* ret);

    CARAPI Start(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount,
        /* [out] */ Int32* ret);

    CARAPI AudioAvailable(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* ret);

    CARAPI Done(
        /* [out] */ Int32* ret);

    CARAPI Error();

    /**
     * The service should call this method if the speech synthesis fails.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * @param errorCode Error code to pass to the client. One of the ERROR_ values from
     *      {@link TextToSpeech}
     */
    CARAPI Error(
        /* [in] */ Int32 errorCode);

    /**
     * Check if {@link #start} was called or not.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * Useful for checking if a fallback from network request is possible.
     */
    CARAPI HasStarted(
        /* [out] */ Boolean* started);

    /**
     * Check if {@link #done} was called or not.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * Useful for checking if a fallback from network request is possible.
     */
    CARAPI HasFinished(
        /* [out] */ Boolean* finished);

private:
    /**
     * Must be called while holding the monitor on {@link #mStateLock}.
     */
    CARAPI CleanUp();

    /**
     * Must be called while holding the monitor on {@link #mStateLock}.
     */
    CARAPI_(void) CloseFile();

    /**
     * Checks whether a given file exists, and deletes it if it does.
     */
    CARAPI_(Boolean) MaybeCleanupExistingFile(
        /* [in] */ IFile* file);

    CARAPI_(AutoPtr<IByteBuffer>) MakeWavHeader(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount,
        /* [in] */ Int32 dataLength);

private:
    static const String TAG;            // = "FileSynthesisRequest";
    static const Boolean DBG;           // = false;

    static const Int32 MAX_AUDIO_BUFFER_SIZE;   // = 8192;

    static const Int32 WAV_HEADER_LENGTH;       // = 44;
    static const Int16 WAV_FORMAT_PCM;          // = 0x0001;

    AutoPtr<IUtteranceProgressDispatcher> mDispatcher;
    AutoPtr<IInterface> mCallerIdentity;
    AutoPtr<IInterface> mStateLock;

    AutoPtr<IFile> mFileName;
    AutoPtr<IFileChannel> mFileChannel;
    Int32 mSampleRateInHz;
    Int32 mAudioFormat;
    Int32 mChannelCount;
    AutoPtr<IRandomAccessFile> mFile;
    Boolean mStarted;
    Boolean mDone;

protected:
    /** Status code of synthesis */
    Int32 mStatusCode;

};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_FileSynthesisCallback_H__
