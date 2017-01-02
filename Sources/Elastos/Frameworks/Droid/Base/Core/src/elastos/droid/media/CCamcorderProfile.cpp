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

#include "elastos/droid/media/CCamcorderProfile.h"
#include "elastos/droid/hardware/CHardwareCamera.h"
#include "elastos/droid/hardware/CHardwareCameraHelper.h"
#include <media/MediaProfiles.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::CHardwareCamera;
using Elastos::Droid::Hardware::CHardwareCameraHelper;
using Elastos::Droid::Hardware::IHardwareCameraInfo;
using Elastos::Droid::Hardware::IHardwareCameraHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

static Object sLock;

const Int32 CCamcorderProfile::QUALITY_LIST_START = ICamcorderProfile::QUALITY_LOW;
const Int32 CCamcorderProfile::QUALITY_LIST_END = ICamcorderProfile::QUALITY_HVGA;
const Int32 CCamcorderProfile::QUALITY_TIME_LAPSE_LIST_START = ICamcorderProfile::QUALITY_TIME_LAPSE_LOW;
const Int32 CCamcorderProfile::QUALITY_TIME_LAPSE_LIST_END = ICamcorderProfile::QUALITY_TIME_LAPSE_4kDCI;
const Int32 CCamcorderProfile::QUALITY_HIGH_SPEED_LIST_START = ICamcorderProfile::QUALITY_HIGH_SPEED_LOW;
const Int32 CCamcorderProfile::QUALITY_HIGH_SPEED_LIST_END = ICamcorderProfile::QUALITY_HIGH_SPEED_2160P;

CAR_INTERFACE_IMPL(CCamcorderProfile, Object, ICamcorderProfile)

CAR_OBJECT_IMPL(CCamcorderProfile)

CCamcorderProfile::CCamcorderProfile()
    : mDuration(0)
    , mQuality(0)
    , mFileFormat(0)
    , mVideoCodec(0)
    , mVideoBitRate(0)
    , mVideoFrameRate(0)
    , mVideoFrameWidth(0)
    , mVideoFrameHeight(0)
    , mAudioCodec(0)
    , mAudioBitRate(0)
    , mAudioSampleRate(0)
    , mAudioChannels(0)
{
}

CCamcorderProfile::~CCamcorderProfile()
{
}

ECode CCamcorderProfile::constructor(
    /* [in] */ Int32 duration,
    /* [in] */ Int32 quality,
    /* [in] */ Int32 fileFormat,
    /* [in] */ Int32 videoCodec,
    /* [in] */ Int32 videoBitRate,
    /* [in] */ Int32 videoFrameRate,
    /* [in] */ Int32 videoWidth,
    /* [in] */ Int32 videoHeight,
    /* [in] */ Int32 audioCodec,
    /* [in] */ Int32 audioBitRate,
    /* [in] */ Int32 audioSampleRate,
    /* [in] */ Int32 audioChannels)
{
    mDuration = duration;
    mQuality = quality;
    mFileFormat = fileFormat;
    mVideoCodec = videoCodec;
    mVideoBitRate = videoBitRate;
    mVideoFrameRate = videoFrameRate;
    mVideoFrameWidth = videoWidth;
    mVideoFrameHeight = videoHeight;
    mAudioCodec = audioCodec;
    mAudioBitRate = audioBitRate;
    mAudioSampleRate = audioSampleRate;
    mAudioChannels = audioChannels;

    return NOERROR;
}

/*static*/
ECode CCamcorderProfile::Get(
    /* [in] */ Int32 quality,
    /* [out] */ ICamcorderProfile** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IHardwareCameraHelper> hardwareCameraHelper;
    CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&hardwareCameraHelper);
    Int32 numberOfCameras;
    hardwareCameraHelper->GetNumberOfCameras(&numberOfCameras);

    AutoPtr<IHardwareCameraInfo> cameraInfo = new CHardwareCamera::CameraInfo();

    Int32 facing;
    for (Int32 i = 0; i < numberOfCameras; i++) {
        hardwareCameraHelper->GetCameraInfo(i, cameraInfo);
        cameraInfo->GetFacing(&facing);
        if (facing == IHardwareCameraInfo::CAMERA_FACING_BACK) {
            return Get(i, quality, result);
        }
    }
    return NOERROR;
}

/*static*/
ECode CCamcorderProfile::Get(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ ICamcorderProfile** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (!((quality >= QUALITY_LIST_START && quality <= QUALITY_LIST_END) ||
            (quality >= QUALITY_TIME_LAPSE_LIST_START && quality <= QUALITY_TIME_LAPSE_LIST_END) ||
            (quality >= QUALITY_HIGH_SPEED_LIST_START &&
            quality <= QUALITY_HIGH_SPEED_LIST_END))) {
        Slogger::E("CCamcorderProfile", "Unsupported quality level:  %d", quality);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NativeGetCamcorderProfile(cameraId, quality, result);
}

/*static*/
ECode CCamcorderProfile::HasProfile(
    /* [in] */ Int32 quality,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IHardwareCameraHelper> hardwareCameraHelper;
    CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&hardwareCameraHelper);
    Int32 numberOfCameras;
    hardwareCameraHelper->GetNumberOfCameras(&numberOfCameras);

    AutoPtr<IHardwareCameraInfo> cameraInfo = new CHardwareCamera::CameraInfo();

    Int32 facing;
    for (Int32 i = 0; i < numberOfCameras; i++) {
        hardwareCameraHelper->GetCameraInfo(i, cameraInfo);
        if (cameraInfo->GetFacing(&facing) == IHardwareCameraInfo::CAMERA_FACING_BACK) {
            HasProfile(i, quality, result);
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CCamcorderProfile::SetDuration(
    /* [in] */ Int32 duration)
{
    mDuration = duration;
    return NOERROR;
}

ECode CCamcorderProfile::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode CCamcorderProfile::SetQuality(
    /* [in] */ Int32 quality)
{
    mQuality = quality;
    return NOERROR;
}

ECode CCamcorderProfile::GetQuality(
    /* [out] */ Int32* quality)
{
    VALIDATE_NOT_NULL(quality);
    *quality = mQuality;
    return NOERROR;
}

ECode CCamcorderProfile::SetFileFormat(
    /* [in] */ Int32 fileFormat)
{
    mFileFormat = fileFormat;
    return NOERROR;
}

ECode CCamcorderProfile::GetFileFormat(
    /* [out] */ Int32* fileFormat)
{
    VALIDATE_NOT_NULL(fileFormat);
    *fileFormat = mFileFormat;
    return NOERROR;
}

ECode CCamcorderProfile::SetVideoCodec(
    /* [in] */ Int32 videoCodec)
{
    mVideoCodec = videoCodec;
    return NOERROR;
}

ECode CCamcorderProfile::GetVideoCodec(
    /* [out] */ Int32* videoCodec)
{
    VALIDATE_NOT_NULL(videoCodec);
    *videoCodec = mVideoCodec;
    return NOERROR;
}

ECode CCamcorderProfile::SetVideoBitRate(
    /* [in] */ Int32 videoBitRate)
{
    mVideoBitRate = videoBitRate;
    return NOERROR;
}

ECode CCamcorderProfile::GetVideoBitRate(
    /* [out] */ Int32* videoBitRate)
{
    VALIDATE_NOT_NULL(videoBitRate);
    *videoBitRate = mVideoBitRate;
    return NOERROR;
}

ECode CCamcorderProfile::SetVideoFrameRate(
    /* [in] */ Int32 videoFrameRate)
{
    mVideoFrameRate = videoFrameRate;
    return NOERROR;
}

ECode CCamcorderProfile::GetVideoFrameRate(
    /* [out] */ Int32* videoFrameRate)
{
    VALIDATE_NOT_NULL(videoFrameRate);
    *videoFrameRate = mVideoFrameRate;
    return NOERROR;
}

ECode CCamcorderProfile::SetVideoFrameWidth(
    /* [in] */ Int32 videoFrameWidth)
{
    mVideoFrameWidth = videoFrameWidth;
    return NOERROR;
}

ECode CCamcorderProfile::GetVideoFrameWidth(
    /* [out] */ Int32* videoFrameWidth)
{
    VALIDATE_NOT_NULL(videoFrameWidth);
    *videoFrameWidth = mVideoFrameWidth;
    return NOERROR;
}

ECode CCamcorderProfile::SetVideoFrameHeight(
    /* [in] */ Int32 videoFrameHeight)
{
    mVideoFrameHeight = videoFrameHeight;
    return NOERROR;
}

ECode CCamcorderProfile::GetVideoFrameHeight(
    /* [out] */ Int32* videoFrameHeight)
{
    VALIDATE_NOT_NULL(videoFrameHeight);
    *videoFrameHeight = mVideoFrameHeight;
    return NOERROR;
}

ECode CCamcorderProfile::SetAudioCodec(
    /* [in] */ Int32 audioCodec)
{
    mAudioCodec = audioCodec;
    return NOERROR;
}

ECode CCamcorderProfile::GetAudioCodec(
    /* [out] */ Int32* audioCodec)
{
    VALIDATE_NOT_NULL(audioCodec);
    *audioCodec = mAudioCodec;
    return NOERROR;
}

ECode CCamcorderProfile::SetAudioBitRate(
    /* [in] */ Int32 audioBitRate)
{
    mAudioBitRate = audioBitRate;
    return NOERROR;
}

ECode CCamcorderProfile::GetAudioBitRate(
    /* [out] */ Int32* audioBitRate)
{
    VALIDATE_NOT_NULL(audioBitRate);
    *audioBitRate = mAudioBitRate;
    return NOERROR;
}

ECode CCamcorderProfile::SetAudioSampleRate(
    /* [in] */ Int32 audioSampleRate)
{
    mAudioSampleRate = audioSampleRate;
    return NOERROR;
}

ECode CCamcorderProfile::GetAudioSampleRate(
    /* [out] */ Int32* audioSampleRate)
{
    VALIDATE_NOT_NULL(audioSampleRate);
    *audioSampleRate = mAudioSampleRate;
    return NOERROR;
}

ECode CCamcorderProfile::SetAudioChannels(
    /* [in] */ Int32 audioChannels)
{
    mAudioChannels = audioChannels;
    return NOERROR;
}

ECode CCamcorderProfile::GetAudioChannels(
    /* [out] */ Int32* audioChannels)
{
    VALIDATE_NOT_NULL(audioChannels);
    *audioChannels = mAudioChannels;
    return NOERROR;
}

/*static*/
ECode CCamcorderProfile::HasProfile(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeHasCamcorderProfile(cameraId, quality);
    return NOERROR;
}

/*JNI Method*/
/*static*/
ECode CCamcorderProfile::NativeGetCamcorderProfile(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ICamcorderProfile** profile)
{
    VALIDATE_NOT_NULL(profile);
    *profile = NULL;

    //ALOGV("native_get_camcorder_profile: %d %d", id, quality);
    if (!IsCamcorderQualityKnown(quality)) {
        Slogger::E("CCamcorderProfile", "Unknown camcorder profile quality");
        return E_RUNTIME_EXCEPTION;
    }

    android::MediaProfiles* sProfiles = android::MediaProfiles::getInstance();
    android::camcorder_quality q = static_cast<android::camcorder_quality>(quality);
    Int32 duration         = sProfiles->getCamcorderProfileParamByName("duration",    cameraId, q);
    Int32 fileFormat       = sProfiles->getCamcorderProfileParamByName("file.format", cameraId, q);
    Int32 videoCodec       = sProfiles->getCamcorderProfileParamByName("vid.codec",   cameraId, q);
    Int32 videoBitRate     = sProfiles->getCamcorderProfileParamByName("vid.bps",     cameraId, q);
    Int32 videoFrameRate   = sProfiles->getCamcorderProfileParamByName("vid.fps",     cameraId, q);
    Int32 videoFrameWidth  = sProfiles->getCamcorderProfileParamByName("vid.width",   cameraId, q);
    Int32 videoFrameHeight = sProfiles->getCamcorderProfileParamByName("vid.height",  cameraId, q);
    Int32 audioCodec       = sProfiles->getCamcorderProfileParamByName("aud.codec",   cameraId, q);
    Int32 audioBitRate     = sProfiles->getCamcorderProfileParamByName("aud.bps",     cameraId, q);
    Int32 audioSampleRate  = sProfiles->getCamcorderProfileParamByName("aud.hz",      cameraId, q);
    Int32 audioChannels    = sProfiles->getCamcorderProfileParamByName("aud.ch",      cameraId, q);

    // Check on the values retrieved
    if (duration == -1 || fileFormat == -1 || videoCodec == -1 || audioCodec == -1 ||
        videoBitRate == -1 || videoFrameRate == -1 || videoFrameWidth == -1 || videoFrameHeight == -1 ||
        audioBitRate == -1 || audioSampleRate == -1 || audioChannels == -1) {
        Slogger::E("CCamcorderProfile", "Error retrieving camcorder profile params");
        return E_RUNTIME_EXCEPTION;
    }

    //jclass camcorderProfileClazz = env->FindClass("android/media/CamcorderProfile");
    //jmethodID camcorderProfileConstructorMethodID = env->GetMethodID(camcorderProfileClazz, "<init>", "(IIIIIIIIIIII)V");
    AutoPtr<CCamcorderProfile> camcorderProfile;
    CCamcorderProfile::NewByFriend(
        duration,
        quality,
        fileFormat,
        videoCodec,
        videoBitRate,
        videoFrameRate,
        videoFrameWidth,
        videoFrameHeight,
        audioCodec,
        audioBitRate,
        audioSampleRate,
        audioChannels,
        (CCamcorderProfile**)&camcorderProfile);
    *profile = (ICamcorderProfile*)camcorderProfile.Get();
    REFCOUNT_ADD(*profile);
    return NOERROR;
}

/*JNI Method*/
/*static*/
Boolean CCamcorderProfile::NativeHasCamcorderProfile(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality)
{
    //ALOGV("native_has_camcorder_profile: %d %d", id, quality);
    if (!IsCamcorderQualityKnown(quality)) {
        return FALSE;
    }

    android::MediaProfiles* sProfiles = android::MediaProfiles::getInstance();
    android::camcorder_quality q = static_cast<android::camcorder_quality>(quality);
    return sProfiles->hasCamcorderProfile(cameraId, q);
    return TRUE;
}

/*JNI Method*/
/*static*/
Boolean CCamcorderProfile::IsCamcorderQualityKnown(
    /* [in] */ Int32 quality)
{
    return ((quality >= android::CAMCORDER_QUALITY_LIST_START &&
             quality <= android::CAMCORDER_QUALITY_LIST_END) ||
            (quality >= android::CAMCORDER_QUALITY_TIME_LAPSE_LIST_START &&
             quality <= android::CAMCORDER_QUALITY_TIME_LAPSE_LIST_END) ||
            (quality >= android::CAMCORDER_QUALITY_HIGH_SPEED_LIST_START &&
             quality <= android::CAMCORDER_QUALITY_HIGH_SPEED_LIST_END));
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
