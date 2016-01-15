#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CDownloadsImpl.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Net::Uri;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CDownloadsImpl)

CAR_INTERFACE_IMPL(CDownloadsImpl, Object, IDownloadsImpl)

ECode CDownloadsImpl::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://downloads/my_downloads"), uri);
}

ECode CDownloadsImpl::GetALL_DOWNLOADS_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://downloads/all_downloads"), uri);
}

ECode CDownloadsImpl::GetPUBLICLY_ACCESSIBLE_DOWNLOADS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://downloads/") + IDownloadsImpl::PUBLICLY_ACCESSIBLE_DOWNLOADS_URI_SEGMENT, uri);
}

ECode CDownloadsImpl::IsStatusInformational(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 100 && status < 200);
    return NOERROR;
}

ECode CDownloadsImpl::IsStatusSuccess(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 200 && status < 300);
    return NOERROR;
}

ECode CDownloadsImpl::IsStatusError(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 400 && status < 600);
    return NOERROR;
}

ECode CDownloadsImpl::IsStatusClientError(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 400 && status < 500);
    return NOERROR;
}

ECode CDownloadsImpl::IsStatusServerError(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 500 && status < 600);
    return NOERROR;
}

ECode CDownloadsImpl::IsNotificationToBeDisplayed(
    /* [in] */ Int32 visibility,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = visibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_COMPLETED ||
            visibility == IDownloadManagerRequest::VISIBILITY_VISIBLE_NOTIFY_ONLY_COMPLETION;
    return NOERROR;
}

ECode CDownloadsImpl::IsStatusCompleted(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (status >= 200 && status < 300) || (status >= 400 && status < 600);
    return NOERROR;
}

ECode CDownloadsImpl::StatusToString(
    /* [in] */ Int32 status,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    switch (status) {
        case STATUS_PENDING:
            *value = String("PENDING");
            break;
        case STATUS_RUNNING:
            *value = String("RUNNING");
            break;
        case STATUS_PAUSED_BY_APP:
            *value = String("PAUSED_BY_APP");
            break;
        case STATUS_WAITING_TO_RETRY:
            *value = String("WAITING_TO_RETRY");
            break;
        case STATUS_WAITING_FOR_NETWORK:
            *value = String("WAITING_FOR_NETWORK");
            break;
        case STATUS_QUEUED_FOR_WIFI:
            *value = String("QUEUED_FOR_WIFI");
            break;
        case STATUS_INSUFFICIENT_SPACE_ERROR:
            *value = String("INSUFFICIENT_SPACE_ERROR");
            break;
        case STATUS_DEVICE_NOT_FOUND_ERROR:
            *value = String("DEVICE_NOT_FOUND_ERROR");
            break;
        case STATUS_SUCCESS:
            *value = String("SUCCESS");
            break;
        case STATUS_BAD_REQUEST:
            *value = String("BAD_REQUEST");
            break;
        case STATUS_NOT_ACCEPTABLE:
            *value = String("NOT_ACCEPTABLE");
            break;
        case STATUS_LENGTH_REQUIRED:
            *value = String("LENGTH_REQUIRED");
            break;
        case STATUS_PRECONDITION_FAILED:
            *value = String("PRECONDITION_FAILED");
            break;
        case STATUS_FILE_ALREADY_EXISTS_ERROR:
            *value = String("FILE_ALREADY_EXISTS_ERROR");
            break;
        case STATUS_CANNOT_RESUME:
            *value = String("CANNOT_RESUME");
            break;
        case STATUS_CANCELED:
            *value = String("CANCELED");
            break;
        case STATUS_UNKNOWN_ERROR:
            *value = String("UNKNOWN_ERROR");
            break;
        case STATUS_FILE_ERROR:
            *value = String("FILE_ERROR");
            break;
        case STATUS_UNHANDLED_REDIRECT:
            *value = String("UNHANDLED_REDIRECT");
            break;
        case STATUS_UNHANDLED_HTTP_CODE:
            *value = String("UNHANDLED_HTTP_CODE");
            break;
        case STATUS_HTTP_DATA_ERROR:
            *value = String("HTTP_DATA_ERROR");
            break;
        case STATUS_HTTP_EXCEPTION:
            *value = String("HTTP_EXCEPTION");
            break;
        case STATUS_TOO_MANY_REDIRECTS:
            *value = String("TOO_MANY_REDIRECTS");
            break;
        case STATUS_BLOCKED:
            *value = String("BLOCKED");
            break;
        default:
            *value = StringUtils::ToString(status);
            break;
    }
    return NOERROR;
}

}
}
}
