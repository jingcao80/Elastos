
#include "elastos/droid/media/CMediaScannerConnectionHelper.h"
#include "elastos/droid/media/CMediaScannerConnection.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaScannerConnectionHelper)

CAR_INTERFACE_IMPL(CMediaScannerConnectionHelper, Object, IMediaScannerConnectionHelper)

ECode CMediaScannerConnectionHelper::ScanFile(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* paths,
    /* [in] */ ArrayOf<String>* mimeTypes,
    /* [in] */ IOnScanCompletedListener* cb)
{
    return CMediaScannerConnection::ScanFile(context, paths, mimeTypes, cb);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
