
#include "elastos/providers/downloadprovider/downloads/StopRequestException.h"

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ String message)
{
    StopRequestException(message);
    mFinalStatus = finalStatus;
}

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ IThrowable* t)
{
    String msg;
    t->GetMessage(&msg);
    StopRequestException(finalStatus, msg);
    InitCause(t);
}

StopRequestException::StopRequestException(
    /* [in] */ Int32 finalStatus,
    /* [in] */ String message,
    /* [in] */ IThrowable* t)
{
    StopRequestException(finalStatus, message);
    InitCause(t);
}

ECode StopRequestException::GetFinalStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFinalStatus;
    return NOERROR;
}

AutoPtr<IStopRequestException> StopRequestException::ThrowUnhandledHttpError(
    /* [in] */ Int32 code,
    /* [in] */ String message)
{
    String error("Unhandled HTTP response: ");
    error += code;
    error += " ";
    error += message;
    if (code >= 400 && code < 600) {
        // throw new StopRequestException(code, error);
        return NULL;
    }
    else if (code >= 300 && code < 400) {
        // throw new StopRequestException(STATUS_UNHANDLED_REDIRECT, error);
        return NULL;
    }
    else {
        // throw new StopRequestException(STATUS_UNHANDLED_HTTP_CODE, error);
        return NULL;
    }
}

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos
