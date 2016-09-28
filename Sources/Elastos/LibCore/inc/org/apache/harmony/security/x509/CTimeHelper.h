
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTIMEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTIMEHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CTimeHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Choice;
using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTimeHelper)
    , public Singleton
    , public ITimeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Choice** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTIMEHELPER_H__
