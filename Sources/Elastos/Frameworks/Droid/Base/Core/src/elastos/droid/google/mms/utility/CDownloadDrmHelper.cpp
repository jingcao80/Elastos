#include "elastos/droid/google/mms/utility/CDownloadDrmHelper.h"
#include "elastos/droid/google/mms/utility/DownloadDrmHelper.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CAR_INTERFACE_IMPL(CDownloadDrmHelper, Singleton, IDownloadDrmHelper)
CAR_SINGLETON_IMPL(CDownloadDrmHelper)

ECode CDownloadDrmHelper::IsDrmMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& mimetype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DownloadDrmHelper::IsDrmMimeType(context, mimetype);
    return NOERROR;
}

ECode CDownloadDrmHelper::IsDrmConvertNeeded(
    /* [in] */ const String& mimetype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DownloadDrmHelper::IsDrmConvertNeeded(mimetype);
    return NOERROR;
}

ECode CDownloadDrmHelper::ModifyDrmFwLockFileExtension(
    /* [in] */ const String& filename,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DownloadDrmHelper::ModifyDrmFwLockFileExtension(filename);
    return NOERROR;
}

ECode CDownloadDrmHelper::GetOriginalMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& path,
    /* [in] */ const String& containingMime,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DownloadDrmHelper::GetOriginalMimeType(context, path, containingMime);
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
