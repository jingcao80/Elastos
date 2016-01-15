
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/http/ErrorStrings.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/http/ErrorStrings.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::R;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

const String ErrorStrings::LOGTAG("Http");

ErrorStrings::ErrorStrings()
{
    /* Utility class, don't instantiate. */
}

ECode ErrorStrings::GetString(
    /* [in] */ Int32 errorCode,
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    Int32 res;
    GetResource(errorCode, &res);
    AutoPtr<Elastos::Core::ICharSequence> cs;
    ECode ec = context->GetText(res, (Elastos::Core::ICharSequence**)&cs);
    if (SUCCEEDED(ec)) {
        cs->ToString(result);
    }

    return ec;
}

ECode ErrorStrings::GetResource(
    /* [in] */ Int32 errorCode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    switch(errorCode) {
        case IEventHandler::OK: {
            *result = R::string::httpErrorOk;
            break;
        }
        case IEventHandler::ERROR: {
            *result = R::string::httpError;
            break;
        }
        case IEventHandler::ERROR_LOOKUP: {
            *result = R::string::httpErrorLookup;
            break;
        }
        case IEventHandler::ERROR_UNSUPPORTED_AUTH_SCHEME: {
            *result = R::string::httpErrorUnsupportedAuthScheme;
            break;
        }
        case IEventHandler::ERROR_AUTH: {
            *result = R::string::httpErrorAuth;
            break;
        }
        case IEventHandler::ERROR_PROXYAUTH: {
            *result = R::string::httpErrorProxyAuth;
            break;
        }
        case IEventHandler::ERROR_CONNECT: {
            *result = R::string::httpErrorConnect;
            break;
        }
        case IEventHandler::ERROR_IO: {
            *result = R::string::httpErrorIO;
            break;
        }
        case IEventHandler::ERROR_TIMEOUT: {
            *result = R::string::httpErrorTimeout;
            break;
        }
        case IEventHandler::ERROR_REDIRECT_LOOP: {
            *result = R::string::httpErrorRedirectLoop;
            break;
        }
        case IEventHandler::ERROR_UNSUPPORTED_SCHEME: {
            *result = R::string::httpErrorUnsupportedScheme;
            break;
        }
        case IEventHandler::ERROR_FAILED_SSL_HANDSHAKE: {
            *result = R::string::httpErrorFailedSslHandshake;
            break;
        }
        case IEventHandler::ERROR_BAD_URL: {
            *result = R::string::httpErrorBadUrl;
            break;
        }
        case IEventHandler::FILE_ERROR: {
            *result = R::string::httpErrorFile;
            break;
        }
        case IEventHandler::FILE_NOT_FOUND_ERROR: {
            *result = R::string::httpErrorFileNotFound;
            break;
        }
        case IEventHandler::TOO_MANY_REQUESTS_ERROR: {
            *result = R::string::httpErrorTooManyRequests;
            break;
        }
        default: {
            // Log.w(LOGTAG, "Using generic message for unknown error code: " + errorCode);
            *result = R::string::httpError;
            break;
        }
    }

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
