#include "elastos/droid/media/session/CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaSessionLegacyHelperHelper, Singleton, IMediaSessionLegacyHelperHelper)

CAR_SINGLETON_IMPL(CMediaSessionLegacyHelperHelper)

ECode CMediaSessionLegacyHelperHelper::GetHelper(
    /* [in] */ IContext * context,
    /* [out] */ IMediaSessionLegacyHelper ** result)
{
    return CMediaSessionLegacyHelper::GetHelper(context, result);
}

ECode CMediaSessionLegacyHelperHelper::GetOldMetadata(
    /* [in] */ IMediaMetadata * metadata,
    /* [in] */ Int32 artworkWidth,
    /* [in] */ Int32 artworkHeight,
    /* [out] */ IBundle ** result)
{
    return CMediaSessionLegacyHelper::GetOldMetadata(metadata, artworkWidth, artworkHeight, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
