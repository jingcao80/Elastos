
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__

#include "_Org_Apache_Harmony_Security_X509_COtherName.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(COtherName)
    , public Object
    , public IOtherName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte>** ppValue);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ const String& typeID,
        /* [in] */ ArrayOf<Byte>* pValue);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_COTHERNAME_H__
