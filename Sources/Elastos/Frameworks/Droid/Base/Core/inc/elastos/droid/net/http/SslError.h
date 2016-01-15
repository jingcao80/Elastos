
#ifndef __ELASTOS_DROID_NET_HTTP_SSLERROR_H__
#define __ELASTOS_DROID_NET_HTTP_SSLERROR_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Security::Cert::IX509Certificate;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * This class represents a set of one or more SSL errors and the associated SSL
 * certificate.
 */
class SslError
    : public Object
    , public ISslError
{
public:
    CAR_INTERFACE_DECL()

    SslError();

    /**
     * Creates a new SslError object using the supplied error and certificate.
     * The URL will be set to the empty string.
     * @param error The SSL error
     * @param certificate The associated SSL certificate
     * @deprecated Use {@link #SslError(int, SslCertificate, String)}
     */
    // @Deprecated
    CARAPI constructor(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* certificate);

    /**
     * Creates a new SslError object using the supplied error and certificate.
     * The URL will be set to the empty string.
     * @param error The SSL error
     * @param certificate The associated SSL certificate
     * @deprecated Use {@link #SslError(int, X509Certificate, String)}
     */
    // @Deprecated
    CARAPI constructor(
        /* [in] */ Int32 error,
        /* [in] */ IX509Certificate* certificate);

    /**
     * Creates a new SslError object using the supplied error, certificate and
     * URL.
     * @param error The SSL error
     * @param certificate The associated SSL certificate
     * @param url The associated URL
     */
    CARAPI constructor(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* certificate,
        /* [in] */ const String& url);

    /**
     * Creates a new SslError object using the supplied error, certificate and
     * URL.
     * @param error The SSL error
     * @param certificate The associated SSL certificate
     * @param url The associated URL
     */
    CARAPI constructor(
        /* [in] */ Int32 error,
        /* [in] */ IX509Certificate* certificate,
        /* [in] */ const String& url);

    /**
     * Creates an SslError object from a chromium error code.
     * @param error The chromium error code
     * @param certificate The associated SSL certificate
     * @param url The associated URL.
     * @hide  chromium error codes only available inside the framework
     */
    static CARAPI SslErrorFromChromiumErrorCode(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* cert,
        /* [in] */ const String& url,
        /* [out] */ ISslError** result);

    /**
     * Gets the SSL certificate associated with this object.
     * @return The SSL certificate, non-null.
     */
    CARAPI GetCertificate(
        /* [out] */ ISslCertificate** result);

    /**
     * Gets the URL associated with this object.
     * @return The URL, non-null.
     */
    CARAPI GetUrl(
        /* [out] */ String* result);

    /**
     * Adds the supplied SSL error to the set.
     * @param error The SSL error to add
     * @return True if the error being added is a known SSL error, otherwise
     *         false.
     */
    CARAPI AddError(
        /* [in] */ Int32 error,
        /* [out] */ Boolean* result);

    /**
     * Determines whether this object includes the supplied error.
     * @param error The SSL error to check for
     * @return True if this object includes the error, otherwise false.
     */
    CARAPI HasError(
        /* [in] */ Int32 error,
        /* [out] */ Boolean* result);

    /**
     * Gets the most severe SSL error in this object's set of errors.
     * Returns -1 if the set is empty.
     * @return The most severe SSL error, or -1 if the set is empty.
     */
    CARAPI GetPrimaryError(
        /* [out] */ Int32* result);

    /**
     * Returns a string representation of this object.
     * @return A String representation of this object.
     */
    CARAPI ToString(
        /* [out] */ String* result);

public:
    /**
     * The SSL error set bitfield (each individual error is a bit index;
     * multiple individual errors can be OR-ed)
     */
    Int32 mErrors;

    /**
     * The SSL certificate associated with the error set
     */
    /* const */ AutoPtr<ISslCertificate> mCertificate;

    /**
     * The URL associated with the error set.
     */
    /* const */ String mUrl;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_SSLERROR_H__
