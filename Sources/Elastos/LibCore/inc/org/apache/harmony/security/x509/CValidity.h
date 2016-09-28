
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__

#include "_Org_Apache_Harmony_Security_X509_CValidity.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CValidity)
    , public Object
    , public IValidity
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNotBefore(
        /* [out] */ IDate** ppDate);

    CARAPI GetNotAfter(
        /* [out] */ IDate** ppDate);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IDate* pNotBefore,
        /* [in] */ IDate* pNotAfter);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
