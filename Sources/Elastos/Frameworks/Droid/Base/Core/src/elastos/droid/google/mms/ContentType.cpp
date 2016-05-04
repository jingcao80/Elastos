/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.android.mms;

using Elastos::Utility::IArrayList;

public class ContentType {
    public static const String MMS_MESSAGE       = "application/vnd.wap.mms-message";
    // The phony content type for generic PDUs (e.g. ReadOrig.ind,
    // Notification.ind, Delivery.ind).
    public static const String MMS_GENERIC       = "application/vnd.wap.mms-generic";
    public static const String MULTIPART_MIXED   = "application/vnd.wap.multipart.mixed";
    public static const String MULTIPART_RELATED = "application/vnd.wap.multipart.related";
    public static const String MULTIPART_ALTERNATIVE = "application/vnd.wap.multipart.alternative";

    public static const String TEXT_PLAIN        = "text/plain";
    public static const String TEXT_HTML         = "text/html";
    public static const String TEXT_VCALENDAR    = "text/x-vCalendar";
    public static const String TEXT_VCARD        = "text/x-vCard";

    public static const String IMAGE_UNSPECIFIED = "image/*";
    public static const String IMAGE_JPEG        = "image/jpeg";
    public static const String IMAGE_JPG         = "image/jpg";
    public static const String IMAGE_GIF         = "image/gif";
    public static const String IMAGE_WBMP        = "image/vnd.wap.wbmp";
    public static const String IMAGE_PNG         = "image/png";
    public static const String IMAGE_X_MS_BMP    = "image/x-ms-bmp";

    public static const String AUDIO_UNSPECIFIED = "audio/*";
    public static const String AUDIO_AAC         = "audio/aac";
    public static const String AUDIO_AAC_MP4     = "audio/aac_mp4";
    public static const String AUDIO_QCELP       = "audio/qcelp";
    public static const String AUDIO_EVRC        = "audio/evrc";
    public static const String AUDIO_AMR_WB      = "audio/amr-wb";
    public static const String AUDIO_AMR         = "audio/amr";
    public static const String AUDIO_IMELODY     = "audio/imelody";
    public static const String AUDIO_MID         = "audio/mid";
    public static const String AUDIO_MIDI        = "audio/midi";
    public static const String AUDIO_MP3         = "audio/mp3";
    public static const String AUDIO_MPEG3       = "audio/mpeg3";
    public static const String AUDIO_MPEG        = "audio/mpeg";
    public static const String AUDIO_MPG         = "audio/mpg";
    public static const String AUDIO_MP4         = "audio/mp4";
    public static const String AUDIO_X_MID       = "audio/x-mid";
    public static const String AUDIO_X_MIDI      = "audio/x-midi";
    public static const String AUDIO_X_MP3       = "audio/x-mp3";
    public static const String AUDIO_X_MPEG3     = "audio/x-mpeg3";
    public static const String AUDIO_X_MPEG      = "audio/x-mpeg";
    public static const String AUDIO_X_MPG       = "audio/x-mpg";
    public static const String AUDIO_3GPP        = "audio/3gpp";
    public static const String AUDIO_X_WAV       = "audio/x-wav";
    public static const String AUDIO_OGG         = "application/ogg";

    public static const String VIDEO_UNSPECIFIED = "video/*";
    public static const String VIDEO_3GPP        = "video/3gpp";
    public static const String VIDEO_3G2         = "video/3gpp2";
    public static const String VIDEO_H263        = "video/h263";
    public static const String VIDEO_MP4         = "video/mp4";

    public static const String APP_SMIL          = "application/smil";
    public static const String APP_WAP_XHTML     = "application/vnd.wap.xhtml+xml";
    public static const String APP_XHTML         = "application/xhtml+xml";

    public static const String APP_DRM_CONTENT   = "application/vnd.oma.drm.content";
    public static const String APP_DRM_MESSAGE   = "application/vnd.oma.drm.message";

    private static const ArrayList<String> sSupportedContentTypes = new ArrayList<String>();
    private static const ArrayList<String> sSupportedImageTypes = new ArrayList<String>();
    private static const ArrayList<String> sSupportedAudioTypes = new ArrayList<String>();
    private static const ArrayList<String> sSupportedVideoTypes = new ArrayList<String>();

    static {
        sSupportedContentTypes->Add(TEXT_PLAIN);
        sSupportedContentTypes->Add(TEXT_HTML);
        sSupportedContentTypes->Add(TEXT_VCALENDAR);
        sSupportedContentTypes->Add(TEXT_VCARD);

        sSupportedContentTypes->Add(IMAGE_JPEG);
        sSupportedContentTypes->Add(IMAGE_GIF);
        sSupportedContentTypes->Add(IMAGE_WBMP);
        sSupportedContentTypes->Add(IMAGE_PNG);
        sSupportedContentTypes->Add(IMAGE_JPG);
        sSupportedContentTypes->Add(IMAGE_X_MS_BMP);
        //supportedContentTypes->Add(IMAGE_SVG); not yet supported.

        sSupportedContentTypes->Add(AUDIO_AAC);
        sSupportedContentTypes->Add(AUDIO_AAC_MP4);
        sSupportedContentTypes->Add(AUDIO_QCELP);
        sSupportedContentTypes->Add(AUDIO_EVRC);
        sSupportedContentTypes->Add(AUDIO_AMR_WB);
        sSupportedContentTypes->Add(AUDIO_AMR);
        sSupportedContentTypes->Add(AUDIO_IMELODY);
        sSupportedContentTypes->Add(AUDIO_MID);
        sSupportedContentTypes->Add(AUDIO_MIDI);
        sSupportedContentTypes->Add(AUDIO_MP3);
        sSupportedContentTypes->Add(AUDIO_MP4);
        sSupportedContentTypes->Add(AUDIO_MPEG3);
        sSupportedContentTypes->Add(AUDIO_MPEG);
        sSupportedContentTypes->Add(AUDIO_MPG);
        sSupportedContentTypes->Add(AUDIO_X_MID);
        sSupportedContentTypes->Add(AUDIO_X_MIDI);
        sSupportedContentTypes->Add(AUDIO_X_MP3);
        sSupportedContentTypes->Add(AUDIO_X_MPEG3);
        sSupportedContentTypes->Add(AUDIO_X_MPEG);
        sSupportedContentTypes->Add(AUDIO_X_MPG);
        sSupportedContentTypes->Add(AUDIO_X_WAV);
        sSupportedContentTypes->Add(AUDIO_3GPP);
        sSupportedContentTypes->Add(AUDIO_OGG);

        sSupportedContentTypes->Add(VIDEO_3GPP);
        sSupportedContentTypes->Add(VIDEO_3G2);
        sSupportedContentTypes->Add(VIDEO_H263);
        sSupportedContentTypes->Add(VIDEO_MP4);

        sSupportedContentTypes->Add(APP_SMIL);
        sSupportedContentTypes->Add(APP_WAP_XHTML);
        sSupportedContentTypes->Add(APP_XHTML);

        sSupportedContentTypes->Add(APP_DRM_CONTENT);
        sSupportedContentTypes->Add(APP_DRM_MESSAGE);

        // add supported image types
        sSupportedImageTypes->Add(IMAGE_JPEG);
        sSupportedImageTypes->Add(IMAGE_GIF);
        sSupportedImageTypes->Add(IMAGE_WBMP);
        sSupportedImageTypes->Add(IMAGE_PNG);
        sSupportedImageTypes->Add(IMAGE_JPG);
        sSupportedImageTypes->Add(IMAGE_X_MS_BMP);

        // add supported audio types
        sSupportedAudioTypes->Add(AUDIO_AAC);
        sSupportedAudioTypes->Add(AUDIO_AAC_MP4);
        sSupportedAudioTypes->Add(AUDIO_QCELP);
        sSupportedAudioTypes->Add(AUDIO_EVRC);
        sSupportedAudioTypes->Add(AUDIO_AMR_WB);
        sSupportedAudioTypes->Add(AUDIO_AMR);
        sSupportedAudioTypes->Add(AUDIO_IMELODY);
        sSupportedAudioTypes->Add(AUDIO_MID);
        sSupportedAudioTypes->Add(AUDIO_MIDI);
        sSupportedAudioTypes->Add(AUDIO_MP3);
        sSupportedAudioTypes->Add(AUDIO_MPEG3);
        sSupportedAudioTypes->Add(AUDIO_MPEG);
        sSupportedAudioTypes->Add(AUDIO_MPG);
        sSupportedAudioTypes->Add(AUDIO_MP4);
        sSupportedAudioTypes->Add(AUDIO_X_MID);
        sSupportedAudioTypes->Add(AUDIO_X_MIDI);
        sSupportedAudioTypes->Add(AUDIO_X_MP3);
        sSupportedAudioTypes->Add(AUDIO_X_MPEG3);
        sSupportedAudioTypes->Add(AUDIO_X_MPEG);
        sSupportedAudioTypes->Add(AUDIO_X_MPG);
        sSupportedAudioTypes->Add(AUDIO_X_WAV);
        sSupportedAudioTypes->Add(AUDIO_3GPP);
        sSupportedAudioTypes->Add(AUDIO_OGG);

        // add supported video types
        sSupportedVideoTypes->Add(VIDEO_3GPP);
        sSupportedVideoTypes->Add(VIDEO_3G2);
        sSupportedVideoTypes->Add(VIDEO_H263);
        sSupportedVideoTypes->Add(VIDEO_MP4);
    }

    // This class should never be instantiated.
    private ContentType() {
    }

    public static Boolean IsSupportedType(String contentType) {
        Return (NULL != contentType) && sSupportedContentTypes->Contains(contentType);
    }

    public static Boolean IsSupportedImageType(String contentType) {
        return IsImageType(contentType) && IsSupportedType(contentType);
    }

    public static Boolean IsSupportedAudioType(String contentType) {
        return IsAudioType(contentType) && IsSupportedType(contentType);
    }

    public static Boolean IsSupportedVideoType(String contentType) {
        return IsVideoType(contentType) && IsSupportedType(contentType);
    }

    public static Boolean IsTextType(String contentType) {
        Return (NULL != contentType) && contentType->StartsWith("text/");
    }

    public static Boolean IsImageType(String contentType) {
        Return (NULL != contentType) && contentType->StartsWith("image/");
    }

    public static Boolean IsAudioType(String contentType) {
        Return (NULL != contentType) && contentType->StartsWith("audio/");
    }

    public static Boolean IsVideoType(String contentType) {
        Return (NULL != contentType) && contentType->StartsWith("video/");
    }

    public static Boolean IsDrmType(String contentType) {
        Return (NULL != contentType)
                && (contentType->Equals(APP_DRM_CONTENT)
                        || contentType->Equals(APP_DRM_MESSAGE));
    }

    public static Boolean IsUnspecified(String contentType) {
        Return (NULL != contentType) && contentType->EndsWith("*");
    }

    @SuppressWarnings("unchecked")
    public static ArrayList<String> GetImageTypes() {
        Return (ArrayList<String>) sSupportedImageTypes->Clone();
    }

    @SuppressWarnings("unchecked")
    public static ArrayList<String> GetAudioTypes() {
        Return (ArrayList<String>) sSupportedAudioTypes->Clone();
    }

    @SuppressWarnings("unchecked")
    public static ArrayList<String> GetVideoTypes() {
        Return (ArrayList<String>) sSupportedVideoTypes->Clone();
    }

    @SuppressWarnings("unchecked")
    public static ArrayList<String> GetSupportedTypes() {
        Return (ArrayList<String>) sSupportedContentTypes->Clone();
    }
}
