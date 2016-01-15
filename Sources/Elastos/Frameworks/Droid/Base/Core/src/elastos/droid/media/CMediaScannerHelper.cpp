
#include "elastos/droid/media/CMediaScannerHelper.h"
#include "elastos/droid/media/CMediaScanner.h"

namespace Elastos {
namespace Droid {
namespace Media {

/*static*/
ECode CMediaScannerHelper::IsNoMediaPath(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    return CMediaScanner::IsNoMediaPath(path, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
