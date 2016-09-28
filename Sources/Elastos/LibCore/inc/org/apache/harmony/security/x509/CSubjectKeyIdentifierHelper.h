
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIERHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CSubjectKeyIdentifierHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectKeyIdentifierHelper)
    , public Singleton
    , public ISubjectKeyIdentifierHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ ISubjectKeyIdentifier** ppIdentifier);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIERHELPER_H__
