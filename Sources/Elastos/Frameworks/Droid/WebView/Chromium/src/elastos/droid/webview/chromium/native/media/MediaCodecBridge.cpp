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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/media/MediaCodecBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/media/api/MediaCodecBridge_dec.h"

//TODO #include "elastos/droid/media/CMediaCodecListHelper.h"
//TODO #include "elastos/droid/media/CMediaCodecHelper.h"
#include "elastos/droid/os/Build.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioTrack;
using Elastos::Droid::Media::CAudioTrack;
using Elastos::Droid::Media::IAudioTrackHelper;
using Elastos::Droid::Media::CAudioTrackHelper;
using Elastos::Droid::Media::IMediaCodec;
using Elastos::Droid::Media::IMediaCodecHelper;
//TODO using Elastos::Droid::Media::CMediaCodecHelper;
using Elastos::Droid::Media::IMediaCodecInfo;
using Elastos::Droid::Media::IMediaCodecCryptoInfo;
using Elastos::Droid::Media::CMediaCodecCryptoInfo;
using Elastos::Droid::Media::IMediaCodecList;
using Elastos::Droid::Media::IMediaCodecListHelper;
using Elastos::Droid::Media::IMediaCodecBufferInfo;
using Elastos::Droid::Media::CMediaCodecBufferInfo;
//TODO using Elastos::Droid::Media::CMediaCodecListHelper;
using Elastos::Droid::Media::IMediaCrypto;
using Elastos::Droid::Media::IMediaFormatHelper;
using Elastos::Droid::Media::CMediaFormatHelper;
using Elastos::Droid::Media::IMediaFormat;
//TODO using Elastos::Droid::Media::ICodecCapabilities;
using Elastos::Droid::View::ISurface;

using Elastos::IO::IBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//===============================================================
//              MediaCodecBridge::DequeueInputResult
//===============================================================

MediaCodecBridge::DequeueInputResult::DequeueInputResult(
    /* [in] */ Int32 status,
    /* [in] */ Int32 index)
    : mStatus(status)
    , mIndex(index)
{
}

//@CalledByNative("DequeueInputResult")
Int32 MediaCodecBridge::DequeueInputResult::Status()
{
    return mStatus;
}

//@CalledByNative("DequeueInputResult")
Int32 MediaCodecBridge::DequeueInputResult::Index()
{
    return mIndex;
}

//callback function definition
Int32 MediaCodecBridge::DequeueInputResult::Status(
            /* [in] */IInterface* obj)
{
    AutoPtr<DequeueInputResult> mObj = (DequeueInputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueInputResult::Status, mObj is NULL");
        return 0;//TODO what kind of value is OK
    }
    return mObj->Status();
}

Int32 MediaCodecBridge::DequeueInputResult::Index(
    /* [in] */IInterface* obj)
{
    AutoPtr<DequeueInputResult> mObj = (DequeueInputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueInputResult::Index, mObj is NULL");
        return -1;//TODO what kind of value is OK
    }
    return mObj->Index();
}

//===============================================================
//                MediaCodecBridge::CodecInfo
//===============================================================

MediaCodecBridge::CodecInfo::CodecInfo(
    /* [in] */ const String& codecType,
    /* [in] */ const String& codecName,
    /* [in] */ Int32 direction)
    : mCodecType(codecType)
    , mCodecName(codecName)
    , mDirection(direction)
{
}

//@CalledByNative("CodecInfo")
String MediaCodecBridge::CodecInfo::CodecType()
{
    return mCodecType;
}

//@CalledByNative("CodecInfo")
String MediaCodecBridge::CodecInfo::CodecName()
{
    return mCodecName;
}

//@CalledByNative("CodecInfo")
Int32 MediaCodecBridge::CodecInfo::Direction()
{
    return mDirection;
}

String MediaCodecBridge::CodecInfo::CodecType(
    /* [in] */IInterface* obj)
{
    AutoPtr<CodecInfo> mObj = (CodecInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::CodecInfo::CodecType, mObj is NULL");
        return String(NULL);
    }
    return mObj->CodecType();
}

String MediaCodecBridge::CodecInfo::CodecName(
    /* [in] */IInterface* obj)
{
    AutoPtr<CodecInfo> mObj = (CodecInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::CodecInfo::CodecName, mObj is NULL");
        return String(NULL);
    }
    return mObj->CodecName();
}

Int32 MediaCodecBridge::CodecInfo::Direction(
    /* [in] */IInterface* obj)
{
    AutoPtr<CodecInfo> mObj = (CodecInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::CodecInfo::Direction, mObj is NULL");
        return 0;//TODO if this is OK
    }
    return mObj->Direction();
}
//===============================================================
//            MediaCodecBridge::DequeueOutputResult
//===============================================================

MediaCodecBridge::DequeueOutputResult::DequeueOutputResult(
    /* [in] */ Int32 status,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 offset,
    /* [in] */ Int64 presentationTimeMicroseconds,
    /* [in] */ Int32 numBytes)
    : mStatus(status)
    , mIndex(index)
    , mFlags(flags)
    , mOffset(offset)
    , mPresentationTimeMicroseconds(presentationTimeMicroseconds)
    , mNumBytes(numBytes)
{
}

//@CalledByNative("DequeueOutputResult")
Int32 MediaCodecBridge::DequeueOutputResult::Status()
{
    return mStatus;
}

//@CalledByNative("DequeueOutputResult")
Int32 MediaCodecBridge::DequeueOutputResult::Index()
{
    return mIndex;
}

//@CalledByNative("DequeueOutputResult")
Int32 MediaCodecBridge::DequeueOutputResult::Flags()
{
    return mFlags;
}

//@CalledByNative("DequeueOutputResult")
Int32 MediaCodecBridge::DequeueOutputResult::Offset()
{
    return mOffset;
}

//@CalledByNative("DequeueOutputResult")
Int64 MediaCodecBridge::DequeueOutputResult::PresentationTimeMicroseconds()
{
    return mPresentationTimeMicroseconds;
}

//@CalledByNative("DequeueOutputResult")
Int32 MediaCodecBridge::DequeueOutputResult::NumBytes()
{
    return mNumBytes;
}

Int32 MediaCodecBridge::DequeueOutputResult::Status(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::Status, mObj is NULL");
        return 0;
    }
    return mObj->Status();
}

Int32 MediaCodecBridge::DequeueOutputResult::Index(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::Index, mObj is NULL");
        return 0;
    }
    return mObj->Index();
}

Int32 MediaCodecBridge::DequeueOutputResult::Flags(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::Flags, mObj is NULL");
        return 0;
    }
    return mObj->Flags();
}

Int32 MediaCodecBridge::DequeueOutputResult::Offset(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::Offset, mObj is NULL");
        return 0;
    }
    return mObj->Offset();
}

Int64 MediaCodecBridge::DequeueOutputResult::PresentationTimeMicroseconds(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::PresentationTimeMicroseconds, mObj is NULL");
        return 0;
    }
    return mObj->PresentationTimeMicroseconds();
}

Int32 MediaCodecBridge::DequeueOutputResult::NumBytes(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DequeueOutputResult> mObj = (DequeueOutputResult*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputResult::NumBytes, mObj is NULL");
        return 0;
    }
    return mObj->NumBytes();
}

//===============================================================
//                      MediaCodecBridge
//===============================================================

const String MediaCodecBridge::TAG("MediaCodecBridge");

// Error code for MediaCodecBridge. Keep this value in sync with
// MediaCodecStatus in media_codec_bridge.h.
const Int32 MediaCodecBridge::MEDIA_CODEC_OK;
const Int32 MediaCodecBridge::MEDIA_CODEC_DEQUEUE_INPUT_AGAIN_LATER;
const Int32 MediaCodecBridge::MEDIA_CODEC_DEQUEUE_OUTPUT_AGAIN_LATER;
const Int32 MediaCodecBridge::MEDIA_CODEC_OUTPUT_BUFFERS_CHANGED;
const Int32 MediaCodecBridge::MEDIA_CODEC_OUTPUT_FORMAT_CHANGED;
const Int32 MediaCodecBridge::MEDIA_CODEC_INPUT_END_OF_STREAM;
const Int32 MediaCodecBridge::MEDIA_CODEC_OUTPUT_END_OF_STREAM;
const Int32 MediaCodecBridge::MEDIA_CODEC_NO_KEY;
const Int32 MediaCodecBridge::MEDIA_CODEC_STOPPED;
const Int32 MediaCodecBridge::MEDIA_CODEC_ERROR;

// Codec direction.  Keep this in sync with media_codec_bridge.h.
const Int32 MediaCodecBridge::MEDIA_CODEC_DECODER;
const Int32 MediaCodecBridge::MEDIA_CODEC_ENCODER;

// Max adaptive playback size to be supplied to the decoder.
const Int32 MediaCodecBridge::MAX_ADAPTIVE_PLAYBACK_WIDTH;
const Int32 MediaCodecBridge::MAX_ADAPTIVE_PLAYBACK_HEIGHT;

// After a flush(), dequeueOutputBuffer() can often produce empty presentation timestamps
// for several frames. As a result, the player may find that the time does not increase
// after decoding a frame. To detect this, we check whether the presentation timestamp from
// dequeueOutputBuffer() is larger than input_timestamp - MAX_PRESENTATION_TIMESTAMP_SHIFT_US
// after a flush. And we set the presentation timestamp from dequeueOutputBuffer() to be
// non-decreasing for the remaining frames.
const Int64 MediaCodecBridge::MAX_PRESENTATION_TIMESTAMP_SHIFT_US;

MediaCodecBridge::MediaCodecBridge(
    /* [in] */ IMediaCodec* mediaCodec,
    /* [in] */ const String& mime,
    /* [in] */ Boolean adaptivePlaybackSupported)
    : mMediaCodec(mediaCodec)
    , mFlushed(TRUE)
    , mLastPresentationTimeUs(0)
    , mMime(mime)
    , mAdaptivePlaybackSupported(adaptivePlaybackSupported)
{
    assert(mediaCodec != NULL);
}

/**
 * Get a list of supported android codec mimes.
 */
//@CalledByNative
AutoPtr<ArrayOf<IInterface*> > MediaCodecBridge::GetCodecsInfo()
{
    // Return the first (highest-priority) codec for each MIME type.
    //Map<String, CodecInfo> encoderInfoMap = new HashMap<String, CodecInfo>();
    //Map<String, CodecInfo> decoderInfoMap = new HashMap<String, CodecInfo>();
    AutoPtr<IMap> encoderInfoMap;
    CHashMap::New((IMap**)&encoderInfoMap);
    AutoPtr<IMap> decoderInfoMap;
    CHashMap::New((IMap**)&decoderInfoMap);
    AutoPtr<IMediaCodecListHelper> mclHelper;
    //TODO CMediaCodecListHelper::AcquireSingleton((IMediaCodecListHelper**)&mclHelper);

    Int32 count;
    mclHelper->GetCodecCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        //MediaCodecInfo info = MediaCodecList.getCodecInfoAt(i);
        AutoPtr<IMediaCodecInfo> info;
        mclHelper->GetCodecInfoAt(i, (IMediaCodecInfo**)&info);

        Boolean isEncoder;
        info->IsEncoder(&isEncoder);
        int direction = isEncoder ? MEDIA_CODEC_ENCODER : MEDIA_CODEC_DECODER;
        String codecString;
        info->GetName(&codecString);
        //String[] supportedTypes = info.getSupportedTypes();
        AutoPtr<ArrayOf<String> > supportedTypes;
        info->GetSupportedTypes((ArrayOf<String>**)&supportedTypes);
        for (Int32 j = 0; j < supportedTypes->GetLength(); ++j) {
            //Map<String, CodecInfo> map = info.isEncoder() ? encoderInfoMap : decoderInfoMap;
            AutoPtr<IMap> map = isEncoder? encoderInfoMap : decoderInfoMap;
            Boolean haveKey;
            AutoPtr<ICharSequence> typeString;
            CString::New((*supportedTypes)[j], (ICharSequence**)&typeString);
            map->ContainsKey(typeString, &haveKey);
            if (!haveKey) {
                AutoPtr<MediaCodecBridge::CodecInfo> codecInfo(
                        new MediaCodecBridge::CodecInfo((*supportedTypes)[j], codecString, direction));
                AutoPtr<IInterface> iCodecInfo = TO_IINTERFACE(codecInfo);
                map->Put(typeString, iCodecInfo);
            }
        }
    }
    //ArrayList<CodecInfo> codecInfos = new ArrayList<CodecInfo>(decoderInfoMap.size() + encoderInfoMap.size());
    Int32 decoderInfoMapSize, encoderInfoMapSize;
    decoderInfoMap->GetSize(&decoderInfoMapSize);
    encoderInfoMap->GetSize(&encoderInfoMapSize);
    AutoPtr<IArrayList> codecInfos;
    CArrayList::New(decoderInfoMapSize + encoderInfoMapSize, (IArrayList**)&codecInfos);
    AutoPtr<ICollection> encoderValues, decoderValues;
    //codecInfos.addAll(encoderInfoMap.values());
    //codecInfos.addAll(decoderInfoMap.values());
    encoderInfoMap->GetValues((ICollection**)&encoderValues);
    decoderInfoMap->GetValues((ICollection**)&decoderValues);
    Boolean encoderAddAll, decoderAddAll;
    codecInfos->AddAll(encoderValues, &encoderAddAll);
    codecInfos->AddAll(decoderValues, &decoderAddAll);
    //return codecInfos.toArray(new CodecInfo[codecInfos.size()]);
    AutoPtr<ArrayOf<IInterface*> > resCodecInfos;
    codecInfos->ToArray((ArrayOf<IInterface*>**)&resCodecInfos);
    return resCodecInfos;
}

String MediaCodecBridge::GetDecoderNameForMime(
    /* [in] */ const String& mime)
{
    AutoPtr<IMediaCodecListHelper> mediaCodecListHelper;
    //TODO CMediaCodecListHelper::AcquireSingleton((IMediaCodecList**)&mediaCodecListHelper);
    Int32 count;
    mediaCodecListHelper->GetCodecCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IMediaCodecInfo> info;
        mediaCodecListHelper->GetCodecInfoAt(i, (IMediaCodecInfo**)&info);
        Boolean isEncoder = FALSE;
        info->IsEncoder(&isEncoder);
        if (isEncoder){
            continue;
        }

        AutoPtr<ArrayOf<String> > supportedTypes;
        info->GetSupportedTypes((ArrayOf<String>**)&supportedTypes);
        Int32 length = supportedTypes->GetLength();
        for (Int32 j = 0; j < length; ++j) {
            if ((*supportedTypes)[j].EqualsIgnoreCase(mime)) {
                String name;
                info->GetName(&name);
                return name;
            }
        }
    }

    return String(NULL);
}

//@CalledByNative
AutoPtr<IInterface> MediaCodecBridge::Create(
    /* [in] */ const String& mime,
    /* [in] */ Boolean isSecure,
    /* [in] */ Int32 direction)
{
    // Creation of ".secure" codecs sometimes crash instead of throwing exceptions
    // on pre-JBMR2 devices.
    if (isSecure && Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN_MR2) {
        return NULL;
    }

    AutoPtr<IMediaCodec> mediaCodec;
    Boolean adaptivePlaybackSupported = FALSE;
    AutoPtr<IMediaCodecHelper> mediaCodecHelper;
    //TODO CMediaCodecHelper::AcquireSingleton((IMediaCodecHelper**)&mediaCodecHelper)
    // try {
        // |isSecure| only applies to video decoders.
        if (mime.StartWith("video") && isSecure && direction == MEDIA_CODEC_DECODER) {
            String decoderName = GetDecoderNameForMime(mime);
            if (decoderName.IsNullOrEmpty()) {
                return NULL;
            }

            if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::KITKAT) {
                // To work around an issue that we cannot get the codec info from the secure
                // decoder, create an insecure decoder first so that we can query its codec
                // info. http://b/15587335.
                AutoPtr<IMediaCodec> insecureCodec;
                mediaCodecHelper->CreateByCodecName(decoderName, (IMediaCodec**)&insecureCodec);
                adaptivePlaybackSupported = CodecSupportsAdaptivePlayback(insecureCodec, mime);
                insecureCodec->ReleaseResources();
            }
            String _decoderName(decoderName);
            _decoderName += ".secure";
            mediaCodecHelper->CreateByCodecName(_decoderName, (IMediaCodec**)&mediaCodec);
        }
        else {
            if (direction == MEDIA_CODEC_ENCODER) {
                mediaCodecHelper->CreateEncoderByType(mime, (IMediaCodec**)&mediaCodec);
            }
            else {
                mediaCodecHelper->CreateDecoderByType(mime, (IMediaCodec**)&mediaCodec);
                adaptivePlaybackSupported = CodecSupportsAdaptivePlayback(mediaCodec, mime);
            }
        }
    // } catch (Exception e) {
    //     Log.e(TAG, "Failed to create MediaCodec: " +  mime + ", isSecure: "
    //             + isSecure + ", direction: " + direction, e);
    // }

    if (mediaCodec == NULL) {
        return NULL;
    }

    AutoPtr<MediaCodecBridge> mcb = new MediaCodecBridge(mediaCodec, mime, adaptivePlaybackSupported);
    AutoPtr<IInterface> ret = TO_IINTERFACE(mcb);
    return ret;
}

//@CalledByNative
void MediaCodecBridge::ReleaseResources()
{
    // try {
        mMediaCodec->ReleaseResources();
    // } catch (IllegalStateException e) {
    //     // The MediaCodec is stuck in a wrong state, possibly due to losing
    //     // the surface.
    //     Log.e(TAG, "Cannot release media codec", e);
    // }

    mMediaCodec = NULL;
    if (mAudioTrack != NULL) {
        mAudioTrack->ReleaseResources();
    }
}

//@CalledByNative
Boolean MediaCodecBridge::Start()
{
    // try {
        mMediaCodec->Start();
        ECode ecode = mMediaCodec->GetInputBuffers((ArrayOf<IByteBuffer*>**)&mInputBuffers);
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Cannot start the media codec", e);
    //     return false;
    // }
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Cannot start the media codec, ecode:0x%x", ecode);
            return FALSE;
        }

    return TRUE;
}

//@CalledByNative
AutoPtr<MediaCodecBridge::DequeueInputResult> MediaCodecBridge::DequeueInputBuffer(
    /* [in] */ Int64 timeoutUs)
{
    Int32 status = MEDIA_CODEC_ERROR;
    Int32 index = -1;
    // try {
        Int32 indexOrStatus;
        ECode ecode = mMediaCodec->DequeueInputBuffer(timeoutUs, &indexOrStatus);
        if (FAILED(ecode))
        {
            Logger::E(TAG, "Failed to dequeue input buffer, ecode:0x%x", ecode);
        }
        if (indexOrStatus >= 0) { // index!
            status = MEDIA_CODEC_OK;
            index = indexOrStatus;
        }
        else if (indexOrStatus == IMediaCodec::INFO_TRY_AGAIN_LATER) {
            Logger::E(TAG, "dequeueInputBuffer: MediaCodec.INFO_TRY_AGAIN_LATER");
            status = MEDIA_CODEC_DEQUEUE_INPUT_AGAIN_LATER;
        }
        else {
            Logger::E(TAG, "Unexpected index_or_status: %d", indexOrStatus);
            //assert(FALSE);
        }
    // } catch (Exception e) {
    //     Log.e(TAG, "Failed to dequeue input buffer", e);
    // }

    AutoPtr<DequeueInputResult> ret = new DequeueInputResult(status, index);
    return ret;
}

//@CalledByNative
Int32 MediaCodecBridge::Flush()
{
    // try {
        mFlushed = TRUE;
        if (mAudioTrack != NULL) {
            // Need to call pause() here, or otherwise flush() is a no-op.
            mAudioTrack->Pause();
            mAudioTrack->Flush();
        }
        mMediaCodec->Flush();
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Failed to flush MediaCodec", e);
    //     return MEDIA_CODEC_ERROR;
    // }

    return MEDIA_CODEC_OK;
}

//@CalledByNative
void MediaCodecBridge::Stop()
{
    mMediaCodec->Stop();
    if (mAudioTrack != NULL) {
        mAudioTrack->Pause();
    }
}

//@CalledByNative
Int32 MediaCodecBridge::GetOutputHeight()
{
    AutoPtr<IMediaFormat> format;
    mMediaCodec->GetOutputFormat((IMediaFormat**)&format);
    Int32 height;
    format->GetInt32(IMediaFormat::KEY_HEIGHT, &height);
    return height;
}

//@CalledByNative
Int32 MediaCodecBridge::GetOutputWidth()
{
    AutoPtr<IMediaFormat> format;
    mMediaCodec->GetOutputFormat((IMediaFormat**)&format);
    Int32 width;
    format->GetInt32(IMediaFormat::KEY_WIDTH, &width);
    return width;
}

//@CalledByNative
AutoPtr<IByteBuffer> MediaCodecBridge::GetInputBuffer(
    /* [in] */ Int32 index)
{
    return (*mInputBuffers)[index];
}

//@CalledByNative
AutoPtr<IByteBuffer> MediaCodecBridge::GetOutputBuffer(
    /* [in] */ Int32 index)
{
    return (*mOutputBuffers)[index];
}

//@CalledByNative
Int32 MediaCodecBridge::GetInputBuffersCount()
{
    return mInputBuffers->GetLength();
}

//@CalledByNative
Int32 MediaCodecBridge::GetOutputBuffersCount()
{
    return mOutputBuffers != NULL ? mOutputBuffers->GetLength() : -1;
}

//@CalledByNative
Int32 MediaCodecBridge::GetOutputBuffersCapacity()
{
    if (mOutputBuffers == NULL)
    {
        return -1;
    }
    else
    {
        Int32 capacity;
        AutoPtr<IBuffer> buf = IBuffer::Probe((*mOutputBuffers)[0]);
        buf->GetCapacity(&capacity);
        return capacity;
    }
    //return mOutputBuffers != NULL ? ((*mOutputBuffers)[0]->GetCapacity(&capacity), capacity) : -1;
}

//@CalledByNative
Boolean MediaCodecBridge::GetOutputBuffers()
{
    // try {
        ECode ecode = mMediaCodec->GetOutputBuffers((ArrayOf<IByteBuffer*>**)&mOutputBuffers);
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Cannot get output buffers", e);
    //     return false;
    // }

    if(FAILED(ecode))
    {
        Logger::E(TAG, "Cannot get output buffers, ecode:0x%x", ecode);
        return FALSE;
    }
    return TRUE;
}

//@CalledByNative
Int32 MediaCodecBridge::QueueInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    ResetLastPresentationTimeIfNeeded(presentationTimeUs);
    // try {
    ECode ecode = mMediaCodec->QueueInputBuffer(index, offset, size, presentationTimeUs, flags);
    // } catch (Exception e) {
    //     Log.e(TAG, "Failed to queue input buffer", e);
    //     return MEDIA_CODEC_ERROR;
    // }
    if(FAILED(ecode))
    {
        Logger::E(TAG, "Failed to queue input buffer, ecode:0x%x", ecode);
        return FALSE;
    }

    return MEDIA_CODEC_OK;
}

//@CalledByNative
void MediaCodecBridge::SetVideoBitrate(
    /* [in] */ Int32 bps)
{
    AutoPtr<IBundle> b;
    CBundle::New((IBundle**)&b);
    b->PutInt32(IMediaCodec::PARAMETER_KEY_VIDEO_BITRATE, bps);
    mMediaCodec->SetParameters(b);
}

//@CalledByNative
void MediaCodecBridge::RequestKeyFrameSoon()
{
    AutoPtr<IBundle> b;
    CBundle::New((IBundle**)&b);
    b->PutInt32(IMediaCodec::PARAMETER_KEY_REQUEST_SYNC_FRAME, 0);
    mMediaCodec->SetParameters(b);
}

//@CalledByNative
Int32 MediaCodecBridge::QueueSecureInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* iv,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Int32>* numBytesOfClearData,
    /* [in] */ ArrayOf<Int32>* numBytesOfEncryptedData,
    /* [in] */ Int32 numSubSamples,
    /* [in] */ Int64 presentationTimeUs)
{
    ResetLastPresentationTimeIfNeeded(presentationTimeUs);
    // try {
        AutoPtr<IMediaCodecCryptoInfo> cryptoInfo;
        CMediaCodecCryptoInfo::New((IMediaCodecCryptoInfo**)&cryptoInfo);
        cryptoInfo->Set(numSubSamples, numBytesOfClearData, numBytesOfEncryptedData,
                keyId, iv, IMediaCodec::CRYPTO_MODE_AES_CTR);
        UInt32 ecode = mMediaCodec->QueueSecureInputBuffer(index, offset, cryptoInfo, presentationTimeUs, 0);
        if(ecode == (ECode)E_CRYPTO_EXCEPTION)
        {
            Logger::E(TAG, "MediaCodec.CryptoException with error code ,ecode:0x%x", ecode);
            return MEDIA_CODEC_ERROR;
        }
        else
        {
            Logger::E(TAG, "Failed to queue secure input buffer, ecode:0x%x", ecode);
            return MEDIA_CODEC_ERROR;
        }
    // } catch (MediaCodec.CryptoException e) {
    //     Log.e(TAG, "Failed to queue secure input buffer", e);
    //     if (e.getErrorCode() == MediaCodec.CryptoException.ERROR_NO_KEY) {
    //         Log.e(TAG, "MediaCodec.CryptoException.ERROR_NO_KEY");
    //         return MEDIA_CODEC_NO_KEY;
    //     }
    //     Log.e(TAG, "MediaCodec.CryptoException with error code " + e.getErrorCode());
    //     return MEDIA_CODEC_ERROR;
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Failed to queue secure input buffer", e);
    //     return MEDIA_CODEC_ERROR;
    // }
    return MEDIA_CODEC_OK;
}

//@CalledByNative
void MediaCodecBridge::ReleaseOutputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Boolean render)
{
    // try {
        ECode ecode = mMediaCodec->ReleaseOutputBuffer(index, render);
    // } catch (IllegalStateException e) {
    //     // TODO(qinmin): May need to report the error to the caller. crbug.com/356498.
    //     Log.e(TAG, "Failed to release output buffer", e);
    // }
        if(FAILED(ecode))
        {
            Logger::E(TAG, "Failed to release output buffer,ecode:0x%x", ecode);
        }
}

//@CalledByNative
AutoPtr<MediaCodecBridge::DequeueOutputResult> MediaCodecBridge::DequeueOutputBuffer(
    /* [in] */ Int64 timeoutUs)
{
    AutoPtr<IMediaCodecBufferInfo> info;
    CMediaCodecBufferInfo::New((IMediaCodecBufferInfo**)&info);
    Int32 status = MEDIA_CODEC_ERROR;
    Int32 index = -1;
    // try {
        Int32 indexOrStatus;
        mMediaCodec->DequeueOutputBuffer(info, timeoutUs, &indexOrStatus);
        Int64 timeUs;
        info->GetPresentationTimeUs(&timeUs);
        if (timeUs < mLastPresentationTimeUs) {
            // TODO(qinmin): return a special code through DequeueOutputResult
            // to notify the native code the the frame has a wrong presentation
            // timestamp and should be skipped.
            info->SetPresentationTimeUs(mLastPresentationTimeUs);
        }

        info->GetPresentationTimeUs(&mLastPresentationTimeUs);

        if (indexOrStatus >= 0) { // index!
            status = MEDIA_CODEC_OK;
            index = indexOrStatus;
        }
        else if (indexOrStatus == IMediaCodec::INFO_OUTPUT_BUFFERS_CHANGED) {
            status = MEDIA_CODEC_OUTPUT_BUFFERS_CHANGED;
        }
        else if (indexOrStatus == IMediaCodec::INFO_OUTPUT_FORMAT_CHANGED) {
            status = MEDIA_CODEC_OUTPUT_FORMAT_CHANGED;
        }
        else if (indexOrStatus == IMediaCodec::INFO_TRY_AGAIN_LATER) {
            status = MEDIA_CODEC_DEQUEUE_OUTPUT_AGAIN_LATER;
        }
        else {
            Logger::E(TAG, "Unexpected index_or_status: %d", indexOrStatus);
            assert(FALSE);
        }
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Failed to dequeue output buffer", e);
    // }

    Int32 flags, offset, size;
    info->GetFlags(&flags);
    info->GetOffset(&offset);
    info->GetSize(&size);
    Int64 presentationTimeUs;
    info->GetPresentationTimeUs(&presentationTimeUs);
    AutoPtr<DequeueOutputResult> ret = new DequeueOutputResult(
            status, index, flags, offset, presentationTimeUs, size);

    return ret;
}

//@CalledByNative
Boolean MediaCodecBridge::ConfigureVideo(
    /* [in] */ IMediaFormat* format,
    /* [in] */ ISurface* surface,
    /* [in] */ IMediaCrypto* crypto,
    /* [in] */ Int32 flags)
{
    // try {
        if (mAdaptivePlaybackSupported) {
            format->SetInt32(IMediaFormat::KEY_MAX_WIDTH, MAX_ADAPTIVE_PLAYBACK_WIDTH);
            format->SetInt32(IMediaFormat::KEY_MAX_HEIGHT, MAX_ADAPTIVE_PLAYBACK_HEIGHT);
        }

        ECode ecode = mMediaCodec->Configure(format, surface, crypto, flags);

        if (FAILED(ecode))
        {
            Logger::E(TAG, "Cannot configure the video codec,ecode:%d", ecode);
            return FALSE;
        }
        return TRUE;
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Cannot configure the video codec", e);
    // }

    //return FALSE;
}

//@CalledByNative
AutoPtr<IInterface> MediaCodecBridge::CreateAudioFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelCount)
{
    AutoPtr<IMediaFormatHelper> helper;
    CMediaFormatHelper::AcquireSingleton((IMediaFormatHelper**)&helper);
    AutoPtr<IMediaFormat> format;
    helper->CreateAudioFormat(mime, sampleRate, channelCount, (IMediaFormat**)&format);
    return format;
}

//@CalledByNative
AutoPtr<IInterface> MediaCodecBridge::CreateVideoDecoderFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IMediaFormatHelper> helper;
    CMediaFormatHelper::AcquireSingleton((IMediaFormatHelper**)&helper);
    AutoPtr<IMediaFormat> format;
    helper->CreateVideoFormat(mime, width, height, (IMediaFormat**)&format);
    return format;
}

//@CalledByNative
AutoPtr<IInterface> MediaCodecBridge::CreateVideoEncoderFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 bitRate,
    /* [in] */ Int32 frameRate,
    /* [in] */ Int32 iFrameInterval,
    /* [in] */ Int32 colorFormat)
{
    AutoPtr<IMediaFormatHelper> helper;
    CMediaFormatHelper::AcquireSingleton((IMediaFormatHelper**)&helper);
    AutoPtr<IMediaFormat> format;
    helper->CreateVideoFormat(mime, width, height, (IMediaFormat**)&format);
    format->SetInt32(IMediaFormat::KEY_BIT_RATE, bitRate);
    format->SetInt32(IMediaFormat::KEY_FRAME_RATE, frameRate);
    format->SetInt32(IMediaFormat::KEY_I_FRAME_INTERVAL, iFrameInterval);
    format->SetInt32(IMediaFormat::KEY_COLOR_FORMAT, colorFormat);
    return format;
}

//@CalledByNative
Boolean MediaCodecBridge::IsAdaptivePlaybackSupported(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (!mAdaptivePlaybackSupported)
        return FALSE;
    return width <= MAX_ADAPTIVE_PLAYBACK_WIDTH && height <= MAX_ADAPTIVE_PLAYBACK_HEIGHT;
}

Boolean MediaCodecBridge::CodecSupportsAdaptivePlayback(
    /* [in] */ IMediaCodec* mediaCodec,
    /* [in] */ const String& mime)
{
    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::KITKAT || mediaCodec == NULL) {
        return FALSE;
    }

    // try {
        AutoPtr<IMediaCodecInfo> info;
        mediaCodec->GetCodecInfo((IMediaCodecInfo**)&info);
        Boolean isEncoder;
        info->IsEncoder(&isEncoder);
        if (isEncoder) {
            return FALSE;
        }

        //TODO below 5 line depends on ICodecCapabilities
        //AutoPtr<ICodecCapabilities> capabilities;
        //info->GetCapabilitiesForType(mime, (ICodecCapabilities**)&capabilities);
        //Boolean isFeatureSupported;
        //return (capabilities != NULL) && (capabilities->IsFeatureSupported(
        //        ICodecCapabilities::FEATURE_AdaptivePlayback, &isFeatureSupported), isFeatureSupported);
        return FALSE;//TODO replace by the above 5 lines
    // } catch (IllegalArgumentException e) {
    //       Log.e(TAG, "Cannot retrieve codec information", e);
    // }

    return FALSE;
}

//@CalledByNative
void MediaCodecBridge::SetCodecSpecificData(
    /* [in] */ IMediaFormat* format,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    String name;
    if (index == 0) {
        name = "csd-0";
    }
    else if (index == 1) {
        name = "csd-1";
    }

    if (!name.IsNullOrEmpty()) {
        AutoPtr<IByteBuffer> buf;
        AutoPtr<IByteBufferHelper> bHelper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bHelper);
        bHelper->Wrap(bytes, (IByteBuffer**)&buf);
        format->SetByteBuffer(name, buf);
    }
}

//@CalledByNative
void MediaCodecBridge::SetFrameHasADTSHeader(
    /* [in] */ IMediaFormat* format)
{
    format->SetInt32(IMediaFormat::KEY_IS_ADTS, 1);
}

//@CalledByNative
Boolean MediaCodecBridge::ConfigureAudio(
    /* [in] */ IMediaFormat* format,
    /* [in] */ IMediaCrypto* crypto,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean playAudio)
{
    // try {
        mMediaCodec->Configure(format, NULL, crypto, flags);
        if (playAudio) {
            Int32 sampleRate;
            format->GetInt32(IMediaFormat::KEY_SAMPLE_RATE, &sampleRate);
            Int32 channelCount;
            format->GetInt32(IMediaFormat::KEY_CHANNEL_COUNT, &channelCount);
            Int32 channelConfig = GetAudioFormat(channelCount);
            // Using 16bit PCM for output. Keep this value in sync with
            // kBytesPerAudioOutputSample in media_codec_bridge.cc.
            AutoPtr<IAudioTrackHelper> atHelper;
            CAudioTrackHelper::AcquireSingleton((IAudioTrackHelper**)&atHelper);
            Int32 minBufferSize;
            atHelper->GetMinBufferSize(sampleRate, channelConfig,
                    IAudioFormat::ENCODING_PCM_16BIT, &minBufferSize);
            CAudioTrack::New(IAudioManager::STREAM_MUSIC, sampleRate, channelConfig,
                    IAudioFormat::ENCODING_PCM_16BIT, minBufferSize, IAudioTrack::MODE_STREAM,
                    (IAudioTrack**)&mAudioTrack);
            Int32 state;
            mAudioTrack->GetState(&state);
            if (state == IAudioTrack::STATE_UNINITIALIZED) {
                mAudioTrack = NULL;
                return FALSE;
            }
        }

        return TRUE;
    // } catch (IllegalStateException e) {
    //     Log.e(TAG, "Cannot configure the audio codec", e);
    // }

    return FALSE;
}

/**
 *  Play the audio buffer that is passed in.
 *
 *  @param buf Audio buffer to be rendered.
 *  @return The number of frames that have already been consumed by the
 *  hardware. This number resets to 0 after each flush call.
 */
//@CalledByNative
Int64 MediaCodecBridge::PlayOutputBuffer(
    /* [in] */ ArrayOf<Byte>* buf)
{
    if (mAudioTrack == NULL) {
        return 0;
    }

    Int32 state;
    mAudioTrack->GetPlayState(&state);
    if (IAudioTrack::PLAYSTATE_PLAYING != state) {
        mAudioTrack->Play();
    }

    Int32 size;
    mAudioTrack->Write(buf, 0, buf->GetLength(), &size);
    if (buf->GetLength() != size) {
        Logger::I(TAG, "Failed to send all data to audio output, expected size: %d, actual size is:%d",
                buf->GetLength(), size);
    }
    // TODO(qinmin): Returning the head position allows us to estimate
    // the current presentation time in native code. However, it is
    // better to use AudioTrack.getCurrentTimestamp() to get the last
    // known time when a frame is played. However, we will need to
    // convert the java nano time to C++ timestamp.
    // If the stream runs too long, getPlaybackHeadPosition() could
    // overflow. AudioTimestampHelper in MediaSourcePlayer has the same
    // issue. See http://crbug.com/358801.
    Int32 position;
    mAudioTrack->GetPlaybackHeadPosition(&position);
    return position;
}

//@CalledByNative
void MediaCodecBridge::SetVolume(
    /* [in] */ Double volume)
{
    if (mAudioTrack != NULL) {
        Int32 res;
        mAudioTrack->SetStereoVolume((Float) volume, (Float) volume, &res);
    }
}

void MediaCodecBridge::ResetLastPresentationTimeIfNeeded(
    /* [in] */ Int64 presentationTimeUs)
{
    if (mFlushed) {
        mLastPresentationTimeUs =
                Elastos::Core::Math::Max(presentationTimeUs - MAX_PRESENTATION_TIMESTAMP_SHIFT_US, (Int64)0);
        mFlushed = FALSE;
    }
}

Int32 MediaCodecBridge::GetAudioFormat(
    /* [in] */ Int32 channelCount)
{
    switch (channelCount) {
        case 1:
            return IAudioFormat::CHANNEL_OUT_MONO;
        case 2:
            return IAudioFormat::CHANNEL_OUT_STEREO;
        case 4:
            return IAudioFormat::CHANNEL_OUT_QUAD;
        case 6:
            return IAudioFormat::CHANNEL_OUT_5POINT1;
        case 8:
            return IAudioFormat::CHANNEL_OUT_7POINT1;
        default:
            return IAudioFormat::CHANNEL_OUT_DEFAULT;
    }
}

void MediaCodecBridge::ReleaseResource(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::ReleaseResources, mObj is NULL");
        return;
    }
    mObj->ReleaseResources();
}

Boolean MediaCodecBridge::Start(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::Start, mObj is NULL");
        return FALSE;
    }
    return mObj->Start();
}

AutoPtr<IInterface> MediaCodecBridge::DequeueInputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 timeoutUs)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueInputBuffer, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->DequeueInputBuffer(timeoutUs));
}

Int32 MediaCodecBridge::Flush(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::Flush, mObj is NULL");
        return 0;
    }
    return mObj->Flush();
}

void MediaCodecBridge::Stop(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

Int32 MediaCodecBridge::GetOutputHeight(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetOutputHeight();
}

Int32 MediaCodecBridge::GetOutputWidth(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetOutputWidth();
}

AutoPtr<IInterface> MediaCodecBridge::GetInputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetInputBuffer, mObj is NULL");
        return NULL;
    }
    return mObj->GetInputBuffer(index);
}

AutoPtr<IInterface> MediaCodecBridge::GetOutputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputBuffer, mObj is NULL");
        return NULL;
    }
    return mObj->GetOutputBuffer(index);
}

Int32 MediaCodecBridge::GetInputBuffersCount(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetInputBuffersCount, mObj is NULL");
        return 0;
    }
    return mObj->GetInputBuffersCount();
}

Int32 MediaCodecBridge::GetOutputBuffersCount(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputBuffersCount, mObj is NULL");
        return 0;
    }
    return mObj->GetOutputBuffersCount();
}

Int32 MediaCodecBridge::GetOutputBuffersCapacity(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputBuffersCapacity, mObj is NULL");
        return 0;
    }
    return mObj->GetOutputBuffersCapacity();
}

Boolean MediaCodecBridge::GetOutputBuffers(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::GetOutputBuffers, mObj is NULL");
        return FALSE;
    }
    return mObj->GetOutputBuffers();
}

Int32 MediaCodecBridge::QueueInputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::QueueInputBuffer, mObj is NULL");
        return 0;
    }
    return mObj->QueueInputBuffer(index, offset, size, presentationTimeUs, flags);
}

void MediaCodecBridge::SetVideoBitrate(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 bps)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::SetVideoBitrate, mObj is NULL");
        return;
    }
    mObj->SetVideoBitrate(bps);
}

void MediaCodecBridge::RequestKeyFrameSoon(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::RequestKeyFrameSoon, mObj is NULL");
        return;
    }
    mObj->RequestKeyFrameSoon();
}

Int32 MediaCodecBridge::QueueSecureInputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* iv,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Int32>* numBytesOfClearData,
    /* [in] */ ArrayOf<Int32>* numBytesOfEncryptedData,
    /* [in] */ Int32 numSubSamples,
    /* [in] */ Int64 presentationTimeUs)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::QueueSecureInputBuffer, mObj is NULL");
        return 0;
    }
    return mObj->QueueSecureInputBuffer(index, offset, iv, keyId, numBytesOfClearData, numBytesOfEncryptedData, numSubSamples, presentationTimeUs);
}

void MediaCodecBridge::ReleaseOutputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 index,
    /* [in] */ Boolean render)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::ReleaseOutputBuffer, mObj is NULL");
        return;
    }
    mObj->ReleaseOutputBuffer(index, render);
}

AutoPtr<IInterface> MediaCodecBridge::DequeueOutputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 timeoutUs)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::DequeueOutputBuffer, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->DequeueOutputBuffer(timeoutUs));
}

Boolean MediaCodecBridge::ConfigureVideo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* format,
    /* [in] */ IInterface* surface,
    /* [in] */ IInterface* crypto,
    /* [in] */ Int32 flags)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::ConfigureVideo, mObj is NULL");
        return FALSE;
    }
    AutoPtr<IMediaFormat> m = IMediaFormat::Probe(format);
    AutoPtr<ISurface> s = ISurface::Probe(surface);
    AutoPtr<IMediaCrypto> c = IMediaCrypto::Probe(crypto);
    return mObj->ConfigureVideo(m, s, c, flags);
}

Boolean MediaCodecBridge::IsAdaptivePlaybackSupported(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::IsAdaptivePlaybackSupported, mObj is NULL");
        return FALSE;
    }
    return mObj->IsAdaptivePlaybackSupported(width, height);
}

void MediaCodecBridge::SetCodecSpecificData(
    /* [in] */ IInterface* format,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoPtr<IMediaFormat> m = IMediaFormat::Probe(format);
    SetCodecSpecificData(m, index, bytes);
}

void MediaCodecBridge::SetFrameHasADTSHeader(
    /* [in] */ IInterface* format)
{
    AutoPtr<IMediaFormat> m = IMediaFormat::Probe(format);
    SetFrameHasADTSHeader(m);
}

Boolean MediaCodecBridge::ConfigureAudio(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* format,
    /* [in] */ IInterface* crypto,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean playAudio)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::ConfigureAudio, mObj is NULL");
        return FALSE;
    }
    AutoPtr<IMediaFormat> m = IMediaFormat::Probe(format);
    AutoPtr<IMediaCrypto> c = IMediaCrypto::Probe(crypto);
    return mObj->ConfigureAudio(m, c, flags, playAudio);
}

Int64 MediaCodecBridge::PlayOutputBuffer(
    /* [in] */ IInterface* obj,
    /* [in] */ ArrayOf<Byte>* buf)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::PlayOutputBuffer, mObj is NULL");
        return 0;
    }
    return mObj->PlayOutputBuffer(buf);
}

void MediaCodecBridge::SetVolume(
    /* [in] */ IInterface* obj,
    /* [in] */ Double volume)
{
    AutoPtr<MediaCodecBridge> mObj = (MediaCodecBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "MediaCodecBridge::SetVolume, mObj is NULL");
        return;
    }
    mObj->SetVolume(volume);
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
