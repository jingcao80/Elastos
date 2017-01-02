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

#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/ContentType.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICloneable;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

//=====================================================================
//                             ContentType
//=====================================================================
//const String ContentType::MMS_MESSAGE("application/vnd.wap.mms-message");
//const String ContentType::MMS_GENERIC("application/vnd.wap.mms-generic");
//const String ContentType::MULTIPART_MIXED("application/vnd.wap.multipart.mixed");
//const String ContentType::MULTIPART_RELATED("application/vnd.wap.multipart.related");
//const String ContentType::MULTIPART_ALTERNATIVE("application/vnd.wap.multipart.alternative");
//const String ContentType::TEXT_PLAIN("text/plain");
//const String ContentType::TEXT_HTML("text/html");
//const String ContentType::TEXT_VCALENDAR("text/x-vCalendar");
//const String ContentType::TEXT_VCARD("text/x-vCard");
//const String ContentType::IMAGE_UNSPECIFIED("image/*");
//const String ContentType::IMAGE_JPEG("image/jpeg");
//const String ContentType::IMAGE_JPG("image/jpg");
//const String ContentType::IMAGE_GIF("image/gif");
//const String ContentType::IMAGE_WBMP("image/vnd.wap.wbmp");
//const String ContentType::IMAGE_PNG("image/png");
//const String ContentType::IMAGE_X_MS_BMP("image/x-ms-bmp");
//const String ContentType::AUDIO_UNSPECIFIED("audio/*");
//const String ContentType::AUDIO_AAC("audio/aac");
//const String ContentType::AUDIO_AAC_MP4("audio/aac_mp4");
//const String ContentType::AUDIO_QCELP("audio/qcelp");
//const String ContentType::AUDIO_EVRC("audio/evrc");
//const String ContentType::AUDIO_AMR_WB("audio/amr-wb");
//const String ContentType::AUDIO_AMR("audio/amr");
//const String ContentType::AUDIO_IMELODY("audio/imelody");
//const String ContentType::AUDIO_MID("audio/mid");
//const String ContentType::AUDIO_MIDI("audio/midi");
//const String ContentType::AUDIO_MP3("audio/mp3");
//const String ContentType::AUDIO_MPEG3("audio/mpeg3");
//const String ContentType::AUDIO_MPEG("audio/mpeg");
//const String ContentType::AUDIO_MPG("audio/mpg");
//const String ContentType::AUDIO_MP4("audio/mp4");
//const String ContentType::AUDIO_X_MID("audio/x-mid");
//const String ContentType::AUDIO_X_MIDI("audio/x-midi");
//const String ContentType::AUDIO_X_MP3("audio/x-mp3");
//const String ContentType::AUDIO_X_MPEG3("audio/x-mpeg3");
//const String ContentType::AUDIO_X_MPEG("audio/x-mpeg");
//const String ContentType::AUDIO_X_MPG("audio/x-mpg");
//const String ContentType::AUDIO_3GPP("audio/3gpp");
//const String ContentType::AUDIO_X_WAV("audio/x-wav");
//const String ContentType::AUDIO_OGG("application/ogg");
//const String ContentType::VIDEO_UNSPECIFIED("video/*");
//const String ContentType::VIDEO_3GPP("video/3gpp");
//const String ContentType::VIDEO_3G2("video/3gpp2");
//const String ContentType::VIDEO_H263("video/h263");
//const String ContentType::VIDEO_MP4("video/mp4");
//const String ContentType::APP_SMIL("application/smil");
//const String ContentType::APP_WAP_XHTML("application/vnd.wap.xhtml+xml");
//const String ContentType::APP_XHTML("application/xhtml+xml");
//const String ContentType::APP_DRM_CONTENT("application/vnd.oma.drm.content");
//const String ContentType::APP_DRM_MESSAGE("application/vnd.oma.drm.message");

static AutoPtr<IArrayList> InitArrayList()
{
    AutoPtr<IArrayList> array;
    CArrayList::New((IArrayList**)&array);
    return array;
}

AutoPtr<IArrayList> ContentType::sSupportedContentTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedImageTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedAudioTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedVideoTypes = InitArrayList();

Boolean ContentType::IsSupportedType(
    /* [in] */ const String& contentType)
{
    Boolean bContain = FALSE;
    return (!contentType.IsNull()) && (sSupportedContentTypes->Contains(CoreUtils::Convert(contentType), &bContain), bContain);
}

Boolean ContentType::IsSupportedImageType(
    /* [in] */ const String& contentType)
{
    return IsImageType(contentType) && IsSupportedType(contentType);
}

Boolean ContentType::IsSupportedAudioType(
    /* [in] */ const String& contentType)
{
    return IsAudioType(contentType) && IsSupportedType(contentType);
}

Boolean ContentType::IsSupportedVideoType(
    /* [in] */ const String& contentType)
{
    return IsVideoType(contentType) && IsSupportedType(contentType);
}

Boolean ContentType::IsTextType(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull()) && contentType.StartWith("text/");
}

Boolean ContentType::IsImageType(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull()) && contentType.StartWith("image/");
}

Boolean ContentType::IsAudioType(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull()) && contentType.StartWith("audio/");
}

Boolean ContentType::IsVideoType(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull()) && contentType.StartWith("video/");
}

Boolean ContentType::IsDrmType(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull())
            && (contentType.Equals(APP_DRM_CONTENT)
                    || contentType.Equals(APP_DRM_MESSAGE));
}

Boolean ContentType::IsUnspecified(
    /* [in] */ const String& contentType)
{
    return (!contentType.IsNull()) && contentType.EndWith("*");
}

AutoPtr<IArrayList> ContentType::GetImageTypes()
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sSupportedImageTypes)->Clone((IInterface**)&p);
    return IArrayList::Probe(p);
}

AutoPtr<IArrayList> ContentType::GetAudioTypes()
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sSupportedAudioTypes)->Clone((IInterface**)&p);
    return IArrayList::Probe(p);
}

AutoPtr<IArrayList> ContentType::GetVideoTypes()
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sSupportedVideoTypes)->Clone((IInterface**)&p);
    return IArrayList::Probe(p);
}

AutoPtr<IArrayList> ContentType::GetSupportedTypes()
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sSupportedContentTypes)->Clone((IInterface**)&p);
    return IArrayList::Probe(p);
}

ContentType::ContentType()
{
}

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
