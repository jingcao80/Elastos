#include "elastos/droid/media/CMediaMetadataHelper.h"
#include "elastos/droid/media/CMediaMetadata.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaMetadataHelper, Singleton, IMediaMetadataHelper)

CAR_SINGLETON_IMPL(CMediaMetadataHelper)

ECode CMediaMetadataHelper::GetKeyFromMetadataEditorKey(
    /* [in] */ Int32 editorKey,
    /* [out] */ String* result)
{
    return CMediaMetadata::GetKeyFromMetadataEditorKey(editorKey, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
