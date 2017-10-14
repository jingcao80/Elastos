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

#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/hardware/CHardwareCamera.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/media/CMediaRecorder.h"
#include "elastos/droid/media/CCamcorderProfile.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <camera/Camera.h>
#include <camera/ICameraService.h>
#include <gui/Surface.h>
#include <hardware/camera.h>
#include <media/mediarecorder.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::CHardwareCamera;
using Elastos::Droid::Hardware::HardwareCamera;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::CSurface;
using Elastos::IO::CFile;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaRecorder::TAG("CMediaRecorder");

static Object sLock;

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaRecorderListener: public android::MediaRecorderListener
{
public:
    JNIMediaRecorderListener(IWeakReference* weak_thiz);
    ~JNIMediaRecorderListener();
    void notify(int msg, int ext1, int ext2);

private:
    JNIMediaRecorderListener();
    AutoPtr<IWeakReference> mWeakObject;    // Weak ref to MediaRecorder Java object to call on
};

JNIMediaRecorderListener::JNIMediaRecorderListener(IWeakReference* weak_thiz)
{
    // We use a weak reference so the MediaRecorder object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mWeakObject = weak_thiz;
}

JNIMediaRecorderListener::~JNIMediaRecorderListener()
{}

void JNIMediaRecorderListener::notify(int msg, int ext1, int ext2)
{
    CMediaRecorder::PostEventFromNative(mWeakObject, msg, ext1, ext2, NULL);
}

// Returns true if it throws an exception.
static ECode process_media_recorder_call(
    /* [in] */ android::status_t opStatus,
    /* [in] */ ECode exception,
    /* [in] */ const char* msg = NULL)
{
    //ALOGV("process_media_recorder_call");
    if (opStatus == (android::status_t)android::INVALID_OPERATION) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (opStatus != android::OK) {
        if (msg != NULL) Logger::E(CMediaRecorder::TAG, msg);
        return exception;
    }
    return NOERROR;
}

static android::sp<android::MediaRecorder> getMediaRecorder(CMediaRecorder* thiz)
{
    AutoLock l(sLock);
    android::MediaRecorder* const p = (android::MediaRecorder*)thiz->mNativeContext;
    return android::sp<android::MediaRecorder>(p);
}

static android::sp<android::MediaRecorder> setMediaRecorder(
    CMediaRecorder* thiz, const android::sp<android::MediaRecorder>& recorder)
{
    AutoLock l(sLock);
    android::sp<android::MediaRecorder> old = (android::MediaRecorder*)thiz->mNativeContext;
    if (recorder.get()) {
        recorder->incStrong(thiz);
    }
    if (old != 0) {
        old->decStrong(thiz);
    }
    thiz->mNativeContext = (Int64)recorder.get();
    return old;
}

static android::sp<android::Surface> get_surface(ISurface* clazz)
{
    ALOGV("get_surface");

    AutoPtr<CSurface> surface = (CSurface*)clazz;
    android::sp<android::Surface> sur;
    sur = reinterpret_cast<android::Surface *>(
            surface->mNativeObject);
    return sur;
}

//================================================================================
//              CMediaRecorder::EventHandler
//================================================================================

const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_EVENT_LIST_START = 1;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_EVENT_ERROR      = 1;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_EVENT_INFO       = 2;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_EVENT_LIST_END   = 99;

const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_TRACK_EVENT_LIST_START = 100;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_TRACK_EVENT_ERROR      = 100;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_TRACK_EVENT_INFO       = 101;
const Int32 CMediaRecorder::EventHandler::MEDIA_RECORDER_TRACK_EVENT_LIST_END   = 1000;

CMediaRecorder::EventHandler::EventHandler(
    /* [in] */ CMediaRecorder* host,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mMediaRecorder(host)
{
}

ECode CMediaRecorder::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    if (mMediaRecorder->mNativeContext == 0) {
        Logger::W(CMediaRecorder::TAG, "mediarecorder went away with unhandled events");
        return NOERROR;
    }

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case MEDIA_RECORDER_EVENT_ERROR:
        case MEDIA_RECORDER_TRACK_EVENT_ERROR:
            if (mMediaRecorder->mOnErrorListener != NULL) {
                return mMediaRecorder->mOnErrorListener->OnError(mMediaRecorder, arg1, arg2);
            }
            return NOERROR;

        case MEDIA_RECORDER_EVENT_INFO:
        case MEDIA_RECORDER_TRACK_EVENT_INFO:
            if (mMediaRecorder->mOnInfoListener != NULL) {
                return mMediaRecorder->mOnInfoListener->OnInfo(mMediaRecorder, arg1, arg2);
            }
            return NOERROR;

        default:
            Logger::E(CMediaRecorder::TAG, "Unknown message type %d", what);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//================================================================================
//              CMediaRecorder
//================================================================================

CAR_INTERFACE_IMPL(CMediaRecorder, Object, IMediaRecorder)

CAR_OBJECT_IMPL(CMediaRecorder)

CMediaRecorder::CMediaRecorder()
    : mNativeContext(0)
{}

CMediaRecorder::~CMediaRecorder()
{
    NativeFinalize();
}

ECode CMediaRecorder::constructor()
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    if (looper != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else if ((looper = Looper::GetMainLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else {
        mEventHandler = NULL;
    }

     String packageName = Elastos::Droid::App::CActivityThread::GetCurrentPackageName();

    /* Native setup requires a weak reference to our object.
     * It's easier to create it here than in C++.
     */
    return NativeSetup(packageName);
}

ECode CMediaRecorder::SetCamera(
    /* [in] */ IHardwareCamera* camera)
{
    // we should not pass a NULL camera to get_native_camera() call.
    if (camera == NULL) {
        Logger::E(TAG, "camera object is a NULL pointer");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::sp<android::Camera> c = CHardwareCamera::get_native_camera((HardwareCamera*)camera, NULL);
    if (c == NULL) {
        // get_native_camera will throw an exception in this case
        return NOERROR;
    }
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    return process_media_recorder_call(
        mr->setCamera(c->remote(), c->getRecordingProxy()),
        E_RUNTIME_EXCEPTION, "setCamera failed.");
}

ECode CMediaRecorder::GetSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    ALOGV("getSurface");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    android::sp<android::IGraphicBufferProducer> bufferProducer = mr->querySurfaceMediaSourceFromMediaServer();
    if (bufferProducer == NULL) {
        // jniThrowException(
        //         env,
        //         "java/lang/IllegalStateException",
        //         "failed to get surface");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Wrap the IGBP in a Java-language Surface.
    android::sp<android::Surface> surface(new android::Surface(bufferProducer, TRUE));
    if (surface == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ISurface> surfaceObj;
    CSurface::New((Int64)surface.get(), (ISurface**)&surfaceObj);
    if (surfaceObj == NULL) {
        *result = NULL;
        return NOERROR;
    }
    *result = surfaceObj;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaRecorder::SetPreviewDisplay(
    /* [in] */ ISurface* sv)
{
    mSurface = sv;
    return NOERROR;
}

ECode CMediaRecorder::SetAudioSource(
    /* [in] */ Int32 audio_source)
{
    //ALOGV("setAudioSource(%d)", as);
    if (audio_source < AUDIO_SOURCE_DEFAULT
        || audio_source >= AUDIO_SOURCE_CNT) {
        Logger::E(TAG, "Invalid audio source");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setAudioSource(audio_source),
        E_RUNTIME_EXCEPTION, "setAudioSource failed.");
}

ECode CMediaRecorder::GetAudioSourceMax(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    // FIXME disable selection of the remote submxi source selection once test code
    //       doesn't rely on it
    *result = IMediaRecorderAudioSource::FM_RX_A2DP;
    return NOERROR;
}

ECode CMediaRecorder::SetVideoSource(
    /* [in] */ Int32 video_source)
{
    //ALOGV("setVideoSource(%d)", vs);
    if (video_source < android::VIDEO_SOURCE_DEFAULT
        || video_source >= android::VIDEO_SOURCE_LIST_END) {
        Logger::E(TAG, "Invalid video source");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setVideoSource(video_source),
        E_RUNTIME_EXCEPTION, "setAudioSource failed.");
}

ECode CMediaRecorder::SetProfile(
    /* [in] */ ICamcorderProfile* profile)
{
    Int32 fileFormat;
    Int32 videoFrameRate, videoFrameWidth, videoFrameHeight;
    Int32 videoBitRate, videoCodec;

    profile->GetFileFormat(&fileFormat);
    profile->GetVideoFrameRate(&videoFrameRate);
    profile->GetVideoFrameWidth(&videoFrameWidth);
    profile->GetVideoFrameHeight(&videoFrameHeight);
    profile->GetVideoBitRate(&videoBitRate);
    profile->GetVideoCodec(&videoCodec);

    SetOutputFormat(fileFormat);
    SetVideoFrameRate(videoFrameRate);
    SetVideoSize(videoFrameWidth, videoFrameHeight);
    SetVideoEncodingBitRate(videoBitRate);
    SetVideoEncoder(videoCodec);

    Int32 quality = 0;
    profile->GetQuality(&quality);
    Int32 audioCodec = 0;
    profile->GetAudioCodec(&audioCodec);
    if (quality >= ICamcorderProfile::QUALITY_TIME_LAPSE_LOW &&
        quality <= ICamcorderProfile::QUALITY_TIME_LAPSE_QVGA) {
        // Nothing needs to be done. Call to setCaptureRate() enables
        // time lapse video recording.
    }
    else if (audioCodec >= 0) {
        Int32 audioBitRate, audioChannels, audioSampleRate, audioCodec;

        profile->GetAudioBitRate(&audioBitRate);
        profile->GetAudioChannels(&audioChannels);
        profile->GetAudioSampleRate(&audioSampleRate);
        profile->GetAudioCodec(&audioCodec);
        SetAudioEncodingBitRate(audioBitRate);
        SetAudioChannels(audioChannels);
        SetAudioSamplingRate(audioSampleRate);
        SetAudioEncoder(audioCodec);
    }
    return NOERROR;
}

ECode CMediaRecorder::SetCaptureRate(
    /* [in] */ Double fps)
{
    // Make sure that time lapse is enabled when this method is called.
    SetParameter("time-lapse-enable=1");

    Double timeBetweenFrameCapture = 1 / fps;
    Int64 timeBetweenFrameCaptureUs = (Int64) (1000000 * timeBetweenFrameCapture);
    StringBuilder sb("time-between-time-lapse-frame-capture=");
    sb.Append(timeBetweenFrameCaptureUs);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetOrientationHint(
    /* [in] */ Int32 degrees)
{
    if (degrees != 0 && degrees != 90 && degrees != 180 && degrees != 270) {
        Logger::E(TAG, "Unsupported angle: %d", degrees);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("video-param-rotation-angle-degrees=");
    sb.Append(degrees);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetLocation(
    /* [in] */ Float latitude,
    /* [in] */ Float longitude)
{
    Int32 latitudex10000  = (Int32) (latitude * 10000 + 0.5);
    Int32 longitudex10000 = (Int32) (longitude * 10000 + 0.5);

    if (latitudex10000 > 900000 || latitudex10000 < -900000) {
        Logger::E(TAG, "Latitude: %f out of range.", latitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (longitudex10000 > 1800000 || longitudex10000 < -1800000) {
        Logger::E(TAG, "Longitude: %f out of range.", longitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SetParameter(String::Format("param-geotag-latitude=%d", latitudex10000));
    SetParameter(String::Format("param-geotag-longitude=%d", longitudex10000));
    return NOERROR;
}

ECode CMediaRecorder::SetOutputFormat(
    /* [in] */ Int32 output_format)
{
    //ALOGV("setOutputFormat(%d)", of);
    if (output_format < android::OUTPUT_FORMAT_DEFAULT
        || output_format >= android::OUTPUT_FORMAT_LIST_END) {
        Logger::E(TAG, "Invalid output format");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setOutputFormat(output_format),
        E_RUNTIME_EXCEPTION, "setOutputFormat failed.");
}

/*JNI Method */ //throws IllegalStateException;
ECode CMediaRecorder::SetVideoSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    //ALOGV("setVideoSize(%d, %d)", width, height);
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    if (width <= 0 || height <= 0) {
        Logger::E(TAG, "invalid video size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return process_media_recorder_call(mr->setVideoSize(width, height),
        E_RUNTIME_EXCEPTION, "setVideoSize failed.");
}

ECode CMediaRecorder::SetVideoFrameRate(
    /* [in] */ Int32 rate)
{
    //ALOGV("setVideoFrameRate(%d)", rate);
    if (rate <= 0) {
        Logger::E(TAG, "invalid frame rate");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setVideoFrameRate(rate),
        E_RUNTIME_EXCEPTION, "SetVideoFrameRate failed.");
}

ECode CMediaRecorder::SetMaxDuration(
    /* [in] */ Int32 max_duration_ms)
{
    //ALOGV("setMaxDuration(%d)", maxDurationMs);
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    char params[64];
    snprintf(params, sizeof(params), "max-duration=%d", max_duration_ms);

    return process_media_recorder_call(mr->setParameters(android::String8(params)),
        E_RUNTIME_EXCEPTION, "SetMaxDuration failed.");
}

ECode CMediaRecorder::SetMaxFileSize(
    /* [in] */ Int64 max_filesize_bytes)
{
    //ALOGV("setMaxFileSize(%lld)", maxFilesizeBytes);
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    char params[64];
    snprintf(params, sizeof(params), "max-filesize=%lld", max_filesize_bytes);

    return process_media_recorder_call(mr->setParameters(android::String8(params)),
        E_RUNTIME_EXCEPTION, "SetMaxFileSize failed.");
}

ECode CMediaRecorder::SetAudioEncoder(
    /* [in] */ Int32 audio_encoder)
{
    //ALOGV("setAudioEncoder(%d)", ae);
    if (audio_encoder < android::AUDIO_ENCODER_DEFAULT
        || audio_encoder >= android::AUDIO_ENCODER_LIST_END) {
        Logger::E(TAG, "Invalid audio encoder");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setAudioEncoder(audio_encoder),
        E_RUNTIME_EXCEPTION, "SetAudioEncoder failed.");
}

ECode CMediaRecorder::SetVideoEncoder(
    /* [in] */ Int32 video_encoder)
{
    //ALOGV("setVideoEncoder(%d)", ve);
    if (video_encoder < android::VIDEO_ENCODER_DEFAULT
        || video_encoder >= android::VIDEO_ENCODER_LIST_END) {
        Logger::E(TAG, "Invalid video encoder");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setVideoEncoder(video_encoder),
        E_RUNTIME_EXCEPTION, "SetAudioEncoder failed.");
}

ECode CMediaRecorder::SetAudioSamplingRate(
    /* [in] */ Int32 samplingRate)
{
    if (samplingRate <= 0) {
        Logger::E(TAG, "Audio sampling rate is not positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("audio-param-sampling-rate=");
    sb.Append(samplingRate);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetAudioChannels(
    /* [in] */ Int32 numChannels)
{
    if (numChannels <= 0) {
        Logger::E(TAG, "Number of channels is not positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("audio-param-number-of-channels=");
    sb.Append(numChannels);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetAudioEncodingBitRate(
    /* [in] */ Int32 bitRate)
{
    if (bitRate <= 0) {
        Logger::E(TAG, "Audio encoding bit rate is not positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("audio-param-encoding-bitrate=");
    sb.Append(bitRate);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetVideoEncodingBitRate(
    /* [in] */ Int32 bitRate)
{
    if (bitRate <= 0) {
        Logger::E(TAG, "Video encoding bit rate is not positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("video-param-encoding-bitrate=");
    sb.Append(bitRate);
    return SetParameter(sb.ToString().string());
}

ECode CMediaRecorder::SetAuxiliaryOutputFile(
    /* [in] */ IFileDescriptor* fd)
{
    Logger::W(TAG, "setAuxiliaryOutputFile(FileDescriptor) is no longer supported.");
    return NOERROR;
}

ECode CMediaRecorder::SetAuxiliaryOutputFile(
    /* [in] */ const String& path)
{
    Logger::W(TAG, "setAuxiliaryOutputFile(String) is no longer supported.");
    return NOERROR;
}

ECode CMediaRecorder::SetOutputFile(
    /* [in] */ IFileDescriptor* fd)
{
    mPath = NULL;
    mFd = fd;
    return NOERROR;
}

ECode CMediaRecorder::SetOutputFile(
    /* [in] */ const String& path)
{
    mFd = NULL;
    mPath = path;
    return NOERROR;
}

ECode CMediaRecorder::NativeSetOutputFile(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    //ALOGV("setOutputFile");
    if (fd == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 rawFd;
    fd->GetDescriptor(&rawFd);
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    android::status_t opStatus = mr->setOutputFile(rawFd, offset, length);
    return process_media_recorder_call(opStatus,
        E_IO_EXCEPTION, "setOutputFile failed.");
}

ECode CMediaRecorder::NativePrepare()
{
    //ALOGV("prepare");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);

    if (mSurface != NULL) {
        const android::sp<android::Surface> native_surface = get_surface(mSurface);

        // The application may misbehave and
        // the preview surface becomes unavailable
        if (native_surface.get() == 0) {
            Logger::E(TAG, "Application lost the surface: invalid preview surface");
            return E_IO_EXCEPTION;
        }

        Logger::I(TAG, "prepare: surface=%p ", native_surface.get());
        ECode ec = process_media_recorder_call(mr->setPreviewSurface(native_surface->getIGraphicBufferProducer()),
            E_RUNTIME_EXCEPTION, "setPreviewSurface failed.");
        if (SUCCEEDED(ec)) return NOERROR;
    }

    return process_media_recorder_call(mr->prepare(),
        E_IO_EXCEPTION, "prepare failed.");
}

ECode CMediaRecorder::Prepare()
{
    if (mPath != NULL) {
        AutoPtr<IRandomAccessFile> file;
        CRandomAccessFile::New(mPath, String("rws"), (IRandomAccessFile**)&file);

        // try {
        AutoPtr<IFileDescriptor> fileDescriptor;
        file->GetFD((IFileDescriptor**)&fileDescriptor);
        NativeSetOutputFile(fileDescriptor, 0, 0);
        // } finally {
        ICloseable::Probe(file)->Close();
        // }
    }
    else if (mFd != NULL) {
        NativeSetOutputFile(mFd, 0, 0);
    }
    else {
        // throw new IOException("No valid output file");
        return E_IO_EXCEPTION;
    }

    NativePrepare();
    return NOERROR;
}

ECode CMediaRecorder::Start()
{
    //ALOGV("start");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->start(),
        E_RUNTIME_EXCEPTION, "start failed.");
}

ECode CMediaRecorder::Pause()
{
    // ALOGV("pause");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->pause(),
        E_RUNTIME_EXCEPTION, "pause failed.");
}

ECode CMediaRecorder::Stop()
{
    //ALOGV("stop");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->stop(),
        E_RUNTIME_EXCEPTION, "stop failed.");
}

ECode CMediaRecorder::Reset()
{
    NativeReset();

    // make sure none of the listeners get called anymore
    mEventHandler->RemoveCallbacksAndMessages(NULL);
    return NOERROR;
}

ECode CMediaRecorder::NativeReset()
{
    //ALOGV("native_reset");
    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->reset(),
        E_RUNTIME_EXCEPTION, "native_reset failed.");
}

ECode CMediaRecorder::GetMaxAmplitude(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->getMaxAmplitude(result),
        E_RUNTIME_EXCEPTION, "getMaxAmplitude failed.");
}

ECode CMediaRecorder::SetOnErrorListener(
    /* [in] */ IMediaRecorderOnErrorListener* l)
{
    mOnErrorListener = l;
    return NOERROR;
}

ECode CMediaRecorder::SetOnInfoListener(
    /* [in] */ IMediaRecorderOnInfoListener* listener)
{
    mOnInfoListener = listener;
    return NOERROR;
}

ECode CMediaRecorder::ReleaseResources()
{
    //ALOGV("release");
    android::sp<android::MediaRecorder> mr = setMediaRecorder(this, 0);
    if (mr != NULL) {
        mr->setListener(NULL);
        mr->release();
    }
    return NOERROR;
}

ECode CMediaRecorder::NativeSetup(
    /* [in] */ const String& packageName)
{
    //ALOGV("setup");
    android::sp<android::MediaRecorder> mr = new android::MediaRecorder();
    if (mr == NULL) {
        Logger::E(TAG, "Out of memory.");
        return E_RUNTIME_EXCEPTION;
    }

    if (mr->initCheck() != android::NO_ERROR) {
        Logger::E(TAG, "Unable to initialize media recorder.");
        return E_RUNTIME_EXCEPTION;
    }

    // create new listener and give it to MediaRecorder
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    android::sp<JNIMediaRecorderListener> listener = new JNIMediaRecorderListener(wr);
    mr->setListener(listener);

   // Convert client name jstring to String16
    android::String16 clientName(packageName.string(), packageName.GetLength());

    // pass client package name for permissions tracking
    mr->setClientName(clientName);

    setMediaRecorder(this, mr);
    return NOERROR;
}

/*JNI Method*/
ECode CMediaRecorder::NativeFinalize()
{
    //ALOGV("release");
    android::sp<android::MediaRecorder> mr = setMediaRecorder(this, 0);
    if (mr != NULL) {
        mr->setListener(NULL);
        mr->release();
    }
    return NOERROR;
}

/*JNI Method*/
ECode CMediaRecorder::SetParameter(
    /* [in] */ const char* nameValuePair)
{
    //ALOGV("setParameter()");
    if (nameValuePair == NULL){
        Logger::E(TAG, "Invalid or empty params string. This parameter will be ignored.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::MediaRecorder> mr = getMediaRecorder(this);
    return process_media_recorder_call(mr->setParameters(android::String8(nameValuePair)),
        E_RUNTIME_EXCEPTION, "setParameter failed.");
}

/*static*/
ECode CMediaRecorder::PostEventFromNative(
    /* [in] */ IWeakReference* mediarecorder_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMediaRecorder> strongObj;
    mediarecorder_ref->Resolve(EIID_IMediaRecorder, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CMediaRecorder* mr = (CMediaRecorder*)strongObj.Get();
    if (mr->mEventHandler != NULL) {
        AutoPtr<IMessage> message;
        mr->mEventHandler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&message);
        Boolean bval;
        return mr->mEventHandler->SendMessage(message, &bval);
    }
    return NOERROR;
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
