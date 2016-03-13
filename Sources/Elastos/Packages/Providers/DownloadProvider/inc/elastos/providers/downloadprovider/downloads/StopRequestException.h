
#ifndef __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_STOPREQUESTEXCEPTION_H__
#define __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_STOPREQUESTEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

/**
 * Raised to indicate that the current request should be stopped immediately.
 *
 * Note the message passed to this exception will be logged and therefore must be guaranteed
 * not to contain any PII, meaning it generally can't include any information about the request
 * URI, headers, or destination filename.
 */
class StopRequestException
    : public Exception
{
public:
    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ String message);

    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ IThrowable* t);

    StopRequestException(
        /* [in] */ Int32 finalStatus,
        /* [in] */ String message,
        /* [in] */ IThrowable* t);

    CARAPI GetFinalStatus(
        /* [out] */ Int32* result);

    static CARAPI_(AutoPtr<IStopRequestException>) ThrowUnhandledHttpError(
        /* [in] */ Int32 code,
        /* [in] */ String message);

private:
    Int32 mFinalStatus;
};

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos

#endif //__ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_STOPREQUESTEXCEPTION_H__
