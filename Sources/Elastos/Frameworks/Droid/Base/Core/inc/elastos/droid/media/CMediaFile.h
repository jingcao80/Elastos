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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAFILE_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAFILE_H__

#include "_Elastos_Droid_Media_CMediaFile.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaFile)
    , public Singleton
    , public IMediaFile
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsAudioFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    CARAPI IsVideoFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    CARAPI IsImageFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    CARAPI IsPlayListFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    CARAPI IsDrmFileType(
        /* [in] */ Int32 fileType,
        /* [out] */ Boolean* result);

    CARAPI GetFileType(
        /* [in] */ const String& path,
        /* [out] */ IMediaFileType** result);

    CARAPI IsMimeTypeMedia(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean* result);

    // generates a title based on file name
    CARAPI GetFileTitle(
        /* [in] */ const String& path,
        /* [out] */ String* result);

    CARAPI GetFileTypeForMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ Int32* result);

    CARAPI GetMimeTypeForFile(
        /* [in] */ const String& path,
        /* [out] */ String* result);

    CARAPI GetFormatCode(
        /* [in] */ const String& fileName,
        /* [in] */ const String& mimeType,
        /* [out] */ Int32* result);

    CARAPI GetMimeTypeForFormatCode(
        /* [in] */ Int32 formatCode,
        /* [out] */ String* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAFILE_H__
