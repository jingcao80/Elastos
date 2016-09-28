
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTSTInfo.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X509::IExtensions;
using Org::Apache::Harmony::Security::X509::IGeneralName;
using Elastos::Core::Object;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CTSTInfo)
    , public Object
    , public ITSTInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetAccuracy(
        /* [out, callee] */ ArrayOf<Int32>** ppAccuracy);

    CARAPI GetExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetGenTime(
        /* [out] */ IDate** ppDate);

    CARAPI GetMessageImprint(
        /* [out] */ IMessageImprint** ppMessageImprint);

    CARAPI GetNonce(
        /* [out] */ IBigInteger** ppNonce);

    CARAPI GetOrdering(
        /* [out] */ Boolean* pOrdering);

    CARAPI GetPolicy(
        /* [out] */ String* pPolicy);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** ppSerialNumber);

    CARAPI GetTsa(
        /* [out] */ IGeneralName** ppTsa);

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ const String& policy,
        /* [in] */ IMessageImprint* pMessageImprint,
        /* [in] */ IBigInteger* pSerialNumber,
        /* [in] */ IDate* pGenTime,
        /* [in] */ ArrayOf<Int32>* pAccuracy,
        /* [in] */ Boolean ordering,
        /* [in] */ IBigInteger* pNonce,
        /* [in] */ IGeneralName* pTsa,
        /* [in] */ IExtensions* pExtensions);

private:
    // TODO: Add your private member variables here.
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__
