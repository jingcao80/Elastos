
#ifndef __ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__
#define __ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__

#include "_Elastosx_Net_Ssl_CDefaultHostnameVerifier.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Security::Cert::IX509Certificate;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * A HostnameVerifier consistent with <a
 * href="http://www.ietf.org/rfc/rfc2818.txt">RFC 2818</a>.
 *
 * @hide accessible via HttpsURLConnection.getDefaultHostnameVerifier()
 */
CarClass(CDefaultHostnameVerifier)
    , public Object
    , public IDefaultHostnameVerifier
    , public IHostnameVerifier
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ISSLSession* session,
        /* [out] */ Boolean* result);

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    /**
     * Returns true if {@code hostName} matches the name or pattern {@code cn}.
     *
     * @param hostName lowercase host name.
     * @param cn certificate host name. May include wildcards like
     *     {@code *.android.com}.
     */
    CARAPI VerifyHostName(
        /* [in] */ const String& hostName,
        /* [in] */ const String& cn,
        /* [out] */ Boolean* result);

private:
    /**
     * Returns true if {@code certificate} matches {@code ipAddress}.
     */
    CARAPI VerifyIpAddress(
        /* [in] */ const String& ipAddress,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    /**
     * Returns true if {@code certificate} matches {@code hostName}.
     */
    CARAPI VerifyHostName(
        /* [in] */ const String& hostName,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    CARAPI GetSubjectAltNames(
        /* [in] */ IX509Certificate* certificate,
        /* [in] */ Int32 type,
        /* [out] */ IList** names);

private:
    static const Int32 ALT_DNS_NAME;// = 2;
    static const Int32 ALT_IPA_NAME;// = 7;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__
