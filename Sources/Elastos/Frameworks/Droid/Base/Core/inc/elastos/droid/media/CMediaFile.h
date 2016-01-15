
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
