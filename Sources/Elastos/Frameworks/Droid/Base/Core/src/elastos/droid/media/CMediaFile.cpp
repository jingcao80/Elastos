#include "elastos/droid/media/CMediaFile.h"
#include "elastos/droid/media/MediaFile.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaFile)
CAR_INTERFACE_IMPL(CMediaFile, Singleton, IMediaFile);

ECode CMediaFile::IsAudioFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsAudioFileType(fileType, result);
}

ECode CMediaFile::IsVideoFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsVideoFileType(fileType, result);
}

ECode CMediaFile::IsImageFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsImageFileType(fileType, result);
}

ECode CMediaFile::IsPlayListFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsPlayListFileType(fileType, result);
}

ECode CMediaFile::IsDrmFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsDrmFileType(fileType, result);
}

ECode CMediaFile::GetFileType(
    /* [in] */ const String& path,
    /* [out] */ IMediaFileType** result)
{
    return MediaFile::GetFileType(path, result);
}

ECode CMediaFile::IsMimeTypeMedia(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* result)
{
    return MediaFile::IsMimeTypeMedia(mimeType, result);
}

ECode CMediaFile::GetFileTitle(
    /* [in] */ const String& path,
    /* [out] */ String* result)
{
    return MediaFile::GetFileTitle(path, result);
}

ECode CMediaFile::GetFileTypeForMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ Int32* result)
{
    return MediaFile::GetFileTypeForMimeType(mimeType, result);
}

ECode CMediaFile::GetMimeTypeForFile(
    /* [in] */ const String& path,
    /* [out] */ String* result)
{
    return MediaFile::GetMimeTypeForFile(path, result);
}

ECode CMediaFile::GetFormatCode(
    /* [in] */ const String& fileName,
    /* [in] */ const String& mimeType,
    /* [out] */ Int32* result)
{
    return MediaFile::GetFormatCode(fileName, mimeType, result);
}

ECode CMediaFile::GetMimeTypeForFormatCode(
    /* [in] */ Int32 formatCode,
    /* [out] */ String* result)
{
    return MediaFile::GetMimeTypeForFormatCode(formatCode, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
