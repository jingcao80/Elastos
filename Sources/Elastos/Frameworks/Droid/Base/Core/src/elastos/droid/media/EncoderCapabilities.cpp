
#include "elastos/droid/media/EncoderCapabilities.h"
#include <elastos/core/AutoLock.h>
#include <media/MediaProfiles.h>

using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

static Object sLock;
android::MediaProfiles *sssProfiles = NULL;

String EncoderCapabilities::TAG("EncoderCapabilities");

Boolean EncoderCapabilities::InitStatic()
{
    // System.loadLibrary("media_jni");
    // ALOGV("native_init");
    synchronized(sLock) {
        if (sssProfiles == NULL) {
            sssProfiles = android::MediaProfiles::getInstance();
        }
    }
    return TRUE;
}

Boolean EncoderCapabilities::mInit = InitStatic();

EncoderCapabilities::VideoEncoderCap::VideoEncoderCap(
    /* [in] */ Int32 codec,
    /* [in] */ Int32 minBitRate,
    /* [in] */ Int32 maxBitRate,
    /* [in] */ Int32 minFrameRate,
    /* [in] */ Int32 maxFrameRate,
    /* [in] */ Int32 minFrameWidth,
    /* [in] */ Int32 maxFrameWidth,
    /* [in] */ Int32 minFrameHeight,
    /* [in] */ Int32 maxFrameHeight)
    : mCodec(codec)
    , mMinBitRate(minBitRate)
    , mMaxBitRate(maxBitRate)
    , mMinFrameRate(minFrameRate)
    , mMaxFrameRate(maxFrameRate)
    , mMinFrameWidth(minFrameWidth)
    , mMaxFrameWidth(maxFrameWidth)
    , mMinFrameHeight(minFrameHeight)
    , mMaxFrameHeight(maxFrameHeight)
{}

EncoderCapabilities::AudioEncoderCap::AudioEncoderCap(
    /* [in] */ Int32 codec,
    /* [in] */ Int32 minBitRate,
    /* [in] */ Int32 maxBitRate,
    /* [in] */ Int32 minSampleRate,
    /* [in] */ Int32 maxSampleRate,
    /* [in] */ Int32 minChannels,
    /* [in] */ Int32 maxChannels)
    : mCodec(codec)
    , mMinChannels(minChannels)
    , mMaxChannels(maxChannels)
    , mMinSampleRate(minSampleRate)
    , mMaxSampleRate(maxSampleRate)
    , mMinBitRate(minBitRate)
    , mMaxBitRate(maxBitRate)
{}

ECode EncoderCapabilities::GetOutputFileFormats(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 nFormats = Native_get_num_file_formats();
    if (nFormats == 0) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > formats = ArrayOf<Int32>::Alloc(nFormats);
    for (Int32 i = 0; i < nFormats; ++i) {
        (*formats)[i] = Native_get_file_format(i);
    }

    *result = formats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode EncoderCapabilities::GetVideoEncoders(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 nEncoders = Native_get_num_video_encoders();
    if (nEncoders == 0) {
        return NOERROR;
    }

    AutoPtr<IArrayList> encoderList;
    CArrayList::New((IArrayList**)&encoderList);
    for (Int32 i = 0; i < nEncoders; ++i) {
        encoderList->Add((IInterface*)(IObject*)Native_get_video_encoder_cap(i));
    }

    *result = IList::Probe(encoderList);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode EncoderCapabilities::GetAudioEncoders(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 nEncoders = Native_get_num_audio_encoders();
    if (nEncoders == 0) {
        return NOERROR;
    }

    AutoPtr<IArrayList> encoderList;
    CArrayList::New((IArrayList**)&encoderList);
    for (Int32 i = 0; i < nEncoders; ++i) {
        encoderList->Add((IInterface*)(IObject*)Native_get_audio_encoder_cap(i));
    }

    *result = IList::Probe(encoderList);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//---------------------------------------------------
//    Native method : android_media_MediaProfiles.cpp
//---------------------------------------------------

Int32 EncoderCapabilities::Native_get_num_file_formats()
{
    // ALOGV("native_get_num_file_formats");
    return sssProfiles->getOutputFileFormats().size();
}

Int32 EncoderCapabilities::Native_get_file_format(
    /* [in] */ Int32 index)
{
    // ALOGV("native_get_file_format: %d", index);
    android::Vector<android::output_format> formats = sssProfiles->getOutputFileFormats();
    Int32 nSize = formats.size();
    if (index < 0 || index >= nSize) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "out of array boundary");
        return -1;
    }
    return formats[index];
}

Int32 EncoderCapabilities::Native_get_num_video_encoders()
{
    // ALOGV("native_get_num_video_encoders");
    return sssProfiles->getVideoEncoders().size();
}

AutoPtr<EncoderCapabilities::VideoEncoderCap> EncoderCapabilities::Native_get_video_encoder_cap(
    /* [in] */ Int32 index)
{
    // ALOGV("native_get_video_encoder_cap: %d", index);
    android::Vector<android::video_encoder> encoders = sssProfiles->getVideoEncoders();
    Int32 nSize = encoders.size();
    if (index < 0 || index >= nSize) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "out of array boundary");
        return NULL;
    }

    android::video_encoder encoder = encoders[index];
    Int32 minBitRate = sssProfiles->getVideoEncoderParamByName("enc.vid.bps.min", encoder);
    Int32 maxBitRate = sssProfiles->getVideoEncoderParamByName("enc.vid.bps.max", encoder);
    Int32 minFrameRate = sssProfiles->getVideoEncoderParamByName("enc.vid.fps.min", encoder);
    Int32 maxFrameRate = sssProfiles->getVideoEncoderParamByName("enc.vid.fps.max", encoder);
    Int32 minFrameWidth = sssProfiles->getVideoEncoderParamByName("enc.vid.width.min", encoder);
    Int32 maxFrameWidth = sssProfiles->getVideoEncoderParamByName("enc.vid.width.max", encoder);
    Int32 minFrameHeight = sssProfiles->getVideoEncoderParamByName("enc.vid.height.min", encoder);
    Int32 maxFrameHeight = sssProfiles->getVideoEncoderParamByName("enc.vid.height.max", encoder);

    // Check on the values retrieved
    if ((minBitRate == -1 || maxBitRate == -1) ||
        (minFrameRate == -1 || maxFrameRate == -1) ||
        (minFrameWidth == -1 || maxFrameWidth == -1) ||
        (minFrameHeight == -1 || maxFrameHeight == -1)) {

        // jniThrowException(env, "java/lang/RuntimeException", "Error retrieving video encoder capability params");
        return NULL;
    }

    // Construct an instance of the VideoEncoderCap and set its member variables
    // jclass videoEncoderCapClazz = env->FindClass("android/media/EncoderCapabilities$VideoEncoderCap");
    // jmethodID videoEncoderCapConstructorMethodID = env->GetMethodID(videoEncoderCapClazz, "<init>", "(IIIIIIIII)V");
    // jobject cap = env->NewObject(videoEncoderCapClazz,
    //                              videoEncoderCapConstructorMethodID,
    //                              static_cast<int>(encoder),
    //                              minBitRate, maxBitRate,
    //                              minFrameRate, maxFrameRate,
    //                              minFrameWidth, maxFrameWidth,
    //                              minFrameHeight, maxFrameHeight);
    // return cap;
    AutoPtr<VideoEncoderCap> cap = new VideoEncoderCap(encoder,
                                                       minBitRate,
                                                       maxBitRate,
                                                       minFrameRate,
                                                       maxFrameRate,
                                                       minFrameWidth,
                                                       maxFrameWidth,
                                                       minFrameHeight,
                                                       maxFrameHeight);
    return cap;
}

Int32 EncoderCapabilities::Native_get_num_audio_encoders()
{
    // ALOGV("native_get_num_audio_encoders");
    return sssProfiles->getAudioEncoders().size();
}

AutoPtr<EncoderCapabilities::AudioEncoderCap> EncoderCapabilities::Native_get_audio_encoder_cap(
    /* [in] */ Int32 index)
{
    // ALOGV("native_get_audio_encoder_cap: %d", index);
    android::Vector<android::audio_encoder> encoders = sssProfiles->getAudioEncoders();
    Int32 nSize = encoders.size();
    if (index < 0 || index >= nSize) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "out of array boundary");
        return NULL;
    }

    android::audio_encoder encoder = encoders[index];
    Int32 minBitRate = sssProfiles->getAudioEncoderParamByName("enc.aud.bps.min", encoder);
    Int32 maxBitRate = sssProfiles->getAudioEncoderParamByName("enc.aud.bps.max", encoder);
    Int32 minSampleRate = sssProfiles->getAudioEncoderParamByName("enc.aud.hz.min", encoder);
    Int32 maxSampleRate = sssProfiles->getAudioEncoderParamByName("enc.aud.hz.max", encoder);
    Int32 minChannels = sssProfiles->getAudioEncoderParamByName("enc.aud.ch.min", encoder);
    Int32 maxChannels = sssProfiles->getAudioEncoderParamByName("enc.aud.ch.max", encoder);

    // Check on the values retrieved
    if ((minBitRate == -1 || maxBitRate == -1) ||
        (minSampleRate == -1 || maxSampleRate == -1) ||
        (minChannels == -1 || maxChannels == -1)) {

        // jniThrowException(env, "java/lang/RuntimeException", "Error retrieving video encoder capability params");
        return NULL;
    }

    // jclass audioEncoderCapClazz = env->FindClass("android/media/EncoderCapabilities$AudioEncoderCap");
    // jmethodID audioEncoderCapConstructorMethodID = env->GetMethodID(audioEncoderCapClazz, "<init>", "(IIIIIII)V");
    // jobject cap = env->NewObject(audioEncoderCapClazz,
    //                              audioEncoderCapConstructorMethodID,
    //                              static_cast<int>(encoder),
    //                              minBitRate, maxBitRate,
    //                              minSampleRate, maxSampleRate,
    //                              minChannels, maxChannels);
    // return cap;
    AutoPtr<AudioEncoderCap> cap = new AudioEncoderCap(encoder,
                                                       minBitRate,
                                                       maxBitRate,
                                                       minSampleRate,
                                                       maxSampleRate,
                                                       minChannels,
                                                       maxChannels);
    return cap;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
