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
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/media/WebAudioMediaCodecBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/media/api/WebAudioMediaCodecBridge_dec.h"
//TODO #include "elastos/droid/media/CMediaExtractor.h"
//TODO #include "elastos/droid/media/CMediaCodecHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaCodec;
using Elastos::Droid::Media::IMediaCodecBufferInfo;
using Elastos::Droid::Media::CMediaCodecBufferInfo;
using Elastos::Droid::Media::IMediaFormat;
using Elastos::Droid::Media::IMediaExtractor;
//TODO using Elastos::Droid::Media::CMediaExtractor;
using Elastos::Droid::Media::IMediaCodecHelper;
//TODO using Elastos::Droid::Media::CMediaCodecHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

const String WebAudioMediaCodecBridge::TAG("WebAudioMediaCodec");

// TODO(rtoy): What is the correct timeout value for reading
// from a file in memory?
const Int64 WebAudioMediaCodecBridge::TIMEOUT_MICROSECONDS;

//@CalledByNative
String WebAudioMediaCodecBridge::CreateTempFile(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IFile> outputDirectory;
    ctx->GetCacheDir((IFile**)&outputDirectory);
    AutoPtr<IFile> outputFile;
    AutoPtr<IFileHelper> fileHelper;
    CFileHelper::AcquireSingleton((IFileHelper**)&fileHelper);
    fileHelper->CreateTempFile(String("webaudio"), String(".dat"), outputDirectory,
        (IFile**)&outputFile);

    String str;
    outputFile->GetAbsolutePath(&str);

    return str;
}

//@CalledByNative
Boolean WebAudioMediaCodecBridge::DecodeAudioFile(
    /* [in] */ IContext* ctx,
    /* [in] */ HANDLE nativeMediaCodecBridge,
    /* [in] */ Int32 inputFD,
    /* [in] */ Int64 dataSize)
{

    if (dataSize < 0 || dataSize > 0x7fffffff)
        return FALSE;

    AutoPtr<IMediaExtractor> extractor;
    //TODO CMediaExtractor::New((IMediaExtractor**)&extractor);

    AutoPtr<IParcelFileDescriptorHelper> parcelFileDescriptorHelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelFileDescriptorHelper);
    AutoPtr<IParcelFileDescriptor> encodedFD;
    parcelFileDescriptorHelper->AdoptFd(inputFD, (IParcelFileDescriptor**)&encodedFD);
    // try {
        AutoPtr<IFileDescriptor> fileDescriptor;
        encodedFD->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
        extractor->SetDataSource(fileDescriptor, 0, dataSize);
    // } catch (Exception e) {
    //     e.printStackTrace();
    //     encodedFD.detachFd();
    //     return false;
    // }

    Int32 count;
    extractor->GetTrackCount(&count);
    if (count <= 0) {
        Int32 result;
        encodedFD->DetachFd(&result);
        return FALSE;
    }

    AutoPtr<IMediaFormat> format;
    extractor->GetTrackFormat(0, (IMediaFormat**)&format);

    // Number of channels specified in the file
    Int32 inputChannelCount;
    format->GetInt32(IMediaFormat::KEY_CHANNEL_COUNT, &inputChannelCount);

    // Number of channels the decoder will provide. (Not
    // necessarily the same as inputChannelCount.  See
    // crbug.com/266006.)
    Int32 outputChannelCount = inputChannelCount;

    Int32 sampleRate;
    format->GetInt32(IMediaFormat::KEY_SAMPLE_RATE, &sampleRate);
    String mime;
    format->GetString(IMediaFormat::KEY_MIME, &mime);

    Int64 durationMicroseconds = 0;
    Boolean containsKey;
    format->ContainsKey(IMediaFormat::KEY_DURATION, &containsKey);
    if (containsKey) {
        // try {
            ECode ecode = format->GetInt64(IMediaFormat::KEY_DURATION, &durationMicroseconds);
        // } catch (Exception e) {
        //     Log.d(TAG, "Cannot get duration");
        // }
            if (FAILED(ecode))
            {
                Logger::D(TAG, "Cannot get duration");
            }
    }

    // If the duration is too long, set to 0 to force the caller
    // not to preallocate space.  See crbug.com/326856.
    // FIXME: What should be the limit? We're arbitrarily using
    // about 2148 sec (35.8 min).
    if (durationMicroseconds > 0x7fffffff) {
        durationMicroseconds = 0;
    }

    String formatStr;
    format->ToString(&formatStr);
    Logger::D(TAG, "Initial: Tracks: %d, Format:%s", count, formatStr.string());

    // Create decoder
    AutoPtr<IMediaCodec> codec;
    // try {
        AutoPtr<IMediaCodecHelper> mediaCodecHelper;
        //TODO CMediaCodecHelper::AcquireSingleton((IMediaCodecHelper**)&mediaCodecHelper);
        ECode ecode = mediaCodecHelper->CreateDecoderByType(mime, (IMediaCodec**)&codec);
    // } catch (Exception e) {
        if (FAILED(ecode))
        {
            Logger::W(TAG, "Failed to create MediaCodec for mime type: %s", mime.string());
            Int32 dfdRes;
            encodedFD->DetachFd(&dfdRes);
            return FALSE;
        }
    // }


    codec->Configure(format, NULL /* surface */, NULL /* crypto */, 0 /* flags */);
    codec->Start();

    AutoPtr<ArrayOf<IByteBuffer*> > codecInputBuffers;
    codec->GetInputBuffers((ArrayOf<IByteBuffer*>**)&codecInputBuffers);
    AutoPtr<ArrayOf<IByteBuffer*> > codecOutputBuffers;
    codec->GetOutputBuffers((ArrayOf<IByteBuffer*>**)&codecOutputBuffers);

    // A track must be selected and will be used to read samples.
    extractor->SelectTrack(0);

    Boolean sawInputEOS = FALSE;
    Boolean sawOutputEOS = FALSE;
    Boolean destinationInitialized = FALSE;

    // Keep processing until the output is done.
    while (!sawOutputEOS) {
        if (!sawInputEOS) {
            // Input side
            Int32 inputBufIndex;
            codec->DequeueInputBuffer(TIMEOUT_MICROSECONDS, &inputBufIndex);

            if (inputBufIndex >= 0) {
                AutoPtr<IByteBuffer> dstBuf = (*codecInputBuffers)[inputBufIndex];
                Int32 sampleSize;
                extractor->ReadSampleData(dstBuf, 0, &sampleSize);
                Int64 presentationTimeMicroSec = 0;

                if (sampleSize < 0) {
                    sawInputEOS = TRUE;
                    sampleSize = 0;
                }
                else {
                    extractor->GetSampleTime(&presentationTimeMicroSec);
                }

                codec->QueueInputBuffer(inputBufIndex,
                                       0, /* offset */
                                       sampleSize,
                                       presentationTimeMicroSec,
                                       sawInputEOS ? IMediaCodec::BUFFER_FLAG_END_OF_STREAM : 0);

                if (!sawInputEOS) {
                    Boolean advRes;
                    extractor->Advance(&advRes);
                }
            }
        }

        // Output side
        AutoPtr<IMediaCodecBufferInfo> info;
        CMediaCodecBufferInfo::New((IMediaCodecBufferInfo**)&info);
        Int32 outputBufIndex;
        codec->DequeueOutputBuffer(info, TIMEOUT_MICROSECONDS, &outputBufIndex);

        if (outputBufIndex >= 0) {
            AutoPtr<IByteBuffer> buf = (*codecOutputBuffers)[outputBufIndex];

            if (!destinationInitialized) {
                // Initialize the destination as late as possible to
                // catch any changes in format. But be sure to
                // initialize it BEFORE we send any decoded audio,
                // and only initialize once.
                Logger::D(TAG, "Final:  Rate: %d, Channels: %d, Mime: %s, Duration: %d microsec",
                        sampleRate, inputChannelCount, mime.string(), durationMicroseconds);

                NativeInitializeDestination(nativeMediaCodecBridge,
                                            inputChannelCount,
                                            sampleRate,
                                            durationMicroseconds);
                destinationInitialized = TRUE;
            }
            Int32 infoSize;
            info->GetSize(&infoSize);
            if (destinationInitialized && infoSize > 0) {
                NativeOnChunkDecoded(nativeMediaCodecBridge, buf, infoSize,
                                     inputChannelCount, outputChannelCount);
            }

            AutoPtr<IBuffer> ibuf = IBuffer::Probe(buf);
            ibuf->Clear();
            codec->ReleaseOutputBuffer(outputBufIndex, FALSE /* render */);

            Int32 infoFlags;
            info->GetFlags(&infoFlags);
            if ((infoFlags & IMediaCodec::BUFFER_FLAG_END_OF_STREAM) != 0) {
                sawOutputEOS = TRUE;
            }
        }
        else if (outputBufIndex == IMediaCodec::INFO_OUTPUT_BUFFERS_CHANGED) {
            codec->GetOutputBuffers((ArrayOf<IByteBuffer*>**)&codecOutputBuffers);
        }
        else if (outputBufIndex == IMediaCodec::INFO_OUTPUT_FORMAT_CHANGED) {
            AutoPtr<IMediaFormat> newFormat;
            codec->GetOutputFormat((IMediaFormat**)&newFormat);
            newFormat->GetInt32(IMediaFormat::KEY_CHANNEL_COUNT, &outputChannelCount);
            newFormat->GetInt32(IMediaFormat::KEY_SAMPLE_RATE, &sampleRate);
            String newFormatStr;
            newFormat->ToString(&newFormatStr);
            Logger::D(TAG, "output format changed to %s", newFormatStr.string());
        }
    }

    Int32 dfdRes;
    encodedFD->DetachFd(&dfdRes);

    codec->Stop();
    codec->ReleaseResources();
    codec = NULL;

    return TRUE;
}

void WebAudioMediaCodecBridge::NativeOnChunkDecoded(
    /* [in] */ HANDLE nativeWebAudioMediaCodecBridge,
    /* [in] */ IByteBuffer* buf,
    /* [in] */ Int32 size,
    /* [in] */ Int32 inputChannelCount,
    /* [in] */ Int32 outputChannelCount)
{
    Elastos_WebAudioMediaCodecBridge_nativeOnChunkDecoded(nativeWebAudioMediaCodecBridge, TO_IINTERFACE(buf), size, inputChannelCount, outputChannelCount);
}

void WebAudioMediaCodecBridge::NativeInitializeDestination(
    /* [in] */ HANDLE nativeWebAudioMediaCodecBridge,
    /* [in] */ Int32 inputChannelCount,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int64 durationMicroseconds)
{
    Elastos_WebAudioMediaCodecBridge_nativeInitializeDestination(nativeWebAudioMediaCodecBridge, inputChannelCount, sampleRate, durationMicroseconds);
}
//callback function definition
String WebAudioMediaCodecBridge::CreateTempFile(
    /* [in] */ IInterface* ctx)
{
    AutoPtr<IContext> c = IContext::Probe(ctx);
    return WebAudioMediaCodecBridge::CreateTempFile(c);
}

Boolean WebAudioMediaCodecBridge::DecodeAudioFile(
    /* [in] */ IInterface* ctx,
    /* [in] */ HANDLE nativeMediaCodecBridge,
    /* [in] */ Int32 inputFD,
    /* [in] */ Int64 dataSize)
{
    AutoPtr<IContext> c = IContext::Probe(ctx);
    return WebAudioMediaCodecBridge::DecodeAudioFile(c, nativeMediaCodecBridge, inputFD, dataSize);
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
