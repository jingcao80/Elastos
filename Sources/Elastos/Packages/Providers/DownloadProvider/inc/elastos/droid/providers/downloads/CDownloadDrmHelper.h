
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADDRMHELPER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADDRMHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadDrmHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

CarClass(CDownloadDrmHelper)
    , public Singleton
    , public IDownloadDrmHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Checks if the Media Type needs to be DRM converted
     *
     * @param mimetype Media type of the content
     * @return True if convert is needed else false
     */
    CARAPI IsDrmConvertNeeded(
        /* [in] */ const String& mimetype,
        /* [out] */ Boolean* result);

    /**
     * Modifies the file extension for a DRM Forward Lock file NOTE: This
     * function shouldn't be called if the file shouldn't be DRM converted
     */
    CARAPI ModifyDrmFwLockFileExtension(
        /* [in] */ const String& filename,
        /* [out] */ String* result);

    /**
     * Return the original MIME type of the given file, using the DRM framework
     * if the file is protected content.
     */
    CARAPI GetOriginalMimeType(
        /* [in] */ IContext* context,
        /* [in] */ IFile* file,
        /* [in] */ const String& currentMime,
        /* [out] */ String* result);

public:
    /** The MIME type of special DRM files */
    static const String MIMETYPE_DRM_MESSAGE;

    /** The extensions of special DRM files */
    static const String EXTENSION_DRM_MESSAGE;

    static const String EXTENSION_INTERNAL_FWDL;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADDRMHELPER_H__
