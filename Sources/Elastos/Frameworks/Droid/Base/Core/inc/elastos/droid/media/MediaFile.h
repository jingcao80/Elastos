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

#ifndef __ELASTOS_DROID_MEDIA_MEDIAFILE_H__
#define __ELASTOS_DROID_MEDIA_MEDIAFILE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Media {

class MediaFile
{
public:
    static CARAPI AddFileType(
        /* [in] */ const String& extension,
        /* [in] */ Int32 fileType,
        /* [in] */ const String& mimeType);

    static CARAPI AddFileType(
        /* [in] */ const String& extension,
        /* [in] */ Int32 fileType,
        /* [in] */ const String& mimeType,
        /* [in] */ Int32 mtpFormatCode);

    static CARAPI IsAudioFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    static CARAPI IsVideoFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    static CARAPI IsImageFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    static CARAPI IsPlayListFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    static CARAPI IsDrmFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    static CARAPI GetFileType(
        /* [in] */ const String& path,
        /* [out] */ IMediaFileType** result);

    static CARAPI IsMimeTypeMedia(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean* result);

    // generates a title based on file name
    static CARAPI GetFileTitle(
        /* [in] */ const String& path,
        /* [out] */ String* result);

    static CARAPI GetFileTypeForMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ Int32* result);

    static CARAPI GetMimeTypeForFile(
        /* [in] */ const String& path,
        /* [out] */ String* result);

    static CARAPI GetFormatCode(
        /* [in] */ const String& fileName,
        /* [in] */ const String& mimeType,
        /* [out] */ Int32* result);

    static CARAPI GetMimeTypeForFormatCode(
        /* [in] */ Int32 formatCode,
        /* [out] */ String* result);

private:
    static CARAPI_(Boolean) IsWMAEnabled();

    static CARAPI_(Boolean) IsWMVEnabled();

    static CARAPI_(Boolean) InitStatic();

    static CARAPI_(Boolean) InitHashMap();

private:
    // Audio file types
    static const Int32 FIRST_AUDIO_FILE_TYPE;
    static const Int32 LAST_AUDIO_FILE_TYPE;

    // MIDI file types
    static const Int32 FIRST_MIDI_FILE_TYPE;
    static const Int32 LAST_MIDI_FILE_TYPE;

    // Video file types
    static const Int32 FIRST_VIDEO_FILE_TYPE;
    static const Int32 LAST_VIDEO_FILE_TYPE;

    // More audio file types
    static const Int32 FIRST_AUDIO_FILE_TYPE_EXT;
    static const Int32 LAST_AUDIO_FILE_TYPE_EXT;

    // More video file types
    static const Int32 FIRST_VIDEO_FILE_TYPE2;
    static const Int32 LAST_VIDEO_FILE_TYPE2;

    // Image file types
    static const Int32 FIRST_IMAGE_FILE_TYPE;
    static const Int32 LAST_IMAGE_FILE_TYPE;

    // Playlist file types
    static const Int32 FIRST_PLAYLIST_FILE_TYPE;
    static const Int32 LAST_PLAYLIST_FILE_TYPE;

    // Drm file types
    static const Int32 FIRST_DRM_FILE_TYPE;
    static const Int32 LAST_DRM_FILE_TYPE;

    static HashMap<String, AutoPtr<IMediaFileType> > sFileTypeMap;
    static HashMap<String, Int32> sMimeTypeMap;
    static HashMap<String, Int32> sFileTypeToFormatMap;
    static HashMap<String, Int32> sMimeTypeToFormatMap;
    static HashMap<Int32, String> sFormatToMimeTypeMap;

    static Boolean mInit;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAFILE_H__
