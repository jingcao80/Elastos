
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADCONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class DownloadInfo;

/**
 * Java counterpart of android DownloadController.
 *
 * Its a singleton class instantiated by the C++ DownloadController.
 */
//@JNINamespace("content")
class DownloadController
    : public Object
{
public:
    /**
     * Class for notifying the application that download has completed.
     */
    class DownloadNotificationService
        : public Object
    {
    public:
        /**
         * Notify the host application that a download is finished.
         * @param downloadInfo Information about the completed download.
         */
        virtual CARAPI_(void) OnDownloadCompleted(
            /* [in] */ const DownloadInfo* downloadInfo) = 0;

        /**
         * Notify the host application that a download is in progress.
         * @param downloadInfo Information about the in-progress download.
         */
        virtual CARAPI_(void) OnDownloadUpdated(
            /* [in] */ const DownloadInfo* downloadInfo) = 0;
    };

public:
    //@CalledByNative return DownloadController
    static CARAPI_(AutoPtr<IInterface>) GetInstance();

    static CARAPI_(void) SetDownloadNotificationService(
        /* [in] */ DownloadNotificationService* service);

    /**
     * Notifies the download delegate of a new GET download and passes all the information
     * needed to download the file.
     *
     * The download delegate is expected to handle the download.
     */
    //@CalledByNative
    CARAPI_(void) NewHttpGetDownload(
        /* [in] */ ContentViewCore* view,
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& cookie,
        /* [in] */ const String& referer,
        /* [in] */ const String& filename,
        /* [in] */ Int64 contentLength);

    /**
     * Notifies the download delegate that a new download has started. This can
     * be either a POST download or a GET download with authentication.
     * @param view ContentViewCore associated with the download item.
     * @param filename File name of the downloaded file.
     * @param mimeType Mime of the downloaded item.
     */
    //@CalledByNative
    CARAPI_(void) OnDownloadStarted(
        /* [in] */ ContentViewCore* view,
        /* [in] */ const String& filename,
        /* [in] */ const String& mimeType);

    /**
     * Notifies the download delegate that a download completed and passes along info about the
     * download. This can be either a POST download or a GET download with authentication.
     */
    //@CalledByNative
    CARAPI_(void) OnDownloadCompleted(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& filename,
        /* [in] */ const String& path,
        /* [in] */ Int64 contentLength,
        /* [in] */ Boolean successful,
        /* [in] */ Int32 downloadId);

    /**
     * Notifies the download delegate about progress of a download. Downloads that use Chrome
     * network stack use custom notification to display the progress of downloads.
     */
    //@CalledByNative
    CARAPI_(void) OnDownloadUpdated(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& filename,
        /* [in] */ const String& path,
        /* [in] */ Int64 contentLength,
        /* [in] */ Boolean successful,
        /* [in] */ Int32 downloadId,
        /* [in] */ Int32 percentCompleted,
        /* [in] */ Int64 timeRemainingInMs);

    /**
     * Notifies the download delegate that a dangerous download started.
     */
    //@CalledByNative
    CARAPI_(void) OnDangerousDownload(
        /* [in] */ ContentViewCore* view,
        /* [in] */ const String& filename,
        /* [in] */ Int32 downloadId);

    static CARAPI_(void*) ElaDownloadControllerCallback_Init();

private:

    static CARAPI_(void) NewHttpGetDownload(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* view,
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& cookie,
        /* [in] */ const String& referer,
        /* [in] */ const String& filename,
        /* [in] */ Int64 contentLength);

    static CARAPI_(void) OnDownloadStarted(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* view,
        /* [in] */ const String& filename,
        /* [in] */ const String& mimeType);

    static CARAPI_(void) OnDownloadCompleted(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* context,
        /* [in] */ const String& url,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& filename,
        /* [in] */ const String& path,
        /* [in] */ Int64 contentLength,
        /* [in] */ Boolean successful,
        /* [in] */ Int32 downloadId);

    static CARAPI_(void) OnDownloadUpdated(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* context,
        /* [in] */ const String& url,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& filename,
        /* [in] */ const String& path,
        /* [in] */ Int64 contentLength,
        /* [in] */ Boolean successful,
        /* [in] */ Int32 downloadId,
        /* [in] */ Int32 percentCompleted,
        /* [in] */ Int64 timeRemainingInMs);

    static CARAPI_(void) OnDangerousDownload(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* view,
        /* [in] */ const String& filename,
        /* [in] */ Int32 downloadId);

    DownloadController();

    static CARAPI_(AutoPtr<ContentViewDownloadDelegate>) DownloadDelegateFromView(
        /* [in] */ ContentViewCore* view);

    // native methods
    CARAPI_(void) NativeInit();

private:
    static const String LOGTAG;
    static AutoPtr<DownloadController> sInstance;

    static AutoPtr<DownloadNotificationService> sDownloadNotificationService;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADCONTROLLER_H__
