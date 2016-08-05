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
    Boolean res = FALSE;
    if (context != NULL) {
        // try {
            assert(0 && "TODO");
            // DrmManagerClient drmClient = new DrmManagerClient(context);
            // if (drmClient != NULL && !mimetype.IsNull() && mimetype.GetLength() > 0) {
            //     res = drmClient->CanHandle(String(""), mimetype);
            // }
        // } catch (IllegalArgumentException e) {
        //     Log.w(TAG,
        //             "DrmManagerClient instance could not be created, context is Illegal.");
        // } catch (IllegalStateException e) {
        //     Log.w(TAG, "DrmManagerClient didn't initialize properly.");
        // }
    }
    *result = res;
    return NOERROR;
}

ECode CDownloadDrmHelper::IsDrmConvertNeeded(
    /* [in] */ const String& mimetype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IDownloadDrmHelper::MIMETYPE_DRM_MESSAGE.Equals(mimetype);
    return NOERROR;
}

ECode CDownloadDrmHelper::ModifyDrmFwLockFileExtension(
    /* [in] */ const String& _filename,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String filename = _filename;
    if (!filename.IsNull()) {
        Int32 extensionIndex = 0;
        extensionIndex = filename.LastIndexOf(".");
        if (extensionIndex != -1) {
            filename = filename.Substring(0, extensionIndex);
        }
        assert(0 && "TODO");
        // filename = filename.Concat(EXTENSION_INTERNAL_FWDL);
    }
    *result = filename;
    return NOERROR;
}

ECode CDownloadDrmHelper::GetOriginalMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& path,
    /* [in] */ const String& containingMime,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String res(containingMime);
    assert(0 && "TODO");
    // AutoPtr<IDrmManagerClient> drmClient = new DrmManagerClient(context);
    // if (drmClient->CanHandle(path, NULL)) {
    //     drmClient->GetOriginalMimeType(path, &res);
    // }
    *result = res;
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
