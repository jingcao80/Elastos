
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DownloadInfo.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Interface to be implemented by the embedder to handle file downloads.
 */
class ContentViewDownloadDelegate : public Object
{
public:
    /**
    * Notify the host application that a file should be downloaded. Replaces
    * onDownloadStart from DownloadListener.
    * @param downloadInfo Information about the requested download.
    */
    virtual CARAPI_(void) RequestHttpGetDownload(
        /* [in] */ DownloadInfo* downloadInfo) = 0;

    /**
     * Notify the host application that a download is started.
     * @param filename File name of the downloaded file.
     * @param mimeType Mime of the downloaded item.
     */
    virtual CARAPI_(void) OnDownloadStarted(
        /* [in] */ const String& filename,
        /* [in] */ const String& mimeType) = 0;

    /**
     * Notify the host application that a download has an extension indicating
     * a dangerous file type.
     * @param filename File name of the downloaded file.
     * @param downloadId The download id.
     */
    virtual CARAPI_(void) OnDangerousDownload(
        /* [in] */ const String& filename,
        /* [in] */ Int32 downloadId) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__
