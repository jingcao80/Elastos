#include "elastos/droid/speech/tts/TextToSpeech.h"
#include "elastos/droid/speech/tts/FileSynthesisCallback.h"
#include "elastos/droid/os/FileUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include "Elastos.Droid.Speech.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"

using Elastos::Core::StringUtils;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::FileUtils;
using Elastos::IO::IBuffer;
using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

const String FileSynthesisCallback::TAG("FileSynthesisRequest");
const Boolean FileSynthesisCallback::DBG = FALSE;

const Int32 FileSynthesisCallback::MAX_AUDIO_BUFFER_SIZE = 8192;

const Int32 FileSynthesisCallback::WAV_HEADER_LENGTH = 44;
const Int16 FileSynthesisCallback::WAV_FORMAT_PCM = 0x0001;

CAR_INTERFACE_IMPL(FileSynthesisCallback, Object, ISynthesisCallback);

FileSynthesisCallback::FileSynthesisCallback()
{}

FileSynthesisCallback::~FileSynthesisCallback()
{}

ECode FileSynthesisCallback::constructor()
{
    return NOERROR;
}

ECode FileSynthesisCallback::constructor(
    /* [in] */ IFileChannel* fileChannel,
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Boolean clientIsUsingV2)
{
    mFileChannel = fileChannel;
    mSampleRateInHz = 0;
    mAudioFormat = 0;
    mChannelCount = 0;
    mStarted = FALSE;
    mDone = FALSE;

    return NOERROR;
}

ECode FileSynthesisCallback::Stop()
{
    AutoLock lock(mStateLock);

    if (mDone) {
        return NOERROR;
    }
    if (mStatusCode == TextToSpeech::STOPPED) {
        return NOERROR;
    }

    mStatusCode = TextToSpeech::STOPPED;
    CleanUp();
    if (mDispatcher != NULL) {
        mDispatcher->DispatchOnStop();
    }

    return NOERROR;
}

ECode FileSynthesisCallback::CleanUp()
{
    CloseFile();

    return NOERROR;
}

void FileSynthesisCallback::CloseFile()
{
    // File will be closed by the SpeechItem in the speech service.
    mFileChannel = NULL;
}

Boolean FileSynthesisCallback::MaybeCleanupExistingFile(
    /* [in] */ IFile* file)
{
    Boolean bExists;
    if ((file->Exists(&bExists), bExists)) {
        //Java:    Log.v(TAG, "File " + file + " exists, deleting.");
        String strPath;
        Logger::V(TAG, String("File ") + (file->GetAbsolutePath(&strPath), strPath) + String(" exists, deleting.\n"));

        Boolean bDelete;
        if (!(file->Delete(&bDelete), bDelete)) {
            //Java:    Log.e(TAG, "Failed to delete " + file);
            Logger::E(TAG, String("Failed to delete ") + strPath + String("\n"));
            return FALSE;
        }
    }
    return TRUE;
}

ECode FileSynthesisCallback::GetMaxBufferSize(
    /* [out] */ Int32* ret)
{
    *ret = MAX_AUDIO_BUFFER_SIZE;
    return NOERROR;
}

ECode FileSynthesisCallback::Start(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 channelCount,
    /* [out] */ Int32* ret)
{
    if (DBG) {
        //Java:    Log.d(TAG, "FileSynthesisRequest.start(" + sampleRateInHz + "," + audioFormat + "," + channelCount + ")");
        Logger::D(TAG, "FileSynthesisRequest.start(%d, %d, %d)", sampleRateInHz, audioFormat, channelCount);
    }

    AutoPtr<IFileChannel> fileChannel;

    synchronized (this) {
        if (mStatusCode == ITextToSpeech::STOPPED) {
            if (DBG) {
                Logger::D(TAG, "Request has been aborted.");
            }
            ErrorCodeOnStop(ret);
            return NOERROR;
        }
        if (mStatusCode != ITextToSpeech::TTS_SUCCESS) {
            if (DBG) {
                Logger::D(TAG, "Error was raised");
            }
            *ret = TextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mStarted) {
            Logger::E(TAG, "Start called twice");
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        mStarted = TRUE;
        mSampleRateInHz = sampleRateInHz;
        mAudioFormat = audioFormat;
        mChannelCount = channelCount;

        if (mDispatcher != NULL) {
            mDispatcher->DispatchOnStart();
        }
        fileChannel = mFileChannel;
    }

    Int32 i;
    AutoPtr<IByteBufferHelper> bbHelper;
    AutoPtr<IByteBuffer> mBytes;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    bbHelper->Allocate(WAV_HEADER_LENGTH, (IByteBuffer**)&mBytes);
    ECode ec = fileChannel->Write(mBytes, &i);

    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to write wav header to output file descriptor");
        synchronized (this) {
            CleanUp();
            mStatusCode = ITextToSpeech::ERROR_OUTPUT;
        }
        *ret = TextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    Logger::E(TAG, "Failed to write wav header to output file descriptor");
    synchronized (this) {
        CleanUp();
        mStatusCode = ITextToSpeech::ERROR_OUTPUT;
    }
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode FileSynthesisCallback::AudioAvailable(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* ret)
{
    if (DBG) {
        //Java:    Log.d(TAG, "FileSynthesisRequest.audioAvailable(" + buffer + "," + offset + "," + length + ")");
        Logger::D(TAG, "FileSynthesisRequest.audioAvailable\n");
    }

    AutoPtr<IFileChannel> fileChannel;

    synchronized (this) {
        if (mStatusCode == ITextToSpeech::STOPPED) {
            if (DBG) {
                Logger::D(TAG, "Request has been aborted.");
            }
            ErrorCodeOnStop(ret);
            return NOERROR;
        }
        if (mStatusCode != ITextToSpeech::TTS_SUCCESS) {
            if (DBG) {
                Logger::D(TAG, "Error was raised");
            }
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mFileChannel == NULL) {
            Logger::E(TAG, "File not open");
            mStatusCode = ITextToSpeech::ERROR_OUTPUT;
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (!mStarted) {
            Logger::E(TAG, "Start method was not called");
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        fileChannel = mFileChannel;
    }

    Int32 i;
    AutoPtr<IByteBufferHelper> bbHelper;
    AutoPtr<IByteBuffer> mBytes;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    bbHelper->Wrap(buffer, offset, length, (IByteBuffer**)&mBytes);
    ECode ec = fileChannel->Write(mBytes, &i);

    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to write to output file descriptor");
        synchronized (this) {
            CleanUp();
            mStatusCode = ITextToSpeech::ERROR_OUTPUT;
        }
        *ret = TextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode FileSynthesisCallback::Done(
    /* [out] */ Int32* ret)
{
    if (DBG){
        //Java:    Log.d(TAG, "FileSynthesisRequest.done()");
        Logger::D(TAG, String("FileSynthesisRequest.done()\n"));
    }

    AutoPtr<IFileChannel> fileChannel;

    Int32 sampleRateInHz = 0;
    Int32 audioFormat = 0;
    Int32 channelCount = 0;

    synchronized (this) {
        if (mDone) {
            if (DBG){
                //Java:    Log.d(TAG, "Duplicate call to done()");
                Logger::W(TAG, String("Duplicate call to done()\n"));
            }
            // This is not an error that would prevent synthesis. Hence no
            // setStatusCode is set.
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }


        if (mStatusCode == ITextToSpeech::STOPPED) {
            if (DBG) {
                Logger::D(TAG, "Request has been aborted.");
            }
            ErrorCodeOnStop(ret);
            return NOERROR;
        }
        if (mDispatcher != NULL && mStatusCode != ITextToSpeech::TTS_SUCCESS &&
                mStatusCode != ITextToSpeech::STOPPED) {
            mDispatcher->DispatchOnError(mStatusCode);
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mFileChannel == NULL) {
            Logger::E(TAG, "File not open");
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        mDone = TRUE;
        fileChannel = mFileChannel;
        sampleRateInHz = mSampleRateInHz;
        audioFormat = mAudioFormat;
        channelCount = mChannelCount;
    }

    ECode ec;
    Int64 size;
    Int32 number;

    // Write WAV header at start of file
    fileChannel->SetPosition(0);
    fileChannel->GetSize(&size);

    Int32 dataLength = (Int32) (size - WAV_HEADER_LENGTH);
    fileChannel->Write(MakeWavHeader(sampleRateInHz, audioFormat, channelCount, dataLength), &number);

    synchronized (mStateLock) {
        CloseFile();
        if (mDispatcher != NULL) {
            mDispatcher->DispatchOnSuccess();
        }
        *ret = TextToSpeech::TTS_SUCCESS;
        return NOERROR;
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to write to output file descriptor");
        synchronized (this) {
            CleanUp();
        }
        *ret = TextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode FileSynthesisCallback::Error()
{
    return Error(ITextToSpeech::ERROR_SYNTHESIS);
}

ECode FileSynthesisCallback::Error(
    /* [in] */ Int32 errorCode)
{
    //Java:    if (DBG) Log.d(TAG, "FileSynthesisRequest.error()");
    Logger::D(TAG, String("FileSynthesisRequest.error()\n"));
    AutoLock lock(mStateLock);
    if (mDone) {
        return NOERROR;
    }

    CleanUp();
     mStatusCode = errorCode;
    return NOERROR;
}

ECode FileSynthesisCallback::HasStarted(
    /* [out] */ Boolean* started)
{
    AutoLock lock(mStateLock);
    *started = mStarted;

    return NOERROR;
}

ECode FileSynthesisCallback::HasFinished(
    /* [out] */ Boolean* finished)
{
    synchronized (mStateLock) {
        *finished = mDone;
    }
    return NOERROR;
}

AutoPtr<IByteBuffer> FileSynthesisCallback::MakeWavHeader(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 channelCount,
    /* [in] */ Int32 dataLength)
{
    Int32 sampleSizeInBytes;
//TODO: Need CAudioFormatBuilder
    //CAudioFormatBuilder::New((IAudioFormatBuilder**)&builder);
    //builder->Build((IAudioFormat**)&mFormat);
    //IAudioFormat->GetBytesPerSample(audioFormat, &sampleSizeInBytes);

    Int32 byteRate = sampleRateInHz * sampleSizeInBytes * channelCount;
    Int16 blockAlign = (Int16) (sampleSizeInBytes * channelCount);
    Int16 bitsPerSample = (Int16) (sampleSizeInBytes * 8);

    //AutoPtr< ArrayOf<Byte> > headerBuf = ArrayOf<Byte>::Alloc(WAV_HEADER_LENGTH);

    AutoPtr<IByteBuffer> header;
    AutoPtr<IByteBufferHelper> bbh;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbh);
    bbh->Allocate(WAV_HEADER_LENGTH, (IByteBuffer**)&header);
    header->SetOrder(Elastos::IO::ByteOrder_LITTLE_ENDIAN);

    //Java:    header.put(new byte[]{ 'R', 'I', 'F', 'F' });
    AutoPtr< ArrayOf<Byte> > riff = ArrayOf<Byte>::Alloc(4);
    (*riff)[0] = 'R';    (*riff)[1] = 'I';    (*riff)[2] = 'F';    (*riff)[3] = 'F';
    header->Put((ArrayOf<Byte>*)riff);

    //Java:    header.putInt(dataLength + WAV_HEADER_LENGTH - 8);  // RIFF chunk size
    header->PutInt32(dataLength + WAV_HEADER_LENGTH - 8);

    //Java:    header.put(new byte[]{ 'W', 'A', 'V', 'E' });
    AutoPtr< ArrayOf<Byte> > wave = ArrayOf<Byte>::Alloc(4);
    (*wave)[0] = 'W';    (*wave)[1] = 'A';    (*wave)[2] = 'V';    (*wave)[3] = 'E';
    header->Put((ArrayOf<Byte>*)wave);

    //Java:    header.put(new byte[]{ 'f', 'm', 't', ' ' });
    AutoPtr< ArrayOf<Byte> > fmt = ArrayOf<Byte>::Alloc(4);
    (*fmt)[0] = 'f';    (*fmt)[1] = 'm';    (*fmt)[2] = 't';    (*fmt)[3] = ' ';
    header->Put((ArrayOf<Byte>*)fmt);

    //Java:    header.putInt(16);  // size of fmt chunk
    header->PutInt32(16);

    //Java:    header.putShort(WAV_FORMAT_PCM);    header.putShort((short) channelCount);
    header->PutInt16(WAV_FORMAT_PCM);
    header->PutInt16((Int16) channelCount);

    //Java:    header.putInt(sampleRateInHz);    header.putInt(byteRate);
    header->PutInt32(sampleRateInHz);
    header->PutInt32(byteRate);

    //Java:    header.putShort(blockAlign);    header.putShort(bitsPerSample);
    header->PutInt16(blockAlign);
    header->PutInt16(bitsPerSample);

    //Java:    header.put(new byte[]{ 'd', 'a', 't', 'a' });
    AutoPtr< ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(4);
    (*data)[0] = 'd';    (*data)[1] = 'a';    (*data)[2] = 't';    (*data)[3] = 'a';
    header->Put((ArrayOf<Byte>*)data);

    //Java:    header.putInt(dataLength);
    header->PutInt32(dataLength);

    IBuffer::Probe(header)->Flip();

    return header;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
