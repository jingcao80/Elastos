
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Media.h"
#include "elastos/droid/webkit/native/media/AudioRecordInput.h"
#include "elastos/droid/webkit/native/media/api/AudioRecordInput_dec.h"

//TODO #include "elastos/droid/media/CAudioRecord.h"
//TODO #include "elastos/droid/media/CAudioRecordHelper.h"
//TODO #include "elastos/droid/media/audiofx/CAcousticEchoCancelerHelper.h"
#include "elastos/droid/os/Process.h"

//TODO #include <elastos/io/CByteBufferHelper.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::IMediaRecorderAudioSource;
using Elastos::Droid::Media::Audiofx::IAudioEffect;
using Elastos::Droid::Media::Audiofx::IAudioEffectDescriptor;
using Elastos::Droid::Media::IAudioRecordHelper;
using Elastos::Droid::Media::Audiofx::IAcousticEchoCancelerHelper;
//TODO using Elastos::Droid::Media::Audiofx::CAcousticEchoCancelerHelper;
//TODO using Elastos::Droid::Media::CAudioRecord;
//TODO using Elastos::Droid::Media::CAudioRecordHelper;

using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
//TODO using Elastos::IO::CByteBufferHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Core::IThread;
using Elastos::Utility::IUUID;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

//===============================================================
//              AudioRecordInput::AudioRecordThread
//===============================================================

AudioRecordInput::AudioRecordThread::AudioRecordThread(
    /* [in] */ AudioRecordInput* owner)
    : mOwner(owner)
    , mKeepAlive(TRUE)
{
}

//@Override
ECode AudioRecordInput::AudioRecordThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_URGENT_AUDIO);
    // try {
    ECode ecode = mOwner->mAudioRecord->StartRecording();
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "startRecording failed", e);
    //     return;
    // }
    if (FAILED(ecode))
    {
        Logger::E(TAG, "startRecording failed, ecode:0x%x", ecode);
        return ecode;
    }

    while (mKeepAlive) {
        Int32 capacity;
        AutoPtr<IBuffer> buf = IBuffer::Probe(mOwner->mBuffer);
        buf->GetCapacity(&capacity);
        Int32 bytesRead;
        mOwner->mAudioRecord->Read(mOwner->mBuffer, capacity, &bytesRead);
        if (bytesRead > 0) {
            mOwner->NativeOnData(mOwner->mNativeAudioRecordInputStream, bytesRead,
                         mOwner->mHardwareDelayBytes);
        }
        else {
            Logger::E(TAG, "read failed: %d", bytesRead);
            if (bytesRead == IAudioRecord::ERROR_INVALID_OPERATION) {
                // This can happen if there is already an active
                // AudioRecord (e.g. in another tab).
                mKeepAlive = FALSE;
            }
        }
    }

    // try {
    ecode = mOwner->mAudioRecord->Stop();
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "stop failed", e);
    // }
    if(FAILED(ecode))
    {
        Logger::E(TAG, "stop failed, ecode:0x%x", ecode);
    }
    return ecode;
}

void AudioRecordInput::AudioRecordThread::JoinRecordThread()
{
    mKeepAlive = FALSE;
    AutoPtr<IThread> thread = IThread::Probe(this);
    Boolean alive;
    while (thread->IsAlive(&alive), alive) {
        // try {
            Join();
        // } catch (InterruptedException e) {
        //     // Ignore.
        // }
    }
}

//===============================================================
//                      AudioRecordInput
//===============================================================

const String AudioRecordInput::TAG("AudioRecordInput");
// Set to true to enable debug logs. Always check in as false.
const Boolean AudioRecordInput::DEBUG = FALSE;
// We are unable to obtain a precise measurement of the hardware delay on
// Android. This is a conservative lower-bound based on measurments. It
// could surely be tightened with further testing.
const Int32 AudioRecordInput::HARDWARE_DELAY_MS;

AudioRecordInput::AudioRecordInput(
    /* [in] */ Int64 nativeAudioRecordInputStream,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channels,
    /* [in] */ Int32 bitsPerSample,
    /* [in] */ Int32 bytesPerBuffer,
    /* [in] */ Boolean usePlatformAEC)
    : mNativeAudioRecordInputStream(nativeAudioRecordInputStream)
    , mSampleRate(sampleRate)
    , mChannels(channels)
    , mBitsPerSample(bitsPerSample)
    , mHardwareDelayBytes(HARDWARE_DELAY_MS * sampleRate / 1000 * bitsPerSample / 8)
    , mUsePlatformAEC(usePlatformAEC)
{
    // We use a direct buffer so that the native class can have access to
    // the underlying memory address. This avoids the need to copy from a
    // jbyteArray to native memory. More discussion of this here:
    // http://developer.android.com/training/articles/perf-jni.html
    //ByteBuffer::AllocateDirect(bytesPerBuffer, (IByteBuffer**)&mBuffer);
    AutoPtr<IByteBufferHelper> bbh;
    //TODO CByteBufferHelper::AcquierSingleton((**IByteBufferHelper)&bbh);
    bbh->AllocateDirect(bytesPerBuffer, (IByteBuffer**)&mBuffer);

    // Rather than passing the ByteBuffer with every OnData call (requiring
    // the potentially expensive GetDirectBufferAddress) we simply have the
    // the native class cache the address to the memory once.
    //
    // Unfortunately, profiling with traceview was unable to either confirm
    // or deny the advantage of this approach, as the values for
    // nativeOnData() were not stable across runs.
    NativeCacheDirectBufferAddress(mNativeAudioRecordInputStream, mBuffer);
}

//@CalledByNative return AudioRecordInput
AutoPtr<IInterface> AudioRecordInput::CreateAudioRecordInput(
    /* [in] */ Int64 nativeAudioRecordInputStream,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channels,
    /* [in] */ Int32 bitsPerSample,
    /* [in] */ Int32 bytesPerBuffer,
    /* [in] */ Boolean usePlatformAEC)
{
    AutoPtr<AudioRecordInput> ret = new AudioRecordInput(nativeAudioRecordInputStream, sampleRate, channels,
                                bitsPerSample, bytesPerBuffer, usePlatformAEC);
    return TO_IINTERFACE(ret);
}

//@SuppressLint("NewApi")
//@CalledByNative
Boolean AudioRecordInput::Open()
{
    if (mAudioRecord != NULL) {
        Logger::E(TAG, "open() called twice without a close()");
        return FALSE;
    }

    Int32 channelConfig;
    if (mChannels == 1) {
        channelConfig = IAudioFormat::CHANNEL_IN_MONO;
    }
    else if (mChannels == 2) {
        channelConfig = IAudioFormat::CHANNEL_IN_STEREO;
    }
    else {
        Logger::E(TAG, "Unsupported number of channels: %d", mChannels);
        return FALSE;
    }

    Int32 audioFormat;
    if (mBitsPerSample == 8) {
        audioFormat = IAudioFormat::ENCODING_PCM_8BIT;
    }
    else if (mBitsPerSample == 16) {
        audioFormat = IAudioFormat::ENCODING_PCM_16BIT;
    }
    else {
        Logger::E(TAG, "Unsupported bits per sample: %d", mBitsPerSample);
        return FALSE;
    }

    // TODO(ajm): Do we need to make this larger to avoid underruns? The
    // Android documentation notes "this size doesn't guarantee a smooth
    // recording under load".
    AutoPtr<IAudioRecordHelper> arHelper;
    //TODO CAudioRecordHelper::AcquireSingleton((IAudioRecordHelper**)&arHelper);
    Int32 minBufferSize;
    arHelper->GetMinBufferSize(mSampleRate, channelConfig, audioFormat, &minBufferSize);
    if (minBufferSize < 0) {
        Logger::E(TAG, "getMinBufferSize error: %d", minBufferSize);
        return FALSE;
    }

    // We will request mBuffer.capacity() with every read call. The
    // underlying AudioRecord buffer should be at least this large.
    Int32 capacity;
    AutoPtr<IBuffer> buf = IBuffer::Probe(mBuffer);
    buf->GetCapacity(&capacity);
    Int32 audioRecordBufferSizeInBytes = Elastos::Core::Math::Max(capacity, minBufferSize);
    // try {
        // TODO(ajm): Allow other AudioSource types to be requested?
    ECode ecode = NOERROR;
    //TODO ecode = CAudioRecord::New(IAudioSource::VOICE_COMMUNICATION,
    //                                   mSampleRate,
    //                                   channelConfig,
    //                                   audioFormat,
   //                                    audioRecordBufferSizeInBytes,
   //                                    (IAudioRecord**)&mAudioRecord);
    // } catch (IllegalArgumentException e) {
    //     Log.e(TAG, "AudioRecord failed", e);
    //     return false;
    // }
    if(FAILED(ecode))
    {
        Logger::E(TAG, "create AudioRecord failed, ecode:0x%x, buffersize:%d", ecode, audioRecordBufferSizeInBytes);
    }

    AutoPtr<IAcousticEchoCancelerHelper> aecHelper;
    //TODO CAcousticEchoCancelerHelper::AcquireSingleton((IAcousticEchoCancelerHelper**)&aecHelper);
    Boolean isAvailable = FALSE;
    aecHelper->IsAvailable(&isAvailable);
    if (isAvailable) {
        Int32 id;
        mAudioRecord->GetAudioSessionId(&id);
        aecHelper->Create(id, (IAcousticEchoCanceler**)&mAEC);
        if (mAEC == NULL) {
            Logger::E(TAG, "AcousticEchoCanceler.create failed");
            return FALSE;
        }

        Int32 ret;
        AutoPtr<IAudioEffect> iae = IAudioEffect::Probe(mAEC);
        iae->SetEnabled(mUsePlatformAEC, &ret);
        if (ret != IAudioEffect::SUCCESS) {
            Logger::E(TAG, "setEnabled error: %d", ret);
            return FALSE;
        }

        if (DEBUG) {
            AutoPtr<IAudioEffect> iae = IAudioEffect::Probe(mAEC);
            AutoPtr<IAudioEffectDescriptor> descriptor;
            iae->GetDescriptor((IAudioEffectDescriptor**)&descriptor);
            String name, implementor, uuid;
            AutoPtr<IUUID> iuuid;
            descriptor->GetName(&name);
            descriptor->GetImplementor(&implementor);
            descriptor->GetUuid((IUUID**)&iuuid);
            iuuid->ToString(&uuid);
            Logger::D(TAG, "AcousticEchoCanceler \n\
                    name: %s \n \
                    implementor: %s \n \
                    uuid: ", name.string(), implementor.string(), uuid.string());
        }
    }

    return TRUE;
}

//@CalledByNative
void AudioRecordInput::Start()
{
    if (mAudioRecord == NULL) {
        Logger::E(TAG, "start() called before open().");
        return;
    }

    if (mAudioRecordThread != NULL) {
        // start() was already called.
        return;
    }

    mAudioRecordThread = new AudioRecordThread(this);
    mAudioRecordThread->Start();
}

//@CalledByNative
void AudioRecordInput::Stop()
{
    if (mAudioRecordThread == NULL) {
        // start() was never called, or stop() was already called.
        return;
    }

    mAudioRecordThread->JoinRecordThread();
    mAudioRecordThread = NULL;
}

//@SuppressLint("NewApi")
//@CalledByNative
void AudioRecordInput::Close()
{
    if (mAudioRecordThread != NULL) {
        Logger::E(TAG, "close() called before stop().");
        return;
    }

    if (mAudioRecord == NULL) {
        // open() was not called.
        return;
    }

    if (mAEC != NULL) {
        AutoPtr<IAudioEffect> iae = IAudioEffect::Probe(mAEC);
        iae->ReleaseResources();
        mAEC = NULL;
    }

    mAudioRecord->ReleaseResources();
    mAudioRecord = NULL;
}

void AudioRecordInput::NativeCacheDirectBufferAddress(
    /* [in] */ Int64 nativeAudioRecordInputStream,
    /* [in] */ IByteBuffer* buffer)
{
    Elastos_AudioRecordInput_nativeCacheDirectBufferAddress(THIS_PROBE(IInterface), (Handle32)nativeAudioRecordInputStream, TO_IINTERFACE(buffer));
}

void AudioRecordInput::NativeOnData(
    /* [in] */ Int64 nativeAudioRecordInputStream,
    /* [in] */ Int32 size,
    /* [in] */ Int32 hardwareDelayBytes)
{
    Elastos_AudioRecordInput_nativeOnData(THIS_PROBE(IInterface), (Handle32)nativeAudioRecordInputStream, size, hardwareDelayBytes);
}

Boolean AudioRecordInput::Open(
    /* [in] */IInterface* obj)
{
    AutoPtr<AudioRecordInput> ariObj = (AudioRecordInput*)IObject::Probe(obj);
    if (NULL == ariObj)
    {
        Logger::E(TAG, "AudioRecordInput::Open, ariObj is NULL");
        return FALSE;
    }
    return ariObj->Open();
}

void AudioRecordInput::Start(
    /* [in] */IInterface* obj)
{
    AutoPtr<AudioRecordInput> ariObj = (AudioRecordInput*)IObject::Probe(obj);
    if (NULL == ariObj)
    {
        Logger::E(TAG, "AudioRecordInput::Start, ariObj is NULL");
        return;
    }
    ariObj->Start();
}

void AudioRecordInput::Stop(
    /* [in] */IInterface* obj)
{
    AutoPtr<AudioRecordInput> ariObj = (AudioRecordInput*)(IObject::Probe(obj));
    if (NULL == ariObj)
    {
        Logger::E(TAG, "AudioRecordInput::Stop, ariObj is NULL");
        return;
    }
    ariObj->Stop();
}

void AudioRecordInput::Close(
    /* [in] */IInterface* obj)
{
    AutoPtr<AudioRecordInput> ariObj = (AudioRecordInput*)(IObject::Probe(obj));
    if (NULL == ariObj)
    {
        Logger::E(TAG, "AudioRecordInput::Close, ariObj is NULL");
        return;
    }
    ariObj->Close();
}

} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
