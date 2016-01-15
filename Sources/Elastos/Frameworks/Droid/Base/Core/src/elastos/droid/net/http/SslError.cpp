
#include "elastos/droid/net/http/SslError.h"
#include "elastos/droid/R.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslError.h"
#include "elastos/droid/net/http/SslCertificate.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::R;

using Elastos::Core::StringUtils;
using Elastos::Security::Cert::IX509Certificate;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(SslError, Object, ISslError)

SslError::SslError()
    : mErrors(0)
{}

ECode SslError::constructor(
    /* [in] */ Int32 error,
    /* [in] */ ISslCertificate* certificate)
{
    return constructor(error, certificate, String(""));
}

ECode SslError::constructor(
    /* [in] */ Int32 error,
    /* [in] */ IX509Certificate* certificate)
{
    AutoPtr<ISslCertificate> cert;
    cert = ISslCertificate::Probe(certificate);
    return constructor(error, cert, String(""));
}

ECode SslError::constructor(
    /* [in] */ Int32 error,
    /* [in] */ ISslCertificate* certificate,
    /* [in] */ const String& url)
{
    if (url.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean result;
    AddError(error, &result);
    mCertificate = certificate;
    mUrl = url;
    return NOERROR;
}

ECode SslError::constructor(
    /* [in] */ Int32 error,
    /* [in] */ IX509Certificate* certificate,
    /* [in] */ const String& url)
{
    AutoPtr<ISslCertificate> cert;
    CSslCertificate::New(certificate, (ISslCertificate**)&cert);
    return constructor(error, cert, url);
}

ECode SslError::SslErrorFromChromiumErrorCode(
    /* [in] */ Int32 error,
    /* [in] */ ISslCertificate* cert,
    /* [in] */ const String& url,
    /* [out] */ ISslError** result)
{
    VALIDATE_NOT_NULL(result)

    // The chromium error codes are in:
    // external/chromium/net/base/net_error_list.h
    assert (error >= -299 && error <= -200);

    if (error == -200) {
        return CSslError::New(CSslError::SSL_IDMISMATCH, cert, url, result);
    }
    if (error == -201) {
        return CSslError::New(CSslError::SSL_DATE_INVALID, cert, url, result);
    }
    if (error == -202) {
        return CSslError::New(CSslError::SSL_UNTRUSTED, cert, url, result);
    }
    // Map all other codes to SSL_INVALID.
    return CSslError::New(CSslError::SSL_INVALID, cert, url, result);
}

ECode SslError::GetCertificate(
    /* [out] */ ISslCertificate** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCertificate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SslError::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = mUrl;
    return NOERROR;
}

ECode SslError::AddError(
    /* [in] */ Int32 error,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean rval = (0 <= error && error < SSL_MAX_ERROR);
    if (rval) {
        mErrors |= (0x1 << error);
    }

    *result = rval;
    return NOERROR;
}

ECode SslError::HasError(
    /* [in] */ Int32 error,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean rval = (0 <= error && error < SSL_MAX_ERROR);
    if (rval) {
        rval = ((mErrors & (0x1 << error)) != 0);
    }

    *result = rval;
    return NOERROR;
}

ECode SslError::GetPrimaryError(
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error)

    if (mErrors != 0) {
        // go from the most to the least severe errors
        for (Int32 iError = SSL_MAX_ERROR - 1; iError >= 0; --iError) {
            if ((mErrors & (0x1 << iError)) != 0) {
                *error = iError;
                return NOERROR;
            }
        }
        // mErrors should never be set to an invalid value.
        assert(FALSE);
    }

    *error = -1;
    return NOERROR;
}

ECode SslError::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 err;
    GetPrimaryError(&err);
    String sCert;
    IObject::Probe(mCertificate)->ToString(&sCert);

    *str = String("primary error: ") + StringUtils::ToString(err)
        + String(" certificate: ") + sCert
        + String(" on URL: ") + mUrl;

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
