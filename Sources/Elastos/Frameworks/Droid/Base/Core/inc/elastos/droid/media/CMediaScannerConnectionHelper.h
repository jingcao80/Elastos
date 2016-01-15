
#ifndef __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTIONHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTIONHELPER_H__

#include "_Elastos_Droid_Media_CMediaScannerConnectionHelper.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaScannerConnectionHelper)
    , public Singleton
    , public IMediaScannerConnectionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Convenience for constructing a {@link MediaScannerConnection}, calling
     * {@link #connect} on it, and calling {@link #scanFile} with the given
     * <var>path</var> and <var>mimeType</var> when the connection is
     * established.
     * @param context The caller's Context, required for establishing a connection to
     * the media scanner service.
     * Success or failure of the scanning operation cannot be determined until
     * {@link MediaScannerConnectionClient#onScanCompleted(String, Uri)} is called.
     * @param paths Array of paths to be scanned.
     * @param mimeTypes Optional array of MIME types for each path.
     * If mimeType is NULL, then the mimeType will be inferred from the file extension.
     * @param callback Optional callback through which you can receive the
     * scanned URI and MIME type; If NULL, the file will be scanned but
     * you will not get a result back.
     * @see scanFile(String, String)
     */
    CARAPI ScanFile(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* paths,
        /* [in] */ ArrayOf<String>* mimeTypes,
        /* [in] */ IOnScanCompletedListener* cb);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASCANNERCONNECTIONHELPER_H__
