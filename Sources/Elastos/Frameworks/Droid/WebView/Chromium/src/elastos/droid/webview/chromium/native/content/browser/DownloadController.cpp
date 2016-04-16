
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DownloadController.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/DownloadController_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DownloadInfo.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

const String DownloadController::LOGTAG("DownloadController");
AutoPtr<DownloadController> DownloadController::sInstance;
AutoPtr<DownloadController::DownloadNotificationService> DownloadController::sDownloadNotificationService;

DownloadController::DownloadController()
{
    NativeInit();
}

//@CalledByNative
AutoPtr<IInterface> DownloadController::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new DownloadController();
    }

    return TO_IINTERFACE(sInstance);
}

AutoPtr<ContentViewDownloadDelegate> DownloadController::DownloadDelegateFromView(
    /* [in] */ ContentViewCore* view)
{
    return view->GetDownloadDelegate();
}

void DownloadController::SetDownloadNotificationService(
    /* [in] */ DownloadController::DownloadNotificationService* service)
{
    sDownloadNotificationService = service;
}

/**
 * Notifies the download delegate of a new GET download and passes all the information
 * needed to download the file.
 *
 * The download delegate is expected to handle the download.
 */
//@CalledByNative
void DownloadController::NewHttpGetDownload(
    /* [in] */ ContentViewCore* view,
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& cookie,
    /* [in] */ const String& referer,
    /* [in] */ const String& filename,
    /* [in] */ Int64 contentLength)
{
    AutoPtr<ContentViewDownloadDelegate> downloadDelegate = DownloadDelegateFromView(view);

    if (downloadDelegate != NULL) {
        AutoPtr<DownloadInfo::Builder> builder = new DownloadInfo::Builder();
        builder->SetUrl(url);
        builder->SetUserAgent(userAgent);
        builder->SetContentDisposition(contentDisposition);
        builder->SetMimeType(mimeType);
        builder->SetCookie(cookie);
        builder->SetReferer(referer);
        builder->SetFileName(filename);
        builder->SetContentLength(contentLength);
        builder->SetIsGETRequest(TRUE);
        AutoPtr<DownloadInfo> downloadInfo = builder->Build();
        downloadDelegate->RequestHttpGetDownload(downloadInfo);
    }
}

/**
 * Notifies the download delegate that a new download has started. This can
 * be either a POST download or a GET download with authentication.
 * @param view ContentViewCore associated with the download item.
 * @param filename File name of the downloaded file.
 * @param mimeType Mime of the downloaded item.
 */
//@CalledByNative
void DownloadController::OnDownloadStarted(
    /* [in] */ ContentViewCore* view,
    /* [in] */ const String& filename,
    /* [in] */ const String& mimeType)
{
    AutoPtr<ContentViewDownloadDelegate> downloadDelegate = DownloadDelegateFromView(view);

    if (downloadDelegate != NULL) {
        downloadDelegate->OnDownloadStarted(filename, mimeType);
    }
}

/**
 * Notifies the download delegate that a download completed and passes along info about the
 * download. This can be either a POST download or a GET download with authentication.
 */
//@CalledByNative
void DownloadController::OnDownloadCompleted(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& filename,
    /* [in] */ const String& path,
    /* [in] */ Int64 contentLength,
    /* [in] */ Boolean successful,
    /* [in] */ Int32 downloadId)
{
    if (sDownloadNotificationService != NULL) {
        AutoPtr<DownloadInfo::Builder> builder = new DownloadInfo::Builder();
        builder->SetUrl(url);
        builder->SetMimeType(mimeType);
        builder->SetFileName(filename);
        builder->SetFilePath(path);
        builder->SetContentLength(contentLength);
        builder->SetIsSuccessful(successful);
        builder->SetDescription(filename);
        builder->SetDownloadId(downloadId);
        builder->SetHasDownloadId(TRUE);
        AutoPtr<DownloadInfo> downloadInfo = builder->Build();
        sDownloadNotificationService->OnDownloadCompleted(downloadInfo);
    }
}

/**
 * Notifies the download delegate about progress of a download. Downloads that use Chrome
 * network stack use custom notification to display the progress of downloads.
 */
//@CalledByNative
void DownloadController::OnDownloadUpdated(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& filename,
    /* [in] */ const String& path,
    /* [in] */ Int64 contentLength,
    /* [in] */ Boolean successful,
    /* [in] */ Int32 downloadId,
    /* [in] */ Int32 percentCompleted,
    /* [in] */ Int64 timeRemainingInMs)
{
    if (sDownloadNotificationService != NULL) {
        AutoPtr<DownloadInfo::Builder> builder = new DownloadInfo::Builder();
        builder->SetUrl(url);
        builder->SetMimeType(mimeType);
        builder->SetFileName(filename);
        builder->SetFilePath(path);
        builder->SetContentLength(contentLength);
        builder->SetIsSuccessful(successful);
        builder->SetDescription(filename);
        builder->SetDownloadId(downloadId);
        builder->SetHasDownloadId(TRUE);
        builder->SetPercentCompleted(percentCompleted);
        builder->SetTimeRemainingInMillis(timeRemainingInMs);
        AutoPtr<DownloadInfo> downloadInfo = builder->Build();
        sDownloadNotificationService->OnDownloadUpdated(downloadInfo);
    }
}

/**
 * Notifies the download delegate that a dangerous download started.
 */
//@CalledByNative
void DownloadController::OnDangerousDownload(
    /* [in] */ ContentViewCore* view,
    /* [in] */ const String& filename,
    /* [in] */ Int32 downloadId)
{
    AutoPtr<ContentViewDownloadDelegate> downloadDelegate = DownloadDelegateFromView(view);
    if (downloadDelegate != NULL) {
        downloadDelegate->OnDangerousDownload(filename, downloadId);
    }
}

// native methods
void DownloadController::NativeInit()
{
    Elastos_DownloadController_nativeInit(TO_IINTERFACE(this));
}

void DownloadController::NewHttpGetDownload(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* view,
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& cookie,
    /* [in] */ const String& referer,
    /* [in] */ const String& filename,
    /* [in] */ Int64 contentLength)
{
    AutoPtr<DownloadController> mObj = (DownloadController*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(LOGTAG, "DownloadController::NewHttpGetDownload, mObj is NULL");
        return;
    }
    AutoPtr<ContentViewCore> cvc = (ContentViewCore*)(IObject::Probe(view));
    mObj->NewHttpGetDownload(cvc, url, userAgent, contentDisposition, mimeType, cookie, referer, filename, contentLength);
}

void DownloadController::OnDownloadStarted(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* view,
    /* [in] */ const String& filename,
    /* [in] */ const String& mimeType)
{
    AutoPtr<DownloadController> mObj = (DownloadController*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(LOGTAG, "DownloadController::OnDownloadStarted, mObj is NULL");
        return;
    }
    AutoPtr<ContentViewCore> cvc = (ContentViewCore*)(IObject::Probe(view));
    mObj->OnDownloadStarted(cvc, filename, mimeType);
}

void DownloadController::OnDownloadCompleted(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* context,
    /* [in] */ const String& url,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& filename,
    /* [in] */ const String& path,
    /* [in] */ Int64 contentLength,
    /* [in] */ Boolean successful,
    /* [in] */ Int32 downloadId)
{
    AutoPtr<DownloadController> mObj = (DownloadController*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(LOGTAG, "DownloadController::OnDownloadCompleted, mObj is NULL");
        return;
    }
    AutoPtr<IContext> c = IContext::Probe(context);
    mObj->OnDownloadCompleted(c, url, mimeType, filename, path, contentLength, successful, downloadId);
}

void DownloadController::OnDownloadUpdated(
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
    /* [in] */ Int64 timeRemainingInMs)
{
    AutoPtr<DownloadController> mObj = (DownloadController*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(LOGTAG, "DownloadController::OnDownloadUpdated, mObj is NULL");
        return;
    }
    AutoPtr<IContext> c = IContext::Probe(context);
    mObj->OnDownloadUpdated(c, url, mimeType, filename, path, contentLength, successful, downloadId, percentCompleted, timeRemainingInMs);
}

void DownloadController::OnDangerousDownload(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* view,
    /* [in] */ const String& filename,
    /* [in] */ Int32 downloadId)
{
    AutoPtr<DownloadController> mObj = (DownloadController*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(LOGTAG, "DownloadController::OnDangerousDownload, mObj is NULL");
        return;
    }
    AutoPtr<ContentViewCore> cvc = (ContentViewCore*)(IObject::Probe(view));
    mObj->OnDangerousDownload(cvc, filename, downloadId);
}


} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
