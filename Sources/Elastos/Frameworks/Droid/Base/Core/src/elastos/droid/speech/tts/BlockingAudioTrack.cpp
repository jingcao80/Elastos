
#include "elastos/droid/speech/tts/BlockingAudioTrack.h"
#include "elastos/droid/media/CAudioTrack.h"
#include "elastos/droid/media/CAudioFormat.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <unistd.h>     //for Method: unsigned sleep(unsigned)
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Media::CAudioTrack;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::CAudioFormat;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

const String BlockingAudioTrack::TAG("TTS.BlockingAudioTrack");
const Boolean BlockingAudioTrack::DBG = FALSE;

const Int64 BlockingAudioTrack::MIN_SLEEP_TIME_MS = 20;

const Int64 BlockingAudioTrack::MAX_SLEEP_TIME_MS = 2500;

const Int64 BlockingAudioTrack::MAX_PROGRESS_WAIT_MS = MAX_SLEEP_TIME_MS;

const Int32 BlockingAudioTrack::MIN_AUDIO_BUFFER_SIZE = 8192;

CAR_INTERFACE_IMPL(BlockingAudioTrack, Object, IBlockingAudioTrack);

BlockingAudioTrack::BlockingAudioTrack()
{}

BlockingAudioTrack::~BlockingAudioTrack()
{}

ECode BlockingAudioTrack::constructor()
{
    return NOERROR;
}

ECode BlockingAudioTrack::constructor(
    /* [in] */ AudioOutputParams* audioParams,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 channelCount)
{
    mAudioParams = audioParams;
    mSampleRateInHz = sampleRate;
    mAudioFormat = audioFormat;
    mChannelCount = channelCount;

    Int32 fmt;
    CAudioFormat::GetBytesPerSample(mAudioFormat, &fmt);
    mBytesPerFrame = fmt * mChannelCount;
    mIsShortUtterance = FALSE;
    mAudioBufferSize = 0;
    mBytesWritten = 0;

    mAudioTrack = NULL;
    mStopped = FALSE;

    return NOERROR;
}

ECode BlockingAudioTrack::Init(
    /* [out] */ Boolean* ret)
{
    AutoPtr<IAudioTrack> track = CreateStreamingAudioTrack();
    {
        AutoLock lock(mAudioTrackLock);
        mAudioTrack = track;
    }

    if (track == NULL) {
        *ret = FALSE;
    } else {
        *ret = TRUE;
    }

    return NOERROR;
}

ECode BlockingAudioTrack::Stop()
{
    AutoLock lock(mAudioTrackLock);
    if (mAudioTrack != NULL) {
        mAudioTrack->Stop();
    }
    mStopped = TRUE;

    return NOERROR;
}

ECode BlockingAudioTrack::Write(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* ret)
{
    AutoPtr<IAudioTrack> track = NULL;
    {
        AutoLock lock(mAudioTrackLock);
        track = mAudioTrack;
    }

    if (track == NULL || mStopped) {
        *ret = -1;
    }
    const Int32 bytesWritten = WriteToAudioTrack(track, data);

    mBytesWritten += bytesWritten;
    *ret = bytesWritten;

    return NOERROR;
}

ECode BlockingAudioTrack::WaitAndRelease()
{
    AutoPtr<IAudioTrack> track = NULL;
    {
        AutoLock lock(mAudioTrackLock);
        track = mAudioTrack;
    }
    if (track == NULL) {
        if (DBG){
            //Java:    Log.d(TAG, "Audio track null [duplicate call to waitAndRelease ?]");
            Logger::D(TAG, "Audio track null [duplicate call to waitAndRelease ?]\n");
            return NOERROR;
        }

    }

    // For "small" audio tracks, we have to stop() them to make them mixable,
    // else the audio subsystem will wait indefinitely for us to fill the buffer
    // before rendering the track mixable.
    //
    // If mStopped is true, the track would already have been stopped, so not
    // much point not doing that again.
    if (mBytesWritten < mAudioBufferSize && !mStopped) {
        if (DBG) {
            Int32 playState;
            track->GetPlayState(&playState);
            Logger::D(TAG, "Stopping audio track to flush audio, state was: %d ,stopped = %d\n", playState, mStopped);
        }

        mIsShortUtterance = TRUE;
        track->Stop();
    }

    // Block until the audio track is done only if we haven't stopped yet.
    if (!mStopped) {
        if (DBG) {
            Int32 code = Object::GetHashCode(mAudioTrack);
            Logger::D(TAG, "Waiting for audio track to complete: %d\n", code);
        }
        BlockUntilDone(mAudioTrack);
    }

    // The last call to AudioTrack.write( ) will return only after
    // all data from the audioTrack has been sent to the mixer, so
    // it's safe to release at this point.
    if (DBG) {
        Int32 code = Object::GetHashCode(mAudioTrack);
        Logger::D(TAG, "Releasing audio track [%d]\n", code);
    }
    {
        AutoLock lock(mAudioTrackLock);
        mAudioTrack = NULL;
    }
    track->ReleaseResources();
    return NOERROR;
}


Int32 BlockingAudioTrack::GetChannelConfig(
    /* [in] */ Int32 channelCount)
{
    if (channelCount == 1) {
        return IAudioFormat::CHANNEL_OUT_MONO;
        return 0;
    } else if (channelCount == 2){
        return IAudioFormat::CHANNEL_OUT_STEREO;
        return 0;
    }

    return 0;
}

Int64 BlockingAudioTrack::GetAudioLengthMs(
    /* [in] */ Int32 numBytes)
{
    Int32 unconsumedFrames = numBytes / mBytesPerFrame;
    Int64 estimatedTimeMs = unconsumedFrames * 1000 / mSampleRateInHz;

    return estimatedTimeMs;
}

Int32 BlockingAudioTrack::WriteToAudioTrack(
    /* [in] */ IAudioTrack* audioTrack,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    Int32 playState;
    if ((audioTrack->GetPlayState(&playState), playState) != IAudioTrack::PLAYSTATE_PLAYING) {
        if (DBG) {
            Int32 code = Object::GetHashCode(audioTrack);
            Logger::D(TAG, "AudioTrack not playing, restarting: %d\n", code);
        }
        audioTrack->Play();
    }

    Int32 count = 0;
    while (count < bytes->GetLength()) {
        // Note that we don't take bufferCopy.mOffset into account because
        // it is guaranteed to be 0.
        Int32 written;
        audioTrack->Write(bytes, count, bytes->GetLength(), &written);
        if (written <= 0) {
            break;
        }
        count += written;
    }
    return count;
}

AutoPtr<IAudioTrack> BlockingAudioTrack::CreateStreamingAudioTrack()
{
    const Int32 channelConfig = GetChannelConfig(mChannelCount);

    Int32 minBufferSizeInBytes;
    CAudioTrack::GetMinBufferSize(mSampleRateInHz, channelConfig, mAudioFormat, &minBufferSizeInBytes);
    Int32 bufferSizeInBytes = Elastos::Core::Math::Max(MIN_AUDIO_BUFFER_SIZE, minBufferSizeInBytes);

    AutoPtr<IAudioFormat> audioFormat;
    /*
    AudioFormat audioFormat = (new AudioFormat.Builder())
            .setChannelMask(channelConfig)
            .setEncoding(mAudioFormat)
            .setSampleRate(mSampleRateInHz).build();
    */
    AutoPtr<IAudioTrack> audioTrack;
    CAudioTrack::New(mAudioParams->mAudioAttributes,
        audioFormat, bufferSizeInBytes, IAudioTrack::MODE_STREAM,
        mAudioParams->mSessionId, (IAudioTrack**)&audioTrack);

    Int32 atState;
    if ((audioTrack->GetState(&atState), atState) != IAudioTrack::STATE_INITIALIZED) {
        Logger::W(TAG, "Unable to create audio track.\n");
        audioTrack->ReleaseResources();
        return NULL;
    }

    mAudioBufferSize = bufferSizeInBytes;

    SetupVolume(audioTrack, mAudioParams->mVolume, mAudioParams->mPan);
    return audioTrack;
}

void BlockingAudioTrack::BlockUntilDone(
    /* [in] */ IAudioTrack* audioTrack)
{
    if (mBytesWritten <= 0) {
        return;
    }

    if (mIsShortUtterance) {
        // In this case we would have called AudioTrack#stop() to flush
        // buffers to the mixer. This makes the playback head position
        // unobservable and notification markers do not work reliably. We
        // have no option but to wait until we think the track would finish
        // playing and release it after.
        //
        // This isn't as bad as it looks because (a) We won't end up waiting
        // for much longer than we should because even at 4khz mono, a short
        // utterance weighs in at about 2 seconds, and (b) such short utterances
        // are expected to be relatively infrequent and in a stream of utterances
        // this shows up as a slightly longer pause.
        BlockUntilEstimatedCompletion();
    } else {
        BlockUntilCompletion(audioTrack);
    }
}

void BlockingAudioTrack::BlockUntilEstimatedCompletion()
{
    const Int32 lengthInFrames = mBytesWritten / mBytesPerFrame;
    const Int64 estimatedTimeMs = (lengthInFrames * 1000 / mSampleRateInHz);

    if (DBG) {
        Logger::D(TAG, "About to sleep for: %ld ms for a short utterance", estimatedTimeMs);
    }

    //try {
        //Thread.sleep(estimatedTimeMs);
        sleep(estimatedTimeMs);
    //} catch (InterruptedException ie) {
        // Do nothing.
    //}
}

void BlockingAudioTrack::BlockUntilCompletion(
    /* [in] */ IAudioTrack* audioTrack)
{
    const Int32 lengthInFrames = mBytesWritten / mBytesPerFrame;

    Int32 previousPosition = -1;
    Int32 currentPosition = 0;
    Int64 blockedTimeMs = 0;

    Int32 playBackHeadPosition;
    Int32 playState;
    while ((currentPosition = (audioTrack->GetPlaybackHeadPosition(&playBackHeadPosition), playBackHeadPosition)) < lengthInFrames &&
                (audioTrack->GetPlayState(&playState), playState) == /*IAudioTrack::PLAYSTATE_PLAYING*/3 && !mStopped) {
        Int32 sampleRate;
        audioTrack->GetSampleRate(&sampleRate);
        const Int64 estimatedTimeMs = ((lengthInFrames - currentPosition) * 1000) / sampleRate;
        const Int64 sleepTimeMs = Clip(estimatedTimeMs, MIN_SLEEP_TIME_MS, MAX_SLEEP_TIME_MS);

        // Check if the audio track has made progress since the last loop
        // iteration. We should then add in the amount of time that was
        // spent sleeping in the last iteration.
        if (currentPosition == previousPosition) {
            // This works only because the sleep time that would have been calculated
            // would be the same in the previous iteration too.
            blockedTimeMs += sleepTimeMs;
            // If we've taken too long to make progress, bail.
            if (blockedTimeMs > MAX_PROGRESS_WAIT_MS) {
                Logger::D(TAG, "Waited unsuccessfully for %ld ms for AudioTrack to make progress, Aborting\n", MAX_PROGRESS_WAIT_MS);
                break;
            }
        }
        else {
            blockedTimeMs = 0;
        }
        previousPosition = currentPosition;

        if (DBG) {
            Logger::D(TAG, "About to sleep for: %ld ms, Playback position: %d, Length in frames: %d\n", sleepTimeMs, currentPosition, lengthInFrames);
        }
        //try {
            sleep(sleepTimeMs);
        //} catch (InterruptedException ie) {
            //break;
        //}
    }
}

void BlockingAudioTrack::SetupVolume(
    /* [in] */ IAudioTrack* audioTrack,
    /* [in] */ Float volume,
    /* [in] */ Float pan)
{
    const Float vol = Clip(volume, 0.0f, 1.0f);
    const Float panning = Clip(pan, -1.0f, 1.0f);

    Float volLeft = vol;
    Float volRight = vol;
    if (panning > 0.0f) {
        volLeft *= (1.0f - panning);
    }
    else if (panning < 0.0f) {
        volRight *= (1.0f + panning);
    }
    if (DBG) {
        Logger::D(TAG, "volLeft= %f, volRight= %f\n", volLeft, volRight);
    }
    Int32 ssvState;
    audioTrack->SetStereoVolume(volLeft, volRight, &ssvState);
    if (ssvState != IAudioTrack::SUCCESS) {
        Logger::E(TAG, "Failed to set volume\n");
    }
}

Int64 BlockingAudioTrack::Clip(
    /* [in] */ Int64 value,
    /* [in] */ Int64 min,
    /* [in] */ Int64 max)
{
    return value < min ? min : (value < max ? value : max);
}

Float BlockingAudioTrack::Clip(
    /* [in] */ Float value,
    /* [in] */ Float min,
    /* [in] */ Float max)
{
    return value < min ? min : (value < max ? value : max);
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
