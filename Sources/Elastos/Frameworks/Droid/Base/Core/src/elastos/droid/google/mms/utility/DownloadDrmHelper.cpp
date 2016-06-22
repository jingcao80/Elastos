
#include "elastos/droid/google/mms/utility//DownloadDrmHelper.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                          DownloadDrmHelper
//=====================================================================
//const String DownloadDrmHelper::MIMETYPE_DRM_MESSAGE("application/vnd.oma.drm.message");
//const String DownloadDrmHelper::EXTENSION_DRM_MESSAGE(".dm");
//const String DownloadDrmHelper::EXTENSION_INTERNAL_FWDL(".fl");
const String DownloadDrmHelper::TAG("DownloadDrmHelper");

Boolean DownloadDrmHelper::IsDrmMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& mimetype)
{
    // ==================before translated======================
    // boolean result = false;
    // if (context != null) {
    //     try {
    //         DrmManagerClient drmClient = new DrmManagerClient(context);
    //         if (drmClient != null && mimetype != null && mimetype.length() > 0) {
    //             result = drmClient.canHandle("", mimetype);
    //         }
    //     } catch (IllegalArgumentException e) {
    //         Log.w(TAG,
    //                 "DrmManagerClient instance could not be created, context is Illegal.");
    //     } catch (IllegalStateException e) {
    //         Log.w(TAG, "DrmManagerClient didn't initialize properly.");
    //     }
    // }
    // return result;
    assert(0);
    return FALSE;
}

Boolean DownloadDrmHelper::IsDrmConvertNeeded(
    /* [in] */ const String& mimetype)
{
    // ==================before translated======================
    // return MIMETYPE_DRM_MESSAGE.equals(mimetype);
    assert(0);
    return FALSE;
}

String DownloadDrmHelper::ModifyDrmFwLockFileExtension(
    /* [in] */ const String& filename)
{
    // ==================before translated======================
    // if (filename != null) {
    //     int extensionIndex;
    //     extensionIndex = filename.lastIndexOf(".");
    //     if (extensionIndex != -1) {
    //         filename = filename.substring(0, extensionIndex);
    //     }
    //     filename = filename.concat(EXTENSION_INTERNAL_FWDL);
    // }
    // return filename;
    assert(0);
    return String("");
}

String DownloadDrmHelper::GetOriginalMimeType(
    /* [in] */ IContext* context,
    /* [in] */ const String& path,
    /* [in] */ const String& containingMime)
{
    // ==================before translated======================
    // String result = containingMime;
    // DrmManagerClient drmClient = new DrmManagerClient(context);
    // try {
    //     if (drmClient.canHandle(path, null)) {
    //         result = drmClient.getOriginalMimeType(path);
    //     }
    // } catch (IllegalArgumentException ex) {
    //     Log.w(TAG,
    //             "Can't get original mime type since path is null or empty string.");
    // } catch (IllegalStateException ex) {
    //     Log.w(TAG, "DrmManagerClient didn't initialize properly.");
    // }
    // return result;
    assert(0);
    return String("");
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
