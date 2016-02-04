
#include "elastos/droid/media/CMediaScannerHelper.h"
#include "elastos/droid/media/CMediaScanner.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaScannerHelper)

CAR_INTERFACE_IMPL(CMediaScannerHelper, Singleton, IMediaScannerHelper)

/*static*/
ECode CMediaScannerHelper::IsNoMediaPath(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    return CMediaScanner::IsNoMediaPath(path, result);
}

ECode CARAPI ClearMediaPathCache(
    /* [in] */ Boolean clearMediaPaths,
    /* [in] */ Boolean clearNoMediaPaths)
{
    return CMediaScanner::ClearMediaPathCache(clearMediaPaths, clearNoMediaPaths);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
